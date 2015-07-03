/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "MuonSimData/MuonSimData.h"
#undef private
#undef protected

#include "MuonEventTPCnv/MuonDigitContainer/MuonSimDataCnv_p1.h"

void MuonSimDataCnv_p1::persToTrans(const Muon::MuonSimData_p1 * persObj, MuonSimData * transObj, MsgStream & log){
   log << MSG::DEBUG << "MuonSimDataCnv_p1::persToTrans" << endreq;
   transObj->m_word = persObj->m_word;
   transObj->x = persObj->x;
   transObj->y = persObj->y;
   transObj->z = persObj->z;
   log << MSG::VERBOSE << "\tconverting m_word:\t" << persObj->m_word << "\tto\t" << transObj->m_word << endreq;
   log << MSG::VERBOSE << "\tconverting x:\t" << persObj->x << "\tto\t" << transObj->x << endreq;
   log << MSG::VERBOSE << "\tconverting y:\t" << persObj->y << "\tto\t" << transObj->y << endreq;
   log << MSG::VERBOSE << "\tconverting z:\t" << persObj->z << "\tto\t" << transObj->z << endreq;
   transObj->m_deposits.resize(persObj->m_deposits.size());
   for(unsigned int ic=0; ic<persObj->m_deposits.size(); ic++){
      HepMcParticleLink transMcPartLink;
      m_mcpartlinkCnv.persToTrans(&persObj->m_deposits[ic].first, &transMcPartLink, log);
      MuonMCData transMcData;
      m_mcdataCnv.persToTrans(&persObj->m_deposits[ic].second, &transMcData, log);
      transObj->m_deposits[ic] = std::make_pair(transMcPartLink, transMcData);
   }
}

void MuonSimDataCnv_p1::transToPers( const MuonSimData * transObj, Muon::MuonSimData_p1 * persObj, MsgStream & log){
   log << MSG::DEBUG << "MuonSimDataCnv_p1::TransToPers" << endreq;
   persObj->m_word = transObj->m_word;
   persObj->x = transObj->x;
   persObj->y = transObj->y;
   persObj->z = transObj->z;
   log << MSG::VERBOSE << "\tconverting m_word:\t" << transObj->m_word << "\tto\t" << persObj->m_word << endreq;
   log << MSG::VERBOSE << "\tconverting x:\t" << transObj->x << "\tto\t" << persObj->x << endreq;
   log << MSG::VERBOSE << "\tconverting y:\t" << transObj->y << "\tto\t" << persObj->y << endreq;
   log << MSG::VERBOSE << "\tconverting z:\t" << transObj->z << "\tto\t" << persObj->z << endreq;
   persObj->m_deposits.resize(transObj->m_deposits.size());
   for(unsigned int ic=0; ic<transObj->m_deposits.size(); ic++){
      HepMcParticleLink_p1 persMcPartLink;
      m_mcpartlinkCnv.transToPers(&transObj->m_deposits[ic].first, &persMcPartLink, log);
      Muon::MuonMCData_p1 persMcData;
      m_mcdataCnv.transToPers(&transObj->m_deposits[ic].second, &persMcData, log);
      persObj->m_deposits[ic] = std::make_pair(persMcPartLink, persMcData);
   }
}
