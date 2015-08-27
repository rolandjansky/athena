/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERIGEE_P1_H
#define PERIGEE_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"

namespace Trk
{
   class Perigee_p1  {
  public:
     Perigee_p1() {}
    
     TPObjRef	m_parameters;   // TrackParameters_p1
     TPObjRef	m_assocSurface; // PerigeeSurface  (Surface_p1)
     TPObjRef	m_localPos;     // localPosition
     
   };
}

#endif 
