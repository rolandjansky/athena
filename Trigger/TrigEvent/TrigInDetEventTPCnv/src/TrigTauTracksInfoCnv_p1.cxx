/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigInDetEvent/TrigTauTracksInfo.h"
#include "TrigInDetEventTPCnv/TrigTauTracksInfo_p1.h"
#undef private
#undef protected
 
#include "TrigInDetEventTPCnv/TrigTauTracksInfoCnv_p1.h"
 
//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigTauTracksInfoCnv_p1::persToTrans( const TrigTauTracksInfo_p1 *persObj,
					   TrigTauTracksInfo    *transObj,
					   MsgStream       &log )
{
  log << MSG::DEBUG << "TrigTauTracksInfoCnv_p1::persToTrans called " << endreq;

  transObj->m_roiID                 = persObj->m_roiID        ;
  transObj->m_nCoreTracks           = persObj->m_nCoreTracks    ;
  transObj->m_nSlowTracks           = persObj->m_nSlowTracks    ;
  transObj->m_nIsoTracks            = persObj->m_nIsoTracks     ;
  
  transObj->m_charge                = persObj->m_charge         ; 
  transObj->m_leadingTrackPt        = persObj->m_leadingTrackPt ;
  transObj->m_scalarPtSumCore       = persObj->m_scalarPtSumCore;
  transObj->m_scalarPtSumIso        = persObj->m_scalarPtSumIso ;
  transObj->m_ptBalance             = persObj->m_ptBalance      ;   
  

  I4Momentum* mom = createTransFromPStore( &m_3fastestP4PtEtaPhiMCnv, persObj->m_3fastest, log );
  transObj->m_3fastest = *mom;
  delete mom;

  fillTransFromPStore( &m_P4PtEtaPhiMCnv, persObj->m_P4PtEtaPhiM, transObj, log );     
}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigTauTracksInfoCnv_p1::transToPers( const TrigTauTracksInfo    *transObj,
					     TrigTauTracksInfo_p1 *persObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigTauTracksInfoCnv_p1::transToPers called " << endreq;

  persObj->m_roiID                 = transObj->m_roiID        ;
  persObj->m_nCoreTracks           = transObj->m_nCoreTracks    ;
  persObj->m_nSlowTracks           = transObj->m_nSlowTracks    ;
  persObj->m_nIsoTracks            = transObj->m_nIsoTracks     ;
         			      	  
  persObj->m_charge                = transObj->m_charge         ; 
  persObj->m_leadingTrackPt        = transObj->m_leadingTrackPt ;
  persObj->m_scalarPtSumCore       = transObj->m_scalarPtSumCore;
  persObj->m_scalarPtSumIso        = transObj->m_scalarPtSumIso ;
  persObj->m_ptBalance             = transObj->m_ptBalance      ;   

  persObj->m_3fastest  = toPersistent( &m_3fastestP4PtEtaPhiMCnv, &transObj->m_3fastest, log );
  persObj->m_P4PtEtaPhiM  = baseToPersistent( &m_P4PtEtaPhiMCnv, transObj, log );
    
}
