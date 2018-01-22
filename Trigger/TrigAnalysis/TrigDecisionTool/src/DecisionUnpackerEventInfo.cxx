/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AsgTools/AsgToolsConf.h"
//only in full Athena
#if defined(ASGTOOL_ATHENA) && !defined(XAOD_ANALYSIS)



#ifndef XAOD_ANALYSIS
//there is no better way????
#define private public
#include "TrigSteeringEvent/Lvl1Result.h"
#undef private

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
  DecisionUnpackerEventInfo::DecisionUnpackerEventInfo(StoreGateSvc* sg, const std::string& key) : m_handle(new DecisionObjectHandleEventInfo(sg,key)){
  }

  DecisionUnpackerEventInfo::~DecisionUnpackerEventInfo(){
    delete m_handle;
  }


  StatusCode DecisionUnpackerEventInfo::unpackItems(const std::vector<uint32_t>& level1TriggerInfo, std::map<unsigned, LVL1CTP::Lvl1Item*>& itemsCache, std::map<std::string, const LVL1CTP::Lvl1Item*>& itemsByName) {
    
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
    
    std::map<unsigned, LVL1CTP::Lvl1Item*>::iterator cacheIt;
    for ( cacheIt = itemsCache.begin(); cacheIt != itemsCache.end(); ++cacheIt ) {    
      unsigned int ctpid = cacheIt->first;
      LVL1CTP::Lvl1Item* item = cacheIt->second;
      ATH_MSG_VERBOSE("Unpacking bits for item: " << ctpid << " " << item->name());
      item->m_passBP = get32BitDecision(ctpid,tbp);
      item->m_passAP = get32BitDecision(ctpid,tap);
      item->m_passAV = get32BitDecision(ctpid,tav);
      itemsByName[item->name()] = item;
    }
    return StatusCode::SUCCESS;
  }


  StatusCode DecisionUnpackerEventInfo::unpackChains(const std::vector<uint32_t>& chainTriggerInfo,
						     std::map<unsigned, HLT::Chain*>& cache,
						     std::map<std::string, const HLT::Chain*>& output) {
    for( auto& cntr_chain : cache){
      auto cntr = cntr_chain.first;      
      auto chain = cntr_chain.second;
      chain->reset();

      //we have to make a guess here 
      //the bit form the event header is set according to 
      //HLT::Chain::chainPassed() i.e. either non-prescaled, non-ressurected && passed
      //or it was passed through.
      //if the bit is set, we will chose the first version for all chains
      //even though some may be passed through
      //if the bit is not set we will set all flags to false

      bool decision =  get32BitDecision( cntr, chainTriggerInfo );
      if(decision){
	chain->setDecisions( true, //raw
			     false,//passedthrough
			     false,//prescaled
			     false //resurrected
			     );
      }
      else {
	chain->setDecisions( false,//raw
			     false,//passedthrough
			     false,//prescaled
			     false //resurrected
			     );
      }
      output[chain->getChainName()] = chain;
    }
    return StatusCode::SUCCESS;
  }

  StatusCode DecisionUnpackerEventInfo::unpackDecision(std::map<std::string, const LVL1CTP::Lvl1Item*>& itemsByName,
						    std::map<CTPID, LVL1CTP::Lvl1Item*>& itemsCache,
						    std::map<std::string, const HLT::Chain*>& l2chainsByName,
						    std::map<CHAIN_COUNTER, HLT::Chain*>& l2chainsCache,
						    std::map<std::string, const HLT::Chain*>& efchainsByName,
						    std::map<CHAIN_COUNTER, HLT::Chain*>& efchainsCache,
                                                    char& /*bgCode*/,
						    bool unpackHLT
						    ){
    
    ATH_MSG_DEBUG("unpacking decision from EventInfo");


    const TriggerInfo* dec = m_handle->getDecision();

    // bgCode = dec->BGCode();
  
    // L1 items
    itemsByName.clear();
    ATH_MSG_DEBUG("Unpacking of L1 items");
    if( unpackItems(dec->level1TriggerInfo(),itemsCache,itemsByName).isFailure() ) {
      ATH_MSG_WARNING("Unpacking  of L1 items failed");
    }
  
 
    // protect from unpacking in case HLT was not run (i.e. configuration chains are 0)
    if (!unpackHLT) return StatusCode::SUCCESS;


    // L2 chains
    l2chainsByName.clear();
    auto level2TriggerInfo = dec->level2TriggerInfo();

    if ( unpackChains(level2TriggerInfo, l2chainsCache, l2chainsByName).isFailure() ) {
      ATH_MSG_WARNING("Unpacking  of L2 chains failed");
    }

    // EF chains
    efchainsByName.clear();
    auto eventFilterInfo = dec->eventFilterInfo();
  
    if ( unpackChains(eventFilterInfo, efchainsCache, efchainsByName).isFailure() ) {
      ATH_MSG_WARNING("Unpacking  of EF/HLT chains failed");    
    }
    
    this->unpacked_decision(true);

    return StatusCode::SUCCESS;
  }

  StatusCode DecisionUnpackerEventInfo::unpackNavigation(HLT::TrigNavStructure* /*nav*/){
    ATH_MSG_WARNING("This is the EventInfo Unpacker, there is no Navigation available -- but someone seems to try to access features triggering a navi unpacking");
    this->unpacked_navigation(true);
    return StatusCode::SUCCESS;
  }

  bool DecisionUnpackerEventInfo::assert_handle(){
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
  
  void DecisionUnpackerEventInfo::validate_handle(){
    m_handle->validate();
  }
  void DecisionUnpackerEventInfo::invalidate_handle(){
    m_handle->invalidate();
    this->unpacked_navigation(false);
    this->unpacked_decision(false);
  }
}
#endif

#endif // full Athena env
