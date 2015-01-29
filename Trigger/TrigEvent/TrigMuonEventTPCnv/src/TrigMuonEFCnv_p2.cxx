/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMuonEvent/TrigMuonEF.h"
#include "TrigMuonEventTPCnv/TrigMuonEF_p2.h"
#undef private
#undef protected

#include "TrigMuonEventTPCnv/TrigMuonEFCnv_p2.h"


void TrigMuonEFCnv_p2::persToTrans(const TrigMuonEF_p2* persObj, 
				    TrigMuonEF* transObj, 
				    MsgStream &log)
{
   log << MSG::DEBUG << "TrigMuonEFCnv_p2::persToTrans called " << endreq;

   transObj->m_muonCode = persObj->m_muonCode ; 
   transObj->m_roi      = persObj->m_roi      ;
   transObj->m_charge   = persObj->m_charge   ;

//   fillTransFromPStore( &m_P4IPtCotThPhiMCnv, persObj->m_P4IPtCotThPhiM, transObj, log );
   m_P4IPtCotThPhiMCnv.persToTrans(&(persObj->m_P4IPtCotThPhiM), transObj, log);
}


void TrigMuonEFCnv_p2::transToPers(const TrigMuonEF* transObj, 
				       TrigMuonEF_p2* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigMuonEFCnv_p2::transToPers called " << endreq;

   persObj->m_muonCode = transObj->m_muonCode ; 
   persObj->m_roi      = transObj->m_roi      ;
   persObj->m_charge   = transObj->m_charge   ;

//   persObj->m_P4IPtCotThPhiM = baseToPersistent( &m_P4IPtCotThPhiMCnv, transObj, log );
   m_P4IPtCotThPhiMCnv.transToPers(transObj, &(persObj->m_P4IPtCotThPhiM), log);
 
}
