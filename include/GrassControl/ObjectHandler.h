#pragma once

#include "Config.h"
#include "Util.h"
#include <CLIBUtil/Distribution.hpp>

namespace GrassControl
{
	struct CliffObject
	{
		const RE::FormID formId;
		const bool isSteep;
		const std::unordered_set<std::string> allowedShapes;
		const std::unordered_set<std::string> blockedShapes;

		bool IsAllowed(std::string name) const
		{
			return allowedShapes.contains(name);
		}

		bool IsBlocked(std::string name) const
		{
			return blockedShapes.contains(name);
		}
	};

	struct PartIgnoredObject
	{
		const RE::FormID formId;
		const std::unordered_set<std::string> ignoredShapes;

		bool IsIgnored(std::string name) const
		{
			return ignoredShapes.contains(name);
		}
	};

	namespace ObjectHandler
	{
		void AddCliffObjects(CSimpleIniA::TNamesDepend& values, std::unordered_map<RE::FormID, CliffObject>& cliffObjects);

		void AddPartIgnoredObjects(CSimpleIniA::TNamesDepend& values, std::unordered_map<RE::FormID, PartIgnoredObject>& partIgnoredObjects);

		void LoadObjectConfigs(std::unordered_map<RE::FormID, CliffObject>& cliffObjects, std::unordered_map<RE::FormID, PartIgnoredObject>& partIgnoredObjects);
	}
}
