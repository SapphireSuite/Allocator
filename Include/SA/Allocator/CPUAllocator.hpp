// Copyright (c) 2023 Sapphire's Suite. All Rights Reserved.

#pragma once

#ifndef SAPPHIRE_ALLOCATOR_CPU_ALLOCATOR_GUARD
#define SAPPHIRE_ALLOCATOR_CPU_ALLOCATOR_GUARD

#include "IAllocator.hpp"

namespace SA
{
	class CPUAllocator : public IAllocator
	{
		/**
		 * @brief Meta data implementation
		 * Manage a block of memory
		 */
		class Meta
		{
			/// Handled block size.
			uint64_t mBlockSize = 0;

		public:
			/// Get previous continuous memory meta.
			Meta* prev = nullptr;

			Meta(uint64_t _blockSize, bool _bIsFree);

			bool IsFreeBlock() const;
			void SetIsFree(bool _bIsFree);

			void* GetBlockStartPtr() const;
			void* GetBlockEndPtr() const;

			/// Get next continuous memory meta.
			Meta* GetNext() const;

			uint64_t GetBlockSize() const;
			void SetBlockSize(uint64_t _blockSize, bool _bIsFree);
		};


		/**
		 * @brief Free Meta data implementation
		 * Use free block space for sized-sorted linked list.
		 */
		class FreeMeta : public Meta
		{
		public:
			FreeMeta* prevFree = nullptr;
			FreeMeta* nextFree = nullptr;
		
			FreeMeta(uint64_t _blockSize);
		};


		void* mMemory = nullptr;

		FreeMeta* mFreeHead = nullptr;

	#if SA_DEBUG || SA_LOG_RELEASE_OPT

		uint64_t mTotalSize = 0;

	#endif

		// Checks whether this meta has a valid (in bound) next meta.
		bool HasValidNext(Meta* _meta) const;

		/// Emplace (create in-place) a Meta.
		Meta* EmplaceMeta(void* _ptr, uint64_t _size);
		
		/// Emplace (create in-place) a FreeMeta.
		FreeMeta* EmplaceFreeMeta(void* _ptr, uint64_t _size);


		/// Insert a FreeMeta to the free meta size-sorted list.
		void InsertFreeMeta(FreeMeta* _meta);

		// /// Remove a FreeMeta from the free meta size-sorted list.
		void RemoveFreeMeta(FreeMeta* _meta);


		FreeMeta* FindSuitableBlock(uint64_t _size);

		bool SplitBlock(FreeMeta* _meta, uint64_t _allocSize);

		FreeMeta* FusionBlock(Meta* _meta);

	public:
		void Create(uint64_t _size, uint32_t _blockNum = 10) override final;
		void Destroy() override final;

		void* Allocate(uint64_t _size) override final;
		void Free(void* _ptr) override final;

	#if SA_DEBUG || SA_LOG_RELEASE_OPT
		std::string DebugHeapStr();
		std::string DebugFreeListStr();
	#endif
	};
}

#endif // SAPPHIRE_ALLOCATOR_CPU_ALLOCATOR_GUARD
