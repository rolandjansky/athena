/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSEGMENTQUALITYCNV_P1_H
#define MUONSEGMENTQUALITYCNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   MuonSegmentQualityCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "MuonSegment/MuonSegmentQuality.h"
#include "MuonEventTPCnv/MuonSegment/MuonSegmentQuality_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrkEventTPCnv/TrkEventPrimitives/FitQualityCnv_p1.h"

class MsgStream;


class MuonSegmentQualityCnv_p1
    : public T_AthenaPoolTPPolyCnvBase<  Trk::FitQuality, Muon::MuonSegmentQuality, Muon::MuonSegmentQuality_p1 >
{
public:
    MuonSegmentQualityCnv_p1() : m_baseCnv(0)  {}

    void persToTrans( const Muon :: MuonSegmentQuality_p1 *persObj,
        Muon :: MuonSegmentQuality    *transObj,
        MsgStream                       &log );
    void transToPers( const Muon :: MuonSegmentQuality    *transObj,
        Muon :: MuonSegmentQuality_p1 *persObj,
        MsgStream                       &log );

protected:
    FitQualityCnv_p1	*m_baseCnv;
};

#endif 

