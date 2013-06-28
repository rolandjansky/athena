/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CylinderIntersector.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVOLUMES_CYLINDERINTERSECTOR_H
#define TRKVOLUMES_CYLINDERINTERSECTOR_H

#include <cmath>

namespace Trk {

  /** @struct CylinderIntersector 
    - identical code to TrkExUtils/RealLinearEquation (but forbidden dependency!)
  */

  struct CylinderIntersector {

  double yOfX;            //!< the result of x
  double segLength;      //!< length of the line segment
   
  CylinderIntersector(double px, double py, double k, double xprime) {
     double deltax = xprime-px;
     yOfX = py + k*(deltax);
     double deltay = yOfX-py;
     segLength = sqrt(deltax*deltax+deltay*deltay);  
  }
  };

} // end of ns

#endif
