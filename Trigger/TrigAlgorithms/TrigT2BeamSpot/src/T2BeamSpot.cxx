/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// $Id: T2BeamSpot.cxx 744847 2016-05-04 15:12:00Z smh $
//

#include "T2BeamSpot.h"

namespace PESA
{

  std::ostream& operator<<( std::ostream& os, const T2BeamSpot& beamSpot )
  {
    os
      << " status = "   << beamSpot.status()
      << ", x0 = "      << beamSpot.posX() 
      << ", y0 = "      << beamSpot.posY() 
      << ", z0 = "      << beamSpot.posZ() 
      << ", sigmaX = "  << beamSpot.sigmaX()
      << ", sigmaY = "  << beamSpot.sigmaY() 
      << ", sigmaZ = "  << beamSpot.sigmaZ()
      << ", tiltX = "   << beamSpot.tiltX() 
      << ", tiltY = "   << beamSpot.tiltY()
      << ", sigmaXY = " << beamSpot.sigmaXY()
      ;
    return os;
  }

}
