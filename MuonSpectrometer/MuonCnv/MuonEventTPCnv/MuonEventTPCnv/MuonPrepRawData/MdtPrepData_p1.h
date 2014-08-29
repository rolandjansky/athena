/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTPREPDATA_P1_TRK_H
#define MDTPREPDATA_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   MdtPrepData_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"
#include "Identifier/IdentifierHash.h"

namespace Muon
{
    class MdtPrepData_p1
    {
    public:
        MdtPrepData_p1() : m_tdc(0), m_adc(0), m_status(0)  {}
     // base
        TPObjRef           m_prd;

        int                m_tdc;
        int                m_adc;
        int                m_status;
        IdentifierHash     m_collectionHash;
    };
}

#endif // PIXEL_CLUSTER_P1_TRK_H
