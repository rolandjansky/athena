/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JacobianPerigeeToCartesian.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTPRIMITIVES_JACOBIANPERIGEETOCARTESIAN_H
#define TRKEVENTPRIMITIVES_JACOBIANPERIGEETOCARTESIAN_H

//Amg
#include "EventPrimitives/EventPrimitives.h"
//Trk
#include "TrkEventPrimitives/ParamDefs.h"

namespace Trk {
  
  /** @class JacobianPerigeeToCartesian
      @brief Jacobian for transforming perigee track parameters
      @f$(d_0, z_0, \phi_0, \theta, q/p)@f$ to
      the cartesian frame @f$(p_x,p_y,p_z,E,x,y,z)@f$.

      It needs a mass in order to calculate the energy, default is
      muon-mass.
      @author Maaike.Limper@cern.ch 
  */
  class JacobianPerigeeToCartesian : public AmgMatrix(7,5) {
    public:
    JacobianPerigeeToCartesian(const double d0, const double z0, const double phi0, const double theta, const double qOverP, const double mass = 105.6583692  );
      ~JacobianPerigeeToCartesian(){}
  };

} // end of namespace

#endif // TRKEVENTPRIMITIVES_JACOBIANPERIGEETOCARTESIAN_H

