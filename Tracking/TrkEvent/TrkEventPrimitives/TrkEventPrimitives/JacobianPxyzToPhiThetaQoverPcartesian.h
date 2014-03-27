/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JacobianPxyzToPhiThetaQoverPcartesian.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTPRIMITIVES_JACOBIANPXYZTOPHITHETAQOVERPCARTESIAN_H
#define TRKEVENTPRIMITIVES_JACOBIANPXYZTOPHITHETAQOVERPCARTESIAN_H

//Amg
#include "EventPrimitives/EventPrimitives.h"
//Trk
#include "TrkEventPrimitives/ParamDefs.h"

namespace Trk {
  
  /** @class JacobianPxyzToPhiThetaQoverPcartesian
      @brief Jacobian in cartesian coordinates for the transformation
       of momentum in Cartesian coordinates @f$ (px,py,pz,charge)@f$
       to momentum in polar cordinates  @f$(\phi,\theta,QoverP)@f$.

      The derivatives are:
      - @f$ r = \sqrt{px\mathrm{^2}+py\mathrm{^2}+pz\mathrm{^2}} 
      - @f$ \frac{\partial \phi}{\partial px}   = -\frac{py}{px\mathrm{^2}+py\mathrm{^2}}  @f$
      - @f$ \frac{\partial \theta}{\partial px} = \frac{px\cdot pz}{r\mathrm{^2}\cdot \sqrt{px\mathrm{^2}+py\mathrm{^2}}} @f$
      - @f$ \frac{\partial qOverP}{\partial px} = -\frac{charge \cdot px}{r\mathrm{^3}} @f$
      - @f$ \frac{\partial \phi}{\partial py}   = -\frac{px}{px\mathrm{^2}+py\mathrm{^2}} @f$
      - @f$ \frac{\partial \theta}{\partial py} =  \frac{py\cdot pz}{r\mathrm{^2}\cdot \sqrt{px\mathrm{^2}+py\mathrm{^2}}}@f$
      - @f$ \frac{\partial QoverP}{\partial py} = -\frac{charge \cdot py}{r\mathrm{^3}} @f$
      - @f$ \frac{\partial \phi}{\partial pz}   = 0 @f$
      - @f$ \frac{\partial \theta}{\partial pz} = \frac{px\mathrm{^2}+py\mathrm{^2}}{r\mathrm{^2} \cdot \sqrt{px\mathrm{^2}+py\mathrm{^2}}} @f$
      - @f$ \frac{\partial pz}{\partial qOverP} = \frac{charge \cdot pz}{r\mathrm{^3}} @f$

      @author Tatjana.Lenz@cern.ch
      
   */
  class JacobianPxyzToPhiThetaQoverPcartesian : public AmgMatrix(3,3) {
    public:
      JacobianPxyzToPhiThetaQoverPcartesian(const double px, const double py, const double pz, double charge);
      ~JacobianPxyzToPhiThetaQoverPcartesian(){}
  };

} // end of namespace

#endif // TRKEVENTPRIMITIVES_JACOBIANPXYZTOPHITHETAQOVERPCARTESIAN_H


