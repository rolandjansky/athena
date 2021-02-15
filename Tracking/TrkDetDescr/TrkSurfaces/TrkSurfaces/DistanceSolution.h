/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DistanceSolution.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSURFACES_DISTANCESOLUTION_H
#define TRKSURFACES_DISTANCESOLUTION_H

#include <cmath>

namespace Trk {

/**
 @class DistanceSolution
 Access to distance solutions. Driven by need to accomodate
 intersections with a cylinder into common interface.

 @author Sarka.Todorova@cern.ch
 */

class DistanceSolution
{
public:
  DistanceSolution() = default;
  DistanceSolution(const DistanceSolution&) = default;
  DistanceSolution(DistanceSolution&&) = default;
  DistanceSolution& operator=(const DistanceSolution&) = default;
  DistanceSolution& operator=(DistanceSolution&&) = default;
  ~DistanceSolution() = default;

   /**Constructor*/
  DistanceSolution(int num,
                   double current = 0.,
                   bool signedDist = false,
                   double first = 0.,
                   double second = 0.);

  // methods to access solutions
  /** Number of intersection solutions*/
  int numberOfSolutions() const;

  /** Distance to first intersection solution along direction*/
  double first() const;

  /** Distance to second intersection solution along direction (for a cylinder
   * surface)*/
  double second() const;

  /** Absolute Distance to closest solution */
  double absClosest() const;

  /** Distance to point of closest approach along direction*/
  double toPointOfClosestApproach() const;

  /** Current distance to surface (spatial), signed (along/opposite to surface
   * normal) if input argument true (absolute value by default)*/
  double currentDistance(bool signedDist = false) const;

  /** This method indicates availability of signed current distance (false for
   * Perigee and StraighLineSurface) */
  bool signedDistance() const;

private:
  int m_num;
  double m_first;
  double m_second;
  double m_current;
  bool m_signedDist;
};

} // end of namespace

#include "TrkSurfaces/DistanceSolution.icc"
#endif // TRKSURFACES_DISTANCESOLUTION_H
