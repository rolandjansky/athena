/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Framework
#include "AthenaKernel/errorcheck.h"
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
  :AthAlgTool(name, type, parent),
   m_filterInfo(),
   m_chainPrescale(),
   m_chainL1Seeds(),
   m_L1NameToCPTID(),
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

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtEBWeightTool::finalize()
{
  //
  // Clean up
  //
  ATH_MSG_DEBUG("finalize()" );

  return StatusCode::SUCCESS;
}

namespace Trig { // Helper function
  float GetPrescale(const std::vector<TrigConfChain>& _chains, std::string _toFind, MsgStream* _log) {
    std::vector<TrigConfChain>::const_iterator _it;
    for (_it = _chains.begin(); _it != _chains.end(); ++_it) {
      TrigConfChain _chain = (*_it);
      if (_chain.getChainName() != _toFind) continue;
      (*_log) << MSG::DEBUG << "--- Tabulating prescale " << _toFind << " PS:" << _chain.getPrescale() << endmsg;
      return _chain.getPrescale();
    }
    (*_log) << MSG::ERROR << "!!! Cannot find PS for " << _toFind << endmsg;
    return 1.;
  }
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtEBWeightTool::Fill(TrigMonConfig *config)
{
  m_isConfigured = false;

  ATH_MSG_DEBUG("In Trig::TrigNtEBWeightTool::Fill(TrigMonConfig)" );
  m_chainL1Seeds.clear();
  m_chainPrescale.clear();

  if (m_filterInfo.size() == 0) { // Load filter maps
    readEBConfigFromXML(config->getRun());
  }

  // For each EB type
  for (int _EB = 0; _EB < kEbTrggerType_SIZE; ++_EB) {
    EBTriggerType _EBEnum = static_cast<EBTriggerType>(_EB);

    ATH_MSG_DEBUG("- Getting prescale for " << EBTriggerNameHLT[_EB] );
    m_chainEnabled[_EBEnum] = true; // Start off assuming enabled

    // Get the L1 chain
    // This depends on the _origional_ menu. So for all data12, it will be L2_ even if reprocessed.
    // Loop over all chains

    std::string _EBChain = EBTriggerNameHLT[_EB];
    std::string _L1Chain;

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
      ATH_MSG_ERROR("-- Cannot find L1 item for " << _EBChain << " in the current menu" );
    } else {
      ATH_MSG_DEBUG("-- Associated EBChain " << _EBChain << " with " << _L1Chain );
    }
    m_chainL1Seeds[_EBEnum] = _L1Chain;


    // Tabulate the total prescale
    float _totalPrescale = 1, _PS;
    _PS = Trig::GetPrescale(_configChains, EBTriggerNameHLT[_EB], &msg()); // Get HLT level
    if (_PS < 0) m_chainEnabled[_EBEnum] = false;
    _totalPrescale *= _PS;

    // If not "noPS" (L1) then also include the L1 prescale
    if ( _EBChain.find("noPS") == std::string::npos ) {
      _PS = Trig::GetPrescale(_configChains, m_chainL1Seeds[_EBEnum], &msg());
      if (_PS < 0) m_chainEnabled[_EBEnum] = false;
      _totalPrescale *= _PS;
    } else {
      ATH_MSG_DEBUG("---- noPS chain, not adding any additional PS for L1." );
    }

    m_chainPrescale[_EBEnum] = _totalPrescale;
    if (m_chainEnabled[_EBEnum] == true) {
      ATH_MSG_INFO("==== Enhanced Bias Chain " << EBTriggerNameHLT[_EB] << " has total prescale " << m_chainPrescale[_EBEnum] );
    } else {
      ATH_MSG_INFO("==== Enhanced Bias Chain " << EBTriggerNameHLT[_EB] << " is DISABLED (PS < 0 in chain)" );
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

    ATH_MSG_DEBUG("Recording L1Chain Name -> CPTID [" << _chain.getChainName() << " -> " << _chain.getCounter() << "]" );
  }

  m_isConfigured = true;

  return true;
}

//---------------------------------------------------------------------------------------
void Trig::TrigNtEBWeightTool::readEBConfigFromXML(unsigned runNumber) {
  m_filterInfo.clear();

  // Load XML info
  std::stringstream ss;
  ss << "enhanced_bias_run_";
  ss << runNumber;
  ss << ".xml";
  std::string _xmlName;
  ss >> _xmlName;
  const std::string _locAthena = PathResolverFindDataFile( _xmlName );

  ATH_MSG_INFO("EB weighting XML: " << _xmlName << " at " << _locAthena );

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
        if(outputLevel() <= MSG::DEBUG) ATH_MSG_DEBUG("Parsing an EB filter: " << _filter->GetText() );
        _currentFilter = _filter->GetText();
      } else if (_filter->GetNodeName() == TString("lv1_filter") && _currentFilter != "") {
        // Save mapping of this EB filter (e.g. EF_eb_physics) to this L1 Item (e.g. L1_RD1_FILLED)
        m_filterInfo.insert(std::make_pair( std::string(_currentFilter), std::string(_filter->GetText()) ));
        if(outputLevel() <= MSG::DEBUG) ATH_MSG_DEBUG("Regular Filter: " <<  _currentFilter.Data() << " -> " << _filter->GetText() );
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
            if(outputLevel() <= MSG::DEBUG) ATH_MSG_DEBUG("EBTRIGS Filter: " << _currentFilter.Data() << " -> " << _eb_hlt_chains->GetText() );
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
      ATH_MSG_WARNING("Cannot do Enhanced Bias weighting, tool failed to configure." );
      return false;
    }

    const std::vector<TrigMonL1Item>& _eventL1Items = event.getL1Items();
    EBBunchGroupType _eventBG = kBG_NONE;

    // We look to see what L1 items *could* have passed (trigger before prescale) and assign a weight based on the total prescale of the chain.
    std::map<EBTriggerType, Bool_t> _passEBTrigBeforePS;

    Double_t _weight = 1.; // Note - this MUST be a double, can be stored as a float after calculations

    Bool_t _isUnbiased = 0;

    // For each EB type
    for (int _EB = 0; _EB < kEbTrggerType_SIZE; ++_EB) {
      EBTriggerType _EBEnum = static_cast<EBTriggerType>(_EB);

      if ( m_chainEnabled[_EBEnum] == false) continue;

      // Get the string we're searching for in the EB unweighting map.
      // For eb_physics and high_eb_physics (the random seeded chains), this is based on the internal configuration of the chain. This info is prefixed with "L1ItemNames" in the map.
      // For eb_physics_noL1PS, it's simply the L1 items which seed this trigger.
      std::string _EBMappingName = EBMappingNameHLT[_EB]; // In Run-2

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
          ATH_MSG_ERROR("Cannot look up in stored menu the CTPID for chain " << _L1Name );
          return false;
        }
        unsigned _L1CPTID = m_L1NameToCPTID[ _L1Name ];

        // If this element of the map was *not* referring to a random seed.
        //if (_L1Name.find("RD0") != std::string::npos || _L1Name.find("RD1") != std::string::npos)  continue;

        // Get if this L1 item *could* have passed the event. unless it's a random streamer then it must be after veto
        //if ( _costData_L1.getIsL1PassedBeforePrescale( _L1Name ) == kFALSE ) continue;
        Int_t _passedL1Raw = 0;
        for (unsigned i = 0; i < _eventL1Items.size(); ++i) {
          if (_eventL1Items.at(i).getCtpId() != _L1CPTID) continue;
          if (_EBEnum == kEb_random_physics || _EBEnum == kEb_random_empty) {
            _passedL1Raw = _eventL1Items.at(i).isPassedAfterVeto();
            _isUnbiased = _passedL1Raw;
            if (_passedL1Raw) ATH_MSG_DEBUG("L1Item " << _L1Name << " considered for " << _EBMappingName << " PASSED after VETO.");
            break;
          } else { // Non RANDOM
            _passedL1Raw = _eventL1Items.at(i).isPassedBeforePrescale();
            ATH_MSG_DEBUG("L1Item " << _L1Name << " considered for " << _EBMappingName << (_passedL1Raw ? " PASSED raw." : " did NOT pass raw.") );
            break;
          }
        }
        if ( _passedL1Raw == 0 ) continue;
 
        if ( _eventBG == kBG_NONE ) { // Set the event's BG
          _eventBG = EBBunchGroupHLT[_EBEnum];
        } else if ( _eventBG != EBBunchGroupHLT[_EBEnum]  ) { // Multiple items - check they agree
          ATH_MSG_WARNING("BG Missmatch from EB chains " << EBBunchGroupNames[ _eventBG ] << " and " << EBBunchGroupNames[ EBBunchGroupHLT[_EBEnum] ]);
        }

        _passEBTrigBeforePS[_EBEnum] = true;
        break;
      }

      // Based on this, assign the appropriate weight.
      if ( _passEBTrigBeforePS[_EBEnum] == true ) {
        _weight *= 1. - ( 1. / m_chainPrescale[_EBEnum] );
        ATH_MSG_DEBUG(_EBMappingName << " with PS " << m_chainPrescale[_EBEnum] << " included in weight. (weight = " << _weight <<" )" );
      }
    }

    // Invert to give the un-weighting factor, careful not to /0.
    if ( TMath::Abs(1. - _weight) < 1e-10) {
      _weight = 0.;
    } else {
      _weight = 1. / ( 1. - _weight );
    }

    ATH_MSG_INFO("This event has final weight:"
      << _weight << ", (float: " << (Float_t) _weight
      << ") and BG " << EBBunchGroupNames[_eventBG]
      << " and isUnbiased flag:" << _isUnbiased << "  this will be stored in locations 45, 46 and 48" );
    event.addVar(kEBWeight, (Float_t) _weight);
    event.addVar(kEBBunchGroup, (Float_t) _eventBG);
    event.addVar(kEBIsUnbiasedFlag, (Float_t) _isUnbiased);

    return true;
}
