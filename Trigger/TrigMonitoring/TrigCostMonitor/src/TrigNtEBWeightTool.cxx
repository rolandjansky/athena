/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Framework
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

// Trigger
#include "TrigSteeringEvent/Chain.h"
#include "TrigSteeringEvent/HLTResult.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"
#include "TrigMonitoringEvent/TrigMonConfig.h"

// Find XML
#include "PathResolver/PathResolver.h"

// XML parse & unweighting specific
#include <TObjString.h>
#include <TDOMParser.h>
#include <TXMLNode.h>
#include <TXMLDocument.h>
#include <TXMLAttr.h>
#include <TString.h>
#include <TMath.h>

// Local
#include "TrigCostMonitor/TrigNtEBWeightTool.h"

// Std
#include <sstream>

//---------------------------------------------------------------------------------------
Trig::TrigNtEBWeightTool::TrigNtEBWeightTool(const std::string &name,
				     const std::string &type,
				     const IInterface  *parent)
  :AlgTool(name, type, parent),
   m_randomSeedWeight(8), // XXX TODO CUSTOMISE
   m_filterInfo(),
   m_chainPrescale(),
   m_chainL1Seeds(),
   m_L1NameToCPTID(),
   m_log(0),
   m_isRun1(1),
   m_isConfigured(0)
{
  declareInterface<Trig::ITrigNtTool>(this);

}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtEBWeightTool::initialize()
{    
  //
  // Retrieve my tools and services
  //
  m_log = new MsgStream(msgSvc(), name());

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtEBWeightTool::finalize()
{
  //
  // Clean up
  //  
  if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << "finalize()" << endreq;
  
  delete m_log; m_log = 0;

  return StatusCode::SUCCESS;
}

namespace Trig { // Helper function
  float GetPrescale(const std::vector<TrigConfChain>& _chains, std::string _toFind, MsgStream* _log) {
    std::vector<TrigConfChain>::const_iterator _it;
    for (_it = _chains.begin(); _it != _chains.end(); ++_it) { 
      TrigConfChain _chain = (*_it);
      if (_chain.getChainName() != _toFind) continue;
      (*_log) << MSG::DEBUG << "--- Tabulating prescale " << _toFind << " PS:" << _chain.getPrescale() << endreq;
      return _chain.getPrescale();
    }
    (*_log) << MSG::ERROR << "!!! Cannot find PS for " << _toFind << endreq;
    return 1.;
  }
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtEBWeightTool::Fill(TrigMonConfig *config)
{
  m_isConfigured = false;

  if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << "In Trig::TrigNtEBWeightTool::Fill(TrigMonConfig)" << endreq;
  m_chainL1Seeds.clear();
  m_chainPrescale.clear();

  // Check run number to see if run 1 or run 2. Boundary is M5 2014
  if (config->getRun() < 239708) m_isRun1 = true;
  else                           m_isRun1 = false;

  if (m_filterInfo.size() == 0) { // Load filter maps
    readEBConfigFromXML(config->getRun());
  }

  // For each EB type
  for (int _EB = 0; _EB < kEbTrggerType_SIZE; ++_EB) {
    EBTriggerType _EBEnum = static_cast<EBTriggerType>(_EB);

    if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << "- Getting prescale for " << EBTriggerNameHLT[_EB] << endreq;
    m_chainEnabled[_EBEnum] = true; // Start off assuming enabled
    
    // Get the L1 chain
    // This depends on the _origional_ menu. So for all data12, it will be L2_ even if reprocessed.
    // Loop over all chains

    std::string _EBChain;
    std::string _L1Chain;
    if (m_isRun1 == true) {
      _EBChain = EBTriggerNameL2[_EB];
    } else {
      _EBChain = EBTriggerNameHLT[_EB];
    }

    const std::vector<TrigConfChain> _configChains = config->getVec<TrigConfChain>();
    std::vector<TrigConfChain>::const_iterator _it;
    // Loop over all chains and find this one
    for (_it = _configChains.begin(); _it != _configChains.end(); ++_it) {
      TrigConfChain _chain = (*_it);
      if (_chain.getChainName() != _EBChain) continue;
      _L1Chain = _chain.getLowerName();
      break;
    }

    if (_L1Chain == "") {
      log() << MSG::ERROR << "-- Cannot find L1 item for " << _EBChain << " in the current menu" << endreq;
    } else {
      if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << "-- Associated EBChain " << _EBChain << " with " << _L1Chain << endreq;
    }
    m_chainL1Seeds[_EBEnum] = _L1Chain;


    // Tabulate the total prescale
    float _totalPrescale = 1, _PS;
    if (m_isRun1 == true) {
      _PS = Trig::GetPrescale(_configChains, EBTriggerNameEF[_EB], m_log); // Get EF level
      if (_PS < 0) m_chainEnabled[_EBEnum] = false;
      _totalPrescale *= _PS;
      //
      _PS = Trig::GetPrescale(_configChains, EBTriggerNameL2[_EB], m_log); // Get L2 level
      if (_PS < 0) m_chainEnabled[_EBEnum] = false;
      _totalPrescale *= _PS;
    } else {
      _PS = Trig::GetPrescale(_configChains, EBTriggerNameHLT[_EB], m_log); // Get HLT level
      if (_PS < 0) m_chainEnabled[_EBEnum] = false;
      _totalPrescale *= _PS;
    }
   
    // If not "noL1PS" then also include the L1 prescale
    if ( _EBEnum != kEb_physics_noL1PS &&
         _EBEnum != kEb_physics_empty && // This should be PS=1!!!!
         _EBEnum != kEb_physics_firstempty && // This should be PS=1!!!!
         _EBEnum != kEb_physics_unpaired_iso && // This should be PS=1!!!!
         _EBEnum != kEb_physics_unpaired_noniso ) { // This should be PS=1!!!!
      _PS = Trig::GetPrescale(_configChains, m_chainL1Seeds[_EBEnum], m_log);
      if (_PS < 0) m_chainEnabled[_EBEnum] = false;
      _totalPrescale *= _PS;
      
      // If seeded from a random trigger, then include a factor of 8 (correct 2014).
      // This is because the random clock runs at 5 MHz rater than 40.
      if ( m_chainL1Seeds[_EBEnum].find("RD0") != std::string::npos
           || m_chainL1Seeds[_EBEnum].find("RD1") != std::string::npos) {
        _totalPrescale *= m_randomSeedWeight;
        if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << "---- Random Seeded: Including an addtional PS factor of " << m_randomSeedWeight << endreq;
      }
    } else {
      if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << "---- noL1PS chain, not adding any additional PS for L1." << endreq;
    }
    
    m_chainPrescale[_EBEnum] = _totalPrescale;
    if (m_chainEnabled[_EBEnum] == true) {
      log() << MSG::INFO << "==== Enhanced Bias Chain " << EBTriggerNameHLT[_EB] << " has total prescale " << m_chainPrescale[_EBEnum] << endreq;
    } else {
      log() << MSG::INFO << "==== Enhanced Bias Chain " << EBTriggerNameHLT[_EB] << " is DISABLED (PS < 0 in chain)" << endreq;
    }
  }

  // Get the CTPID mapping for L1 items
  m_L1NameToCPTID.clear();
  std::vector<TrigConfChain>::const_iterator _it;
  for (_it = config->getVec<TrigConfChain>().begin(); _it != config->getVec<TrigConfChain>().end(); ++_it) { 
    TrigConfChain _chain = (*_it);
    if (_chain.getLevelId() != 1) continue;
    m_L1NameToCPTID[_chain.getChainName()] = _chain.getCounter();
    m_CPTIDToL1Name[_chain.getCounter()] = _chain.getChainName();

    // Check for signs of Bunch Group. We're going to just go by the name here as we don't keep BG info in this summary.
    // Should be fine - can be readdressed in future if there are problems.
    if (_chain.getChainName().find("_BPTX") != std::string::npos 
      || _chain.getChainName().find("_BGRP") != std::string::npos
      || _chain.getChainName().find("_L1A_Mon") != std::string::npos) { // Ignore the beam pickup triggers and "L1A_MON"
      m_L1IDToBunchGroup[_chain.getCounter()] = kBG_NONE;
    } else if (_chain.getChainName().find("_FIRSTEMPTY") != std::string::npos) {
      m_L1IDToBunchGroup[_chain.getCounter()] = kBG_FIRSTEMPTY;
    } else if (_chain.getChainName().find("_EMPTY") != std::string::npos) {
      m_L1IDToBunchGroup[_chain.getCounter()] = kBG_EMPTY;
    } else if (_chain.getChainName().find("_UNPAIRED_ISO") != std::string::npos) {
      m_L1IDToBunchGroup[_chain.getCounter()] = kBG_UNPAIRED_ISO;
    } else if (_chain.getChainName().find("_UNPAIRED_NONISO") != std::string::npos) {
      m_L1IDToBunchGroup[_chain.getCounter()] = kBG_UNPAIRED_NONISO;
    } else {
      m_L1IDToBunchGroup[_chain.getCounter()] = kBG_FILLED;
    }

    if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << "Recording L1Chain Name -> CPTID [" << _chain.getChainName() << " -> " << _chain.getCounter() << "], ID -> BG [" << _chain.getCounter() << " -> " << EBBunchGroupNames[ m_L1IDToBunchGroup[_chain.getCounter()] ] << "]" << endreq;
  }

  m_isConfigured = true;

  return true;
}

//---------------------------------------------------------------------------------------
void Trig::TrigNtEBWeightTool::readEBConfigFromXML(unsigned runNumber) {
  m_filterInfo.clear();

  if (runNumber != 212967 
   && runNumber != 247110) {
    log() << MSG::ERROR << "EB configuration for run " << runNumber << " is not available automatically. XML file needs to be supplied." << endreq;
  }

  // Load XML info
  std::stringstream ss;
  ss << "enhanced_bias_run_";
  ss << runNumber;
  ss << ".xml";
  std::string _xmlName;
  ss >> _xmlName;
  const std::string _locAthena = PathResolverFindDataFile( _xmlName );
         
  log() << MSG::INFO << "EB weighting XML: " << _xmlName << " at " << _locAthena << endreq;
  
  TDOMParser* _xmlParser = new TDOMParser();
  _xmlParser->SetValidate(kFALSE);
  _xmlParser->ParseFile( _locAthena.c_str() );
  
  TXMLDocument* _xmlDoc = _xmlParser->GetXMLDocument();
  TXMLNode* _rootNode = _xmlDoc->GetRootNode();
  TXMLNode* _children = _rootNode->GetChildren();
  TXMLNode* _filtersNode = 0;
  // Navigate down XML, find "filters" node
  while (_children != 0) {
    if (_children->GetNodeName() == TString("filters")) {
      // We should only find this node once, if we are finding it twice that's bad
      assert( _filtersNode == 0 );
      _filtersNode = _children;
    }
    _children = _children->GetNextNode();
  }
  
  // We should now have identified the filters node in the XML. Time to read from it.
  // TODO make this nicer if being kept
  assert( _filtersNode != 0 );
  assert( _filtersNode->HasChildren() );
  TXMLNode* _filtersChildren = _filtersNode->GetChildren();
  // Loop over all "filter" under "filters"
  while (_filtersChildren != 0) {
    TXMLNode* _filter = 0;
    if (_filtersChildren->HasChildren()) {
      _filter = _filtersChildren->GetChildren();
    }
    TString _currentFilter = "";
    
    // Loop over all children of a single "filter"
    while (_filter != 0) {
      if (_filter->GetNodeName() == TString("filter_name")) {
        if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << "Parsing an EB filter: " << _filter->GetText() << endreq;
        _currentFilter = _filter->GetText();
      } else if (_filter->GetNodeName() == TString("lv1_filter") && _currentFilter != "") {
        // Save mapping of this EB filter (e.g. EF_eb_physics) to this L1 Item (e.g. L1_RD1_FILLED)
        m_filterInfo.insert(std::make_pair( std::string(_currentFilter), std::string(_filter->GetText()) ));
        if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << "Regular Filter: " <<  _currentFilter.Data() << " -> " << _filter->GetText() << endreq;
      } else if (_filter->GetNodeName() == TString("eb_hlt_chains") ) {
        // Look into this node too
        TXMLNode* _eb_hlt_chains = 0;
        if (_filter->HasChildren()) {
          _eb_hlt_chains = _filter->GetChildren();
        }
        while (_eb_hlt_chains != 0) {
          if (_eb_hlt_chains->GetNodeName() == TString("eb_hlt_name")) {
            _currentFilter = TString("L1ItemNames ") + _eb_hlt_chains->GetText();
          } else if (_eb_hlt_chains->GetNodeName() == TString("lv1_ebtrigs") && _currentFilter != "") {
            m_filterInfo.insert(std::make_pair( std::string(_currentFilter), std::string(_eb_hlt_chains->GetText()) ));
            if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << "EBTRIGS Filter: " << _currentFilter.Data() << " -> " << _eb_hlt_chains->GetText() << endreq;
          }
          _eb_hlt_chains = _eb_hlt_chains->GetNextNode();
        }
      }
      _filter = _filter->GetNextNode();
    }
    _filtersChildren = _filtersChildren->GetNextNode();
  }
  return;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtEBWeightTool::Fill(TrigMonEvent &event)
{

    if (m_isConfigured == false) {
      log() << MSG::WARNING << "Cannot do Enhanced Bias weighting, tool failed to configure." << endreq;
      return false;
    }

    // First, what BG type were we? Go based on what passed
    EBBunchGroupType _eventBG = kBG_NONE;
    const std::vector<TrigMonL1Item>& _eventL1Items = event.getL1Items();
    for (unsigned i = 0; i < _eventL1Items.size(); ++i) {
      unsigned _ID = _eventL1Items.at(i).getCtpId();
      EBBunchGroupType _BG = m_L1IDToBunchGroup[ _ID ];
      if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << "Event has L1 item " << m_CPTIDToL1Name[_ID] 
        << ", passes? " << _eventL1Items.at(i).isPassedBeforePrescale() 
        << ", and is in BG " << EBBunchGroupNames[_BG] << endreq;
      if (_BG == kBG_NONE) continue; 
      if (_eventL1Items.at(i).isPassedBeforePrescale() == false) continue;
      if (_eventBG != kBG_NONE && _eventBG != _BG) {
        log() << MSG::ERROR << "Event has mismatched bunchgroups in it, both " 
          << EBBunchGroupNames[_BG] << " and " 
          << EBBunchGroupNames[_eventBG] << endreq;
        for (unsigned j = 0; j < _eventL1Items.size(); ++j) {
          log() << MSG::ERROR << "-> L1 item " << m_CPTIDToL1Name[_eventL1Items.at(j).getCtpId()] 
            << ", passes? " << _eventL1Items.at(j).isPassedBeforePrescale() 
            << ", and is in BG " << EBBunchGroupNames[ m_L1IDToBunchGroup[ _eventL1Items.at(j).getCtpId() ] ] << endreq;
        }
      }
      _eventBG = _BG;
    }

    // Check we could assign the BG
    if (_eventBG == kBG_NONE) {
      log() << MSG::ERROR << "Unable to determine the bunch group for this event. L1 items were:";
      for (unsigned i = 0; i < _eventL1Items.size(); ++i) {
        log() << MSG::ERROR << "-> L1 item " << m_CPTIDToL1Name[_eventL1Items.at(i).getCtpId()] 
          << ", passes? " << _eventL1Items.at(i).isPassedBeforePrescale() 
          << ", and is in BG " << EBBunchGroupNames[ m_L1IDToBunchGroup[ _eventL1Items.at(i).getCtpId() ] ] << endreq;
      }
    }

    // We look to see what L1 items *could* have passed (trigger before prescale) and assign a weight based on the total prescale of the chain.
    std::map<EBTriggerType, Bool_t> _passEBTrigBeforePS;
    
    Double_t _weight = 1.; // Note - this MUST be a double, can be stored as a float after calculations
    
    // For each EB type
    for (int _EB = 0; _EB < kEbTrggerType_SIZE; ++_EB) {
      EBTriggerType _EBEnum = static_cast<EBTriggerType>(_EB);
      
      // We do random on its own at the end
      if ( _EBEnum == kEb_random || 
           _EBEnum == kEb_random_firstempty || 
           _EBEnum == kEb_random_empty ||
           _EBEnum == kEb_random_unpaired_iso) continue;

      if ( m_chainEnabled[_EBEnum] == false) continue;
      
      // Get the string we're searching for in the EB unweighting map.
      // For eb_physics and high_eb_physics (the random seeded chains), this is based on the internal configuration of the chain. This info is prefixed with "L1ItemNames" in the map.
      // For eb_physics_noL1PS, it's simply the L1 items which seed this trigger.
      std::string _EBMappingName = EBMappingNameEF[_EB]; // in Run-1
      if (m_isRun1 == false) {
        _EBMappingName = EBMappingNameHLT[_EB]; // In Run-2
      }
      
      // Start off assuming that L1 would not pass this event
      _passEBTrigBeforePS[_EBEnum] = false;
      
      // Loop over all elements of the map
      for ( std::map<std::string, std::string>::iterator it = m_filterInfo.begin(); it != m_filterInfo.end(); ++it) {
        std::string _HLTName = it->first;
        std::string _L1Name = it->second;

        // If this element of the map corresponds to the EB trigger we're currently investigating
        if (_HLTName != _EBMappingName ) continue;

        // Get CTPID for this L1 item
        if (m_L1NameToCPTID.count( _L1Name ) == 0) {
          log() << MSG::ERROR << "Cannot look up in stored menu the CTPID for chain " << _L1Name << endreq;
          return false;
        }
        unsigned _L1CPTID = m_L1NameToCPTID[ _L1Name ];
        
        // If this element of the map was *not* referring to a random seed.
        if (_L1Name.find("RD0") != std::string::npos || _L1Name.find("RD1") != std::string::npos)  continue;
        
        // Get if this L1 item *could* have passed the event.
        //if ( _costData_L1.getIsL1PassedBeforePrescale( _L1Name ) == kFALSE ) continue;
        Int_t _passedL1Raw = 0;
        for (unsigned i = 0; i < _eventL1Items.size(); ++i) {
          if (_eventL1Items.at(i).getCtpId() != _L1CPTID) continue;
          _passedL1Raw = _eventL1Items.at(i).isPassedBeforePrescale();
          if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << "L1Item " << _L1Name << " considered for " << _EBMappingName << (_passedL1Raw ? " PASSED raw." : " did NOT pass raw.") << endreq;
          break;
        }
        if ( _passedL1Raw == 0 ) continue;

        _passEBTrigBeforePS[_EBEnum] = true;
        break;
      }
      
      // Based on this, assign the appropriate weight.
      if ( _passEBTrigBeforePS[_EBEnum] == true ) {
        _weight *= 1. - ( 1. / m_chainPrescale[_EBEnum] );
        if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG <<_EBMappingName << " with PS " << m_chainPrescale[_EBEnum] << " included in weight. (weight = " << _weight <<" )" << endreq;
      }
    }
    
    // Don't forget about random, this gives a constant weight
    if (_eventBG == kBG_FILLED &&  m_chainEnabled[kEb_random] == true) {
      _weight *= 1. - ( 1. / m_chainPrescale[kEb_random] );
      if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << EBTriggerNameHLT[kEb_random] << " with PS " << m_chainPrescale[kEb_random] << " included in weight. (weight = " << _weight <<" )" << endreq;
    } else if (_eventBG == kBG_FIRSTEMPTY &&  m_chainEnabled[kEb_random_firstempty] == true) {
      _weight *= 1. - ( 1. / m_chainPrescale[kEb_random_firstempty] );
      if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << EBTriggerNameHLT[kEb_random_firstempty] << " with PS " << m_chainPrescale[kEb_random_firstempty] << " included in weight. (weight = " << _weight <<" )" << endreq;
    } else if (_eventBG == kBG_EMPTY &&  m_chainEnabled[kEb_random_empty] == true) {
      _weight *= 1. - ( 1. / m_chainPrescale[kEb_random_empty] );
      if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << EBTriggerNameHLT[kEb_random_empty] << " with PS " << m_chainPrescale[kEb_random_empty] << " included in weight. (weight = " << _weight <<" )" << endreq;
    } else if (_eventBG == kBG_UNPAIRED_ISO &&  m_chainEnabled[kEb_random_unpaired_iso] == true) {
      _weight *= 1. - ( 1. / m_chainPrescale[kEb_random_unpaired_iso] );
      if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << EBTriggerNameHLT[kEb_random_unpaired_iso] << " with PS " << m_chainPrescale[kEb_random_unpaired_iso] << " included in weight. (weight = " << _weight <<" )" << endreq;
    } else {
      if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << "Not including EB random, chain is disabled." << endreq;
    }

    // Invert to give the un-weighting factor, careful not to /0.
    if ( TMath::Abs(1. - _weight) < 1e-10) {
      _weight = 0.;
    } else {
      _weight = 1. / ( 1. - _weight );
    }

    if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << "This event has final weight:" 
      << _weight << ", (float: " << (Float_t) _weight 
      << ") and BG " << EBBunchGroupNames[_eventBG] <<" this will be stored in locations 45 and 46" << endreq;
    event.addVar(45, (Float_t) _weight);
    event.addVar(46, (Float_t) _eventBG);

    return true;
}
