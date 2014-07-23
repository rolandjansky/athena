/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PackedArray.cxx,v 1.2 2008-12-12 04:26:19 ssnyder Exp $
/**
 * @file CxxUtils/src/PackedArray.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2007
 * @brief An array of unsigned values of some bit size, packed tightly.
 */


#include "CxxUtils/PackedArray.h"
#include <stdexcept>
#include <climits>
#include <cassert>


namespace {


/// Number of bits per entry in the base vector.
const int nper = sizeof (unsigned int) * CHAR_BIT;


/**
 * @brief Return a mask with the lower @c sz bits set.
 * @param sz Number of mask bits to set.
 */
inline
CxxUtils::PackedArray::value_type mask (size_t sz)
{
  return (1<<sz) - 1;
}


} // anonymous namespace


namespace CxxUtils {


/**
 * @brief Calculate the number of entries in the base vector needed
 *        to hold @n entries with the current bitsize.
 * @param n Number of packed entries desired.
 */
inline
PackedArray::size_type PackedArray::nbase (size_type n) const
{
  return (n * m_bitsize + nper-1) / nper;
}


/**
 * @brief Find the index in the base vector where entry @n starts.
 * @param n Packed entry desired.
 */
inline
PackedArray::size_type PackedArray::tondx (size_type n) const
{
  return (n * m_bitsize) / nper;
}


/**
 * @brief Find the bit offset of entry @n within its entry in the base vector.
 * @param n Packed entry desired.
 */
inline
int PackedArray::tooff (size_type n) const
{
  return (n * m_bitsize) % nper;
}


/**
 * @brief Return the entry at base index @c ndx/offset @c off.
 * @param ndx Index of the entry in the base vector at which the
 *            packed entry starts.
 * @param off Bit offset within that entry where the packed entry starts.
 */
inline
PackedArray::value_type PackedArray::doget (size_type ndx, int off) const
{
  // Get the bits from the first entry.
  value_type v = m_vec[ndx] >> off;

  // If the packed entry wraps between two base entries, collect the bits
  // from the next base entry.
  if (m_bitsize > nper - off) {
    int bits = m_bitsize - (nper - off);
    v |= ((m_vec[ndx+1] & mask(bits)) << (nper - off));
  }

  // Mask down to the proper number of bits.
  return v & m_mask;
}


/**
 * @brief Set the entry at base index @c ndx/offset @c off.
 * @param ndx Index of the entry in the base vector at which the
 *            packed entry starts.
 * @param off Bit offset within that entry where the packed entry starts.
 * @param v   Value to which the entry should be set.
 */
inline
void PackedArray::doset (size_type ndx, int off, value_type v)
{
  // Set the bits in the first entry.
  m_vec[ndx] = (m_vec[ndx] & ~(m_mask<<off)) | ((v&m_mask) << off);

  // If the packed entry wraps between two base entries, set the bits
  // in the next entry.
  if (m_bitsize > nper - off) {
    value_type mask2 = mask (m_bitsize - (nper - off));
    m_vec[ndx+1] = (m_vec[ndx+1] & ~mask2) | ((v >> (nper - off)) & mask2);
  }
}


/**
 * @brief Check that @c n is in range and throw @c out_of_range if not.
 * @param n Index to check.
 */
void PackedArray::range_check (size_type n) const
{
  if (n >= m_size) {
    throw std::out_of_range ("PackedArray");
  }
}


/**
 * @brief Constructor.
 * @param bitsize The size, in bits, of each element.
 *                Must be greater than zero, and not larger than
 *                the size of an unsigned int.
 * @param allocator Allocator for the underlying vector.
 */
PackedArray::PackedArray (int bitsize,
                          const allocator_type& allocator/*=allocator_type()*/)
  : m_bitsize (bitsize),
    m_size (0),
    m_mask  (mask (bitsize)),
    m_vec (allocator)
{
  assert (m_bitsize > 0 && m_bitsize <= nper);
}


/**
 * @brief Constructor.
 * @param bitsize The size, in bits, of each element.
 *                Must be greater than zero, and not larger than
 *                the size of an unsigned int.
 * @param n       Initial number of entries in the container.
 * @param val     Value to which the initial entries are to be set.
 * @param allocator Allocator for the underlying vector.
 */
PackedArray::PackedArray (int bitsize,
                          size_type n,
                          value_type val /*= 0*/,
                          const allocator_type& allocator/*=allocator_type()*/)
  : m_bitsize (bitsize),
    m_size (n),
    m_mask  (mask (bitsize)),
    m_vec (nbase(n), 0, allocator)
{
  assert (m_bitsize > 0 && m_bitsize <= nper);
  if (val != 0) {
    for (size_type i = 0; i < n; i++)
      set (i, val);
  }
}


/**
 * @brief Set the container to multiple copies of the same value.
 * @param n       Number of entries to which the container is to be set.
 * @param val     Value to which the entries are to be set.
 */
void PackedArray::assign (size_type n, value_type u /*= 0*/)
{
  m_size = n;
  m_vec.clear();
  m_vec.resize (nbase(n));
  if (u != 0) {
    for (size_type i = 0; i < n; i++)
      set (i, u);
  }
}


/**
 * @brief Returns the allocator of the underlying vector.
 */
PackedArray::allocator_type PackedArray::get_allocator() const
{
  return m_vec.get_allocator();
}


/**
 * @brief Returns the number of elements in the collection.
 */
PackedArray::size_type PackedArray::size() const
{
  return m_size;
}


/**
 * @brief Returns the @c size() of the largest possible collection.
 */
PackedArray::size_type PackedArray::max_size() const
{
  return m_vec.max_size();
}


/**
 * @brief Returns the total number of elements that the collection can hold
 *        before needing to allocate more memory.
 */
PackedArray::size_type PackedArray::capacity() const
{
  return m_vec.capacity() * nper / m_bitsize;
}


/**
 * @brief Resizes the collection to the specified number of elements.
 * @param sz The new size of the collection.
 * @param c  Value to which any new elements are to be set.
 */
void PackedArray::resize (size_type sz, value_type c /*= 0*/)
{
  m_vec.resize (nbase (sz));
  if (sz > m_size) {
    // Making the container bigger.  Need to fill the remaining values.
    if (c != 0) {
      // Set them to something non-zero.
      for (size_t i = m_size; i < sz; i++)
        set (i, c);
    }
    else {
      // Filling the new entries with 0.
      // New elements in the base vector will have been set to 0.
      // However, we also need to zero out any remaining packed elements
      // (or piece thereof) in the last base element that was occupied
      // before the resize.
      int off = tooff (m_size);
      // Don't need to do anything if packed entries exactly fit
      // in the allocated base size.
      if (off != 0) {
        size_t ndx = tondx (m_size);
        m_vec[ndx] &= mask (off);
      }
    }
  }
  m_size = sz;
}


/**
 * @brief Returns @c true if the collection is empty.
 */
bool PackedArray::empty() const
{
  return m_size == 0;
}


/**
 * @brief Attempt to preallocate enough memory for a specified number
 *        of elements.
 * @param n Number of elements required.
 */
void PackedArray::reserve (size_type n)
{
  m_vec.reserve (nbase (n));
}


/**
 * @brief Return the entry at index @c n.
 * @param n The index of the entry to retrieve.
 */
PackedArray::value_type PackedArray::get (size_type n) const
{
  return doget (tondx (n), tooff (n));
}


/**
 * @brief Set the entry at index @c n.
 * @param n The index of the entry to set.
 * @param val The new value for the entry at index @c n.
 */
void PackedArray::set (size_type n, value_type val)
{
  return doset (tondx (n), tooff (n), val);
}


/**
 * @brief Access an element, as an rvalue.
 * @param n Array index to access.
 * @return The element at @a n.
 *
 * No bounds checking is done.
 * Note that we return a @c value_type rather than a reference.
 */
PackedArray::value_type PackedArray::operator[] (size_type n) const
{
  return doget (tondx (n), tooff (n));
}


/**
 * @brief Access an element, as an lvalue.
 * @param n Array index to access.
 * @return Proxy to the element at @a n.
 *
 * No bounds checking is done.
 * Note that we return a proxy object rather than a reference.
 */
PackedArray::proxy PackedArray::operator[] (size_type n)
{
  return proxy (*this, n);
}


/**
 * @brief Access an element, as an rvalue.
 * @param n Array index to access.
 * @return The element at @a n.
 *
 * Will raise @c std::out_of_range if the index is out-of-bounds.
 * Note that we return a @c value_type rather than a reference.
 */
PackedArray::value_type PackedArray::at (size_type n) const
{
  range_check (n);
  return doget (tondx (n), tooff (n));
}


/**
 * @brief Access an element, as an lvalue.
 * @param n Array index to access.
 * @return Proxy to the element at @a n.
 *
 * Will raise @c std::out_of_range if the index is out-of-bounds.
 * Note that we return a proxy object rather than a reference.
 */
PackedArray::proxy PackedArray::at (size_type n)
{
  range_check (n);
  return proxy (*this, n);
}


/**
 * @brief Access the first element in the collection as an rvalue.
 * @return The first element in the collection.
 *
 * No checking is done to ensure that the container is not empty.
 * Note that we return a @c value_type rather than a reference.
 */
PackedArray::value_type PackedArray::front () const
{
  return doget (0, 0);
}


/**
 * @brief Access the last element in the collection as an rvalue.
 * @return The last element in the collection.
 *
 * No checking is done to ensure that the container is not empty.
 * Note that we return a @c value_type rather than a reference.
 */
PackedArray::value_type PackedArray::back () const
{
  return doget (tondx (m_size-1), tooff (m_size-1));
}


/**
 * @brief Access the first element in the collection as an lvalue.
 * @return Proxy to the first element in the collection.
 *
 * No checking is done to ensure that the container is not empty.
 * Note that we return a proxy object rather than a reference.
 */
PackedArray::proxy PackedArray::front ()
{
  return proxy (*this, 0);
}


/**
 * @brief Access the last element in the collection as an lvalue.
 * @return Proxy to the last element in the collection.
 *
 * No checking is done to ensure that the container is not empty.
 * Note that we return a proxy object rather than a reference.
 */
PackedArray::proxy PackedArray::back ()
{
  return proxy (*this, m_size-1);
}


/**
 * @brief Add an element to the end of the collection.
 * @param x The element to add to the collection.
 */
void PackedArray::push_back (value_type x)
{
  ++m_size;
  size_t nb = nbase (m_size);
  if (nb != m_vec.size())
    m_vec.resize (nb);
  doset (tondx (m_size-1), tooff (m_size-1), x);
}


/**
 * @brief Remove the last element from the collection.
 */
void PackedArray::pop_back ()
{
  --m_size;
  size_t nb = nbase (m_size);
  if (nb != m_vec.size())
    m_vec.resize (nb);
}


/**
 * @brief Swap this collection with another.
 * @param other The collection with which to swap.
 */
void PackedArray::swap (PackedArray& other)
{
  std::swap (m_bitsize, other.m_bitsize);
  std::swap (m_size,    other.m_size);
  std::swap (m_mask,    other.m_mask);
  std::swap (m_vec,     other.m_vec);
}


/**
 * @brief Erase all the elements in the collection.
 */
void PackedArray::clear ()
{
  m_size = 0;
  m_vec.clear();
}


/**
 * @brief Change the bitsize of the container.
 * @brief bitsize The new bitsize.
 *
 * This method may only be called when the container is empty.
 */
void PackedArray::set_bitsize (int bitsize)
{
  assert (m_size == 0);
  assert (bitsize > 0 && bitsize <= nper);
  m_bitsize = bitsize;
  m_mask = mask (bitsize);
}


/**
 * @brief Return the bitsize of the container.
 */
int PackedArray::bitsize () const
{
  return m_bitsize;
}


} // namespace CxxUtils
