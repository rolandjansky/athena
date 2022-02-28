/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TgcPrepDataReplicationToolAllBCto3BC.h"

#include "MuonCnvToolInterfaces/IDC_Helper.h"

#include "MuonDigitContainer/TgcDigit.h"
#include "EventPrimitives/EventPrimitives.h"


//================ Constructor =================================================
Muon::TgcPrepDataReplicationToolAllBCto3BC::TgcPrepDataReplicationToolAllBCto3BC 
  (const std::string& t, const std::string& n, const IInterface* p)
  : base_class(t, n, p) {}

//================ Initialization ==============================================
StatusCode Muon::TgcPrepDataReplicationToolAllBCto3BC::initialize() {
  ATH_CHECK(m_idHelperSvc.retrieve());
  
  for(int ibc = 0; ibc < BC_ALL; ibc++) {
    std::ostringstream location;
    location << "TGC_Measurements" 
             << (ibc == BC_PREVIOUS ? "PriorBC" : "")                      
             << (ibc == BC_NEXT ? "NextBC" : "");
    m_3BCKeys.at(ibc) = location.str();
  }

  ATH_CHECK(m_3BCKeys.initialize());
  ATH_CHECK(m_AllBCKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode Muon::TgcPrepDataReplicationToolAllBCto3BC::replicate(const EventContext& ctx) const
{
    return convertAllBCto3BC(ctx);
}

StatusCode Muon::TgcPrepDataReplicationToolAllBCto3BC::convertAllBCto3BC(const EventContext& ctx) const {

  SG::ReadHandle<TgcPrepDataContainer> tgcAll{m_AllBCKey, ctx};
 
  if(!tgcAll.isValid()) {
    ATH_MSG_FATAL("Cannot retrieve TGC_MeasurementsAllBCs");
    return StatusCode::FAILURE;
  }


  // convert
  auto tgc3BCHandles = m_3BCKeys.makeHandles(ctx);
  for (int ibc = 0; ibc < BC_ALL; ibc++){
      tgc3BCHandles.at(ibc) = std::make_unique<TgcPrepDataContainer>(m_idHelperSvc->tgcIdHelper().module_hash_max());
  }

  for (const Muon::TgcPrepDataCollection * coll : *tgcAll) {
    for (const Muon::TgcPrepData* to_copy : *coll) {

      uint16_t bcBitMap = to_copy->getBcBitMap();
      std::array<uint16_t, BC_NUM> hasBC{0};
      if (bcBitMap & TgcPrepData::BCBIT_PREVIOUS) 
        hasBC[BC_PREVIOUS] = TgcPrepData::BCBIT_PREVIOUS;
      if (bcBitMap & TgcPrepData::BCBIT_CURRENT)  
        hasBC[BC_CURRENT] = TgcPrepData::BCBIT_CURRENT;
      if (bcBitMap & TgcPrepData::BCBIT_NEXT)     
        hasBC[BC_NEXT] = TgcPrepData::BCBIT_NEXT;

      const Identifier channelId = to_copy->identify();
      const Identifier elementId = m_idHelperSvc->tgcIdHelper().elementID(channelId);
      std::array<Muon::TgcPrepDataCollection*, BC_ALL> collections{};
      for (int ibc = 0; ibc < BC_ALL; ibc++) {
        collections[ibc] = Muon::IDC_Helper::getCollection<TgcPrepDataContainer, TgcIdHelper>
                            (elementId, tgc3BCHandles[ibc].ptr(), m_idHelperSvc->tgcIdHelper(), msg());
     
        if (!hasBC[ibc]) continue;
        Muon::TgcPrepData* newPrepData = makeTgcPrepData(to_copy, hasBC[ibc]);
        newPrepData->setHashAndIndex(collections[ibc]->identifyHash(), collections[ibc]->size());
        collections[ibc]->push_back(newPrepData);
      }
    }
  }


  return StatusCode::SUCCESS;
}


Muon::TgcPrepData*
Muon::TgcPrepDataReplicationToolAllBCto3BC::makeTgcPrepData(const Muon::TgcPrepData* to_copy, uint16_t bcBitMap)
{
  Identifier channelId = to_copy->identify();
  IdentifierHash tgcHashId = to_copy->collectionHash();
  const std::vector<Identifier> &identifierList = to_copy->rdoList();
  const Amg::MatrixX newErrHitPos{to_copy->localCovariance()};
  const MuonGM::TgcReadoutElement* descriptor = to_copy->detectorElement();
  Muon::TgcPrepData* newPrepData = new TgcPrepData(channelId, tgcHashId, to_copy->localPosition(),
                                                   identifierList, newErrHitPos, descriptor);
  newPrepData->setBcBitMap(bcBitMap);

  return newPrepData;
}
