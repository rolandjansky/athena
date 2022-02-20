/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TgcPrepDataReplicationTool3BCtoAllBC.h"
#include "TgcPrepDataReplicationToolAllBCto3BC.h"
#include "MuonCnvToolInterfaces/IDC_Helper.h"

#include "MuonDigitContainer/TgcDigit.h"
#include "EventPrimitives/EventPrimitives.h"

//================ Constructor =================================================
Muon::TgcPrepDataReplicationTool3BCtoAllBC::TgcPrepDataReplicationTool3BCtoAllBC 
  (const std::string& t, const std::string& n, const IInterface* p)
  : base_class(t, n, p) {}

//================ Initialization ==============================================
StatusCode Muon::TgcPrepDataReplicationTool3BCtoAllBC::initialize()
{
  ATH_CHECK(AthAlgTool::initialize());
  ATH_CHECK(m_idHelperSvc.retrieve());
  
  for(int ibc = 0; ibc < BC_ALL; ibc++) {
    std::ostringstream location;
    location << "TGC_Measurements" 
             << (ibc == BC_PREVIOUS ? "PriorBC" : "")                      
             << (ibc == BC_NEXT ? "NextBC" : "");
    m_3BCKeys.at(ibc) = location.str();
  }

  ATH_CHECK(m_3BCKeys.initialize());


  return StatusCode::SUCCESS;
}

StatusCode Muon::TgcPrepDataReplicationTool3BCtoAllBC::replicate(const EventContext& ctx) const
{
    return convert3BCtoAllBC(ctx);
}

StatusCode Muon::TgcPrepDataReplicationTool3BCtoAllBC::convert3BCtoAllBC(const EventContext& ctx) const {

  SG::WriteHandle<TgcPrepDataContainer> tgcPrepDataContainerAll(m_AllBCKey, ctx);
  ATH_CHECK(tgcPrepDataContainerAll.record(std::make_unique<TgcPrepDataContainer>(m_idHelperSvc->tgcIdHelper().module_hash_max()) ));
  
  auto tgc3BCs = m_3BCKeys.makeHandles(ctx);

  // convert
  int ibc{0};
  for (SG::ReadHandle<TgcPrepDataContainer>& handle : m_3BCKeys.makeHandles(ctx)) {
    uint16_t bcBitMap = 0;
    if (ibc == BC_PREVIOUS)     bcBitMap = TgcPrepData::BCBIT_PREVIOUS;
    else if (ibc == BC_CURRENT) bcBitMap = TgcPrepData::BCBIT_CURRENT;
    else if (ibc == BC_NEXT)    bcBitMap = TgcPrepData::BCBIT_NEXT;
    ++ibc;

    if(!handle.isValid()) {
      ATH_MSG_FATAL("Cannot retrieve " << handle.key());
      return StatusCode::FAILURE;
    }
    for (const Muon::TgcPrepDataCollection* coll : *handle) {
    
      for (const Muon::TgcPrepData* to_copy : *coll) {
        const Identifier channelId = to_copy->identify();
        const Identifier elementId = m_idHelperSvc->tgcIdHelper().elementID(channelId);

        Muon::TgcPrepDataCollection* collection = Muon::IDC_Helper::getCollection<TgcPrepDataContainer, TgcIdHelper>
                            (elementId, tgcPrepDataContainerAll.ptr(), m_idHelperSvc->tgcIdHelper(), msg());

        Muon::TgcPrepDataCollection::iterator duplicateInAllBCs = 
        std::find_if(collection->begin(),collection->end(),[&channelId](const TgcPrepData* duplicate){
            return channelId == duplicate->identify();
        });
        if(duplicateInAllBCs != collection->end()) {
          TgcPrepData *prd = *duplicateInAllBCs;
          prd->setBcBitMap(prd->getBcBitMap() | bcBitMap);
        } else {
          Muon::TgcPrepData* newPrepData = TgcPrepDataReplicationToolAllBCto3BC::makeTgcPrepData(to_copy, bcBitMap);
          newPrepData->setHashAndIndex(collection->identifyHash(), collection->size());
          collection->push_back(newPrepData);
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

