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
  float GetPrescale(const std::vector<TrigConfChain>& chains, std::string toFind, MsgStream* log) {
    std::vector<TrigConfChain>::const_iterator it;
    for (it = chains.begin(); it != chains.end(); ++it) {
      TrigConfChain chain = (*it);
      if (chain.getChainName() != toFind) continue;
      (*log) << MSG::DEBUG << "--- Tabulating prescale " << toFind << " PS:" << chain.getPrescale() << endmsg;
      return chain.getPrescale();
    }
    (*log) << MSG::ERROR << "!!! Cannot find PS for " << toFind << endmsg;
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
  for (int EB = 0; EB < kEbTrggerType_SIZE; ++EB) {
    EBTriggerType EBEnum = static_cast<EBTriggerType>(EB);

    ATH_MSG_DEBUG("- Getting prescale for " << EBTriggerNameHLT[EB] );
    m_chainEnabled[EBEnum] = true; // Start off assuming enabled

    // Get the L1 chain
    // This depends on the _original_ menu. So for all data12, it will be L2_ even if reprocessed.
    // Loop over all chains

    std::string EBChain = EBTriggerNameHLT[EB];
    std::string L1Chain;

    const std::vector<TrigConfChain> configChains = config->getVec<TrigConfChain>();
    std::vector<TrigConfChain>::const_iterator it;
    // Loop over all chains and find this one
    for (it = configChains.begin(); it != configChains.end(); ++it) {
      TrigConfChain chain = (*it);
      if (chain.getChainName() != EBChain) continue;
      L1Chain = chain.getLowerName();
      break;
    }

    if (L1Chain == "") {
      ATH_MSG_ERROR("-- Cannot find L1 item for " << EBChain << " in the current menu" );
    } else {
      ATH_MSG_DEBUG("-- Associated EBChain " << EBChain << " with " << L1Chain );
    }
    m_chainL1Seeds[EBEnum] = L1Chain;


    // Tabulate the total prescale
    float totalPrescale = 1, PS;
    PS = Trig::GetPrescale(configChains, EBTriggerNameHLT[EB], &msg()); // Get HLT level
    if (PS < 0) m_chainEnabled[EBEnum] = false;
    totalPrescale *= PS;

    // If not "noPS" (L1) then also include the L1 prescale
    if ( EBChain.find("noPS") == std::string::npos ) {
      PS = Trig::GetPrescale(configChains, m_chainL1Seeds[EBEnum], &msg());
      if (PS < 0) m_chainEnabled[EBEnum] = false;
      totalPrescale *= PS;
    } else {
      ATH_MSG_DEBUG("---- noPS chain, not adding any additional PS for L1." );
    }

    m_chainPrescale[EBEnum] = totalPrescale;
    if (m_chainEnabled[EBEnum] == true) {
      ATH_MSG_INFO("==== Enhanced Bias Chain " << EBTriggerNameHLT[EB] << " has total prescale " << m_chainPrescale[EBEnum] );
    } else {
      ATH_MSG_INFO("==== Enhanced Bias Chain " << EBTriggerNameHLT[EB] << " is DISABLED (PS < 0 in chain)" );
    }
  }

  // Get the CTPID mapping for L1 items
  m_L1NameToCPTID.clear();
  std::vector<TrigConfChain>::const_iterator it;
  for (it = config->getVec<TrigConfChain>().begin(); it != config->getVec<TrigConfChain>().end(); ++it) {
    TrigConfChain chain = (*it);
    if (chain.getLevelId() != 1) continue;
    m_L1NameToCPTID[chain.getChainName()] = chain.getCounter();
    m_CPTIDToL1Name[chain.getCounter()] = chain.getChainName();

    ATH_MSG_DEBUG("Recording L1Chain Name -> CPTID [" << chain.getChainName() << " -> " << chain.getCounter() << "]" );
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
  std::string xmlName;
  ss >> xmlName;
  const std::string locAthena = PathResolverFindDataFile( xmlName );

  ATH_MSG_INFO("EB weighting XML: " << xmlName << " at " << locAthena );

  TDOMParser* xmlParser = new TDOMParser();
  xmlParser->SetValidate(kFALSE);
  xmlParser->ParseFile( locAthena.c_str() );

  TXMLDocument* xmlDoc = xmlParser->GetXMLDocument();
  TXMLNode* rootNode = xmlDoc->GetRootNode();
  TXMLNode* children = rootNode->GetChildren();
  TXMLNode* filtersNode = 0;
  // Navigate down XML, find "filters" node
  while (children != 0) {
    if (children->GetNodeName() == TString("filters")) {
      // We should only find this node once, if we are finding it twice that's bad
      assert( filtersNode == 0 );
      filtersNode = children;
    }
    children = children->GetNextNode();
  }

  // We should now have identified the filters node in the XML. Time to read from it.
  // TODO make this nicer if being kept
  assert( filtersNode != 0 );
  assert( filtersNode->HasChildren() );
  TXMLNode* filtersChildren = filtersNode->GetChildren();
  // Loop over all "filter" under "filters"
  while (filtersChildren != 0) {
    TXMLNode* filter = 0;
    if (filtersChildren->HasChildren()) {
      filter = filtersChildren->GetChildren();
    }
    TString currentFilter = "";

    // Loop over all children of a single "filter"
    while (filter != 0) {
      if (filter->GetNodeName() == TString("filter_name")) {
        ATH_MSG_DEBUG("Parsing an EB filter: " << filter->GetText() );
        currentFilter = filter->GetText();
      } else if (filter->GetNodeName() == TString("lv1_filter") && currentFilter != "") {
        // Save mapping of this EB filter (e.g. EF_eb_physics) to this L1 Item (e.g. L1_RD1_FILLED)
        m_filterInfo.insert(std::make_pair( std::string(currentFilter), std::string(filter->GetText()) ));
        ATH_MSG_DEBUG("Regular Filter: " <<  currentFilter.Data() << " -> " << filter->GetText() );
      } else if (filter->GetNodeName() == TString("eb_hlt_chains") ) {
        // Look into this node too
        TXMLNode* eb_hlt_chains = 0;
        if (filter->HasChildren()) {
          eb_hlt_chains = filter->GetChildren();
        }
        while (eb_hlt_chains != 0) {
          if (eb_hlt_chains->GetNodeName() == TString("eb_hlt_name")) {
            currentFilter = TString("L1ItemNames ") + eb_hlt_chains->GetText();
          } else if (eb_hlt_chains->GetNodeName() == TString("lv1_ebtrigs") && currentFilter != "") {
            m_filterInfo.insert(std::make_pair( std::string(currentFilter), std::string(eb_hlt_chains->GetText()) ));
            ATH_MSG_DEBUG("EBTRIGS Filter: " << currentFilter.Data() << " -> " << eb_hlt_chains->GetText() );
          }
          eb_hlt_chains = eb_hlt_chains->GetNextNode();
        }
      }
      filter = filter->GetNextNode();
    }
    filtersChildren = filtersChildren->GetNextNode();
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

    const std::vector<TrigMonL1Item>& eventL1Items = event.getL1Items();
    EBBunchGroupType eventBG = kBG_NONE;

    // We look to see what L1 items *could* have passed (trigger before prescale) and assign a weight based on the total prescale of the chain.
    std::map<EBTriggerType, Bool_t> passEBTrigBeforePS;

    Double_t weight = 1.; // Note - this MUST be a double, can be stored as a float after calculations

    Bool_t isUnbiased = 0;

    // For each EB type
    for (int EB = 0; EB < kEbTrggerType_SIZE; ++EB) {
      EBTriggerType EBEnum = static_cast<EBTriggerType>(EB);

      if ( m_chainEnabled[EBEnum] == false) continue;

      // Get the string we're searching for in the EB unweighting map.
      // For eb_physics and high_eb_physics (the random seeded chains), this is based on the internal configuration of the chain. This info is prefixed with "L1ItemNames" in the map.
      // For eb_physics_noL1PS, it's simply the L1 items which seed this trigger.
      std::string EBMappingName = EBMappingNameHLT[EB]; // In Run-2

      // Start off assuming that L1 would not pass this event
      passEBTrigBeforePS[EBEnum] = false;

      // Loop over all elements of the map
      for ( std::map<std::string, std::string>::iterator it = m_filterInfo.begin(); it != m_filterInfo.end(); ++it) {
        std::string HLTName = it->first;
        std::string L1Name = it->second;

        // If this element of the map corresponds to the EB trigger we're currently investigating
        if (HLTName != EBMappingName ) continue;

        // Get CTPID for this L1 item
        if (m_L1NameToCPTID.count( L1Name ) == 0) {
          ATH_MSG_ERROR("Cannot look up in stored menu the CTPID for chain " << L1Name );
          return false;
        }
        unsigned L1CPTID = m_L1NameToCPTID[ L1Name ];

        // If this element of the map was *not* referring to a random seed.
        //if (L1Name.find("RD0") != std::string::npos || L1Name.find("RD1") != std::string::npos)  continue;

        // Get if this L1 item *could* have passed the event. unless it's a random streamer then it must be after veto
        //if ( _costData_L1.getIsL1PassedBeforePrescale( _L1Name ) == kFALSE ) continue;
        Int_t passedL1Raw = 0;
        for (unsigned i = 0; i < eventL1Items.size(); ++i) {
          if (eventL1Items.at(i).getCtpId() != L1CPTID) continue;
          if (EBEnum == kEb_random_physics || EBEnum == kEb_random_empty) {
            passedL1Raw = eventL1Items.at(i).isPassedAfterVeto();
            isUnbiased = passedL1Raw;
            if (passedL1Raw) ATH_MSG_DEBUG("L1Item " << L1Name << " considered for " << EBMappingName << " PASSED after VETO.");
            break;
          } else { // Non RANDOM
            passedL1Raw = eventL1Items.at(i).isPassedBeforePrescale();
            ATH_MSG_DEBUG("L1Item " << L1Name << " considered for " << EBMappingName << (passedL1Raw ? " PASSED raw." : " did NOT pass raw.") );
            break;
          }
        }
        if ( passedL1Raw == 0 ) continue;
 
        if ( eventBG == kBG_NONE ) { // Set the event's BG
          eventBG = EBBunchGroupHLT[EBEnum];
        } else if ( eventBG != EBBunchGroupHLT[EBEnum]  ) { // Multiple items - check they agree
          ATH_MSG_WARNING("BG Missmatch from EB chains " << EBBunchGroupNames[ eventBG ] << " and " << EBBunchGroupNames[ EBBunchGroupHLT[EBEnum] ]);
        }

        passEBTrigBeforePS[EBEnum] = true;
        break;
      }

      // Based on this, assign the appropriate weight.
      if ( passEBTrigBeforePS[EBEnum] == true ) {
        weight *= 1. - ( 1. / m_chainPrescale[EBEnum] );
        ATH_MSG_DEBUG(EBMappingName << " with PS " << m_chainPrescale[EBEnum] << " included in weight. (weight = " << weight <<" )" );
      }
    }

    // Invert to give the un-weighting factor, careful not to /0.
    if ( TMath::Abs(1. - weight) < 1e-10) {
      weight = 0.;
    } else {
      weight = 1. / ( 1. - weight );
    }

    ATH_MSG_INFO("This event has final weight:"
      << weight << ", (float: " << (Float_t) weight
      << ") and BG " << EBBunchGroupNames[eventBG]
      << " and isUnbiased flag:" << isUnbiased << "  this will be stored in locations 45, 46 and 48" );
    event.addVar(kEBWeight, (Float_t) weight);
    event.addVar(kEBBunchGroup, (Float_t) eventBG);
    event.addVar(kEBIsUnbiasedFlag, (Float_t) isUnbiased);

    return true;
}
