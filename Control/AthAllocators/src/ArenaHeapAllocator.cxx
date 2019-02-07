/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaHeapAllocator.cxx 470529 2011-11-24 23:54:22Z ssnyder $
/**
 * @file  AthAllocators/src/ArenaHeapAllocator.cxx
 * @author scott snyder
 * @date May 2007
 * @brief Heap-based allocator.
 *        Out-of-line implementations.
 */

#include "AthAllocators/ArenaHeapAllocator.h"
#include "AthAllocators/ArenaBlock.h"
#include <vector>
#include <algorithm>
#include <cassert>


namespace SG {


/**
 * @brief Constructor.
 * @param params The parameters structure for this allocator.
 *               See @c  ArenaAllocatorBase.h for the contents.
 */
ArenaHeapAllocator::ArenaHeapAllocator (const Params& params)
  : ArenaBlockAllocatorBase (params),
    m_freeptr (nullptr)
{
  // Consistency check.
  assert (params.linkOffset + sizeof (pointer) <= params.eltSize);
}


/**
 * @brief Destructor.  This will free all the Allocator's storage.
 */
ArenaHeapAllocator::~ArenaHeapAllocator()
{
  erase();
}


/**
 * @brief Move constructor.
 */
ArenaHeapAllocator::ArenaHeapAllocator
  (ArenaHeapAllocator&& other)
    : ArenaBlockAllocatorBase (std::move (other)),
      m_freeptr (other.m_freeptr)
{
  other.m_freeptr = nullptr;
}


/**
 * @brief Move assignment.
 */
ArenaHeapAllocator&
ArenaHeapAllocator::operator=
  (ArenaHeapAllocator&& other)
{
  if (this != &other) {
    ArenaBlockAllocatorBase::operator= (std::move (other));
    m_freeptr = other.m_freeptr;
    other.m_freeptr = nullptr;
  }
  return *this;
}


/**
 * @brief Swap.
 */
void ArenaHeapAllocator::swap (ArenaHeapAllocator& other)
{
  if (this != &other) {
    ArenaBlockAllocatorBase::swap (other);
    std::swap (m_freeptr, other.m_freeptr);
  }
}


/**
 * @brief Free all allocated elements.
 *
 * All elements allocated are returned to the free state.
 * @c clear should be called on them if it was provided.
 * The elements may continue to be cached internally, without
 * returning to the system.
 */
void ArenaHeapAllocator::reset()
{
  if (!m_blocks) return;
  if (m_params.clear) {
    if (m_params.canReclear || m_freeptr == nullptr) {
      // Just call clear() on all blocks --- allocated or not.
      ArenaBlock::applyList (m_blocks, m_params.clear, m_blocks->size());
    }
    else {
      // We can only call clear() on allocated blocks.
      slowClear();
    }
  }

  // Move all blocks back to the free list.
  ArenaBlock::appendList (&m_freeblocks, m_blocks);

  // Reset state.
  m_blocks = nullptr;
  m_freeptr = nullptr;
  m_stats.elts.inuse = 0;
  m_stats.blocks.free += m_stats.blocks.inuse;
  m_stats.blocks.inuse = 0;
}


/**
 * @brief Free all allocated elements and release memory back to the system.
 *
 * All elements allocated are freed, and all allocated blocks of memory
 * are released back to the system.
 * @c destructor should be called on them if it was provided
 * (preceded by @c clear if provided and @c mustClear was set).
 */
void ArenaHeapAllocator::erase()
{
  ArenaBlockAllocatorBase::erase();
  m_freeptr = nullptr;
}


/**
 * @brief Add more free elements to the pool, and allocate a new element.
 */
ArenaHeapAllocator::pointer ArenaHeapAllocator::refill()
{
  // Get a new block.
  ArenaBlock* newblock = getBlock();

  // Set up the links for the new free elements.
  pointer lastelt = nullptr;
  size_t sz = newblock->size();
  size_t elt_size = newblock->eltSize();
  for (size_t i=1; i < sz; i++) {
    pointer elt = newblock->index (i, elt_size);
    link(elt) = lastelt;
    lastelt = elt;
  }
  // Set the free pointer to the next-to-last one.
  m_freeptr = newblock->index (sz-1, elt_size);

  // And return the last one.
  return newblock->index (0, elt_size);
}


/**
 * @brief Call @c clear() for all allocated elements.
 */
void ArenaHeapAllocator::slowClear()
{
  // Make a list of all free elements, in sorted order.
  std::vector<pointer> free_ptrs;
  free_ptrs.reserve (m_stats.elts.total - m_stats.elts.inuse);
  for (pointer p = m_freeptr; p; p = link(p)) {
    free_ptrs.push_back (p);
  }
  std::sort (free_ptrs.begin(), free_ptrs.end());

  // Make a list of all used blocks, in sorted order.
  std::vector<ArenaBlock*> blocks;
  for (ArenaBlock* p = m_blocks; p; p = p->link()) {
    blocks.push_back (p);
  }
  std::sort (blocks.begin(), blocks.end());

  // Walk through both of these lists.
  // For each block, walk through its elements, and call @c clear
  // for those not on the free list.
  std::vector<pointer>::iterator pi = free_ptrs.begin();
  std::vector<pointer>::iterator pi_end = free_ptrs.end();
  std::vector<ArenaBlock*>::iterator bi = blocks.begin();
  std::vector<ArenaBlock*>::iterator bi_end = blocks.end();
  func_t* clear = m_params.clear;
  for (; bi != bi_end; ++bi) {
    ArenaBlock& bl = **bi;
    size_t sz = bl.size();
    size_t elt_size = bl.eltSize();
    for (size_t i = 0; i < sz; i++) {
      pointer ptr = bl.index (i, elt_size);
      if (pi != pi_end && ptr == *pi) {
        ++pi;
      }
      else {
        clear (ptr);
      }
    }
  }
}


} // namespace SG
