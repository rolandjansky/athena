/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrackSummary/MuonTrackSummary.h"
#include "TrkEventTPCnv/TrkTrackSummary/MuonTrackSummaryCnv_p2.h"

void MuonTrackSummaryCnv_p2::persToTrans( const Trk::MuonTrackSummary_p2 *persObj,
                                            Trk::MuonTrackSummary    *transObj, MsgStream &/*log*/ )
{
    transObj->m_nscatterers         = persObj->m_nscatterers;
    transObj->m_npseudoMeasurements = persObj->m_npseudoMeasurements;
    
    unsigned int size = persObj->m_isMdt.size();
    transObj->m_chamberHitSummary.clear();
    transObj->m_chamberHitSummary.reserve(size);
    unsigned int i=0;
    unsigned int j=0;
    for ( ; i<size ; ++i, j+=11 ){
        transObj->m_chamberHitSummary.emplace_back(Identifier(persObj->m_data[j]),persObj->m_isMdt[i]);
        Trk::MuonTrackSummary::ChamberHitSummary& chamb_summary = transObj->m_chamberHitSummary.back();
   
        chamb_summary.m_first.nhits       = persObj->m_data[j+1];
        chamb_summary.m_first.nholes      = persObj->m_data[j+2];
        chamb_summary.m_first.noutliers   = persObj->m_data[j+3];
        chamb_summary.m_first.ndeltas     = persObj->m_data[j+4];
        chamb_summary.m_first.ncloseHits  = persObj->m_data[j+5];

        chamb_summary.m_second.nhits      = persObj->m_data[j+6];
        chamb_summary.m_second.nholes     = persObj->m_data[j+7];
        chamb_summary.m_second.noutliers  = persObj->m_data[j+8];
        chamb_summary.m_second.ndeltas    = persObj->m_data[j+9];
        chamb_summary.m_second.ncloseHits = persObj->m_data[j+10];
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
    for (const Trk::MuonTrackSummary::ChamberHitSummary& s :
           transObj->chamberHitSummary())
    {
        persObj->m_isMdt[i]  = s.isMdt()  ;

        persObj->m_data[j]   = s.chamberId().get_identifier32().get_compact();
                        
        persObj->m_data[j+1] = s.mdtMl1().nhits      ;
        persObj->m_data[j+2] = s.mdtMl1().nholes     ;
        persObj->m_data[j+3] = s.mdtMl1().noutliers  ;
        persObj->m_data[j+4] = s.mdtMl1().ndeltas    ;
        persObj->m_data[j+5] = s.mdtMl1().ncloseHits ;
                        
        persObj->m_data[j+6] = s.mdtMl2().nhits      ;
        persObj->m_data[j+7] = s.mdtMl2().nholes     ;
        persObj->m_data[j+8] = s.mdtMl2().noutliers  ;
        persObj->m_data[j+9] = s.mdtMl2().ndeltas    ;
        persObj->m_data[j+10]= s.mdtMl2().ncloseHits ;        

        ++i;
        j += 11;
    }
}
