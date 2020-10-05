/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TgcRdoToPrepDataTool.h"

#include "MuonDigitContainer/TgcDigit.h"
#include "MuonTrigCoinData/TgcCoinData.h"

Muon::TgcRdoToPrepDataTool::TgcRdoToPrepDataTool(const std::string& t, const std::string& n, const IInterface* p)
  : AthAlgTool(t, n, p), 
    TgcRdoToPrepDataToolCore(t, n, p)
{
}  

StatusCode Muon::TgcRdoToPrepDataTool::initialize()
{
  ATH_MSG_VERBOSE("Starting init");
  ATH_CHECK( TgcRdoToPrepDataToolCore::initialize() );
  ATH_MSG_DEBUG("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

StatusCode Muon::TgcRdoToPrepDataTool::decode(std::vector<IdentifierHash>& requestedIdHashVect, 
  std::vector<IdentifierHash>& selectedIdHashVect)
{
  int sizeVectorRequested = requestedIdHashVect.size();
  ATH_MSG_DEBUG("decode for " << sizeVectorRequested << " offline collections called");

  // clear output vector of selected data collections containing data 
  selectedIdHashVect.clear(); 
  
  bool nothingToDo[NBC+1];  
  for(int ibc=0; ibc<NBC+1; ibc++) nothingToDo[ibc] = false; 

  if(!m_tgcCabling) {
    StatusCode status = getCabling();
    if(!status.isSuccess()) return status; 
  }

  /// clean up containers for Hits
  for(int ibc=0; ibc<NBC+1; ibc++) {      
    int bcTag=ibc+1;
 
    if(!evtStore()->contains<Muon::TgcPrepDataContainer>(m_outputprepdataKeys.at(ibc).key())) {

     
      // initialize with false  
      std::fill(m_decodedOnlineId.begin(), m_decodedOnlineId.end(), false);
      SG::WriteHandle<TgcPrepDataContainer>  handle(m_outputprepdataKeys[ibc]);
      
      // record the container in storeGate
      handle = std::unique_ptr<TgcPrepDataContainer> (new TgcPrepDataContainer(m_idHelperSvc->tgcIdHelper().module_hash_max()));
      // cache the pointer, storegate retains ownership
      m_tgcPrepDataContainer[ibc] = handle.ptr();
      if(!handle.isValid()) {
	ATH_MSG_FATAL("Could not record container of TGC PrepRawData at " << m_outputprepdataKeys[ibc].key());
	return StatusCode::FAILURE;
      } else {
	ATH_MSG_DEBUG("TGC PrepData Container recorded in StoreGate with key " << m_outputprepdataKeys[ibc].key());
      }

      //true: un-seeded mode (no need to decode this event after this execution)
      //false: seeded mode (still need to decode this event even after this execution) 
      m_fullEventDone[ibc] = sizeVectorRequested==0;

      m_decodedRdoCollVec.clear(); // The information of decoded RDO in the previous event is cleared. 
    }
    else {
      const TgcPrepDataContainer* tgcPrepDataContainer_c = nullptr;
      ATH_CHECK( evtStore()->retrieve (tgcPrepDataContainer_c, m_outputprepdataKeys[ibc].key()) );
      m_tgcPrepDataContainer[ibc] = const_cast<TgcPrepDataContainer*> (tgcPrepDataContainer_c);

      // If you come here, this event is partially or fully decoded.
      ATH_MSG_DEBUG("TGC PrepData Container at " << 
		    (bcTag==TgcDigit::BC_PREVIOUS ? "Previous"
		     :  (bcTag==TgcDigit::BC_NEXT ? "Next" 
                         :  (bcTag==(NBC+1) ? "All" : "Current"))) << 
		    " BC is already in StoreGate");

      if(m_fullEventDone[ibc]) { // un-seeded mode has been used for this event 
        ATH_MSG_DEBUG("Whole events at " << 
		      (bcTag==TgcDigit::BC_PREVIOUS ? "Previous"
		       :  (bcTag==TgcDigit::BC_NEXT ? "Next" 
                           :  (bcTag==(NBC+1) ? "All" : "Current"))) << 
		      " BC have already been decoded; nothing to do");
	nothingToDo[ibc] = true;
	continue;
      }

      if(sizeVectorRequested==0) { // un-seeded mode will be used
	m_fullEventDone[ibc] = true; // no need to decode this event after this execution
      }
    }
  }
  
  // If at least one BC has to do something, nothingToDoForAllBC should be false.  
  bool nothingToDoForAllBC = true;
  for(int ibc=0; ibc<NBC; ibc++) {
    if(!nothingToDo[ibc]) nothingToDoForAllBC = false;
  } 

  /// clean up containers for Coincidence
  for(int ibc=0; ibc<NBC; ibc++) {

    if(!evtStore()->contains<Muon::TgcCoinDataContainer>(m_outputCoinKeys.at(ibc).key())) {
      // this happens the first time in the event !
      SG::WriteHandle<TgcCoinDataContainer>  handle(m_outputCoinKeys[ibc]);
      
      // record the container in storeGate
      handle = std::unique_ptr<TgcCoinDataContainer> (new TgcCoinDataContainer(m_idHelperSvc->tgcIdHelper().module_hash_max()));
      
      // cache the pointer, storegate retains ownership
      m_tgcCoinDataContainer[ibc] = handle.ptr();

      if(!handle.isValid()) {
        ATH_MSG_FATAL("Could not record container of TGC CoinData at " << m_outputCoinKeys[ibc].key());
        return StatusCode::FAILURE;
      } else {
        ATH_MSG_DEBUG("TGC CoinData Container recorded in StoreGate with key " << m_outputCoinKeys[ibc].key());
      }
    } else {
      ATH_MSG_DEBUG("TGC CoinData Container is already in StoreGate");
      const TgcCoinDataContainer* tgcCoinDataContainer_c = nullptr;
      ATH_CHECK( evtStore()->retrieve (tgcCoinDataContainer_c, m_outputCoinKeys[ibc].key()) );
      m_tgcCoinDataContainer[ibc] = const_cast<TgcCoinDataContainer*> (tgcCoinDataContainer_c);
    }
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
	  //Since OnlineIds are not unique, need some additional filtering on offline hashId 
	  //to avoid decoding RDO outside of an RoI
	  Identifier offlineId;
	  IdentifierHash tgcHashId;
	  IdContext tgcContext = m_idHelperSvc->tgcIdHelper().module_context();

	  if(m_tgcCabling->getElementIDfromReadoutID(offlineId, (*itD)->subDetectorId(), (*itD)->rodId(), (*itD)->sswId(), (*itD)->slbId(), (*itD)->bitpos())){
	    if(m_idHelperSvc->tgcIdHelper().get_hash(offlineId, tgcHashId, &tgcContext)){
	      if(std::find(requestedIdHashVect.begin(), requestedIdHashVect.end(), tgcHashId) != requestedIdHashVect.end()){
		selectDecoder(itD, (*iRdo));
	      }
	    }
	  }
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
