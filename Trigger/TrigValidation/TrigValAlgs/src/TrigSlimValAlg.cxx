/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigNavigation/Navigation.h"

#include "TrigValAlgs/TrigSlimValAlg.h"

TrigSlimValAlg::TrigSlimValAlg(const std::string& name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_slimmingTool("HLT::TrigNavigationSlimmingTool/TrigNavigationSlimmingTool"),
  m_eventSeen(0)
{

  declareProperty("TrigDecisionTool", m_trigDecisionTool);
  declareProperty("SlimmingTool", m_slimmingTool);

}

StatusCode TrigSlimValAlg::initialize() {

  // load the trig decision tool
  if( !m_trigDecisionTool.empty() ) {
    ATH_CHECK( m_trigDecisionTool.retrieve() );
  }
  else {
    ATH_MSG_ERROR ("Could not retrive the TrigDecisionTool as it was not specified!" );
    return StatusCode::FAILURE;
  }
  
  // load the slimming tool
  if( !m_slimmingTool.empty() ) {
    ATH_CHECK( m_slimmingTool.retrieve() );
  }
  else {
    ATH_MSG_ERROR ("Could not retrive the TrigNavigationSlimmingTool as it was not specified!" );
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;

}

StatusCode TrigSlimValAlg::clear() {

  return StatusCode::SUCCESS;

}

StatusCode TrigSlimValAlg::execute() {

  if(!m_eventSeen) {
    // cache groups and streams
    m_configuredGroups = m_trigDecisionTool->getListOfGroups();
    m_configuredStreams = m_trigDecisionTool->getListOfStreams();
    m_eventSeen = 1;
  }
  
  // grab the navigation
  Trig::ExpertMethods *navAccess = m_trigDecisionTool->ExperimentalAndExpertMethods();
  navAccess->enable();
  const HLT::NavigationCore *cnav = navAccess->getNavigation();
  HLT::NavigationCore *navigation = const_cast<HLT::NavigationCore*>(cnav);

  if(navigation == 0) {
    ATH_MSG_WARNING ("Could not get navigation from Trigger Decision Tool" );
    ATH_MSG_WARNING ("Navigation will not be slimmed in this event" );
    return StatusCode::SUCCESS;
  }

  // print the TrigSlimValidation header
  ATH_MSG_INFO ("REGTEST  " << "======== START of TrigSlimValidation DUMP ========" );

  // store some needed information from the navigation structure
  std::map<std::string, int> *featureOccurrences = m_slimmingTool->getFeatureOccurrences(navigation);
  TrigSlimValAlg::elementSet *allTriggerElements = this->getAllTriggerElements(navigation);

  // Print the aggregate navigation structure information

  // build the counters
  NumberCounter *numberCounter = new NumberCounter(navigation);
  IntermediateCounter *intermediateCounter = new IntermediateCounter(navigation);
  FeaturelessCounter *featurelessCounter = new FeaturelessCounter(navigation);
  GhostCounter *ghostCounter = new GhostCounter(navigation);
  RoICounter *roICounter = new RoICounter(navigation);
  SeedsRelationCounter *seedsRelationCounter = new SeedsRelationCounter(navigation);
  SeededByRelationCounter *seededByRelationCounter = new SeededByRelationCounter(navigation);
  SameRoIRelationCounter *sameRoIRelationCounter = new SameRoIRelationCounter(navigation);
  FeatureCounter *featureCounter = new FeatureCounter(navigation);

  // build the updaters
  SumUpdate *sumUpdate = new SumUpdate();
  MaxUpdate *maxUpdate = new MaxUpdate();
  MinUpdate *minUpdate = new MinUpdate();

  ATH_MSG_INFO ("REGTEST  " << "Number of trigger elements: " 
                << recursiveCount( navigation, numberCounter, sumUpdate ) );
  ATH_MSG_INFO ("REGTEST  " << "Number of distinct features: "
                << featureOccurrences->size() );
  ATH_MSG_INFO ("REGTEST  " << "Number of intermediate trigger elements: " 
                << recursiveCount( navigation, intermediateCounter, sumUpdate ) );
  ATH_MSG_INFO ("REGTEST  " << "Number of featureless trigger elements: " 
                << recursiveCount( navigation, featurelessCounter, sumUpdate ) );
  ATH_MSG_INFO ("REGTEST  " << "Number of ghost trigger elements: " 
                << recursiveCount( navigation, ghostCounter, sumUpdate ) );
  ATH_MSG_INFO ("REGTEST  " << "Number of RoIs: " 
                << recursiveCount( navigation, roICounter, sumUpdate ) );
  ATH_MSG_INFO ("REGTEST  " << "Longest chain: " 
                << recursiveCount( navigation, numberCounter, maxUpdate ) );
  ATH_MSG_INFO ("REGTEST  " << "Shortest chain: " 
                << recursiveCount( navigation, numberCounter, minUpdate ) );
  ATH_MSG_INFO ("REGTEST  " << "Number of feature links: " 
                << recursiveCount( navigation, featureCounter, sumUpdate ) );
  ATH_MSG_INFO ("REGTEST  " << "Number of seeds relations: " 
                << recursiveCount( navigation, seedsRelationCounter, sumUpdate ) );
  ATH_MSG_INFO ("REGTEST  " << "Number of seeded by relations: " 
                << recursiveCount( navigation, seededByRelationCounter, sumUpdate ) );
  ATH_MSG_INFO ("REGTEST  " << "Number of same RoI relations: " 
                << recursiveCount( navigation, sameRoIRelationCounter, sumUpdate ) );

  // delete counters
  delete numberCounter; numberCounter = 0;
  delete intermediateCounter; intermediateCounter = 0;
  delete featurelessCounter; featurelessCounter = 0;
  delete ghostCounter; ghostCounter = 0;
  delete roICounter; roICounter = 0;
  delete seedsRelationCounter; seedsRelationCounter = 0;
  delete seededByRelationCounter; seededByRelationCounter = 0;
  delete sameRoIRelationCounter; sameRoIRelationCounter = 0;

  // delete the updaters
  delete sumUpdate; sumUpdate = 0;
  delete maxUpdate; maxUpdate = 0;
  delete minUpdate; minUpdate = 0;


  // print the feature information
  for(std::map<std::string, int>::const_iterator iter = featureOccurrences->begin();
      iter != featureOccurrences->end(); ++iter)
    ATH_MSG_INFO ("REGTEST  " << "Occurrences of feature " << (*iter).first
                  << ": " << (*iter).second );

  // print the stream information
 
  // store elements unassociated with any stream.  Note that by changing
  // the TriggerElementLessThan class in the headerfile, we can change
  // exactly what we mean by "unassociated"
  TrigSlimValAlg::elementSet associatedElements;
  for(std::vector<std::string>::const_iterator streamIter = m_configuredStreams.begin();
      streamIter != m_configuredStreams.end(); ++streamIter) {
    // collect all the elements associated with the stream
    TrigSlimValAlg::elementSet *streamElements = 
      this->getTEsFromChainGroup(m_trigDecisionTool->getChainGroup(std::string("STREAM_") + *streamIter));
    associatedElements.insert(streamElements->begin(), streamElements->end());

    ATH_MSG_INFO ("REGTEST  " << "Elements associated with stream " << *streamIter 
                  << ": " << streamElements->size() );

    delete streamElements;

  }

  ATH_MSG_INFO ("REGTEST  " << "Elements unassociated with any stream: "
                << allTriggerElements->size() - associatedElements.size() );

  // print the group information
  // store elements unassociated with any group.  Note that by changing
  // the TriggerElementLessThan class in the headerfile, we can change
  // exactly what we mean by "unassociated"
  associatedElements.clear();
  for(std::vector<std::string>::const_iterator groupIter = m_configuredGroups.begin();
      groupIter != m_configuredGroups.end(); ++groupIter) {
    // collect all the elements associated with the group
    TrigSlimValAlg::elementSet *groupElements = 
      this->getTEsFromChainGroup(m_trigDecisionTool->getChainGroup(std::string("GROUP_") + *groupIter));
    associatedElements.insert(groupElements->begin(), groupElements->end());

    ATH_MSG_INFO ("REGTEST  " << "Elements associated with group " << (*groupIter)
                  << ": " << groupElements->size() );

    delete groupElements;

  }

  ATH_MSG_INFO ("REGTEST  " << "Elements unassociated with any group: "
                << allTriggerElements->size() - associatedElements.size() );
  
  // clean up and finish
  delete allTriggerElements; allTriggerElements = 0;
  delete featureOccurrences; featureOccurrences = 0;
  
  // print the TrigSlimValidation footer
  ATH_MSG_INFO ("REGTEST  " << "======== END of TrigSlimValidation DUMP ========" );

  return StatusCode::SUCCESS;

}

StatusCode TrigSlimValAlg::finalize() {

  return StatusCode::SUCCESS;

}

int TrigSlimValAlg::recursiveCount(HLT::NavigationCore *navigation,
                                   TrigSlimValAlg::ElementCounter *ec, 
                                   TrigSlimValAlg::CountUpdate    *cu,
                                   HLT::TriggerElement            *te) {

  if(!ec)
    return 0;

  if(!cu)
    return 0;

  if(!navigation)
    return 0;

  if(!te)
    te = navigation->getInitialNode();

  if(!te)
    return 0;

  int count = -1;
  for(std::vector<HLT::TriggerElement*>::const_iterator iter = 
      te->getRelated(HLT::TriggerElement::seedsRelation).begin();
      iter != te->getRelated(HLT::TriggerElement::seedsRelation).end(); ++iter) {
    // note we have to be clever to avoid double counting as a node can have more
    // than one seeded by relation.  To avoid this, ignore the daughter node unless
    // you are the first mother node
    std::vector<HLT::TriggerElement*> mothers = (*iter)->getRelated(HLT::TriggerElement::seededByRelation);
    if(mothers.size() > 0 && mothers[0] == te)
      count = cu->update(count, this->recursiveCount( navigation, ec, cu, *iter ));
  }

  if(count < 0)
    count = 0;

  return count + ec->count(te);

}

TrigSlimValAlg::elementSet *TrigSlimValAlg::getAllTriggerElements(HLT::NavigationCore *navigation, HLT::TriggerElement *te) {

  if(!navigation)
    return 0;

  if(!te)
    te = navigation->getInitialNode();

  if(!te)
    return 0;


  TrigSlimValAlg::elementSet *tes = new TrigSlimValAlg::elementSet;

  tes->insert(te);

  std::vector<HLT::TriggerElement*> children = te->getRelated(HLT::TriggerElement::seedsRelation);
  std::vector<HLT::TriggerElement*>::iterator iter;
  for(iter = children.begin(); iter != children.end(); ++iter) {
    TrigSlimValAlg::elementSet* tesChildren = this->getAllTriggerElements( navigation, *iter );
    if(!tesChildren)
      continue;
    // merge the sets
    TrigSlimValAlg::elementSet temp;
    std::set_union(tes->begin(), tes->end(), 
                   tesChildren->begin(), tesChildren->end(),
                   std::inserter(temp, temp.begin()));
    tes->swap(temp);
                    
    delete tesChildren; tesChildren = 0;
  }

  return tes;

}

TrigSlimValAlg::elementSet *TrigSlimValAlg::getTEsFromChainGroup(const Trig::ChainGroup *cg) {
  
  TrigSlimValAlg::elementSet *tes = new TrigSlimValAlg::elementSet;

  std::vector<Trig::Combination> combinations = m_trigDecisionTool->features(cg).getCombinations();

  for(std::vector<Trig::Combination>::const_iterator iter = combinations.begin();
      iter != combinations.end(); ++iter) {
    std::vector<const HLT::TriggerElement*> combTEs = iter->tes();
    tes->insert(combTEs.begin(), combTEs.end());
  }

  return tes;

}

