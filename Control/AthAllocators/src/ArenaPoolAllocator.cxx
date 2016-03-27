/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaPoolAllocator.cxx 470529 2011-11-24 23:54:22Z ssnyder $
/**
 * @file  AthAllocators/src/ArenaPoolAllocator.cxx
 * @author scott snyder
 * @date May 2007
 * @brief Pool-based allocator.
 *        Out-of-line implementations.
 */

#include "AthAllocators/ArenaPoolAllocator.h"
#include "AthAllocators/ArenaBlock.h"
#include <cassert>


namespace SG {


/**
 * @brief Helper: common code for advancing an iterator.
 * @param base Element pointer.
 * @param block Block pointer.
 */
inline
ArenaPoolAllocator::pointer
ArenaPoolAllocator_iterator_increment (ArenaPoolAllocator::pointer base,
                                       ArenaBlock* & block)
{
  // If we haven't yet reached the start of this block, move the iterator
  // back one.
  if (base > block->index(0,0))
    return base - block->eltSize();
  else {
    // Move to the previous block.
    block = block->link();
    if (block)
      return block->index (block->size()-1, block->eltSize());
    else
      return 0;
  }
}


/**
 * @brief Move the iterator forward.
 */
void ArenaPoolAllocator::iterator::increment()
{
  this->base_reference() =
    ArenaPoolAllocator_iterator_increment (this->base_reference(), m_block);
}


/**
 * @brief Move the iterator forward.
 */
void ArenaPoolAllocator::const_iterator::increment()
{
  ArenaPoolAllocator::pointer base =
    const_cast<ArenaPoolAllocator::pointer> (this->base_reference());
  base = ArenaPoolAllocator_iterator_increment (base, m_block);
  this->base_reference() = base;
}


/**
 * @brief Constructor.
 * @param params The parameters structure for this allocator.
 *               See @c  ArenaAllocatorBase.h for the contents.
 */
ArenaPoolAllocator::ArenaPoolAllocator (const Params& params)
  : ArenaBlockAllocatorBase (params),
    m_ptr (0),
    m_end (0)
{
}


/**
 * @brief Destructor.  This will free all the Allocator's storage.
 */
ArenaPoolAllocator::~ArenaPoolAllocator()
{
  erase();
}


/**
 * @brief Free all allocated elements.
 *
 * All elements allocated are returned to the free state.
 * @c clear should be called on them if it was provided.
 * The elements may continue to be cached internally, without
 * returning to the system.
 */
void ArenaPoolAllocator::reset()
{
  // Clear each block in turn.
  while (m_blocks)
    clearBlock();

  // Check that things are consistent.
  assert (m_stats.elts.inuse == 0);
  assert (m_ptr == 0);
  assert (m_end == 0);
}


/**
 * @brief Free all allocated elements and release memory back to the system.
 *
 * All elements allocated are freed, and all allocated blocks of memory
 * are released back to the system.
 * @c destructor should be called on them if it was provided
 * (preceded by @c clear if provided and @c mustClear was set).
 */
void ArenaPoolAllocator::erase()
{
  // Delete all the blocks.
  ArenaBlockAllocatorBase::erase();

  // Reset pointers.
  m_ptr = m_end = 0;
}


/**
 * @brief Reset pool back to a previous state.
 * @param blk The pointer back to which to reset.
 *
 * This will free (a la @c reset) the element @c p and all elements
 * that have been allocated after it from this allocator.
 */
void ArenaPoolAllocator::resetTo (pointer blk)
{
  // Clear the topmost block, as long as it doesn't contain the sought-after
  // pointer.
  ArenaBlock* p = m_blocks;
  assert (p != 0);
  size_t elt_size = p->eltSize();
  while (p && !(blk >= p->index(0, elt_size) &&
                blk < p->index(p->size(), elt_size)))
  {
    clearBlock();
    p = m_blocks;
  }

  // We'll trip this if the supplied pointer wasn't in any block.
  assert (p != 0);

  // If the element is at the beginning of this block, just clear the whole
  // thing.
  if (blk == p->index(0, elt_size))
    clearBlock();
  else {
    // Otherwise, we need to clear some of the elements in this block.
    // See if we need to call @c clear.
    func_t* clear = m_params.clear;
    if (clear) {
      // Yeah, we do.  Call @c clear on each element in turn.
      while (m_ptr > blk) {
        m_ptr -= elt_size;
        clear (m_ptr);
        --m_stats.elts.inuse;
      }
      // We'll trip this if the supplied pointer wasn't on an element
      // boundary.
      assert (m_ptr == blk);
    }
    else {
      // We don't need to call @c clear.
      // So we can do it the fast way --- just reset pointers.
      m_stats.elts.inuse -= (m_ptr - blk) / elt_size;
      m_ptr = blk;
    }
  }
}


/**
 * @brief Starting pool iterator.
 *
 * This will iterate over all allocated elements (in unspecified order).
 * It is a @c forward_iterator.
 */
ArenaPoolAllocator::iterator ArenaPoolAllocator::begin()
{
  // If @c m_ptr is one set, it is one more than the last allocated element.
  return iterator (m_ptr ? m_ptr - m_params.eltSize : 0, m_blocks);
}


/**
 * @brief Starting pool const iterator.
 *
 * This will iterate over all allocated elements (in unspecified order).
 * It is a @c forward_iterator.
 */
ArenaPoolAllocator::const_iterator ArenaPoolAllocator::begin() const
{
  // If @c m_ptr is one set, it is one more than the last allocated element.
  return const_iterator (m_ptr ? m_ptr - m_params.eltSize : 0, m_blocks);
}


/**
 * @brief Ending pool iterator.
 */
ArenaPoolAllocator::iterator ArenaPoolAllocator::end()
{
  // Use a null iterator to signal the end.
  return iterator ();
}


/**
 * @brief Ending pool const iterator.
 */
ArenaPoolAllocator::const_iterator ArenaPoolAllocator::end() const
{
  // Use a null iterator to signal the end.
  return const_iterator ();
}


/**
 * @brief Add more free elements to the pool.
 */
void ArenaPoolAllocator::refill()
{
  // Get a new block.
  ArenaBlock* newblock = getBlock();

  // Set the pointers.
  m_ptr = newblock->index (0,                m_params.eltSize);
  m_end = newblock->index (newblock->size(), m_params.eltSize);
}


/**
 * @brief Reset all elements in the topmost block, and move the block
 *        to the free list.
 */
void ArenaPoolAllocator::clearBlock()
{
  // The topmost block.
  ArenaBlock* p = m_blocks;

  // Nothing to do if there are no allocated blocks!
  if (!p) return;

  size_t elt_size = p->eltSize();

  // The first element of the block.
  pointer base = p->index(0, elt_size);

  // Do we need to call @c clear?  If so, call it on all allocated elements
  // in this block.
  func_t* clear = m_params.clear;
  if (clear) {
    pointer elt = base;
    while (elt < m_ptr) {
      clear (elt);
      elt += elt_size;
    }
  }

  // Update statistics.
  --m_stats.blocks.inuse;
  ++m_stats.blocks.free;
  m_stats.elts.inuse -= (m_ptr - base) / elt_size;

  // Move the block from the allocated to the free list.
  m_blocks = p->link();
  p->link() = m_freeblocks;
  m_freeblocks = p;
  p = m_blocks;

  // Reset the pointers.
  if (p) {
    m_ptr = m_end = p->index(p->size());
  }
  else {
    m_ptr = m_end = 0;
  }
}


} // namespace SG
