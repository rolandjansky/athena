/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"
#include "AthContainers/AuxElement.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/GaudiException.h"

#include "StoreGate/WriteHandle.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"

#include <unordered_map>
#include <regex>

static const SG::AuxElement::Accessor< std::vector<TrigCompositeUtils::DecisionID> > readWriteAccessor("decisions");
static const SG::AuxElement::ConstAccessor< std::vector<TrigCompositeUtils::DecisionID> > readOnlyAccessor("decisions");

namespace TrigCompositeUtils {  

  SG::WriteHandle<DecisionContainer> createAndStore( const SG::WriteHandleKey<DecisionContainer>& key, const EventContext& ctx ) {
    SG::WriteHandle<DecisionContainer> handle( key, ctx );
    auto data = std::make_unique<DecisionContainer>() ;
    auto aux = std::make_unique<DecisionAuxContainer>() ;
    data->setStore( aux.get() );
    handle.record( std::move( data ), std::move( aux )  ).ignore();
    return handle;
  }

  void createAndStore( SG::WriteHandle<DecisionContainer>& handle ) {
    auto data = std::make_unique<DecisionContainer>() ;
    auto aux = std::make_unique<DecisionAuxContainer>() ;
    data->setStore( aux.get() );
    handle.record( std::move( data ), std::move( aux )  ).ignore();
  }

  Decision* newDecisionIn ( DecisionContainer* dc, const std::string& name) {
    Decision * x = new Decision;
    dc->push_back( x );
    if ( ! name.empty() ) {
      x->setName( name );
    }
    return x;
  }

  Decision* newDecisionIn ( DecisionContainer* dc, const Decision* dOld, const std::string& name, const EventContext& ctx ) {
    Decision* dNew =  newDecisionIn( dc, name );
    linkToPrevious(dNew, dOld, ctx); // Sets up link to 'seed' collection, points to dOld
    return dNew;
  }

  void addDecisionID( DecisionID id,  Decision* d ) {   
    std::vector<DecisionID>& decisions = readWriteAccessor( *d );
    if ( decisions.size() == 0 or decisions.back() != id) 
      decisions.push_back( id );
  }
  
  void decisionIDs( const Decision* d, DecisionIDContainer& destination ) {    
    const std::vector<DecisionID>& decisions = readOnlyAccessor( *d );    
    destination.insert( decisions.begin(), decisions.end() );
  }

  const std::vector<DecisionID>& decisionIDs( const Decision* d ) {    
    return readOnlyAccessor( *d );    
  }

  std::vector<DecisionID>& decisionIDs( Decision* d ) {
    return readWriteAccessor( *d );
  }

  void insertDecisionIDs(const Decision* src, Decision* dest ){
    DecisionIDContainer srcIds;
    decisionIDs( src, srcIds ); // Now stored in a set
    insertDecisionIDs( srcIds, dest);
  }

  void insertDecisionIDs( const DecisionIDContainer& src, Decision* dest ) {
    DecisionIDContainer collateIDs;
    // Decision are xAOD objects backed by a std::vector
    // Here we use a std::set to de-duplicate IDs from src and dest before setting dest
    decisionIDs( dest, collateIDs ); // Set operation 1. Get from dest
    collateIDs.insert( src.begin(), src.end() ); // Set operation 2. Get from src
    decisionIDs( dest ).clear(); // Clear target
    // Copy from set to (ordered) vector
    decisionIDs( dest ).insert( decisionIDs(dest).end(), collateIDs.begin(), collateIDs.end() );
  }

  void uniqueDecisionIDs(Decision* dest) {
    // Re-use above insertDecisionIDs method.
    // This implicitly performs de-duplication
    return insertDecisionIDs(dest, dest);
  }

  bool allFailed( const Decision* d ) {
    const std::vector<DecisionID>& decisions = readOnlyAccessor( *d );    
    return decisions.empty();
  }

  bool isAnyIDPassing( const Decision* d,  const DecisionIDContainer& required ) {
    for ( DecisionID id : readOnlyAccessor( *d ) ) {
      if ( required.count( id ) > 0 ) {
        return true;
      }
    }
    return false;
  }    

  bool passed( DecisionID id, const DecisionIDContainer& idSet ) {
    return idSet.count( id ) != 0;
  }

  ElementLink<DecisionContainer> decisionToElementLink(const Decision* d, const EventContext& ctx) {
    const DecisionContainer* container = dynamic_cast<const DecisionContainer*>( d->container() );
    if( ! container ) {
      throw GaudiException("Using convertToElementLink(d) requires that the Decision d is already in a container",
        "TrigCompositeUtils::convertToElementLink", StatusCode::FAILURE);
    }
    return ElementLink<DecisionContainer>(*container, d->index(), ctx);
  }

  void linkToPrevious( Decision* d, const std::string& previousCollectionKey, size_t previousIndex ) {
    ElementLink<DecisionContainer> seed = ElementLink<DecisionContainer>( previousCollectionKey, previousIndex );
    if (!seed.isValid()) {
      throw GaudiException("Invalid Decision Link key or index provided", "TrigCompositeUtils::linkToPrevious", StatusCode::FAILURE);
    } else {
      d->addObjectCollectionLink("seed", seed);
    }
  }

  void linkToPrevious( Decision* d, const Decision* dOld, const EventContext& ctx ) {
    d->addObjectCollectionLink(seedString(), decisionToElementLink(dOld, ctx));
  }

  bool hasLinkToPrevious( const Decision* d ) {
    return d->hasObjectCollectionLinks( seedString() );
  }

  const ElementLinkVector<DecisionContainer> getLinkToPrevious( const Decision* d ) {
    return d->objectCollectionLinks<DecisionContainer>( seedString() );
  }


  bool copyLinks(const Decision* src, Decision* dest) {
    return dest->copyAllLinksFrom(src);
  }


  HLT::Identifier createLegName(const HLT::Identifier& chainIdentifier, size_t counter) {
    if (chainIdentifier.name().substr(0,4) != "HLT_") {
      throw GaudiException("chainIdentifier '"+chainIdentifier.name()+"' does not start 'HLT_'",
        "TrigCompositeUtils::createLegName", StatusCode::FAILURE);
    }
    if (counter > 999) {
      throw GaudiException("Leg counters above 999 are invalid.", "TrigCompositeUtils::createLegName", StatusCode::FAILURE);
    }
    std::stringstream legStringStream;
    legStringStream << "leg" << std::setfill('0') << std::setw(3) << counter << "_" << chainIdentifier.name();
    return HLT::Identifier( legStringStream.str() );
  }

  
  HLT::Identifier getIDFromLeg(const HLT::Identifier& legIdentifier) {
    if (legIdentifier.name().find("HLT_",0)==0 ){
      return legIdentifier;
    } else if (isLegId(legIdentifier)){
      return HLT::Identifier(legIdentifier.name().substr(7));
    } else{
      throw GaudiException("legIdentifier '"+legIdentifier.name()+"' does not start with 'HLT_' or 'leg' ",
        "TrigCompositeUtils::getIDFromLeg", StatusCode::FAILURE);
    }
  }

  
  bool isLegId(const HLT::Identifier& legIdentifier) {
    return (legIdentifier.name().find("leg",0)==0);
  }
  
  
  const Decision* find( const Decision* start, const std::function<bool( const Decision* )>& filter ) {
    if ( filter( start ) ) return start;

    if ( hasLinkToPrevious(start) ) {
      const ElementLinkVector<DecisionContainer> seeds = getLinkToPrevious(start);
      for (const ElementLink<DecisionContainer>& seedEL : seeds) {
        const Decision* result = find( *seedEL, filter );
        if (result) return result;
      }
    }
    
    return nullptr;
  }

  bool HasObject::operator()( const Decision* composite ) const {
    return composite->hasObjectLink( m_name );
  }

  bool HasObjectCollection::operator()( const Decision* composite ) const {
    return composite->hasObjectCollectionLinks( m_name );
  }

 std::vector<const Decision*> getRejectedDecisionNodes(StoreGateSvc* eventStore, const DecisionID id) {
    std::vector<const Decision*> output;
    // The list of containers we need to read can change on a file-by-file basis (it depends on the SMK)
    // Hence we query SG for all collections rather than maintain a large and ever changing ReadHandleKeyArray

    std::vector<std::string> keys;
    eventStore->keys(static_cast<CLID>( ClassID_traits< DecisionContainer >::ID() ), keys);

    // Loop over each DecisionContainer,
    for (const std::string& key : keys) {
      // Get and check this container
      if ( key.find("HLTNav") != 0 ) {
        continue; // Only concerned about the decision containers which make up the navigation, they have name prefix of HLTNav
      }
      if ( key == "HLTNav_Summary" ) {
        continue; //  This is where accepted paths start. We are looking for rejected ones
      }
      const DecisionContainer* container = nullptr;
      if ( eventStore->retrieve( container, key ).isFailure() ) {
        throw std::runtime_error("Unable to retrieve " + key + " from event store.");
      }
      for (const Decision* d : *container) {
        if (!d->hasObjectLink(featureString())) {
          // TODO add logic for ComboHypo where this is expected
          continue; // Only want Decision objects created by HypoAlgs
        }
        const ElementLinkVector<DecisionContainer> mySeeds = d->objectCollectionLinks<DecisionContainer>(seedString());
        if (mySeeds.size() == 0) {
          continue;
        }
        const bool allSeedsValid = std::all_of(mySeeds.begin(), mySeeds.end(), [](const ElementLink<DecisionContainer>& s) { return s.isValid(); });
        if (!allSeedsValid) {
          MsgStream(Athena::getMessageSvc(), "TrigCompositeUtils::getRejectedDecisionNodes") << MSG::WARNING
            << "A Decision object in " << key << " has invalid seeds. "
            << "The trigger navigation information is incomplete. Skipping this Decision object." << endmsg;
          continue;
        }
        DecisionIDContainer activeChainsIntoThisDecision;
        decisionIDs(*(mySeeds.at(0)), activeChainsIntoThisDecision); // Get list of active chains from the first parent
        if (mySeeds.size() > 1) {
          for (size_t i = 1; i < mySeeds.size(); ++i) {
            // If there are more than one parent, we only want to keep the intersection of all of the seeds
            DecisionIDContainer moreActiveChains;
            decisionIDs(*(mySeeds.at(i)), moreActiveChains);
            DecisionIDContainer intersection;
            std::set_intersection(activeChainsIntoThisDecision.begin(), activeChainsIntoThisDecision.end(),
              moreActiveChains.begin(), moreActiveChains.end(),
              std::inserter(intersection, intersection.begin()));
            activeChainsIntoThisDecision = intersection; // Update the output to only be the intersection and continue to any other seeds
          }
        }
        // We now know what chains were active coming into this Decision (d) from ALL seeds
        // This is the logic required for each HypoTool to have activated and checked if its chain passes
        // So the size of activeChainsIntoThisDecision corresponds to the number of HypoTools which will have run
        // What do we care about? A chain, or all chains?
        DecisionIDContainer chainsToCheck;
        if (id == 0) { // We care about *all* chains
          chainsToCheck = activeChainsIntoThisDecision;
        } else { // We care about *one* chain
          chainsToCheck.insert(id);
        }
        // We have found a rejected decision node *iff* a chainID to check is *not* present here
        // I.e. the HypoTool for the chain returned a NEGATIVE decision
        DecisionIDContainer activeChainsPassedByThisDecision;
        decisionIDs(d, activeChainsPassedByThisDecision);
        for (const DecisionID checkID : chainsToCheck) {
          if (activeChainsPassedByThisDecision.count(checkID) == 0 && // I was REJECTED here ...
              activeChainsIntoThisDecision.count(checkID) == 1) { // ... but PASSSED by all my inputs
            output.push_back(d);
            break;
          }
        }
      }
    }
    return output;
  }

  void recursiveGetDecisionsInternal(const Decision* node, 
    const Decision* comingFrom, 
    NavGraph& navGraph, 
    const DecisionID id,
    const bool enforceDecisionOnNode) {

    // Does this Decision satisfy the chain requirement?
    DecisionIDContainer idSet = {id};
    if (enforceDecisionOnNode && id != 0 && !isAnyIDPassing(node, idSet)) {
      return; // Stop propagating down this leg. It does not concern the chain with DecisionID = id
    }

    // This Decision object is part of this path through the Navigation
    navGraph.addNode(node, comingFrom);
    
    // Continue to the path(s) by looking at this Decision object's seed(s)
    if ( hasLinkToPrevious(node) ) {
      // Do the recursion
      for ( ElementLink<DecisionContainer> seed : getLinkToPrevious(node)) {
        const Decision* seedDecision = *(seed); // Dereference ElementLink
        // Sending true as final parameter for enforceDecisionOnStartNode as we are recursing away from the supplied start node
        recursiveGetDecisionsInternal(seedDecision, node, navGraph, id, /*enforceDecisionOnNode*/ true);
      }
    }
    return;
  }

  void recursiveGetDecisions(const Decision* start, 
    NavGraph& navGraph, 
    const DecisionID id,
    const bool enforceDecisionOnStartNode) {

    // Note: we do not require navGraph to be an empty graph. We can extend it.
    recursiveGetDecisionsInternal(start, /*comingFrom*/nullptr, navGraph, id, enforceDecisionOnStartNode);
    
    return;
  }


  std::string dump( const Decision* tc, std::function< std::string( const Decision* )> printerFnc ) {
    std::string ret; 
    ret += printerFnc( tc );
    if ( hasLinkToPrevious(tc) ) {
      const ElementLinkVector<DecisionContainer> seeds = getLinkToPrevious(tc);
      for (const ElementLink<DecisionContainer>& seedEL : seeds) {
        ret += " -> " + dump( *seedEL, printerFnc );
      }
    }
    return ret;
  }

  
  const std::string& initialRoIString() {
    return Decision::s_initialRoIString;
  }

  const std::string& initialRecRoIString() {
    return Decision::s_initialRecRoIString;
  }

  const std::string& roiString() {
    return Decision::s_roiString;
  }

  const std::string& viewString() {
    return Decision::s_viewString;
  }

  const std::string& featureString() {
    return Decision::s_featureString;
  }

  const std::string& seedString() {
    return Decision::s_seedString;
  }
  
}

