/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTTPCNV_COMPETINGMUONCLUSTERSONTRACKCNV_P2_H
#define MUONEVENTTPCNV_COMPETINGMUONCLUSTERSONTRACKCNV_P2_H

//-----------------------------------------------------------------------------
//
// file:   CompetingMuonClustersOnTrack_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkEventTPCnv/TrkSurfaces/SurfaceCnv_p2.h"
#include "TrkEventTPCnv/TrkRIO_OnTrack/RIO_OnTrackCnv_p1.h"
#include "TrkEventTPCnv/TrkCompetingRIOsOnTrack/CompetingRIOsOnTrackCnv_p1.h"

#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "MuonEventTPCnv/MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack_p1.h"
#include "TrkSurfaces/Surface.h"

class MsgStream;

class CompetingMuonClustersOnTrackCnv_p2
    : public T_AthenaPoolTPPolyCnvBase< Trk::MeasurementBase,
    Muon::CompetingMuonClustersOnTrack,
    Muon::CompetingMuonClustersOnTrack_p1 >
{
public:
    CompetingMuonClustersOnTrackCnv_p2() : m_cRotCnv(0), m_localParCnv(0), m_errorMxCnv(0), m_surfCnv(0) {}
    
    virtual void persToTrans( const Muon::CompetingMuonClustersOnTrack_p1   *persObj,
                                    Muon::CompetingMuonClustersOnTrack      *transObj,
                                    MsgStream                               &log );
                                    
    virtual void transToPers( const Muon::CompetingMuonClustersOnTrack      *transObj,
                                    Muon::CompetingMuonClustersOnTrack_p1   *persObj,
                                    MsgStream                               &log );

protected:
    CompetingRIOsOnTrackCnv_p1* m_cRotCnv;
    LocalParametersCnv_p1*      m_localParCnv;
    ErrorMatrixCnv_p1*          m_errorMxCnv;
    ITPConverterFor<Trk::Surface>*    m_surfCnv;
};

#endif 

