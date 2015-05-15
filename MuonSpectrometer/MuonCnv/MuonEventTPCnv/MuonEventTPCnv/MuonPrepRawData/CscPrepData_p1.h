/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCPREPDATA_P1_TRK_H
#define CSCPREPDATA_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   CscPrepData_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"
#include "Identifier/IdentifierHash.h"

namespace Muon
{
    class CscPrepData_p1
    {
    public:
        CscPrepData_p1()
         : m_charge(0), m_time(0.), m_status(0)
         {}
     // base
        TPObjRef            m_prd;
        
        /** For transient object this is in MuonCluster, but can simplify structure by placing it here for peristent.*/
        IdentifierHash      m_collectionHash;
        
        int                 m_charge;
        float               m_time;
        int                 m_status; //!< in here we pack both the transient values m_status and m_timeStatus (introduced in MuonPrepRawData-03-11-00)
    };
}

#endif 
