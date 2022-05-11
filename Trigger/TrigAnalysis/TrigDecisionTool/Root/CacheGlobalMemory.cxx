/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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

#include "AsgDataHandles/ReadHandle.h"
#include "CxxUtils/checker_macros.h"

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
#include "AsgMessaging/Check.h"
#endif


const Trig::ChainGroup* Trig::CacheGlobalMemory::createChainGroup(const std::vector< std::string >& triggerNames,
                                                                  const std::string& alias,
                                                                  TrigDefs::Group props) const {
  // mutex in case this is called directly
  std::lock_guard<std::recursive_mutex> lock(m_cgmMutex);
  auto nc_this ATLAS_THREAD_SAFE = const_cast<Trig::CacheGlobalMemory*>(this);

  // create a proper key
  std::vector< std::string > key=Trig::keyWrap(triggerNames);

  auto [itr, inserted] = m_chainGroups.try_emplace (key, /*ChainGroup*/ key, *nc_this);
  if (inserted) {
    nc_this->updateChainGroup(itr->second, props);
    m_chainGroupsRef[key] = &(itr->second);
  }
  // this overwrites the pointer in the map each time in case the alias needs defining
  if (!alias.empty()) {
    std::vector< std::string > key_alias=Trig::keyWrap(Trig::convertStringToVector(alias));
    auto [alias_itr, alias_inserted] = m_chainGroupsRef.try_emplace (key_alias, &m_chainGroups.at(key));
    if (!alias_inserted && alias_itr->second!=m_chainGroupsRef[key]) {
        throw std::runtime_error("TrigDecisionTool: The alias "+alias+
                                 " already exists and cannot be overwritten.");
    }
  }

  return m_chainGroupsRef[key];
}

void Trig::CacheGlobalMemory::updateChainGroup(Trig::ChainGroup& chainGroup, TrigDefs::Group props) {
  chainGroup.update(m_confChains, m_confItems, props);
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
    m_confItems = nullptr;
    return;
  }
  m_confItems = &(ctp->menu().items());

  // rebuild all the caches with decision information

  //clear cache completely because underlying config objects might have changed
  m_itemsCache.clear();

  const std::vector<float>& prescales = ctp->prescaleSet().prescales_float();
  for(auto item : ctp->menu().items() ) {
     const auto ctpid = item->ctpId();
     m_itemsCache.try_emplace(ctpid,
                              /*LVL1CTP::Lvl1Item*/ item->name(), 0,
                              0, 0, 0, prescales[ctpid]);
     ATH_MSG_DEBUG( "new configuration for item" << item->name() );
  }
  ATH_MSG_DEBUG( "Updating configuration, done with L1" );

  //clear cache completely because underlying config objects might have changed
  m_l2chainsCache.clear();
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
        m_l2chainsCache.emplace(cntr, /*HLT::Chain*/ch);
      } else {//merged chains are stored in efchains
        m_efchainsCache.emplace(cntr, /*HLT::Chain*/ch);
      }
    }
    ATH_MSG_DEBUG( "Updating configuration, done with basic HLT based on "
       << m_confChains->size() << " configuration chains" );


    // code for the streams and pre-defined groups
    // -- assume that the groups and streams have all changed!!!
    m_groups.clear();
    m_streams.clear();

    for(auto ch : *m_confChains) {
      if (( ch->level() == "EF" || ch->level() == "HLT") && !ch->streams().empty() ) {
        ATH_MSG_DEBUG( "Stream: " << ch->chain_name() << " " << ch->streams().size() );
        for(auto stream : ch->streams()) {
          ATH_MSG(DEBUG) << " " << stream->stream();
          m_streams[stream->stream()].push_back(ch->chain_name());
        }
        ATH_MSG(DEBUG) << endmsg;
      }
      if ( ( ch->level() == "EF" || ch->level() == "HLT") && !ch->groups().empty() ) {
        ATH_MSG_DEBUG( "Groups: " << ch->chain_name() << " " << ch->groups().size() );
        for(auto& group : ch->groups()) {
          ATH_MSG(DEBUG) << " " << group;
          m_groups[group].push_back(ch->chain_name());
        }
        ATH_MSG(DEBUG) << endmsg;
      }
    }
  }

  // update all defined chainGroups
  for (auto& [key, group] : m_chainGroups) {
    updateChainGroup(group);
  }
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
  return nullptr;
}

const HLT::Chain* Trig::CacheGlobalMemory::chain(const TrigConf::HLTChain& ch) const {
  return chain(ch.chain_name());
}

const TrigConf::HLTChain* Trig::CacheGlobalMemory::config_chain(const std::string& name) const {
  ChainHashMap_t::const_iterator f = m_mConfChains.find(name);
  if ( f == m_mConfChains.end() ){
    return nullptr;
  }
  return f->second;
}

struct itemByName {
  explicit itemByName(const std::string& name) : m_name(name) {}
  bool operator() (const TrigConf::TriggerItem* it) {
    return m_name == it->name();
  }
  const std::string & m_name;
};

const TrigConf::TriggerItem* Trig::CacheGlobalMemory::config_item(const std::string& name) const {
   TrigConf::ItemContainer::const_iterator f = find_if(m_confItems->begin(), m_confItems->end(), itemByName(name));
   if ( f == m_confItems->end() )
      return nullptr;
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
  return m_itemsCache.find(ctpid)->second.prescaleFactor();
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
   return nullptr;
}

bool Trig::CacheGlobalMemory::assert_decision() const {
  std::lock_guard<std::recursive_mutex> lock(m_cgmMutex);
  ATH_MSG_VERBOSE("asserting decision with unpacker " << m_unpacker.get());

  // here we unpack the decision. Note: the navigation will be unpacked only on demand (see navigation())
  bool contains_xAOD_decision = false;

#ifndef XAOD_ANALYSIS // Full Athena
  bool is_l1result_configured = false;
  bool contains_decision = false;
  bool contains_old_event_info = false;
#endif

  const EventContext& context = Gaudi::Hive::currentContext();
  if(!m_unpacker){
    ATH_MSG_INFO("decision not set on first (?) assert. deciding how to unpack");

    //Lukas 26-06-2015: we're hard coding the configuration for now
    //but we have setters and getters for m_trigDecisionKey (as CGM datamemer)
    //so we could in the future use the ones set by the python configuration
    //we're hardcoding in order not to require python configuration changes

    if (!m_decisionKeyPtr->empty()) {
       SG::ReadHandle<xAOD::TrigDecision> decisionReadHandle = SG::makeHandle(*m_decisionKeyPtr, context);
      contains_xAOD_decision = decisionReadHandle.isValid();
    }

#ifndef XAOD_ANALYSIS // Full Athena

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
      m_unpacker = std::make_unique<DecisionUnpackerStandalone>(m_decisionKeyPtr, m_run2NavigationKeyPtr);
    }
    else if( is_l1result_configured ){
      ATH_MSG_INFO("SG contains AOD decision, use DecisionUnpackerAthena");
      m_unpacker = std::make_unique<DecisionUnpackerAthena>(m_oldDecisionKeyPtr);
    }
    else if (contains_old_event_info) {
      ATH_MSG_INFO("SG contains NO(!) L1Result in the AOD TrigDecision, assuming also no HLTResult. Read from EventInfo");
      m_unpacker = std::make_unique<DecisionUnpackerEventInfo>(m_oldEventInfoKeyPtr);
    }
#else
    if ( contains_xAOD_decision ){
      ATH_MSG_INFO("SG contains xAOD decision, use DecisionUnpackerStandalone");
      m_unpacker = std::make_unique<DecisionUnpackerStandalone>(m_decisionKeyPtr, m_run2NavigationKeyPtr);
    }
#endif

  }//if(!m_unpacker)

  // cppcheck-suppress duplicateCondition
  if(!m_unpacker){
    std::stringstream extra;
#ifndef XAOD_ANALYSIS // Full Athena
    extra << ". Looked for old TrigDec::TrigDecision? "
      << (m_oldDecisionKeyPtr->empty() ? "NO" : "YES")
      << ", has TrigDec::TrigDecision? "
      << (contains_decision ? "YES" : "NO")
      << ", TrigDec::TrigDecision has L1? "
      << (is_l1result_configured ? "YES" : "NO")
      << ". Looked for old EventInfo? "
      << (m_oldEventInfoKeyPtr->empty() ? "NO" : "YES")
      << ", has old EventInto? "
      << (contains_old_event_info ? "YES" : "NO");
#endif
    ATH_MSG_ERROR("No source of Trigger Decision in file. "
      << "(Looked for xAOD::TrigDecision? "
      << (m_decisionKeyPtr->empty() ? "NO" : "YES")
      << ", has xAOD::TrigDecision? "
      << (contains_xAOD_decision ? "YES" : "NO")
      << extra.str()
      << ". Check UseRun1DecisionFormat and UseOldEventInfoDecisionFormat flags if reading pre-xAOD or BS input).");
    throw std::runtime_error("Trig::CacheGlobalMemory::assert_decision(): No source of Trigger Decision in file.");
  }

  if( !m_decisionUnpacked ) {
    // this method is locked
    auto nc_this ATLAS_THREAD_SAFE = const_cast<Trig::CacheGlobalMemory*>(this);
    if( nc_this->unpackDecision(context).isFailure() ) {
      ATH_MSG_WARNING( "TrigDecion object incorrect (for chains)" );
    }
    else{
      ATH_MSG_VERBOSE("unpacked decision");
    }
  }

  return true;
}

void Trig::CacheGlobalMemory::reset_decision() {
  m_decisionUnpacked = false;
  m_navigationUnpacked = false;
}

StatusCode Trig::CacheGlobalMemory::unpackDecision(const EventContext& ctx) {
  // locked already through assert_decision

  ATH_MSG_DEBUG("Unpacking TrigDecision");
  m_decisionUnpacked = true;
  m_deleteAtEndOfEvent.clear();

  bool unpackHLT = ( m_confChains != nullptr );
  ATH_CHECK( m_unpacker->unpackDecision( ctx, m_itemsByName, m_itemsCache,
          m_l2chainsByName, m_l2chainsCache,
          m_efchainsByName, m_efchainsCache,
          m_bgCode, unpackHLT ) );
  return StatusCode::SUCCESS;
}


StatusCode Trig::CacheGlobalMemory::unpackNavigation() {
  std::lock_guard<std::recursive_mutex> lock(m_cgmMutex);

  m_navigationUnpacked = true;

  // Navigation
  // protect from unpacking in case HLT was not run
  // (i.e. configuration chains are 0)
  if( m_confChains == nullptr ) {
    return StatusCode::SUCCESS;
  }

  // Failing to unpack the navigation is not a failure, as it may be missing
  // from the xAOD file:
  const EventContext& context = Gaudi::Hive::currentContext();
  if( ! m_unpacker->unpackNavigation( context, m_navigation ).isSuccess() ) {
    [[maybe_unused]] static std::atomic<bool> warningPrinted =
      [&]() { ATH_MSG_WARNING( "TrigNavigation unpacking failed" );
              return true; }();
  }
  // Return gracefully:
  return StatusCode::SUCCESS;
}

Trig::CacheGlobalMemory::AnyTypeDeleter::~AnyTypeDeleter()
{
  clear();
}
