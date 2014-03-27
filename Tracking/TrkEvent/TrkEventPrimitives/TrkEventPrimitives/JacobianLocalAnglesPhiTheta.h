/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JacobianLocalAnglesPhiTheta.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTPRIMITIVES_JACOBIANLOCALANGLESPHITHETA_H
#define TRKEVENTPRIMITIVES_JACOBIANLOCALANGLESPHITHETA_H

//Amg
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {
  
  /** @class JacobianLocalAnglesPhiTheta

      This jacobian transforms the local direction representation as given
      in the Trk::LocalDirection class into the corresponding polar and azimutal angle 
      @f$ \phi, \theta @f$, therefor it is of dimension (2x2).

      The Rotation given for construction is the one declared by the Surface frame:

      Surface::transform()::getRotation()

      @author Andreas.Salzburger@cern.ch
      
   */
  class JacobianLocalAnglesPhiTheta : public AmgMatrix(2,2) {
    public:
      JacobianLocalAnglesPhiTheta(const double angleXZ, const double angleYZ, const Amg::RotationMatrix3D& rot);
      ~JacobianLocalAnglesPhiTheta(){}
  };

} // end of namespace

#endif // TRKEVENTPRIMITIVES_JACOBIANLOCALANGLESPHITHETA_H
