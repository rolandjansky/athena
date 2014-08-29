/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPC_CLUSTER_ON_TRACK_P1_TRK_H
#define RPC_CLUSTER_ON_TRACK_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   RpcClusterOnTrack_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"
#include "DataModelAthenaPool/ElementLink_p1.h"


namespace Muon
{
    /** Version 1 of the persistent class representing the transient class Muon::RpcClusterOnTrack */
   class RpcClusterOnTrack_p1
   {
  public:
     RpcClusterOnTrack_p1():m_positionAlongStrip(0.0) {}

     // base SiClusterOnTrack_p1, inheriting from RIO_OnTrack_p1
     TPObjRef                       m_RIO;
     
      /** Link to PRD */
      ElementLinkInt_p1  m_prdLink;
      
      /** position along strip (double in transient world)*/
      float m_positionAlongStrip;
   };
}

#endif 
