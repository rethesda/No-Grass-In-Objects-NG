#include "GrassControl/ObjectHandler.h"

namespace GrassControl
{
	void ObjectHandler::AddCliffObjects(CSimpleIniA::TNamesDepend& values, std::unordered_map<RE::FormID, CliffObject>& cliffObjects)
	{
		for (const auto& key : values) {
			std::string keyStr = key.pItem;
			auto splitKey = clib_util::string::split(keyStr, "|");

			auto foundID = Util::CachedFormList::TryParse(splitKey[0], "CliffObjects", true, false)->GetAllFormIDs();
			if (foundID.empty()) {
				logger::warn("Invalid formID {} found, skipping", splitKey[0]);
				break;
			}

			auto formID = *foundID.begin();
			splitKey.erase(splitKey.begin());

			std::unordered_set<std::string> allowedShapes;
			std::unordered_set<std::string> blockedShapes;

			bool steep = false;

			for (auto& str : splitKey) {
				if (str.contains("~")) {
					str.erase(std::remove(str.begin(), str.end(), '~'), str.end());
					blockedShapes.insert(str);
				} else if (str.contains("Steep") || str.contains("steep")) {
					steep = true;
				} else {
					allowedShapes.insert(str);
				}
			}

			cliffObjects.emplace(formID, CliffObject{ formID, steep, allowedShapes, blockedShapes });
		}
	}

	void ObjectHandler::AddPartIgnoredObjects(CSimpleIniA::TNamesDepend& values, std::unordered_map<RE::FormID, PartIgnoredObject>& partIgnoredObjects)
	{
		for (const auto& key : values) {
			std::string keyStr = key.pItem;
			auto splitKey = clib_util::string::split(keyStr, "|");

			auto foundID = Util::CachedFormList::TryParse(splitKey[0], "IgnoredShapes", true, false)->GetAllFormIDs();
			if (foundID.empty()) {
				logger::warn("Invalid formID {} found, skipping", splitKey[0]);
				break;
			}

			auto formID = *foundID.begin();
			splitKey.erase(splitKey.begin());

			std::unordered_set<std::string> blockedShapes;

			for (auto& str : splitKey) {
				blockedShapes.insert(str);
			}

			partIgnoredObjects.emplace(formID, PartIgnoredObject{ formID, blockedShapes });
		}
	}

	void ObjectHandler::LoadObjectConfigs(std::unordered_map<RE::FormID, CliffObject>& cliffObjects, std::unordered_map<RE::FormID, PartIgnoredObject>& partIgnoredObjects)
	{
		std::vector<std::string> configs = clib_util::distribution::get_configs(R"(Data\)", "_NGIO"sv);

		if (configs.empty()) {
			logger::warn("No .ini files with _NGIO suffix were found within the Data folder. Overrides will not be loaded");
			return;
		}

		logger::info("{} matching inis found", configs.size());

		for (const auto& config : configs) {
			logger::info("Loading object config {}...", config);

			CSimpleIniA ini;
			ini.SetUnicode();
			ini.SetMultiKey();
			ini.SetAllowKeyOnly();

			if (const auto rc = ini.LoadFile(config.c_str()); rc < 0) {
				logger::error("error occured while reading INI");
				continue;
			}

			CSimpleIniA::TNamesDepend sections;
			ini.GetAllSections(sections);
			sections.sort(CSimpleIniA::Entry::LoadOrder());

			for (auto& [_section, comment, keyOrder] : sections) {
				std::string section = _section;
				if (section.contains('|')) {
					auto splitSection = clib_util::string::split(section, "|");

					CSimpleIniA::TNamesDepend values;
					ini.GetAllKeys(section.c_str(), values);
					values.sort(CSimpleIniA::Entry::LoadOrder());

					if (values.empty())
						continue;

					if (splitSection[0] == "CliffObjects") {
						logger::info("{} grass objects found", values.size());
						AddCliffObjects(values, cliffObjects);
					} else if (splitSection[0] == "RayCastShapes") {
						logger::info("{} ignored triShapes found", values.size());
						AddPartIgnoredObjects(values, partIgnoredObjects);
					}

				} else {
					CSimpleIniA::TNamesDepend values;
					ini.GetAllKeys(section.c_str(), values);
					values.sort(CSimpleIniA::Entry::LoadOrder());

					if (!values.empty()) {
						if (section == "CliffObjects") {
							logger::info("{} grass objects found", values.size());
							AddCliffObjects(values, cliffObjects);
						} else if (section == "RayCastShapes") {
							logger::info("{} ignored triShapes found", values.size());
							AddPartIgnoredObjects(values, partIgnoredObjects);
						}
					}
				}
			}

			logger::info("... Finished loading object config");
		}

		logger::info("All Object Configs loaded");
	}
}
