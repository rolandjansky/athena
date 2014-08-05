/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: TrigDecisionTool
 * @Package: TrigDecisionTool
 * @Class  : CacheGlobalMemory
 *
 * @brief registry for ChainGroups
 *
 * @author Michael Begel  <michael.begel@cern.ch> - Brookhaven National Laboratory
 *
 ***********************************************************************************/

#include <string>
#include <vector>
#include <iostream>
#include <iterator>

#include "TrigNavigation/ComboIterator.h"
#include "TrigSteeringEvent/Chain.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "AthenaKernel/getMessageSvc.h"
#include "TrigNavigation/NavigationCore.h"
#include "TrigNavigation/TriggerElement.h"

#include "boost/foreach.hpp"

#define private public
#include "TrigSteeringEvent/Lvl1Result.h"
#undef private

#include "TrigDecisionTool/CacheGlobalMemory.h"
#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/TDTUtilities.h"

Trig::CacheGlobalMemory::CacheGlobalMemory() 
  : Logger ("CacheGlobalMemory"),
    m_decisionObjectHandle(0),
    m_navigation(0),
    m_confItems(0),  
    m_confChains(0), 
    m_bgCode(0)
{}


Trig::CacheGlobalMemory::~CacheGlobalMemory() {
  delete m_decisionObjectHandle;
  // delete chain groups
  ChGrIt cgIt;
  for ( cgIt = m_chainGroups.begin(); cgIt != m_chainGroups.end(); ++cgIt ) { delete cgIt->second; }
  std::map<CTPID, LVL1CTP::Lvl1Item*>::const_iterator iIt;
  for ( iIt = m_itemsCache.begin(); iIt != m_itemsCache.end(); ++iIt ) { delete iIt->second; }
  std::map<CHAIN_COUNTER, HLT::Chain*>::const_iterator cIt;
  for ( cIt = m_l2chainsCache.begin(); cIt != m_l2chainsCache.end(); ++cIt ) { delete cIt->second; }
  for ( cIt = m_efchainsCache.begin(); cIt != m_efchainsCache.end(); ++cIt ) { delete cIt->second; }
}

const Trig::ChainGroup* Trig::CacheGlobalMemory::createChainGroup(const std::vector< std::string >& triggerNames,
                                                                  const std::string& alias) {
  // create a proper key
  std::vector< std::string > key=Trig::keyWrap(triggerNames);

  if (m_chainGroups.find(key)==m_chainGroups.end()) {
    m_chainGroups[key]=new ChainGroup( key, *this );
    updateChainGroup(m_chainGroups[key]);
    m_chainGroupsRef[key]=m_chainGroups[key];
  }
  // this overwrites the pointer in the map each time in case the alias needs defining
  if (alias!="") {
    std::vector< std::string > key_alias=Trig::keyWrap(Trig::convertStringToVector(alias));
    if (m_chainGroupsRef.find(key_alias)==m_chainGroupsRef.end()) {
      m_chainGroupsRef[key_alias]=m_chainGroups[key];
    }
    else { 
      if (m_chainGroupsRef[key_alias]!=m_chainGroupsRef[key]) {
        throw std::runtime_error("TrigDecisionTool: The alias "+alias+" already exists and cannot be overwritten.");
      }
    }
  }
  return m_chainGroupsRef[key];
}

void Trig::CacheGlobalMemory::updateChainGroup(Trig::ChainGroup* chainGroup) {
  chainGroup->update(m_confChains, m_confItems);
}


void Trig::CacheGlobalMemory::update(const TrigConf::HLTChainList* confChains,
                                     const TrigConf::CTPConfig* ctp) {

  if (msgLvl(MSG::INFO))
    log() << MSG::INFO << "Updating configuration" << endreq;  
  // store a global reference to the initial answer
  m_confChains = confChains;
  if ( !ctp ) {
    log() << MSG::WARNING << "No LVL1 config, something went wrong, TDT will not attempt accessing HLT too" << endreq;  
    m_confItems = 0;
    return;
  }
  m_confItems = &(ctp->menu().items());

  // rebuild all the caches with decision information

  m_itemsCache.clear();
  std::vector<int> prescales = ctp->prescaleSet().prescales();
  BOOST_FOREACH(const TrigConf::TriggerItem* item, ctp->menu().items() ) {
     unsigned int ctpid = item->ctpId();
     if ( m_itemsCache[ctpid] )
        delete m_itemsCache[ctpid];
     m_itemsCache[ctpid] = new LVL1CTP::Lvl1Item(item->name(),
                                                 0,
                                                 0, 0, 0, prescales[ctpid]);
     if (msgLvl(MSG::DEBUG))
        log() << MSG::DEBUG << " new configuration for item" << item->name() << endreq;
  }
  if (msgLvl(MSG::INFO))
    log() << MSG::INFO << "Updating configuration, done with L1" << endreq;  
  
  m_l2chainsCache.clear();
  m_efchainsCache.clear();
  m_mConfChains.clear();
  if ( ! confChains ) {
    if (msgLvl(MSG::INFO))
      log() << MSG::INFO << "No chains in configuration, probably run w/o HLT" << endreq;  

  } else {

    // updating internal map of conf chains (this map is only used for fast lookup)
    BOOST_FOREACH(const TrigConf::HLTChain* ch, *m_confChains) {
      m_mConfChains[ch->chain_name().c_str()] = ch;
    }

    // updating internal cache of HLT::Chains
    BOOST_FOREACH(const TrigConf::HLTChain* ch, *m_confChains) {
      //    std::cerr << "CacheGlobalMemory::update updating chain" << (*cChIt)->chain_name() << std::endl;
      int cntr = ch->chain_counter();
      if( ch->level()=="L2" ) {       
        if (m_l2chainsCache.find(cntr) != m_l2chainsCache.end()) {
          delete m_l2chainsCache[cntr];
        }
        m_l2chainsCache[cntr] = new HLT::Chain(ch);
      } else {//merged chains are stored in efchains
        if (m_efchainsCache.find(cntr) != m_efchainsCache.end()) {
          delete m_efchainsCache[cntr];
        }
        m_efchainsCache[cntr] = new HLT::Chain(ch);
      }
    }
    if (msgLvl(MSG::INFO))
      log() << MSG::INFO << "Updating configuration, done with basic HLT" << endreq;  


    // code for the streams and pre-defined groups
    // -- assume that the groups and streams have all changed!!!
    m_groups.clear();
    m_streams.clear();
    std::vector<std::string>::iterator gpIt;

    BOOST_FOREACH(const TrigConf::HLTChain* ch, *m_confChains) {
      if (( ch->level() == "EF" || ch->level() == "HLT") && ch->streams().size()>0 ) {
          if (msgLvl(MSG::DEBUG))
             log() << MSG::DEBUG
                   << "Stream: " << ch->chain_name()
                   << " " << ch->streams().size() << endreq;
          BOOST_FOREACH(TrigConf::HLTStreamTag* stream, ch->streams()) {
             if (msgLvl(MSG::DEBUG))
                log() << " " << stream->stream();
             m_streams[stream->stream()].push_back(ch->chain_name());
          }
          if (msgLvl(MSG::DEBUG))
             log() << endreq;
       }
      if ( ( ch->level() == "EF" || ch->level() == "HLT") && ch->groups().size()>0 ) {
          if (msgLvl(MSG::DEBUG))
             log() << MSG::DEBUG
                   << "Groups: " << ch->chain_name()
                   << " " << ch->groups().size() << endreq;
          BOOST_FOREACH(const std::string& group, ch->groups()) {
             if (msgLvl(MSG::DEBUG))
                log() << " " << group;
             m_groups[group].push_back(ch->chain_name());
          }
          if (msgLvl(MSG::DEBUG))
             log() << endreq;
       }
    }
   
    //
    std::map<std::string, std::vector<std::string> >::iterator mstIt;
    for (mstIt=m_streams.begin(); mstIt != m_streams.end(); mstIt++) {      
      const std::string alias("STREAM_"+mstIt->first);
      std::vector< std::string > key_alias=Trig::keyWrap(Trig::convertStringToVector(alias));
      ChGrIt preIt = m_chainGroupsRef.find(key_alias);
      if (  preIt != m_chainGroupsRef.end()) {
        if (msgLvl(MSG::INFO))
          log() << MSG::INFO << "Replacing predefined, stream based, chain group: " <<  alias << endreq;
        // cg already exists (from previous config, we need to update it)
        preIt->second->m_patterns = mstIt->second;
        updateChainGroup(preIt->second);	
      } else {
        createChainGroup(mstIt->second,alias);
      }
      
    }
    for (mstIt=m_groups.begin(); mstIt != m_groups.end(); mstIt++) {
      const std::string alias("GROUP_"+mstIt->first);
      std::vector< std::string > key_alias=Trig::keyWrap(Trig::convertStringToVector(alias));
      ChGrIt preIt = m_chainGroupsRef.find(key_alias);
      if (preIt != m_chainGroupsRef.end()) {
        if (msgLvl(MSG::INFO))
          log() << MSG::INFO << "Replacing predefined, config group based, chain group: " <<  alias << endreq;
        preIt->second->m_patterns = mstIt->second;
        updateChainGroup(preIt->second);
      } else {
        createChainGroup(mstIt->second,alias);
      }
    }
    if (msgLvl(MSG::INFO))
      log() << MSG::INFO << "ChainGroups for streams and configuration groups defined/updated " << endreq;  
  }


  // update all previously defined chainGroups
  for (ChGrIt it=m_chainGroups.begin();
       it!=m_chainGroups.end();
       it++) {
    updateChainGroup(it->second);
  }   
  if (msgLvl(MSG::INFO))
    log() << MSG::INFO << "Updating configuration, done with ChainGroups defined so far" << endreq;  


  if (msgLvl(MSG::INFO))
    log() << MSG::INFO << "Updating configuration done" << endreq;  
}


const HLT::Chain* Trig::CacheGlobalMemory::chain(const std::string& name) const {
  std::map<std::string, const HLT::Chain*>::const_iterator i = m_efchainsByName.find(name);
  if (i != m_efchainsByName.end()) {
    return i->second;
  }
  
  i = m_l2chainsByName.find(name);
  if ( i != m_l2chainsByName.end() ) {
    return i->second;
  }
  return 0;
}

const HLT::Chain* Trig::CacheGlobalMemory::chain(const TrigConf::HLTChain& ch) const {
  return chain(ch.chain_name());
}


struct chainByName {
  chainByName (const std::string& name) : m_name(name) {}
  bool operator() (const TrigConf::HLTChain* ch) {
    return m_name == ch->chain_name();
  }
  const std::string & m_name; 
};

const TrigConf::HLTChain* Trig::CacheGlobalMemory::config_chain(const std::string& name) const {
  ChainHashMap_t::const_iterator f = m_mConfChains.find(name.c_str());
  if ( f == m_mConfChains.end() )
    return 0;
  return f->second;
}

struct itemByName {
  itemByName(const std::string& name) : m_name(name) {}
  bool operator() (const TrigConf::TriggerItem* it) {
    return m_name == it->name();
  }
  const std::string & m_name;
};

const TrigConf::TriggerItem* Trig::CacheGlobalMemory::config_item(const std::string& name) const {
   TrigConf::ItemContainer::const_iterator f = find_if(m_confItems->begin(), m_confItems->end(), itemByName(name));
   if ( f == m_confItems->end() )
      return 0;
   return *f;
}

int Trig::CacheGlobalMemory::item_prescale(int ctpid) const {
  // find items in cache
  if ( m_itemsCache.count(ctpid) == 0 ) {
    log() << MSG::ERROR
	  << "item of CTP: " << ctpid
	  << " is not present in the configuration" << endreq;
    return 0;
  }
  return m_itemsCache.find(ctpid)->second->prescaleFactor();
}

const LVL1CTP::Lvl1Item* Trig::CacheGlobalMemory::item(const TrigConf::TriggerItem& i) const {
  return item(i.name());
    //   unsigned cntr = i.ctpId();
    //   if ( m_items.find(cntr) != m_items.end())
    //     return m_items.find(cntr)->second;
    //   return 0;
}

const LVL1CTP::Lvl1Item* Trig::CacheGlobalMemory::item(const std::string& name) const {
  if ( m_itemsByName.find(name) != m_itemsByName.end())
     return m_itemsByName.find(name)->second;
   return 0;
}



bool Trig::CacheGlobalMemory::assert_decision() {
  if (!m_decisionObjectHandle) {
    log() << MSG::ERROR
	  << "Logic ERROR, no handle for TrigDecisionTool " << endreq;   
    return false;
  } 
  if (m_decisionObjectHandle->get() == 0 ) {
    if (msgLvl(MSG::INFO))
      log() << MSG::INFO
            << "No TrigDecision object accessible " << endreq;   
    return false;
  }
  //  log() << MSG::DEBUG
  //	<< "TrigDecision object accessible" << endreq;   

  if (!m_decisionObjectHandle->valid()) {
    if ( unpackDecision( m_decisionObjectHandle->get() ).isFailure() ) {
      log() << MSG::WARNING << "TrigDecion object incorrect (for chains)" << endreq;
    }
    if ( unpackNavigation( m_decisionObjectHandle->get() ).isFailure() ) {
      log() << MSG::WARNING << "TrigDecion object incorrect (for navigation)" << endreq;
    }
    m_decisionObjectHandle->validate();
  }
  return true;
}




StatusCode Trig::CacheGlobalMemory::unpackItems(const LVL1CTP::Lvl1Result& result) {
  std::map<unsigned, LVL1CTP::Lvl1Item*>::iterator cacheIt;
  for ( cacheIt = m_itemsCache.begin(); cacheIt != m_itemsCache.end(); ++cacheIt ) {
    
    unsigned int ctpid = cacheIt->first;
    LVL1CTP::Lvl1Item* item = cacheIt->second;
    if (msgLvl(MSG::VERBOSE))
      log() << MSG::VERBOSE << "Unpacking bits for item: " << ctpid << " " << item->name() << endreq;
    item->m_passBP = result.isPassedBeforePrescale(ctpid);
    item->m_passAP = result.isPassedAfterPrescale(ctpid);
    item->m_passAV = result.isPassedAfterVeto(ctpid);
    m_itemsByName[item->name()] = item;


  }
  return StatusCode::SUCCESS;
}



StatusCode Trig::CacheGlobalMemory::unpackChains(const std::vector<uint32_t>& serialized_chains,
                                                 std::map<unsigned, HLT::Chain*>& cache,
                                                 std::map<std::string, const HLT::Chain*>& output) {
   
  if( serialized_chains.size() == 0 ) {
    log() << MSG::WARNING << "ChainResult is empty" << endreq;
    return StatusCode::FAILURE;
  }


  std::vector<uint32_t>::const_iterator rawIt = serialized_chains.begin();
  rawIt++; // skip first number as it is count
  for ( ; rawIt != serialized_chains.end(); ++rawIt ) {

    unsigned cntr = HLT::Chain::inquireChainCounter(*rawIt);

    // localte now the chain
    std::map<unsigned, HLT::Chain*>::iterator cacheIt = cache.find(cntr);
    if ( cacheIt == cache.end() ) {
      log() << MSG::WARNING << "Missing chain of counter in the configuration: " << cntr << endreq;
      return StatusCode::FAILURE;
    } else {
      cacheIt->second->reset();
      cacheIt->second->deserialize(*rawIt);
      output[cacheIt->second->getChainName()] = cacheIt->second;
      if (msgLvl(MSG::VERBOSE))
        log() << MSG::VERBOSE << "Updated chain in this event : " << *(cacheIt->second) << endreq;
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode Trig::CacheGlobalMemory::unpackDecision(const TrigDec::TrigDecision* dec) {
  if (msgLvl(MSG::DEBUG))
    log() << MSG::DEBUG << "Unpacking TrigDecision " << endreq;

  if (msgLvl(MSG::DEBUG))
    log() << MSG::DEBUG << "clearing the delete-end-of-event store " << endreq;
  m_deleteAtEndOfEvent.clear();


  // L1 items
  m_itemsByName.clear();
  if (msgLvl(MSG::DEBUG))
    log() << MSG::DEBUG << "Unpacking of L1 items" << endreq;
  if( unpackItems(dec->getL1Result()).isFailure() ) {
    log() << MSG::WARNING << "Unpacking  of L1 items failed" << endreq;
  }
  
  m_bgCode = dec->BGCode();
 
  // protect from unpacking in case HLT was not run (i.e. configuration chains are 0)
  if ( m_confChains == 0 ) 
    return StatusCode::SUCCESS;


  // L2 chains
  m_l2chainsByName.clear();
  if ( dec->getL2Result().getHLTLevel() != HLT::UNKNOWN ) {
    const std::vector<uint32_t>& l2_serialized_chains = dec->getL2Result().getChainResult();
    if (msgLvl(MSG::DEBUG))
      log() << MSG::DEBUG << l2_serialized_chains.size() << " L2 chains" << endreq;
    
    if ( unpackChains(l2_serialized_chains, m_l2chainsCache, m_l2chainsByName).isFailure() ) {
      log() << MSG::WARNING << "Unpacking  of L2 chains failed" << endreq;
    }
  }
  
  // EF chains
  m_efchainsByName.clear();
  const std::vector<uint32_t>& ef_serialized_chains = dec->getEFResult().getChainResult();
  if (msgLvl(MSG::DEBUG)) 
    log() << MSG::DEBUG << ef_serialized_chains.size() << " EF/HLT chains" << endreq;

  if ( ! ef_serialized_chains.empty()) {
    if ( unpackChains(ef_serialized_chains, m_efchainsCache, m_efchainsByName).isFailure() ) {
      log() << MSG::WARNING << "Unpacking  of EF/HLT chains failed" << endreq;    
    }
  } else {
    if (msgLvl(MSG::DEBUG))
      log() << MSG::DEBUG << "Empty EF/HLT chains" << endreq;
  }
  
  
  return StatusCode::SUCCESS;
}

StatusCode Trig::CacheGlobalMemory::unpackNavigation(const TrigDec::TrigDecision* dec) {
  // Navigation 
  // protect from unpacking in case HLT was not run (i.e. configuration chains are 0)
  if ( m_confChains == 0 ) 
    return StatusCode::SUCCESS;
  if (msgLvl(MSG::DEBUG))
    log() << MSG::DEBUG << "Unpacking Navigation " << endreq;

  if (m_navigation) {
    m_navigation->reset();
    // try to get navigation from EF/HLT 
    if (msgLvl(MSG::DEBUG))
      log() << MSG::DEBUG << "Trying to unpack EF/HLT Navigation of size: " 
            << dec->getEFResult().getNavigationResult().size() << endreq;

    bool unpacking_status = !dec->getEFResult().getNavigationResult().empty()
      && m_navigation->deserialize(dec->getEFResult().getNavigationResult());

    if ( ! unpacking_status ) {
      if (msgLvl(MSG::DEBUG))
        log() << MSG::DEBUG << "EF/HLT Navigation unpacking failed";
	  if (!dec->getL2Result().getNavigationResult().empty()){
		log() << ", falling back to L2 Navigation of size: "
              << dec->getL2Result().getNavigationResult().size() << endreq;      
        unpacking_status = m_navigation->deserialize(dec->getL2Result().getNavigationResult());
      }
	  else log() << endreq;	
   }
    if ( ! unpacking_status ) {
      if (msgLvl(MSG::DEBUG))
        log() << MSG::DEBUG << "Full (L2 & EF) Navigation unpacking failed" << endreq;
    } else {
      if (msgLvl(MSG::DEBUG))
        log() << MSG::DEBUG << "Unpacked Navigation " << endreq;  
      
      if (msgLvl(MSG::VERBOSE))
        log() << MSG::VERBOSE <<  *(navigation()) << endreq;
    }

  }
  return StatusCode::SUCCESS;
}

Trig::CacheGlobalMemory::AnyTypeDeleter::~AnyTypeDeleter() 
{
  clear();
}
 

