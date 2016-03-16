/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigTauTracksInfo.h"
#include "TrigInDetEventTPCnv/TrigTauTracksInfo_p1.h"
#include "TrigInDetEventTPCnv/TrigTauTracksInfoCnv_p1.h"
 
//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigTauTracksInfoCnv_p1::persToTrans( const TrigTauTracksInfo_p1 *persObj,
					   TrigTauTracksInfo    *transObj,
					   MsgStream       &log )
{
  log << MSG::DEBUG << "TrigTauTracksInfoCnv_p1::persToTrans called " << endreq;

  transObj->setRoiId                 (persObj->m_roiID)        ;
  transObj->setNCoreTracks           (persObj->m_nCoreTracks)    ;
  transObj->setNSlowTracks           (persObj->m_nSlowTracks)    ;
  transObj->setNIsoTracks            (persObj->m_nIsoTracks)     ;
  
  transObj->setCharge                (persObj->m_charge)         ; 
  transObj->setLeadingTrackPt        (persObj->m_leadingTrackPt) ;
  transObj->setScalarPtSumCore       (persObj->m_scalarPtSumCore);
  transObj->setScalarPtSumIso        (persObj->m_scalarPtSumIso) ;
  transObj->setPtBalance             (persObj->m_ptBalance)      ;   
  

  P4PtEtaPhiM* mom = createTransFromPStore( &m_3fastestP4PtEtaPhiMCnv, persObj->m_3fastest, log );
  transObj->set3fastestPtEtaPhiM (mom->pt(),
                                  mom->eta(),
                                  mom->phi(),
                                  mom->m());
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

  persObj->m_roiID                 = transObj->roiId()        ;
  persObj->m_nCoreTracks           = transObj->nCoreTracks()    ;
  persObj->m_nSlowTracks           = transObj->nSlowTracks()    ;
  persObj->m_nIsoTracks            = transObj->nIsoTracks()     ;
         			      	  
  persObj->m_charge                = transObj->charge()         ; 
  persObj->m_leadingTrackPt        = transObj->leadingTrackPt() ;
  persObj->m_scalarPtSumCore       = transObj->scalarPtSumCore();
  persObj->m_scalarPtSumIso        = transObj->scalarPtSumIso() ;
  persObj->m_ptBalance             = transObj->ptBalance()      ;   

  persObj->m_3fastest  = toPersistent( &m_3fastestP4PtEtaPhiMCnv, &transObj->threeFastestTracks(), log );
  persObj->m_P4PtEtaPhiM  = baseToPersistent( &m_P4PtEtaPhiMCnv, transObj, log );
    
}
