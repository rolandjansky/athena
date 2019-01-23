/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimData/MuonSimData.h"
#include "MuonEventTPCnv/MuonDigitContainer/MuonSimDataCnv_p2.h"

void MuonSimDataCnv_p2::persToTrans(const Muon::MuonSimData_p2 * persObj, MuonSimData * transObj, MsgStream & log){
   log << MSG::DEBUG << "MuonSimDataCnv_p2::persToTrans" << endmsg;
   //log << MSG::VERBOSE << "\tconverting m_word:\t" << persObj->m_word << "\tto\t" << transObj->m_word << endmsg;
   //log << MSG::VERBOSE << "\tconverting x:\t" << persObj->x << "\tto\t" << transObj->x << endmsg;
   //log << MSG::VERBOSE << "\tconverting y:\t" << persObj->y << "\tto\t" << transObj->y << endmsg;
   //log << MSG::VERBOSE << "\tconverting z:\t" << persObj->z << "\tto\t" << transObj->z << endmsg;

   std::vector<MuonSimData::Deposit> deposits;
   deposits.reserve(persObj->m_deposits.size());
   HepMcParticleLink transMcPartLink;
   for (const std::pair<HepMcParticleLink_p2, Muon::MuonMCData_p1>& p :
          persObj->m_deposits)
   {
      m_mcpartlinkCnv.persToTrans(&p.first, &transMcPartLink, log);
      MuonMCData transMcData;
      m_mcdataCnv.persToTrans(&p.second, &transMcData, log);
      deposits.emplace_back(transMcPartLink, transMcData);
   }

   *transObj = MuonSimData (std::move(deposits),
                            persObj->m_word);

   Amg::Vector3D pos (persObj->x, persObj->y, persObj->z);
   transObj->setPosition (pos);
   transObj->setTime(persObj->t);
}

void MuonSimDataCnv_p2::transToPers( const MuonSimData * transObj, Muon::MuonSimData_p2 * persObj, MsgStream & log){
  log << MSG::DEBUG << "MuonSimDataCnv_p2::TransToPers" << endmsg;
  persObj->m_word = transObj->word();
  Amg::Vector3D pos = transObj->globalPosition();
  persObj->x = pos.x();
  persObj->y = pos.y();
  persObj->z = pos.z();
  persObj->t = transObj->getTime();
  persObj->m_deposits.clear();
  persObj->m_deposits.reserve(transObj->getdeposits().size());
  for (const MuonSimData::Deposit& d : transObj->getdeposits()) {
    HepMcParticleLink_p2 persMcPartLink;
    m_mcpartlinkCnv.transToPers(&d.first, &persMcPartLink, log);
    Muon::MuonMCData_p1 persMcData;
    m_mcdataCnv.transToPers(&d.second, &persMcData, log);
    persObj->m_deposits.emplace_back(persMcPartLink, persMcData);
  }
}
