/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JacobianPhiThetaQoverPToPxyz.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTPRIMITIVES_JACOBIANPHITHETAQOVERPTOPXYZ_H
#define TRKEVENTPRIMITIVES_JACOBIANPHITHETAQOVERPTOPXYZ_H

//Amg
#include "EventPrimitives/EventPrimitives.h"
//Trk
#include "TrkEventPrimitives/ParamDefs.h"

namespace Trk {
  
  /** @class JacobianPhiThetaQoverPToPxyz
      Jacobian for the transformation of momentum in polar coordinates @f$ (\phi,\theta,qOverP)@f$ to momentum in Cartesian coordinates  @f$(px, py, pz)@f$ :
      
      - @f$ \frac{\partial px}{\partial \phi}   = -\frac{sin \phi \cdot sin \theta}{|qOverP|}  @f$ <br>
      - @f$ \frac{\partial px}{\partial \theta} = \frac{cos \phi \cdot cos \theta}{|qOverP|}  @f$ <br>
      - @f$ \frac{\partial px}{\partial qOverP} = -(+)\frac{cos \phi \cdot sin \theta}{qOverP \mathrm{^2}} @f$ <br>
	the sign depends on the charge sign (if charge > 0 sign = " - ")
      - @f$ \frac{\partial py}{\partial \phi}   = \frac{cos \phi \cdot sin \theta}{|qOverP|} @f$ <br>
      - @f$ \frac{\partial py}{\partial \theta} = \frac{sin \phi \cdot cos \theta}{|qOverP|} @f$ <br>
      - @f$ \frac{\partial py}{\partial qOverP} = -(+)\frac{sin \phi \cdot sin \theta}{qOverP \mathrm{^2}} @f$ <br>
      - @f$ \frac{\partial pz}{\partial \phi}   = 0 @f$ <br>
      - @f$ \frac{\partial pz}{\partial \theta} = -\frac{sin \theta}{|qOverP|} @f$ <br>
      - @f$ \frac{\partial pz}{\partial qOverP} = -(+)\frac{cos \theta}{qOverP \mathrm{^2}} @f$ <br>

      @author Tatjana.Lenz@cern.ch
      
   */
  class JacobianPhiThetaQoverPToPxyz : public AmgMatrix(3,3) {
    public:
      JacobianPhiThetaQoverPToPxyz(const double phi, const double theta, const double QoverP);
      ~JacobianPhiThetaQoverPToPxyz(){}
  };

} // end of namespace

#endif // TRKEVENTPRIMITIVES_JACOBIANPHITHETAQOVERPTOPXYZ_H


