/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JacobianCartesianToPolar.h, c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTPRIMITIVES_JACOBIANCARTESIANTOPOLAR_H
#define TRKEVENTPRIMITIVES_JACOBIANCARTESIANTOPOLAR_H

// Amg
#include "EventPrimitives/EventPrimitives.h"
//Trk
#include "TrkEventPrimitives/ParamDefs.h"

namespace Trk {
  
  /** @class JacobianCartesianToPolar
      Jacobian for the transformation of cartesian
      parametes @f$(locX,locY,\phi,\theta, q/p)@f$ to polar parameters @f$ (locR,locPhi,\phi,\theta, q/p)@f$:
      
      - @f$ \frac{\partial r}{\partial locX} = \frac{lx}{\sqrt{lx^2 + ly^2}} @f$ <br>
      - @f$ \frac{\partial r}{\partial loxY} = \frac{ly}{\sqrt{lx^2 + ly^2}} @f$ <br>
      - @f$ \frac{\partial \phi}{\partial locX} = \frac{-ly}{lx^2 + ly^2}@f$ <br>
      - @f$ \frac{\partial \phi}{\partial locY} = \frac{lx}{lx^2 + ly^2} @f$ <br>
      
      @author Andreas.Salzburger@cern.ch     
   */
  class JacobianCartesianToPolar : public AmgMatrix(5,5) {
    public:
      JacobianCartesianToPolar(const double lx, const double ly);
      ~JacobianCartesianToPolar(){}
  };

} // end of namespace

#endif // TRKEVENTPRIMITIVES_JACOBIANCARTESIANTOPOLAR_H


