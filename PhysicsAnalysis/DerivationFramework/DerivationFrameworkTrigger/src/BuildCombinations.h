/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORKTRIGGER_BUILDCOMBINATIONS_H
#define DERIVATIONFRAMEWORKTRIGGER_BUILDCOMBINATIONS_H

#include "RangedItr.h"

namespace DerivationFramework { namespace TriggerMatchingUtils {

  /**
   * @brief Helper function for inserting an element into a sorted vector
   * @tparam T The type stored in the vector
   * @param[out] vec The vector to insert into
   * @param ele The element to insert
   * @return Whether or not the element was inserted.
   * The element will not be inserted if it already exists.
   */
  template <typename T>
    bool insertIntoSortedVector(std::vector<T>& vec, const T& ele);

  /**
   * @brief Helper function to create a sorted vector from an unsorted one.
   * @tparam T The type of the iterator
   * @param begin The start of the unsorted range
   * @param end The end of the unsorted range
   */
  template <typename T>
    std::vector<typename T::value_type> sorted(T begin, T end);

  /**
   * @brief Get all valid, unique combinations of distinct elements from the
   * input ranges.
   * @param inputs The ranges over vectors of possible elements.
   */
  // First, a note on the type of the argument. This is essentially a way of
  // passing a vector of vectors but only with iterators over the vectors.
  template <typename T>
    std::vector<std::vector<T>> getAllDistinctCombinations(
        std::vector<RangedItr<typename std::vector<T>::const_iterator>>& inputs);

}} //> end namespace DerivationFramework::TriggerMatchingUtils
#include "BuildCombinations.icc"
#endif //> !DERIVATIONFRAMEWORKTRIGGER_BUILDCOMBINATIONS_H
