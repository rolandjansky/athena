/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AlignedDynArray.h
 * @date   26th November 2019
 * @author amorley, christos
 * @brief  Dynamic array fullfilling alignment requirements
 *********************************************************************************/

#ifndef GSFUtils_AlignedDynArray_H
#define GSFUtils_AlignedDynArray_H
#include <cstdlib>
#include <memory>
namespace GSFUtils {
template<typename T, size_t Alignment>
/**
 * @bried A wrapper around std::aligned_alloc
 * https://en.cppreference.com/w/cpp/memory/c/aligned_alloc
 *
 * Provides
 * - Additional RAII functionality 
 * - Default initialization of elements
 * - Initialization with copies of elements with value value.
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
  AlignedDynArray(AlignedDynArray&&);
  /// Move assignment operator
  AlignedDynArray& operator=(AlignedDynArray&&);
  /// Destructor
  ~AlignedDynArray();

  /// Conversions to T*
  operator T*();
  /// Conversions to const T*
  operator const T*() const;

  /// index array operators
  T& operator[](const std::size_t pos);
  const T& operator[](const std::size_t pos) const;

  /// size of allocated buffer
  std::size_t size() const;

private:
  void cleanup();
  T* m_buffer = nullptr;
  size_t m_size = 0;
};

} // namespace GSFUtils

// impl .icc
#include "TrkGaussianSumFilter/AlignedDynArray.icc"

#endif
