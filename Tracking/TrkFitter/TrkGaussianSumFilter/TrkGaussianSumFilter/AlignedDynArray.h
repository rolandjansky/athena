/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AlignedDynArray.h
 * @date   26th November 2019
 * @author Anthony Morley, Christos Anastopoulos
 * @brief  Dynamic array fullfilling alignment requirements
 */

#ifndef GSFUtils_AlignedDynArray_H
#define GSFUtils_AlignedDynArray_H
#include <cstdlib>
#include <memory>
namespace GSFUtils {
template<typename T, size_t Alignment>
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

class AlignedDynArray
{
public:
  /// Deleted default constructor
  AlignedDynArray() = delete;
  /// Deleted default copy constructor
  AlignedDynArray(AlignedDynArray const&) = delete;
  /// Deleted default assignment operator
  AlignedDynArray& operator=(AlignedDynArray const&) = delete;

  /// Constructor default initializing elements
  explicit AlignedDynArray(size_t n);
  /// Constructor initializing elements to value
  explicit AlignedDynArray(size_t n, const T& value);

  /// Move copy constructor
  AlignedDynArray(AlignedDynArray&&) noexcept;
  /// Move assignment operator
  AlignedDynArray& operator=(AlignedDynArray&&) noexcept;
  /// Destructor
  ~AlignedDynArray();

  /// Get the underlying buffer
  T* buffer () noexcept;
  /// Get the underlying buffer
  const T* buffer() const noexcept;

  /// index array operators
  T& operator[](const std::size_t pos) noexcept;
  const T& operator[](const std::size_t pos) const noexcept;

  /// size of allocated buffer
  std::size_t size() const noexcept;

private:
  void cleanup() noexcept;
  T* m_buffer = nullptr;
  size_t m_size = 0;
};

} // namespace GSFUtils

// impl .icc
#include "TrkGaussianSumFilter/AlignedDynArray.icc"

#endif
