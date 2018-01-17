// $Id$
/**
 * @file CxxUtils/src/ConcurrentBitset.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2017
 * @brief Variable-sized bitset allowing (mostly) concurrent access.
 */


#include "CxxUtils/ConcurrentBitset.h"


namespace CxxUtils {


//*********************************************************************************
// Constructors, destructors, assignment.


/**
 * @brief Constructor.
 * @param nbits Initial number of bits to allocate for the map.
 */
ConcurrentBitset::ConcurrentBitset (bit_t nbits /*= 0*/)
  : m_impl (newImpl (nbits))
{
  new (m_impl) Impl (nbits);
}


/**
 * @brief Copy constructor.
 * @param other Container to copy.
 *
 * The copy is not atomic.  If a non-atomic update is simultaneously made
 * to @c other, then the copy may have this update only partially completed.
 */
ConcurrentBitset::ConcurrentBitset (const ConcurrentBitset& other)
  : m_impl (newImpl ((*other.m_impl).nbits()))
{
  new (m_impl) Impl ((*other.m_impl));
}


/**
 * @brief Constructor from an initializer list.
 * @param List of values to set.
 * @param nbits Number of bits to allocate for the map.
 *              If 0, then set the size based on the maximum value in the list.
 *
 * This allows setting specific bits in the set, like
 *@code
 *  ConcurrentBitset bs { 1, 5, 10};
 @endcode
*/
ConcurrentBitset::ConcurrentBitset (std::initializer_list<bit_t> l,
                                    bit_t nbits /*= 0*/)
{
  if (nbits == 0) {
    // Set the size of the set based on the maximum value in the list.
    auto max_it = std::max_element (l.begin(), l.end());
    if (max_it != l.end()) {
      nbits = *max_it + 1;
    }
    // Round up.
    nbits = (nbits + BLOCKSIZE-1) & ~MASK;
  }
  m_impl = newImpl (nbits);
  new (m_impl) Impl (nbits);
  for (bit_t b : l) {
    set (b);
  }
}


/**
 * @brief Destructor.
 */
ConcurrentBitset::~ConcurrentBitset()
{
  free (m_impl);
  emptyGarbage();
}


/**
 * @brief Assignment.
 * @param other Bitset from which to assign.
 *
 * The copy is not atomic.  If a non-atomic update is simultaneously made
 * to @c other, then the copy may have this update only partially completed.
 */
ConcurrentBitset& ConcurrentBitset::operator= (const ConcurrentBitset& other)
{
  if (this != &other) {
    const Impl* otherImpl = other.m_impl;
    expand (otherImpl->nbits());
    (*m_impl).assign (*otherImpl);
  }
  return *this;
}


/**
 * @brief Clean up old versions of the set.
 *
 * The insert and assignment operations may need to grow the set.
 * The original version of the set is not deleted immediately, since other threads
 * may still be accessing it.  Call this when no other threads can be accessing
 * old versions in order to clean them up.
 */
void ConcurrentBitset::emptyGarbage()
{
  lock_t lock (m_mutex);
  for (Impl* p : m_garbage) {
    free (p);
  }
  m_garbage.clear();
}


/**
 * @brief Expand the container: out-of-line portion.
 * @param new_nbits The desired new size of the container.
 */
void ConcurrentBitset::expandOol (bit_t new_nbits)
{
  // Need to take out the lock.
  lock_t lock (m_mutex);
  // Check the size again while we're holding the lock.
  bit_t nbits = (*m_impl).nbits();
  if (new_nbits > nbits) {
    // We need to expand.  Allocate a new implementation and initialize it,
    // copying from the existing implementation.
    Impl* i = newImpl (new_nbits);
    new (i) Impl (*m_impl, new_nbits);

    // Remember that we need to delete the old object.
    m_garbage.push_back (m_impl);

    // Publish the new one.
    m_impl = i;
  }
}


//*********************************************************************************
// Implementation.


/**
 * @brief Count the number of 1 bits in the set.
 */
ConcurrentBitset::bit_t
ConcurrentBitset::Impl::count() const
{
  bit_t n = 0;
  for (bit_t i=0; i<m_nblocks; i++) {
    n += CxxUtils::count_ones (m_data[i]);
  }
  return n;
}


/**
 * @brief Return true if there are no 1 bits in the set.
 */
bool ConcurrentBitset::Impl::none() const
{
  for (bit_t i = 0; i < m_nblocks; i++) {
    if (m_data[i]) return false;
  }
  return true;
}


/**
 * @brief Return true if all bits in the set are 1.
 */
bool ConcurrentBitset::Impl::all() const
{
  // Check all blocks except the last.
  for (bit_t i = 0; i < m_nblocks-1; i++) {
    if (m_data[i] != ~static_cast<Block_t>(0)) return false;
  }
  // Special case for the last, since the last block may not be full.
  if (m_nblocks > 0) {
    if (m_data[m_nblocks] != ones<Block_t> (m_nbits - (m_nblocks-1)*BLOCKSIZE)) {
      return false;
    }
  }
  return true;
}


/**
 * @brief Turn on all bits in the set.
 *
 * This operation is not necessarily atomic; a simultaneous read may be able
 * to see the operation partially done.
 */
void ConcurrentBitset::Impl::set()
{
  for (bit_t i=0; i<m_nblocks; i++) {
    m_data[i].store (~static_cast<Block_t>(0), std::memory_order_relaxed);
  }
  std::atomic_thread_fence (std::memory_order_seq_cst);
  if (m_nblocks > 0) {
    m_hwm = m_nblocks-1;
  }
}


/**
 * @brief Flip the state of all bits in the set.
 *
 * This operation is not necessarily atomic; a simultaneous read may be able
 * to see the operation partially done.
 */
void ConcurrentBitset::Impl::flip()
{
  for (bit_t i=0; i<m_nblocks; i++) {
    m_data[i] ^= ~static_cast<Block_t>(0);
  }
  if (m_nblocks > 0) {
    m_hwm = m_nblocks-1;
  }
}


} // namespace CxxUtils
