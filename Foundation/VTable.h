#pragma once

#include "Foundation.h"

namespace Foundation::VTable {
	inline void**& get(void* inst, size_t offset = 0)
	{
		return *reinterpret_cast<void***>((size_t)inst + offset);
	}

	inline const void** get(const void* inst, size_t offset = 0)
	{
		return *reinterpret_cast<const void***>((size_t)inst + offset);
	}

	template< typename Fn >
	inline Fn get_vfunc(const void* inst, size_t index, size_t offset = 0)
	{
		return reinterpret_cast<Fn>(get(inst, offset)[index]);
	}

	template <typename Fn, typename T>
	static inline T* find(void* base, size_t sz) {
		if (!base) return nullptr;
		if (!sz) return nullptr;

		void** pointee;
		int at = -1;
		int off = 0;

		while (true) {
			at += 1;
			pointee = reinterpret_cast<void**>(get(base, off));
			if (get_vfunc<Fn>(pointee[at]))
				break;

			off += 1;
		}

		if (at == -1) throw std::runtime_error("Bad offset.");
		return (T*)pointee[at];
	}
}