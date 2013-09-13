/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMuonEvent/TrigMuonEF.h"
#include "TrigMuonEventTPCnv/TrigMuonEF_p1.h"
#undef private
#undef protected

#include "TrigMuonEventTPCnv/TrigMuonEFCnv_p1.h"


void TrigMuonEFCnv_p1::persToTrans(const TrigMuonEF_p1* persObj, 
				    TrigMuonEF* transObj, 
				    MsgStream &log)
{
   log << MSG::DEBUG << "TrigMuonEFCnv_p1::persToTrans called " << endreq;

   transObj->m_muonCode = persObj->m_muonCode ; 
   transObj->m_roi      = persObj->m_roi      ;
   transObj->m_charge   = persObj->m_charge   ;

   fillTransFromPStore( &m_P4IPtCotThPhiMCnv, persObj->m_P4IPtCotThPhiM, transObj, log );
}


void TrigMuonEFCnv_p1::transToPers(const TrigMuonEF* transObj, 
				       TrigMuonEF_p1* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigMuonEFCnv_p1::transToPers called " << endreq;

   persObj->m_muonCode = transObj->m_muonCode ; 
   persObj->m_roi      = transObj->m_roi      ;
   persObj->m_charge   = transObj->m_charge   ;

   persObj->m_P4IPtCotThPhiM = baseToPersistent( &m_P4IPtCotThPhiMCnv, transObj, log );
 
}
