/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigTauTracksInfo.h"
#include "TrigInDetEventTPCnv/TrigTauTracksInfo_p2.h"
#include "TrigInDetEventTPCnv/TrigTauTracksInfoCnv_p2.h"
 
//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigTauTracksInfoCnv_p2::persToTrans( const TrigTauTracksInfo_p2 *persObj,
					   TrigTauTracksInfo    *transObj,
					   MsgStream       &log )
{
  log << MSG::DEBUG << "TrigTauTracksInfoCnv_p2::persToTrans called " << endreq;

  transObj->setRoiId                 (persObj->m_allTheInts[0])  ;
  transObj->setNCoreTracks           (persObj->m_allTheInts[1])  ;
  transObj->setNSlowTracks           (persObj->m_allTheInts[2])  ;
  transObj->setNIsoTracks            (persObj->m_allTheInts[3])  ;
  
  transObj->setCharge                (persObj->m_allTheFloats[0]) ; 
  transObj->setLeadingTrackPt        (persObj->m_allTheFloats[1]) ;
  transObj->setScalarPtSumCore       (persObj->m_allTheFloats[2]) ;
  transObj->setScalarPtSumIso        (persObj->m_allTheFloats[3]) ;
  transObj->setPtBalance             (persObj->m_allTheFloats[4]) ;   
  
  m_P4PtEtaPhiMCnv.persToTrans(&(persObj->m_P4PtEtaPhiM), transObj, log);
  P4PtEtaPhiM fastest3;
  m_3fastestP4PtEtaPhiMCnv.persToTrans(&(persObj->m_3fastest), &fastest3, log);
  transObj->set3fastestPtEtaPhiM (fastest3.pt(), fastest3.eta(),
                                  fastest3.phi(), fastest3.m());
}
 
//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigTauTracksInfoCnv_p2::transToPers( const TrigTauTracksInfo    *transObj,
					     TrigTauTracksInfo_p2 *persObj,
					     MsgStream       &log )
{
  log << MSG::DEBUG << "TrigTauTracksInfoCnv_p2::transToPers called " << endreq;

  persObj->m_allTheInts[0] = transObj->roiId()        ;
  persObj->m_allTheInts[1] = transObj->nCoreTracks()    ;
  persObj->m_allTheInts[2] = transObj->nSlowTracks()    ;
  persObj->m_allTheInts[3] = transObj->nIsoTracks()     ;
         			      	  
  persObj->m_allTheFloats[0] = transObj->charge()         ; 
  persObj->m_allTheFloats[1] = transObj->leadingTrackPt() ;
  persObj->m_allTheFloats[2] = transObj->scalarPtSumCore();
  persObj->m_allTheFloats[3] = transObj->scalarPtSumIso() ;
  persObj->m_allTheFloats[4] = transObj->ptBalance()      ;   

  m_P4PtEtaPhiMCnv.transToPers(transObj, &(persObj->m_P4PtEtaPhiM), log);
  m_3fastestP4PtEtaPhiMCnv.transToPers(&transObj->threeFastestTracks(),
                                       &(persObj->m_3fastest), log);

}
