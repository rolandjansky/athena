/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPC_CLUSTER_ON_TRACK_p2_TRK_H
#define RPC_CLUSTER_ON_TRACK_p2_TRK_H

//-----------------------------------------------------------------------------
//
// file:   RpcClusterOnTrack_p2.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"
#include "DataModelAthenaPool/ElementLink_p3.h"


namespace Muon
{
    /** Version 1 of the persistent class representing the transient class Muon::RpcClusterOnTrack */
   class RpcClusterOnTrack_p2
   {
  public:
     RpcClusterOnTrack_p2() : m_positionAlongStrip(0.0){}

     // base SiClusterOnTrack_p2, inheriting from RIO_OnTrack_p2
     TPObjRef                       m_RIO;
     
      /** Link to PRD */
      ElementLinkInt_p3  m_prdLink;
      
      /** position along strip (double in transient world)*/
      float m_positionAlongStrip;
   };
}

#endif 
