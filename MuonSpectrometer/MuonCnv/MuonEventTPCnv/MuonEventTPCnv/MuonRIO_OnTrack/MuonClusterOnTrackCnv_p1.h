/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_CLUSTER_ON_TRACK_CNV_P1_H
#define MUON_CLUSTER_ON_TRACK_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   MuonClusterOnTrackCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/MuonClusterOnTrack_p1.h"

#include "TrkEventTPCnv/TrkRIO_OnTrack/RIO_OnTrackCnv_p1.h"

class MsgStream;


/** NO LONGER NEEDED, except for backwards compatibility.*/
class MuonClusterOnTrackCnv_p1
    : public T_AthenaPoolTPAbstractPolyCnvBase< Trk::MeasurementBase,
                                        Muon::MuonClusterOnTrack,
                                        Muon::MuonClusterOnTrack_p1 >
{
public:
    MuonClusterOnTrackCnv_p1() : m_RIOCnv(0) {}

    void persToTrans(   const Muon :: MuonClusterOnTrack_p1 *persObj,
                        Muon :: MuonClusterOnTrack          *transObj,
                        MsgStream                           &log );
    void transToPers(   const Muon :: MuonClusterOnTrack    *transObj,
                        Muon :: MuonClusterOnTrack_p1       *persObj,
                        MsgStream                           &log );

protected:
    RIO_OnTrackCnv_p1           *m_RIOCnv;
};

#endif // MUON_CLUSTER_ON_TRACK_CNV_P1_H
