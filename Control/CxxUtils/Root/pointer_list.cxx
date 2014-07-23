/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/src/pointer_list.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2009, from earlier code.
 * @brief A fast way to store a variable-sized collection of pointers.
 */


#include "CxxUtils/pointer_list.h"
#include <algorithm>


namespace CxxUtils {


/**
 * @brief Constructor.
 * @param nelt Number of pointers per block.
 *             (excluding the end pointer).
 * @param nblock Number of blocks to allocate per chunk.
 * @param end_mask Mask to use for end-of-block test.
 * @param end_offs Offset to use for end-of-block test.
 */
pointer_list_base::allocator::allocator (size_t nelt,
                                         size_t nblock,
                                         unsigned long end_mask,
                                         unsigned long end_offs)
  : m_nelt (nelt),
    m_nblock (nblock),
    m_chunks (0),
    m_nthis (nblock),
    m_nchunks (0),
    m_end_mask (end_mask),
    m_end_offs (end_offs)
{
}


/**
 * @brief Destructor.  Deletes all blocks from this allocator.
 */
pointer_list_base::allocator::~allocator()
{
  // Loop over the chunks, deleting each one.
  chunk* ch = m_chunks;
  while (ch) {
    chunk* next = ch->m_next;
    delete [] reinterpret_cast<char*> (ch);
    ch = next;
  }
}


/**
 * @brief Allocate a new chunk of blocks.
 */
void pointer_list_base::allocator::refill()
{
  char* p = new char
    [(sizeof(chunk) + m_nblock*list_block::size(m_nelt)) +
     list_block::size(m_nelt)-1];
  chunk* ch = reinterpret_cast<chunk*> (p);

  // Align.
  unsigned long pp = reinterpret_cast<unsigned long> (ch+1);
  pp = (pp + m_end_mask) & ~m_end_mask;
  ch->m_blocks = reinterpret_cast<list_block*> (pp);

  ch->m_next = m_chunks;
  m_chunks = ch;
  ++m_nchunks;

  // No blocks allocated so far from this chunk.
  m_nthis = 0;
}


/**
 * @brief Erase the container.  O(1).
 * Note: doesn't free memory.
 * Memory currently in use will be reused when the container
 * is filled again.
 */
void pointer_list_base::clear()
{
  if (m_head)
    m_insert = &m_head->m_data[0];
  m_size = 0;
}


/**
 * @brief Allocate the first block of the list.
 */
void pointer_list_base::firstblock ()
{
  m_head = getblock();
  m_insert = &m_head->m_data[0];
}


/**
 * @brief Extend the list with another block.
 *        @c m_insert should be at the end of the last block.
 */
void pointer_list_base::nextblock ()
{
  // There may be one already allocated.  Use it if so.
  list_block* newblock = 
    reinterpret_cast<list_block*> (*m_insert);
  if (!newblock) {
    newblock = getblock();
    *m_insert = newblock;
  }
  m_insert = &newblock->m_data[0];
}


/**
 * @brief Allocate a new block.
 */
pointer_list_base::list_block* pointer_list_base::getblock ()
{
  list_block* b = m_pool.allocate();
  size_t maxndx = m_pool.nelt();

  // Make sure only the last element has the sentinel bit set.
  std::fill (b->m_data, b->m_data + maxndx, value_type());
  b->m_data[maxndx] = 0;

  return b;
}
    

} // namespace CxxUtils

