///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// algorithms.h 
// Header file for CxxUtils::copy_if - copied from gcc4.4
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef CXXUTILS_ALGORITHMS_H 
#define CXXUTILS_ALGORITHMS_H 

/**
 *  @brief Copy the elements of a sequence for which a predicate is true.
 *  @ingroup mutating_algorithms
 *  @param  first   An input iterator.
 *  @param  last    An input iterator.
 *  @param  result  An output iterator.
 *  @param  pred    A predicate.
 *  @return   An iterator designating the end of the resulting sequence.
 *
 *  Copies each element in the range @p [first,last) for which
 *  @p pred returns true to the range beginning at @p result.
 *
 *  copy_if() is stable, so the relative order of elements that are
 *  copied is unchanged.
 *
 *  <i>Example</i>: 
 *  @code
 *  CxxUtils::copy_if( in.begin(), in.end(),
 *                     std::back_inserter(out), filter );
 *  @endcode
 *      where in and out are STL-like containers and filter is a predicate
 */

namespace CxxUtils {

template<typename InputIterator, typename OutputIterator,
         typename Predicate>
OutputIterator
copy_if(InputIterator first, InputIterator last,
	OutputIterator result, Predicate pred)
{
  for (; first != last; ++first)
    if (pred(*first)) {
      *result = *first;
      ++result;
    }
  return result;
}

}//> end CxxUtils namespace

#endif //> CXXUTILS_ALGORITHMS_H
