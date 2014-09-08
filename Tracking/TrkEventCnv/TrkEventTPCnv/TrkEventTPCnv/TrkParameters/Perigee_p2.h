/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERIGEE_P2_H
#define PERIGEE_P2_H

#include "AthenaPoolUtilities/TPObjRef.h"

namespace Trk
{
   class Perigee_p2  {
  public:
     Perigee_p2() {}
    
     TPObjRef	m_parameters;   // TrackParameters_p1
     TPObjRef	m_assocSurface; // PerigeeSurface  (Surface_p1)
     
   };
}

#endif 
