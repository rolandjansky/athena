/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "MuonSimData/CscSimData.h"
#undef private
#undef protected

#include "MuonEventTPCnv/MuonDigitContainer/CscMcDataCnv_p1.h"

void CscMcDataCnv_p1::persToTrans(const Muon::CscMcData_p1 * persObj, CscMcData * transObj, MsgStream & log){
   log << MSG::DEBUG << "CscMcDataCnv_p1::persToTrans" << endreq;
   transObj->m_energy = persObj->m_energy;
   transObj->m_ypos = persObj->m_ypos;
   transObj->m_zpos = persObj->m_zpos;
   transObj->m_charge = persObj->m_charge;
   log << MSG::VERBOSE << "\tconverting m_energy:\t" << persObj->m_energy << "\tto\t" << transObj->m_energy << endreq;
   log << MSG::VERBOSE << "\tconverting m_ypos:\t" << persObj->m_ypos << "\tto\t" << transObj->m_ypos << endreq;
   log << MSG::VERBOSE << "\tconverting m_zpos:\t" << persObj->m_zpos << "\tto\t" << transObj->m_zpos << endreq;
   log << MSG::VERBOSE << "\tconverting m_charge:\t" << persObj->m_charge << "\tto\t" << transObj->m_charge << endreq;
}

void CscMcDataCnv_p1::transToPers( const CscMcData * transObj, Muon::CscMcData_p1 * persObj, MsgStream & log){
   log << MSG::DEBUG << "CscMcDataCnv_p1::transToPers" << endreq;
   persObj->m_energy = transObj->m_energy;
   persObj->m_ypos = transObj->m_ypos;
   persObj->m_zpos = transObj->m_zpos;
   persObj->m_charge = transObj->m_charge;
   log << MSG::VERBOSE << "\tconverting m_energy:\t" << transObj->m_energy << "\tto\t" << persObj->m_energy << endreq;
   log << MSG::VERBOSE << "\tconverting m_ypos:\t" << transObj->m_ypos << "\tto\t" << persObj->m_ypos << endreq;
   log << MSG::VERBOSE << "\tconverting m_zpos:\t" << transObj->m_zpos << "\tto\t" << persObj->m_zpos << endreq;
   log << MSG::VERBOSE << "\tconverting m_charge:\t" << transObj->m_charge << "\tto\t" << persObj->m_charge << endreq;
}
