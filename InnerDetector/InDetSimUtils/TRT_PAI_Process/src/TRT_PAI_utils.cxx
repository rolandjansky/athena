/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_PAI_utils.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <vector>
//___________________________________________________________________________

float TRT_PAI_utils::Interpolate(const float& xval,
				 const std::vector<float>& xtabulated,
				 const std::vector<float>& ytabulated) {
  //
  // Interpolate the tabulated x and y values and return the yvalue
  //   that is interpolated from xval.
  // If xval is outside the tabulated x values, returns 0.;

#ifndef NDEBUG

  if ( xtabulated.size() != ytabulated.size() ) {
    std::cerr << "TRT_PAI_Process::TRT_PAI_utils::Interpolate:";
    std::cerr << " ERROR - tabulated arrays must be of similar size"
	      << std::endl;
    std::cerr << "xsize= " << xtabulated.size()
	      << " ysize= " << ytabulated.size()
	      << std::endl;

    return 0;
  }
  if ( xtabulated.empty() ) {
    std::cerr << "TRT_PAI_Process::TRT_PAI_utils::Interpolate:"
	      << "ERROR - tabulated arrays are empty"
	      << std::endl;
    return 0;
  }

  // Maybe also test the assumption that they are already sorted here??

#endif

  // A rather special case:
  if ( xtabulated.size()==1 ) {
    return ytabulated.at(1); //FIXME - surely this will fail if ytabulated.size()==1??
  }
  // For performance reasons, we only get the beginnings and endings out once:
  const std::vector<float>::const_iterator xBeginIter(xtabulated.begin());
  const std::vector<float>::const_iterator xEndIter(xtabulated.end());

  const std::vector<float>::const_iterator xLowerBoundIter(lower_bound(xBeginIter, xEndIter, xval));
  // xLowerBoundIter is now the first item that does not come before xval.
  //
  // This means that:
  // - If one of the xtabulated values equals to xval, xLowerBoundIter points to that value
  // - Otherwise, xLowerBoundIter points to the next tabulated value - or the last value.

  //Y|                                 
  // |                             E
  // |                           +
  // |                         D
  // |                       +
  // |                   
  // |                   +
  // | 
  // |               +
  // |             C
  // |           +
  // |         
  // |       +
  // |     B
  // |   +
  // | A 
  // |   
  // |__________________________________
  //                                   X
  //  '+' represent entries in xtabulated/ytabulated vectors.
  //  Case (A) xval < *(xBeginIter)
  //  xLowerBoundIter==xBeginIter
  //  ==> Special Case: Use xBeginIter and xBeginIter+1
  //  Cases (B), (C) & (D)
  //  Both xLowerBoundIter-1 and xLowerBoundIter are valid iterators
  //  ==> Normal Case: Use xLowerBoundIter-1 and xLowerBoundIter
  //  Case (E) xval > *(xEndIter-1)
  //  xLowerBoundIter==xEndIter
  //  ==> Special Case: xEndIter-2 and xEndIter-1

  if ( xLowerBoundIter==xEndIter ) {
    //Note: this xLowerBoundIter==xEndIter check can cause endless amounts of pain if left out!
    // Interpolation/extrapolation determined by the last two points.
    const std::vector<float>::const_iterator yEndIter(ytabulated.end());
    return TRT_PAI_utils::calculate_interpolation(xval, *(xEndIter-2), *(xEndIter-1), *(yEndIter-2), *(yEndIter-1));
  }

  if ( ((*xLowerBoundIter)-xval)<std::numeric_limits<float>::epsilon() ) {
    //Should never check for exact equality in floats only that they are the same within precision.
    //We lie exactly on a grid point, and must return the corresponding y value
    std::vector<float>::const_iterator yIter(ytabulated.begin());
    std::advance(yIter, std::distance(xBeginIter, xLowerBoundIter));
    return *yIter;
  }

  // We want to interpolate normally, but if xval falls outside the
  // tabulated range, we will extrapolate the tabulation by the line
  // connecting the last two points in either end:

  if (xLowerBoundIter==xBeginIter) {
    // Interpolation/extrapolation determined by the first two points.
    const std::vector<float>::const_iterator yBeginIter(ytabulated.begin());
    return TRT_PAI_utils::calculate_interpolation(xval, *xBeginIter, *(xBeginIter+1), *yBeginIter, *(yBeginIter+1));
  } 

  // We are comfortably inside the array:
  std::vector<float>::const_iterator ylo(ytabulated.begin());
  std::advance(ylo, std::distance(xBeginIter, xLowerBoundIter));
  return TRT_PAI_utils::calculate_interpolation(xval, *(xLowerBoundIter-1), *xLowerBoundIter, *(ylo-1), *ylo);
}

//___________________________________________________________________________
