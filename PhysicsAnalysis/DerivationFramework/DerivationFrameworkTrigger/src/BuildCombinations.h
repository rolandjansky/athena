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
    bool insertIntoSortedVector(std::vector<T>& vec, const T& ele)
    {
      auto itr = std::lower_bound(vec.begin(), vec.end(), ele);
      // if no lower bound is found or the found element doesn't equal ele then
      // insert ele at this point
      if (itr == vec.end() || *itr != ele) {
        vec.insert(itr, ele);
        return true;
      }
      else
        return false;
    }

  /**
   * @brief Helper function to create a sorted vector from an unsorted one.
   * @tparam T The type of the iterator
   * @param begin The start of the unsorted range
   * @param end The end of the unsorted range
   */
  template <typename T>
    std::vector<typename T::value_type> sorted(T begin, T end)
    {
      std::vector<typename T::value_type> output(begin, end);
      std::sort(output.begin(), output.end() );
      return output;
    }
  

  /**
   * @brief Get all valid, unique combinations of distinct elements from the
   * input ranges.
   * @param inputs The ranges over vectors of possible elements.
   */
  // First, a note on the type of the argument. This is essentially a way of
  // passing a vector of vectors but only with iterators over the vectors.
  template <typename T>
    std::vector<std::vector<T>> getAllDistinctCombinations(
        std::vector<RangedItr<typename std::vector<T>::const_iterator>>& inputs)
  {
    // This is where the output will go
    std::vector<std::vector<T>> output;

    // If inputs is empty or if any of the ranges within inputs are empty then
    // no combinations will be found!
    if (inputs.empty() || std::any_of(inputs.begin(), inputs.end(),
          [] (const auto& rangedItr) { return rangedItr.exhausted(); }) )
      return output;

    // This is the current output combination being built
    std::vector<T> currentCombination;

    // This iterator points to where in the vector of ranged iterators we are
    // looking. This can therefore often be acting as an iterator to and
    // iterator! *itr is an iterator, **itr is a const xAOD::IParticle*!
    // To disambiguate I will refer to itr as the outer iterator and *itr as the
    // inner iterator.
    auto itr = inputs.begin();
    while (true) {
      // Have we exhausted our current inner iterator?
      if (itr == inputs.end() || itr->exhausted() ) {
        // If the outer iterator has returned to the beginning then we're done
        // and every combination has been checked.
        if (itr == inputs.begin() )
          break;
        // otherwise we need to continue. To do this we
        // - reset the current inner iterator. This is so that the next time we
        //   return to it we look through all the elements again
        // - step the outer iterator back
        // - remove the last element of the current combination. That
        //   element will be replaced by the next valid one from the list *now*
        //   pointed to by the outer iterator.
        if (itr != inputs.end() )
          itr->restart();
        --itr;
        currentCombination.pop_back();
      } //> end if inner iterator is exhausted
      else {
        // Check if the pointed-to element is already in the current combination
        const T& currentCandidate = **itr;
        // Now that we've accessed this value, step past it
        ++*itr;
        if (std::find(
              currentCombination.begin(), 
              currentCombination.end(), 
              currentCandidate) == currentCombination.end() ) {
          // If it isn't, then add it.
          currentCombination.push_back(currentCandidate);
          // Step the outer iterator forwards so we're looking at the next list
          ++itr;
          if (itr == inputs.end() ) {
            // If we're at the end then this is a complete combination, add it
            // to the output.
            insertIntoSortedVector(output, sorted(
                  currentCombination.begin(),
                  currentCombination.end() ) );
          }
        } //> end if currentCandidate is in currentCombination
      } //> end else (if inner iterator is exhausted)
    } //> end while loop
    return output;
  } //> end function getAllDistinctCombinations

}} //> end namespace DerivationFramework::TriggerMatchingUtils
#endif //> !DERIVATIONFRAMEWORKTRIGGER_BUILDCOMBINATIONS_H
