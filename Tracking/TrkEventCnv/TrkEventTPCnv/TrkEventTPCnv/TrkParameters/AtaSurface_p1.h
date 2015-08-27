/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATA_AtaSurface_p1_TRK_H
#define ATA_AtaSurface_p1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   AtaSurface_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"

namespace Trk
{
   class AtaSurface_p1  {
  public:
     AtaSurface_p1() {}

     // TrackParameters_p1
     TPObjRef	m_parameters;
          // DiscSurface_p1
     TPObjRef	m_assocSurface;
   };
}

#endif 
