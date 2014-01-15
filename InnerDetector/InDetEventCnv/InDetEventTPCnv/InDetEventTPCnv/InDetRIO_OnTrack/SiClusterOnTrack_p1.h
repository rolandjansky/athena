/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SI_CLUSTER_ON_TRACK_P1_TRK_H
#define SI_CLUSTER_ON_TRACK_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   SiClusterOnTrack_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolUtilities/TPObjRef.h"

#include "Identifier/IdentifierHash.h"
#include <vector>

namespace InDet
{
   class SiClusterOnTrack_p1    {
  public:
     SiClusterOnTrack_p1():m_isbroad(false) {}

     // base RIO_OnTrack_p1
     TPObjRef                       m_RIO;
     IdentifierHash::value_type     m_idDE;
     bool                           m_isbroad;
   };
}

#endif // SI_CLUSTER_ON_TRACK_P1_TRK_H
