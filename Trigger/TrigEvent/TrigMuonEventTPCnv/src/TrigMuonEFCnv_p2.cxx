/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/TrigMuonEF.h"
#include "TrigMuonEventTPCnv/TrigMuonEF_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCnv_p2.h"


void TrigMuonEFCnv_p2::persToTrans(const TrigMuonEF_p2* persObj, 
				    TrigMuonEF* transObj, 
				    MsgStream &log)
{
   log << MSG::DEBUG << "TrigMuonEFCnv_p2::persToTrans called " << endreq;

   transObj->set_muonCode (persObj->m_muonCode); 
   transObj->set_RoINum   (persObj->m_roi);
   transObj->set_Charge   (persObj->m_charge);

//   fillTransFromPStore( &m_P4IPtCotThPhiMCnv, persObj->m_P4IPtCotThPhiM, transObj, log );
   m_P4IPtCotThPhiMCnv.persToTrans(&(persObj->m_P4IPtCotThPhiM), transObj, log);
}


void TrigMuonEFCnv_p2::transToPers(const TrigMuonEF* transObj, 
				       TrigMuonEF_p2* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigMuonEFCnv_p2::transToPers called " << endreq;

   persObj->m_muonCode = transObj->MuonCode(); 
   persObj->m_roi      = transObj->RoINum();
   persObj->m_charge   = transObj->Charge();

//   persObj->m_P4IPtCotThPhiM = baseToPersistent( &m_P4IPtCotThPhiMCnv, transObj, log );
   m_P4IPtCotThPhiMCnv.transToPers(transObj, &(persObj->m_P4IPtCotThPhiM), log);
 
}
