/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// $Id: T2BeamSpot.cxx 361213 2011-04-23 18:26:44Z bartoldu $
//

#include "T2BeamSpot.h"

using std::ostream;

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
