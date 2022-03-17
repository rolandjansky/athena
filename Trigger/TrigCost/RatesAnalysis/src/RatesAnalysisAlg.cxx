/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// RatesAnalysis includes
#include "RatesAnalysis/RatesAnalysisAlg.h"
#include "EventInfo/EventStreamInfo.h"

#include "xAODEventInfo/EventInfo.h"

#include "TrigConfL1Data/BunchGroupSet.h"
#include "TrigConfData/HLTMenu.h"
#include "TrigConfData/L1Menu.h"
#include "TrigConfData/HLTPrescalesSet.h"
#include "TrigConfData/L1PrescalesSet.h"
//uncomment the line below to use the HistSvc for outputting trees and histograms
#include "GaudiKernel/ITHistSvc.h"
#include "TH1.h"

#include <sstream>


RatesAnalysisAlg::RatesAnalysisAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthAnalysisAlgorithm( name, pSvcLocator ),
  m_targetMu(0.),
  m_targetBunches(0.),
  m_targetLumi(0.),
  m_runNumber(0.),
  m_ratesDenominator(0),
  m_eventCounter(0),
  m_weightedEventCounter(0),
  m_scalingHist(nullptr),
  m_bcidHist(nullptr),
  m_metadataTree(nullptr)
{}

RatesAnalysisAlg::~RatesAnalysisAlg() {}

StatusCode RatesAnalysisAlg::newScanTrigger(const std::string& name,
  const double thresholdMin,
  const double thresholdMax,
  const uint32_t thresholdBins,
  const RatesScanTrigger::TriggerBehaviour_t behaviour,
  const double prescale,
  const std::string& seedName,
  const double seedPrecale,
  const Method_t method,
  const ExtrapStrat_t extrapolation) {

  if (method != kMANUAL) {
    ATH_MSG_ERROR("Sorry - ScanTriggers have to be implemented with the kMANUAL method for now.");
    return StatusCode::FAILURE;
  }

  const ExtrapStrat_t e = (m_enableLumiExtrapolation ? extrapolation : ExtrapStrat_t::kNONE); 
  m_scanTriggers.emplace(name, std::make_unique<RatesScanTrigger>(name, msgSvc(), thresholdMin, thresholdMax, thresholdBins, behaviour, prescale, seedName, seedPrecale, e));
  RatesScanTrigger* newScanTrigger = m_scanTriggers.at(name).get();
  if (isRandomSeed(name, seedName)) newScanTrigger->setSeedsFromRandom(true);
  ATH_MSG_DEBUG("newScanTrigger " <<  name << " added");
  return StatusCode::SUCCESS;
}


StatusCode RatesAnalysisAlg::newScanTrigger(const std::string& name,
  const std::vector<double>& thresholdBinEdges,
  const RatesScanTrigger::TriggerBehaviour_t behaviour,
  const double prescale,
  const std::string& seedName,
  const double seedPrecale,
  const Method_t method,
  const ExtrapStrat_t extrapolation) {

  if (method != kMANUAL) {
    ATH_MSG_ERROR("Sorry - ScanTriggers have to be implemented with the kMANUAL method for now.");
    return StatusCode::FAILURE;
  }

  const ExtrapStrat_t e = (m_enableLumiExtrapolation ? extrapolation : ExtrapStrat_t::kNONE); 

  m_scanTriggers.emplace(name, std::make_unique<RatesScanTrigger>(name, msgSvc(), thresholdBinEdges, behaviour, prescale, seedName, seedPrecale, e));
  RatesScanTrigger* newScanTrigger = m_scanTriggers.at(name).get();
  if (isRandomSeed(name, seedName)) newScanTrigger->setSeedsFromRandom(true);
  ATH_MSG_DEBUG("newScanTrigger " <<  name << " added");
  return StatusCode::SUCCESS;
}


StatusCode RatesAnalysisAlg::newTrigger(const std::string& name,
  const double prescale,
  const double expressPrescale,
  const std::string& seedName,
  const double seedPrecale,
  const std::string& groups,
  const Method_t method,
  const ExtrapStrat_t extrapolation) {

  std::set<std::string> groupSet;
  std::istringstream ss(groups);
  while (ss) {
    std::string group;
    if (!std::getline(ss, group, ',')) break;
    groupSet.insert(group);
  }

  return newTrigger(name, prescale, expressPrescale, seedName, seedPrecale, groupSet, method, extrapolation);
}

StatusCode RatesAnalysisAlg::newTrigger(const std::string& name,
  const double prescale,
  const double expressPrescale,
  const std::string& seedName,
  const double seedPrecale,
  const std::set<std::string>& groups,
  const Method_t method,
  const ExtrapStrat_t extrapolation) {

  if (m_eventCounter > 1) { // All triggers must be defined before we start looping over the sample
    ATH_MSG_FATAL("Too late to call newTrigger. All emulated triggers must be registered during ratesInitialize().");
    return StatusCode::FAILURE;
  }

  if (method == kEXISTING) ATH_CHECK( checkExistingTrigger(name, seedName) ); // Check this agrees with what is in the AOD

  // Check if it already exists
  if (m_triggers.count(name) == 1) {
    ATH_MSG_WARNING("Trigger " <<  name << " is already registered.");
    return StatusCode::SUCCESS;
  }

  const ExtrapStrat_t e = (m_enableLumiExtrapolation ? extrapolation : ExtrapStrat_t::kNONE); 

  m_triggers.emplace(name, std::make_unique<RatesTrigger>(name, msgSvc(), prescale, expressPrescale, seedName, seedPrecale, m_doHistograms, e));
  RatesTrigger* newTriggerPtr = m_triggers.at(name).get();

  if (isRandomSeed(name, seedName)) newTriggerPtr->setSeedsFromRandom(true);

  // Only worthwhile doing the remainder if the trigger is not disabled. Otherwise will slow everything down
  if (newTriggerPtr->getDisabled()) {
    ATH_MSG_DEBUG("newTrigger " <<  name << " added (disabled)");
    return StatusCode::SUCCESS;
  }  

  if (method == kAUTO) {
    m_autoTriggers.push_back(name);
  } else if (method == kEXISTING) {
    m_existingTriggers[name] = m_tdt->getChainGroup(name);
    m_lowerTrigger[name] = seedName;
  }

  // Add this trigger to its groups
  if (m_doTriggerGroups) {
    for (const std::string& group : groups) {
      // Ignore BW and PS groups
      if (group.find("BW") == 0 || group.find("PS") == 0) continue;
      if (m_groups.count(group) == 0) {
        m_groups.emplace(group, std::make_unique<RatesGroup>(group, msgSvc(), m_doHistograms, m_enableLumiExtrapolation));
        // As the group is formed from at least one active trigger - it must be active itself (counter example - CPS group of a PS=-1 trigger)
        m_activeGroups.insert( m_groups.at(group).get() );
      }
      m_groups.at(group)->addToGroup( newTriggerPtr );
      // For CPS, we let the trigger know that it is special
      if (isCPS(group)) {
        if (newTriggerPtr->getCPSID() != 0) ATH_MSG_WARNING("Trigger " << name << " can only be in one coherent prescale group.");
        newTriggerPtr->setCPS(group); // This changes the CPSID
        const size_t CPSID = newTriggerPtr->getCPSID();
        // Find the lowest prescale of any member in this CPS group
        if (m_lowestPrescale.count(CPSID) == 0) m_lowestPrescale[CPSID] = FLT_MAX;
        if (prescale < m_lowestPrescale[CPSID]) m_lowestPrescale[CPSID] = prescale;
      }
    }
  }

  // Add to total rates
  const uint32_t level = getLevel(name);
  if (m_doGlobalGroups) {
    if      (level == 2) m_globalGroups.at(m_l2GroupName)->addToGroup( newTriggerPtr );
    else if (level == 1) m_globalGroups.at(m_l1GroupName)->addToGroup( newTriggerPtr );
  }
  // Add to express group - if express prescale is enabled
  if (level == 2 && expressPrescale >= 1) {
    if (m_doGlobalGroups) m_globalGroups.at(m_expressGroupName)->addToGroup( newTriggerPtr );
    if (m_doExpressRates) m_expressTriggers.insert( newTriggerPtr );
  }

  ATH_MSG_DEBUG("newTrigger " <<  name << " added");
  return StatusCode::SUCCESS;
}

StatusCode RatesAnalysisAlg::addAllExisting() {
  ATH_CHECK(checkGotTDT());
  return addExisting(".*");
}

StatusCode RatesAnalysisAlg::addExisting(const std::string pattern) {
  // Check we have the TDT
  ATH_CHECK(checkGotTDT());

  const auto& triggers = m_tdt->getListOfTriggers(pattern);
  ATH_MSG_INFO("Read " << triggers.size() << " triggers from AOD.");

  // Check if chain was disabled in athena job
  const bool runWithPrescaleJSON = !m_prescalesJSON.value().empty();
  const TrigConf::HLTPrescalesSet& hltPrescalesSet = m_configSvc->hltPrescalesSet(Gaudi::Hive::currentContext());
  for( auto & p : hltPrescalesSet.data().get_child("prescales") ) {
    if ((!m_prescalesJSON.value().count(p.first) && !runWithPrescaleJSON) || hltPrescalesSet.prescale(p.first).prescale < 0){
      m_prescalesJSON[p.first]["prescale"] = hltPrescalesSet.prescale(p.first).prescale;

      //TODO - check also express prescales. can be done when TrigConf::HLTPrescale express stream methods are in place

      if (hltPrescalesSet.prescale(p.first).prescale < 0){
        ATH_MSG_WARNING("Trigger " << p.first << " disabled in supplied AOD file. DISABLING");
      }
    }
  }

  const TrigConf::L1PrescalesSet& l1PrescalesSet = m_configSvc->l1PrescalesSet(Gaudi::Hive::currentContext());
  for( auto & p : l1PrescalesSet.prescales() ) {
    if ((!m_prescalesJSON.value().count(p.first) && !runWithPrescaleJSON) || p.second.prescale < 0){
      m_prescalesJSON[p.first]["prescale"] = p.second.prescale;

      if (p.second.prescale < 0){
        ATH_MSG_WARNING("Trigger " << p.first << " disabled in supplied AOD file. DISABLING");
      }
    }
  }

  // Iterate over the triggers and add them
  for (const auto& trigger : triggers) {
    ATH_MSG_DEBUG("Considering " << trigger );
    const bool isHLT = (getLevel(trigger) == 2); 
    const auto trigConf = (isHLT ? m_tdt->ExperimentalAndExpertMethods().getChainConfigurationDetails(trigger) : nullptr);
    if (isHLT && !trigConf) {
      ATH_MSG_ERROR("Problem with TDT trig conf - cannot get details for " << trigger << ", will be ignored.");
      continue;
    }
    const std::string lowerName = (isHLT ? trigConf->lower_chain_name() : "");
    std::set<std::string> groups = std::set<std::string>(); // To be filled later from the HLTMenu

    if (isHLT) {
      // If this is a HLT item, we require it to be seeded by at most one item. This allows us to use a factorising rates algorithm
      if (lowerName.find(",") != std::string::npos) {
        ATH_MSG_WARNING("Can not add " << trigger << " due to multiple L1 seeds." );
        continue;
      }

      if (lowerName.empty()) {
        ATH_MSG_WARNING("Can not add " << trigger << " due to multiple L1 seeds: L1All" );
        continue;
      }
      
      // Check it also wasn't disabled in the reprocessing (e.g. auto prescaled out in a perf or tightperf menu)
      if (trigConf->prescale() < 1.) { // Note this prescale is from ATHENA
        ATH_MSG_DEBUG("Will not add " << trigger << ", it was disabled in the reprocessing.");
        continue;
      }

      ATH_CHECK(m_configSvc.isValid());
      const TrigConf::HLTMenu& hltMenu = m_configSvc->hltMenu(Gaudi::Hive::currentContext());

      TrigConf::HLTMenu::const_iterator chain = std::find_if(hltMenu.begin(), hltMenu.end(), [&] (const TrigConf::Chain& c) {return c.name() == trigger;});
      if (chain == hltMenu.end()){
        ATH_MSG_WARNING("Chain " << trigger << " not found in the menu!");
        continue;
      }

      std::vector<std::string> chainGroups = (*chain).groups();
      std::vector<std::string> chainStreams = (*chain).streams();

      ATH_MSG_DEBUG(" chain " << trigger << " has "  << chainGroups.size() << " groups and " << chainStreams.size() << " streams");

      groups.insert(chainGroups.begin(), chainGroups.end());
      for (const std::string& stream : chainStreams){
        groups.insert("STREAM:" + stream );
      }
    }

    // Get the prescale, express prescale and lower prescale. Note these prescales are from SUPPLIED JSON. 
    double prescale = 1., expressPrescale = -1., lowerPrescale = 1.;
    if (m_prescalesJSON.size() != 0) {
      if (m_prescalesJSON.value().count(trigger) == 0) {
        ATH_MSG_WARNING("Unable to find " << trigger << " in supplied JSON. DISABLING." );
        prescale = 0.;
      } else {
        prescale        = m_prescalesJSON[trigger]["prescale"];
	      expressPrescale = m_prescalesJSON[trigger]["prescale_express"];
      }
      if (isHLT) {
        if (m_prescalesJSON.value().count(lowerName) == 0) {
          ATH_MSG_WARNING("Unable to find " << trigger << "'s seed, " << lowerName << ", in supplied JSON. DISABLING." );
          lowerPrescale = 0.;
        } else {
          lowerPrescale = m_prescalesJSON[lowerName]["prescale"];
        }
      }
    }


    // We now have all the info needed to add this trigger
    ATH_MSG_DEBUG("Registering existing trigger " << trigger << " for automatic TDT based rates prediction." );
    ATH_CHECK( newTrigger(trigger, prescale, expressPrescale, lowerName, lowerPrescale, groups, kEXISTING) );
  }

  for (const auto& trigger : m_prescalesJSON) {
    if (trigger.second.at("prescale") > 0 &&  std::find(triggers.begin(), triggers.end(), trigger.first) == triggers.end()) {
       ATH_MSG_WARNING( "Trigger " << trigger.first << " in supplied JSON is NOT AVAILABLE in the supplied AOD file.");
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode RatesAnalysisAlg::checkGotTDT() {
  if (m_tdt.empty()){
    ATH_MSG_ERROR("TriggerDecisionTool is not available!");
    return StatusCode::FAILURE;
  }
  [[maybe_unused]] static std::atomic<bool> printed = [&]() {
    ATH_MSG_INFO("TDT contains: " << m_tdt->getListOfTriggers().size() << " triggers, "
                 << m_tdt->getListOfStreams().size() << " streams and "
                 << m_tdt->getListOfGroups().size() << " groups.");
    return true;
  }();
  return StatusCode::SUCCESS;
}


StatusCode RatesAnalysisAlg::checkExistingTrigger(const std::string& name, const std::string& seedName) {
  ATH_CHECK(checkGotTDT());
  const auto& triggers = m_tdt->getListOfTriggers(name);
  if (triggers.size() != 1) {
    ATH_MSG_FATAL("Unable to find existing trigger " << name << " in this AOD.");
    return StatusCode::FAILURE;
  }
  if (getLevel(name) == 1) return StatusCode::SUCCESS;
  // L1 items will crash if we call this on them.
  const auto trigConf = m_tdt->ExperimentalAndExpertMethods().getChainConfigurationDetails(triggers.at(0));
  if (trigConf->lower_chain_name() != seedName) {
    ATH_MSG_FATAL("Tried to register an existing trigger '" << name << "' seeding from '" << seedName << "' but in this AOD it seeds from '" << trigConf->lower_chain_name() << "'");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode RatesAnalysisAlg::setTriggerDesicison(const std::string& name, const bool triggerIsPassed, const bool triggerIsActive) {
  // Currently - we call execute on setPassed, so the user would be unable to overwrite a decision set e.g. by the TDT.
  // so for now we only accept positive decisions here.
  if (triggerIsPassed || triggerIsActive) {
    const auto iterator = m_triggers.find(name);
    if (iterator == m_triggers.end()) {
      ATH_MSG_ERROR("Cannot find trigger " << name << " did you call newTrigger for this in initialize?");
      return StatusCode::FAILURE;
    }
    iterator->second->setPassedAndExecute(triggerIsPassed, triggerIsActive, m_weightingValues); // There is logic in the RatesTrigger to prevent multiple calls per event by accident.
    m_activatedTriggers.insert( iterator->second.get() );
  }
  return StatusCode::SUCCESS;
}


StatusCode RatesAnalysisAlg::setTriggerDesicison(const std::string& name, const double threshold) {
  const auto iterator = m_scanTriggers.find(name);
  if (iterator == m_scanTriggers.end()) {
    ATH_MSG_ERROR("Cannot find scan-trigger " << name << " did you call newScanTrigger for this in initialize?");
    return StatusCode::FAILURE;
  }
  iterator->second->setPassedAndExecute(threshold, m_weightingValues); // There is logic in the RatesScanTrigger to prevent multiple calls per event by accident.
  m_activatedTriggers.insert( static_cast<RatesTrigger*>(iterator->second.get()));
  return StatusCode::SUCCESS;
}

StatusCode RatesAnalysisAlg::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");

  if (!m_tdt.empty()){
    ATH_CHECK( m_tdt.retrieve() );
  }

  if(!m_configSvc.empty()) {
    ATH_CHECK( m_configSvc.retrieve() );
  }

  ATH_CHECK( m_enhancedBiasRatesTool.retrieve() ); 

  if (m_doUniqueRates && !m_doGlobalGroups) {
    ATH_MSG_ERROR("DoUniqueRates=True requires DoGlobalGroups=True");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode RatesAnalysisAlg::populateTriggers() {  
  // Let user add their triggers
  ATH_MSG_INFO("Initializing User's Triggers (note: we are actually now in the event loop)");

  if (m_doGlobalGroups) {
    m_globalGroups.emplace(m_l1GroupName,      std::make_unique<RatesGroup>(m_l1GroupName, msgSvc(),      m_doHistograms, m_enableLumiExtrapolation));
    m_globalGroups.emplace(m_l2GroupName,      std::make_unique<RatesGroup>(m_l2GroupName, msgSvc(),      m_doHistograms, m_enableLumiExtrapolation));
    m_globalGroups.at(m_l2GroupName)->setDoCachedWeights( m_doUniqueRates ); // This extra sub-weight caching is only utilised by unique-rate groups
    if (m_doExpressRates) {
      m_globalGroups.emplace(m_expressGroupName, std::make_unique<RatesGroup>(m_expressGroupName, msgSvc(), m_doHistograms, m_enableLumiExtrapolation));
      m_globalGroups.at(m_expressGroupName)->setExpressGroup( true );
    }
  }

  // This runs the derived class's code to add whatever triggers are desired.
  // Should be calling newTrigger(...), newScanTrigger(...) or addExisting(...), addAllExisting().
  ATH_CHECK( ratesInitialize() ); 

  ATH_MSG_INFO("Computing coherent factors for coherent prescale groups.");
  // Now we are not going to get any more chains - we can fill in the coherent prescale factors
  for (const auto& trigger : m_triggers) {
    const size_t CPSID = trigger.second->getCPSID(); 
    if (CPSID != 0) trigger.second->setCoherentFactor( m_lowestPrescale.at(CPSID) );
  }

  if (m_doUniqueRates && m_doGlobalGroups) {
    ATH_MSG_INFO("Creating extra groups to calculate unique rates.");
    const RatesGroup* l2GroupPtr = m_globalGroups.at(m_l2GroupName).get(); // The finalised list of all HLT chains
    const RatesGroup* l1GroupPtr = m_globalGroups.at(m_l1GroupName).get(); // The finalised list of all L1 chains
    for (const auto& trigger : m_triggers) {
      const uint32_t level = getLevel(trigger.first);
      m_uniqueGroups.emplace(trigger.first, std::make_unique<RatesGroup>(trigger.first, msgSvc(), false, m_enableLumiExtrapolation)); // Each trigger gets its own unique group. No hist needed
      RatesTrigger* triggerPtr = m_triggers.at(trigger.first).get();
      RatesGroup* uniqueGroupPtr = m_uniqueGroups.at(trigger.first).get();
      triggerPtr->setUniqueGroup( uniqueGroupPtr ); // Create two-way links
      uniqueGroupPtr->setUniqueTrigger( triggerPtr ); // Create two-way links
      // Copy in the global rates topology and make note of the unique rates master group
      if      (level == 2) uniqueGroupPtr->duplicateChildren( l2GroupPtr );
      else if (level == 1) uniqueGroupPtr->duplicateChildren( l1GroupPtr );
      else continue;
      // Remove this one chain from the group (a unique rate is the rate of the entire menu minus one chain)
      uniqueGroupPtr->removeFromGroup( triggerPtr );
      if (getLevel(trigger.first) == 2) {
        // For HLT, we can be more computationally efficient by utilising cached info from the hlt group
        // We remove from the group all other L1 seeds except for the one seeding our chain.
        // This sub-weight is the only one which can change. The combined weight of all other L1 seeds
        // can be cached by the master group and fetched from there.
        uniqueGroupPtr->removeOtherL1( triggerPtr ); 
        uniqueGroupPtr->setUseCachedWeights(true);
      }
      // Efficiency - if the trigger is disabled, no need to actually calculate anything for it.
      if (trigger.second->getDisabled() == false) {
        m_activeGroups.insert( uniqueGroupPtr ); // Add this to the event loop
      }
    }
  }

  ATH_MSG_INFO("Retrieving HLT chain's ID and Group from HLT menu.");

  m_hltChainIDGroup.resize(m_triggers.size());
  for (size_t i = 0; i < m_triggers.size(); i++)
    m_hltChainIDGroup.at(i).resize(3);

  if(!m_configSvc.empty() && m_configSvc.isValid()) {
    const TrigConf::HLTMenu& hltmenu = m_configSvc->hltMenu( Gaudi::Hive::currentContext() );
    
    TrigConf::HLTMenu::const_iterator chain_itr = hltmenu.begin();
    TrigConf::HLTMenu::const_iterator chain_end = hltmenu.end();
    size_t c = 0;

    for( ; chain_itr != chain_end; ++chain_itr ) {
      std::string chainName = ( *chain_itr ).name() ;
      unsigned int chainID = ( *chain_itr ).counter();
      std::vector<std::string> chainGroups = ( *chain_itr ).groups();
      for (std::string& stream : (*chain_itr).streams()){
        chainGroups.push_back("STREAM:" + stream);
      }
      std::string singlechainGroups = "";
      for (unsigned int j=0; j < chainGroups.size(); ++j){
        if (j==0) singlechainGroups += chainGroups[j];
        else singlechainGroups += ", "+chainGroups[j];
      }
            
      m_hltChainIDGroup.at(c).at(0) = chainName;
      m_hltChainIDGroup.at(c).at(1) = std::to_string(chainID);
      m_hltChainIDGroup.at(c).at(2) = singlechainGroups;
      ++c;
    }
  }

  ATH_MSG_INFO("Retrieving L1 item's ID from L1 menu.");

  if(!m_configSvc.empty() && m_configSvc.isValid()) {
    const TrigConf::L1Menu& l1menu = m_configSvc->l1Menu( Gaudi::Hive::currentContext() );

    m_l1ItemID.resize(l1menu.size());
    for (size_t i = 0; i < l1menu.size(); i++) {
      // No groups for items
      m_l1ItemID.at(i).resize(2);
    }
    
    TrigConf::L1Menu::const_iterator item_itr = l1menu.begin();
    TrigConf::L1Menu::const_iterator item_end = l1menu.end();

    size_t c = 0;
    for( ; item_itr != item_end; ++item_itr ) {
      m_l1ItemID.at(c).at(0) =  (*item_itr).name();
      m_l1ItemID.at(c).at(1) = std::to_string((*item_itr).ctpId());
      ++c;
    }
  }

  // Print all triggers
  if (msgLevel(MSG::DEBUG)) {
    if (m_triggers.size()) {
      ATH_MSG_DEBUG("################## Configured to estimate rates for the following triggers:");
      for (const auto& trigger : m_triggers) ATH_MSG_DEBUG(trigger.second->printConfig());
    }
    if (m_scanTriggers.size()) {
      ATH_MSG_DEBUG("################## Configured to estimate rates for the following scan triggers:");
      for (const auto& trigger : m_scanTriggers) ATH_MSG_DEBUG(trigger.second->printConfig());
    }
    if (m_groups.size()) {
      ATH_MSG_DEBUG("################## Configured to estimate rates for the following groups of triggers:");
      for (const auto& group : m_groups) ATH_MSG_DEBUG(group.second->printConfig());
    }
    if (m_globalGroups.size()) {
      ATH_MSG_DEBUG("################## Configured to estimate rates for the following global groups of triggers:");
      for (const auto& group : m_globalGroups) ATH_MSG_DEBUG(group.second->printConfig());
    }
  }

  if (m_doHistograms) {
    ATH_MSG_DEBUG("################## Registering normalisation histogram:");
    m_scalingHist = new TH1D("normalisation",";;",3,0.,3.);
    ATH_CHECK( histSvc()->regHist("/RATESTREAM/normalisation", m_scalingHist) );
    m_bcidHist = new TH1D("bcid",";BCID;Events",3565,-.5,3564.5);
    ATH_CHECK( histSvc()->regHist("/RATESTREAM/bcid", m_bcidHist) );
    ATH_MSG_DEBUG("################## Registering metadata tree histogram:");
    ATH_CHECK( histSvc()->regTree("/RATESTREAM/metadata", std::make_unique<TTree>("metadata", "metadata")) );
    ATH_CHECK( histSvc()->getTree("/RATESTREAM/metadata", m_metadataTree) );
    if (m_triggers.size()) {
    ATH_MSG_DEBUG("################## Registering trigger histograms:");
      for (const auto& trigger : m_triggers) {
        if (!trigger.second->doHistograms()) continue; // Not all may be doing histograming
        std::string lvlSubdir = "";
        if (trigger.second->getName().find("L1") == 0){
          lvlSubdir = "Rate_ChainL1_HLT/";
        } else if (trigger.second->getName().find("HLT") == 0) {
          lvlSubdir = "Rate_ChainHLT_HLT/";
        }
        ATH_CHECK( trigger.second->giveDataHist(histSvc(), std::string("/RATESTREAM/All/" + lvlSubdir + trigger.first + "/data")) );
        ATH_CHECK( trigger.second->giveMuHist(histSvc(), std::string("/RATESTREAM/All/" + lvlSubdir + trigger.first + "/rateVsMu")) );
        if (m_useBunchCrossingData) ATH_CHECK( trigger.second->giveTrainHist(histSvc(), std::string("/RATESTREAM/All/" + lvlSubdir + trigger.first + "/rateVsTrain")) );
        else trigger.second->clearTrainHist();
      }
    }
    if (m_scanTriggers.size()) {
    ATH_MSG_DEBUG("################## Registering scan trigger histograms:");
      for (const auto& trigger : m_scanTriggers) {
        ATH_CHECK( trigger.second->giveThresholdHist(histSvc(), std::string("/RATESTREAM/ScanTriggers/" + trigger.first + "/rateVsThreshold")) );
      }
    }
    if (m_groups.size()) {
      ATH_MSG_DEBUG("################## Registering group histograms:");
      for (const auto& group : m_groups) {
        if (!group.second->doHistograms()) continue;
        std::string groupName = group.first;
        std::replace( groupName.begin(), groupName.end(), ':', '_');
        ATH_CHECK( group.second->giveDataHist(histSvc(), std::string("/RATESTREAM/All/Rate_Group_HLT/" + groupName + "/data")) );
        ATH_CHECK( group.second->giveMuHist(histSvc(), std::string("/RATESTREAM/All/Rate_Group_HLT/" + groupName + "/rateVsMu")) );
        if (m_useBunchCrossingData) ATH_CHECK( group.second->giveTrainHist(histSvc(), std::string("/RATESTREAM/All/Rate_Group_HLT/" + groupName + "/rateVsTrain")) );
        else group.second->clearTrainHist();
      }
    }
    if (m_globalGroups.size()) {
      ATH_MSG_DEBUG("################## Registering global group histograms:");
      for (const auto& group : m_globalGroups) {
        if (!group.second->doHistograms()) continue;
        ATH_CHECK( group.second->giveDataHist(histSvc(), std::string("/RATESTREAM/All/Rate_Group_HLT/RATE_GLOBAL_" + group.first + "/data")) );
        ATH_CHECK( group.second->giveMuHist(histSvc(), std::string("/RATESTREAM/All/Rate_Group_HLT/RATE_GLOBAL_" + group.first + "/rateVsMu")) );
        if (m_useBunchCrossingData) ATH_CHECK( group.second->giveTrainHist(histSvc(), std::string("/RATESTREAM/All/Rate_Group_HLT/RATE_GLOBAL_" + group.first + "/rateVsTrain")) );
        else group.second->clearTrainHist();
      }
    }
  }

  // Has the user set a lumi extrapolation? If not - set a default
  if (m_enableLumiExtrapolation && m_targetLumi == 0) setTargetLumi(1e34);

  // We now know the final lumi scaling so we can set the bunch scaling
  const uint32_t ebPairedBunches = m_enhancedBiasRatesTool->getPairedBunches();
  ATH_MSG_INFO("Number of paired bunches in input file:" << m_enhancedBiasRatesTool->getPairedBunches());
  m_weightingValues.m_bunchFactor = m_targetBunches / (double)ebPairedBunches;

  return StatusCode::SUCCESS;
}

StatusCode RatesAnalysisAlg::execute() {  
  ATH_MSG_DEBUG("Executing " << name() << " on event " << m_eventCounter << "...");
  if (m_eventCounter++ == 0) { // First time in execute loop - cannot access TDT before this.
    ATH_CHECK( populateTriggers() );
  }

  // Get event characteristics
  const xAOD::EventInfo* eventInfo(nullptr);
  uint32_t distance = 0;
  ATH_CHECK( evtStore()->retrieve(eventInfo, "EventInfo") );
  ATH_CHECK( m_enhancedBiasRatesTool->getDistanceIntoTrain(eventInfo, distance) );

  // Get the weighting & scaling characteristics
  m_weightingValues.m_enhancedBiasWeight = m_enhancedBiasRatesTool->getEBWeight(eventInfo);
  m_weightingValues.m_eventMu = std::ceil(eventInfo->actualInteractionsPerCrossing()); // This always seems to be a half integer
  m_weightingValues.m_eventLumi = m_enhancedBiasRatesTool->getLBLumi(eventInfo);
  m_weightingValues.m_isUnbiased = m_enhancedBiasRatesTool->isUnbiasedEvent(eventInfo);
  m_weightingValues.m_distanceInTrain = distance;
  m_weightingValues.m_eventLiveTime = m_enhancedBiasRatesTool->getEBLiveTime(eventInfo); 

  if (m_useBunchCrossingData && m_vetoStartOfTrain > 0 && m_weightingValues.m_distanceInTrain < m_vetoStartOfTrain) return StatusCode::SUCCESS;

  // Bunch factor doesn't change as a fn. of the run. Reminder: m_bunchFactor = m_targetBunches / (double)ebPairedBunches;
  m_weightingValues.m_muFactor = m_targetMu / m_weightingValues.m_eventMu;
  m_weightingValues.m_linearLumiFactor = m_targetLumi / m_weightingValues.m_eventLumi;
  m_weightingValues.m_expoMuFactor = m_weightingValues.m_bunchFactor * exp( m_expoScalingFactor * (m_targetMu - m_weightingValues.m_eventMu) );

  // Ignore zero weighted events. Typically these come from bad LB
  if (RatesHistoBase::isZero(m_weightingValues.m_enhancedBiasWeight)) return StatusCode::SUCCESS;

  // Do automated triggers 
  ATH_CHECK( executeTriggerEmulation() );

  // Do TDT-controlled triggers
  ATH_CHECK( executeTrigDecisionToolTriggers() );

  // Run user's code. Do manual triggers
  ATH_CHECK( ratesExecute() );

  // Execute groups
  for (const auto& group : m_globalGroups) group.second->execute(m_weightingValues); // Physics, L1, express: Must execute before m_uniqueGroups (which are in active groups). Map.
  for (const auto& group : m_activeGroups) group->execute(m_weightingValues); // Individual groups, CPS groups and active unique groups. Set.

  // Reset triggers
  for (const auto& trigger : m_activatedTriggers) trigger->reset();
  m_activatedTriggers.clear();

  // Keep track of elapsed walltime
  m_ratesDenominator += m_weightingValues.m_eventLiveTime;
  m_weightedEventCounter += m_weightingValues.m_enhancedBiasWeight;

  if (m_doHistograms) {
    m_bcidHist->Fill(eventInfo->bcid(), m_weightingValues.m_enhancedBiasWeight);
    m_scalingHist->Fill(0.5, m_weightingValues.m_eventLiveTime); // Walltime
    m_scalingHist->Fill(1.5, 1.); // Total events
    m_scalingHist->Fill(2.5, m_weightingValues.m_enhancedBiasWeight); // Total events weighted
  }

  // Some debug info
  if (m_eventCounter % 1000 == 0) {
    ATH_MSG_INFO( "Event " << m_eventCounter << " " << m_weightingValues.print() << " currentWallTime:" << m_ratesDenominator );
  }

  setFilterPassed(true); //if got here, assume that means algorithm passed
  return StatusCode::SUCCESS;
}

StatusCode RatesAnalysisAlg::executeTrigDecisionToolTriggers() {
  for (const auto& trigger : m_existingTriggers) {
    const bool passed = trigger.second->isPassed();
    // L1 chains are always active, HLT chains are active if their L1 passed.
    const std::string& lower = m_lowerTrigger[trigger.first];
    // Expect this find operation to fail for L1 chains (lower = "")
    const std::unordered_map<std::string, const Trig::ChainGroup*>::const_iterator it = m_existingTriggers.find(lower);
    const bool active = (it == m_existingTriggers.end() ? true : it->second->isPassed());
    ATH_CHECK( setTriggerDesicison(trigger.first, passed, active) );
  }
  return StatusCode::SUCCESS;
}

StatusCode RatesAnalysisAlg::executeTriggerEmulation() {
  // TODO emulation code here
  for (const auto& trigger : m_autoTriggers) {
    ATH_MSG_WARNING("Cannot do rates for " << trigger << ". Automatic trigger emulation is not yet included, sorry :(");
  }
  return StatusCode::SUCCESS;
}

StatusCode RatesAnalysisAlg::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  ATH_CHECK( ratesFinalize() );
  if (m_scanTriggers.size()) {
    ATH_MSG_INFO("################## Computed Rate Scans for Threshold-Scan Items:");
    for (const auto& trigger : m_scanTriggers) ATH_MSG_INFO(trigger.second->printRate(m_ratesDenominator));
  }
  if (m_triggers.size()) {
    ATH_MSG_INFO("################## Computed Rate Estimations for Single Items:");
    std::set<std::string> keys; // Used an unordered map for speed, but now we'd like the items in order
    for (const auto& trigger : m_triggers) keys.insert(trigger.first);
    for (const std::string& key : keys) ATH_MSG_INFO(m_triggers.at(key)->printRate(m_ratesDenominator));
  }
  if (m_expressTriggers.size()) {
    ATH_MSG_INFO("################## Computed Express Rate Estimations for Single Items:");
    for (const auto& trigger : m_expressTriggers) ATH_MSG_INFO(trigger->printExpressRate(m_ratesDenominator));
  }
  if (m_groups.size()) {
    ATH_MSG_INFO("################## Computed Rate Estimations for Groups:");
    for (const auto& group : m_groups) ATH_MSG_INFO(group.second->printRate(m_ratesDenominator));
  }
  if (m_globalGroups.size()) {
    ATH_MSG_INFO("################## Computed Rate Estimations for Global Groups:");
    for (const auto& group : m_globalGroups) ATH_MSG_INFO(group.second->printRate(m_ratesDenominator));
  }
  ATH_MSG_INFO("################## LHC Conditions and weighting information:");
  printInputSummary();
  printTarget();
  printStatistics();
  ATH_MSG_INFO("##################");

  writeMetadata();

  return StatusCode::SUCCESS;
}

void RatesAnalysisAlg::setTargetLumiMu(const double lumi, const double mu) {
  if (m_eventCounter > 1) { // All settings must be defined before we start looping over the sample
    ATH_MSG_WARNING("Too late to call setTargetLumiMu. Do this during ratesInitialize().");
    return;
  }
  m_targetLumi = lumi;
  if (isZero(mu)) {
    ATH_MSG_WARNING("Cannot have <mu> = 0. Setting to 1.");
    m_targetMu = 1;
  } else {
    m_targetMu = mu;
  }
  m_targetBunches = round((lumi * m_inelasticCrossSection) / (mu * EnhancedBiasWeighter::LHC_FREQUENCY));
  if (m_targetBunches == 0 || m_targetBunches > EnhancedBiasWeighter::FULL_RING) {
    ATH_MSG_WARNING("Un-physical number of bunches " << m_targetBunches << ", should be within 1 < N < " << EnhancedBiasWeighter::FULL_RING+1);
    if (m_targetBunches == 0) ++m_targetBunches;
  }
  printTarget();
}

void RatesAnalysisAlg::setTargetLumiBunches(const double lumi, const int32_t bunches) {
  if (m_eventCounter > 1) { // All settings must be defined before we start looping over the sample
    ATH_MSG_WARNING("Too late to call setTargetLumiBunches. Do this during ratesInitialize().");
    return;
  }
  m_targetLumi = lumi;
  if (bunches == 0) {
    ATH_MSG_WARNING("Cannot have bunches = 0. Setting to 1.");
    m_targetBunches = 1;
  } else {
    m_targetBunches = bunches;
  }
  m_targetMu = (lumi * m_inelasticCrossSection) / (bunches * EnhancedBiasWeighter::LHC_FREQUENCY);
  printTarget();
}


void RatesAnalysisAlg::setTargetMuBunches(const double mu, const int32_t bunches) {
  if (m_eventCounter > 1) { // All settings must be defined before we start looping over the sample
    ATH_MSG_WARNING("Too late to call setTargetMuBunches. Do this during ratesInitialize().");
    return;
  }
  if (bunches == 0) {
    ATH_MSG_WARNING("Cannot have paired bunches = 0. Setting to 1.");
    m_targetBunches = 1;
  } else {
    m_targetBunches = bunches;
  }
  if (isZero(mu)) {
    ATH_MSG_WARNING("Cannot have mu = 0. Setting to 1.");
    m_targetMu = 1;
  } else {
    m_targetMu = mu;
  }
  m_targetLumi = (mu * bunches * EnhancedBiasWeighter::LHC_FREQUENCY) / m_inelasticCrossSection;
  printTarget();
}

void RatesAnalysisAlg::printTarget() const {
  if (m_enableLumiExtrapolation) {
    ATH_MSG_INFO("Calculating rates for a target L_inst. = " << m_targetLumi << " cm-2s-1, mu = " << m_targetMu << ", paired bunches = " << m_targetBunches);
  } else {
    ATH_MSG_INFO("Luminosity extrapolation is switched off. Input files will determin the paired bunches, L_inst. and mu profile.");
  }
}

void RatesAnalysisAlg::printStatistics() const {
  ATH_MSG_INFO("Processed " << m_eventCounter << " raw events, " << m_weightedEventCounter << " weighted. Total LHC wall-time of " << m_ratesDenominator << " s.");
}

void RatesAnalysisAlg::printInputSummary() const {
  ATH_MSG_INFO("Input " << (m_enhancedBiasRatesTool->isMC() ? "MC" : "EB Data") 
    << " with <L_inst.> = "
    << m_enhancedBiasRatesTool->getAverageLumi() 
    << " cm-2s-1, <mu> = "
    << m_enhancedBiasRatesTool->getAverageMu()
    << ", paired bunches = " 
    << m_enhancedBiasRatesTool->getPairedBunches());
}

bool RatesAnalysisAlg::isCPS(const std::string& group) const {
  return (group.find("CPS") != std::string::npos);
}

bool RatesAnalysisAlg::isRandomSeed(const std::string& me, const std::string& seed) const {
  if (me.find("L1_RD") != std::string::npos) return true;
  if (me.find("L1RD") != std::string::npos) return true;
  if (seed.find("L1_RD") != std::string::npos) return true;
  return false;
}

uint32_t RatesAnalysisAlg::getLevel(const std::string& name) const {
  if (name.find("HLT_") != std::string::npos) return 2;
  if (name.find("L1_") != std::string::npos) return 1;
  return 2;
}

void RatesAnalysisAlg::writeMetadata() {
  if (!m_metadataTree) {
    return;
  }

  m_runNumber = m_enhancedBiasRatesTool->getRunNumber();
  m_metadataTree->Branch("runNumber", &m_runNumber);
  
  m_metadataTree->Branch("targetMu", &m_targetMu);
  m_metadataTree->Branch("targetBunches", &m_targetBunches);
  m_metadataTree->Branch("targetLumi", &m_targetLumi);

  std::vector<std::string> triggers;
  std::vector<std::string> lowers;
  std::vector<double> prescales;
  std::vector<double> express;
  triggers.reserve(m_triggers.size());
  lowers.reserve(m_triggers.size());
  prescales.reserve(m_triggers.size());
  express.reserve(m_triggers.size());
  for (const auto& trigger : m_triggers) {
    triggers.push_back(trigger.first);
    lowers.push_back(trigger.second->getSeedName());
    prescales.push_back(trigger.second->getPrescale() );
    express.push_back(trigger.second->getPrescale(true /*includeExpress*/) );
  }

  for (const auto& group : m_groups) {
    triggers.push_back(group.first);
    lowers.push_back("-");
    prescales.push_back(-1);
    express.push_back(-1);
  }

  for (const auto& group : m_globalGroups) {
    triggers.push_back("RATE_GLOBAL_" + group.first);
    lowers.push_back("-");
    prescales.push_back(-1);
    express.push_back(-1);
  }

  m_metadataTree->Branch("triggers", &triggers);
  m_metadataTree->Branch("lowers", &lowers);
  m_metadataTree->Branch("prescales", &prescales);
  m_metadataTree->Branch("express", &express);

  std::vector<int32_t> bunchGroups;
  bunchGroups.reserve(16);

  uint32_t masterKey = 0;
  uint32_t hltPrescaleKey = 0;
  uint32_t lvl1PrescaleKey = 0;

  if(!m_configSvc.empty() && m_configSvc.isValid()) {
    const TrigConf::BunchGroupSet* bgs = m_configSvc->bunchGroupSet();
    for (const TrigConf::BunchGroup& bg : bgs->bunchGroups()) {
      bunchGroups.push_back(bg.bunches().size());
    }
    masterKey = m_configSvc->masterKey();
    hltPrescaleKey = m_configSvc->hltPrescaleKey();
    lvl1PrescaleKey = m_configSvc->lvl1PrescaleKey();
  }

  if (bunchGroups.size() == 0 || std::all_of(bunchGroups.begin(), bunchGroups.end(), [](int i) { return i==0; })) {
    bunchGroups = m_enhancedBiasRatesTool->getBunchGroups();
  }

  m_metadataTree->Branch("bunchGroups", &bunchGroups);

  m_metadataTree->Branch("hltChainIDGroup", &m_hltChainIDGroup);
  m_metadataTree->Branch("l1ItemID", &m_l1ItemID);

  m_metadataTree->Branch("masterKey", &masterKey);
  m_metadataTree->Branch("lvl1PrescaleKey", &lvl1PrescaleKey);
  m_metadataTree->Branch("hltPrescaleKey", &hltPrescaleKey);

  std::string atlasProject = std::getenv("AtlasProject");
  std::string atlasVersion = std::getenv("AtlasVersion");
  m_metadataTree->Branch("AtlasProject", &atlasProject);
  m_metadataTree->Branch("AtlasVersion", &atlasVersion);

  m_metadataTree->Fill();

}
