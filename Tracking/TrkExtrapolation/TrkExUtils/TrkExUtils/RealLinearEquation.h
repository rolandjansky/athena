/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RealLinearEquation.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXUTILS_REALLINEAREQUATION_H
#define TRKEXUTILS_REALLINEAREQUATION_H

#include <utility>
#include <cmath>

namespace Trk {
  
  /** @struct RealLinearEquation
      Mathematic struct for solving real linear equation
   
     <b>Mathematical motivation</b>:<br>
     
     - 2D: The equation is given by:<br>
     @f$ y(x') = k \cdot x' + d  @f$
     
       The constructor is called from a point @f$ p=(p_x,p_y) @f$ that fullfills
       the equation, an incline @f$ k @f$ and a parameter @f$ x' @f$.
     
    @author Andreas.Salzburger@cern.ch   
   */

struct RealLinearEquation
{

  double yOfX;      //!< the result of x
  double segLength; //!< length of the line segment

  RealLinearEquation(double px, double py, double k, double xprime)
  {
    double deltax = xprime - px;
    yOfX = py + k * (deltax);
    double deltay = yOfX - py;
    segLength = sqrt(deltax * deltax + deltay * deltay);
  }
};

} // end of namespace

#endif  

