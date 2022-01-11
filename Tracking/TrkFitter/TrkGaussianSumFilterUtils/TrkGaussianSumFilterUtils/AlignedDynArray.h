/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author Anthony Morley, Christos Anastopoulos
 * @brief  Dynamic array fullfilling alignment requirements
 */

#ifndef GSFUtils_AlignedDynArray_H
#define GSFUtils_AlignedDynArray_H
#include <cstdlib>
#include <memory>
#include "CxxUtils/assume_aligned.h"
namespace GSFUtils {

/**
 * A wrapper around std::aligned_alloc
 *
 * The main usage is to create an alligned buffer
 * array to be used with vector instructions
 *
 * Provides
 * - Additional RAII functionality
 * - Default initialization of elements
 * - Value initialization of elements.
 */
template<typename T, size_t ALIGNMENT>
struct AlignedDynArray
{

  // These come from the posix_memalign
  // which is the typical underlying implementation
  static_assert(ALIGNMENT != 0, "Zero alignment 0");
  static_assert(
    (ALIGNMENT & (ALIGNMENT - 1)) == 0,
    "Alignment not a power of 2");
  static_assert(
    (ALIGNMENT % sizeof(void*)) == 0,
    "Alignment not an integral multiple of sizeof(void*)");

  ///@{
  // Standard typedefs
  typedef T value_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef value_type* iterator;
  typedef const value_type* const_iterator;
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;
  /// @}

  /// Deleted default constructor
  AlignedDynArray() = delete;

  /// Deleted default copy constructor
  AlignedDynArray(AlignedDynArray const&) = delete;

  /// Deleted default assignment operator
  AlignedDynArray& operator=(AlignedDynArray const&) = delete;

  /// Constructor with default initializing elements
  explicit AlignedDynArray(const size_type n);

  /// Constructor initializing elements to value
  explicit AlignedDynArray(const size_type n, const T& value);

  /// Move copy constructor
  AlignedDynArray(AlignedDynArray&&) noexcept;

  /// Move assignment operator
  AlignedDynArray& operator=(AlignedDynArray&&) noexcept;

  /// Destructor
  ~AlignedDynArray();

  /// Get the underlying buffer
  pointer buffer() noexcept;

  /// Get the underlying buffer  (const)
  const_pointer buffer() const noexcept;

  /// index array operator
  reference operator[](size_type pos) noexcept;

  /// index array operator (const)
  const_reference operator[](size_type pos) const noexcept;

  /// iterator pointing to the first element
  iterator begin() noexcept;

  /// const iterator pointing to the first element
  const_iterator begin() const noexcept;

  /// iterator pointing to the past-the-end  element
  iterator end() noexcept;

  /// const iterator pointing to the past-the-end  element
  const_iterator end() const noexcept;

  ///  number of elements/size
  size_type size() const noexcept;

  /// returns true is size == 0
  bool empty() const noexcept;

private:
  /// Helper method for calling the dtor for the elements
  void cleanup();

  /// Pointer to the underlying buffer
  pointer m_buffer = nullptr;

  /// Num of elements/size
  size_type m_size = 0;
};

} // namespace GSFUtils

// impl .icc
#include "TrkGaussianSumFilterUtils/AlignedDynArray.icc"

#endif
