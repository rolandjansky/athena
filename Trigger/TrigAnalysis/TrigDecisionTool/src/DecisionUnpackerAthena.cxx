/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//only in full Athena
#if !defined(XAOD_STANDALONE) && !defined(XAOD_ANALYSIS)



#ifndef XAOD_ANALYSIS
#include "TrigSteeringEvent/Lvl1Result.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrigNavStructure/TrigNavStructure.h"
#include "TrigDecisionTool/DecisionUnpackerAthena.h"
#include "TrigDecisionTool/DecisionObjectHandleAthena.h"
#include "TrigNavigation/NavigationCore.h"

namespace Trig {
  DecisionUnpackerAthena::DecisionUnpackerAthena(SG::ReadHandleKey<TrigDec::TrigDecision>* olddeckey) : m_handle(new DecisionObjectHandleAthena(olddeckey)){
  }

  DecisionUnpackerAthena::~DecisionUnpackerAthena(){
    delete m_handle;
  }


  StatusCode DecisionUnpackerAthena::unpackItems(const LVL1CTP::Lvl1Result& result,std::map<unsigned, LVL1CTP::Lvl1Item*>& itemsCache, std::unordered_map<std::string, const LVL1CTP::Lvl1Item*>& itemsByName) {
    itemsByName.reserve( itemsByName.size() + itemsCache.size() );
    for ( auto cacheIt = itemsCache.begin() ; cacheIt != itemsCache.end(); ++cacheIt ) {
      unsigned int ctpid = cacheIt->first;
      LVL1CTP::Lvl1Item* item = cacheIt->second;
      ATH_MSG_VERBOSE("Unpacking bits for item: " << ctpid << " " << item->name());
      bool passBP = result.isPassedBeforePrescale(ctpid);
      bool passAP = result.isPassedAfterPrescale(ctpid);
      bool passAV = result.isPassedAfterVeto(ctpid);
      LVL1CTP::Lvl1Item itemNew (item->name(), item->hashId(),
                                 passBP, passAP, passAV,
                                 item->prescaleFactor());
      *item = std::move (itemNew);
      itemsByName[item->name()] = item;
    }
    return StatusCode::SUCCESS;
  }


  StatusCode DecisionUnpackerAthena::unpackChains(const std::vector<uint32_t>& serialized_chains,
						   std::map<unsigned, HLT::Chain*>& cache,
						   std::unordered_map<std::string, const HLT::Chain*>& output) {
   
    if( serialized_chains.size() == 0 ) {
      ATH_MSG_WARNING("ChainResult is empty");
      return StatusCode::FAILURE;
    }

    output.reserve( output.size() + cache.size() );

    std::vector<uint32_t>::const_iterator rawIt = serialized_chains.begin();
    ++rawIt; // skip first number as it is count
    for ( ; rawIt != serialized_chains.end(); ++rawIt ) {

      unsigned cntr = HLT::Chain::inquireChainCounter(*rawIt);

      // localte now the chain
      auto cacheIt = cache.find(cntr);
      if ( cacheIt == cache.end() ) {
        ATH_MSG_WARNING("Missing chain of counter in the configuration: " << cntr);
        return StatusCode::FAILURE;
      } else {
        cacheIt->second->reset();
        cacheIt->second->deserialize(*rawIt);
        output[cacheIt->second->getChainName()] = cacheIt->second;
        ATH_MSG_VERBOSE("Updated chain in this event : " << *(cacheIt->second));
      }
    }
    return StatusCode::SUCCESS;
  }

  StatusCode DecisionUnpackerAthena::unpackDecision(std::unordered_map<std::string, const LVL1CTP::Lvl1Item*>& itemsByName,
						    std::map<CTPID, LVL1CTP::Lvl1Item*>& itemsCache,
						    std::unordered_map<std::string, const HLT::Chain*>& l2chainsByName,
						    std::map<CHAIN_COUNTER, HLT::Chain*>& l2chainsCache,
						    std::unordered_map<std::string, const HLT::Chain*>& efchainsByName,
						    std::map<CHAIN_COUNTER, HLT::Chain*>& efchainsCache,
						    char& bgCode,
						    bool unpackHLT
						    ){
    
    const TrigDec::TrigDecision* dec = m_handle->getDecision();

    bgCode = dec->BGCode();

    // L1 items
    itemsByName.clear();
    ATH_MSG_DEBUG("Unpacking of L1 items");
    if( unpackItems(dec->getL1Result(),itemsCache,itemsByName).isFailure() ) {
      ATH_MSG_WARNING("Unpacking  of L1 items failed");
    }
  
 
    // protect from unpacking in case HLT was not run (i.e. configuration chains are 0)
    if (!unpackHLT) return StatusCode::SUCCESS;


    // L2 chains
    l2chainsByName.clear();
    ATH_MSG_DEBUG("Unpacking of L2 chains");
    if ( dec->getL2Result().getHLTLevel() != HLT::UNKNOWN ) {
      const std::vector<uint32_t>& l2_serialized_chains = dec->getL2Result().getChainResult();
      ATH_MSG_DEBUG(l2_serialized_chains.size() << " L2 chains");
    
      if ( unpackChains(l2_serialized_chains, l2chainsCache, l2chainsByName).isFailure() ) {
        ATH_MSG_WARNING("Unpacking  of L2 chains failed");
      }
    }
  
    // EF chains
    ATH_MSG_DEBUG("Unpacking of EF/HLT chains");
    efchainsByName.clear();
    const std::vector<uint32_t>& ef_serialized_chains = dec->getEFResult().getChainResult();
    ATH_MSG_DEBUG(ef_serialized_chains.size() << " EF/HLT chains");
  
    if ( ! ef_serialized_chains.empty()) {
      if ( unpackChains(ef_serialized_chains, efchainsCache, efchainsByName).isFailure() ) {
        ATH_MSG_WARNING("Unpacking  of EF/HLT chains failed");    
      }
    } else {
      ATH_MSG_DEBUG("Empty EF/HLT chains");
    }
  
  
    this->unpacked_decision(true);

    return StatusCode::SUCCESS;
  }

  StatusCode DecisionUnpackerAthena::unpackNavigation(HLT::TrigNavStructure* nav){
    ATH_MSG_DEBUG("Unpacking Navigation");
    const TrigDec::TrigDecision* dec = m_handle->getDecision();
    if (nav) {
      HLT::NavigationCore* fullNav = dynamic_cast<HLT::NavigationCore*>(nav);
      
      // cppcheck-suppress oppositeInnerCondition
      if(!fullNav){
        ATH_MSG_WARNING("downcast failed");
      }
      
      fullNav->reset();


      // try to get navigation from EF/HLT 
      ATH_MSG_DEBUG("Trying to unpack EF/HLT Navigation of size: " << dec->getEFResult().getNavigationResult().size());
      
      bool unpacking_status = !dec->getEFResult().getNavigationResult().empty() && fullNav->deserialize(dec->getEFResult().getNavigationResult());
      
      if ( ! unpacking_status ) {
        if (msgLvl(MSG::DEBUG)) {
          msg() << MSG::DEBUG << "EF/HLT Navigation unpacking failed";
        }
        if (!dec->getL2Result().getNavigationResult().empty()) {
          msg() << ", falling back to L2 Navigation of size: "
		            << dec->getL2Result().getNavigationResult().size() << endmsg;      
	        unpacking_status = nav->deserialize(dec->getL2Result().getNavigationResult());
	      } else {
          msg() << endmsg;
        }	
      }
      if ( ! unpacking_status ) {
        ATH_MSG_DEBUG("Full (L2 & EF) Navigation unpacking failed");
      } else {
        ATH_MSG_DEBUG("Unpacked Navigation ");  
      } 
    }
    this->unpacked_navigation(true);

    return StatusCode::SUCCESS;
  }

  bool DecisionUnpackerAthena::assert_handle(){
    if (!m_handle) {
      ATH_MSG_ERROR("Logic ERROR, no handle for TrigDecisionTool ");   
      return false;
    } 
    if (m_handle->getDecision() == 0 ) {
      ATH_MSG_INFO("No TrigDecision object accessible ");   
      return false;
    }
    if(m_handle->valid()){
      //in this case we do not want to mess with it.
      return false;
    }
    return true;
  }
  void DecisionUnpackerAthena::validate_handle(){
    m_handle->validate();
  }
  void DecisionUnpackerAthena::invalidate_handle(){
    m_handle->reset(); // This used to be invalidate(), but we now use a ReadHandle, so it has to be a full reset.
    this->unpacked_navigation(false);
    this->unpacked_decision(false);
  }
}
#endif

#endif // full Athena env
