/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TgcPrepDataReplicationTool3BCtoAllBC.h"
#include "TgcPrepDataReplicationToolAllBCto3BC.h"
#include "MuonCnvToolInterfaces/IDC_Helper.h"

#include "MuonDigitContainer/TgcDigit.h"
#include "EventPrimitives/EventPrimitives.h"

//================ Constructor =================================================
Muon::TgcPrepDataReplicationTool3BCtoAllBC::TgcPrepDataReplicationTool3BCtoAllBC 
  (const std::string& t, const std::string& n, const IInterface* p)
  : AthAlgTool(t, n, p),
  m_3BCKeys{"dummy", "dummy", "dummy"},
  m_AllBCKey("TGC_MeasurementsAllBCs")
{
  declareProperty("BC3Keys", m_3BCKeys);
  declareProperty("AllBCKey", m_AllBCKey);
}  

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

StatusCode Muon::TgcPrepDataReplicationTool3BCtoAllBC::replicate()
{
    return convert3BCtoAllBC();
}

StatusCode Muon::TgcPrepDataReplicationTool3BCtoAllBC::convert3BCtoAllBC()
{

  SG::WriteHandle<TgcPrepDataContainer> tgcPrepDataContainerAll(m_AllBCKey);
  tgcPrepDataContainerAll = std::unique_ptr<TgcPrepDataContainer>( new TgcPrepDataContainer(m_idHelperSvc->tgcIdHelper().module_hash_max()) );
  
  auto tgc3BCs = m_3BCKeys.makeHandles();

  // convert

  for (int ibc = 0; ibc < BC_ALL; ibc++) {
    uint16_t bcBitMap = 0;
    if (ibc == BC_PREVIOUS)     bcBitMap = TgcPrepData::BCBIT_PREVIOUS;
    else if (ibc == BC_CURRENT) bcBitMap = TgcPrepData::BCBIT_CURRENT;
    else if (ibc == BC_NEXT)    bcBitMap = TgcPrepData::BCBIT_NEXT;


    if(tgc3BCs.at(ibc).isValid()==false) {
      ATH_MSG_FATAL("Cannot retrieve " << tgc3BCs.at(ibc).key());
    return StatusCode::FAILURE;
    }

    Muon::TgcPrepDataContainer::const_iterator tgcItr   = tgc3BCs[ibc]->begin();
    Muon::TgcPrepDataContainer::const_iterator tgcItrE  = tgc3BCs[ibc]->end();
  
    for (; tgcItr != tgcItrE; ++tgcItr) {
      Muon::TgcPrepDataCollection::const_iterator tgcColItr  = (*tgcItr)->begin();
      Muon::TgcPrepDataCollection::const_iterator tgcColItrE = (*tgcItr)->end();

      for (; tgcColItr != tgcColItrE; ++tgcColItr) {
        Identifier channelId = (*tgcColItr)->identify();
        Identifier elementId = m_idHelperSvc->tgcIdHelper().elementID(channelId);

        Muon::TgcPrepDataCollection* collection = Muon::IDC_Helper::getCollection<TgcPrepDataContainer, TgcIdHelper>
                            (elementId, tgcPrepDataContainerAll.ptr(), m_idHelperSvc->tgcIdHelper(), msg());

        bool duplicateInAllBCs = false;
        TgcPrepDataCollection::iterator tgcAllItr  = collection->begin();
        TgcPrepDataCollection::iterator tgcAllItrE = collection->end();
        for(; tgcAllItr != tgcAllItrE; tgcAllItr++) {
          if(channelId == (*tgcAllItr)->identify()) {
            duplicateInAllBCs = true;
            break;
          }
        }

        if(duplicateInAllBCs) {
          TgcPrepData *prd = *tgcAllItr;
          uint16_t bcBitMap_tmp = prd->getBcBitMap();
          prd->setBcBitMap(bcBitMap_tmp | bcBitMap);
        } else {
          Muon::TgcPrepData* newPrepData = TgcPrepDataReplicationToolAllBCto3BC::makeTgcPrepData(tgcColItr, bcBitMap);
          newPrepData->setHashAndIndex(collection->identifyHash(), collection->size());
          collection->push_back(newPrepData);
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode Muon::TgcPrepDataReplicationTool3BCtoAllBC::queryInterface(const InterfaceID& riid, void** ppvIf) {
  ATH_MSG_DEBUG("queryInterface()");
  if(ITgcPrepDataReplicationTool::interfaceID().versionMatch(riid)) {
    *ppvIf = dynamic_cast<ITgcPrepDataReplicationTool*>(this);
    addRef();
    ATH_MSG_DEBUG("InterfaceID successfully matched with ITgcPrepDataReplicationTool one.");
    return StatusCode::SUCCESS;
  }

  return AthAlgTool::queryInterface(riid, ppvIf);
}


