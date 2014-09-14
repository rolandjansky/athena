/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "boost/foreach.hpp"

#include "TrigDecisionTool/ChainGroup.h"

#include "AthenaKernel/IThinningSvc.h"
#include "TrigNavigation/Navigation.h"
#include "TrigNavigation/TriggerElement.h"
//#include "TrigNavigation/TriggerElementFactory.h"
#include "TrigNavigation/Holder.h"


#include "TrigNavTools/TrigNavigationSlimmingTool.h"



/**********************************************************************
 *
 * Constructors and destructors
 *
 **********************************************************************/

HLT::TrigNavigationSlimmingTool::TrigNavigationSlimmingTool( const std::string& type,
							     const std::string& name,
							     const IInterface* parent ) 
  : AthAlgTool(type, name, parent), 
    m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool"),
    m_thinningSvc("", name), 
    m_seenEvent(0) {

  declareInterface<TrigNavigationSlimmingTool>(this);
  m_deletedFeatures = new std::vector<std::pair<CLID, uint16_t> >;
  
    // job option configurable properties
  declareProperty("ResultKey", m_resultKey = "HLTResult_HLT");
  declareProperty("WriteTree", m_writeTree=true, "Replaces the navigation in HLTResult by the slimmed one.");
  declareProperty("ReloadNavigation", m_reloadNavigation=false, "Makes all clients of the navigation aware of the slimming.");
  declareProperty("PrintTree", m_printTree=false, "Does extensive navigation printout.");
  declareProperty("TrigDecisionTool", m_trigDecisionTool, "Tool handle to TDT/Navigation.");
  declareProperty("ThinningSvc", m_thinningSvc, "Synchronize feature indexes wiht this instance of ThinningSvc");
  declareProperty("Squeeze", m_doSqueeze = false, "Remove TEs which have no features attached.");
  declareProperty("ProtectChains", m_protectChains = false);
  declareProperty("RemoveGhosts", m_removeGhosts = false);
  declareProperty("RemoveFeatureless", m_removeFeatureless = false);
  declareProperty("ProtectOtherStreams", m_protectOtherStreams = false);
  declareProperty("RemoveFailedChains", m_removeFailedChains = false);
  declareProperty("RemoveEmptyRoIs", m_removeEmptyRoIs = false);
  declareProperty("GroupInclusionList", m_groupInclusionList);
  declareProperty("GroupExclusionList", m_groupExclusionList);
  declareProperty("StreamInclusionList", m_streamInclusionList);
  declareProperty("StreamExclusionList", m_streamExclusionList, "List of streams to exclude");
  declareProperty("ChainInclusionList", m_chainInclusionList);
  declareProperty("ChainExclusionList", m_chainExclusionList);
  declareProperty("FeatureInclusionList", m_featureInclusionList, "This features will be kept. This setting overrules the FeatureExclusionList. Only list of types or type#key pairs are supported.");
  declareProperty("FeatureExclusionList", m_featureExclusionList, "This features will be dropeed. It can be specified as * meaning all, or as a list of typenames, or typename#key pairs.");
  declareProperty("BranchInclusionList", m_branchInclusionList);
  declareProperty("BranchExclusionList", m_branchExclusionList);
  declareProperty("DropNavigation", m_dropNavigation=false, "Drop the navigation totally, only effective if the WriteTree is True."); 

}


HLT::TrigNavigationSlimmingTool::~TrigNavigationSlimmingTool() {
  if(m_deletedFeatures) {
    delete m_deletedFeatures;
    m_deletedFeatures = 0;
  }
}

/**********************************************************************
 *
 * Initialization and Finalization 
 *
 **********************************************************************/

StatusCode HLT::TrigNavigationSlimmingTool::intialize() {
  m_navigation = 0;

  // load the required tools
  if( not m_trigDecisionTool.empty() ) {
    if ( m_trigDecisionTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL ( "Unable to retrieve the TrigDecisionTool!" );
      ATH_MSG_FATAL ( "Please check your job options file" );
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO ( "Successfully retrived the TrigDecisionTool!" );
  }
  else {
    ATH_MSG_FATAL ( "Could not retrive the TrigDecisionTool as it was not specified!" );
    return StatusCode::FAILURE;
  }

  if( not m_thinningSvc.empty() ) {
    if (  m_thinningSvc.retrieve().isFailure()) {
      ATH_MSG_FATAL ( "ThinningSvc configured but can not retrieve it" );
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO ( "Successfully retrived the ThinningSvc" );
  }

  ATH_MSG_DEBUG ( "Leaving TrigNavigationSlimmingTool::Initialize" );
  // make sure that the inclusions/exclusions are self consistent i.e. they can not be sued at the same time
  if ( m_featureExclusionList.size() &&  m_featureInclusionList.size() ) {
    ATH_MSG_ERROR( "Can't use the features inclusion and exclusion lists at the same time.");
    return StatusCode::FAILURE;
  }
  m_featureKeepSet.insert(m_featureInclusionList.begin(), m_featureInclusionList.end());
  m_featureDropSet.insert(m_featureExclusionList.begin(), m_featureExclusionList.end());


  return StatusCode::SUCCESS;
}


StatusCode HLT::TrigNavigationSlimmingTool::doSlimming() {

  if (m_dropNavigation) {
    const HLT::HLTResult *constHltResult = 0;
    StatusCode sc = evtStore()->retrieve(constHltResult, m_resultKey);
    if(!constHltResult || sc.isFailure()) {
      ATH_MSG_WARNING ( "Unable to load HLTResult with key " << m_resultKey << " from StoreGate..." );
      ATH_MSG_WARNING ( "Navigation will not be slimmed nor anything else will be done to it" );
      return StatusCode::SUCCESS;
    }
    const_cast<HLT::HLTResult*>(constHltResult)->getNavigationResult().clear();
    const_cast<HLT::HLTResult*>(constHltResult)->getNavigationResultCuts().clear();
    ATH_MSG_DEBUG ( "Navigation content erased in: " << m_resultKey );
    return StatusCode::SUCCESS;
  } // else the content is just cleared
  ATH_MSG_DEBUG ( "Will slim the navigation" );
    




  // grab the navigation
  Trig::ExpertMethods *navAccess = m_trigDecisionTool->ExperimentalAndExpertMethods();
  navAccess->enable();
  const HLT::NavigationCore *cnav = navAccess->getNavigation();
  HLT::NavigationCore *navigation = const_cast<HLT::NavigationCore*>(cnav);

  if(navigation == 0) {
    ATH_MSG_WARNING ( "Could not get navigation from Trigger Decision Tool" );
    ATH_MSG_WARNING ( "Navigation will not be slimmed in this event" );
    return StatusCode::SUCCESS;
  }
    
  // if this is the first event we've seen, we need to process the group and stream info
  if(!m_seenEvent) {

    // remember we've seen it
    m_seenEvent = 1;

    // remember the configured chain names, as they will be useful later
    m_configuredChainNames = m_trigDecisionTool->getListOfTriggers();

    // everything that will be done is based upon chain inclusion and exclusion lists,
    // so we need to update them to include this information
    std::vector<const Trig::ChainGroup*> includedChainGroups;
    std::vector<const Trig::ChainGroup*> excludedChainGroups;

    // process the protect other stream information
    if(m_protectOtherStreams) {
      std::vector<std::string> streamNames = m_trigDecisionTool->getListOfStreams();
      for(std::vector<std::string>::const_iterator iter = streamNames.begin();
          iter != streamNames.end(); ++iter) {
        if(std::find(m_streamExclusionList.begin(), m_streamExclusionList.end(), (*iter) ) == m_streamExclusionList.end() &&
            std::find(m_streamInclusionList.begin(), m_streamInclusionList.end(), (*iter) ) == m_streamInclusionList.end())
          m_streamInclusionList.push_back(*iter);
      }
    }

    // process the stream information
    for(std::vector<std::string>::const_iterator iter = m_streamInclusionList.begin();
        iter != m_streamInclusionList.end(); ++iter)
      includedChainGroups.push_back(m_trigDecisionTool->getChainGroup(std::string("STREAM_") + *iter));
    for(std::vector<std::string>::const_iterator iter = m_streamExclusionList.begin();
        iter != m_streamExclusionList.end(); ++iter)
      excludedChainGroups.push_back(m_trigDecisionTool->getChainGroup(std::string("STREAM_") + *iter));

    // process the group information
    for(std::vector<std::string>::const_iterator iter = m_groupInclusionList.begin();
        iter != m_groupInclusionList.end(); ++iter)
      includedChainGroups.push_back(m_trigDecisionTool->getChainGroup(std::string("GROUP_") + *iter));
    for(std::vector<std::string>::const_iterator iter = m_groupExclusionList.begin();
        iter != m_groupExclusionList.end(); ++iter)
      excludedChainGroups.push_back(m_trigDecisionTool->getChainGroup(std::string("GROUP_") + *iter));

    // process the chain information
    for(std::vector<std::string>::const_iterator iter = m_chainInclusionList.begin();
        iter != m_chainInclusionList.end(); ++iter)
      includedChainGroups.push_back(m_trigDecisionTool->getChainGroup(*iter));
    for(std::vector<std::string>::const_iterator iter = m_chainExclusionList.begin();
        iter != m_chainExclusionList.end(); ++iter)
      excludedChainGroups.push_back(m_trigDecisionTool->getChainGroup(*iter));

    // combine the chain groups
    std::vector<std::string> includedTriggers;
    for(std::vector<const Trig::ChainGroup*>::const_iterator iterCG = includedChainGroups.begin();
        iterCG != includedChainGroups.end(); ++iterCG) {
      std::vector<std::string> triggers = (*iterCG)->getListOfTriggers();
      for(std::vector<std::string>::const_iterator iterT = triggers.begin();
          iterT != triggers.end(); ++iterT) {
        includedTriggers.push_back(*iterT);
      }
    }

    std::vector<std::string> excludedTriggers;
    for(std::vector<const Trig::ChainGroup*>::const_iterator iterCG = excludedChainGroups.begin();
        iterCG != excludedChainGroups.end(); ++iterCG) {
      std::vector<std::string> triggers = (*iterCG)->getListOfTriggers();
      for(std::vector<std::string>::const_iterator iterT = triggers.begin();
          iterT != triggers.end(); ++iterT) {
        excludedTriggers.push_back(*iterT);
      }
    }

    // print the input for debugging purposes
    ATH_MSG_DEBUG ( "Input status:" );
    ATH_MSG_DEBUG ( "Stream inclusion list: " );
    for(std::vector<std::string>::iterator iter = m_streamInclusionList.begin(); iter != m_streamInclusionList.end(); ++iter)
      ATH_MSG_DEBUG ( "\tStream: " << *iter );
    ATH_MSG_DEBUG ( "Stream exclusion list: " );
    for(std::vector<std::string>::iterator iter = m_streamExclusionList.begin(); iter != m_streamExclusionList.end(); ++iter)
      ATH_MSG_DEBUG ( "\tStream: " << *iter );
    ATH_MSG_DEBUG ( "Group inclusion list: " );
    for(std::vector<std::string>::iterator iter = m_groupInclusionList.begin(); iter != m_groupInclusionList.end(); ++iter)
      ATH_MSG_DEBUG ( "\tGroup: " << *iter );
    ATH_MSG_DEBUG ( "Group exclusion list: " );
    for(std::vector<std::string>::iterator iter = m_groupExclusionList.begin(); iter != m_groupExclusionList.end(); ++iter)
      ATH_MSG_DEBUG ( "\tGroup: " << *iter );
    ATH_MSG_DEBUG ( "Chain inclusion list: " );
    for(std::vector<std::string>::iterator iter = m_chainInclusionList.begin(); iter != m_chainInclusionList.end(); ++iter)
      ATH_MSG_DEBUG ( "\tChain: " << *iter );
    ATH_MSG_DEBUG ( "Chain exclusion list: " );
    for(std::vector<std::string>::iterator iter = m_chainExclusionList.begin(); iter != m_chainExclusionList.end(); ++iter)
      ATH_MSG_DEBUG ( "\tChain: " << *iter );


    // build the final chain groups
    m_inclusionChainGroup = m_trigDecisionTool->getChainGroup(includedTriggers);
    m_exclusionChainGroup = m_trigDecisionTool->getChainGroup(excludedTriggers);

    // print for debugging purposes
    ATH_MSG_DEBUG ( "Configured status: " );
    std::vector<std::string> finalIncludedStreams = m_inclusionChainGroup->getListOfStreams();
    std::vector<std::string> finalIncludedGroups = m_inclusionChainGroup->getListOfGroups();
    std::vector<std::string> finalIncludedChains = m_inclusionChainGroup->getListOfTriggers();
    std::vector<std::string> finalExcludedStreams = m_exclusionChainGroup->getListOfStreams();
    std::vector<std::string> finalExcludedGroups = m_exclusionChainGroup->getListOfGroups();
    std::vector<std::string> finalExcludedChains = m_exclusionChainGroup->getListOfTriggers();

    ATH_MSG_DEBUG ( "Stream inclusion list: " );
    for(std::vector<std::string>::iterator iter = finalIncludedStreams.begin(); iter != finalIncludedStreams.end(); ++iter)
      ATH_MSG_DEBUG ( "\tStream: " << *iter );
    ATH_MSG_DEBUG ( "Stream exclusion list: " );
    for(std::vector<std::string>::iterator iter = finalExcludedStreams.begin(); iter != finalExcludedStreams.end(); ++iter)
      ATH_MSG_DEBUG ( "\tStream: " << *iter );
    ATH_MSG_DEBUG ( "Group inclusion list: " );
    for(std::vector<std::string>::iterator iter = finalIncludedGroups.begin(); iter != finalIncludedGroups.end(); ++iter)
      ATH_MSG_DEBUG ( "\tGroup: " << *iter );
    ATH_MSG_DEBUG ( "Group exclusion list: " );
    for(std::vector<std::string>::iterator iter = finalExcludedGroups.begin(); iter != finalExcludedGroups.end(); ++iter)
      ATH_MSG_DEBUG ( "\tGroup: " << *iter );
    ATH_MSG_DEBUG ( "Chain inclusion list: " );
    for(std::vector<std::string>::iterator iter = finalIncludedChains.begin(); iter != finalIncludedChains.end(); ++iter)
      ATH_MSG_DEBUG ( "\tChain: " << *iter );
    ATH_MSG_DEBUG ( "Chain exclusion list: " );
    for(std::vector<std::string>::iterator iter = finalExcludedChains.begin(); iter != finalExcludedChains.end(); ++iter)
      ATH_MSG_DEBUG ( "\tChain: " << *iter );

  } // end initialize on first event

  // remove branches
  if(m_branchInclusionList.size() > 0 || m_branchExclusionList.size() > 0) {
    if ( removeBranches(navigation, &m_branchInclusionList, &m_branchExclusionList).isFailure()) {
      ATH_MSG_WARNING ( "Unexpected error from removeBranches!" );
      ATH_MSG_WARNING ( "Navigation will not be slimmed" );
      return StatusCode::SUCCESS;
    }
  }

  // perform squeezing if applicable

  // protect chains as specified
  std::vector<HLT::TriggerElement*> *tesToProtect = 0;
  if(m_protectChains) { 
    tesToProtect = new std::vector<HLT::TriggerElement*>;
    // iterate through all the configured chains
    for(std::vector<std::string>::iterator iter = m_configuredChainNames.begin();
        iter != m_configuredChainNames.end(); ++iter) {
      // grab the TEs for that chain
      std::vector<HLT::TriggerElement*> *chainTEs = this->getTEsFromChainGroup(m_trigDecisionTool->getChainGroup(*iter));
      if(chainTEs) {
        // if a TE does not seed another TE in that chain, protect it
        for(std::vector<HLT::TriggerElement*>::const_iterator iterTE = chainTEs->begin();
            iterTE != chainTEs->end(); ++iterTE) {
          // grab the TEs the TE in question seeds
          std::vector<HLT::TriggerElement*> seeds = (*iterTE)->getRelated(HLT::TriggerElement::seedsRelation);
          bool isChainTerminal = true;
          // check if any of the TEs it seeds are in the chain
          for(std::vector<HLT::TriggerElement*>::const_iterator iterSeeds = seeds.begin();
              iterSeeds != seeds.end(); ++iterSeeds) {
            if(std::find(chainTEs->begin(), chainTEs->end(), (*iterSeeds)) != chainTEs->end()) {
              isChainTerminal = false;
              break;
            }
          }
          // if it is terminal for the chain, protect it
          if(isChainTerminal)
            tesToProtect->push_back(*iterTE);
        }
        delete chainTEs;
      }
    }
  }

  // do the squeezing
  if(m_doSqueeze) {
    if ( removeIntermediateTriggerElements(navigation, 0, tesToProtect).isFailure() ) {
      ATH_MSG_WARNING ( "Unexpected error from removeIntermediateTriggerElements!" );
      ATH_MSG_WARNING ( "Navigation will not be slimmed" );
      return StatusCode::SUCCESS;
    }
  }
  if(tesToProtect)
    delete tesToProtect;

  // remove ghosts if applicable
  if(m_removeGhosts) {
    if ( removeGhostTriggerElements(navigation).isFailure() ) {
      ATH_MSG_WARNING ( "Unexpected error from removeGhostTriggerElements!" );
      ATH_MSG_WARNING ( "Navigation will not be slimmed" );
      return StatusCode::SUCCESS;
    }
  }

  // remove features
  if( m_featureKeepSet.size() > 0 || m_featureDropSet.size() > 0 ) {
    ATH_MSG_DEBUG("will remove features");
    if ( removeFeatures(navigation, m_featureKeepSet, m_featureDropSet).isFailure()) {
      ATH_MSG_WARNING ( "Unexpected error from removeFeatures!" );
      ATH_MSG_WARNING ( "Navigation will not be slimmed" );
      return StatusCode::SUCCESS;
    }
  }

  // remove featureless TEs if applicable
  if(m_removeFeatureless) {
    if ( removeFeaturelessTriggerElements(navigation).isFailure() ) {
      ATH_MSG_WARNING ( "Unexpected error from removeFeaturelessTriggerElements!" );
      ATH_MSG_WARNING ( "Navigation will not be slimmed" );
      return StatusCode::SUCCESS;
    }
  }

  // grab the TE's corresponding to the included and excluded chains
  std::vector<HLT::TriggerElement*> *includedTriggerElements = this->getTEsFromChainGroup(m_inclusionChainGroup);
  std::vector<HLT::TriggerElement*> *excludedTriggerElements = this->getTEsFromChainGroup(m_exclusionChainGroup);

  // Add elements from failed chains if applicable
  if(m_removeFailedChains) {
    std::vector<HLT::TriggerElement*> *tes = this->getTEsFromFailedChains();
    for(std::vector<HLT::TriggerElement*>::iterator iter = tes->begin(); iter != tes->end();
        ++iter) {
      if(std::find(includedTriggerElements->begin(), includedTriggerElements->end(), *iter)
          == includedTriggerElements->end() &&
          std::find(excludedTriggerElements->begin(), excludedTriggerElements->end(), *iter)
          == excludedTriggerElements->end())
        excludedTriggerElements->push_back(*iter);
    }
    delete tes; tes = 0;
  }

  ATH_MSG_DEBUG ( "Have " << includedTriggerElements->size() << " TE's to be included, "
		  << "and " << excludedTriggerElements->size() << " TE's to be excluded" );

  // All we really need is a list of TE's to exclude.  If we have no TE's explicitly marked
  // for exclusion, then we go through the tree and remove all the TE's except for the ones
  // on the inclusion list.  If an exclusion list is specified, we simply remove all elements
  // on that list (note that we've already ensured that no elements on the inclusion list
  // get put on the exclusion list).  Note that the slimming tool handles all of this.

  if ( removeTriggerElementsFromNavigation(navigation, includedTriggerElements, excludedTriggerElements, 0, false).isFailure() ) {
    ATH_MSG_WARNING ( "Unexpected error in removeTriggerElementsFromNavigation!" );
    ATH_MSG_WARNING ( "Navigation will not be slimmed" );
    return StatusCode::SUCCESS;
  }

  delete includedTriggerElements;
  delete excludedTriggerElements;

  // if desired, clean up the navigation by removing the RoI nodes that have no elements left
  if(m_removeEmptyRoIs) {
    std::vector<HLT::TriggerElement*> RoINodes = navigation->getDirectSuccessors(navigation->getInitialNode());
    for(std::vector<HLT::TriggerElement*>::iterator iter = RoINodes.begin(); iter != RoINodes.end(); ++iter) {
      if(navigation->getDirectSuccessors(*iter).size() == 0) {
        if ( removeTriggerElementFromNavigation(navigation, *iter).isFailure() ) {
          ATH_MSG_WARNING ( "Unexpected error in removeEmptyRoIs" );
          ATH_MSG_WARNING ( "Navigation will not be slimmed" );
          return StatusCode::SUCCESS;
        }
      }
    }
  }

  if ( m_thinningSvc ) {
    ATH_MSG_DEBUG ( "Nonempty ThinningSvc, synchronizing to it" );
    /*
    // make some fake thinning for a test
    const TrigRoiDescriptorCollection* col(0);
    CHECK( evtStore()->retrieve(col, "HLT_TrigRoiDescriptorCollection_initialRoI") );
    IThinningSvc::VecFilter_t filter(col->size(), true);
    ATH_MSG_DEBUG ( "Preparing the test on the collection of size: " << col->size() );
    //    unsigned counter{};
    for ( size_t idx = 0; idx < col->size(); ++idx ) {
    // example worked on the paper
    if ( idx == 1 or idx == 3 or idx == 5 or idx == 6)
    filter[idx] = false;
    }    
    CHECK(m_thinningSvc->filter(*col,  filter ));
    // end of test insert
    */
    CHECK( adjustIndicesAfterThinning(navigation, m_thinningSvc) );
  }


  if(m_printTree)
    ATH_MSG_INFO ( *navigation );

  if(m_writeTree) {

    ATH_MSG_DEBUG ( "Serializing slimmed navigation structure, Getting HLTResult" );
    

    const HLT::HLTResult *constHltResult = 0;
    if ( evtStore()->retrieve(constHltResult, m_resultKey).isFailure() || !constHltResult ) {
      ATH_MSG_WARNING ( "Unable to load HLTResult with key " << m_resultKey << " from StoreGate..." );
      ATH_MSG_WARNING ( "Navigation will not be slimmed" );
      return StatusCode::SUCCESS;
    }
    HLT::HLTResult* result = const_cast<HLT::HLTResult*>(constHltResult);

    std::vector<uint32_t>& navData  = result->getNavigationResult();
    std::vector<unsigned int>& cuts = result->getNavigationResultCuts();
    
    navData.clear();
    cuts.clear();
    ATH_MSG_DEBUG ( "Actual serialziation starts now" );
    navigation->serialize( navData, cuts );
    ATH_MSG_DEBUG ( "Done" );

  } 
  
  
  if (m_reloadNavigation ) {

    ATH_MSG_DEBUG ( "Refreshing the navigation content..." );

    std::vector<uint32_t> navData; 
    std::vector<unsigned int> cuts;
    navigation->serialize(navData, cuts);
    navigation->reset();
    navigation->deserialize(navData);
  }
  return StatusCode::SUCCESS;
} // end of doSlimming



StatusCode HLT::TrigNavigationSlimmingTool::finalize() {
  m_navigation = 0;
  return StatusCode::SUCCESS;
}

/**********************************************************************
 *
 * TriggerSqueezing
 *
 **********************************************************************/

StatusCode HLT::TrigNavigationSlimmingTool::removeIntermediateTriggerElements(HLT::NavigationCore* navigation, 
									      TriggerElement *te,
									      std::vector<TriggerElement*> *tesToProtect) {


  // if the navigation is null, there's nothing we can do, so return a failure
  if(!navigation) {
    ATH_MSG_WARNING ( "removeIntermediateTriggerElements: Navigation handle is NULL... returning" );
    return StatusCode::FAILURE;
  }

  // if no argument was passed for te, we'll take the initial node of the navigation
  if(!te)
    te = navigation->getInitialNode();

  // if we can't get the initial node, the navigation structure is empty, and we can't do anything
  if(!te) {
    ATH_MSG_WARNING ( "removeIntermediateTriggerElements: Navigation is empty... returning" );
    return StatusCode::FAILURE;
  }

  // remember the navigation for later use
  m_navigation = navigation;

  // the idea is to recursively check each element until you find the terminal elements

  // if te is a terminal node, you're done
  if(navigation->isTerminalNode(te))
    return StatusCode::SUCCESS;

  // otherwise, gather the successors
  // note that we copy them to a new vector as we're going to be changing the relations around
  std::vector<TriggerElement*> successors(navigation->getDirectSuccessors(te));

  // if the node is not an ROI node or the initial node, remove it and propagate the relations
  if(!(navigation->isRoINode(te) || navigation->isInitialNode(te)) ) {
    // check if its protected
    if(!tesToProtect || std::find(tesToProtect->begin(), tesToProtect->end(), te) == tesToProtect->end()) {
      if(this->removeTriggerElementFromNavigation(navigation, te) == StatusCode::FAILURE)
	return StatusCode::FAILURE;
    }
  }

  // work recursively on the successors
  while(successors.size() > 0) {
    TriggerElement *curr = successors.back();
    StatusCode sc = this->removeIntermediateTriggerElements(navigation, curr, tesToProtect);
    if(sc.isFailure())
      return sc;
    successors.pop_back();
  }

  return StatusCode::SUCCESS;

}

StatusCode HLT::TrigNavigationSlimmingTool::removeTriggerElementFromNavigation(HLT::NavigationCore* navigation,
									       TriggerElement *te,
									       bool propagateFeatures) {

  // both arguments need be non-null or there's nothing we can do
  if(!navigation) {
    return StatusCode::FAILURE;
  }

  if(!te) {
    return StatusCode::FAILURE;
  }

  // refuse to remove the initial node
  if(navigation->isInitialNode(te)) {
    return StatusCode::FAILURE;
  }

  // remember the navigation structure for future use
  m_navigation = navigation;

  // mark the element as transient to prevent it from being serialized
  te->setTransient();

  // propagate the features to its children
  if(propagateFeatures)
    this->propagateFeaturesToChildren(te);

  // check the relations type by type

  // for those in the same RoI, we need only remove the relationship from ones who are related
  std::vector<TriggerElement*> sameRoI = te->m_relations[TriggerElement::sameRoIRelation];

  for(std::vector<TriggerElement*>::iterator nodeInRoI = sameRoI.begin(); nodeInRoI != sameRoI.end(); ++nodeInRoI) {
     
    // get the relations, and remove the ones that point to te
    std::vector<TriggerElement*> *relations = &((*nodeInRoI)->m_relations[ TriggerElement::sameRoIRelation ]);
    StatusCode sc = this->removeTriggerElementFromVector(te, relations);
    if(sc.isFailure())
      return sc;

  }

  // for those who te is seededBy, we need to remove the seeds relation, and propagate it down
  // (if te is not terminal)

  std::vector<TriggerElement*> seededBy = te->m_relations[TriggerElement::seededByRelation];

  for(std::vector<TriggerElement*>::iterator seededByNode = seededBy.begin(); seededByNode != seededBy.end();
      ++seededByNode) {

    // get the relations and remove the ones that point to te
    std::vector<TriggerElement*> *relations = &((*seededByNode)->m_relations[ TriggerElement::seedsRelation ]);
    StatusCode sc = this->removeTriggerElementFromVector(te, relations);
    if(sc.isFailure())
      return sc;

    // now add all the nodes te seeds onto the node we just removed the seeds relation from
    if(!m_navigation->isTerminalNode(te))
      (*seededByNode)->relate( te->m_relations[ TriggerElement::seedsRelation ], TriggerElement::seedsRelation );
  }

  // for those who te seeds, we need to remove the seededBy relation, and propagate it up
  // (if te is not the inital node - hey, you never know!)

  std::vector<TriggerElement*> seeds = te->m_relations[TriggerElement::seedsRelation];

  for(std::vector<TriggerElement*>::iterator seedsNode = seeds.begin(); seedsNode != seeds.end();
      ++seedsNode) {

    // get the relations and remove the ones that point to te
    std::vector<TriggerElement*> *relations = &((*seedsNode)->m_relations[ TriggerElement::seededByRelation ]);
    StatusCode sc = this->removeTriggerElementFromVector(te, relations);
    if(sc.isFailure())
      return sc;

    // now add all the nodes te seeds onto the node we just removed the seeds relation from
    if(!m_navigation->isInitialNode(te))
      (*seedsNode)->relate( te->m_relations[ TriggerElement::seededByRelation ], TriggerElement::seededByRelation );
  }

  return StatusCode::SUCCESS;
  
}
         
StatusCode HLT::TrigNavigationSlimmingTool::removeTriggerElementsFromNavigation(HLT::NavigationCore* navigation,
                                                                                std::vector<HLT::TriggerElement*> *inclusionList,
                                                                                std::vector<HLT::TriggerElement*> *exclusionList,
                                                                                HLT::TriggerElement *te,
                                                                                bool propagateFeatures) {

  

  
  if(!navigation) {
    ATH_MSG_WARNING ( "removeTriggerElementsFromNavigation: navigation Tool Handle is NULL.... returning" );
    return StatusCode::FAILURE;
  }

  // remember the navigation for later use
  m_navigation = navigation;

  if(!te)
    te = navigation->getInitialNode();

  if(!te) {
    ATH_MSG_WARNING ( "removeTriggerElementsFromNavigation: Navigation is empty.... returning" );
    return StatusCode::FAILURE;
  }

  // grab all the descending nodes - we're going to mess up the relations,
  // so we should make a copy of it.
  std::vector<TriggerElement*> children(te->m_relations[TriggerElement::seedsRelation]);

  // remove this node if necessary
  if(!this->toBeIncluded(te, inclusionList, exclusionList)) {
    StatusCode sc = this->removeTriggerElementFromNavigation(navigation, te, propagateFeatures);
    if(sc.isFailure())
      return sc;
  }

  // check the children
  for(std::vector<TriggerElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
    StatusCode sc = this->removeTriggerElementsFromNavigation(navigation, inclusionList, exclusionList, *iter, propagateFeatures);
    if(sc.isFailure())
      return sc;
  }
  

  return StatusCode::SUCCESS;

}

/**********************************************************************
 *
 * Feature Removal 
 *
 **********************************************************************/

StatusCode HLT::TrigNavigationSlimmingTool::removeFeatures(HLT::NavigationCore* navigation,
							   const std::set<std::string>& keepSet, 
							   const std::set<std::string>& dropSet) {
  // check that the navigation is non-NULL
  if(!navigation) {
    ATH_MSG_WARNING ( "removeFeatures: Navigation Tool Handle is NULL.... returning" );
    return StatusCode::FAILURE;
  }

  // remember the navigation structure for later
  m_navigation = navigation;

   


  // turn the inclusion and exclusion lists into set of pairs <CLID, SubTypeIndex> of this collections which needs to be dropped
  // in fact this a bit waste of time that we reclaulate this each time, but, one can imagine slimming events from different runs/configurations
  // but then we sould have to check some configuration in data (i.e. SMK in the HLTResult, and recompute m_deletedFeatures only if it changes)
  std::set<std::pair<CLID, uint16_t> > toDelete;


  //HLT::NavigationCore::FeaturesStructure::const_iterator types_iterator;
  typedef  std::map<uint16_t, HLTNavDetails::IHolder*> HoldersBySubType;
  //std::map<uint16_t, HLTNavDetails::IHolder*>::const_iterator holders_iterator;
  for( const HLT::NavigationCore::FeaturesStructure::value_type& ftype: m_navigation->m_featuresByIndex ) {
    for( const HoldersBySubType::value_type& holder: ftype.second ) {
      bool todel = false;
      HLTNavDetails::IHolder *h = holder.second;
      if(!h) { // check if h is null
	ATH_MSG_WARNING("holder.second is null pointer; skipping...");
	continue;
      }
      // check if this needs to be kept
      if ( !keepSet.empty() ) {
	if (  keepSet.count(h->collectionName())
	      || keepSet.count(h->collectionName()+"#"+h->key())   ) {
	  continue;
	} else {
	  todel = true;
	  ATH_MSG_DEBUG("will be dropping references associated to: " << h->collectionName()<<"#"<<h->key() );
	}
      }
      // check if this needs to be dropped
      if ( !dropSet.empty() ) {
	if ( dropSet.count(h->collectionName()) 
	     || dropSet.count(h->collectionName()+"#"+h->key()) ) {
	  todel =true;
	  ATH_MSG_DEBUG("will be dropping references associated to: " << h->collectionName()<<"#"<<h->key() );
	}
      }
      if ( todel ) 
	toDelete.insert(std::make_pair(h->typeClid(), h->subTypeIndex() ));
      // get rid of the holders themselves
      //m_navigation->m_featuresByIndex[ h->typeClid() ].erase( h->subTypeIndex() ); // Erasing an element of the map we're looping over
      m_navigation->m_featuresByLabel.at( h->typeClid() ).erase( h->label() ); // Use new C++11 map accessor (does bounds check)
      delete holder.second;
    }     
  }





  // plan is to recursively read through the whole tree looking for features
  return this->removeFeatures(m_navigation, toDelete);

  // now that we've removed them from the tree, we need to remove them from
  // the navigation structure as well.  We do this by finding the holders that
  // match the deleted labels and removing them.

  return StatusCode::SUCCESS;

}

StatusCode HLT::TrigNavigationSlimmingTool::removeFeatures(HLT::NavigationCore *nav,
							   const std::set<std::pair<CLID, uint16_t> >& toDelete) {



  // we have the following problem:
  // (a) features are stored as a vector where order can possibly matter
  // (b) its very time consuming to delete items from the middle of a vector
  // (c) a large fraction of elements will not have any features deleted
  //
  // To handle this, we're going to build an array that keeps track of
  // whether an element needs be deleted.  If none do, nothing gets changed.
  // If some do, we build a new vector and copy it over.  This prevents
  // the big time sink from deleting elements in the middle of a vector. 
  
  // another option would be to build a new vector when using inclusion
  // lists and remove elements when using exclusion lists

  using namespace HLT;
  //  TriggerElementFactory *factory = nav->m_factory;
  
  typedef std::pair<CLID, uint16_t> CI;

  std::vector< TriggerElement* > allTEs;
  nav->getAll(allTEs, false); // false includes also inactive TEs
  for ( const CI& ci: toDelete) {
    nav->m_featuresByIndex.at( ci.first ).erase( ci.second ); // Erase holders here
    for (TriggerElement* te: allTEs) {
      std::vector< TriggerElement::FeatureAccessHelper >& feas = te->getFeatureAccessHelpers();
      for ( TriggerElement::FeatureAccessHelper& fea: feas ) {
	if ( fea.getCLID() == ci.first && fea.getIndex().subTypeIndex() == ci.second )
	  fea.setForget(true); // when we mar this then the serializattion of TEs will simpley skip this one, easy, no?
      }
    }
  }


  return StatusCode::SUCCESS;

}

/**********************************************************************
 *
 * Branch Removal
 *
 **********************************************************************/

StatusCode HLT::TrigNavigationSlimmingTool::removeBranches(HLT::NavigationCore* navigation,
							   std::vector<std::string> *inclusionList,
							   std::vector<std::string> *exclusionList) {
 
  // check that the navigation is non-NULL
  if(!navigation) {
    ATH_MSG_WARNING ( "removeBranches: Navigation Tool Handle is NULL... returning" );
    return StatusCode::FAILURE;
  }

  // remember the navigation structure for later
  m_navigation = navigation;

  // check the RoI's
  std::vector<TriggerElement*> RoINodes = m_navigation->getDirectSuccessors(m_navigation->getInitialNode());
  for(std::vector<TriggerElement*>::iterator iter = RoINodes.begin(); iter != RoINodes.end(); ++iter) {

    if(!this->toBeIncluded( *iter, inclusionList, exclusionList )) {
      // we need to remove this RoI node as well as all nodes descended from it
      StatusCode sc = this->recursivelyRemoveNodesFromNavigation( m_navigation, *iter );
      if(sc.isFailure())
        return sc;
    }

  }

  return StatusCode::SUCCESS;

}


StatusCode HLT::TrigNavigationSlimmingTool::recursivelyRemoveNodesFromNavigation(HLT::NavigationCore* navigation,
										 TriggerElement *te) {

  if(!navigation)
    return StatusCode::FAILURE;

  // remember navigation for later use
  m_navigation = navigation;

  if(!te)
    return StatusCode::FAILURE;

  // grab all the descending nodes - we're going to mess up the relations,
  // so we should make a copy of it.
  std::vector<TriggerElement*> children(te->m_relations[TriggerElement::seedsRelation]);

  // remove this node
  StatusCode sc = this->removeTriggerElementFromNavigation(navigation, te);
  if(sc.isFailure())
    return sc;

  // remove the children
  for(std::vector<TriggerElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
    StatusCode sc = this->recursivelyRemoveNodesFromNavigation(navigation, *iter);
    if(sc.isFailure())
      return sc;
  }

  return StatusCode::SUCCESS;

}

/**********************************************************************
 *
 * Name generators 
 *
 **********************************************************************/

std::vector<std::string> *HLT::TrigNavigationSlimmingTool::getAllFeatureNames(
									      HLT::NavigationCore* navigation,
									      TriggerElement *te) {

  if(!navigation)
    return 0;

  // remember the navigation for later
  m_navigation = navigation;

  if(!te) {
    te = navigation->getInitialNode();
  }

  if(!te)
    return 0;

  // ignore features in the initial node and the RoI node
  std::vector<std::string> *names = new std::vector<std::string>;
  if(!navigation->isInitialNode(te) && !navigation->isRoINode(te)) {
    const std::vector<TriggerElement::FeatureAccessHelper>& features = te->getFeatureAccessHelpers();
    for(std::vector<TriggerElement::FeatureAccessHelper>::const_iterator iter = features.begin(); iter != features.end(); ++iter) {
      names->push_back(this->getLabel( *iter ));
    }
  }

  // add on the labels from the children
  std::vector<TriggerElement*> children = te->getRelated(TriggerElement::seedsRelation);
  for(std::vector<TriggerElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
    std::vector<std::string> *childNames = this->getAllFeatureNames(navigation, *iter);
    for(std::vector<std::string>::iterator cname = childNames->begin(); cname != childNames->end(); ++cname) {
      if(std::find(names->begin(), names->end(), *cname) == names->end())
        names->push_back( *cname );
    }
    delete childNames;
  }

  return names;

}

std::map<std::string, int> *HLT::TrigNavigationSlimmingTool::getFeatureOccurrences(
										   HLT::NavigationCore* navigation,
										   TriggerElement *te) {

  if(!navigation)
    return 0;

  // remember the navigation for later
  m_navigation = navigation;

  if(!te)
    te = navigation->getInitialNode();

  if(!te)
    return 0;

  std::map<std::string, int> *featureOccurrences = new std::map<std::string, int>;

  // ignore features in the initial node and the RoI node
  if(!navigation->isInitialNode(te) && !navigation->isRoINode(te)) {
    const std::vector<TriggerElement::FeatureAccessHelper>& features = te->getFeatureAccessHelpers();
    for(std::vector<TriggerElement::FeatureAccessHelper>::const_iterator iter = features.begin(); iter != features.end(); ++iter) {
      if( !(featureOccurrences->insert( std::pair< std::string, int>(this->getLabel( *iter ), 1) ).second ) )
        (*featureOccurrences)[this->getLabel(*iter)] += 1;
    }
  }

  // add on the labels from the children
  std::vector<TriggerElement*> children = te->getRelated(TriggerElement::seedsRelation);
  for(std::vector<TriggerElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
    std::map<std::string, int> *childOccurrences = this->getFeatureOccurrences(navigation, *iter);
    for(std::map<std::string, int>::iterator occ = childOccurrences->begin(); occ != childOccurrences->end(); ++occ) {
      if( !(featureOccurrences->insert( *occ ).second) )
        (*featureOccurrences)[ (*occ).first ] += (*occ).second;
    }
    delete childOccurrences;
  }

  return featureOccurrences;

}



std::vector<std::string> *HLT::TrigNavigationSlimmingTool::getAllBranchNames(
									     HLT::NavigationCore* navigation) { 

  if(!navigation)
    return 0;

  // remember the navigation for later
  m_navigation = navigation;

  std::vector<std::string> *names = new std::vector<std::string>;

  std::vector<TriggerElement*> branches = navigation->getDirectSuccessors(navigation->getInitialNode());
  for(std::vector<TriggerElement*>::iterator roi = branches.begin(); roi != branches.end(); roi++) {
    const std::vector<TriggerElement::FeatureAccessHelper>& features = (*roi)->getFeatureAccessHelpers();
    for(std::vector<TriggerElement::FeatureAccessHelper>::const_iterator iter = features.begin(); iter != features.end(); ++iter) {
      std::string name = this->getLabel( *iter );
      if(std::find(names->begin(), names->end(), name) == names->end())
        names->push_back(name);
    }
  }

  return names;

}

int HLT::TrigNavigationSlimmingTool::countFeature(HLT::NavigationCore* navigation, std::string *name, TriggerElement *te) {

  if(!navigation)
    return 0;

  // remember the navigation for later
  m_navigation = navigation;

  if(!te)
    te = navigation->getInitialNode();

  if(!te)
    return 0;

  if(!name)
    return 0;

  int count = 0;

  const std::vector<TriggerElement::FeatureAccessHelper>& features = te->getFeatureAccessHelpers();
  for(std::vector<TriggerElement::FeatureAccessHelper>::const_iterator iter = features.begin(); iter != features.end(); ++iter) {
    if( *name == this->getLabel( *iter) )
      count++;
  } 

  std::vector<TriggerElement*> children = navigation->getDirectSuccessors(te);
  for(std::vector<TriggerElement*>::iterator iter = children.begin(); iter != children.end(); ++iter)
    count += this->countFeature(navigation, name, *iter);

  return count;

}

std::vector<std::string> *HLT::TrigNavigationSlimmingTool::featureLabels(const HLT::TriggerElement *te) {

  if(!te)
    return 0;

  std::vector<std::string> *labels = new std::vector<std::string>;

  const std::vector<HLT::TriggerElement::FeatureAccessHelper>& features = te->getFeatureAccessHelpers();

  for(std::vector<HLT::TriggerElement::FeatureAccessHelper>::const_iterator iter = features.begin();
      iter != features.end(); ++iter)
    labels->push_back( this->getLabel(*iter) );

  return labels;

}

StatusCode HLT::TrigNavigationSlimmingTool::removeGhostTriggerElements(HLT::NavigationCore* navigation,
								       TriggerElement *te) { 


  if(!navigation) {
    ATH_MSG_WARNING ( "removeGhostTriggerElements: Navigation Tool Handle is NULL... returning" );
    return StatusCode::FAILURE;
  }

  // remember the navigation for later
  m_navigation = navigation;

  if(!te)
    te = navigation->getInitialNode();

  if(!te) {
    ATH_MSG_WARNING ( "removeGhostTriggerElements: Navigation is empty... returning" );
    return StatusCode::FAILURE;
  }

  // grab all the descending nodes - we're going to mess up the relations,
  // so we should make a copy of it.
  std::vector<TriggerElement*> children(te->m_relations[TriggerElement::seedsRelation]);

  // remove this node if necessary
  if(te->ghost()) {
    StatusCode sc = this->removeTriggerElementFromNavigation(navigation, te);
    if(sc.isFailure())
      return sc;
  }

  // check the children
  for(std::vector<TriggerElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
    StatusCode sc = this->removeGhostTriggerElements(navigation, *iter);
    if(sc.isFailure())
      return sc;
  }

  return StatusCode::SUCCESS;

}

StatusCode HLT::TrigNavigationSlimmingTool::removeFeaturelessTriggerElements(HLT::NavigationCore* navigation,
									     TriggerElement *te) { 


  
  if(!navigation) {
    ATH_MSG_WARNING ( "Navigation Tool Handle is NULL... returning" );
    return StatusCode::FAILURE;
  }

  // remember the navigation for later
  m_navigation = navigation;

  if(!te)
    te = navigation->getInitialNode();

  if(!te) {
    ATH_MSG_WARNING ( "Navigation is empty... returning" );
    return StatusCode::FAILURE;
  }

  // grab all the descending nodes - we're going to mess up the relations,
  // so we should make a copy of it.
  std::vector<TriggerElement*> children(te->m_relations[TriggerElement::seedsRelation]);

  // remove this node if necessary
  if(te->getFeatureAccessHelpers().size() == 0) {
    StatusCode sc = this->removeTriggerElementFromNavigation(navigation, te);
    if(sc.isFailure())
      return sc;
  }

  // check the children
  for(std::vector<TriggerElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
    StatusCode sc = this->removeFeaturelessTriggerElements(navigation, *iter);
    if(sc.isFailure())
      return sc;
  }

  return StatusCode::SUCCESS;

}

/**********************************************************************
 *
 * Generic Helper Functions
 *
 **********************************************************************/

int HLT::TrigNavigationSlimmingTool::removeTriggerElementFromVector(TriggerElement *te, std::vector<TriggerElement*> *v) {

  // note - deleting from a vector is a relatively slow operation.  As order of the relations does not matter,
  // it is probably better to implement the relations in the TriggerElement class as a set rather than as a
  // vector.  For now, though, we'll do a little trick:
  // We find the TriggerElement that the last element of the vector points to, then we'll overwrite the relation
  // we wish to get rid of with that element and pop the back of the vector off.
  // Note that this works even if the element we want to get rid of is the last (or only) element in the vector
    
  if(!v || v->empty())
    return 0;

  int removed = 0;

  int size = v->size();
  TriggerElement *last = v->back();
  std::vector<TriggerElement*>::iterator iter = v->begin();
  for(int i = 0; i < size; i++) {

    if( (*iter) == te) {
      removed++;
      size--;
      (*iter) = last;
      v->pop_back();
      if(!v->empty())
	last = v->back();
      else 
	last = 0;
    }

    iter++;

  }

  return removed;

}

bool HLT::TrigNavigationSlimmingTool::toBeIncluded(TriggerElement *te, 
						   std::vector<std::string> *inclusionList, std::vector<std::string> *exclusionList) {

  // if the inclusion list exists, then we use that
  if(inclusionList && inclusionList->size() > 0) {

    // check if the trigger element has a feature that is included in the list
    int onExclusionList = 0;
    for(std::vector< TriggerElement::FeatureAccessHelper >::const_iterator iter =
	  te->getFeatureAccessHelpers().begin();
        iter != te->getFeatureAccessHelpers().end(); ++iter) {
      // grab the label and check if it was found in the inclusion list
      if(std::find(inclusionList->begin(), inclusionList->end(), this->getLabel( *iter )) != inclusionList->end())
        return true;
      // now find if its in the exclusion list
      if(exclusionList && std::find(exclusionList->begin(), exclusionList->end(), 
				    this->getLabel( *iter )) != exclusionList->end())
        onExclusionList = 1;
    }

    // if it has a feature on the exclusion list, and none on the inclusion list,
    // it should be excluded
    if(onExclusionList)
      return false;

    // we've gone through the feature list, and nothing was on the exclusion list,
    // so we should keep it
    return true;
  }

  // if we get here, then there's no inclusionList, so we need to use the exclusion list

  // check if it exists - if it doesn't, pass everything
  if(!exclusionList || exclusionList->size() == 0)
    return true;

  for(std::vector< TriggerElement::FeatureAccessHelper >::const_iterator iter = 
	te->getFeatureAccessHelpers().begin();
      iter != te->getFeatureAccessHelpers().end(); ++iter) {
    // grab the label and check if it was found in the exclusion list
    if(std::find(exclusionList->begin(), exclusionList->end(), this->getLabel( *iter )) != exclusionList->end())
      return false;
  }

  // if we've made it here, its passed the exclusion list and so should be included
  return true;

}

bool HLT::TrigNavigationSlimmingTool::toBeIncluded(TriggerElement *te, 
						   std::vector<TriggerElement*> *inclusionList, 
						   std::vector<TriggerElement*> *exclusionList) {

  if(!te)
    return false;

  // this function never allows inital nodes or RoI nodes to be removed
  if( m_navigation->isInitialNode(te) || m_navigation->isRoINode(te) )
    return true;

  // if the inclusion list exists, then we use that
  // If its on the inclusion list, its kept for sure
  // If its on the exclusion list, its only kept if
  // its not on the inclusion list.
  if(inclusionList && inclusionList->size() > 0) {
    
    // check if the element is in the list
    if(std::find_if(inclusionList->begin(), inclusionList->end(), 
		    TriggerElementFind(te)) != inclusionList->end())
      return true;
    // now, check if its in the exclusion list
    if(exclusionList && std::find_if(exclusionList->begin(), exclusionList->end(),
				     TriggerElementFind(te)) != exclusionList->end())
      return false;
    // if its on neither list, keep it
    return true;
  }

  // if we get here, then there's no inclusionList, so we need to use the exclusion list

  // check if it exists - if it doesn't, pass everything
  if(!exclusionList || exclusionList->size() == 0)
    return true;

  // check if the te belongs to the exclusion list
  if(std::find_if(exclusionList->begin(), exclusionList->end(), 
		  TriggerElementFind(te)) != exclusionList->end()) {
    return false;
  }

  // if we've made it here, its passed the exclusion list and so should be included
  return true;

}

std::string HLT::TrigNavigationSlimmingTool::getLabel(const TriggerElement::FeatureAccessHelper &fah) const {

  CLID clid = fah.getCLID();
  std::string label;
  HLTNavDetails::IHolder *h = m_navigation->getHolder(clid, fah.getIndex().subTypeIndex());
  if(h) label = h->label();

  return label;
}

void HLT::TrigNavigationSlimmingTool::propagateFeaturesToChildren(const TriggerElement *te) {

  if(!te)
    return;

  const std::vector<TriggerElement::FeatureAccessHelper> features = te->getFeatureAccessHelpers() ;

  std::vector<TriggerElement*> children = te->m_relations[TriggerElement::seedsRelation];
  for(std::vector<TriggerElement*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
    // add the parents features to the front of the child features list features list
    (*iter)->getFeatureAccessHelpers().insert((*iter)->getFeatureAccessHelpers().begin(), features.begin(), features.end());
  }

}

HLT::TrigNavigationSlimmingTool::TriggerElementFind::TriggerElementFind(const TriggerElement *te) {

  // return the id the RoI node
  if(!te) {
    m_id = 0;
    m_RoI = 0;
  }
  else { 
    m_id = te->getId();

    const TriggerElement *mother = te;
    m_RoI = te;
    while( mother->getRelated(TriggerElement::seededByRelation).size() > 0 ) {
      m_RoI = mother;
      mother = m_RoI->getRelated(TriggerElement::seededByRelation)[0];
    }

  }

}

bool HLT::TrigNavigationSlimmingTool::TriggerElementFind::operator()(const TriggerElement *te) {

  if(!te)
    return 0;

  // if we didn't initialize the RoI, then we can't match
  if(!m_RoI)
    return false;

  // check that the id's match
  if(te->getId() != m_id)
    return 0;

  // Note that we can have elements with the same id as long as they are in difference RoI's
  // Thus, we must ensure that the RoI's are the same

  // Find the RoI
  const TriggerElement *mother = te;
  const TriggerElement *RoI = te;

  while( mother->getRelated(TriggerElement::seededByRelation).size() > 0 ) {
    RoI = mother;
    mother = RoI->getRelated(TriggerElement::seededByRelation)[0];
  }

  // now, RoI equality is defined by all the features that match, where a feature
  // match is given by a matching CLID, subtypeIndex, and objectIndex

  const std::vector< TriggerElement::FeatureAccessHelper > firstFeatures = m_RoI->getFeatureAccessHelpers();
  const std::vector< TriggerElement::FeatureAccessHelper > secondFeatures = m_RoI->getFeatureAccessHelpers();

  if(firstFeatures.size() != secondFeatures.size())
    return false;

  for(unsigned int i = 0; i < firstFeatures.size(); i++)  {
    TriggerElement::FeatureAccessHelper firstFeature = firstFeatures[i];
    TriggerElement::FeatureAccessHelper secondFeature = secondFeatures[i];

    if(firstFeature.getCLID() != secondFeature.getCLID())
      return false;
    if(firstFeature.getIndex().subTypeIndex() != secondFeature.getIndex().subTypeIndex())
      return false;
    if(firstFeature.getIndex().objectsBegin() != secondFeature.getIndex().objectsBegin())
      return false;
    if(firstFeature.getIndex().objectsEnd() != secondFeature.getIndex().objectsEnd())
      return false;

  }

  // if we've made it this far, all the features match (what should we do if there
  // are no features?), so we have found a match!

  return true;

}


namespace {

  static const std::size_t RemovedIdx = static_cast<std::size_t>(-1);
  struct IndexRecalculator {
    IndexRecalculator( IThinningSvc* thinningSvc, const void *collection) 
      : m_thinningSvc(thinningSvc),
	m_collection(collection)
    {}

    size_t getNewIndex(size_t oldIndex) {
      if ( oldIndex >= m_indices.size() ) {
	expand(oldIndex);
      }
      return m_indices[oldIndex];
    }

    void expand(size_t maxIndex ) {
      const size_t checkedSoFar = m_validIndices.size();
      m_indices.resize(maxIndex+1, RemovedIdx);
      m_validIndices.resize(maxIndex+1);

      for ( size_t toScan = checkedSoFar; toScan <= maxIndex; ++ toScan ) {
	size_t newIndex = m_thinningSvc->index(m_collection, toScan);
	m_validIndices[toScan] = newIndex;	
      }

      for ( size_t toScan = checkedSoFar; toScan <= maxIndex; ++toScan ) {
	m_indices[toScan] = std::count_if(m_validIndices.begin(), m_validIndices.begin()+toScan, 
					  [](const size_t x){ return x != RemovedIdx; }  );
      }
	
    }

    std::vector<size_t> m_indices = {0}; // the 0 index will be 0 no mater what is slimmed
    std::vector<size_t> m_validIndices; // the 0 index will be 0 no mater what is slimmed
    IThinningSvc* m_thinningSvc;
    const void* m_collection;
  };
}

StatusCode HLT::TrigNavigationSlimmingTool::adjustIndicesAfterThinning(HLT::NavigationCore* navigation, ServiceHandle<IThinningSvc>& thinningSvc ) {
  ATH_MSG_DEBUG ( "Running the adjustIndicesAfterThinning" );
  for(auto& clidMap: navigation->m_featuresByLabel) {
    for ( auto& labelHolder: clidMap.second ) {
      auto holder = labelHolder.second;
      holder->syncWithSG();
      auto containerPointer = holder->containerTypeProxy().cptr();
      if ( thinningSvc->thinningOccurred(containerPointer)  )  {
	ATH_MSG_DEBUG ( "Thinning occured for this container" << *holder <<", going to ajust the indices" );
	// ThinningSvc::index method returns the valid new index for unslimmed object and an invalid index IThinningSvc::RemovedIndex for the ones that were removed
	// The way to calulate new indexes for the ranges describing ROIs (X,Y) is to count the number valid indexes from X to 0 and from Y to 0
	// This would be quite inefficient so we need to make a vector wiht new indexes
	// this vector in the end will be of size equal to the original collection and at position X will have new values.
	// Hoever the difficulty is that we are not able to obtain the size of the collection and we need to build this vector as we meet the indexs during 
	// the scan of TEs. 
	// Since this is quite tricky code it is outsourced to a helper class IndexRecalculator.
	
	IndexRecalculator recalculator( &*thinningSvc, containerPointer);
	// nowe we need to go over the TEs
	for ( const auto& te: navigation->getAllTEs() ) {
	  for ( auto& fea: te->getFeatureAccessHelpers() ) {
	    if ( fea.getCLID() == holder->typeClid() 
		 and fea.getIndex().subTypeIndex() == holder->subTypeIndex() ) {
	      uint32_t begin = fea.getIndex().objectsBegin();
	      uint32_t end   = fea.getIndex().objectsEnd();
	      uint32_t newEnd = recalculator.getNewIndex(end);
	      uint32_t newBegin = recalculator.getNewIndex(begin);
	      
	      const_cast<HLT::TriggerElement::ObjectIndex&>(fea.getIndex()).updateBeginAndEnd(newBegin, newEnd);
	      //ATH_MSG_DEBUG( "Indices changed to " << newBegin << " " << newEnd);

	    }	  
	  }
	}

      } // if thinning occures 
      else {
	//	ATH_MSG_DEBUG("Thinning did not occure on the " << * holder );
      }
    } // overll holders
  } // over types (CLIDs)
  return StatusCode::SUCCESS;
}



std::vector<HLT::TriggerElement*> *HLT::TrigNavigationSlimmingTool::getTEsFromFailedChains() {

  std::vector<HLT::TriggerElement*> *tes = new std::vector<HLT::TriggerElement*>;
  for(std::vector<std::string>::const_iterator iter = m_configuredChainNames.begin();
      iter != m_configuredChainNames.end(); ++iter) {
    if(!m_trigDecisionTool->isPassed(*iter)) {
      std::vector<HLT::TriggerElement*> *chainTEs = this->getTEsFromChainGroup(m_trigDecisionTool->getChainGroup(*iter));
      tes->insert(tes->end(), chainTEs->begin(), chainTEs->end());
      delete chainTEs; chainTEs = 0;
    }
  }

  return tes;

}

std::vector<HLT::TriggerElement*> *HLT::TrigNavigationSlimmingTool::getTEsFromChainGroup(const Trig::ChainGroup *cg) {

  std::vector<HLT::TriggerElement*> *tes = new std::vector<HLT::TriggerElement*>;
  
  std::vector<Trig::Combination> combinations = m_trigDecisionTool->features(cg).getCombinations();

  for(std::vector<Trig::Combination>::const_iterator iter = combinations.begin();
      iter != combinations.end(); ++iter) {
    std::vector<const HLT::TriggerElement*> combTEs = iter->tes();
    for(std::vector<const HLT::TriggerElement*>::const_iterator iterTE = combTEs.begin();
        iterTE != combTEs.end(); ++iterTE) {
      tes->push_back(const_cast<HLT::TriggerElement*>(*iterTE));
    }
  }

  return tes;
}
