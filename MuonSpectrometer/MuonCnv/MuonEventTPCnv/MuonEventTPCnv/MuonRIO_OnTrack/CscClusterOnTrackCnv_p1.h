/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSC_CLUSTER_ON_TRACK_CNV_P1_H
#define CSC_CLUSTER_ON_TRACK_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   CscClusterOnTrackCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/CscClusterOnTrack_p1.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/MuonClusterOnTrackCnv_p1.h"

#include "TrkEventTPCnv/TrkRIO_OnTrack/RIO_OnTrackCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
#include "AthLinks/ElementLink.h"
#include "MuonPrepRawData/CscPrepData.h"

class MsgStream;


class CscClusterOnTrackCnv_p1
    : 
    public T_AthenaPoolTPPolyCnvBase<   Trk::MeasurementBase,
                                        Muon::CscClusterOnTrack,
                                        Muon::CscClusterOnTrack_p1 >
{
public:
    CscClusterOnTrackCnv_p1() : m_RIOCnv(0) {}

    void persToTrans(   const Muon :: CscClusterOnTrack_p1  *persObj,
                        Muon :: CscClusterOnTrack           *transObj,
                        MsgStream                           &log );
    void transToPers(   const Muon :: CscClusterOnTrack     *transObj,
                        Muon :: CscClusterOnTrack_p1        *persObj,
                        MsgStream                           &log );

protected:
    RIO_OnTrackCnv_p1*                               m_RIOCnv;
    ElementLinkCnv_p1< ElementLinkToIDC_CSC_Container >     m_elCnv;     
};

#endif // CSC_CLUSTER_ON_TRACK_CNV_P1_H
