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
    asg::AsgMessaging("CacheGlobalMemory"),
    m_unpacker( 0 ),
    m_navigation(0),
    m_confItems(0),  
    m_confChains(0), 
    m_bgCode(0)
{}


Trig::CacheGlobalMemory::~CacheGlobalMemory() {
  delete m_unpacker;
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
  
  if( m_unpacker->assert_handle() ) {
    if( unpackDecision().isFailure() ) {
      ATH_MSG_WARNING( "TrigDecion object incorrect (for chains)" );
    }
    if( unpackNavigation().isFailure() ) {
      static bool warningPrinted = false;
      if( ! warningPrinted ) {
	ATH_MSG_WARNING( "TrigDecion object incorrect (for navigation)" );
	warningPrinted = true;
      }
    }
    m_unpacker->validate_handle();
  }
  
  return true;
}

StatusCode Trig::CacheGlobalMemory::unpackDecision() {
  
  ATH_MSG_DEBUG("Unpacking TrigDecision ");
  ATH_MSG_DEBUG("clearing the delete-end-of-event store");
  m_deleteAtEndOfEvent.clear();
  
  bool unpackHLT = ( m_confChains != 0 );
 ATH_CHECK( m_unpacker->unpackDecision( m_itemsByName, m_itemsCache,
					m_l2chainsByName, m_l2chainsCache,
					m_efchainsByName, m_efchainsCache,
					m_bgCode, unpackHLT ) );
 
  return StatusCode::SUCCESS;
}


StatusCode Trig::CacheGlobalMemory::unpackNavigation() {
  
  // Navigation
  // protect from unpacking in case HLT was not run
  // (i.e. configuration chains are 0)
  if( m_confChains == 0 ) {
    return StatusCode::SUCCESS;
  }

  // Failing to unpack the navigation is not a failure, as it may be missing
  // from the xAOD file:
  if( ! m_unpacker->unpackNavigation( m_navigation ).isSuccess() ) {
    static bool warningPrinted = false;
    if( ! warningPrinted ) {
      ATH_MSG_WARNING( "TrigNavigation unpacking failed" );
      warningPrinted = true;
    }
  }
  
  // Return gracefully:
  return StatusCode::SUCCESS;
}

Trig::CacheGlobalMemory::AnyTypeDeleter::~AnyTypeDeleter() 
{
  clear();
}
 

