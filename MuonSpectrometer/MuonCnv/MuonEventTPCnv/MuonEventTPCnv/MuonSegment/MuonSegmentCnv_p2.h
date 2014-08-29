/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTTPCNV_MuonSegmentCnv_p2_H
#define MUONEVENTTPCNV_MuonSegmentCnv_p2_H

//-----------------------------------------------------------------------------
//
// file:   MuonSegmentCnv_p2.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonEventTPCnv/MuonSegment/MuonSegment_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/SurfaceCnv_p2.h"
#include "TrkEventTPCnv/TrkSegment/SegmentCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalDirectionCnv_p1.h"
#include "TrkSurfaces/Surface.h"

class MsgStream;

class MuonSegmentCnv_p2
    : public T_AthenaPoolTPPolyCnvBase< Trk::MeasurementBase,
                    Muon::MuonSegment,
                    Muon::MuonSegment_p1 >
{
public:
    MuonSegmentCnv_p2() : m_segmentCnv(0), m_surfCnv(0) {}

    void persToTrans(   const Muon :: MuonSegment_p1    *persObj,
                        Muon::MuonSegment               *transObj,
                        MsgStream                       &log );
    void transToPers(   const Muon :: MuonSegment       *transObj,
                        Muon::MuonSegment_p1            *persObj,
                        MsgStream                       &log );

protected:
    SegmentCnv_p1*              m_segmentCnv;
    LocalDirectionCnv_p1        m_localDirCnv;
    ITPConverterFor<Trk::Surface>*    m_surfCnv;
};

#endif
