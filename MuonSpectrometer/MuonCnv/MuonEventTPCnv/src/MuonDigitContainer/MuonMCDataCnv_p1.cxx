/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimData/MuonSimData.h"
#include "MuonEventTPCnv/MuonDigitContainer/MuonMCDataCnv_p1.h"

void MuonMCDataCnv_p1::persToTrans(const Muon::MuonMCData_p1 * persObj, MuonMCData * transObj, MsgStream & log){
   log << MSG::DEBUG << "MuonMCDataCnv_p1::persToTrans" << endmsg;
   transObj->SetData (persObj->m_first, persObj->m_second);
   //log << MSG::VERBOSE << "\tconverting m_first:\t" << persObj->m_first << "\tto\t" << transObj->m_first << endmsg;
   //log << MSG::VERBOSE << "\tconverting m_second:\t" << persObj->m_second << "\tto\t" << transObj->m_second << endmsg;
}

void MuonMCDataCnv_p1::transToPers( const MuonMCData * transObj, Muon::MuonMCData_p1 * persObj, MsgStream & log){
   log << MSG::DEBUG << "MuonMCDataCnv_p1::transToPers" << endmsg;
   persObj->m_first = transObj->firstEntry();
   persObj->m_second = transObj->secondEntry();
   //log << MSG::VERBOSE << "\tconverting m_first:\t" << transObj->m_first << "\tto\t" << persObj->m_first << endmsg;
   //log << MSG::VERBOSE << "\tconverting m_second:\t" << transObj->m_second << "\tto\t" << persObj->m_second << endmsg;
}
