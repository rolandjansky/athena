/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TgcRdoToPrepDataToolMT.h"

#include "MuonDigitContainer/TgcDigit.h"
#include "MuonTrigCoinData/TgcCoinData.h"
#include "MuonCnvToolInterfaces/IDC_Helper.h"
#include "GaudiKernel/ThreadLocalContext.h"

Muon::TgcRdoToPrepDataToolMT::TgcRdoToPrepDataToolMT(const std::string& t, const std::string& n, const IInterface* p)
  : base_class(t, n, p),
    m_prdContainerCacheKeys{"","","",""},
    m_coinContainerCacheKeys{"","",""}
{
}  

StatusCode Muon::TgcRdoToPrepDataToolMT::initialize()
{
  ATH_MSG_VERBOSE("Starting init");
  ATH_CHECK( TgcRdoToPrepDataToolCore::initialize() );

  // Build names for the keys same as done for output containers
  if(m_prdContainerCacheKeyStr.value() != ""){
    for(int ibc=0; ibc<NBC+1; ibc++) {      
      int bcTag=ibc+1;
      std::ostringstream location;
      location << m_prdContainerCacheKeyStr.value() << (bcTag==TgcDigit::BC_PREVIOUS ? "PriorBC" : "")
         << (bcTag==TgcDigit::BC_NEXT ? "NextBC" : "") << (bcTag==(NBC+1) ? "AllBCs" : "");    
      m_prdContainerCacheKeys.at(ibc) = location.str();
      ATH_MSG_DEBUG( location.str() );
    }
  }

  if(m_coinContainerCacheKeyStr != ""){
    for(int ibc=0; ibc<NBC; ibc++) {
      int bcTag=ibc+1;
      std::ostringstream location;
      location << m_coinContainerCacheKeyStr << (bcTag==TgcDigit::BC_PREVIOUS ? "PriorBC" : "")
               << (bcTag==TgcDigit::BC_NEXT ? "NextBC" : "");
      m_coinContainerCacheKeys.at(ibc) = location.str();
      ATH_MSG_DEBUG( location.str() );
    }
  }

  // Only initialise if we passed in the cache name
  ATH_CHECK( m_prdContainerCacheKeys.initialize( m_prdContainerCacheKeyStr.value() != "" ) );
  ATH_CHECK( m_coinContainerCacheKeys.initialize( m_coinContainerCacheKeyStr != "" ) );


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

  const CablingInfo* cinfo = getCabling();
  if (!cinfo) {
    return StatusCode::FAILURE;
  }
  const ITGCcablingSvc* tgcCabling = cinfo->m_tgcCabling;

  /// clean up containers for Hits
  for(int ibc=0; ibc<NBC+1; ibc++) {      
    // initialize with false  
    SG::WriteHandle<TgcPrepDataContainer>  handle(m_outputprepdataKeys[ibc]);
    
    const bool externalCachePRD = !m_prdContainerCacheKeys[ibc].key().empty();
    if (!externalCachePRD) {
      // record the container in storeGate
      StatusCode recStatus = handle.record( std::make_unique<TgcPrepDataContainer>(m_idHelperSvc->tgcIdHelper().module_hash_max()) );
      if(recStatus.isFailure() || !handle.isValid()) {
        ATH_MSG_FATAL("Could not record container of TGC PrepRawData at " << m_outputprepdataKeys[ibc].key());
        return StatusCode::FAILURE;
      } else {
        ATH_MSG_DEBUG("TGC PrepData Container recorded in StoreGate with key " << m_outputprepdataKeys[ibc].key());
      }
      // cache the pointer, storegate retains ownership
      state.m_tgcPrepDataContainer[ibc] = handle.ptr();
    }
    else {
      // use the cache to get the container
      SG::UpdateHandle<TgcPrepDataCollection_Cache> update(m_prdContainerCacheKeys[ibc]);
      if (!update.isValid()){
        ATH_MSG_FATAL("Invalid UpdateHandle " << m_prdContainerCacheKeys[ibc].key());
        return StatusCode::FAILURE;
      }
      StatusCode status = handle.record(std::make_unique<Muon::TgcPrepDataContainer>(update.ptr()));
      if (status.isFailure() || !handle.isValid() )   {
        ATH_MSG_FATAL("Could not record container of TGC PrepData Container using cache " 
          << m_prdContainerCacheKeys[ibc].key() << " - " <<m_outputprepdataKeys[ibc].key()); 
        return StatusCode::FAILURE;
      }
      state.m_tgcPrepDataContainer[ibc] = handle.ptr();
      ATH_MSG_DEBUG("Created container using cache for " << m_prdContainerCacheKeys[ibc].key());
    }
  
  }
  std::vector<const TgcRdo*> decodedRdoCollVec;
  std::vector<bool> decodedOnlineId (cinfo->m_MAX_N_ROD, false);
  
  // Could be removed (but creates a big git diff becaues of re-indenting)
  const bool nothingToDoForAllBC = false;
  
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
        // for each RDO collection we collect up all the PRD collections and then write them once filled
        // need a vector because we have the collections for the different bunch crosssings
        std::vector< std::unordered_map< IdentifierHash, std::unique_ptr<TgcPrepDataCollection> > > prdCollectionMap(NBC);
        for (const TgcRawData* rd : *rdo) {
	  //Since OnlineIds are not unique, need some additional filtering on offline hashId 
	  //to avoid decoding RDO outside of an RoI
	  Identifier offlineId;
	  IdentifierHash tgcHashId;
	  IdContext tgcContext = m_idHelperSvc->tgcIdHelper().module_context();

	  if(tgcCabling->getElementIDfromReadoutID(offlineId, rd->subDetectorId(), rd->rodId(), rd->sswId(), rd->slbId(), rd->bitpos())){
	    if(m_idHelperSvc->tgcIdHelper().get_hash(offlineId, tgcHashId, &tgcContext)!=1){
	      if(std::find(requestedIdHashVect.begin(), requestedIdHashVect.end(), tgcHashId) != requestedIdHashVect.end()){
		selectDecoder(state, getCoinCollection,
                              *rd, rdo, prdCollectionMap);
	      }
	    }
	  }
        }
        decodedRdoCollVec.push_back(rdo);
        // now write all the PRD collections
        for(unsigned int ibc = 0; ibc < prdCollectionMap.size(); ++ibc) {
          ATH_MSG_DEBUG("Process collection map for BC " << ibc);
          auto& thismap = prdCollectionMap[ibc];
          for(auto& mapPair : thismap) {
            IdentifierHash tgcHashId = mapPair.first;
            if(tgcHashId.is_valid()) { 
              TgcPrepDataContainer::IDC_WriteHandle lock = state.m_tgcPrepDataContainer[ibc]->getWriteHandle(tgcHashId);
              if (!lock.alreadyPresent()) {
                ATH_MSG_DEBUG("Calling addOrDelete " << ibc << " " << tgcHashId);
                ATH_CHECK(lock.addOrDelete(std::move(mapPair.second)));
              } else ATH_MSG_DEBUG("lock.alreadyPresent for " << ibc << " " << tgcHashId);
            } else{
              ATH_MSG_ERROR("Invalid hash ID when trying to write, this should not happen");
            }
            
          }// loop on collections in the map
        }//loop on BC
      

      }// loop on RDO collections


      

      // show the vector of IdentifierHash which contains the data within requested range
      showIdentifierHashVector(state, selectedIdHashVect);
    } else {
      ATH_MSG_DEBUG("Start loop over rdos - unseeded mode");
    
      TgcRdoContainer::const_iterator rdoColli   = rdoContainer->begin();
      TgcRdoContainer::const_iterator rdoColli_e = rdoContainer->end();
      for(; rdoColli!=rdoColli_e; ++rdoColli) {
        // loop over all elements of the rdo container 
        const TgcRdo* rdoColl = *rdoColli;
        std::vector< std::unordered_map< IdentifierHash, std::unique_ptr<TgcPrepDataCollection> > > prdCollectionMap(NBC);
        if(rdoColl->size()>0 && !isAlreadyConverted(decodedRdoCollVec, rdoCollVec, rdoColl)) {
          ATH_MSG_DEBUG(" Number of RawData in this rdo " << rdoColl->size());
          for (const TgcRawData* rd : *rdoColl) {
            selectDecoder(state, getCoinCollection,
                          *rd, rdoColl, prdCollectionMap);
          }
          decodedRdoCollVec.push_back(rdoColl);
        }

        // now write all the PRD collections
        for(unsigned int ibc = 0; ibc < prdCollectionMap.size(); ++ibc) {
          auto& thismap = prdCollectionMap[ibc];
          for(auto& mapPair : thismap) {
            IdentifierHash tgcHashId = mapPair.first;
            if(tgcHashId.is_valid()) {
              TgcPrepDataContainer::IDC_WriteHandle lock = state.m_tgcPrepDataContainer[ibc]->getWriteHandle(tgcHashId);
              if (!lock.alreadyPresent()) {
                ATH_MSG_DEBUG("Call addOrDelete for " << ibc << " tgcHashId: " << tgcHashId << " size = " << mapPair.second->size());
                ATH_CHECK(lock.addOrDelete(std::move(mapPair.second)));
              }
            } else{
              ATH_MSG_ERROR("Invalid hash ID when trying to write, this should not happen");
            }
          }//loop on collections in the map        
        }//loop on BC

      }//RDO collection loop
      // show the vector of IdentifierHash which contains the data
      showIdentifierHash(state);
    }//unseeded mode
  }

  // Need to create the collection for all BC, which can be done from the collections we have just written

  // first collect up all the HashIDs in any of the containers
  std::set<IdentifierHash> hashesInAnyBC;
  for(unsigned int ibc = 0; ibc < NBC; ++ibc) {
    const TgcPrepDataContainer* prdCont = state.m_tgcPrepDataContainer[ibc];
    const std::vector<IdentifierHash> theseHashes = prdCont->GetAllCurrentHashes();
    hashesInAnyBC.insert(theseHashes.begin(), theseHashes.end());
  }
  ATH_MSG_DEBUG("Found " << hashesInAnyBC.size() << " hashes that must be added to AllBC container");
  // Now loop on all Hash IDs and fill the AllBC container
  for(const auto& tgcHashId : hashesInAnyBC) {
    TgcPrepDataContainer::IDC_WriteHandle lock = state.m_tgcPrepDataContainer[NBC]->getWriteHandle(tgcHashId);
    if(!lock.alreadyPresent()) {
      // collection not there, need to fill it
      std::unique_ptr<TgcPrepDataCollection> collAllBc = std::make_unique<TgcPrepDataCollection>(tgcHashId);
      // look over the collections in the different BC
      for(unsigned int ibc = 0; ibc < NBC; ++ibc) {
        const TgcPrepDataCollection* coll = state.m_tgcPrepDataContainer[ibc]->indexFindPtr(tgcHashId);
        if( coll != nullptr) { // can be null as not necessarily present in every BC
          collAllBc->setIdentifier(coll->identify());
          for(auto prd : *coll) { // loop on PRD in this collection
            TgcPrepData* prdToUpdate = nullptr;
            for(auto prdAllBc : *collAllBc) { // find matching PRD in the AllBC collection
              if( prd->identify() == prdAllBc->identify() ){
                prdToUpdate = prdAllBc;
                break;
              }// matching PRD                        
            }// loop on All BC PRDs
            if(!prdToUpdate) {
              // No PRD exists in AllBC collection, so make a new one from clone of one in the other containers
              ATH_MSG_DEBUG("Adding new TgcPrepData to AllBC collection");
              TgcPrepData* prdAllBc = new TgcPrepData(*prd);
              prdAllBc->setHashAndIndex(collAllBc->identifyHash(), collAllBc->size());
              collAllBc->push_back( prdAllBc ); // collection takes ownership
              prdToUpdate = prdAllBc;
            }
            if(prdToUpdate) { 
              ATH_MSG_DEBUG("Updating TgcPrepData in AllBC collection");
              // Update the bit map for the BC label
              uint16_t bcBitMap = 0;
              if(ibc == 0) bcBitMap = TgcPrepData::BCBIT_PREVIOUS;
              else if (ibc == 1) bcBitMap = TgcPrepData::BCBIT_CURRENT;
              else if (ibc == 2) bcBitMap = TgcPrepData::BCBIT_NEXT;
              const uint16_t bcBitMap_current = prdToUpdate->getBcBitMap();
              ATH_MSG_VERBOSE("Old bitmap " << bcBitMap_current << " adding " << bcBitMap << " to get " << (bcBitMap_current | bcBitMap));
              prdToUpdate->setBcBitMap( (bcBitMap_current | bcBitMap) );
            } else ATH_MSG_ERROR("prdToUpdate is 0, this should not happen");
          }//loop on PRDs
        }// non-zero collection
      }// loop on BC
      ATH_MSG_DEBUG("Calling addOrDelete for AllBC colelction " << tgcHashId);
      ATH_CHECK(lock.addOrDelete(std::move(collAllBc)));
    }// !lock.alreadyPresent
    else ATH_MSG_DEBUG("AllBC Collection already present, no need to create it");
  }//loop on all identifier hashes

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
