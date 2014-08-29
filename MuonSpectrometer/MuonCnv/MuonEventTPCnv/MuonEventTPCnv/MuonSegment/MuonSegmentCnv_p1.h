/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTTPCNV_MUONSEGMENTCNV_P1_H
#define MUONEVENTTPCNV_MUONSEGMENTCNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   MuonSegmentCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "MuonSegment/MuonSegment.h"
#include "MuonEventTPCnv/MuonSegment/MuonSegment_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/SurfaceCnv_p1.h"
#include "TrkEventTPCnv/TrkSegment/SegmentCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalDirectionCnv_p1.h"

class MsgStream;

class MuonSegmentCnv_p1
    : public T_AthenaPoolTPPolyCnvBase< Trk::MeasurementBase,
                    Muon::MuonSegment,
                    Muon::MuonSegment_p1 >
{
public:
    MuonSegmentCnv_p1() : m_segmentCnv(0) {}

    void persToTrans(   const Muon :: MuonSegment_p1    *persObj,
                        Muon::MuonSegment               *transObj,
                        MsgStream                       &log );
    void transToPers(   const Muon :: MuonSegment       *transObj,
                        Muon::MuonSegment_p1            *persObj,
                        MsgStream                       &log );

protected:
    SegmentCnv_p1*              m_segmentCnv;
    LocalDirectionCnv_p1        m_localDirCnv;
};

#endif
