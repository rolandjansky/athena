/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IntersectionSolution.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXUTILS_INTERSECTIONSOLUTION_H
#define TRKEXUTILS_INTERSECTIONSOLUTION_H

// Trk

#include <vector>
#include <iosfwd>
#include <memory>


class MsgStream;

namespace Trk {
class TrackSurfaceIntersection;
/** @class IntersectionSolution
   @author Andreas.Salzburger@cern.ch
   updated June 2021 (shaun roe) to be simple typedef
  */
  
typedef std::vector<std::unique_ptr<const Trk::TrackSurfaceIntersection>> 
  IntersectionSolution;
typedef std::vector<std::unique_ptr<const Trk::TrackSurfaceIntersection>>::const_iterator
  IntersectionSolutionIter;


/**Overload of << operator for both, MsgStream and std::ostream for debug
 * output*/
MsgStream&
operator<<(MsgStream& sl, const IntersectionSolution& sf);
std::ostream&
operator<<(std::ostream& sl, const IntersectionSolution& sf);

} // end of namespace

#endif // TRKEXUTILS_INTERSECTIONSOLUTION_H

