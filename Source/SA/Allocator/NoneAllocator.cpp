// Copyright (c) 2023 Sapphire's Suite. All Rights Reserved.

#include <NoneAllocator.hpp>

namespace SA
{
	static NoneAllocator gInstance = NoneAllocator();
	NoneAllocator& NoneAllocator::Instance = gInstance;


	void NoneAllocator::Create(uint64_t _size, uint32_t _blockNum)
	{
		(void)_size;
		(void)_blockNum;
	}
	
	void NoneAllocator::Destroy()
	{
	}


	void* NoneAllocator::Allocate(uint64_t _size)
	{
		return ::operator new(_size);
	}

	void NoneAllocator::Free(void* _ptr)
	{
		::operator delete(_ptr);
	}
}
