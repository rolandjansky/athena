/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TGC_CnvTool.cxx, (c) ATLAS Detector software
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TgcRdoToPrepDataToolMT.h"

#include "MuonDigitContainer/TgcDigit.h"
#include "MuonTrigCoinData/TgcCoinData.h"


Muon::TgcRdoToPrepDataToolMT::TgcRdoToPrepDataToolMT(const std::string& t, const std::string& n, const IInterface* p)
  : AthAlgTool(t, n, p), 
    TgcRdoToPrepDataToolCore(t, n, p)
{
  declareProperty("TgcPrdContainerCacheKeys", m_prdContainerCacheKeys, "Optional external cache for the TGC PRD container (array)");
  declareProperty("TgcCoinContainerCacheKeys", m_coinContainerCacheKeys, "Optional external cache for the TGC Trigger Coin container (array)");

}  

Muon::TgcRdoToPrepDataToolMT::~TgcRdoToPrepDataToolMT()
{
}

StatusCode Muon::TgcRdoToPrepDataToolMT::initialize()
{
  ATH_MSG_VERBOSE("Starting init");
  ATH_CHECK( TgcRdoToPrepDataToolCore::initialize() );
  ATH_CHECK( m_prdContainerCacheKeys.initialize( !m_prdContainerCacheKeys.keys().empty() ) );
  ATH_CHECK( m_coinContainerCacheKeys.initialize( !m_coinContainerCacheKeys.keys().empty() ) );
  ATH_MSG_DEBUG("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

StatusCode Muon::TgcRdoToPrepDataToolMT::finalize()
{
  return TgcRdoToPrepDataToolCore::finalize();
}

StatusCode Muon::TgcRdoToPrepDataToolMT::decode(std::vector<IdentifierHash>& requestedIdHashVect, std::vector<IdentifierHash>& selectedIdHashVect)
{

  int sizeVectorRequested = requestedIdHashVect.size();
  ATH_MSG_DEBUG("decode for " << sizeVectorRequested << " offline collections called");

  // clear output vector of selected data collections containing data 
  selectedIdHashVect.clear(); 
  
  // This seems redundant in the MT setup
  bool nothingToDo[NBC+1];  
  for(int ibc=0; ibc<NBC+1; ibc++){
    nothingToDo[ibc] = false; 
  }
  bool nothingToDoForAllBC = true;
  for(int ibc=0; ibc<NBC; ibc++) {
    if(!nothingToDo[ibc]) nothingToDoForAllBC = false;
  } 
  // ---
  if(!m_tgcCabling) {
    StatusCode status = getCabling();
    if(!status.isSuccess()) return status; 
  }

  // initialize with false - does not need to be inside IBC loop
  std::fill(m_decodedOnlineId.begin(), m_decodedOnlineId.end(), false);

  /// clean up containers for Hits
  for(int ibc=0; ibc<NBC+1; ibc++) {
    // PRD
    SG::WriteHandle<TgcPrepDataContainer>  handle(m_outputprepdataKeys[ibc]);
    // Caching of PRD container
    const bool externalCachePRD = !m_prdContainerCacheKeys[ibc].key().empty();
    if (!externalCachePRD) {
      // without the cache we just record the container
      StatusCode status = handle.record(std::make_unique<Muon::TgcPrepDataContainer>(m_muonIdHelperTool->tgcIdHelper().module_hash_max()));
      if (status.isFailure() || !handle.isValid() )   {
        ATH_MSG_FATAL("Could not record container of TGC PrepRawData at " << m_outputprepdataKeys[ibc].key());
        return StatusCode::FAILURE;;
      }
      ATH_MSG_DEBUG("Created container " << m_outputprepdataKeys[ibc].key());
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
      ATH_MSG_DEBUG("Created container using cache for " << m_prdContainerCacheKeys[ibc].key());
    }
    
    //true: un-seeded mode (no need to decode this event after this execution)
    //false: seeded mode (still need to decode this event even after this execution) 
    m_fullEventDone[ibc] = sizeVectorRequested==0;

    // Pass the container from the handle
    m_tgcPrepDataContainer[ibc] = handle.ptr();
  }


  /// clean up containers for Coincidence
  for(int ibc=0; ibc<NBC; ibc++) {

    // PRD
    SG::WriteHandle<TgcCoinDataContainer>  handle(m_outputCoinKeys[ibc]);
    // Caching of PRD container
    const bool externalCacheCoin = !m_coinContainerCacheKeys[ibc].key().empty();
    if (!externalCacheCoin) {
      // without the cache we just record the container
      StatusCode status = handle.record(std::make_unique<Muon::TgcCoinDataContainer>(m_muonIdHelperTool->tgcIdHelper().module_hash_max()));
      if (status.isFailure() || !handle.isValid() )   {
        ATH_MSG_FATAL("Could not record container of TGC CoinData at " << m_outputCoinKeys[ibc].key());
        return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG("Created container " << m_outputCoinKeys[ibc].key());
    } 
    else {
      // use the cache to get the container
      SG::UpdateHandle<TgcCoinDataCollection_Cache> update(m_coinContainerCacheKeys[ibc]);
      if (!update.isValid()){
        ATH_MSG_FATAL("Invalid UpdateHandle " << m_coinContainerCacheKeys[ibc].key());
        return StatusCode::FAILURE;
      }
      StatusCode status = handle.record(std::make_unique<Muon::TgcCoinDataContainer>(update.ptr()));
      if (status.isFailure() || !handle.isValid() )   {
        ATH_MSG_FATAL("Could not record container of TGC CoinData Container using cache " 
          << m_coinContainerCacheKeys[ibc].key() << " - " <<m_outputCoinKeys[ibc].key()); 
        return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG("Created container using cache for " << m_coinContainerCacheKeys[ibc].key());
    }
    // Pass the container from the handle
    m_tgcCoinDataContainer[ibc] = handle.ptr();
  }

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
      std::vector<IdentifierHash>::iterator tgchid   = requestedIdHashVect.begin(); 
      std::vector<IdentifierHash>::iterator tgchid_e = requestedIdHashVect.end(); 
      for(; tgchid!=tgchid_e; tgchid++) { 
        IdentifierHash offlineCollHash = *tgchid; 
        uint16_t onlineId = m_hashToOnlineId.at(static_cast<unsigned int>(offlineCollHash)); 

        if(m_decodedOnlineId.at(onlineId)) { 
          ATH_MSG_DEBUG("The ROB with onlineId " << onlineId << " which contains hash " 
                        << static_cast<unsigned int>(offlineCollHash) 
                        << " is already decoded and skipped"); 
          continue; 
        } 

        m_decodedOnlineId.at(onlineId) = true; // The ROB with this onlineId will be decoded only once 

        TgcRdoContainer::const_iterator rdo_container_it   = rdoContainer->begin(); 
        TgcRdoContainer::const_iterator rdo_container_it_e = rdoContainer->end(); 
        for(; rdo_container_it!=rdo_container_it_e; rdo_container_it++) { 
          const TgcRdo* rdoColl = *rdo_container_it; 
          if(rdoColl->identify()==onlineId) { 
            if(!isAlreadyConverted(rdoCollVec, rdoColl)) { 
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
    
      std::vector<const TgcRdo*>::const_iterator iRdo   = rdoCollVec.begin();
      std::vector<const TgcRdo*>::const_iterator iRdo_e = rdoCollVec.end();
      for(; iRdo!=iRdo_e; iRdo++) {
        TgcRdo::const_iterator itD   = (*iRdo)->begin(); 
        TgcRdo::const_iterator itD_e = (*iRdo)->end();
        for(; itD!=itD_e; itD++) { 
          selectDecoder(itD, (*iRdo));
        }
        m_decodedRdoCollVec.push_back(*iRdo);
      }
      // show the vector of IdentifierHash which contains the data within requested range
      showIdentifierHashVector(selectedIdHashVect);
    } else {
      ATH_MSG_DEBUG("Start loop over rdos - unseeded mode");
    
      TgcRdoContainer::const_iterator rdoColli   = rdoContainer->begin();
      TgcRdoContainer::const_iterator rdoColli_e = rdoContainer->end();
      for(; rdoColli!=rdoColli_e; rdoColli++) {
        // loop over all elements of the rdo container 
        const TgcRdo* rdoColl = *rdoColli;

        if(rdoColl->size()>0 && !isAlreadyConverted(rdoCollVec, rdoColl)) {
          ATH_MSG_DEBUG(" Number of RawData in this rdo " << rdoColl->size());
          TgcRdo::const_iterator itD   = rdoColl->begin(); 
          TgcRdo::const_iterator itD_e = rdoColl->end();
          for(; itD!=itD_e; itD++) { 
            selectDecoder(itD, rdoColl);
          }
          m_decodedRdoCollVec.push_back(rdoColl);
        }
      }
      // show the vector of IdentifierHash which contains the data
      showIdentifierHash();
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
      if(isIdentifierHashFoundInAnyTgcPrepDataContainer(hash) || isIdentifierHashFoundInAnyTgcCoinDataContainer(hash)) { 
        if(find(selectedIdHashVect.begin(), selectedIdHashVect.end(), hash)==selectedIdHashVect.end()) { 
          // Avoid duplication 
          selectedIdHashVect.push_back(hash); 
        } 
      } 
    } 
  } else { // Un-seeded mode 
    // Fill the hashes with hits or coincidences 
    fillIdentifierHashVector(selectedIdHashVect); 
  } 

  return StatusCode::SUCCESS;
}
