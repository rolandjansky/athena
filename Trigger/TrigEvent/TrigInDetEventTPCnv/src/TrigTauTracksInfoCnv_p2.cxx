/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigInDetEvent/TrigTauTracksInfo.h"
#include "TrigInDetEventTPCnv/TrigTauTracksInfo_p2.h"
#undef private
#undef protected
 
#include "TrigInDetEventTPCnv/TrigTauTracksInfoCnv_p2.h"
 
//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigTauTracksInfoCnv_p2::persToTrans( const TrigTauTracksInfo_p2 *persObj,
					   TrigTauTracksInfo    *transObj,
					   MsgStream       &log )
{
  log << MSG::DEBUG << "TrigTauTracksInfoCnv_p2::persToTrans called " << endreq;

  transObj->m_roiID                 = persObj->m_allTheInts[0]  ;
  transObj->m_nCoreTracks           = persObj->m_allTheInts[1]  ;
  transObj->m_nSlowTracks           = persObj->m_allTheInts[2]  ;
  transObj->m_nIsoTracks            = persObj->m_allTheInts[3]  ;
  
  transObj->m_charge                = persObj->m_allTheFloats[0] ; 
  transObj->m_leadingTrackPt        = persObj->m_allTheFloats[1] ;
  transObj->m_scalarPtSumCore       = persObj->m_allTheFloats[2] ;
  transObj->m_scalarPtSumIso        = persObj->m_allTheFloats[3] ;
  transObj->m_ptBalance             = persObj->m_allTheFloats[4] ;   
  
  m_P4PtEtaPhiMCnv.persToTrans(&(persObj->m_P4PtEtaPhiM), transObj, log);
  m_3fastestP4PtEtaPhiMCnv.persToTrans(&(persObj->m_3fastest), transObj, log);

}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigTauTracksInfoCnv_p2::transToPers( const TrigTauTracksInfo    *transObj,
					     TrigTauTracksInfo_p2 *persObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigTauTracksInfoCnv_p2::transToPers called " << endreq;

  persObj->m_allTheInts[0] = transObj->m_roiID        ;
  persObj->m_allTheInts[1] = transObj->m_nCoreTracks    ;
  persObj->m_allTheInts[2] = transObj->m_nSlowTracks    ;
  persObj->m_allTheInts[3] = transObj->m_nIsoTracks     ;
         			      	  
  persObj->m_allTheFloats[0] = transObj->m_charge         ; 
  persObj->m_allTheFloats[1] = transObj->m_leadingTrackPt ;
  persObj->m_allTheFloats[2] = transObj->m_scalarPtSumCore;
  persObj->m_allTheFloats[3] = transObj->m_scalarPtSumIso ;
  persObj->m_allTheFloats[4] = transObj->m_ptBalance      ;   

  m_P4PtEtaPhiMCnv.transToPers(transObj, &(persObj->m_P4PtEtaPhiM), log);
  m_3fastestP4PtEtaPhiMCnv.transToPers(transObj, &(persObj->m_3fastest), log);

}
