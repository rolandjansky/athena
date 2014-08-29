/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCPREPDATA_P1_TRK_H
#define TGCPREPDATA_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   TgcPrepData_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"
#include "Identifier/IdentifierHash.h"

namespace Muon
{
   class TgcPrepData_p1
   {
  public:
     TgcPrepData_p1() {}
     // base
     TPObjRef   m_prd;
     /** For transient object this is in MuonCluster, but can simplify structure by placing it here for peristent.*/
     IdentifierHash      m_collectionHash;
   };
}

#endif 
