// Copyright (c) 2023 Sapphire's Suite. All Rights Reserved.

#include <NullAllocator.hpp>

namespace SA
{
	static NullAllocator gInstance = NullAllocator();
	NullAllocator& NullAllocator::Instance = gInstance;


	void NullAllocator::Create(uint64_t _size, uint32_t _blockNum)
	{
		(void)_size;
		(void)_blockNum;
	}
	
	void NullAllocator::Destroy()
	{
	}


	void* NullAllocator::Allocate(uint64_t _size)
	{
		return ::operator new(_size);
	}

	void NullAllocator::Free(void* _ptr)
	{
		::operator delete(_ptr);
	}
}
