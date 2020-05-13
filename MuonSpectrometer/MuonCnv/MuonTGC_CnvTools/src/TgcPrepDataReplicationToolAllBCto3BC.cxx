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
  : AthAlgTool(t, n, p),
  m_3BCKeys{"dummy", "dummy", "dummy"},
  m_AllBCKey("TGC_MeasurementsAllBCs")
{
  declareProperty("BC3Keys", m_3BCKeys);
  declareProperty("AllBCKey", m_AllBCKey);
}  

//================ Initialization ==============================================
StatusCode Muon::TgcPrepDataReplicationToolAllBCto3BC::initialize()
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
  ATH_CHECK(m_AllBCKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode Muon::TgcPrepDataReplicationToolAllBCto3BC::replicate()
{
    return convertAllBCto3BC();
}

StatusCode Muon::TgcPrepDataReplicationToolAllBCto3BC::convertAllBCto3BC()
{

  SG::ReadHandle<TgcPrepDataContainer> tgcAll(m_AllBCKey);
 
  if(!tgcAll.isValid()) {
    ATH_MSG_FATAL("Cannot retrieve TGC_MeasurementsAllBCs");
    return StatusCode::FAILURE;
  }


  // convert
  auto tgc3BCHandles = m_3BCKeys.makeHandles();
  for (int ibc = 0; ibc < BC_ALL; ibc++){
      tgc3BCHandles.at(ibc) = std::unique_ptr<TgcPrepDataContainer>( new TgcPrepDataContainer(m_idHelperSvc->tgcIdHelper().module_hash_max()) );
  }

  Muon::TgcPrepDataContainer::const_iterator tgcAllItr   = tgcAll->begin();
  Muon::TgcPrepDataContainer::const_iterator tgcAllItrE  = tgcAll->end();
  
  for (; tgcAllItr != tgcAllItrE; ++tgcAllItr) {
    Muon::TgcPrepDataCollection::const_iterator tgcAllColItr  = (*tgcAllItr)->begin();
    Muon::TgcPrepDataCollection::const_iterator tgcAllColItrE = (*tgcAllItr)->end();

    for (; tgcAllColItr != tgcAllColItrE; ++tgcAllColItr) {

      uint16_t bcBitMap = (*tgcAllColItr)->getBcBitMap();
      uint16_t hasBC[BC_NUM] = {0};
      if (bcBitMap & TgcPrepData::BCBIT_PREVIOUS) 
        hasBC[BC_PREVIOUS] = TgcPrepData::BCBIT_PREVIOUS;
      if (bcBitMap & TgcPrepData::BCBIT_CURRENT)  
        hasBC[BC_CURRENT] = TgcPrepData::BCBIT_CURRENT;
      if (bcBitMap & TgcPrepData::BCBIT_NEXT)     
        hasBC[BC_NEXT] = TgcPrepData::BCBIT_NEXT;

      Identifier channelId = (*tgcAllColItr)->identify();
      Identifier elementId = m_idHelperSvc->tgcIdHelper().elementID(channelId);
      std::array<Muon::TgcPrepDataCollection*, BC_ALL> collections;
      for (int ibc = 0; ibc < BC_ALL; ibc++) {
        collections[ibc] = Muon::IDC_Helper::getCollection<TgcPrepDataContainer, TgcIdHelper>
                            (elementId, tgc3BCHandles[ibc].ptr(), m_idHelperSvc->tgcIdHelper(), msg());
     
        if (!hasBC[ibc]) continue;
        Muon::TgcPrepData* newPrepData = makeTgcPrepData(tgcAllColItr, hasBC[ibc]);
        newPrepData->setHashAndIndex(collections[ibc]->identifyHash(), collections[ibc]->size());
        collections[ibc]->push_back(newPrepData);
      }
    }
  }


  return StatusCode::SUCCESS;
}


Muon::TgcPrepData*
Muon::TgcPrepDataReplicationToolAllBCto3BC::makeTgcPrepData(Muon::TgcPrepDataCollection::const_iterator itr, uint16_t bcBitMap)
{
  Identifier channelId = (*itr)->identify();
  IdentifierHash tgcHashId = (*itr)->collectionHash();
  const std::vector<Identifier> &identifierList = (*itr)->rdoList();
  const Amg::MatrixX* errHitPos = &(*itr)->localCovariance();
  const MuonGM::TgcReadoutElement* descriptor = (*itr)->detectorElement();

  const Amg::MatrixX* newErrHitPos = new Amg::MatrixX(*errHitPos);

  Muon::TgcPrepData* newPrepData = new TgcPrepData(channelId, tgcHashId, (*itr)->localPosition(),
                                                   identifierList, newErrHitPos, descriptor);
  newPrepData->setBcBitMap(bcBitMap);

  return newPrepData;
}




StatusCode Muon::TgcPrepDataReplicationToolAllBCto3BC::queryInterface(const InterfaceID& riid, void** ppvIf) {
  ATH_MSG_DEBUG("queryInterface()");
  if(ITgcPrepDataReplicationTool::interfaceID().versionMatch(riid)) {
    *ppvIf = dynamic_cast<ITgcPrepDataReplicationTool*>(this);
    addRef();
    ATH_MSG_DEBUG("InterfaceID successfully matched with ITgcPrepDataReplicationTool one.");
    return StatusCode::SUCCESS;
  }

  return AthAlgTool::queryInterface(riid, ppvIf);
}


