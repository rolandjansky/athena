/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// EnhancedBiasAugmentationTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Tim Martin (Tim.Martin@cern.ch)
//
// Load the EnhancedBias weight needed to weight an enhanced bias event to zero bias (for rate estimations)
// This comes from XML and is decorated onto the EventInfo
// 
// Load the number of events taken per LB for this EB run (from XML) and also LB length (from COOL)
// This gives the effective live-time per event at P1, the denominator of rates calculations

#include "DerivationFrameworkTrigger/EnhancedBiasAugmentationTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "PathResolver/PathResolver.h"
#include <vector>
#include <string>

#include <TXMLEngine.h>
#include <TObjString.h>
#include <TDOMParser.h>
#include <TXMLNode.h>
#include <TXMLDocument.h>
#include <TXMLAttr.h>

namespace DerivationFramework {

  EnhancedBiasAugmentationTool::EnhancedBiasAugmentationTool(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_bcTool( "Trig::LHCBunchCrossingTool/BunchCrossingTool" )
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty( "BCTool", m_bcTool );
    declareProperty( "RunNumber", m_runNumber = 0 );
  }

  StatusCode EnhancedBiasAugmentationTool::initialize()
  {
    StatusCode sc = StatusCode::SUCCESS;

    ATH_MSG_INFO( "Initializing..." );
    ATH_CHECK( m_bcTool.retrieve() );
    ATH_MSG_DEBUG( "Retrieved the bunch crossing tool" );

    loadLumi();
    loadWeights();
 
    return sc;
  }

  StatusCode EnhancedBiasAugmentationTool::addBranches() const
  {

    // Set up the decorator
    SG::AuxElement::Decorator< float > decoratorEBWeight("EnhancedBiasWeight"); 
    SG::AuxElement::Decorator< float > decoratorEBLivetime("EnhancedBiasLivetime"); 

    SG::AuxElement::Decorator< char >  decoratorUnbiasedFlag("IsUnbiasedEventFlag");
    SG::AuxElement::Decorator< char >  decoratorGoodLBFlag("IsGoodLBFlag");  

    SG::AuxElement::Decorator< int >  decoratorBCIDDistance("BCIDDistanceFromFront");  

    // Get the event info
    const xAOD::EventInfo* eventInfo = evtStore()->retrieve< const xAOD::EventInfo >();
    
    if(!eventInfo) {
      ATH_MSG_ERROR ("Couldn't retrieve xAOD::EventInfo");
      return StatusCode::FAILURE;
    }

    if (m_eventNumberToIdMap.count(eventInfo->eventNumber()) == 1) {
      int32_t weightID = m_eventNumberToIdMap.at( eventInfo->eventNumber());
      decoratorEBWeight(*eventInfo)     = m_idToWeightMap.at( weightID );
      decoratorUnbiasedFlag(*eventInfo) = m_idToUnbiasedMap.at( weightID); 
    } else {
      ATH_MSG_WARNING ("Couldn't find enhanced bias weight for event " << eventInfo->eventNumber());
      decoratorEBWeight(*eventInfo)     = -1;
      decoratorUnbiasedFlag(*eventInfo) = -1; 
    }

    if (m_eventsPerLB.count(eventInfo->lumiBlock()) == 1) {
      int32_t eventsInThisLB = m_eventsPerLB.at( eventInfo->lumiBlock() );
      float lbLength = m_readLumiBlock.getLumiBlockLength( eventInfo->lumiBlock() );
      // This event is one in eventsInThisLB, so has an effective temporal contribution of:
      float eventLivetime = -1;
      if (eventsInThisLB > 0 && fabs(lbLength) > 1e-10) eventLivetime = (1. / eventsInThisLB) * lbLength;
      decoratorEBLivetime(*eventInfo) = eventLivetime;
      decoratorGoodLBFlag(*eventInfo) = m_goodLB.at(eventInfo->lumiBlock()); 
    } else {
      ATH_MSG_WARNING ("Couldn't find LB info for LB:" << eventInfo->lumiBlock());
      decoratorEBLivetime(*eventInfo) = -1;
      decoratorGoodLBFlag(*eventInfo) = -1; 
    }

    decoratorBCIDDistance(*eventInfo) = m_bcTool->distanceFromFront( eventInfo->bcid(), Trig::IBunchCrossingTool::BunchCrossings );

    return StatusCode::SUCCESS;;
  }

  void EnhancedBiasAugmentationTool::loadWeights() 
  {

    // Construct name
    std::stringstream fileNameDev, fileName;
    fileName    << "TrigCostRootAnalysis/EnhancedBiasWeights_" << m_runNumber << ".xml";
    fileNameDev << "dev/TrigCostRootAnalysis/EnhancedBiasWeights_" << m_runNumber << ".xml";

    std::string weightingFile = PathResolverFindCalibFile( fileName.str() );  // Check standard area
    if (weightingFile == "") {
      weightingFile = PathResolverFindCalibFile( fileNameDev.str() ); // Else check DEV area
    }

    if (weightingFile == "") {
      ATH_MSG_WARNING ("Could not retrieve " << fileName.str() << " or " << fileNameDev.str() << ", will not append enhanced bias weights");
      return;
    }

    TXMLEngine* xml = new TXMLEngine();
    XMLDocPointer_t xmlDoc = xml->ParseFile( weightingFile.c_str() );

    if (xmlDoc == nullptr) {
      ATH_MSG_WARNING ("Could not parse " << fileName.str() << ", will not append enhanced bias weights");
      delete xml;
      return;
    }

    // Navigate XML
    XMLNodePointer_t mainNode = xml->DocGetRootElement(xmlDoc);
    assert( xml->GetNodeName(mainNode) == std::string("run") );
    XMLNodePointer_t weightsNode = xml->GetChild( mainNode );
    XMLNodePointer_t eventsNode  = xml->GetNext( weightsNode );

    XMLNodePointer_t weightNode = xml->GetChild( weightsNode );
    XMLNodePointer_t eventNode  = xml->GetChild( eventsNode );

    while ( weightNode != 0 ) { // Loop over all weight elements
      assert( xml->GetNodeName(weightNode) == std::string("weight") );

      int32_t id   = stringToNum(xml->GetAttr(weightNode, "id"), (int32_t)0);
      float weight = stringToNum(xml->GetAttr(weightNode, "value"), (float)0.);
      char unbiased = 0;
      if ( xml->HasAttr(weightNode, "unbiased") == true ) {
        unbiased = stringToNum(xml->GetAttr(weightNode, "unbiased"), (char)0);
      }

      m_idToWeightMap[id] = weight;
      m_idToUnbiasedMap[id] = unbiased;

      weightNode = xml->GetNext(weightNode);
    }

    while ( eventNode != 0 ) { // Loop over all event elements
      assert( xml->GetNodeName(eventNode) == std::string("e") ); //Event
      uint64_t eventNumber  = stringToNum(xml->GetAttr(eventNode, "n"), (uint64_t)0); //Number
      int32_t eventWeightID = stringToNum(xml->GetAttr(eventNode, "w"), (int32_t)0); //Weight ID

      m_eventNumberToIdMap[eventNumber] = eventWeightID;

      eventNode = xml->GetNext(eventNode);
    }

    ATH_MSG_INFO ("Loaded " << m_eventNumberToIdMap.size() << " event weights for run " << m_runNumber);
    delete xml;
    return;
  }

  void EnhancedBiasAugmentationTool::loadLumi()
  {
    // Fetch LB time from COOL for this run
    m_readLumiBlock.updateLumiBlocks( m_runNumber );

    // Read in number of events to expect 
    // Construct name
    std::stringstream fileNameDev, fileName;
    fileName    << "TrigCostRootAnalysis/enhanced_bias_run_" << m_runNumber << ".xml";
    fileNameDev << "dev/TrigCostRootAnalysis/enhanced_bias_run_" << m_runNumber << ".xml";

    std::string runFile = PathResolverFindCalibFile( fileName.str() );  // Check standard area
    if (runFile == "") {
      runFile = PathResolverFindCalibFile( fileNameDev.str() ); // Else check DEV area
    }

    if (runFile == "") {
      ATH_MSG_WARNING ("Could not retrieve " << fileName.str() << ", will not append livetimes");
      return;
    }

    TXMLEngine* xml = new TXMLEngine();
    XMLDocPointer_t xmlDoc = xml->ParseFile( runFile.c_str() );

    if (xmlDoc == nullptr) {
      ATH_MSG_WARNING ("Could not parse " << fileName.str() << ", will not append livetimes");
      delete xml;
      return;
    }

    XMLNodePointer_t mainNode = xml->DocGetRootElement(xmlDoc);
    assert( xml->GetNodeName(mainNode) == std::string("trigger") );
    XMLNodePointer_t listNode = xml->GetChild( mainNode );

    while ( listNode != 0 ) { // Loop over all menu elements
      const std::string listName = xml->GetNodeName(listNode);

      if (listName == "lb_list") {
        XMLNodePointer_t node = xml->GetChild( listNode );
        while( node != 0) {
          assert( xml->GetNodeName(node) == std::string("lb") );
          uint32_t lb      = stringToNum( xml->GetAttr(node, "id"), (uint32_t)0 );
          uint32_t nEvents = stringToNum( xml->GetNodeContent(node), (uint32_t)0 );
          std::string flag;
          if ( xml->HasAttr(node, "flag") == true ) {
            flag = xml->GetAttr(node, "flag");
          }

          m_eventsPerLB[lb] = nEvents;
          m_goodLB[lb] = (flag == "bad" ? 0 : 1);

          node = xml->GetNext(node);
        }
      } else {
        // Currently we don't care about the rest of this file
      }

      listNode = xml->GetNext(listNode);
    }

    ATH_MSG_INFO ("Loaded " << m_eventsPerLB.size() << " EnhancedBias lumi block's info for run " << m_runNumber);
    delete xml;
    return;
  }

  template <class T>
  T EnhancedBiasAugmentationTool::stringToNum(const std::string& i, T typeOfClass) const
  {
    std::istringstream( i ) >> typeOfClass;
    return typeOfClass;
  }

}
