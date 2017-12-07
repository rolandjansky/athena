/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimData/CscSimData.h"
#include "MuonEventTPCnv/MuonDigitContainer/CscSimDataCnv_p1.h"

void CscSimDataCnv_p1::persToTrans(const Muon::CscSimData_p1 * persObj, CscSimData * transObj, MsgStream & log){
   log << MSG::DEBUG << "CscSimDataCnv_p1::persToTrans" << endmsg;

   std::vector<CscSimData::Deposit> deposits;
   deposits.reserve(persObj->m_deposits.size());
   for (const std::pair<HepMcParticleLink_p1, Muon::CscMcData_p1>& p :
          persObj->m_deposits)
   {
      HepMcParticleLink transMcPartLink;
      m_mcpartlinkCnv.persToTrans(&p.first, &transMcPartLink, log);
      CscMcData transMcData;
      m_mcdataCnv.persToTrans(&p.second, &transMcData, log);
      deposits.emplace_back(transMcPartLink, transMcData);
   }

   *transObj = CscSimData (std::move(deposits),
                           persObj->m_word);
}

void CscSimDataCnv_p1::transToPers( const CscSimData * transObj, Muon::CscSimData_p1 * persObj, MsgStream & log){
   log << MSG::DEBUG << "CscSimDataCnv_p1::transToPers" << endmsg;
   persObj->m_word = transObj->word();
   log << MSG::VERBOSE << "\tconverting m_word:\t" << transObj->word() << "\tto\t" << persObj->m_word << endmsg;
   persObj->m_deposits.clear();
   persObj->m_deposits.reserve(transObj->getdeposits().size());
   for (const CscSimData::Deposit& d : transObj->getdeposits()) {
      HepMcParticleLink_p1 persMcPartLink;
      m_mcpartlinkCnv.transToPers(&d.first, &persMcPartLink, log);
      Muon::CscMcData_p1 persMcData;
      m_mcdataCnv.transToPers(&d.second, &persMcData, log);
      persObj->m_deposits.emplace_back (persMcPartLink, persMcData);
   }
}
