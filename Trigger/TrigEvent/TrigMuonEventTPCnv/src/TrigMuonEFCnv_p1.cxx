/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/TrigMuonEF.h"
#include "TrigMuonEventTPCnv/TrigMuonEF_p1.h"
#include "TrigMuonEventTPCnv/TrigMuonEFCnv_p1.h"


void TrigMuonEFCnv_p1::persToTrans(const TrigMuonEF_p1* persObj, 
				    TrigMuonEF* transObj, 
				    MsgStream &log)
{
   log << MSG::DEBUG << "TrigMuonEFCnv_p1::persToTrans called " << endreq;

   transObj->set_muonCode (persObj->m_muonCode); 
   transObj->set_RoINum   (persObj->m_roi);
   transObj->set_Charge   (persObj->m_charge);

   fillTransFromPStore( &m_P4IPtCotThPhiMCnv, persObj->m_P4IPtCotThPhiM, transObj, log );
}


void TrigMuonEFCnv_p1::transToPers(const TrigMuonEF* transObj, 
				       TrigMuonEF_p1* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigMuonEFCnv_p1::transToPers called " << endreq;

   persObj->m_muonCode = transObj->MuonCode(); 
   persObj->m_roi      = transObj->RoINum();
   persObj->m_charge   = transObj->Charge();

   persObj->m_P4IPtCotThPhiM = baseToPersistent( &m_P4IPtCotThPhiMCnv, transObj, log );
 
}
