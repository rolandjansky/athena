/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MuonCompetingRIOsOnTrackCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "MuonEventTPCnv/MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrackCnv_p2.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"

void 
CompetingMuonClustersOnTrackCnv_p2::persToTrans( const Muon::CompetingMuonClustersOnTrack_p1 *persObj,
                                                       Muon::CompetingMuonClustersOnTrack *transObj, 
                                                       MsgStream &log )
{
  auto containedChildRots = std::make_unique<std::vector< const Muon::MuonClusterOnTrack * > >();

  for (const TPObjRef& ref : persObj->m_containedChildRots) {
    ITPConverterFor<Trk::MeasurementBase>  *rotCnv = 0;
    const Muon::MuonClusterOnTrack* mcot = dynamic_cast<const Muon::MuonClusterOnTrack*>(createTransFromPStore(&rotCnv, ref, log));
    containedChildRots->push_back( mcot );
  }
   
  Trk::Surface* associatedSurface = this->createTransFromPStore( &m_surfCnv, persObj->m_associatedSurface, log );

  *transObj = Muon::CompetingMuonClustersOnTrack (Trk::LocalParameters(),
                                                  Amg::MatrixX(),
                                                  associatedSurface,
                                                  containedChildRots.release(),
                                                  nullptr // assgnProb
                                                  );

   fillTransFromPStore( &m_cRotCnv, persObj->m_competingROT, transObj, log );
}

void 
CompetingMuonClustersOnTrackCnv_p2::transToPers( const Muon::CompetingMuonClustersOnTrack *transObj,
                                                        Muon::CompetingMuonClustersOnTrack_p1 *persObj, 
                                                        MsgStream &log ) 
{
  persObj->m_competingROT = baseToPersistent( &m_cRotCnv,  transObj, log );

  for (const Muon::MuonClusterOnTrack* p : transObj->containedROTs()) {
    ITPConverterFor<Trk::MeasurementBase>  *rotCnv = 0;
    persObj->m_containedChildRots.push_back( toPersistent(&rotCnv, p, log) );
  }
    
  persObj->m_associatedSurface     = toPersistent(&m_surfCnv, transObj->associatedSurfaceRaw(), log);
}


