// Copyright (c) 2023 Sapphire's Suite. All Rights Reserved.

#pragma once

#ifndef SAPPHIRE_ALLOCATOR_IALLOCATOR_GUARD
#define SAPPHIRE_ALLOCATOR_IALLOCATOR_GUARD

#include <cstdint>

#include <SA/Collections/Debug>

namespace SA
{
	class IAllocator
	{
	public:
		virtual void Create(uint64_t _size, uint32_t _blockNum = 10) = 0;
		virtual void Destroy() = 0;

		virtual void* Allocate(uint64_t _size) = 0;
		virtual void Free(void* _ptr) = 0;
	};
}

#endif // SAPPHIRE_ALLOCATOR_IALLOCATOR_GUARD
