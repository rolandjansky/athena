/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#define private public
#define protected public
#include "TrkTrackSummary/MuonTrackSummary.h"
#undef private
#undef protected

#include "TrkEventTPCnv/TrkTrackSummary/MuonTrackSummaryCnv_p2.h"

void MuonTrackSummaryCnv_p2::persToTrans( const Trk::MuonTrackSummary_p2 *persObj,
                                            Trk::MuonTrackSummary    *transObj, MsgStream &/*log*/ )
{
    transObj->m_nscatterers         = persObj->m_nscatterers;
    transObj->m_npseudoMeasurements = persObj->m_npseudoMeasurements;
    
    unsigned int size = persObj->m_isMdt.size();
    transObj->m_chamberHitSummary.resize(size); // fill vector with default instances of ChamberHitSummary
    unsigned int i=0;
    unsigned int j=0;
    for ( ; i<size ; ++i, j+=11 ){
        transObj->m_chamberHitSummary[i].m_isMdt           = persObj->m_isMdt[i];
                                                           
        transObj->m_chamberHitSummary[i].m_chId            = Identifier(persObj->m_data[j]);
        transObj->m_chamberHitSummary[i].first.nhits       = persObj->m_data[j+1];
        transObj->m_chamberHitSummary[i].first.nholes      = persObj->m_data[j+2];
        transObj->m_chamberHitSummary[i].first.noutliers   = persObj->m_data[j+3];
        transObj->m_chamberHitSummary[i].first.ndeltas     = persObj->m_data[j+4];
        transObj->m_chamberHitSummary[i].first.ncloseHits  = persObj->m_data[j+5];

        transObj->m_chamberHitSummary[i].second.nhits      = persObj->m_data[j+6];
        transObj->m_chamberHitSummary[i].second.nholes     = persObj->m_data[j+7];
        transObj->m_chamberHitSummary[i].second.noutliers  = persObj->m_data[j+8];
        transObj->m_chamberHitSummary[i].second.ndeltas    = persObj->m_data[j+9];
        transObj->m_chamberHitSummary[i].second.ncloseHits = persObj->m_data[j+10];
    }
}

void MuonTrackSummaryCnv_p2::transToPers( const Trk::MuonTrackSummary    *transObj,
                                            Trk::MuonTrackSummary_p2 *persObj, MsgStream &/*log*/ )
{
    persObj->m_nscatterers         = transObj->m_nscatterers;
    persObj->m_npseudoMeasurements = transObj->m_npseudoMeasurements;

    unsigned int size = transObj->m_chamberHitSummary.size();
    
    //resize vectors
    persObj->m_data.resize(size*11); 
    persObj->m_isMdt.resize(size,0); 
                                        
    unsigned int i=0;
    unsigned int j=0;
    for ( ; i<size ; ++i,j+=11 ){
      persObj->m_isMdt[i]       = transObj->m_chamberHitSummary[i].m_isMdt  ;

        persObj->m_data[j]   = transObj->m_chamberHitSummary[i].m_chId.get_identifier32().get_compact();   
                        
        persObj->m_data[j+1] = transObj->m_chamberHitSummary[i].first.nhits      ;
        persObj->m_data[j+2] = transObj->m_chamberHitSummary[i].first.nholes     ;
        persObj->m_data[j+3] = transObj->m_chamberHitSummary[i].first.noutliers  ;
        persObj->m_data[j+4] = transObj->m_chamberHitSummary[i].first.ndeltas    ;
        persObj->m_data[j+5] = transObj->m_chamberHitSummary[i].first.ncloseHits ;
                        
        persObj->m_data[j+6] = transObj->m_chamberHitSummary[i].second.nhits      ;
        persObj->m_data[j+7] = transObj->m_chamberHitSummary[i].second.nholes     ;
        persObj->m_data[j+8] = transObj->m_chamberHitSummary[i].second.noutliers  ;
        persObj->m_data[j+9] = transObj->m_chamberHitSummary[i].second.ndeltas    ;
        persObj->m_data[j+10] = transObj->m_chamberHitSummary[i].second.ncloseHits ;        
    }
}
