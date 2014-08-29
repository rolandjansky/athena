/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "MuonSimData/MuonSimData.h"
#undef private
#undef protected

#include "MuonEventTPCnv/MuonDigitContainer/MuonMCDataCnv_p1.h"

void MuonMCDataCnv_p1::persToTrans(const Muon::MuonMCData_p1 * persObj, MuonMCData * transObj, MsgStream & log){
   log << MSG::DEBUG << "MuonMCDataCnv_p1::persToTrans" << endreq;
   transObj->m_first = persObj->m_first;
   transObj->m_second = persObj->m_second;
   log << MSG::VERBOSE << "\tconverting m_first:\t" << persObj->m_first << "\tto\t" << transObj->m_first << endreq;
   log << MSG::VERBOSE << "\tconverting m_second:\t" << persObj->m_second << "\tto\t" << transObj->m_second << endreq;
}

void MuonMCDataCnv_p1::transToPers( const MuonMCData * transObj, Muon::MuonMCData_p1 * persObj, MsgStream & log){
   log << MSG::DEBUG << "MuonMCDataCnv_p1::transToPers" << endreq;
   persObj->m_first = transObj->m_first;
   persObj->m_second = transObj->m_second;
   log << MSG::VERBOSE << "\tconverting m_first:\t" << transObj->m_first << "\tto\t" << persObj->m_first << endreq;
   log << MSG::VERBOSE << "\tconverting m_second:\t" << transObj->m_second << "\tto\t" << persObj->m_second << endreq;
}
