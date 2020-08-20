/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: TrigDecisionTool
 * @Package: TrigDecisionTool
 * @Class  : CacheGlobalMemory
 *
 * @brief registry for ChainGroups
 *
 * @author Michael Begel  <michael.begel@cern.ch> - Brookhaven National Laboratory
 * @author Lukas Heinrich <lukas.heinrich@cern.ch> - NYU
 * 
 ***********************************************************************************/

#include <string>
#include <vector>
#include <iostream>
#include <iterator>
#include <unordered_map>

#include "TrigSteeringEvent/Chain.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigNavStructure/TriggerElement.h"

#include "TrigSteeringEvent/Lvl1Item.h"

#include "TrigDecisionTool/CacheGlobalMemory.h"
#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/TDTUtilities.h"

#ifndef XAOD_ANALYSIS
#include "TrigDecisionEvent/TrigDecision.h"
#include "TrigDecisionTool/DecisionUnpackerAthena.h"
#include "TrigDecisionTool/DecisionUnpackerEventInfo.h"
#endif

#include "xAODTrigger/TrigDecision.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "TrigDecisionTool/DecisionUnpackerStandalone.h"

#ifndef XAOD_STANDALONE
#include "AthenaBaseComps/AthCheckMacros.h"
#else
#include "AsgTools/Check.h"
#endif


Trig::CacheGlobalMemory::CacheGlobalMemory() :
  m_store( nullptr ),
  m_unpacker( nullptr ),
  m_navigation(nullptr),
  m_confItems(nullptr),
  m_confChains(nullptr),
  m_expressStreamContainer(nullptr),
  m_decisionKeyPtr(nullptr),
  m_oldDecisionKeyPtr(nullptr),
  m_oldEventInfoKeyPtr(nullptr),
  m_navigationKeyPtr(nullptr),
  m_bgCode(0)
{}

Trig::CacheGlobalMemory::~CacheGlobalMemory() {
   delete m_unpacker;

   for( auto cgIt = m_chainGroups.begin(); cgIt != m_chainGroups.end();
        ++cgIt ) {
      delete cgIt->second;
   }
   for( auto iIt = m_itemsCache.begin(); iIt != m_itemsCache.end(); ++iIt ) {
      delete iIt->second;
   }
   for( auto cIt = m_l2chainsCache.begin(); cIt != m_l2chainsCache.end();
        ++cIt ) {
      delete cIt->second;
   }
   for( auto cIt = m_efchainsCache.begin(); cIt != m_efchainsCache.end();
       ++cIt ) {
      delete cIt->second;
   }
}

const Trig::ChainGroup* Trig::CacheGlobalMemory::createChainGroup(const std::vector< std::string >& triggerNames,
                                                                  const std::string& alias) {
  // mutex in case this is called directly
  std::lock_guard<std::recursive_mutex> lock(m_cgmMutex);
  // create a proper key
  std::vector< std::string > key=Trig::keyWrap(triggerNames);

  auto res = m_chainGroups.try_emplace (key, nullptr);
  if (res.second) {
    res.first->second = new ChainGroup( key, *this );
    updateChainGroup(res.first->second);
    m_chainGroupsRef[key] = res.first->second;
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
  std::lock_guard<std::recursive_mutex> lock(m_cgmMutex);
  ATH_MSG_DEBUG( "Updating configuration" );
  // store a global reference to the initial answer
  m_confChains = confChains;
  if ( !ctp ) {
    ATH_MSG_WARNING( "No LVL1 config, something went wrong, TDT will "
         "not attempt accessing HLT too" );
    m_confItems = 0;
    return;
  }
  m_confItems = &(ctp->menu().items());

  // rebuild all the caches with decision information

   //clear cache completely becuase underlying config objects might have changed
   for(auto& item : m_itemsCache){delete item.second;}
   m_itemsCache.clear();

  std::vector<float> prescales = ctp->prescaleSet().prescales_float();
  for(auto item : ctp->menu().items() ) {
     unsigned int ctpid = item->ctpId();
     m_itemsCache[ctpid] = new LVL1CTP::Lvl1Item(item->name(),
                                                 0,
                                                 0, 0, 0, prescales[ctpid]);
     ATH_MSG_DEBUG( " new configuration for item" << item->name() );
  }
  ATH_MSG_DEBUG( "Updating configuration, done with L1" );  
  
  //clear cache completely becuase underlying config objects might have changed
  for(auto& c : m_l2chainsCache){delete c.second;}
  m_l2chainsCache.clear(); 
  for(auto& c : m_efchainsCache){delete c.second;}
  m_efchainsCache.clear(); 
  m_mConfChains.clear();
  if ( ! confChains ) {
    ATH_MSG_WARNING( "No chains in configuration, probably run w/o HLT" );
  } else {

    ATH_MSG_DEBUG("Updating Configuration chains. Number of conf chains: " << m_confChains->size()); 

    // updating internal map of conf chains (this map is only used for fast lookup)
    for(auto ch : *m_confChains) {
      m_mConfChains[ch->chain_name()] = ch;
    }

    // updating internal cache of HLT::Chains
    for(auto ch : *m_confChains) {
      //    std::cerr << "CacheGlobalMemory::update updating chain" << (*cChIt)->chain_name() << std::endl;
      int cntr = ch->chain_counter();
      if( ch->level()=="L2" ) {       
        m_l2chainsCache[cntr] = new HLT::Chain(ch);
      } else {//merged chains are stored in efchains
        m_efchainsCache[cntr] = new HLT::Chain(ch);
      }
    }
    ATH_MSG_DEBUG( "Updating configuration, done with basic HLT based on "
       << m_confChains->size() << " configuration chains" );


    // code for the streams and pre-defined groups
    // -- assume that the groups and streams have all changed!!!
    m_groups.clear();
    m_streams.clear();

    for(auto ch : *m_confChains) {
      if (( ch->level() == "EF" || ch->level() == "HLT") && ch->streams().size()>0 ) {
        ATH_MSG_DEBUG( "Stream: " << ch->chain_name() << " " << ch->streams().size() );
        for(auto stream : ch->streams()) {
          if( msgLvl( MSG::DEBUG ) ) {
            msg() << " " << stream->stream();
          }
          m_streams[stream->stream()].push_back(ch->chain_name());
        }
        if( msgLvl( MSG::DEBUG ) ) {
          msg() << endmsg;
        }
      }
      if ( ( ch->level() == "EF" || ch->level() == "HLT") && ch->groups().size()>0 ) {
        ATH_MSG_DEBUG( "Groups: " << ch->chain_name() << " " << ch->groups().size() );
        for(auto& group : ch->groups()) {
          if( msgLvl( MSG::DEBUG ) ) {
            msg() << " " << group;
          }
          m_groups[group].push_back(ch->chain_name());
        }
        if( msgLvl( MSG::DEBUG ) ) {
          msg() << endmsg;
        }
      }
    }
    //
    std::map<std::string, std::vector<std::string> >::iterator mstIt;
    for (mstIt=m_streams.begin(); mstIt != m_streams.end(); ++mstIt) {
      const std::string alias("STREAM_"+mstIt->first);
      std::vector< std::string > key_alias=Trig::keyWrap(Trig::convertStringToVector(alias));
      ChGrIt preIt = m_chainGroupsRef.find(key_alias);
      if (  preIt != m_chainGroupsRef.end()) {
        ATH_MSG_INFO( "Replacing predefined, stream based, chain group: "
          << alias );
        // cg already exists (from previous config, we need to update it)
        preIt->second->m_patterns = mstIt->second;
        updateChainGroup(preIt->second);  
      } else {
        createChainGroup(mstIt->second,alias);
      }
      
    }
    for (mstIt=m_groups.begin(); mstIt != m_groups.end(); ++mstIt) {
      const std::string alias("GROUP_"+mstIt->first);
      std::vector< std::string > key_alias=Trig::keyWrap(Trig::convertStringToVector(alias));
      ChGrIt preIt = m_chainGroupsRef.find(key_alias);
      if (preIt != m_chainGroupsRef.end()) {
        ATH_MSG_INFO( "Replacing predefined, config group based, chain "
          << "group: " << alias );
        preIt->second->m_patterns = mstIt->second;
        updateChainGroup(preIt->second);
      } else {
        createChainGroup(mstIt->second,alias);
      }
    }
    ATH_MSG_DEBUG( "ChainGroups for streams and configuration groups "
       "defined/updated" );
  }


  // update all previously defined chainGroups
  for (ChGrIt it=m_chainGroups.begin();
       it!=m_chainGroups.end();
       it++) {
    updateChainGroup(it->second);
  }   
   ATH_MSG_DEBUG( "Updating configuration, done with ChainGroups defined so "
                  "far" );
   ATH_MSG_DEBUG( "Updating configuration done" );
}


const HLT::Chain* Trig::CacheGlobalMemory::chain(const std::string& name) const {
  std::lock_guard<std::recursive_mutex> lock(m_cgmMutex);
  auto i = m_efchainsByName.find(name);
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

const TrigConf::HLTChain* Trig::CacheGlobalMemory::config_chain(const std::string& name) const {
  ChainHashMap_t::const_iterator f = m_mConfChains.find(name);
  if ( f == m_mConfChains.end() ){
    return 0;
  }
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

float Trig::CacheGlobalMemory::item_prescale(int ctpid) const {
  // Cannot be locked (CacheGlobalMemory::update > createChainGroup > ChainGroup::update > calculatePrescale > L1Prescale > CacheGlobalMemory::item_prescale)
  // find items in cache
  if ( m_itemsCache.count(ctpid) == 0 ) {
    ATH_MSG_ERROR( "item of CTP: " << ctpid
       << " is not present in the configuration" );
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

const xAOD::TrigCompositeContainer* Trig::CacheGlobalMemory::expressStreamContainer() const {
  if(!m_expressStreamContainer){
    StatusCode sc = store()->retrieve(m_expressStreamContainer, "HLT_Express_stream_HLT");
    if(sc.isFailure()){
      ATH_MSG_WARNING("could not retrieve express stream container");
    }
  }
  return m_expressStreamContainer;
}

bool Trig::CacheGlobalMemory::assert_decision() {
  std::lock_guard<std::recursive_mutex> lock(m_cgmMutex);
  ATH_MSG_VERBOSE("asserting decision with unpacker " << m_unpacker);

  // here we unpack the decision. Note: the navigation will be unpacked only on demand (see navigation())
  bool contains_xAOD_decision = false;
  bool is_l1result_configured = false;
  bool contains_decision = false;
  bool contains_old_event_info = false;
  
  if(!m_unpacker){
    ATH_MSG_INFO("decision not set on first (?) assert. deciding how to unpack");
   
    //Lukas 26-06-2015: we're hard coding the configuration for now
    //but we have setters and getters for m_trigDecisionKey (as CGM datamemer)
    //so we could in the future use the ones set by the python configuration
    //we're hardcoding in order not to require python configuration changes

    const EventContext context = Gaudi::Hive::currentContext();
    if (!m_decisionKeyPtr->empty()) {
       SG::ReadHandle<xAOD::TrigDecision> decisionReadHandle = SG::makeHandle(*m_decisionKeyPtr, context);
      contains_xAOD_decision = decisionReadHandle.isValid();
    }

#ifndef XAOD_ANALYSIS

    if (!m_oldDecisionKeyPtr->empty()) {
      SG::ReadHandle<TrigDec::TrigDecision> oldDecisionReadHandle = SG::makeHandle(*m_oldDecisionKeyPtr, context);
      contains_decision = oldDecisionReadHandle.isValid();
      if (contains_decision) {
        is_l1result_configured = oldDecisionReadHandle->getL1Result().isConfigured();
      }
    }

    if (!m_oldEventInfoKeyPtr->empty()) {
      SG::ReadHandle<EventInfo> oldEventInfoHandle = SG::makeHandle(*m_oldEventInfoKeyPtr, context);
      contains_old_event_info = oldEventInfoHandle.isValid();
    }

    // January 2020: Note, switched the order here. DecisionUnpackerStandalone is given preference
    // over DecisionUnpackerAthena
    if ( contains_xAOD_decision ){
      ATH_MSG_INFO("SG contains xAOD decision, use DecisionUnpackerStandalone");
      setUnpacker(new DecisionUnpackerStandalone(m_decisionKeyPtr, m_navigationKeyPtr));
    }
    else if( is_l1result_configured ){
      ATH_MSG_INFO("SG contains AOD decision, use DecisionUnpackerAthena");
      setUnpacker(new DecisionUnpackerAthena(m_oldDecisionKeyPtr));
    }
    else if (contains_old_event_info) {
      ATH_MSG_INFO("SG contains NO(!) L1Result in the AOD TrigDecision, assuming also no HLTResult. Read from EventInfo");
      setUnpacker(new DecisionUnpackerEventInfo(m_oldEventInfoKeyPtr));
    }
#else
    if ( contains_xAOD_decision ){
      ATH_MSG_INFO("SG contains xAOD decision, use DecisionUnpackerStandalone");
      setUnpacker(new DecisionUnpackerStandalone(m_decisionKeyPtr, m_navigationKeyPtr));
    }
#endif
    
  }//if(!m_unpacker)

  if(!m_unpacker){
    ATH_MSG_ERROR("No source of Trigger Decision in file. "
      << "(Looked for xAOD::TrigDecision? "
      << (m_decisionKeyPtr->empty() ? "NO" : "YES")
      << ", has xAOD::TrigDecision? " 
      << (contains_xAOD_decision ? "YES" : "NO")
      << ". Looked for old TrigDec::TrigDecision? "
      << (m_oldDecisionKeyPtr->empty() ? "NO" : "YES")
      << ", has TrigDec::TrigDecision? " 
      << (contains_decision ? "YES" : "NO")
      << ", TrigDec::TrigDecision has L1? " 
      << (is_l1result_configured ? "YES" : "NO")
      << ". Looked for old EventInfo? "
      << (m_oldEventInfoKeyPtr->empty() ? "NO" : "YES")
      << ", has old EventInto? "  
      << (contains_old_event_info ? "YES" : "NO")
      << ". Check UseRun1DecisionFormat and UseOldEventInfoDecisionFormat flags if reading pre-xAOD or BS input).");
    throw std::runtime_error("Trig::CacheGlobalMemory::assert_decision(): No source of Trigger Decision in file.");
  }

  if( m_unpacker->assert_handle() ) {
    ATH_MSG_VERBOSE("asserted handle");

    if( unpackDecision().isFailure() ) {
      ATH_MSG_WARNING( "TrigDecion object incorrect (for chains)" );
    }
    else{
      ATH_MSG_VERBOSE("unpacked decision");
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
  std::lock_guard<std::recursive_mutex> lock(m_cgmMutex);
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
 

