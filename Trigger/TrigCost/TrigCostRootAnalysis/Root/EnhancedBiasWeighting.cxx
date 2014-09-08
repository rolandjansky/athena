// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <iostream>
#include <fstream>
#include <assert.h>
#include "cstdlib" //std::getenv

// ROOT include(s):
#include <TError.h>
// XML parse & unweighting specific
#include <TObjString.h>
#include <TDOMParser.h>
#include <TXMLNode.h>
#include <TXMLDocument.h>
#include <TXMLAttr.h>

// Local include(s):
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/Utility.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/EnhancedBiasWeighting.h"
#include "../TrigCostRootAnalysis/Config.h"


namespace TrigCostRootAnalysis {

  EnhancedBiasWeighting::EnhancedBiasWeighting() : m_runNumber(0), m_isConfigured(0) {
  }
  
  EnhancedBiasWeighting::EnhancedBiasWeighting(UInt_t _runNumber) : m_runNumber(0), m_isConfigured(0) {
    setup(_runNumber);
  }
  
  EnhancedBiasWeighting::~EnhancedBiasWeighting() {
  }
  
  Bool_t EnhancedBiasWeighting::setup(UInt_t _runNumber) {
  
    m_randomSeedWeight = 8.0;
    m_online_SMK = -1;
    m_online_L1PSK = -1;
    m_online_HLTPSK = -1;
    
    m_filterInfo.clear();
    m_runNumber = _runNumber;
    
    //Just XML for now.
    Bool_t _setupFilter = kFALSE;
    _setupFilter = readMapFromXML();
    
    
    m_isConfigured = _setupFilter;
    return _setupFilter;
  }
  
  Bool_t EnhancedBiasWeighting::readMapFromXML() {
  
    std::string _xmlName = std::string( std::getenv("ROOTCOREDIR") )
                           + std::string("/data/TrigCostRootAnalysis/enhanced_bias_run_")
                           + std::string(intToString( m_runNumber ))
                           + std::string(".xml");
                           
    Info("Utility::calculateEBEventWeight", "EB weighting XML: %s", _xmlName.c_str() );
    
    TDOMParser* _xmlParser = new TDOMParser();
    _xmlParser->SetValidate(kFALSE);
    Int_t _parseCode = _xmlParser->ParseFile( _xmlName.c_str() );
    // Parse XML
    UNUSED( _parseCode );
    assert( _parseCode == 0 );
    
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
          Info("EnhancedBiasWeighting::readMapFromXML", "Parsing an EB filter: %s", _filter->GetText() );
          _currentFilter = _filter->GetText();
        } else if (_filter->GetNodeName() == TString("lv1_filter") && _currentFilter != "") {
          // Save mapping of this EB filter (e.g. EF_eb_physics) to this L1 Item (e.g. L1_RD1_FILLED)
          m_filterInfo.insert(std::make_pair( std::string(_currentFilter), std::string(_filter->GetText()) ));
          Info("EnhancedBiasWeighting::readMapFromXML", "   Filter: %s -> %s", _currentFilter.Data(), _filter->GetText() );
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
              Info("EnhancedBiasWeighting::readMapFromXML", "   Filter: %s -> %s", _currentFilter.Data(), _eb_hlt_chains->GetText() );
            }
            _eb_hlt_chains = _eb_hlt_chains->GetNextNode();
          }
        }
        _filter = _filter->GetNextNode();
      }
      _filtersChildren = _filtersChildren->GetNextNode();
    }
    
    return kTRUE;
  }
  
  void EnhancedBiasWeighting::loadPrescales(Int_t _online_SMK, Int_t _online_L1PSK, Int_t _online_HLTPSK) {
  
    // Check if already loaded
    if ( m_online_SMK == _online_SMK
         && m_online_L1PSK == _online_L1PSK
         && m_online_HLTPSK == _online_HLTPSK ) {
      return;
    }
    
    //If not, load from config.
    //TODO CODE HERE TO CACHE CURRENT LIVE KEYS AND LOAD CORRECT EB CONFIG
    
    m_online_SMK = _online_SMK;
    m_online_L1PSK = _online_L1PSK;
    m_online_HLTPSK = _online_HLTPSK;
    
    Info("EnhancedBiasWeighting::loadPrescales", "Loading online configuration and prescales for %i:%i:%i", _online_SMK, _online_L1PSK, _online_HLTPSK);
    
    // For each EB type
    for (int _EB = 0; _EB < kEbTrggerType_SIZE; ++_EB) {
      EBTriggerType _EBEnum = static_cast<EBTriggerType>(_EB);
      
      // Get the L1 chain
      // This depends on the _origional_ menu. So for all data12, it will be L2_ even if reprocessed.
      if (Config::config().getInt(kDoHLTOnline) == kFALSE) {
        m_chainL1Seeds[_EBEnum] = TrigConfInterface::getLowerChainName( EBTriggerNameL2[_EB] );
      } else {
        m_chainL1Seeds[_EBEnum] = TrigConfInterface::getLowerChainName( EBTriggerNameHLT[_EB] );
      }
      
      // Tabulate the total prescale
      Float_t _totalPrescale = 1;
      if (Config::config().getInt(kDoHLTOnline) == kFALSE) {
        _totalPrescale *= TrigConfInterface::getPrescale( EBTriggerNameEF[_EB] );
        _totalPrescale *= TrigConfInterface::getPrescale( EBTriggerNameL2[_EB] );
      } else {
        _totalPrescale *= TrigConfInterface::getPrescale( EBTriggerNameHLT[_EB] );
      }
      
      // If not "noL1PS" then also include the L1 prescale
      if ( _EBEnum != kEb_physics_noL1PS ) {
        _totalPrescale *= TrigConfInterface::getPrescale( m_chainL1Seeds[_EBEnum] );
        
        // If seeded from a random trigger, then include a factor of 8 (correct 2014).
        // This is because the random clock runs at 5 MHz rater than 40.
        if ( m_chainL1Seeds[_EBEnum].find("RD0") != std::string::npos
             || m_chainL1Seeds[_EBEnum].find("RD1") != std::string::npos) {
          _totalPrescale *= m_randomSeedWeight;
        }
      }
      
      m_chainPrescale[_EBEnum] = _totalPrescale;
      Info("EnhancedBiasWeighting::loadPrescales", "EB Chain %s has total prescale %f.", EBTriggerNameHLT[_EB].c_str(), m_chainPrescale[_EBEnum]);
    }
  }
  
  Float_t EnhancedBiasWeighting::getWeight(const TrigCostData &_costData_L1, const TrigCostData &_costData_HLT) {
    UNUSED(_costData_HLT); //TODO change this?
  
    // First, get the keys which were run online.
    Int_t _online_SMK = TrigConfInterface::getCurrentSMK();
    Int_t _online_L1PSK = TrigConfInterface::getCurrentL1PSK();
    Int_t _online_HLTPSK = TrigConfInterface::getCurrentHLTPSK();
    //TODO this is currently the REPROCESSING keys! we need the ONLINE info in the D3PD!!
    
    // Now, check if we have the total prescale calculated for this keyset
    loadPrescales( _online_SMK, _online_L1PSK, _online_HLTPSK );
    
    // We look to see what L1 items *could* have passed (trigger before prescale) and assign a weight based on the total prescale of the chain.
    std::map<EBTriggerType, Bool_t> _passEBTrigBeforePS;
    
    Float_t _weight = 1.;
    
    // For each EB type
    for (int _EB = 0; _EB < kEbTrggerType_SIZE; ++_EB) {
      EBTriggerType _EBEnum = static_cast<EBTriggerType>(_EB);
      
      // We do random on its own
      if ( _EBEnum == kEb_random ) continue;
      
      // Get the string we're searching for in the EB unweighting map.
      // For eb_physics and high_eb_physics (the random seeded chains), this is based on the internal configuration of the chain. This info is prefixed with "L1ItemNames" in the map.
      // For eb_physics_noL1PS, it's simply the L1 items which seed this trigger.
      std::string _EBMappingName = EBMappingNameEF[_EB];
      if ( Config::config().getInt(kDoHLTOnline) == kTRUE ) _EBMappingName = EBMappingNameHLT[_EB];
      
      // Start off assuming that L1 would not pass this event
      _passEBTrigBeforePS[_EBEnum] = kFALSE;
      
      // Loop over all elements of the map
      for ( std::map<std::string, std::string>::iterator it = m_filterInfo.begin(); it != m_filterInfo.end(); ++it) {
        std::string _HLTName = it->first;
        std::string _L1Name = it->second;
        
        // If this element of the map corresponds to the EB trigger we're currently investigating
        if (_HLTName != _EBMappingName ) continue;
        
        // If this element of the map was *not* referring to a random seed.
        if (_L1Name.find("RD0") != std::string::npos || _L1Name.find("RD1") != std::string::npos)  continue;
        
        // Get if this L1 item *could* have passed the event.
        if ( _costData_L1.getIsL1PassedBeforePrescale( _L1Name ) == kFALSE ) continue;
        
        _passEBTrigBeforePS[_EBEnum] = kTRUE;
        Info("EnhancedBiasWeighting::getWeight", "- Item %s considered for %s passed before PS at L1", _L1Name.c_str(), _EBMappingName.c_str() );
      }
      
      // Based on this, assign the appropriate weight.
      if ( _passEBTrigBeforePS[_EBEnum] == kTRUE ) {
        _weight *= 1. - ( 1. / m_chainPrescale[_EBEnum] );
        Info("EnhancedBiasWeighting::getWeight", "%s included in weight. (weight = %f )", _EBMappingName.c_str(), _weight);
      }
    }
    
    // Don't forget about random, this gives a constant weight
    _weight *= 1. - ( 1. / m_chainPrescale[kEb_random] );
    Info("EnhancedBiasWeighting::getWeight", "random included in weight. (weight = %f )", _weight);
    
    // Invert to give the un-weighting factor;
    _weight = 1. / ( 1. - _weight );
    
    Info ("EnhancedBiasWeighting::getWeight", "This event has final weight: %f", _weight);
    return _weight;
  }
  
  
} // namespace TrigCostRootAnalysis
