/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGC_CLUSTER_ON_TRACK_CNV_P1_H
#define TGC_CLUSTER_ON_TRACK_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   TgcClusterOnTrackCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "MuonRIO_OnTrack/TgcClusterOnTrack.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/TgcClusterOnTrack_p1.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/MuonClusterOnTrackCnv_p1.h"

#include "TrkEventTPCnv/TrkRIO_OnTrack/RIO_OnTrackCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
#include "AthLinks/ElementLink.h"
#include "MuonPrepRawData/TgcPrepData.h"

class MsgStream;


class TgcClusterOnTrackCnv_p1
    : 
    public T_AthenaPoolTPPolyCnvBase<   Trk::MeasurementBase,
                                        Muon::TgcClusterOnTrack,
                                        Muon::TgcClusterOnTrack_p1 >
{
public:
    TgcClusterOnTrackCnv_p1() : m_RIOCnv(0) {}

    void persToTrans( const Muon :: TgcClusterOnTrack_p1 *persObj,
        Muon :: TgcClusterOnTrack    *transObj,
        MsgStream                       &log );
    void transToPers( const Muon :: TgcClusterOnTrack    *transObj,
        Muon :: TgcClusterOnTrack_p1 *persObj,
        MsgStream                       &log );

protected:
    RIO_OnTrackCnv_p1*                               m_RIOCnv;
    ElementLinkCnv_p1< ElementLinkToIDC_TGC_Container >     m_elCnv;     
};

#endif // TGC_CLUSTER_ON_TRACK_CNV_P1_H
