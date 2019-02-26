// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PackedArray.h,v 1.2 2007-06-14 22:38:48 ssnyder Exp $
/**
 * @file CxxUtils/PackedArray.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2007
 * @brief An array of unsigned values of some bit size, packed tightly.
 */

#ifndef CXXUTILS_PACKEDARRAY_H
#define CXXUTILS_PACKEDARRAY_H


#include <vector>
#include <cstddef>


namespace CxxUtils {


/**
 * @brief An array of unsigned values of some bit size, packed tightly.
 *
 * When creating an instance of this class, specify the bit size of the
 * entries.  The entries will be packed so that each takes exactly that
 * many bits.  For example, if the bit size is 10, 16 entries will
 * be packed into 5 32-bit words.  The bitsize may be changed, but
 * only if the container is empty.
 *
 * Any values assigned to the array that are too large to be represented
 * in the specified number of bits will have the high bits silently dropped.
 *
 * The interface is modeled after @c std::vector, except that there are no
 * iterators.  They could be added if there is a need for them.
 */
class PackedArray
{
private:
  // Type of the underlying vector holding the data.
  typedef std::vector<unsigned int> basetype;

public:
  // Standard STL container typedefs.
  typedef size_t                   size_type;
  typedef unsigned int             value_type;
  typedef basetype::allocator_type allocator_type;

  /**
   * @brief proxy class for representing an lvalue to an element
   *        of @c PackedArray.
   */
  class proxy
  {
  public:
    /// Constructor, from a @c PackedArray and index @c n.
    proxy (PackedArray& arr, size_type n) : m_arr (arr), m_n (n) {}

    /// Retrieve the element referenced by the proxy.
    operator value_type() const { return m_arr.get (m_n); }

    /// Set the element referenced by the proxy to @c v.
    proxy& operator= (value_type v) { m_arr.set (m_n, v); return *this; }

  private:
    /// Reference to the container referenced by the proxy.
    PackedArray& m_arr;

    /// Index of the element referenced by the proxy.
    size_type m_n;
  };


  /**
   * @brief Constructor.
   * @param bitsize The size, in bits, of each element.
   *                Must be greater than zero, and not larger than
   *                the size of an unsigned int.
   * @param allocator Allocator for the underlying vector.
   */
  PackedArray (int bitsize = 8,
               const allocator_type& allocator = allocator_type());

  /**
   * @brief Constructor.
   * @param bitsize The size, in bits, of each element.
   *                Must be greater than zero, and not larger than
   *                the size of an unsigned int.
   * @param n       Initial number of entries in the container.
   * @param val     Value to which the initial entries are to be set.
   * @param allocator Allocator for the underlying vector.
   */
  PackedArray (int bitsize,
               size_type n,
               value_type val = 0,
               const allocator_type& allocator = allocator_type());

  /**
   * @brief Set the container to multiple copies of the same value.
   * @param n       Number of entries to which the container is to be set.
   * @param u       Value to which the entries are to be set.
   */
  void assign (size_type n, value_type u);

  /**
   * @brief Returns the allocator of the underlying vector.
   */
  allocator_type get_allocator() const;

  /**
   * @brief Returns the number of elements in the collection.
   */
  size_type size() const;

  /**
   * @brief Returns the @c size() of the largest possible collection.
   */
  size_type max_size() const;

  /**
   * @brief Returns the total number of elements that the collection can hold
   *        before needing to allocate more memory.
   */
  size_type capacity() const;

  /**
   * @brief Resizes the collection to the specified number of elements.
   * @param sz The new size of the collection.
   * @param c  Value to which any new elements are to be set.
   */
  void resize (size_type sz, value_type c = 0);

  /**
   * @brief Returns @c true if the collection is empty.
   */
  bool empty() const;

  /**
   * @brief Attempt to preallocate enough memory for a specified number
   *        of elements.
   * @param n Number of elements required.
   */
  void reserve (size_type n);

  /**
   * @brief Return the entry at index @c n.
   * @param n The index of the entry to retrieve.
   */
  value_type get(size_type n) const;

  /**
   * @brief Set the entry at index @c n.
   * @param n The index of the entry to set.
   * @param val The new value for the entry at index @c n.
   */
  void set(size_type n, value_type val);

  /**
   * @brief Access an element, as an rvalue.
   * @param n Array index to access.
   * @return The element at @a n.
   *
   * No bounds checking is done.
   * Note that we return a @c value_type rather than a reference.
   */
  value_type operator[](size_type n) const;

  /**
   * @brief Access an element, as an lvalue.
   * @param n Array index to access.
   * @return Proxy to the element at @a n.
   *
   * No bounds checking is done.
   * Note that we return a proxy object rather than a reference.
   */
  proxy operator[](size_type n);

  /**
   * @brief Access an element, as an rvalue.
   * @param n Array index to access.
   * @return The element at @a n.
   *
   * Will raise @c std::out_of_range if the index is out-of-bounds.
   * Note that we return a @c value_type rather than a reference.
   */
  value_type at(size_type n) const;

  /**
   * @brief Access an element, as an lvalue.
   * @param n Array index to access.
   * @return Proxy to the element at @a n.
   *
   * Will raise @c std::out_of_range if the index is out-of-bounds.
   * Note that we return a proxy object rather than a reference.
   */
  proxy at(size_type n);

  /**
   * @brief Access the first element in the collection as an rvalue.
   * @return The first element in the collection.
   *
   * No checking is done to ensure that the container is not empty.
   * Note that we return a @c value_type rather than a reference.
   */
  value_type front() const;

  /**
   * @brief Access the last element in the collection as an rvalue.
   * @return The last element in the collection.
   *
   * No checking is done to ensure that the container is not empty.
   * Note that we return a @c value_type rather than a reference.
   */
  value_type back() const;

  /**
   * @brief Access the first element in the collection as an lvalue.
   * @return Proxy to the first element in the collection.
   *
   * No checking is done to ensure that the container is not empty.
   * Note that we return a proxy object rather than a reference.
   */
  proxy front();

  /**
   * @brief Access the last element in the collection as an lvalue.
   * @return Proxy to the last element in the collection.
   *
   * No checking is done to ensure that the container is not empty.
   * Note that we return a proxy object rather than a reference.
   */
  proxy back();

  /**
   * @brief Add an element to the end of the collection.
   * @param x The element to add to the collection.
   */
  void push_back (value_type x);

  /**
   * @brief Remove the last element from the collection.
   */
  void pop_back();

  /**
   * @brief Swap this collection with another.
   * @param other The collection with which to swap.
   */
  void swap (PackedArray& other);

  /**
   * @brief Erase all the elements in the collection.
   */
  void clear();

  /**
   * @brief Change the bitsize of the container.
   * @brief bitsize The new bitsize.
   *
   * This method may only be called when the container is empty.
   */
  void set_bitsize (int bitsize);

  /**
   * @brief Return the bitsize of the container.
   */
  int bitsize () const;


private:
  /// The current bitsize of the container.
  int m_bitsize;

  /// The current number of entries in the container.
  size_type m_size;

  /// Mask with m_bitsize bits set.
  value_type m_mask;

  /// Underlying vector holding the data.
  basetype m_vec;

  /// Calculate the number of entries in the base vector needed
  /// to hold @n entries with the current bitsize.
  size_t nbase (size_type n) const;

  /// Find the index in the base vector where entry @n starts.
  size_t tondx (size_type n) const;

  /// Find the bit offset of entry @n within its entry in the base vector.
  int tooff (size_type n) const;

  /// Return the entry at base index @c ndx/offset @c off.
  value_type doget (size_type ndx, int off) const;

  /// Set the entry at base index @c ndx/offset @c off to @c v.
  void doset (size_type ndx, int off, value_type v);

  /// Check that @c n is in range and throw @c out_of_range if not.
  void range_check (size_type n) const;
};


} // namespace CxxUtils


#endif // not CXXUTILS_PACKEDARRAY_H
