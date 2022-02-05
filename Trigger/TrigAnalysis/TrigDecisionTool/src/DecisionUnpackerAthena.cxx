/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS // Full Athena only

#include "TrigSteeringEvent/Lvl1Result.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrigNavStructure/TrigNavStructure.h"
#include "TrigDecisionTool/DecisionUnpackerAthena.h"
#include "TrigNavigation/NavigationCore.h"
#include "TrigDecisionEvent/TrigDecision.h"


namespace Trig {
  DecisionUnpackerAthena::DecisionUnpackerAthena(SG::ReadHandleKey<TrigDec::TrigDecision>* olddeckey) : 
    m_deckey(olddeckey) {
  }

  DecisionUnpackerAthena::~DecisionUnpackerAthena(){
  }


  StatusCode DecisionUnpackerAthena::unpackItems(const LVL1CTP::Lvl1Result& result,
                                                 std::map<unsigned, LVL1CTP::Lvl1Item>& itemsCache,
                                                 std::unordered_map<std::string, const LVL1CTP::Lvl1Item*>& itemsByName) const {
    itemsByName.reserve( itemsByName.size() + itemsCache.size() );
    for ( auto& [ctpid, item] : itemsCache ) {
      ATH_MSG_VERBOSE("Unpacking bits for item: " << ctpid << " " << item.name());
      const bool passBP = result.isPassedBeforePrescale(ctpid);
      const bool passAP = result.isPassedAfterPrescale(ctpid);
      const bool passAV = result.isPassedAfterVeto(ctpid);
      LVL1CTP::Lvl1Item itemNew (item.name(), item.hashId(),
                                 passBP, passAP, passAV,
                                 item.prescaleFactor());
      item = std::move (itemNew);
      itemsByName[item.name()] = &item;
    }
    return StatusCode::SUCCESS;
  }


  StatusCode DecisionUnpackerAthena::unpackChains(const std::vector<uint32_t>& serialized_chains,
						   std::map<unsigned, HLT::Chain>& cache,
						   std::unordered_map<std::string, const HLT::Chain*>& output) const {
   
    if( serialized_chains.empty() ) {
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
        cacheIt->second.reset();
        cacheIt->second.deserialize(*rawIt);
        output[cacheIt->second.getChainName()] = &cacheIt->second;
        ATH_MSG_VERBOSE("Updated chain in this event : " << cacheIt->second);
      }
    }
    return StatusCode::SUCCESS;
  }

  StatusCode DecisionUnpackerAthena::unpackDecision(const EventContext& ctx,
                            std::unordered_map<std::string, const LVL1CTP::Lvl1Item*>& itemsByName,
						    std::map<CTPID, LVL1CTP::Lvl1Item>& itemsCache,
						    std::unordered_map<std::string, const HLT::Chain*>& l2chainsByName,
						    std::map<CHAIN_COUNTER, HLT::Chain>& l2chainsCache,
						    std::unordered_map<std::string, const HLT::Chain*>& efchainsByName,
						    std::map<CHAIN_COUNTER, HLT::Chain>& efchainsCache,
						    char& bgCode,
						    bool unpackHLT
						    ) const {

    SG::ReadHandle<TrigDec::TrigDecision> trigDec(*m_deckey, ctx);
    const TrigDec::TrigDecision* dec = trigDec.cptr();

    bgCode = dec->BGCode();

    // L1 items
    itemsByName.clear();
    ATH_MSG_DEBUG("Unpacking of L1 items");
    if( unpackItems(dec->getL1Result(),itemsCache,itemsByName).isFailure() ) {
      ATH_MSG_WARNING("Unpacking of L1 items failed");
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
        ATH_MSG_WARNING("Unpacking of L2 chains failed");
      }
    }
  
    // EF chains
    ATH_MSG_DEBUG("Unpacking of EF/HLT chains");
    efchainsByName.clear();
    const std::vector<uint32_t>& ef_serialized_chains = dec->getEFResult().getChainResult();
    ATH_MSG_DEBUG(ef_serialized_chains.size() << " EF/HLT chains");
  
    if ( ! ef_serialized_chains.empty()) {
      if ( unpackChains(ef_serialized_chains, efchainsCache, efchainsByName).isFailure() ) {
        ATH_MSG_WARNING("Unpacking of EF/HLT chains failed");
      }
    } else {
      ATH_MSG_DEBUG("Empty EF/HLT chains");
    }

    return StatusCode::SUCCESS;
  }

  StatusCode DecisionUnpackerAthena::unpackNavigation(const EventContext& ctx,
                                                      HLT::TrigNavStructure* nav) const {
    ATH_MSG_DEBUG("Unpacking Navigation");
    SG::ReadHandle<TrigDec::TrigDecision> trigDec(*m_deckey, ctx);
    const TrigDec::TrigDecision* dec = trigDec.cptr();
    if (nav) {
      HLT::NavigationCore* fullNav = dynamic_cast<HLT::NavigationCore*>(nav);
      
      if(!fullNav){
        ATH_MSG_WARNING("downcast failed");
        return StatusCode::FAILURE;
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

    return StatusCode::SUCCESS;
  }

}

#endif // full Athena env
