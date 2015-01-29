/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JacobianCartesianToPerigee.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTPRIMITIVES_JACOBIANCARTESIANTOPERIGEE_H
#define TRKEVENTPRIMITIVES_JACOBIANCARTESIANTOPERIGEE_H

//Amg
#include "EventPrimitives/EventPrimitives.h"
//Trk
#include "TrkEventPrimitives/ParamDefs.h"

namespace Trk {
  
  /** @class JacobianCartesianToPerigee
      @brief Jacobian from cartesian parameters 
       @f$ (p_x, p_y, p_z, E, x, y, z) @f$ to perigee parameters
       @f$ (d_0, z_0, \phi_0, \theta, q/p) @f$.

      It needs the particle charge to provide the correct
      perigee @f$ q/p @f$ sign.
      @author Maaike.Limper@cern.ch 
  */
  class JacobianCartesianToPerigee : public AmgMatrix(5,7) {
    public:
    JacobianCartesianToPerigee(const double px, const double py, const double pz, const double E, const double x, const double y, const double z, const double charge );
  };

} // end of namespace

#endif // TRKEVENTPRIMITIVES_JACOBIANCARTESIANTOPERIGEE_H

