/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JacobianPolarToCartesian.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTPRIMITIVES_JACOBIANPOLARTOCARTESIAN_H
#define TRKEVENTPRIMITIVES_JACOBIANPOLARTOCARTESIAN_H

// Geo & Maths
#include "EventPrimitives/EventPrimitives.h"
//Trk
#include "TrkEventPrimitives/ParamDefs.h"

namespace Trk {
  
  /** @class JacobianPolarToCartesian
      Jacobian for the transformation of polar parameters @f$ (locR,locPhi,\phi,\theta, q/p)@f$ to cartesian
      parametes @f$(locX,locY,\phi,\theta, q/p)@f$ :
      
      - @f$ \frac{\partial locX}{\partial r} = cos\phi @f$ <br>
      - @f$ \frac{\partial locX}{\partial \phi} = -r\cot sin\phi @f$ <br>
      - @f$ \frac{\partial locY}{\partial r} = sin\phi @f$ <br>
      - @f$ \frac{\partial locY}{\partial \phi} = r\cdot cos\phi @f$ <br>
      
      @author Andreas.Salzburger@cern.ch
      
   */
  class JacobianPolarToCartesian : public AmgMatrix(5,5) {
    public:
      JacobianPolarToCartesian(const double r, const double phi);
      ~JacobianPolarToCartesian(){}
  };

} // end of namespace

#endif // TRKEVENTPRIMITIVES_JACOBIANPOLARTOCARTESIAN_H


