/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimData/CscSimData.h"
#include "MuonEventTPCnv/MuonDigitContainer/CscMcDataCnv_p1.h"

void CscMcDataCnv_p1::persToTrans(const Muon::CscMcData_p1 * persObj, CscMcData * transObj, MsgStream & log){
   log << MSG::DEBUG << "CscMcDataCnv_p1::persToTrans" << endreq;

   *transObj = CscMcData(persObj->m_energy,
                         persObj->m_ypos,
                         persObj->m_zpos);
   transObj->setCharge (persObj->m_charge);

   log << MSG::VERBOSE << "\tconverting m_energy:\t" << persObj->m_energy << "\tto\t" << transObj->energy() << endreq;
   log << MSG::VERBOSE << "\tconverting m_ypos:\t" << persObj->m_ypos << "\tto\t" << transObj->ypos() << endreq;
   log << MSG::VERBOSE << "\tconverting m_zpos:\t" << persObj->m_zpos << "\tto\t" << transObj->zpos() << endreq;
   log << MSG::VERBOSE << "\tconverting m_charge:\t" << persObj->m_charge << "\tto\t" << transObj->charge() << endreq;
}

void CscMcDataCnv_p1::transToPers( const CscMcData * transObj, Muon::CscMcData_p1 * persObj, MsgStream & log){
   log << MSG::DEBUG << "CscMcDataCnv_p1::transToPers" << endreq;
   persObj->m_energy = transObj->energy();
   persObj->m_ypos = transObj->ypos();
   persObj->m_zpos = transObj->zpos();
   persObj->m_charge = transObj->charge();
   log << MSG::VERBOSE << "\tconverting m_energy:\t" << transObj->energy() << "\tto\t" << persObj->m_energy << endreq;
   log << MSG::VERBOSE << "\tconverting m_ypos:\t" << transObj->ypos() << "\tto\t" << persObj->m_ypos << endreq;
   log << MSG::VERBOSE << "\tconverting m_zpos:\t" << transObj->zpos() << "\tto\t" << persObj->m_zpos << endreq;
   log << MSG::VERBOSE << "\tconverting m_charge:\t" << transObj->charge() << "\tto\t" << persObj->m_charge << endreq;
}
