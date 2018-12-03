/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimData/CscSimData.h"
#include "MuonEventTPCnv/MuonDigitContainer/CscSimDataCnv_p2.h"

void CscSimDataCnv_p2::persToTrans(const Muon::CscSimData_p2 * persObj, CscSimData * transObj, MsgStream & log){
   log << MSG::DEBUG << "CscSimDataCnv_p2::persToTrans" << endreq;

   std::vector<CscSimData::Deposit> deposits;
   deposits.reserve(persObj->m_deposits.size());
   for (const std::pair<HepMcParticleLink_p2, Muon::CscMcData_p1>& p :
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

void CscSimDataCnv_p2::transToPers( const CscSimData *, Muon::CscSimData_p2 *, MsgStream & /*log*/){
  throw std::runtime_error("CscSimDataCnv_p2::transToPers is not supported in this release!");
}
