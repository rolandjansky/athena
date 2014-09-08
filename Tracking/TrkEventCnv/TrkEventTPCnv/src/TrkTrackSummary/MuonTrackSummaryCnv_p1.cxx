/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#define private public
#define protected public
#include "TrkTrackSummary/MuonTrackSummary.h"
#undef private
#undef protected

#include "TrkEventTPCnv/TrkTrackSummary/MuonTrackSummaryCnv_p1.h"

void MuonTrackSummaryCnv_p1::persToTrans( const Trk::MuonTrackSummary_p1 *persObj,
                                            Trk::MuonTrackSummary    *transObj, MsgStream &/*log*/ )
{
    transObj->m_nscatterers       = persObj->m_nscatterers;
    transObj->m_npseudoMeasurements      = persObj->m_npseudoMeasurements;
    
    unsigned int size = persObj->m_chId.size();
    transObj->m_chamberHitSummary.resize(size); // fill vector with default instances of ChamberHitSummary
    unsigned int i=0;
    for ( ; i<size ; ++i ){
        transObj->m_chamberHitSummary[i].m_chId           = Identifier(persObj->m_chId[i]);
        transObj->m_chamberHitSummary[i].m_isMdt          = persObj->m_isMdt[i];

        transObj->m_chamberHitSummary[i].first.nhits      = persObj->nPhihits[i];
        transObj->m_chamberHitSummary[i].first.nholes     = persObj->nPhiholes[i];
        transObj->m_chamberHitSummary[i].first.noutliers  = persObj->nPhioutliers[i];
        transObj->m_chamberHitSummary[i].first.ndeltas    = persObj->nPhideltas[i];
        transObj->m_chamberHitSummary[i].first.ncloseHits = persObj->nPhicloseHits[i];

        transObj->m_chamberHitSummary[i].second.nhits      = persObj->nEtahits[i];
        transObj->m_chamberHitSummary[i].second.nholes     = persObj->nEtaholes[i];
        transObj->m_chamberHitSummary[i].second.noutliers  = persObj->nEtaoutliers[i];
        transObj->m_chamberHitSummary[i].second.ndeltas    = persObj->nEtadeltas[i];
        transObj->m_chamberHitSummary[i].second.ncloseHits = persObj->nEtacloseHits[i];
    }
}


void MuonTrackSummaryCnv_p1::transToPers( const Trk::MuonTrackSummary    *transObj,
                                            Trk::MuonTrackSummary_p1 *persObj, MsgStream &/*log*/ )
{
    persObj->m_nscatterers       = transObj->m_nscatterers;
    persObj->m_npseudoMeasurements      = transObj->m_npseudoMeasurements;

    unsigned int size = transObj->m_chamberHitSummary.size();
    
    //resize vectors
    persObj->m_chId.resize(size); 
    persObj->m_isMdt.resize(size,0); 
                                          
    persObj->nEtahits.resize(size); 
    persObj->nEtaholes.resize(size);  
    persObj->nEtaoutliers.resize(size); 
    persObj->nEtadeltas.resize(size); 
    persObj->nEtacloseHits.resize(size);
             
    persObj->nPhihits.resize(size); 
    persObj->nPhiholes.resize(size);  
    persObj->nPhioutliers.resize(size); 
    persObj->nPhideltas.resize(size); 
    persObj->nPhicloseHits.resize(size);
    
    unsigned int i=0;
    for ( ; i<size ; ++i ){
        persObj->m_chId[i]        = transObj->m_chamberHitSummary[i].m_chId.get_identifier32().get_compact();   
        persObj->m_isMdt[i]       = transObj->m_chamberHitSummary[i].m_isMdt  ;
        
        persObj->nPhihits[i]      = transObj->m_chamberHitSummary[i].first.nhits      ;
        persObj->nPhiholes[i]     = transObj->m_chamberHitSummary[i].first.nholes     ;
        persObj->nPhioutliers[i]  = transObj->m_chamberHitSummary[i].first.noutliers  ;
        persObj->nPhideltas[i]    = transObj->m_chamberHitSummary[i].first.ndeltas    ;
        persObj->nPhicloseHits[i] = transObj->m_chamberHitSummary[i].first.ncloseHits ;

        persObj->nEtahits[i]      = transObj->m_chamberHitSummary[i].second.nhits      ;
        persObj->nEtaholes[i]     = transObj->m_chamberHitSummary[i].second.nholes     ;
        persObj->nEtaoutliers[i]  = transObj->m_chamberHitSummary[i].second.noutliers  ;
        persObj->nEtadeltas[i]    = transObj->m_chamberHitSummary[i].second.ndeltas    ;
        persObj->nEtacloseHits[i] = transObj->m_chamberHitSummary[i].second.ncloseHits ;        
    }
}
