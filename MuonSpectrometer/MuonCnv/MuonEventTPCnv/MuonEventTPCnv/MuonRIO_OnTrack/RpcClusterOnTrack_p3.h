/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPC_CLUSTER_ON_TRACK_p3_TRK_H
#define RPC_CLUSTER_ON_TRACK_p3_TRK_H

//-----------------------------------------------------------------------------
//
// file:   RpcClusterOnTrack_p3.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"
#include "DataModelAthenaPool/ElementLink_p3.h"


namespace Muon
{
    /** Version 1 of the persistent class representing the transient class Muon::RpcClusterOnTrack */
   class RpcClusterOnTrack_p3
   {
  public:
     RpcClusterOnTrack_p3() : m_id(0),m_positionAlongStrip(0.0),m_time(0.0){}

      /** base RIO_OnTrack_p1 info*/
      Identifier32::value_type        m_id;
      TPObjRef                      m_localParams;
      TPObjRef                      m_localErrMat;
           
      /** Link to PRD */
      ElementLinkInt_p3  m_prdLink;
      
      /** position along strip (double in transient world)*/
      float m_positionAlongStrip;

      /** Time in ns*/
      float m_time;
   };
}

#endif 
