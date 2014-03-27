/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JacobianPxyzToPhiThetaQoverPspherical.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTPRIMITIVES_JACOBIANPXYZTOPHITHETAQOVERPSPHERICAL_H
#define TRKEVENTPRIMITIVES_JACOBIANPXYZTOPHITHETAQOVERPSPHERICAL_H

// Amg
#include "EventPrimitives/EventPrimitives.h"
//Trk
#include "TrkEventPrimitives/ParamDefs.h"

namespace Trk {
  
  /** @class JacobianPxyzToPhiThetaQoverPspherical
      @brief Jacobian in spherical coordinates for the transformation
       of momentum in Cartesian coordinates @f$ (px,py,pz,charge)@f$
       to momentum in spherical cordinates  @f$(\phi,\theta,QoverP)@f$.

      The derivatives are: 
      - @f$ \frac{\partial \phi}{\partial px}   = -\frac{fabs(qOverP) \cdot sin\phi}{sin\theta}  @f$ <br>
      - @f$ \frac{\partial \theta}{\partial px} =  fabs(qOverP)\cdot cos\phi \cdot cos \theta @f$ <br>
      - @f$ \frac{\partial qOverP}{\partial px} =  -frac{fabs(qOverP)\mathrm{^3} \cdot cos\phi \cdot sin\theta}{qOverP} @f$ <br>
      - @f$ \frac{\partial \phi}{\partial py}   = \frac{fabs(qOverP) \cdot cos\phi}{sin\theta} @f$ <br>
      - @f$ \frac{\partial \theta}{\partial py} =  fabs(qOverP)\cdot sin\phi \cdot cos\theta @f$ <br>
      - @f$ \frac{\partial QoverP}{\partial py} =  -frac{fabs(qOverP)\mathrm{^3} \cdot sin\phi \cdot sin\theta}{qOverP} @f$ <br>
      - @f$ \frac{\partial \phi}{\partial pz}   = 0 @f$ <br>
      - @f$ \frac{\partial \theta}{\partial pz} = fabs(qOverP)\cdot sin\theta @f$ <br>
      - @f$ \frac{\partial pz}{\partial qOverP} = -frac{fabs(qOverP)\mathrm{^3} \cdot cos\theta}{qOverP} @f$ <br>

      @author Tatjana.Lenz@cern.ch
      
   */
  class JacobianPxyzToPhiThetaQoverPspherical : public AmgMatrix(3,3) {
    public:
      JacobianPxyzToPhiThetaQoverPspherical(const double phi, const double theta, const double qOverP);
      ~JacobianPxyzToPhiThetaQoverPspherical(){}
  };

} // end of namespace

#endif // TRKEVENTPRIMITIVES_JACOBIANPXYZTOPHITHETAQOVERPSPHERICAL_H


