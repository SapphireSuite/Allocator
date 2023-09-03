// Copyright (c) 2023 Sapphire's Suite. All Rights Reserved.

#include <CPUAllocator.hpp>
#include <NoneAllocator.hpp>

namespace SA
{
	constexpr uint64_t cMinBlockSize = 16;

	//==================== Meta ====================

	CPUAllocator::Meta::Meta(uint64_t _blockSize, bool _bIsFree)
	{
		SetBlockSize(_blockSize, _bIsFree);
	}


	bool CPUAllocator::Meta::IsFreeBlock() const
	{
		// size & 1000 0000...
		return mBlockSize & 0x8000000000000000;
	}

	void CPUAllocator::Meta::SetIsFree(bool _bIsFree)
	{
		if(_bIsFree)
			mBlockSize |= 0x8000000000000000;
		else
			mBlockSize &= 0x7FFFFFFFFFFFFFFF;
	}


	void* CPUAllocator::Meta::GetBlockStartPtr() const
	{
		return const_cast<void*>(static_cast<const void*>(this + 1)); // Offset 1 Meta.
	}

	void* CPUAllocator::Meta::GetBlockEndPtr() const
	{
		const uint64_t blockStartAddr = reinterpret_cast<uint64_t>(GetBlockStartPtr());
		const uint64_t blockEndAddr = blockStartAddr + GetBlockSize();

		return reinterpret_cast<void*>(blockEndAddr);
	}

	CPUAllocator::Meta* CPUAllocator::Meta::GetNext() const
	{
		return static_cast<Meta*>(GetBlockEndPtr());
	}


	uint64_t CPUAllocator::Meta::GetBlockSize() const
	{
		// size & 0111 1111...
		return mBlockSize & 0x7FFFFFFFFFFFFFFF;
	}

	void CPUAllocator::Meta::SetBlockSize(uint64_t _blockSize, bool _bIsFree)
	{
		mBlockSize = _blockSize;

		if(_bIsFree)
			mBlockSize |= 0x8000000000000000;
	}


	//==================== FreeMeta ====================
	
	CPUAllocator::FreeMeta::FreeMeta(uint64_t _blockSize) : Meta(_blockSize, true)
	{
	}


	//==================== Allocator ====================

	void CPUAllocator::Create(uint64_t _size, uint32_t _blockNum)
	{
		const uint64_t totalSize = _size + _blockNum * sizeof(Meta);

		mMemory = ::operator new(totalSize);

		mFreeHead = EmplaceFreeMeta(mMemory, totalSize - sizeof(Meta));

	#if SA_DEBUG || SA_LOG_RELEASE_OPT
		mTotalSize = totalSize;

		SA_LOG((L"Allocator of size [%1] created.", totalSize), Info, SA.Allocator);
	#endif
	}

	void CPUAllocator::Destroy()
	{
		SA_ASSERT((Nullptr, mMemory), SA.Allocator, L"Try destroy nullptr allocator");

		::operator delete(mMemory);

	#if SA_DEBUG || SA_LOG_RELEASE_OPT
		SA_LOG((L"Allocator of size [%1] destroyed.", mTotalSize), Info, SA.Allocator);

		mTotalSize = 0;
	#endif
	}


	void* CPUAllocator::Allocate(uint64_t _size)
	{
		if(_size < cMinBlockSize)
			_size = cMinBlockSize;

		FreeMeta* meta = FindSuitableBlock(_size);

		if(!meta)
		{
			SA_LOG(L"Allocator out of memory! Use NoneAllocator instead.", Warning, SA.Allocator);

			return NoneAllocator::Instance.Allocate(_size);
		}

		// Detach block from free list.
		RemoveFreeMeta(meta);

		// Try splitting block.
		if(!SplitBlock(meta, _size))
		{
			meta->SetIsFree(false);
		}

		return meta->GetBlockStartPtr();
	}

	void CPUAllocator::Free(void* _ptr)
	{
		if(_ptr <= mMemory || _ptr >= static_cast<char*>(mMemory) + mTotalSize)
		{
			NoneAllocator::Instance.Free(_ptr);
			return;
		}

		const uint64_t blockAddr = reinterpret_cast<uint64_t>(_ptr);
		Meta* meta = reinterpret_cast<Meta*>(blockAddr - sizeof(Meta));

		FreeMeta* newFreeMeta = FusionBlock(meta);

		// No fusion possible
		if(!newFreeMeta)
			newFreeMeta = static_cast<FreeMeta*>(meta);

		newFreeMeta->SetIsFree(true);
		InsertFreeMeta(newFreeMeta);
	}
	

	CPUAllocator::FreeMeta* CPUAllocator::FindSuitableBlock(uint64_t _size)
	{
		// sorted by size: find first suitable block.
		for(auto curr = mFreeHead; curr != nullptr; curr = curr->nextFree)
		{
			if(curr->GetBlockSize() >= _size)
				return curr;
		}

		return nullptr;
	}

	bool CPUAllocator::SplitBlock(FreeMeta* _meta, uint64_t _allocSize)
	{
		static_assert(cMinBlockSize >= sizeof(FreeMeta) - sizeof(Meta));

		const uint64_t blockBaseSize = _meta->GetBlockSize();
		const uint64_t splittedBlockSize = blockBaseSize - _allocSize - sizeof(Meta);

		// Should split block
		if(splittedBlockSize >= sizeof(Meta) + cMinBlockSize)
		{
			_meta->SetBlockSize(_allocSize, false); // Set to future not-free block.

			FreeMeta* splittedFreeMeta = EmplaceFreeMeta(_meta->GetBlockEndPtr(), splittedBlockSize);
			splittedFreeMeta->prev = _meta;

			InsertFreeMeta(splittedFreeMeta);

			return true;
		}

		return false;
	}

	CPUAllocator::FreeMeta* CPUAllocator::FusionBlock(Meta* _meta)
	{
		FreeMeta* mergedMeta = nullptr;

		if(_meta->prev && _meta->prev->IsFreeBlock())
		{
			FreeMeta* prev = static_cast<FreeMeta*>(_meta->prev);

			RemoveFreeMeta(prev);

			if(HasValidNext(_meta))
				_meta->GetNext()->prev = prev;

			prev->SetBlockSize(prev->GetBlockSize() + _meta->GetBlockSize() + sizeof(Meta), true);

			_meta = mergedMeta = prev;
		}


		if(HasValidNext(_meta))
		{
			FreeMeta* nextMeta = static_cast<FreeMeta*>(_meta->GetBlockEndPtr());

			if(nextMeta->IsFreeBlock())
			{
				RemoveFreeMeta(nextMeta);

				const uint64_t newBlockSize = _meta->GetBlockSize() + nextMeta->GetBlockSize() + sizeof(Meta);
				
				_meta->SetBlockSize(newBlockSize, true);

				mergedMeta = static_cast<FreeMeta*>(_meta);
			}
		}

		return mergedMeta;
	}


	bool CPUAllocator::HasValidNext(Meta* _meta) const
	{
		return reinterpret_cast<uint64_t>(_meta->GetBlockEndPtr()) < reinterpret_cast<uint64_t>(mMemory) + mTotalSize;
	}

	CPUAllocator::Meta* CPUAllocator::EmplaceMeta(void* _ptr, uint64_t _size)
	{
		return new(_ptr) Meta(_size, false);
	}

	CPUAllocator::FreeMeta* CPUAllocator::EmplaceFreeMeta(void* _ptr, uint64_t _size)
	{
		return new(_ptr) FreeMeta(_size);
	}

	void CPUAllocator::InsertFreeMeta(FreeMeta* _meta)
	{
		const uint64_t freeSize = _meta->GetBlockSize();
		FreeMeta* savedPrevious = nullptr;

		for(auto curr = mFreeHead; curr != nullptr; curr = curr->nextFree)
		{
			if(curr->GetBlockSize() >= freeSize)
			{
				_meta->prevFree = curr->prevFree;

				if(curr->prevFree)
					curr->prevFree->nextFree = _meta;
				else
					mFreeHead = _meta;

				_meta->nextFree = curr;
				curr->prevFree = _meta;

				return;
			}

			savedPrevious = curr;
		}

		if(!savedPrevious)
		{
			// Begin of list reached
			_meta->nextFree = mFreeHead;
			mFreeHead = _meta;
		}
		else
		{
			// End of list reached
			savedPrevious->nextFree = _meta;
			_meta->prevFree = savedPrevious;
		}
	}

	void CPUAllocator::RemoveFreeMeta(FreeMeta* _meta)
	{
		if(_meta->prevFree)
			_meta->prevFree = _meta->nextFree;
		else
			mFreeHead = _meta->nextFree;

		if(_meta->nextFree)
			_meta->nextFree->prevFree = _meta->prevFree;
	}

#if SA_DEBUG || SA_LOG_RELEASE_OPT
	std::string CPUAllocator::DebugHeapStr()
	{
		std::string str;

		Meta* meta = static_cast<Meta*>(mMemory);

		for(uint64_t currSize = 0u; ;)
		{
			str += '[' + std::to_string(meta->GetBlockSize()) + '|';
			str += meta->IsFreeBlock() ? "     " : "/////";
			str += "]-";

			currSize += meta->GetBlockSize() + sizeof(Meta);

			if(currSize >= mTotalSize)
				break;

			meta = meta->GetNext();
		}

		return str;
	}

	std::string CPUAllocator::DebugFreeListStr()
	{
		std::string str;

		for(auto curr = mFreeHead; curr != nullptr; curr = curr->nextFree)
		{
			str += '[' + std::to_string(curr->GetBlockSize()) + '|';
			str += curr->IsFreeBlock() ? "     " : "/////";
			str += "]-";
		}

		return str;
	}
#endif
}
