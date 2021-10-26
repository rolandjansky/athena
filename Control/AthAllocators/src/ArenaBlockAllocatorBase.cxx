/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  AthAllocators/src/ArenaBlockAllocatorBase.cxx
 * @author scott snyder
 * @date May 2007
 * @brief Common functionality for block-oriented allocators.
 *        Out-of-line implementations.
 */

#include "AthAllocators/ArenaBlockAllocatorBase.h"
#include "AthAllocators/ArenaBlock.h"
#include "AthAllocators/exceptions.h"


namespace SG {


/**
 * @brief Constructor.
 * @param params The parameters structure for this allocator.
 *               See @c  ArenaAllocatorBase.h for the contents.
 */
ArenaBlockAllocatorBase::ArenaBlockAllocatorBase (const Params& params)
  : m_params (params),
    m_blocks (nullptr),
    m_freeblocks (nullptr),
    m_stats(),
    m_protected (false)
{
}


/**
 * @brief Destructor.
 */
ArenaBlockAllocatorBase::~ArenaBlockAllocatorBase()
{
  // Should be called by the derived class dtor.
  // Can't do it from here since it may call reset(), which is
  // pure virtual in this class.
  //erase();
}


/**
 * @brief Move constructor.
 */
ArenaBlockAllocatorBase::ArenaBlockAllocatorBase
  (ArenaBlockAllocatorBase&& other)
    : m_params (other.m_params),
      m_blocks (other.m_blocks),
      m_freeblocks (other.m_freeblocks),
      m_stats (other.m_stats),
      m_protected (other.m_protected)
{
  other.m_blocks = nullptr;
  other.m_freeblocks = nullptr;
  other.m_stats.clear();
  other.m_protected = false;
}


/**
 * @brief Move assignment.
 */
ArenaBlockAllocatorBase&
ArenaBlockAllocatorBase::operator=
  (ArenaBlockAllocatorBase&& other)
{
  if (this != &other) {
    erase();
    m_params = other.m_params;
    m_blocks = other.m_blocks;
    m_freeblocks = other.m_freeblocks;
    m_stats = other.m_stats;
    m_protected = other.m_protected;
    other.m_blocks = nullptr;
    other.m_freeblocks = nullptr;
    other.m_stats.clear();
    other.m_protected = false;
  }
  return *this;
}


/**
 * @brief Swap.
 */
void ArenaBlockAllocatorBase::swap (ArenaBlockAllocatorBase& other)
{
  if (this != &other) {
    std::swap (m_params, other.m_params);
    std::swap (m_blocks, other.m_blocks);
    std::swap (m_freeblocks, other.m_freeblocks);
    std::swap (m_stats, other.m_stats);
    std::swap (m_protected, other.m_protected);
  }
}


/**
 * @brief Set the total number of elements cached by the allocator.
 * @param size The desired pool size.
 *
 * This allows changing the number of elements that are currently free
 * but cached.  Any allocated elements are not affected by this call.
 *
 * If @c size is greater than the total number of elements currently
 * cached, then more will be allocated.  This will preferably done
 * with a single block, but that is not guaranteed; in addition, the
 * allocator may allocate more elements than is requested.
 *
 * If @c size is smaller than the total number of elements currently
 * cached, as many blocks as possible will be released back to the system.
 * It may not be possible to release the number of elements requested;
 * this should be implemented on a best-effort basis.
 */
void ArenaBlockAllocatorBase::reserve (size_t size)
{
  if (m_protected) {
    throw SG::ExcProtected();
  }
  if (size > m_stats.elts.total) {
    // Growing the pool.
    // Make a new block of the required size.
    size_t sz = size - m_stats.elts.total;
    ArenaBlock* newblock = ArenaBlock::newBlock (sz, m_params.eltSize,
                                                 m_params.constructor);

    // Update statistics (others are derived in stats()).
    ++m_stats.blocks.free;
    ++m_stats.blocks.total;
    m_stats.elts.total += newblock->size();

    // Add to the free list.
    newblock->link() = m_freeblocks;
    m_freeblocks = newblock;
  }
  else {
    // Shrinking the pool.
    // Loop while we can get rid of the first free block.
    while (size < m_stats.elts.total &&
           m_freeblocks &&
           m_freeblocks->size() <= (m_stats.elts.total - size))
    {
      // Remove it from the free list.
      ArenaBlock* p = m_freeblocks;
      m_freeblocks = m_freeblocks->link();

      // Update statistics (others are derived in stats()).
      m_stats.elts.total -= p->size();
      --m_stats.blocks.free;
      --m_stats.blocks.total;

      // Free the block.
      ArenaBlock::destroy (p, m_params.destructor);
    }
  }
}


/**
 * @brief Free all allocated elements and release memory back to the system.
 *
 * All elements allocated are freed, and all allocated blocks of memory
 * are released back to the system.
 * @c destructor should be called on them if it was provided
 * (preceded by @c clear if provided and @c mustClear was set).
 */
void ArenaBlockAllocatorBase::erase()
{
  if (m_protected) {
    throw SG::ExcProtected();
  }
  // Do we need to run clear() on the allocated elements?
  // If so, do so via reset().
  if (m_params.mustClear && m_params.clear) {
    reset();
  }

  // Kill the block lists (both free and in use).
  ArenaBlock::destroyList (m_blocks, m_params.destructor);
  ArenaBlock::destroyList (m_freeblocks, m_params.destructor);
  m_blocks = m_freeblocks = nullptr;

  // Reset statistics.
  m_stats.clear();
}


/**
 * @brief Return the statistics block for this allocator.
 */
ArenaAllocatorBase::Stats ArenaBlockAllocatorBase::stats() const
{
  // Calculate derived statistics.
  Stats stats = m_stats;
  stats.elts.free = stats.elts.total - stats.elts.inuse;
  stats.bytes.inuse = stats.elts.inuse   * m_params.eltSize +
                      stats.blocks.inuse * ArenaBlock::overhead();
  stats.bytes.total = stats.elts.total   * m_params.eltSize +
                      stats.blocks.total * ArenaBlock::overhead();
  stats.bytes.free  = stats.elts.free   * m_params.eltSize +
                      stats.blocks.free * ArenaBlock::overhead();
  return stats;
}


/**
 * @brief Return the name of this allocator.
 */
const std::string& ArenaBlockAllocatorBase::name() const
{
  return m_params.name;
}


/**
 * @brief Return this allocator's parameters.
 */
const ArenaAllocatorBase::Params&
ArenaBlockAllocatorBase::params() const
{
  return m_params;
}


/**
 * @brief Return an empty block, either newly-allocated or from the
 *        free list.  Update statistics appropriately.
 */
ArenaBlock* ArenaBlockAllocatorBase::getBlock()
{
  if (m_protected) {
    throw SG::ExcProtected();
  }

  ArenaBlock* newblock = m_freeblocks;
  if (newblock) {
    // There's something on the free list.  Remove it and update statistics.
    m_freeblocks = newblock->link();
    --m_stats.blocks.free;
  }
  else {
    // Otherwise, we need to make a new block.
    newblock = ArenaBlock::newBlock (m_params.nblock, m_params.eltSize,
                                     m_params.constructor);
    m_stats.elts.total += newblock->size();
    ++m_stats.blocks.total;
  }
  // Finish updating statistics.
  // (Remaining ones are computed in stats().)
  ++m_stats.blocks.inuse;

  // Link it into the in-use list and return.
  newblock->link() = m_blocks;
  m_blocks = newblock;
  return newblock;
}


/**
 * @brief Write-protect the memory managed by this allocator.
 *
 * Adjust protection on the memory managed by this allocator
 * to disallow writes.
 */
void ArenaBlockAllocatorBase::protect()
{
  SG::ArenaBlock::protectList (m_blocks);
  m_protected = true;
}


/**
 * @brief Write-enable the memory managed by this allocator.
 *
 * Adjust protection on the memory managed by this allocator
 * to allow writes.
 */
void ArenaBlockAllocatorBase::unprotect()
{
  if (m_protected) {
    SG::ArenaBlock::unprotectList (m_blocks);
    m_protected = false;
  }
}


} // namespace SG
