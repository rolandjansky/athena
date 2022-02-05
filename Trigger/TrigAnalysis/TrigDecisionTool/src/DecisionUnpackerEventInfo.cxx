/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#ifndef XAOD_ANALYSIS // Full Athena only

#include "TrigSteeringEvent/Lvl1Result.h"

#include "StoreGate/StoreGateSvc.h"
#include "TrigNavStructure/TrigNavStructure.h"
#include "TrigDecisionTool/DecisionUnpackerEventInfo.h"

#include "EventInfo/TriggerInfo.h"


#include "TrigNavigation/NavigationCore.h"


bool get32BitDecision( unsigned int index,
		       const std::vector< uint32_t >& vec ) {
  
  if( index >= vec.size() * 32 ) {
    return 0;
  }
  uint32_t word = vec[ index / 32 ];
  return ( ( ( word >> ( index % 32 ) ) & 0x1 ) != 0 );
}


namespace Trig {
  DecisionUnpackerEventInfo::DecisionUnpackerEventInfo(SG::ReadHandleKey<EventInfo>* oldEventInfoKey) :
    m_eventInfoKey(oldEventInfoKey){
  }

  DecisionUnpackerEventInfo::~DecisionUnpackerEventInfo(){
  }


  StatusCode DecisionUnpackerEventInfo::unpackItems(const std::vector<uint32_t>& level1TriggerInfo,
                                                    std::map<unsigned, LVL1CTP::Lvl1Item>& itemsCache,
                                                    std::unordered_map<std::string, const LVL1CTP::Lvl1Item*>& itemsByName) const {
    
    uint32_t L1SIZE = level1TriggerInfo.size()/3;
    std::vector<uint32_t>::const_iterator begin, end;
    
    begin = level1TriggerInfo.begin();
    end = begin;
    std::advance(end,L1SIZE);
    std::vector<uint32_t> tbp(begin,end);
    
    begin = end;
    std::advance(end,L1SIZE);
    std::vector<uint32_t> tap(begin,end);
    
    begin = end;
    std::advance(end,L1SIZE);
    std::vector<uint32_t> tav(begin,end);

    itemsByName.reserve( itemsByName.size() + itemsCache.size() );
    for ( auto [ctpid, item] : itemsCache ) {
      ATH_MSG_VERBOSE("Unpacking bits for item: " << ctpid << " " << item.name());
      const bool passBP = get32BitDecision(ctpid,tbp);
      const bool passAP = get32BitDecision(ctpid,tap);
      const bool passAV = get32BitDecision(ctpid,tav);
      LVL1CTP::Lvl1Item itemNew (item.name(), item.hashId(),
                                 passBP, passAP, passAV,
                                 item.prescaleFactor());
      item = std::move (itemNew);
      itemsByName[item.name()] = &item;
    }
    return StatusCode::SUCCESS;
  }


  StatusCode DecisionUnpackerEventInfo::unpackChains(const std::vector<uint32_t>& chainTriggerInfo,
						     std::map<unsigned, HLT::Chain>& cache,
						     std::unordered_map<std::string, const HLT::Chain*>& output) const {
    output.reserve( output.size() + cache.size() );
    for( auto& [cntr, chain] : cache){
      chain.reset();

      //we have to make a guess here 
      //the bit form the event header is set according to 
      //HLT::Chain::chainPassed() i.e. either non-prescaled, non-ressurected && passed
      //or it was passed through.
      //if the bit is set, we will chose the first version for all chains
      //even though some may be passed through
      //if the bit is not set we will set all flags to false

      bool decision =  get32BitDecision( cntr, chainTriggerInfo );
      if(decision){
	chain.setDecisions( true, //raw
			     false,//passedthrough
			     false,//prescaled
			     false //resurrected
			     );
      }
      else {
	chain.setDecisions( false,//raw
			     false,//passedthrough
			     false,//prescaled
			     false //resurrected
			     );
      }
      output[chain.getChainName()] = &chain;
    }
    return StatusCode::SUCCESS;
  }

  StatusCode DecisionUnpackerEventInfo::unpackDecision(
                            const EventContext& ctx,
                            std::unordered_map<std::string, const LVL1CTP::Lvl1Item*>& itemsByName,
                            std::map<CTPID, LVL1CTP::Lvl1Item>& itemsCache,
                            std::unordered_map<std::string, const HLT::Chain*>& l2chainsByName,
                            std::map<CHAIN_COUNTER, HLT::Chain>& l2chainsCache,
                            std::unordered_map<std::string, const HLT::Chain*>& efchainsByName,
                            std::map<CHAIN_COUNTER, HLT::Chain>& efchainsCache,
                            char& /*bgCode*/,
                            bool unpackHLT
                            ) const {
    
    ATH_MSG_DEBUG("unpacking decision from EventInfo");

    SG::ReadHandle<EventInfo> eventInfo(*m_eventInfoKey, ctx);
    const TriggerInfo* dec = eventInfo->trigger_info();

    // bgCode = dec->BGCode();
  
    // L1 items
    itemsByName.clear();
    ATH_MSG_DEBUG("Unpacking of L1 items");
    if( unpackItems(dec->level1TriggerInfo(),itemsCache,itemsByName).isFailure() ) {
      ATH_MSG_WARNING("Unpacking of L1 items failed");
    }
  
 
    // protect from unpacking in case HLT was not run (i.e. configuration chains are 0)
    if (!unpackHLT) return StatusCode::SUCCESS;


    // L2 chains
    l2chainsByName.clear();
    const auto& level2TriggerInfo = dec->level2TriggerInfo();

    if ( unpackChains(level2TriggerInfo, l2chainsCache, l2chainsByName).isFailure() ) {
      ATH_MSG_WARNING("Unpacking of L2 chains failed");
    }

    // EF chains
    efchainsByName.clear();
    const auto& eventFilterInfo = dec->eventFilterInfo();
  
    if ( unpackChains(eventFilterInfo, efchainsCache, efchainsByName).isFailure() ) {
      ATH_MSG_WARNING("Unpacking of EF/HLT chains failed");
    }
    
    return StatusCode::SUCCESS;
  }

  StatusCode DecisionUnpackerEventInfo::unpackNavigation(const EventContext&,
                                                         HLT::TrigNavStructure*) const {
    ATH_MSG_WARNING("This is the EventInfo Unpacker, there is no Navigation available -- but someone seems to try to access features triggering a navi unpacking");
    return StatusCode::SUCCESS;
  }

}

#endif // full Athena env
