/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// TgcPrepDataReplicationTool.cxx, (c) ATLAS Detector software
////////////////////////////////////////////////////////////////////////////////

#include "TgcPrepDataReplicationTool.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonCnvToolInterfaces/IDC_Helper.h"

#include "MuonDigitContainer/TgcDigit.h"
#include "EventPrimitives/EventPrimitives.h"

//================ Constructor =================================================
Muon::TgcPrepDataReplicationTool::TgcPrepDataReplicationTool
  (const std::string& t, const std::string& n, const IInterface* p)
  : AthAlgTool(t, n, p),
  m_muonMgr(0),
  m_tgcHelper(0)
{
  for(int ibc = 0; ibc < BC_NUM; ibc++) m_tgcPrepDataContainer[ibc] = 0;
}  


//================ Destructor ==================================================
Muon::TgcPrepDataReplicationTool::~TgcPrepDataReplicationTool()
{}


//================ Initialization ==============================================
StatusCode Muon::TgcPrepDataReplicationTool::initialize()
{
  StatusCode sc = AthAlgTool::initialize();
  if(sc.isFailure()) return sc;

  sc = detStore()->retrieve(m_muonMgr);
  if(sc.isFailure()) {
    ATH_MSG_FATAL("Cannot retrieve MuonDetectorManager");
    return sc;
  }

  /// get tgcIdHelper from muonMgr
  m_tgcHelper = m_muonMgr->tgcIdHelper();

  /// create an empty TGC container for filling
  for(int ibc = 0; ibc < BC_NUM; ibc++) {
    try {
      m_tgcPrepDataContainer[ibc] = new TgcPrepDataContainer(m_tgcHelper->module_hash_max());
    } catch(const std::bad_alloc&) {
      ATH_MSG_FATAL("Could not create a new TGC PrepRawData container!");
      return StatusCode::FAILURE;
    }
      m_tgcPrepDataContainer[ibc]->addRef();
  }

  return StatusCode::SUCCESS;
}


//================ Finalization ================================================
StatusCode Muon::TgcPrepDataReplicationTool::finalize()
{
  for(int ibc = 0; ibc < BC_NUM; ibc++) {
    m_tgcPrepDataContainer[ibc]->release();
  }

  StatusCode sc = AthAlgTool::finalize();

  return sc;
}


//================ Decoding ====================================================
StatusCode Muon::TgcPrepDataReplicationTool::replicate()
{
  bool hasContainer[BC_NUM] = {false}; 
  ATH_MSG_VERBOSE("replicate");

  for(int ibc = 0; ibc < BC_NUM; ibc++) {
    std::ostringstream location;
    location << "TGC_Measurements" 
             << (ibc == BC_PREVIOUS ? "PriorBC" : "")                      
             << (ibc == BC_NEXT ? "NextBC" : "") 
             << (ibc == BC_ALL ? "AllBCs" : "");
    if(evtStore()->contains<Muon::TgcPrepDataContainer>(location.str())) 
      hasContainer[ibc] = true;
  }

  if (hasContainer[BC_ALL] && (!hasContainer[BC_PREVIOUS])
       && (!hasContainer[BC_CURRENT]) && (!hasContainer[BC_NEXT])) {
    StatusCode sc = convertAllBCto3BC();
    if(sc.isFailure()) return sc;
  }

  if (!hasContainer[BC_ALL] && hasContainer[BC_PREVIOUS]
       && hasContainer[BC_CURRENT] && hasContainer[BC_NEXT]) {
    StatusCode sc = convert3BCtoAllBC();
    if(sc.isFailure()) return sc;
  }

  return StatusCode::SUCCESS;
}


StatusCode Muon::TgcPrepDataReplicationTool::convertAllBCto3BC()
{

  const Muon::TgcPrepDataContainer* tgcAll = 0;

  StatusCode sc = evtStore()->retrieve(tgcAll, "TGC_MeasurementsAllBCs");
  if(sc.isFailure()) {
    ATH_MSG_FATAL("Cannot retrieve TGC_MeasurementsAllBCs");
    return sc;
  }

  // cleanup
  for (int ibc = 0; ibc < BC_ALL; ibc++) m_tgcPrepDataContainer[ibc]->cleanup();

  // convert
  uint16_t bcBitMap = 0;
  Muon::TgcPrepDataCollection* collections[BC_ALL] = {0};

  Muon::TgcPrepDataContainer::const_iterator tgcAllItr   = tgcAll->begin();
  Muon::TgcPrepDataContainer::const_iterator tgcAllItrE  = tgcAll->end();
  
  for (; tgcAllItr != tgcAllItrE; ++tgcAllItr) {
    Muon::TgcPrepDataCollection::const_iterator tgcAllColItr  = (*tgcAllItr)->begin();
    Muon::TgcPrepDataCollection::const_iterator tgcAllColItrE = (*tgcAllItr)->end();

    for (; tgcAllColItr != tgcAllColItrE; ++tgcAllColItr) {

      bcBitMap = (*tgcAllColItr)->getBcBitMap();
      uint16_t hasBC[BC_NUM] = {0};
      if (bcBitMap & TgcPrepData::BCBIT_PREVIOUS) 
        hasBC[BC_PREVIOUS] = TgcPrepData::BCBIT_PREVIOUS;
      if (bcBitMap & TgcPrepData::BCBIT_CURRENT)  
        hasBC[BC_CURRENT] = TgcPrepData::BCBIT_CURRENT;
      if (bcBitMap & TgcPrepData::BCBIT_NEXT)     
        hasBC[BC_NEXT] = TgcPrepData::BCBIT_NEXT;

      Identifier channelId = (*tgcAllColItr)->identify();
      Identifier elementId = m_tgcHelper->elementID(channelId);

      for (int ibc = 0; ibc < BC_ALL; ibc++) {
        collections[ibc] = Muon::IDC_Helper::getCollection<TgcPrepDataContainer, TgcIdHelper>
                            (elementId, m_tgcPrepDataContainer[ibc], m_tgcHelper, msg());
     
        if (!hasBC[ibc]) continue;
        Muon::TgcPrepData* newPrepData = makeTgcPrepData(tgcAllColItr, hasBC[ibc]);
        newPrepData->setHashAndIndex(collections[ibc]->identifyHash(), collections[ibc]->size());
        collections[ibc]->push_back(newPrepData);
      }
    }
  }

  // record
  for (int ibc = 0; ibc < BC_ALL; ibc++) {
    std::ostringstream location;
    location << "TGC_Measurements" 
             << (ibc == BC_PREVIOUS ? "PriorBC" : "")                      
             << (ibc == BC_NEXT ? "NextBC" : ""); 

    sc = evtStore()->record(m_tgcPrepDataContainer[ibc], location.str());
    if(sc.isFailure()) {
      ATH_MSG_FATAL("Could not record container of TGC PrepRawData at " << location.str());
      return sc;
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode Muon::TgcPrepDataReplicationTool::convert3BCtoAllBC()
{
  // cleanup
  m_tgcPrepDataContainer[BC_ALL]->cleanup();

  // convert
  const Muon::TgcPrepDataContainer* tgc3BCs[BC_ALL] = {0};
  Muon::TgcPrepDataCollection* collection = 0;

  for (int ibc = 0; ibc < BC_ALL; ibc++) {
    uint16_t bcBitMap = 0;
    if (ibc == BC_PREVIOUS)     bcBitMap = TgcPrepData::BCBIT_PREVIOUS;
    else if (ibc == BC_CURRENT) bcBitMap = TgcPrepData::BCBIT_CURRENT;
    else if (ibc == BC_NEXT)    bcBitMap = TgcPrepData::BCBIT_NEXT;

    std::ostringstream location;
    location << "TGC_Measurements" 
             << (ibc == BC_PREVIOUS ? "PriorBC" : "")                      
             << (ibc == BC_NEXT ? "NextBC" : ""); 

    StatusCode sc = evtStore()->retrieve(tgc3BCs[ibc], location.str());
    if(sc.isFailure()) {
      ATH_MSG_FATAL("Cannot retrieve " << location.str());
      return sc;
    }

    Muon::TgcPrepDataContainer::const_iterator tgcItr   = tgc3BCs[ibc]->begin();
    Muon::TgcPrepDataContainer::const_iterator tgcItrE  = tgc3BCs[ibc]->end();
  
    for (; tgcItr != tgcItrE; ++tgcItr) {
      Muon::TgcPrepDataCollection::const_iterator tgcColItr  = (*tgcItr)->begin();
      Muon::TgcPrepDataCollection::const_iterator tgcColItrE = (*tgcItr)->end();

      for (; tgcColItr != tgcColItrE; ++tgcColItr) {
        Identifier channelId = (*tgcColItr)->identify();
        Identifier elementId = m_tgcHelper->elementID(channelId);

        collection = Muon::IDC_Helper::getCollection<TgcPrepDataContainer, TgcIdHelper>
                            (elementId, m_tgcPrepDataContainer[BC_ALL], m_tgcHelper, msg());

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
          Muon::TgcPrepData* newPrepData = makeTgcPrepData(tgcColItr, bcBitMap);
          newPrepData->setHashAndIndex(collection->identifyHash(), collection->size());
          collection->push_back(newPrepData);
        }
      }
    }
  }

  // record
  StatusCode sc = evtStore()->record(m_tgcPrepDataContainer[BC_ALL], "TGC_MeasurementsAllBCs");
  if(sc.isFailure()) {
    ATH_MSG_FATAL("Could not record container of TGC PrepRawData at TGC_MeasurementsAllBCs");
    return sc;
  }

  return StatusCode::SUCCESS;
}


Muon::TgcPrepData*
Muon::TgcPrepDataReplicationTool::makeTgcPrepData(Muon::TgcPrepDataCollection::const_iterator itr, uint16_t bcBitMap)
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




StatusCode Muon::TgcPrepDataReplicationTool::queryInterface(const InterfaceID& riid, void** ppvIf) {
  ATH_MSG_DEBUG("queryInterface()");
  if(ITgcPrepDataReplicationTool::interfaceID().versionMatch(riid)) {
    *ppvIf = dynamic_cast<ITgcPrepDataReplicationTool*>(this);
    addRef();
    ATH_MSG_DEBUG("InterfaceID successfully matched with ITgcPrepDataReplicationTool one.");
    return StatusCode::SUCCESS;
  }

  return AthAlgTool::queryInterface(riid, ppvIf);
}
