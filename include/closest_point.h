// Copyright 2009-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

// Modfied as needed to use be self contained, switching to using glm. The original source code is available at https://github.com/RenderKit/embree/blob/master/tutorials/common/math/closest_point.h

#pragma once

namespace embree
{
	inline glm::vec3 closestPointTriangle(glm::vec3 const& p, glm::vec3 const& a, glm::vec3 const& b, glm::vec3 const& c)
	{
		const glm::vec3 ab = b - a;
		const glm::vec3 ac = c - a;
		const glm::vec3 ap = p - a;

		const float d1 = dot(ab, ap);
		const float d2 = dot(ac, ap);
		if (d1 <= 0.f && d2 <= 0.f)
			return a;

		const glm::vec3 bp = p - b;
		const float d3 = dot(ab, bp);
		const float d4 = dot(ac, bp);
		if (d3 >= 0.f && d4 <= d3)
			return b;

		const glm::vec3 cp = p - c;
		const float d5 = dot(ab, cp);
		const float d6 = dot(ac, cp);
		if (d6 >= 0.f && d5 <= d6)
			return c;

		const float vc = d1 * d4 - d3 * d2;
		if (vc <= 0.f && d1 >= 0.f && d3 <= 0.f) {
			const float v = d1 / (d1 - d3);
			return a + v * ab;
		}

		const float vb = d5 * d2 - d1 * d6;
		if (vb <= 0.f && d2 >= 0.f && d6 <= 0.f) {
			const float v = d2 / (d2 - d6);
			return a + v * ac;
		}

		const float va = d3 * d6 - d5 * d4;
		if (va <= 0.f && (d4 - d3) >= 0.f && (d5 - d6) >= 0.f) {
			const float v = (d4 - d3) / ((d4 - d3) + (d5 - d6));
			return b + v * (c - b);
		}

		const float denom = 1.f / (va + vb + vc);
		const float v = vb * denom;
		const float w = vc * denom;
		return a + v * ab + w * ac;
	}
}
