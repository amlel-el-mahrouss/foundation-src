#pragma once

#include "Foundation.h"

namespace Foundation {
	class cert {
	public:
		cert() = default;
		virtual ~cert() = default;

		cert& operator=(const cert&) = delete;
		cert(const cert&) = delete;

		virtual char* make() = 0;
		virtual bool check() = 0;

	};
}