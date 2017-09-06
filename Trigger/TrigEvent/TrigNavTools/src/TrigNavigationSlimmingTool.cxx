/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include "boost/foreach.hpp"

#include "TrigDecisionTool/ChainGroup.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "AthenaKernel/IThinningSvc.h"
#include "TrigNavigation/Navigation.h"
#include "TrigNavigation/TriggerElement.h"
//#include "TrigNavigation/TriggerElementFactory.h"
#include "TrigNavigation/Holder.h"


#include "TrigNavTools/TrigNavigationSlimmingTool.h"

#include <iostream>

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
    m_thinningSvc("", name)
{

  declareInterface<TrigNavigationSlimmingTool>(this);
  
  // job option configurable properties
  declareProperty("TrigDecisionTool", m_trigDecisionTool, "Tool handle to TDT/Navigation.");
  declareProperty("ThinningSvc", m_thinningSvc, "Synchronize feature indexes wiht this instance of ThinningSvc");
  declareProperty("ChainsRegex", m_chainsRegex="", "Keep only information related to this chains");
  declareProperty("FeatureInclusionList", m_featureInclusionList, "This features will be kept. This setting overrules the FeatureExclusionList. Only list of types or type#key pairs are supported.");
  declareProperty("FeatureExclusionList", m_featureExclusionList, "This features will be dropeed. It can be specified as * meaning all, or as a list of typenames, or typename#key pairs.");
  declareProperty("ReportOperations", m_report=false, "Additional verbosity flag, when enabled the operations on trigger elements are reported (VERBOSE logging level)");

  m_actionsMap["Drop"]    = &HLT::TrigNavigationSlimmingTool::drop;
  m_actionsMap["Reload"]  = &HLT::TrigNavigationSlimmingTool::reload;
  m_actionsMap["Restore"] = &HLT::TrigNavigationSlimmingTool::restore;
  m_actionsMap["Print"]   = &HLT::TrigNavigationSlimmingTool::print;
  m_actionsMap["Save"]    = &HLT::TrigNavigationSlimmingTool::save;
  m_actionsMap["Squeeze"]       = &HLT::TrigNavigationSlimmingTool::squeeze;
  m_actionsMap["DropFeatures"]  = &HLT::TrigNavigationSlimmingTool::dropFeatures;
  m_actionsMap["DropRoIs"]      = &HLT::TrigNavigationSlimmingTool::dropRoIs;
  m_actionsMap["DropEmptyRoIs"] = &HLT::TrigNavigationSlimmingTool::dropEmptyRoIs;
  m_actionsMap["DropFeatureless"] = &HLT::TrigNavigationSlimmingTool::dropFeatureless;
  m_actionsMap["SyncThinning"]     = &HLT::TrigNavigationSlimmingTool::syncThinning;
  m_actionsMap["DropChains"]     = &HLT::TrigNavigationSlimmingTool::dropChains;


  std::string possibleActions;
  for ( auto a: m_actionsMap )
    possibleActions += a.first +" ";
  declareProperty("Actions", m_actions, "Operations which need to be done on the navigation"+possibleActions);
}


HLT::TrigNavigationSlimmingTool::~TrigNavigationSlimmingTool() {
}

/**********************************************************************
 *
 * Initialization and Finalization 
 *
 **********************************************************************/

StatusCode HLT::TrigNavigationSlimmingTool::initialize() {

  ATH_MSG_DEBUG( "TrigNavigationSlimmingTool::initialize()" << name() );

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
  if ( m_featureExclusionList.size() and m_featureInclusionList.size() ) {
    ATH_MSG_ERROR( "Can't use the features inclusion and exclusion lists at the same time.");
    return StatusCode::FAILURE;
  }

  m_featureKeepSet.insert( m_featureInclusionList.begin(), m_featureInclusionList.end() );
  m_featureDropSet.insert( m_featureExclusionList.begin(), m_featureExclusionList.end() );
  if ( m_featureKeepSet.size() or m_featureDropSet.size() ) {
    ATH_MSG_DEBUG("Configured features removeal keep:" <<  m_featureInclusionList
		   << " drop: " << m_featureExclusionList );
  } else {
    if ( find(m_actions.begin(), m_actions.end(), "DropFeatures") != m_actions.end() ) {      
      ATH_MSG_FATAL("Dropping features is demanded but neither inclusion and exclusion lists are set");
      return StatusCode::FAILURE;
    }    
  }



  // verify if all actions are possible 
  for ( auto& action: m_actions) {
    if ( m_actionsMap.find(action) == m_actionsMap.end()) {
      ATH_MSG_FATAL("Action not implemented (check for typo) " << action << " possible " << m_actions);
      return StatusCode::FAILURE;
    }
  }  
  return StatusCode::SUCCESS;
}


StatusCode
HLT::TrigNavigationSlimmingTool::drop(State& state) const
{
  state.destinationNavigation.clear();
  ATH_MSG_DEBUG ( "Navigation dropped entirely" );
  return StatusCode::SUCCESS;
}

StatusCode
HLT::TrigNavigationSlimmingTool::reload (State& state) const
{
  state.destinationNavigation.clear();
  std::vector<unsigned int> cuts;
  std::vector<uint32_t> temp;
  state.navigation.serialize(temp, cuts);
  state.navigation.reset();
  state.navigation.prepare();
  state.navigation.deserialize(temp);
  ATH_MSG_DEBUG ( "Reloaded the navigation content in TDT (all clients will see reduced navigation content) ..." );
  return StatusCode::SUCCESS;
}


StatusCode HLT::TrigNavigationSlimmingTool::save(State& state) const {
  std::vector<unsigned int> cuts;
  state.navigation.serialize(state.destinationNavigation, cuts);
  ATH_MSG_DEBUG ( "Saved the slimmed navigation" );
  return StatusCode::SUCCESS;
}

StatusCode
HLT::TrigNavigationSlimmingTool::restore(State& state) const
{
  state.navigation.reset();
  state.navigation.prepare();
  state.navigation.deserialize(state.originalNavigation);
  ATH_MSG_DEBUG ( "Restored the original navigation" );
  return StatusCode::SUCCESS;
}



StatusCode HLT::TrigNavigationSlimmingTool::print(State& state) const {
  ATH_MSG_DEBUG ( "Navigation printout \n" << state.navigation );
  return StatusCode::SUCCESS;
}

StatusCode HLT::TrigNavigationSlimmingTool::squeeze(State& state) const {  
  for ( auto te: state.navigation.getAllTEs() ) {
    if ( state.navigation.isInitialNode(te) 
	 or state.navigation.isRoINode(te) 
	 or state.navigation.isTerminalNode(te) 
	 or state.tesToProtect.find(te->getId()) != state.tesToProtect.end())
      continue;
    //    if ( te->getId() == 4032407525 ) m_report = true;
    CHECK( removeTriggerElement(state, te) );
    //    m_report = false;
  }

  return StatusCode::SUCCESS;
}
StatusCode HLT::TrigNavigationSlimmingTool::dropFeatures(State& state) const {
  // turn the inclusion and exclusion lists into set of pairs <CLID, SubTypeIndex> of this collections which needs to be dropped
  // in fact this a bit waste of time that we reclaulate this each time, but, one can imagine slimming events from different runs/configurations
  // but then we sould have to check some configuration in data (i.e. SMK in the HLTResult, and recompute m_deletedFeatures only if it changes)
  std::set<std::pair<CLID, uint16_t> > toDelete;
  std::set<std::pair<CLID, uint16_t> > toRetain;


  //HLT::NavigationCore::FeaturesStructure::const_iterator types_iterator;
  //typedef  std::map<uint16_t, HLTNavDetails::IHolder*> HoldersBySubType;
  //std::map<uint16_t, HLTNavDetails::IHolder*>::const_iterator holders_iterator;
  for( auto h : state.navigation.m_holderstorage.getAllHolders<HLTNavDetails::IHolder>() ) {
    if(!h) { // check if h is null
      ATH_MSG_WARNING("holder.second is null pointer; skipping...");
      continue;
    }
    //ATH_MSG_VERBOSE("Checking what to do with " 
    //		      << h->collectionName()+"#"+h->key());
    // check if this needs to be kept
    if ( not m_featureKeepSet.empty() ) {
      if (  m_featureKeepSet.count(h->collectionName())
	    || m_featureKeepSet.count(h->key()) ) {
	toRetain.insert(std::make_pair(h->typeClid(), h->subTypeIndex() ));
	ATH_MSG_DEBUG("will be keeping references associated to: " << h->collectionName()<<"#"<<h->key() << " clid: " << h->typeClid() );
      } 
    }
    // check if this needs to be dropped
    if ( not m_featureDropSet.empty() ) {
      if ( m_featureDropSet.count(h->collectionName()) 
	   || m_featureDropSet.count(h->key()) ) {
	toDelete.insert(std::make_pair(h->typeClid(), h->subTypeIndex() ));
	ATH_MSG_DEBUG("will be dropping references associated to: " << h->collectionName()<<"#"<<h->key() );
      }
    }
  }     
  

  if ( not toRetain.empty() )
    return retainFeatures(state, toRetain);
  if ( not toDelete.empty() )
    return removeFeatures(state, toDelete);

  // now that we've removed them from the tree, we need to remove them from
  // the navigation structure as well.  We do this by finding the holders that
  // match the deleted labels and removing them.
  // to be implemented !!!

  return StatusCode::SUCCESS;  
}

StatusCode HLT::TrigNavigationSlimmingTool::dropRoIs(State& state) const {
  for ( auto te: state.navigation.getAllTEs() ) {
    if ( state.navigation.isRoINode(te) )
      CHECK( removeTriggerElement(state, te) );
  }
  return StatusCode::SUCCESS;  
}

StatusCode HLT::TrigNavigationSlimmingTool::dropEmptyRoIs(State& state) const {
  for ( auto te: state.navigation.getAllTEs() ) {
    if ( state.navigation.isRoINode(te) 
	 and te->getRelated(TriggerElement::seedsRelation).empty() )
      CHECK( removeTriggerElement(state, te) );
  }
  return StatusCode::SUCCESS;  
}

StatusCode HLT::TrigNavigationSlimmingTool::dropFeatureless(State& state) const {
  for ( auto te: state.navigation.getAllTEs() ) {
    if ( te->getFeatureAccessHelpers().empty() )
      CHECK( removeTriggerElement(state, te) );
  }  
  return StatusCode::SUCCESS;  
}


StatusCode HLT::TrigNavigationSlimmingTool::dropChains(State& state) const {
  if ( m_chainsRegex.empty() ) {
    return StatusCode::SUCCESS;
  }
  // now run over the tree and drop alle TEs except RoIs and intial which are not in the TEs to protect
  for ( auto te: state.navigation.getAllTEs() ) {
    if ( state.navigation.isInitialNode(te)
	 or state.navigation.isRoINode(te) ) {
      continue;
    }
    if ( state.tesToProtect.find(te->getId()) == state.tesToProtect.end() ) {
      CHECK( removeTriggerElement(state, te));
    }
  }
  return StatusCode::SUCCESS;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode HLT::TrigNavigationSlimmingTool::doSlimming( std::vector<uint32_t>& slimmed_and_serialized) const {

  // grab the navigation
  Trig::ExpertMethods *navAccess = m_trigDecisionTool->ExperimentalAndExpertMethods();
  navAccess->enable();
  // FIXME: const_cast
  HLT::NavigationCore *cnav = const_cast<HLT::NavigationCore*>(navAccess->getNavigation());

  if(cnav == 0) {
    ATH_MSG_WARNING ( "Could not get navigation from Trigger Decision Tool" );
    ATH_MSG_WARNING ( "Navigation will not be slimmed in this event" );
    return StatusCode::SUCCESS;
  }
    
  State state (*cnav, slimmed_and_serialized);
  CHECK( lateFillConfiguration(state) );
  {
    state.originalNavigation.clear();
    std::vector<uint32_t> cuts;
    cnav->serialize(state.originalNavigation, cuts);
  }

  for ( auto& action: m_actions ) {
    ATH_MSG_DEBUG("Applying action " << action << " on the navigation ");
    auto ifunc = m_actionsMap.find (action);
    if (ifunc != m_actionsMap.end()) {
      auto function = ifunc->second;
      CHECK( (this->*function)(state) );
    }
  }
  return StatusCode::SUCCESS;
}



StatusCode HLT::TrigNavigationSlimmingTool::finalize() {
  return StatusCode::SUCCESS;
}



std::map<std::string, int> *HLT::TrigNavigationSlimmingTool::getFeatureOccurrences(HLT::NavigationCore* navigation) {
  if(!navigation)
    return 0;
  
  if (navigation->getInitialNode() == 0 )
    return 0;
  
  std::map<std::string, int> *featureOccurrences = new std::map<std::string, int>;


  for ( auto te : navigation->getAllTEs() ) {
    for ( const auto& fea: te->getFeatureAccessHelpers() ) {
      (*featureOccurrences)[getLabel(*navigation, fea)] += 1;
    }
  }

  return featureOccurrences;
}

std::string HLT::TrigNavigationSlimmingTool::getLabel(const HLT::NavigationCore& navigation,
                                                      const TriggerElement::FeatureAccessHelper &fah) const {

  CLID clid = fah.getCLID();
  std::string label;
  HLTNavDetails::IHolder *h = navigation.getHolder(clid, fah.getIndex().subTypeIndex());
  if(h) label = h->label();

  return label;
}





StatusCode
HLT::TrigNavigationSlimmingTool::lateFillConfiguration(State& state) const {
  // remember the configured chain names, as they will be useful later  
  // ??? Originally, this was done once and cached in the tool.
  //     If this takes too long, consider storing it in the detector store.
  auto chainGroup = m_trigDecisionTool->getChainGroup(m_chainsRegex);
  ATH_MSG_INFO("Will keep information related to this chains" << chainGroup->getListOfTriggers());
  auto confTEs = chainGroup->getHLTTriggerElements();
  for ( auto& vec: confTEs) {
    for ( auto confTEPtr: vec) {
      state.tesToProtect.insert(confTEPtr->id());
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode HLT::TrigNavigationSlimmingTool::removeTriggerElement(State& state,
                                                                 TriggerElement *te,
								 bool propagateFeatures) const {
  // refuse to remove the initial node
  if(state.navigation.isInitialNode(te)) {
    return StatusCode::SUCCESS;
  }
  if ( m_report )  ATH_MSG_VERBOSE("Removing TE of ID: " << te->getId() );
  
  // mark the element as transient to prevent it from being serialized
  te->setTransient();


  // propagate the features to its children
  if ( propagateFeatures )
    CHECK( propagateFeaturesToChildren(te) );



  if ( m_report )  ATH_MSG_VERBOSE("Removeing TE of ID: " << te->getId() << " removing same RoI relations" );
  // for those in the same RoI, we need only remove the relationship from ones who are related
  std::vector<TriggerElement*>& sameRoI = te->m_relations[TriggerElement::sameRoIRelation];

  for( auto nodeInRoI : sameRoI ) {
    if ( m_report )  ATH_MSG_VERBOSE("Removeing TE of ID: " << te->getId() << " bypassing same RoI relation " );
    // get the relations, and remove the ones that point to te
    std::vector<TriggerElement*>& relations = nodeInRoI->m_relations[ TriggerElement::sameRoIRelation ];
    CHECK( removeTriggerElementFromVector (te, relations) );
  }

  // for those who te is seededBy, we need to remove the seeds relation, and propagate it down
  // (if te is not terminal)

  std::vector<TriggerElement*> seededBy = te->m_relations[TriggerElement::seededByRelation];

  for(auto seededByNode:  seededBy ) {

    // get the relations and remove the ones that point to te
    std::vector<TriggerElement*>& relations = seededByNode->m_relations[ TriggerElement::seedsRelation ];
    CHECK( removeTriggerElementFromVector (te, relations) );
    
    

    // now add all the nodes te seeds onto the node we just removed the seeds relation from
    if(!state.navigation.isTerminalNode(te))
      seededByNode->relate( te->m_relations[ TriggerElement::seedsRelation ], TriggerElement::seedsRelation );
  }

  // for those who te seeds, we need to remove the seededBy relation, and propagate it up
  // (if te is not the inital node - hey, you never know!)

  std::vector<TriggerElement*>& seeds = te->m_relations[TriggerElement::seedsRelation];

  for( auto seedsNode: seeds ) {

    // get the relations and remove the ones that point to te
    std::vector<TriggerElement*>& relations = seedsNode->m_relations[ TriggerElement::seededByRelation ];
    CHECK ( removeTriggerElementFromVector(te, relations) );

    // now add all the nodes te seeds onto the node we just removed the seeds relation from
    if(!state.navigation.isInitialNode(te))
      seedsNode->relate( te->m_relations[ TriggerElement::seededByRelation ], TriggerElement::seededByRelation );
  }

  return StatusCode::SUCCESS;
}

/**********************************************************************
 *
 * Feature Removal 
 *
 **********************************************************************/
StatusCode HLT::TrigNavigationSlimmingTool::removeFeatures(State& state,
                                                           const std::set<std::pair<CLID, uint16_t> >& toDelete) const {
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
  ATH_MSG_DEBUG("Will remove " << toDelete.size()<< " feature type/key");
  using namespace HLT; 
   for (auto te: state.navigation.getAllTEs()) {
    for ( auto& fea: te->getFeatureAccessHelpers() ) {
      if ( toDelete.find( std::make_pair(fea.getCLID(), fea.getIndex().subTypeIndex()) ) != toDelete.end() )	
	fea.setForget(true); // when we mark this then the serialization of TEs will simpley skip this one, easy, no
    }
  }
   // get back here
   // get rid of the holders themselves
      //m_navigation->m_featuresByIndex[ h->typeClid() ].erase( h->subTypeIndex() ); // Erasing an element of the map we're looping over
   //m_navigation->m_featuresByLabel.at( h->typeClid() ).erase( h->label() ); // Use new C++11 map accessor (does bounds check)
   //      delete holder.second;

  return StatusCode::SUCCESS;
}


StatusCode HLT::TrigNavigationSlimmingTool::retainFeatures(State& state,
                                                           const std::set<std::pair<CLID, uint16_t> >& toRetain) const {
  ATH_MSG_DEBUG("Will retain " << toRetain.size()<< " feature type/key");
  using namespace HLT; 
   for (auto te: state.navigation.getAllTEs()) {
    for ( auto& fea: te->getFeatureAccessHelpers() ) {
      if ( toRetain.find( std::make_pair(fea.getCLID(), fea.getIndex().subTypeIndex()) ) == toRetain.end() )	
	fea.setForget(true); // when we mark this then the serialization of TEs will simpley skip this one, easy, no
    }
  }
  return StatusCode::SUCCESS;
  
}

StatusCode
HLT::TrigNavigationSlimmingTool::removeFeaturelessTriggerElements(State& state,
                                                                  TriggerElement *) { 
  ATH_MSG_DEBUG ( "Running the adjustIndicesAfterThinning" );

  for ( auto te: state.navigation.getAllTEs() ) {
    size_t featuresCount = std::count_if(te->getFeatureAccessHelpers().begin(), 
					 te->getFeatureAccessHelpers().end(), 
					 [](const TriggerElement::FeatureAccessHelper& fea){ return fea.forget(); }
					 );
    if( featuresCount == 0 ) {
      CHECK( removeTriggerElement( state, te, false ) );      
    } 
  }
  return StatusCode::SUCCESS;
}

/**********************************************************************
 *
 * Generic Helper Functions
 *
 **********************************************************************/
StatusCode HLT::TrigNavigationSlimmingTool::removeTriggerElementFromVector(TriggerElement *te, std::vector<TriggerElement*>& v) const {
    
  if( v.empty())
    return StatusCode::SUCCESS;
  
  std::vector<TriggerElement*>::iterator newend = std::remove( v.begin(), v.end(), te);
  v.erase(newend, v.end());
  
  return StatusCode::SUCCESS;
}

bool HLT::TrigNavigationSlimmingTool::toBeIncluded(State& state,
                                                   TriggerElement *te, 
						   std::vector<std::string> *inclusionList, std::vector<std::string> *exclusionList) {

  // if the inclusion list exists, then we use that
  if(inclusionList && inclusionList->size() > 0) {

    // check if the trigger element has a feature that is included in the list
    int onExclusionList = 0;
    for(std::vector< TriggerElement::FeatureAccessHelper >::const_iterator iter =
	  te->getFeatureAccessHelpers().begin();
        iter != te->getFeatureAccessHelpers().end(); ++iter) {
      // grab the label and check if it was found in the inclusion list
      if(std::find(inclusionList->begin(), inclusionList->end(), this->getLabel(state.navigation, *iter )) != inclusionList->end())
        return true;
      // now find if its in the exclusion list
      if(exclusionList && std::find(exclusionList->begin(), exclusionList->end(), 
				    this->getLabel( state.navigation, *iter )) != exclusionList->end())
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
    if(std::find(exclusionList->begin(), exclusionList->end(), this->getLabel(state.navigation, *iter )) != exclusionList->end())
      return false;
  }

  // if we've made it here, its passed the exclusion list and so should be included
  return true;

}

bool HLT::TrigNavigationSlimmingTool::toBeIncluded(State& state,
                                                   TriggerElement *te, 
						   std::vector<TriggerElement*> *inclusionList, 
						   std::vector<TriggerElement*> *exclusionList) {

  if(!te)
    return false;

  // this function never allows inital nodes or RoI nodes to be removed
  if( state.navigation.isInitialNode(te) || state.navigation.isRoINode(te) )
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

StatusCode
HLT::TrigNavigationSlimmingTool::propagateFeaturesToChildren(const TriggerElement *te) const {

  if ( not te )
    return StatusCode::SUCCESS;

  const std::vector<TriggerElement::FeatureAccessHelper>& features = te->getFeatureAccessHelpers() ;

  std::vector<TriggerElement*> children = te->m_relations[TriggerElement::seedsRelation];
  for( auto ch: children ) {
    if ( m_report ) ATH_MSG_VERBOSE("Propagating features to child: " << ch << " " << ch->getId() );
    // add the parents features to the front of the child features list features list
    // skip this FEAs which are anyway to be discarded
    for( auto& fea: features ) {
      if ( fea.forget() ) 
	continue;
      if ( m_report ){
	std::stringstream ss;
	ss << fea;
	ATH_MSG_VERBOSE("Propagating feature " << ss.str() );
      }
      ch->getFeatureAccessHelpers().insert(ch->getFeatureAccessHelpers().begin(), fea );
    }     
    //    te->getFeatureAccessHelpers().insert((*iter)->getFeatureAccessHelpers().begin(), features.begin(), features.end());
  }
  return StatusCode::SUCCESS;
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

StatusCode HLT::TrigNavigationSlimmingTool::syncThinning(State& state) const {
  ATH_MSG_DEBUG ( "Running the syncThinning" );
  auto holders = state.navigation.m_holderstorage.getAllHolders<HLTNavDetails::IHolder>();
  for(auto holder : holders) {
    if ( not evtStore()->contains(holder->containerClid(), holder->label() ) ) {
      ATH_MSG_DEBUG("Skipping feature missing in the store: " << holder->label());
      continue;
    }
      holder->syncWithSG();
      auto containerPointer = holder->containerTypeProxy().cptr();
      if ( m_thinningSvc->thinningOccurred(containerPointer)  )  {
	ATH_MSG_DEBUG ( "Thinning occured for this container" << *holder <<", going to ajust the indices" );
	// ThinningSvc::index method returns the valid new index for unslimmed object and an invalid index IThinningSvc::RemovedIndex for the ones that were removed
	// The way to calulate new indexes for the ranges describing ROIs (X,Y) is to count the number valid indexes from X to 0 and from Y to 0
	// This would be quite inefficient so we need to make a vector wiht new indexes
	// this vector in the end will be of size equal to the original collection and at position X will have new values.
	// Hoever the difficulty is that we are not able to obtain the size of the collection and we need to build this vector as we meet the indexs during 
	// the scan of TEs. 
	// Since this is quite tricky code it is outsourced to a helper class IndexRecalculator.
	
	IndexRecalculator recalculator( &*m_thinningSvc, containerPointer);
	// nowe we need to go over the TEs
	for ( const auto& te: state.navigation.getAllTEs() ) {
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
  } // holder loop
  return StatusCode::SUCCESS;
}


