/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"
#include "AthContainers/AuxElement.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/GaudiException.h"

#include "StoreGate/WriteHandle.h"
#include "DecisionHandling/TrigCompositeUtils.h"

#include <unordered_map>

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
    decisionIDs( dest ).insert( decisionIDs(dest).end(), src.begin(), src.end() );
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

  void linkToPrevious( Decision* d, const std::string& previousCollectionKey, size_t previousIndex ) {
    ElementLink<DecisionContainer> seed = ElementLink<DecisionContainer>( previousCollectionKey, previousIndex );
    if (!seed.isValid()) {
      throw GaudiException("Invalid Decision Link key or index provided", "TrigCompositeUtils::linkToPrevious", StatusCode::FAILURE);
    } else {
      d->addObjectCollectionLink("seed", seed);
    }
  }

  void linkToPrevious( Decision* d, const Decision* dOld, const EventContext& ctx ) {

    const DecisionContainer* container = dynamic_cast<const DecisionContainer*>( dOld->container() );
    if( ! container ) {
      throw GaudiException("Using linkToPrevious with a previous decision requires that dOld is already in a container",
        "TrigCompositeUtils::linkToPrevious", StatusCode::FAILURE);
    }    

    const ElementLink<DecisionContainer> seedLink = ElementLink<DecisionContainer>(*container, dOld->index(), ctx);
    d->addObjectCollectionLink(seedString(), seedLink);
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
      const DecisionContainer* container = nullptr;
      if ( eventStore->retrieve( container, key ).isFailure() ) {
        throw std::runtime_error("Unable to retrieve " + key + " from event store.");
      }
      for (const Decision* d : *container) {
        if (!d->hasObjectLink(featureString())) {
          // TODO add logic for ComboHypo where this is expected
          continue;
        }
        const ElementLinkVector<DecisionContainer> mySeeds = d->objectCollectionLinks<DecisionContainer>(seedString());
        if (mySeeds.size() == 0) {
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
        for (const DecisionID id : chainsToCheck) {
          if (activeChainsPassedByThisDecision.count(id) == 0) { // I was REJECTED
            output.push_back(d);
            break;
          }
        }
      }
    }
    return output;
  }

  void recursiveGetDecisionsInternal(const Decision* start, 
    const size_t location, 
    std::vector<ElementLinkVector<DecisionContainer>>& linkVector, 
    const DecisionID id,
    const bool enforceDecisionOnNode) {

    // Does this Decision satisfy the chain requirement?
    DecisionIDContainer idSet = {id};
    if (enforceDecisionOnNode && id != 0 && !isAnyIDPassing(start, idSet)) {
      return; // Stop propagating down this leg. It does not concern the chain with DecisionID = id
    }

    // This Decision object is part of this linear path through the Navigation
    const DecisionContainer* container = dynamic_cast<const DecisionContainer*>( start->container() );
    ElementLink<DecisionContainer> startLink = ElementLink<DecisionContainer>(*container, start->index());
    linkVector.at(location).push_back( startLink ); 
    
    // Continue to the path(s) by looking at this Decision object's seed(s)
    if ( hasLinkToPrevious(start) ) {
      const ElementLinkVector<DecisionContainer> seedsVector = getLinkToPrevious(start);

      // If there is more than one seed then we need to fork.
      // Each fork implies copying the (linear) vector of links up to this point.
      // As this forking may have happened more than once before, we need to remember which seed 
      // corresponds to which fork. Done here via a map.
      std::unordered_map<size_t,size_t> mapSeedToLinkVector;
      for (size_t seed = 0; seed < seedsVector.size(); ++seed) {
        if (seed == 0) {
          mapSeedToLinkVector.insert( std::make_pair(seed, location) );
        } else {
          linkVector.push_back( linkVector.at(location) );
          mapSeedToLinkVector.insert( std::make_pair(seed, linkVector.size() - 1) );
        }
      }

      // Do the recursion
      for (size_t seed = 0; seed < seedsVector.size(); ++seed) {
        const Decision* seedDecision = *(seedsVector.at(seed)); // Dereference ElementLink
        size_t linkVectorLocation = mapSeedToLinkVector.find(seed)->second;
        // Sending true as final parameter for enforceDecisionOnStartNode as we are recursing away from the supplied start node
        recursiveGetDecisionsInternal(seedDecision, linkVectorLocation, linkVector, id, true);
      }
    }
    return;
  }

  void recursiveGetDecisions(const Decision* start, 
    std::vector<ElementLinkVector<DecisionContainer>>& linkVector, 
    const DecisionID id,
    const bool enforceDecisionOnStartNode) {

    // Note: we do not require linkVector to be an empty vector. We can append to it.
    linkVector.push_back( ElementLinkVector<DecisionContainer>() ); // Our starting point
    const size_t startingElement = linkVector.size() - 1;
    recursiveGetDecisionsInternal(start, startingElement, linkVector, id, enforceDecisionOnStartNode);
    // Writing finished.
    // Now - remove defunct branches. These are zero length entries or entries which did not propagate all the way up to the root nodes
    // (this occurs when the decision ID was not valid along a prospective branch)
    std::vector<ElementLinkVector<DecisionContainer>>::iterator vecIt = linkVector.begin();
    std::advance(vecIt, startingElement);
    // vecIt is an iterator which now corresponds to the first element added by this call to recursiveGetDecisions
    for (; vecIt != linkVector.end();) {
      bool shouldRemove = false;
      if (vecIt->size() == 0) {
        // No Decision ELs were added to the inner ElementLinkVector.
        shouldRemove = true;
      } else {
        ElementLink<DecisionContainer> finalDecision = vecIt->back();
        DecisionIDContainer idSet = {id};
        if ((*finalDecision)->name() != "L1" || hasLinkToPrevious(*finalDecision)) {
          // If the back Decision is not L1, then it's not from the top of the graph. And the path should be removed.
          // TODO hasLinkToPrevious check should be redundant soon - remove in future
          shouldRemove = true;
        } else if (id != 0 && !isAnyIDPassing(*finalDecision, idSet)) {
          // The final hop to this L1 node was not valid for the chain in question
          shouldRemove = true;
        }
      } 
      if (shouldRemove) {
        vecIt = linkVector.erase(vecIt);
      } else {
        ++vecIt;
      }
    }
    return;
  }


  std::string dump( const Decision*  tc, std::function< std::string( const Decision* )> printerFnc ) {
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

  const std::string& intitalRecRoIString() {
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

