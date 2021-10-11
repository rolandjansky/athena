/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TgcRdoToPrepDataToolMT.h"

#include "MuonDigitContainer/TgcDigit.h"
#include "MuonTrigCoinData/TgcCoinData.h"
#include "MuonCnvToolInterfaces/IDC_Helper.h"
#include "GaudiKernel/ThreadLocalContext.h"

Muon::TgcRdoToPrepDataToolMT::TgcRdoToPrepDataToolMT(const std::string& t, const std::string& n, const IInterface* p)
  : base_class(t, n, p)
{
}  

StatusCode Muon::TgcRdoToPrepDataToolMT::initialize()
{
  ATH_MSG_VERBOSE("Starting init");
  ATH_CHECK( TgcRdoToPrepDataToolCore::initialize() );
  ATH_MSG_DEBUG("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

StatusCode Muon::TgcRdoToPrepDataToolMT::decode(std::vector<IdentifierHash>& requestedIdHashVect, 
  std::vector<IdentifierHash>& selectedIdHashVect) const
{
  // MT version of this method always adds containers. Caching will be added later.

  State state;

  int sizeVectorRequested = requestedIdHashVect.size();
  ATH_MSG_DEBUG("decode for " << sizeVectorRequested << " offline collections called");

  // clear output vector of selected data collections containing data 
  selectedIdHashVect.clear(); 
  
  bool nothingToDo[NBC+1];  
  for(int ibc=0; ibc<NBC+1; ibc++) nothingToDo[ibc] = false; 

  const CablingInfo* cinfo = getCabling();
  if (!cinfo) {
    return StatusCode::FAILURE;
  }
  const ITGCcablingSvc* tgcCabling = cinfo->m_tgcCabling;

  /// clean up containers for Hits
  for(int ibc=0; ibc<NBC+1; ibc++) {      
    // initialize with false  
    SG::WriteHandle<TgcPrepDataContainer>  handle(m_outputprepdataKeys[ibc]);
    
    // record the container in storeGate
    handle = std::unique_ptr<TgcPrepDataContainer> (new TgcPrepDataContainer(m_idHelperSvc->tgcIdHelper().module_hash_max()));
    // cache the pointer, storegate retains ownership
    state.m_tgcPrepDataContainer[ibc] = handle.ptr();
    if(!handle.isValid()) {
      ATH_MSG_FATAL("Could not record container of TGC PrepRawData at " << m_outputprepdataKeys[ibc].key());
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("TGC PrepData Container recorded in StoreGate with key " << m_outputprepdataKeys[ibc].key());
    }
  }
  std::vector<const TgcRdo*> decodedRdoCollVec;
  std::vector<bool> decodedOnlineId (cinfo->m_MAX_N_ROD, false);
  
  // If at least one BC has to do something, nothingToDoForAllBC should be false.  
  bool nothingToDoForAllBC = true;
  for(int ibc=0; ibc<NBC; ibc++) {
    if(!nothingToDo[ibc]) nothingToDoForAllBC = false;
  }
  
  /// clean up containers for Coincidence
  for(int ibc=0; ibc<NBC; ibc++) {
    // this happens the first time in the event !
    SG::WriteHandle<TgcCoinDataContainer>  handle(m_outputCoinKeys[ibc]);
    
    // record the container in storeGate
    handle = std::unique_ptr<TgcCoinDataContainer> (new TgcCoinDataContainer(m_idHelperSvc->tgcIdHelper().module_hash_max()));
    
    // cache the pointer, storegate retains ownership
    state.m_tgcCoinDataContainer[ibc] = handle.ptr();

    if(!handle.isValid()) {
      ATH_MSG_FATAL("Could not record container of TGC CoinData at " << m_outputCoinKeys[ibc].key());
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("TGC CoinData Container recorded in StoreGate with key " << m_outputCoinKeys[ibc].key());
    }
  }

  getHitCollection_func getHitCollection = [&] (Identifier id, int locId) -> TgcPrepDataCollection*
  {
    TgcPrepDataCollection*& mappedCollection = state.m_tgcPrepDataCollections[locId][id];
    if (!mappedCollection) {
      mappedCollection = Muon::IDC_Helper::addCollection<TgcPrepDataContainer, TgcIdHelper>
        (id, state.m_tgcPrepDataContainer[locId], m_idHelperSvc->tgcIdHelper(), msg());
    }
    return mappedCollection;
  };
  getCoinCollection_func getCoinCollection = [&] (Identifier id, int locId) -> TgcCoinDataCollection*
  {
    TgcCoinDataCollection*& mappedCollection = state.m_tgcCoinDataCollections[locId][id];
    if (!mappedCollection) {
      mappedCollection = Muon::IDC_Helper::addCollection<TgcCoinDataContainer, TgcIdHelper>
        (id, state.m_tgcCoinDataContainer[locId], m_idHelperSvc->tgcIdHelper(), msg());
    }
    return mappedCollection;
  };

  if(!nothingToDoForAllBC) { // If still need to do something 
    // if TGC decoding is switched off stop here
    if(!m_decodeData) {
      ATH_MSG_DEBUG("Stored empty container. Decoding TGC RDO into TGC PrepRawData is switched off");
      return StatusCode::SUCCESS;
    }

    ATH_MSG_DEBUG("Decoding TGC RDO into TGC PrepRawData");

    // retrieve the collection of RDO
    ATH_MSG_DEBUG("Retriving TGC RDO container from the store");
    auto rdoContainer = SG::makeHandle(m_rdoContainerKey);
    if(!rdoContainer.isValid()) {
      ATH_MSG_WARNING("No TGC RDO container in StoreGate!");
      return StatusCode::SUCCESS;
    }

    ///////////// here the RDO container is retrieved and filled -whatever input type we start with- => check the size 
    if(rdoContainer->size()==0) { 
      // empty csm container - no tgc rdo in this event
      ATH_MSG_DEBUG("Empty rdo container - no tgc rdo in this event");
      return StatusCode::SUCCESS;
    }

    ATH_MSG_DEBUG("Not empty rdo container in this event, the container size is " << 
		  rdoContainer->size());
  
    // select RDOs to be decoded when seeded mode is used  
    std::vector<const TgcRdo*> rdoCollVec; 
    if(sizeVectorRequested!=0) { 
      unsigned int nRdo = 0;
      for (IdentifierHash offlineCollHash : requestedIdHashVect) {
        uint16_t onlineId = cinfo->m_hashToOnlineId.at(static_cast<unsigned int>(offlineCollHash)); 

        if(decodedOnlineId.at(onlineId)) { 
          ATH_MSG_DEBUG("The ROB with onlineId " << onlineId << " which contains hash " 
                        << static_cast<unsigned int>(offlineCollHash) 
                        << " is already decoded and skipped"); 
          continue; 
        } 

        decodedOnlineId.at(onlineId) = true; // The ROB with this onlineId will be decoded only once 

        TgcRdoContainer::const_iterator rdo_container_it   = rdoContainer->begin(); 
        TgcRdoContainer::const_iterator rdo_container_it_e = rdoContainer->end(); 
        for(; rdo_container_it!=rdo_container_it_e; ++rdo_container_it) { 
          const TgcRdo* rdoColl = *rdo_container_it; 
          if(rdoColl->identify()==onlineId) { 
            if(!isAlreadyConverted(decodedRdoCollVec, rdoCollVec, rdoColl)) { 
              rdoCollVec.push_back(rdoColl); 
              nRdo++; 
            } 
            break; 
          } 
        } 
      } 
      ATH_MSG_DEBUG("Number of RDOs to be converted is " << nRdo); 
    } // End of selection of RDOs to be decoded

    // Decode Hits
    if(sizeVectorRequested!=0) {
      ATH_MSG_DEBUG("Start loop over rdos - seeded mode");

      for (const TgcRdo* rdo : rdoCollVec) {
        for (const TgcRawData* rd : *rdo) {
	  //Since OnlineIds are not unique, need some additional filtering on offline hashId 
	  //to avoid decoding RDO outside of an RoI
	  Identifier offlineId;
	  IdentifierHash tgcHashId;
	  IdContext tgcContext = m_idHelperSvc->tgcIdHelper().module_context();

	  if(tgcCabling->getElementIDfromReadoutID(offlineId, rd->subDetectorId(), rd->rodId(), rd->sswId(), rd->slbId(), rd->bitpos())){
	    if(m_idHelperSvc->tgcIdHelper().get_hash(offlineId, tgcHashId, &tgcContext)!=1){
	      if(std::find(requestedIdHashVect.begin(), requestedIdHashVect.end(), tgcHashId) != requestedIdHashVect.end()){
		selectDecoder(getHitCollection, getCoinCollection,
                              *rd, rdo);
	      }
	    }
	  }
        }
        decodedRdoCollVec.push_back(rdo);
      }
      // show the vector of IdentifierHash which contains the data within requested range
      showIdentifierHashVector(state, selectedIdHashVect);
    } else {
      ATH_MSG_DEBUG("Start loop over rdos - unseeded mode");
    
      TgcRdoContainer::const_iterator rdoColli   = rdoContainer->begin();
      TgcRdoContainer::const_iterator rdoColli_e = rdoContainer->end();
      for(; rdoColli!=rdoColli_e; ++rdoColli) {
        // loop over all elements of the rdo container 
        const TgcRdo* rdoColl = *rdoColli;

        if(rdoColl->size()>0 && !isAlreadyConverted(decodedRdoCollVec, rdoCollVec, rdoColl)) {
          ATH_MSG_DEBUG(" Number of RawData in this rdo " << rdoColl->size());
          for (const TgcRawData* rd : *rdoColl) {
            selectDecoder(getHitCollection, getCoinCollection,
                          *rd, rdoColl);
          }
          decodedRdoCollVec.push_back(rdoColl);
        }
      }
      // show the vector of IdentifierHash which contains the data
      showIdentifierHash(state);
    }
  }

  // Fill the hashes with hits or coincidences 
  if(sizeVectorRequested!=0) { // Seeded mode 
    // Add requestedIdHashVect (input) hashes which have hit or coincidence PRDs to selectedIdHashVect (output) 
    // RDO collection is with granularity of a sector (there are 24 sectors in total) 
    // PRD collection is with granularity of a chamber (there are 1578 chambers in total) 
    // One chamber has one IdentifierHash (0 to 1577) 
    for(int iHash=0; iHash<sizeVectorRequested; iHash++) { 
      IdentifierHash hash = requestedIdHashVect.at(iHash); 
      if(isIdentifierHashFoundInAnyTgcPrepDataContainer(state, hash) || isIdentifierHashFoundInAnyTgcCoinDataContainer(state, hash)) { 
        if(find(selectedIdHashVect.begin(), selectedIdHashVect.end(), hash)==selectedIdHashVect.end()) { 
          // Avoid duplication 
          selectedIdHashVect.push_back(hash); 
        } 
      } 
    } 
  } else { // Un-seeded mode 
    // Fill the hashes with hits or coincidences 
    fillIdentifierHashVector(state, selectedIdHashVect); 
  } 

  return StatusCode::SUCCESS;
}

void Muon::TgcRdoToPrepDataToolMT::printPrepData() const
{
  const EventContext& ctx = Gaudi::Hive::currentContext();

  const TgcPrepDataContainer* tgcPrepDataContainer[NBC+1] = {nullptr};
  for(int ibc=0; ibc<NBC+1; ibc++) {
    SG::ReadHandleKey<TgcPrepDataContainer> k (m_outputprepdataKeys[ibc].key());
    k.initialize().ignore();
    tgcPrepDataContainer[ibc] = SG::makeHandle(k, ctx).get();
  }

  const TgcCoinDataContainer* tgcCoinDataContainer[NBC] = {nullptr};
  for(int ibc=0; ibc<NBC; ibc++) {
    SG::ReadHandleKey<TgcCoinDataContainer> k (m_outputCoinKeys[ibc].key());
    k.initialize().ignore();
    tgcCoinDataContainer[ibc] = SG::makeHandle(k, ctx).get();
  }

  printPrepDataImpl (tgcPrepDataContainer, tgcCoinDataContainer);
}
