/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// See similar workaround the lack of CLID in standalone releases in TrigComposite_v1.h
#include "xAODBase/IParticleContainer.h"

#include "AsgDataHandles/WriteHandle.h"
#include "AsgDataHandles/ReadHandle.h"
#include "AthContainers/AuxElement.h"

#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "boost/algorithm/string/predicate.hpp"

#include <unordered_map>
#include <regex>
#include <iomanip> // std::setfill
#include <mutex>

static const SG::AuxElement::Accessor< std::vector<TrigCompositeUtils::DecisionID> > readWriteAccessor("decisions");
static const SG::AuxElement::ConstAccessor< std::vector<TrigCompositeUtils::DecisionID> > readOnlyAccessor("decisions");



namespace TrigCompositeUtils {  

  ANA_MSG_SOURCE (msgFindLink, "TrigCompositeUtils.findLink")
  ANA_MSG_SOURCE (msgRejected, "TrigCompositeUtils.getRejectedDecisionNodes")


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

#if !defined(XAOD_STANDALONE) && !defined(XAOD_ANALYSIS) // Full athena
  ElementLink<DecisionContainer> decisionToElementLink(const Decision* d, const EventContext& ctx) {
    const DecisionContainer* container = dynamic_cast<const DecisionContainer*>( d->container() );
    if( ! container ) {
      throw std::runtime_error("TrigCompositeUtils::convertToElementLink Using convertToElementLink(d) requires that the Decision d is already in a container");
    }
    return ElementLink<DecisionContainer>(*container, d->index(), ctx);
  }
#else // Analysis or Standalone
  ElementLink<DecisionContainer> decisionToElementLink(const Decision* d, const EventContext&) {
    const DecisionContainer* container = dynamic_cast<const DecisionContainer*>( d->container() );
    if( ! container ) {
      throw std::runtime_error("TrigCompositeUtils::convertToElementLink Using convertToElementLink(d) requires that the Decision d is already in a container");
    }
    return ElementLink<DecisionContainer>(*container, d->index());
  }
#endif

  void linkToPrevious( Decision* d, const std::string& previousCollectionKey, size_t previousIndex  ) {
    ElementLink<DecisionContainer> seed = ElementLink<DecisionContainer>( previousCollectionKey, previousIndex );
    if (!seed.isValid()) {
      throw std::runtime_error("TrigCompositeUtils::linkToPrevious Invalid Decision Link key or index provided");
    } else {
      d->addObjectCollectionLink(seedString(), seed);
    }
  }

  void linkToPrevious( Decision* d, const Decision* dOld, const EventContext& ctx ) {
    d->addObjectCollectionLink(seedString(), decisionToElementLink(dOld, ctx));
  }

  bool hasLinkToPrevious( const Decision* d ) {
    return d->hasObjectCollectionLinks( seedString() );
  }

  const std::vector<ElementLink<DecisionContainer>> getLinkToPrevious( const Decision* d ) {
    return d->objectCollectionLinks<DecisionContainer>( seedString() );
  }


  bool copyLinks(const Decision* src, Decision* dest) {
    return dest->copyAllLinksFrom(src);
  }


  HLT::Identifier createLegName(const HLT::Identifier& chainIdentifier, size_t counter) {
    if (!isChainId(chainIdentifier)) {
      throw std::runtime_error("TrigCompositeUtils::createLegName chainIdentifier '"+chainIdentifier.name()+"' does not start 'HLT_'");
    }
    if (counter > 999) {
      throw std::runtime_error("TrigCompositeUtils::createLegName Leg counters above 999 are invalid.");
    }
    std::stringstream legStringStream;
    legStringStream << "leg" << std::setfill('0') << std::setw(3) << counter << "_" << chainIdentifier.name();
    return HLT::Identifier( legStringStream.str() );
  }

  
  HLT::Identifier getIDFromLeg(const HLT::Identifier& legIdentifier) {
    if (isChainId(legIdentifier)){
      return legIdentifier;
    } else if (isLegId(legIdentifier)){
      return HLT::Identifier(legIdentifier.name().substr(7));
    } else{
      throw std::runtime_error("TrigCompositeUtils::getIDFromLeg legIdentifier '"+legIdentifier.name()+"' does not start with 'HLT_' or 'leg' ");
    }
  }

  int32_t getIndexFromLeg(const HLT::Identifier& legIdentifier) {
    if (isChainId(legIdentifier)){
      return 0;
    } else if (!isLegId(legIdentifier)) {
      return -1;
    }
    return std::stoi( legIdentifier.name().substr(3,3) ); 
  }

  
  bool isLegId(const HLT::Identifier& legIdentifier) {
    return (legIdentifier.name().substr(0,3) == "leg");
  }

  bool isChainId(const HLT::Identifier& chainIdentifier) {
    return (chainIdentifier.name().substr(0,4) == "HLT_");
  }
  
  
  const Decision* find( const Decision* start, const std::function<bool( const Decision* )>& filter ) {
    if ( filter( start ) ) return start;

    if ( hasLinkToPrevious(start) ) {
      const std::vector<ElementLink<DecisionContainer>> seeds = getLinkToPrevious(start);
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

  const Decision* getTerminusNode(SG::ReadHandle<DecisionContainer>& container) {
    for (const Decision* decision : *container) {
      if (decision->name() == summaryPassNodeName()) {
        return decision;
      }
    }
    return nullptr;
  }

  std::vector<const Decision*> getRejectedDecisionNodes(const asg::EventStoreType* eventStore,
    const std::string& summaryCollectionKey,
    const DecisionIDContainer& ids,
    const std::set<std::string>& keysToIgnore) {

    // The following list contains all known summary store identifiers where the graph nodes are spread out over O(100s) or O(1000s)
    // of different SG collections. This is the raw output from running the trigger online.
    // When dealing with this, we need to query eventStore->keys in every event to obtain the full set of collections to process.
    static const std::vector<std::string> knownDistributedSummaryStores{"HLTNav_Summary"};

    // The following list contains all known summary store identifiers where all nodes from the graph have been compactified / condensed
    // down into a single container. Here we just have to search this one container.
    static const std::vector<std::string> knownCompactSummaryStores{"HLTNav_Summary_OnlineSlimmed",
      "HLTNav_Summary_ESDSlimmed",
      "HLTNav_Summary_AODSlimmed",
      "HLTNav_Summary_DAODSlimmed"};

    std::vector<std::string> keys; // The SG keys we will be exploring to find rejected decision nodes

    if (std::find(knownDistributedSummaryStores.cbegin(), knownDistributedSummaryStores.cend(), summaryCollectionKey) != knownDistributedSummaryStores.end() or summaryCollectionKey == "") {
      
      // If we have a distributed store then we need to query SG to find all keys.
      // This should be a rare case now that we run compactification "online" (i.e. immediately after the trigger has executed) 
#ifndef XAOD_STANDALONE
      // The list of containers we need to read can change on a file-by-file basis (it depends on the SMK)
      // Hence we query SG for all collections rather than maintain a large and ever changing ReadHandleKeyArray
      eventStore->keys(static_cast<CLID>( ClassID_traits< DecisionContainer >::ID() ), keys);
#else
      eventStore->event(); // Avoid unused warning
      throw std::runtime_error("Cannot obtain rejected HLT features in AnalysisBase when reading from uncompactified navigation containers, run trigger navigation slimming first if you really need this.");
#endif

    } else if (std::find(knownCompactSummaryStores.cbegin(), knownCompactSummaryStores.cend(), summaryCollectionKey) != knownCompactSummaryStores.end()) {

      keys.push_back(summaryCollectionKey);

    } else {

      using namespace msgRejected;
      ANA_MSG_WARNING("getRejectedDecisionNodes has not been told about final collection " << summaryCollectionKey << " please update this function. Assuming that it is already compact.");
      // Safest to assume that this is a compact summary store
      keys.push_back(summaryCollectionKey);

    }

    std::vector<const Decision*> output; // The return vector of identified nodes where one of the chains in 'ids' was rejected

    // Loop over each DecisionContainer,
    for (const std::string& key : keys) {
      // Get and check this container
      if ( ! boost::starts_with (key, "HLTNav_") ) {
        continue; // Only concerned about the decision containers which make up the navigation, they have name prefix of HLTNav
      }
      if (keysToIgnore.count(key) == 1) {
        continue; // Have been asked to not explore this SG container
      }
      SG::ReadHandle<DecisionContainer> containerRH(key);
      if (!containerRH.isValid()) {
        throw std::runtime_error("Unable to retrieve " + key + " from event store.");
      }
      for (const Decision* d : *containerRH) {
        if (!d->hasObjectLink(featureString())) {
          // TODO add logic for ComboHypo where this is expected
          continue; // Only want Decision objects created by HypoAlgs
        }
        const std::vector<ElementLink<DecisionContainer>> mySeeds = d->objectCollectionLinks<DecisionContainer>(seedString());
        if (mySeeds.size() == 0) {
          continue;
        }
        const bool allSeedsValid = std::all_of(mySeeds.begin(), mySeeds.end(), [](const ElementLink<DecisionContainer>& s) { return s.isValid(); });
        if (!allSeedsValid) {
          using namespace msgRejected;
          ANA_MSG_WARNING("A Decision object in " << key << " has invalid seeds. "
            << "The trigger navigation information is incomplete. Skipping this Decision object.");
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
        if (ids.size() == 0) { // We care about *all* chains
          chainsToCheck = activeChainsIntoThisDecision;
        } else { // We care about specified chains
          chainsToCheck = ids;
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
    std::set<const Decision*>& fullyExploredFrom,
    const DecisionIDContainer& ids,
    const bool enforceDecisionOnNode) {

    // Does this Decision satisfy the chain requirement?
    if (enforceDecisionOnNode && ids.size() != 0 && !isAnyIDPassing(node, ids)) {
      return; // Stop propagating down this leg. It does not concern the chain with DecisionID = id
    }

    // This Decision object is part of this path through the Navigation
    navGraph.addNode(node, comingFrom);

#if TRIGCOMPUTILS_ENABLE_EARLY_EXIT == 1
    // Note we have to do this check here (after calling addNode) rather than just before calling recursiveGetDecisionsInternal
    if (fullyExploredFrom.count(node) == 1) {
      // We have fully explored this branch
      return;
    }
#endif
    
    // Continue to the path(s) by looking at this Decision object's seed(s)
    if ( hasLinkToPrevious(node) ) {
      // Do the recursion
      for ( const ElementLink<DecisionContainer>& seed : getLinkToPrevious(node)) {
        const Decision* seedDecision = *(seed); // Dereference ElementLink
        // Sending true as final parameter for enforceDecisionOnStartNode as we are recursing away from the supplied start node
        recursiveGetDecisionsInternal(seedDecision, node, navGraph, fullyExploredFrom, ids, /*enforceDecisionOnNode*/ true);
      }
    }

    // Have fully explored down from this point
    fullyExploredFrom.insert(node);

    return;
  }

  void recursiveGetDecisions(const Decision* start, 
    NavGraph& navGraph, 
    const DecisionIDContainer& ids,
    const bool enforceDecisionOnStartNode) {

    std::set<const Decision*> fullyExploredFrom;
    // Note: we do not require navGraph to be an empty graph. We can extend it.
    recursiveGetDecisionsInternal(start, /*comingFrom*/nullptr, navGraph, fullyExploredFrom, ids, enforceDecisionOnStartNode);
    
    return;
  }


  void recursiveFlagForThinning(NavGraph& graph, 
    const bool keepOnlyFinalFeatures,
    const std::vector<std::string>& nodesToDrop)
  {
    std::set<NavGraphNode*> fullyExploredFrom;
    for (NavGraphNode* finalNode : graph.finalNodes()) {
      recursiveFlagForThinningInternal(finalNode, /*modeKeep*/true, fullyExploredFrom, keepOnlyFinalFeatures, nodesToDrop);
    }
  }


  void recursiveFlagForThinningInternal(NavGraphNode* node,
    bool modeKeep,
    std::set<NavGraphNode*>& fullyExploredFrom,
    const bool keepOnlyFinalFeatures,
    const std::vector<std::string>& nodesToDrop)
  {

    // If modeKeep == true, then by default we are KEEPING the nodes as we walk the navigation,
    // otherwise by default we are THINNING the nodes
    bool keep = modeKeep;

    // The calls to node->node() here are going from the transient NavGraphNode 
    // to the underlying const Decision* from the input collection

    if (keepOnlyFinalFeatures) {
      // Check if we have reached the first feature
      if ( modeKeep == true && node->node()->hasObjectLink(featureString()) ) {
        // We keep this node, and its immidiate parents (InputMaker nodes) as these have the ROI link which we also want
        keep = true;
        // TODO - these calls mean we bypass the nodesToDrop check here, any other way?
        // TODO One solution here is to impliment a link-move option, move the ROI
        // TODO from the InputMaker node down to the HypoAlg node
        for (NavGraphNode* seed : node->seeds()) {
          seed->keep();
        }
        // We change the default behaviour to be modeKeep = false
        // such that by default we start to NOT flag all the parent nodes to be kept
        modeKeep = false;
      }
      if (node->node()->name() == hltSeedingNodeName()) {
        // We also keep the initial node from the HLTSeeding
        keep = true;
      }
    }

    // Check also against NodesToDrop
    for (const std::string& toDrop : nodesToDrop) {
      if (node->node()->name() == toDrop) {
        keep = false;
        break;
      }
    }

    // Inform the node that it should NOT be thinned away.
    if (keep) {
      node->keep();
    }

    for (NavGraphNode* seed : node->seeds()) {
#if TRIGCOMPUTILS_ENABLE_EARLY_EXIT == 1
      if (fullyExploredFrom.count(seed) == 1) {
        // We have fully explored this branch
        continue;
      }
#endif
      // Recursivly call all the way up the graph to the initial nodes from the HLTSeeding
      recursiveFlagForThinningInternal(seed, modeKeep, fullyExploredFrom, keepOnlyFinalFeatures, nodesToDrop);
    }

    // Have fully explored down from this point
    fullyExploredFrom.insert(node);
  }

  // Note: This version of the function recurses through a full navigation graph (initial input: Decision Object)
  bool typelessFindLinks(const Decision* start, 
    const std::string& linkName,
    std::vector<uint32_t>& keyVec, 
    std::vector<uint32_t>& clidVec, 
    std::vector<uint16_t>& indexVec, 
    std::vector<const Decision*>& sourceVec,
    const unsigned int behaviour, 
    std::set<const Decision*>* fullyExploredFrom)
  {
    using namespace msgFindLink;

    // As the append vectors are user-supplied, perform some input validation. 
    if (keyVec.size() != clidVec.size() or clidVec.size() != indexVec.size()) {
      ANA_MSG_WARNING("In typelessFindLinks, keyVec, clidVec, indexVec must all be the same size. Instead have:"
        << keyVec.size() << ", " << clidVec.size()  << ", " << indexVec.size());
      return false;
    }

    // Locate named links. Both collections of links and individual links are supported.
    bool found = typelessFindLinksCommonLinkCollection(start, linkName, keyVec, clidVec, indexVec, sourceVec);
   
    // Early exit
    if (found && behaviour == TrigDefs::lastFeatureOfType) {
      return true;
    }
    // If not Early Exit, then recurse
    for (const ElementLink<DecisionContainer>& seed : getLinkToPrevious(start)) {
#if TRIGCOMPUTILS_ENABLE_EARLY_EXIT == 1
      if (fullyExploredFrom != nullptr) {
        // We only need to recursively explore back from each node in the graph once.
        // We can keep a record of nodes which we have already explored, these we can safely skip over.
        if (fullyExploredFrom->count(*seed) == 1) {
          continue; 
        }
      }
#endif
      found |= typelessFindLinks(*seed, linkName, keyVec, clidVec, indexVec, sourceVec, behaviour, fullyExploredFrom);
    }
    // Fully explored this node
    if (fullyExploredFrom != nullptr) {
      fullyExploredFrom->insert(start);
    }
    return found;
  }

  // Note: This version of the function recurses through a sub-graph of the full navigation graph (initial input: NavGraphNode)
  bool typelessFindLinks(const NavGraphNode* start, 
    const std::string& linkName,
    std::vector<uint32_t>& keyVec, 
    std::vector<uint32_t>& clidVec, 
    std::vector<uint16_t>& indexVec, 
    std::vector<const Decision*>& sourceVec,
    const unsigned int behaviour, 
    std::set<const Decision*>* fullyExploredFrom)
  {
    using namespace msgFindLink;

    // As the append vectors are user-supplied, perform some input validation. 
    if (keyVec.size() != clidVec.size() or clidVec.size() != indexVec.size()) {
      ANA_MSG_WARNING("In typelessFindLinks, keyVec, clidVec, indexVec must all be the same size. Instead have:"
        << keyVec.size() << ", " << clidVec.size()  << ", " << indexVec.size());
      return false;
    }

    const Decision* start_decisionObject = start->node(); 
    // Locate named links. Both collections of links and individual links are supported.
    bool found = typelessFindLinksCommonLinkCollection(start_decisionObject, linkName, keyVec, clidVec, indexVec, sourceVec);

    // Early exit
    if (found && behaviour == TrigDefs::lastFeatureOfType) {
      return true;
    }
    // If not Early Exit, then recurse
    for (const NavGraphNode* seed : start->seeds()) {
#if TRIGCOMPUTILS_ENABLE_EARLY_EXIT == 1
      if (fullyExploredFrom != nullptr) {
        // We only need to recursively explore back from each node in the graph once.
        // We can keep a record of nodes which we have already explored, these we can safely skip over.
        const Decision* seed_decisionObject = seed->node(); 
        if (fullyExploredFrom->count(seed_decisionObject) == 1) {
          continue; 
        }
      }
#endif
      found |= typelessFindLinks(seed, linkName, keyVec, clidVec, indexVec, sourceVec, behaviour, fullyExploredFrom);
    }
    // Fully explored this node
    if (fullyExploredFrom != nullptr) {
      fullyExploredFrom->insert(start_decisionObject);
    }
    return found;
  }


  bool typelessFindLinksCommonLinkCollection(const Decision* start,
    const std::string& linkName,
    std::vector<uint32_t>& keyVec, 
    std::vector<uint32_t>& clidVec,
    std::vector<uint16_t>& indexVec, 
    std::vector<const Decision*>& sourceVec) 
  {
    bool found = false;
    std::vector<uint32_t> tmpKeyVec;
    std::vector<uint32_t> tmpClidVec;
    std::vector<uint16_t> tmpIndexVec;
    if (start->hasObjectCollectionLinks(linkName)) {
      found = start->typelessGetObjectCollectionLinks(linkName, tmpKeyVec, tmpClidVec, tmpIndexVec);
    }
    if (start->hasObjectLink(linkName)) {
      uint32_t tmpKey{0};
      uint32_t tmpClid{0};
      uint16_t tmpIndex{0};
      found |= start->typelessGetObjectLink(linkName, tmpKey, tmpClid, tmpIndex);
      tmpKeyVec.push_back(tmpKey);
      tmpClidVec.push_back(tmpClid);
      tmpIndexVec.push_back(tmpIndex);
    }
    // De-duplicate
    for (size_t tmpi = 0; tmpi < tmpKeyVec.size(); ++tmpi) {
      bool alreadyAdded = false;
      const uint32_t tmpKey = tmpKeyVec.at(tmpi);
      const uint32_t tmpClid = tmpClidVec.at(tmpi);
      const uint16_t tmpIndex = tmpIndexVec.at(tmpi);
      for (size_t veci = 0; veci < keyVec.size(); ++veci) {
        if (keyVec.at(veci) == tmpKey 
          and clidVec.at(veci) == tmpClid
          and indexVec.at(veci) == tmpIndex)
        {
          alreadyAdded = true;
          break;
        }
      }
      if (!alreadyAdded) {
        keyVec.push_back( tmpKey );
        clidVec.push_back( tmpClid );
        indexVec.push_back( tmpIndex );
        sourceVec.push_back( start );
      }
    }
    return found;
  }



  bool typelessFindLink(const Decision* start,
    const std::string& linkName, 
    uint32_t& key, 
    uint32_t& clid, 
    uint16_t& index,
    const Decision*& source,
    const bool suppressMultipleLinksWarning)
  {
    using namespace msgFindLink;
    // We use findLink in cases where there is only one link to be found, or if there are multiple then we 
    // only want the most recent.
    // Hence we can supply TrigDefs::lastFeatureOfType.                                                         /--> parent3(link)
    // We can still have more then one link found if there is a branch in the navigation. E.g. start --> parent1 --> parent2(link)
    // If both parent2 and parent3 possessed an admissible ElementLink, then the warning below will trigger, and only one of the
    // links will be returned (whichever of parent2 or parent3 happened to be the first seed of parent1).
    std::vector<uint32_t> keyVec;
    std::vector<uint32_t> clidVec;
    std::vector<uint16_t> indexVec;
    std::vector<const Decision*> sourceVec;
    std::set<const xAOD::TrigComposite*> fullyExploredFrom;

    const bool result = typelessFindLinks(start, linkName, keyVec, clidVec, indexVec, sourceVec, TrigDefs::lastFeatureOfType, &fullyExploredFrom);
    if (!result) {
      return false; // Nothing found
    }

    if (keyVec.size() > 1 && !suppressMultipleLinksWarning) {
      ANA_MSG_WARNING (keyVec.size() << " typeless links found for " << linkName
                       << " returning the first link, consider using findLinks.");
    }
    key = keyVec.at(0);
    clid = clidVec.at(0);
    index = indexVec.at(0);
    source = sourceVec.at(0);
    return true; 
  }


  bool typelessFindLink(const NavGraph& subGraph,
    const std::string& linkName, 
    uint32_t& key,
    uint32_t& clid,
    uint16_t& index,
    const Decision*& source,
    const bool suppressMultipleLinksWarning)
  {
    using namespace msgFindLink;
    // Note: This function should be the same as its predecessor, just using a NavGraph to start rather than a Decision*
    // As a result, it can search from more than one Decision* (the NavGraph may have more than one final node)
    // but it will still warn if this results in more than one link being located.
    std::vector<uint32_t> keyVec;
    std::vector<uint32_t> clidVec;
    std::vector<uint16_t> indexVec;
    std::vector<const Decision*> sourceVec;
    std::set<const Decision*> fullyExploredFrom;

    bool result = false;
    for (const NavGraphNode* finalNode : subGraph.finalNodes()) {
      result |= typelessFindLinks(finalNode, linkName, keyVec, clidVec, indexVec, sourceVec, TrigDefs::lastFeatureOfType, &fullyExploredFrom);
    }

    if (!result) {
      return false; // Nothing found
    }

    if (keyVec.size() > 1 && !suppressMultipleLinksWarning) {
      ANA_MSG_WARNING (keyVec.size() << " typeless links found for " << linkName
                       << " returning the first link, consider using findLinks.");
    }
    key = keyVec.at(0);
    clid = clidVec.at(0);
    index = indexVec.at(0);
    source = sourceVec.at(0);
    return true; 
  }


  Combinations buildCombinations(
    const std::string& chainName,
    const std::vector<LinkInfo<xAOD::IParticleContainer>>& features,
    const std::vector<std::size_t>& legMultiplicities,
    const std::function<bool(const std::vector<LinkInfo<xAOD::IParticleContainer>>&)>& filter)
  {
    Combinations combinations(filter);
    combinations.reserve(legMultiplicities.size());
    if (legMultiplicities.size() == 1)
      combinations.addLeg(legMultiplicities.at(0), features);
    else
      for (std::size_t legIdx = 0; legIdx < legMultiplicities.size(); ++legIdx)
      {
        HLT::Identifier legID = createLegName(chainName, legIdx);
        std::vector<LinkInfo<xAOD::IParticleContainer>> legFeatures;
        for (const LinkInfo<xAOD::IParticleContainer>& info : features)
          if (isAnyIDPassing(info.source, {legID.numeric()}))
            legFeatures.push_back(info);
      combinations.addLeg(legMultiplicities.at(legIdx), std::move(legFeatures));
      }
    return combinations;
  }


  Combinations buildCombinations(
    const std::string& chainName,
    const std::vector<LinkInfo<xAOD::IParticleContainer>>& features,
    const std::vector<std::size_t>& legMultiplicities,
    FilterType filter)
  {
    return buildCombinations(chainName, features, legMultiplicities, getFilter(filter));
  }

  Combinations buildCombinations(
    const std::string& chainName,
    const std::vector<LinkInfo<xAOD::IParticleContainer>>& features,
    const TrigConf::HLTChain *chainInfo,
    const std::function<bool(const std::vector<LinkInfo<xAOD::IParticleContainer>>&)>& filter)
  {
    return buildCombinations(chainName, features, chainInfo->leg_multiplicities(), filter);
  }

  Combinations buildCombinations(
    const std::string& chainName,
    const std::vector<LinkInfo<xAOD::IParticleContainer>>& features,
    const TrigConf::HLTChain *chainInfo,
    FilterType filter)
  {
    return buildCombinations(chainName, features, chainInfo, getFilter(filter));
  }


  std::string dump( const Decision* tc, const std::function< std::string( const Decision* )>& printerFnc ) {
    std::string ret; 
    ret += printerFnc( tc );
    if ( hasLinkToPrevious(tc) ) {
      const std::vector<ElementLink<DecisionContainer>> seeds = getLinkToPrevious(tc);
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
  
  const std::string& hltSeedingNodeName(){
    return Decision::s_hltSeedingNodeNameString;
  }

  const std::string& filterNodeName(){
    return Decision::s_filterNodeNameString;
  }

  const std::string& inputMakerNodeName(){
    return Decision::s_inputMakerNodeNameString;
  }

  const std::string& hypoAlgNodeName(){
    return Decision::s_hypoAlgNodeNameString;
  }

  const std::string& comboHypoAlgNodeName(){
    return Decision::s_comboHypoAlgNodeNameString;
  }

  const std::string& summaryFilterNodeName(){
    return Decision::s_summaryFilterNodeNameString;
  }

  const std::string& summaryPassNodeName(){
    return Decision::s_summaryPassNodeNameString;
  }

  const std::string& summaryPrescaledNodeName(){
    return Decision::s_summaryPrescaledNodeNameString;
  }

}

