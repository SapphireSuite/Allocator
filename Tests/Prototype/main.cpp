// Copyright (c) 2023 Sapphire's Suite. All Rights Reserved.

#include <SA/Collections/Allocator>
using namespace SA;

int main()
{
	SA::Debug::InitDefaultLogger();

	CPUAllocator allocator;

	allocator.Create(1000);


	SA_LOG((L"Heap: %1", allocator.DebugHeapStr()));
	SA_LOG((L"FreeList: %1", allocator.DebugFreeListStr()), Warning);

	void* p1 = allocator.Allocate(25);

	SA_LOG((L"Heap: %1", allocator.DebugHeapStr()));
	SA_LOG((L"FreeList: %1", allocator.DebugFreeListStr()), Warning);

	void* p2 = allocator.Allocate(27);

	SA_LOG((L"Heap: %1", allocator.DebugHeapStr()));
	SA_LOG((L"FreeList: %1", allocator.DebugFreeListStr()), Warning);

	void* p3 = allocator.Allocate(104);

	SA_LOG((L"Heap: %1", allocator.DebugHeapStr()));
	SA_LOG((L"FreeList: %1", allocator.DebugFreeListStr()), Warning);

	void* p4 = allocator.Allocate(200);

	SA_LOG((L"Heap: %1", allocator.DebugHeapStr()));
	SA_LOG((L"FreeList: %1", allocator.DebugFreeListStr()), Warning);

	allocator.Free(p2);

	SA_LOG((L"Heap: %1", allocator.DebugHeapStr()));
	SA_LOG((L"FreeList: %1", allocator.DebugFreeListStr()), Warning);

	allocator.Free(p3);

	SA_LOG((L"Heap: %1", allocator.DebugHeapStr()));
	SA_LOG((L"FreeList: %1", allocator.DebugFreeListStr()), Warning);

	allocator.Free(p4);

	SA_LOG((L"Heap: %1", allocator.DebugHeapStr()));
	SA_LOG((L"FreeList: %1", allocator.DebugFreeListStr()), Warning);

	allocator.Free(p1);

	SA_LOG((L"Heap: %1", allocator.DebugHeapStr()));
	SA_LOG((L"FreeList: %1", allocator.DebugFreeListStr()), Warning);
	

	allocator.Destroy();

	return 0;
}
