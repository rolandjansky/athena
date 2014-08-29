/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCPREPDATA_P2_TRK_H
#define RPCPREPDATA_P2_TRK_H

//-----------------------------------------------------------------------------
//
// file:   RpcPrepData_p2.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"
#include "Identifier/IdentifierHash.h"

namespace Muon
{
   class RpcPrepData_p2
   {
  public:
     RpcPrepData_p2() : m_collectionHash(), m_time(0.0), m_triggerInfo(0), m_ambiguityFlag(0) {}
     // base
     TPObjRef   m_prd;

    /** For transient object this is in MuonCluster, but can simplify structure by placing it here for peristent.*/
     IdentifierHash      m_collectionHash;

     float      m_time;
     int        m_triggerInfo;
     int        m_ambiguityFlag;
   };
}

#endif 
