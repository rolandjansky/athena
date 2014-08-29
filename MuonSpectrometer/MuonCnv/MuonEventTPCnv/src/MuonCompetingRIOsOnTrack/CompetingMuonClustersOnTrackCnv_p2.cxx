/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MuonCompetingRIOsOnTrackCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#undef private
#undef protected
#include "MuonEventTPCnv/MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrackCnv_p2.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"

void 
CompetingMuonClustersOnTrackCnv_p2::persToTrans( const Muon::CompetingMuonClustersOnTrack_p1 *persObj,
                                                       Muon::CompetingMuonClustersOnTrack *transObj, 
                                                       MsgStream &log )
{
   fillTransFromPStore( &m_cRotCnv, persObj->m_competingROT, transObj, log );
   
   std::vector< TPObjRef >::const_iterator  it = persObj->m_containedChildRots.begin(), 
                                            itE = persObj->m_containedChildRots.end();
                                            
   transObj->m_containedChildRots = new std::vector< const Muon::MuonClusterOnTrack * >;
   
   for (; it!=itE;it++) {
       ITPConverterFor<Trk::MeasurementBase>  *rotCnv = 0;
       const Muon::MuonClusterOnTrack* mcot = dynamic_cast<const Muon::MuonClusterOnTrack*>(createTransFromPStore(&rotCnv, *it, log));
       transObj->m_containedChildRots->push_back( mcot );
   }
   
   transObj->m_associatedSurface = this->createTransFromPStore( &m_surfCnv, persObj->m_associatedSurface, log );
}

void 
CompetingMuonClustersOnTrackCnv_p2::transToPers( const Muon::CompetingMuonClustersOnTrack *transObj,
                                                        Muon::CompetingMuonClustersOnTrack_p1 *persObj, 
                                                        MsgStream &log ) 
{
    persObj->m_competingROT = baseToPersistent( &m_cRotCnv,  transObj, log );

    std::vector< const Muon::MuonClusterOnTrack * >::const_iterator it  = transObj->m_containedChildRots->begin(), 
                                                                    itE = transObj->m_containedChildRots->end();
    //transObj->m_containedChildRots = new std::vector< const MuonClusterOnTrack * >;
    
    for (; it!=itE;it++) {
        ITPConverterFor<Trk::MeasurementBase>  *rotCnv = 0;
        persObj->m_containedChildRots.push_back( toPersistent(&rotCnv, *it, log) );
    }
    
    persObj->m_associatedSurface     = toPersistent(&m_surfCnv, transObj->m_associatedSurface, log);
}


