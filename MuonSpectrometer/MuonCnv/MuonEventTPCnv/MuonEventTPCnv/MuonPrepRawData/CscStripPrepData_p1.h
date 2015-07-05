/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCSTRIPPREPDATA_P1_TRK_H
#define CSCSTRIPPREPDATA_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   CscStripPrepData_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"
#include "Identifier/IdentifierHash.h"

namespace Muon
{
    class CscStripPrepData_p1
    {
    public:
        CscStripPrepData_p1()
         : m_timeOfFirstSample(0.), m_samplingTime(0), m_samplingPhase(0)
         {}
     // base
        TPObjRef                m_prd;
        IdentifierHash          m_collectionHash;
     //const Trk::GlobalPosition *  m_globalPosition
     //const MuonGM::CscReadoutElement *    m_detEl

        std::vector< float >    m_sampleCharges;
        float                   m_timeOfFirstSample;
        unsigned short          m_samplingTime;
        bool                    m_samplingPhase;
    };
}

#endif 
