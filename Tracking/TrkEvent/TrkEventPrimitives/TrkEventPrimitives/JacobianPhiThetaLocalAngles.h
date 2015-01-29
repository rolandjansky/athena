/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JacobianPhiThetaLocalAngles.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTPRIMITIVES_JACOBIANPHITHETALOCALANGLES_H
#define TRKEVENTPRIMITIVES_JACOBIANPHITHETALOCALANGLES_H

// Amg
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {
  
  /** @class JacobianPhiThetaLocalAngles

      This jacobian transforms the polar and azimutal angle 
      @f$ \phi, \theta @f$ of a global momentum representation 
      to a local momentum representation as given
      in the Trk::LocalMomentum class, therefor it is of dimension (2x2).

      The Rotation given for construction is the one declared by the Surface frame:

      Surface::transform()::getRotation()

      @author Andreas.Salzburger@cern.ch
      
   */
  class JacobianPhiThetaLocalAngles : public AmgMatrix(2,2) {
    public:
      JacobianPhiThetaLocalAngles(const Amg::RotationMatrix3D& rot);
      JacobianPhiThetaLocalAngles (const double phi, const double theta, const Amg::RotationMatrix3D& rot);
      ~JacobianPhiThetaLocalAngles(){}
  };

} // end of namespace

#endif // TRKEVENTPRIMITIVES_JACOBIANPHITHETALOCALANGLES_H
