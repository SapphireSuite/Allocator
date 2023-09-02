// Copyright (c) 2023 Sapphire's Suite. All Rights Reserved.

#pragma once

#ifndef SAPPHIRE_NULL_ALLOCATOR_IALLOCATOR_GUARD
#define SAPPHIRE_NULL_ALLOCATOR_IALLOCATOR_GUARD

#include "IAllocator.hpp"

namespace SA
{
	class NullAllocator : public IAllocator
	{
	public:
		static NullAllocator& Instance;

		void Create(uint64_t _size, uint32_t _blockNum = 10) override final;
		void Destroy() override final;

		void* Allocate(uint64_t _size) override final;
		void Free(void* _ptr) override final;
	};
}

#endif // SAPPHIRE_NULL_ALLOCATOR_IALLOCATOR_GUARD
