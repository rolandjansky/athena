/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LineIntersection2D.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXUTILS_LINEINTERSECTION2D_H
#define TRKEXUTILS_LINEINTERSECTION2D_H

#include <cmath>
#include <utility>

#ifndef NUMERICALSTABLE
#define NUMERICALSTABLE 10e-7
#endif

namespace Trk {

/* @struct LineIntersction2D

  Helper method having 2 lines given with two sets of points each

  (x1,y1)(x2,y2) - line a
  (X1,Y1)(X2,Y2) - line b

  @author Andreas.Salzburger@cern.ch
 */

struct LineIntersection2D
{

  bool valid;
  double interX;
  double interY;
  double k, K, d, D;

  LineIntersection2D(double x1,
                     double y1,
                     double x2,
                     double y2,
                     double X1,
                     double Y1,
                     double X2,
                     double Y2)
  {
    // diffs, k's and d's
    double deltax = x2 - x1;
    double deltay = y2 - y1;
    double deltaX = X2 - X1;
    double deltaY = Y2 - Y1;
    k = deltay / deltax;
    K = deltaY / deltaX;
    d = y2 - k * x2;
    D = Y2 - K * X2;
    // check if valid
    valid = ((k - K) * (k - K) > NUMERICALSTABLE);
    if (valid) {
      interX = (D - d) / (k - K);
      interY = interX * k + d;
    } else {
      interX = -99999.;
      interY = -99999.;
    }
  }
};

} // end of namespace

#endif

