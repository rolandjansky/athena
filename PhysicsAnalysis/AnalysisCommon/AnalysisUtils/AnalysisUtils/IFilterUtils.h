///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IFilterUtils.h 
// Header file for class IFilterUtils
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ANALYSISUTILS_IFILTERUTILS_H 
#define ANALYSISUTILS_IFILTERUTILS_H 

/** IFilterUtils : Implements a copy iterator (STL is missing this utility, 
 *  though one could use the std::remove_if with the negative predicate).
 *  The output destination container is filled with the current
 *  iterator on the input container, only if the predicate is true.
 *  <i>Example</i>: 
 *  @code
 *  AnalysisUtils::copy_if( in.begin(), in.end(),
 *                          std::back_inserter(out), filter );
 *  @endcode
 *      where in and out are STL-like containers and filter is a predicate
 *      (in our case, it could be a reference toward an IFilter).
 */

namespace AnalysisUtils {

template<class In, class Out, class Pred>
Out copy_if(In first, const In& last, Out res, const Pred& p)
{
  for ( ; first != last; ++first ) {
    if ( p(*first) ) {
      *res++ = *first;
    }
  }
  return res;
}

}//> end AnalysisUtils namespace

#endif //> ANALYSISUTILS_IFILTERUTILSS_H
