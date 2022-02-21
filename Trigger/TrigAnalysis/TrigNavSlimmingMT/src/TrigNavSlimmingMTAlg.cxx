
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigNavSlimmingMTAlg.h"

#include "TrigTimeAlgs/TrigTimeStamp.h"

#include "xAODParticleEvent/ParticleAuxContainer.h"
#include "xAODTrigMissingET/TrigMissingETAuxContainer.h"

using namespace TrigCompositeUtils;


// Particle Specialization
// We transiently interact via the xAOD::IParticle interface.
// There is code inside the TrigCompositeUtils::Decision to check
// at runtime if this inheritance is possible.
// But we copy into a xAOD::Particle object.
template<>
StatusCode TrigNavSlimmingMTAlg::doRepack(TrigCompositeUtils::Decision* decision,
  SG::WriteHandle<xAOD::ParticleContainer>* writeHandle,
  const std::string& edgeName) const
{

  if (not decision->hasObjectLink(edgeName, ClassID_traits<xAOD::IParticleContainer>::ID())) { // Note: IParticle
    // Nothing to do
    return StatusCode::SUCCESS;
  }

  ElementLink<xAOD::IParticleContainer> currentEL = decision->objectLink<xAOD::IParticleContainer>(edgeName); // Note: IParticle

  if (!currentEL.isValid()) {
    ATH_MSG_WARNING("Unable to repack '" << edgeName << "' of container type xAOD::IParticleContainer for '"
      << decision->name() << "' node, the link is invalid.");
    ATH_MSG_DEBUG("Dump of DecisionObject: " << *decision);
    return StatusCode::SUCCESS;
  }

  (**writeHandle).push_back( new xAOD::Particle() ); // Need to do this before performing the copy to assign with the Aux store

  const xAOD::IParticle* current = *currentEL;
  xAOD::Particle* remapped = (**writeHandle).back();

  remapped->setP4( current->p4() );

  ElementLink<xAOD::ParticleContainer> remappedEL(**writeHandle, (**writeHandle).size()-1);
  decision->setObjectLink<xAOD::ParticleContainer>(edgeName, remappedEL); // Overwrite the existing link

  ATH_MSG_DEBUG("Repacked from index:" << currentEL.index() << " from key:" << currentEL.dataID() 
    << ", to index:" << remappedEL.index() << " to key:" << remappedEL.key());

  return StatusCode::SUCCESS;
}


// ROI Specialization.
// Not an xAOD object, can use a direct copy constructor.
template<>
StatusCode TrigNavSlimmingMTAlg::doRepackCopy(const TrigRoiDescriptor* object,
  SG::WriteHandle<TrigRoiDescriptorCollection>* writeHandle) const
{
  // Specialization for ROIs, utilize copy constructor (no Aux store here)
  (**writeHandle).push_back( new TrigRoiDescriptor(*object) );
  return StatusCode::SUCCESS;
}


TrigNavSlimmingMTAlg::TrigNavSlimmingMTAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{
}


StatusCode TrigNavSlimmingMTAlg::initialize() {
  ATH_CHECK( m_primaryInputCollection.initialize() );
  ATH_CHECK( m_outputCollection.initialize() );
  ATH_CHECK( m_outputRepackedROICollectionKey.initialize(m_repackROIs) );
  ATH_CHECK( m_outputRepackedFeaturesCollectionKey_Particle.initialize(m_repackFeatures) );
  ATH_CHECK( m_outputRepackedFeaturesCollectionKey_MET.initialize(m_repackFeatures) );

  const bool removeRoI = (std::find(m_edgesToDrop.begin(), m_edgesToDrop.end(), roiString()) != m_edgesToDrop.end());
  const bool removeInitialRoI = (std::find(m_edgesToDrop.begin(), m_edgesToDrop.end(), initialRoIString()) != m_edgesToDrop.end());

  if (m_repackROIs and (removeRoI or removeInitialRoI)) {
    ATH_MSG_WARNING("Possible miss-configuration. Cannot repack ROIs in the navigation slimming if they are being dropped");
  }

  if (not m_trigDec.empty()) {
    ATH_CHECK( m_trigDec.retrieve() );
  }
  for (const std::string& output : m_allOutputContainers) {
    if (output == m_primaryInputCollection.key()) {
      continue; // We do want to search for failed nodes in the primary input (it may already be merged)
    }
    // We don't want to search for failed nodes in other possible summary keys, we might read in the 
    // summary collection from another running instance (e.g. an AODSlim alg reading in the output of
    // ESDSlim in a RAWtoALL job).
    m_allOutputContainersSet.insert(output);
  }
  m_allOutputContainersSet.insert(m_outputCollection.key());
  msg() << MSG::INFO << "Initialized. Will *not* inspect the following SG Keys: ";
  for (const std::string& key : m_allOutputContainersSet) {
    msg() << key << " ";
  }
  msg() << endmsg;
  return StatusCode::SUCCESS;
}


StatusCode TrigNavSlimmingMTAlg::execute(const EventContext& ctx) const {

  // Prepare IO
  Outputs outputContainers;

  SG::WriteHandle<TrigRoiDescriptorCollection> outputROIs;
  SG::WriteHandle<xAOD::ParticleContainer> outputParticles; 
  SG::WriteHandle<xAOD::TrigMissingETContainer> outputMETs;
  if (m_repackROIs) {
    outputROIs = createAndStoreNoAux(m_outputRepackedROICollectionKey, ctx);
    outputContainers.rois = &outputROIs;
  }
  if (m_repackFeatures) {
    outputParticles = createAndStoreWithAux<xAOD::ParticleContainer, xAOD::ParticleAuxContainer>(m_outputRepackedFeaturesCollectionKey_Particle, ctx);
    outputMETs = createAndStoreWithAux<xAOD::TrigMissingETContainer, xAOD::TrigMissingETAuxContainer>(m_outputRepackedFeaturesCollectionKey_MET, ctx);
    outputContainers.particles = &outputParticles;
    outputContainers.mets = &outputMETs;
  }

  SG::WriteHandle<DecisionContainer> outputNavigation = createAndStore(m_outputCollection, ctx);
  outputContainers.nav = &outputNavigation;

  SG::ReadHandle<DecisionContainer> primaryInputHandle = SG::ReadHandle(m_primaryInputCollection, ctx);
  ATH_CHECK(primaryInputHandle.isValid());

  const Decision* terminusNode = TrigCompositeUtils::getTerminusNode(primaryInputHandle);
  if (!terminusNode) {
    ATH_MSG_ERROR("Unable to locate the HLTPassRaw from the primary input navigation collection, size:" << primaryInputHandle->size());
    return StatusCode::FAILURE;
  }

  // Stage 1. Build a transient representation of the navigation graph.
  TrigTimeStamp stage1;
  NavGraph transientNavGraph;

  // We can optionally only keep data for a given set of chains. An empty set means to keep for all chains.
  DecisionIDContainer chainIDs = {};
  if (not m_chainsFilter.empty()) {
    ATH_CHECK(fillChainIDs(chainIDs));
    ATH_MSG_DEBUG("Supplied " << m_chainsFilter.size() << " chain patterns. This converts to " << chainIDs.size() << " DecisionIDs to be preserved.");
  }
  if (chainIDs.size() == 0) {
    ATH_MSG_DEBUG("chainIDs size is zero. No HLT-chain based filtering of the navigation graph will be performed.");
  }

  std::set<const Decision*> fullyExploredFrom;
  // Note: We use the "internal" version of this call such that we maintain our own cache, 
  // as we may need to call this more than once if keepFailedBranches is ture
  TrigCompositeUtils::recursiveGetDecisionsInternal(terminusNode, 
    nullptr, // 'Coming from' is nullptr for the first call of the recursive function
    transientNavGraph, 
    fullyExploredFrom, 
    chainIDs, 
    /*enforce chainIDs on terminus node*/ true);

  ATH_MSG_DEBUG("Collated nodes from passing paths, now have " << transientNavGraph.nodes() << " nodes with " << transientNavGraph.edges() << "edges");

  // Stage 2. We can optionally include branches through the graph which were never accepted by any chain.
  TrigTimeStamp stage2;
  // These branches do not connect to the terminusNode, so we have to go hunting them explicitly.
  // We need to pass in the evtStore as these nodes can be spread out over numerous collections.
  // Like with the terminus node, we can restrict this search to only nodes which were rejected by certain chains.
  // We also want to restrict the search to exclude the output collections of any other TrigNavSlimminMTAlg instances
  // and let the function know what the primary input collection is - from the name of this we can tell if we need to search one or many containers.
  if (m_keepFailedBranches) {
    std::vector<const Decision*> rejectedNodes = TrigCompositeUtils::getRejectedDecisionNodes(&*evtStore(), m_primaryInputCollection.key(), chainIDs, m_allOutputContainersSet);
    for (const Decision* rejectedNode : rejectedNodes) {
      // We do *not* enforce that a member of chainIDs must be present in the starting node (rejectedNode)
      // specifically because we know that at least one of chainIDs was _rejected_ here, but is active in the rejected
      // node's seeds.
      TrigCompositeUtils::recursiveGetDecisionsInternal(rejectedNode, 
        nullptr, // 'Coming from' is nullptr for the first call of the recursive function
        transientNavGraph, 
        fullyExploredFrom, 
        chainIDs, 
        /*enforce chainIDs on terminus node*/ false);
    }
    ATH_MSG_DEBUG("Collated nodes from failing paths, now have " << transientNavGraph.nodes() << " nodes with " << transientNavGraph.edges() << "edges");
  }

  // Stage 3. Walk all paths through the graph. Flag for thinning.
  TrigTimeStamp stage3;
  // Final nodes includes the terminus node, plus any rejected nodes (if these were collated).
  TrigCompositeUtils::recursiveFlagForThinning(transientNavGraph, m_keepOnlyFinalFeatures, m_nodesToDrop);

  // Stage 4. Do the thinning. Re-wire removed nodes as we go.
  TrigTimeStamp stage4;
  const size_t nodesBefore = transientNavGraph.nodes();
  const size_t edgesBefore = transientNavGraph.edges();
  std::vector<const Decision*> thinnedInputNodes = transientNavGraph.thin();

  // TODO - thinnedInputNodes will be dropped, these may link to "features", "roi", or other objects in other containers.
  // Need to let the slimming svc know that we no longer need the objects pointed to here, and hence they can be thinned.

  ATH_MSG_DEBUG("Trigger navigation graph thinning going from " << nodesBefore << " nodes with " << edgesBefore << " edges, to "
    << transientNavGraph.nodes() << " nodes with " << transientNavGraph.edges() << " edges");

  if (msg().level() <= MSG::VERBOSE) {
    ATH_MSG_VERBOSE("The navigation graph has been slimmed to the following paths");
    transientNavGraph.printAllPaths(msg(), MSG::VERBOSE);
  }

  // Stage 5. Fill the transientNavGraph structure (with NavGraphNode* nodes) back into an xAOD::DecisionContainer (with xAOD::Decision* nodes).
  TrigTimeStamp stage5;
  IOCacheMap cache; // Used to keep a one-to-one relationship between the const input Decision* and the mutable output Decision*
  // Do the terminus node first - such that it ends up at index 0 of the outputNavigation (fast to locate in the future)
  Decision* terminusNodeOut = nullptr;
  ATH_CHECK(inputToOutput(terminusNode, terminusNodeOut, cache, outputContainers, chainIDs, ctx));
  // Don't have to walk the graph here, just iterate through the set of (thinned) nodes.
  // We won't end up with two terminus nodes because of this (it checks that the node hasn't already been processed)
  const std::vector<NavGraphNode*> allNodes = transientNavGraph.allNodes();
  for (const NavGraphNode* inputNode : allNodes) {
    Decision* outputNode = nullptr;
    ATH_CHECK(inputToOutput(inputNode->node(), outputNode, cache, outputContainers, chainIDs, ctx));
    // TODO - anything else to do here with outputNode? We cannot hook up its seeding yet, we may not yet have output nodes for all of its seeds.
  }
  // Now we have all of the new nodes in the output collection, can link them all up with their slimmed seeding relationships.
  for (const NavGraphNode* inputNode : allNodes) {
    ATH_CHECK(propagateSeedingRelation(inputNode, cache, ctx));
  }

  if (msg().level() <= MSG::DEBUG) {
    ATH_MSG_DEBUG("Navigation slimming and thinning timings:");
    ATH_MSG_DEBUG("  1. Transient Graph of Passed Nodes = " << stage1.millisecondsDifference(stage2) << " ms");
    ATH_MSG_DEBUG("  2. Transient Graph of Failed Nodes = " << stage2.millisecondsDifference(stage3) << " ms");
    ATH_MSG_DEBUG("  3. Flag Transient Graph For Thinning = " << stage3.millisecondsDifference(stage4) << " ms");
    ATH_MSG_DEBUG("  4. Perform Transient Graph Thinning = " << stage4.millisecondsDifference(stage5) << " ms");
    ATH_MSG_DEBUG("  5. Write xAOD Graph = " << stage5.millisecondsSince() << " ms");
  }

  return StatusCode::SUCCESS;  
}


StatusCode TrigNavSlimmingMTAlg::fillChainIDs(DecisionIDContainer& chainIDs) const {
  for (const std::string& filter : m_chainsFilter) {
    // We do this as filter->chains stage as filter could be a regexp matching a large number of chains
    const Trig::ChainGroup* cg = m_trigDec->getChainGroup(filter);
    std::vector<std::string> chains = cg->getListOfTriggers();
    for (const std::string& chain : chains) {
      const TrigConf::HLTChain* hltChain = m_trigDec->ExperimentalAndExpertMethods().getChainConfigurationDetails(chain);
      const HLT::Identifier chainID( hltChain->chain_name() );
      chainIDs.insert( chainID.numeric() );
      const std::vector<size_t> legMultiplicites = hltChain->leg_multiplicities();
      if (legMultiplicites.size() == 0) {
        ATH_MSG_ERROR("chain " << chainID << " has invalid configuration, no multiplicity data.");
      } else if (legMultiplicites.size() > 1) {
        // For multi-leg chains, the DecisionIDs are handled per leg.
        // We don't care here exactly how many objects are required per leg, just that there are two-or-more legs
        for (size_t legNumeral = 0; legNumeral < legMultiplicites.size(); ++legNumeral) {
          const HLT::Identifier legID = TrigCompositeUtils::createLegName(chainID, legNumeral);
          chainIDs.insert( legID.numeric() );
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode TrigNavSlimmingMTAlg::inputToOutput(
  const TrigCompositeUtils::Decision* input, 
  TrigCompositeUtils::Decision* output,
  IOCacheMap& cache, 
  Outputs& outputContainers,
  const DecisionIDContainer& chainIDs,
  const EventContext& ctx) const
{
  IOCacheMap::const_iterator it = cache.find(input);
  if (it != cache.end()) {
    output = it->second;
  } else {
    output = newDecisionIn(outputContainers.nav->ptr(), input, input->name(), ctx);
    ATH_CHECK(propagateLinks(input, output));
    ATH_CHECK(propagateDecisionIDs(input, output, chainIDs));
    ATH_CHECK(repackLinks(output, outputContainers));
    cache[input] = output;
  }
  return StatusCode::SUCCESS;
}


StatusCode TrigNavSlimmingMTAlg::propagateSeedingRelation(  
  const TrigCompositeUtils::NavGraphNode* inputNode, 
  IOCacheMap& cache,
  const EventContext& ctx) const
{
  const Decision* inputDecision = inputNode->node(); // The incoming Decision objects, with links into the old graph 
  Decision* outputDecision = nullptr; // The outgoing Decision object, without graph links so far
  {
    IOCacheMap::const_iterator it = cache.find(inputDecision);
    ATH_CHECK( it != cache.end() );
    outputDecision = it->second;
  }
  for (const NavGraphNode* seed : inputNode->seeds()) {
    const Decision* inputSeedDecision = seed->node(); // A Decision object the incoming Decision object links to (old graph)
    const Decision* outputSeedDecision = nullptr; // The equivalent Decision Object in the slimmed outgoing graph
    {
      IOCacheMap::const_iterator it = cache.find(inputSeedDecision);
      ATH_CHECK( it != cache.end() );
      outputSeedDecision = it->second;
    }
    // Perform the linking only using nodes from the slimmed output graph
    TrigCompositeUtils::linkToPrevious(outputDecision, outputSeedDecision, ctx);
  }
  return StatusCode::SUCCESS;
}


StatusCode TrigNavSlimmingMTAlg::propagateLinks(  
  const TrigCompositeUtils::Decision* input, 
  TrigCompositeUtils::Decision* output) const
{
  // ElementLinks form the edges in the graph

  output->copyAllLinksFrom( input );
  for (const std::string& toRemove : m_edgesToDrop) {
    output->removeObjectLink(toRemove);
    output->removeObjectCollectionLinks(toRemove);
    // TODO - let the slimming svc know that we no longer need these objects
  }

  // Do not propagate "seed" links - TrigNavSlimmingMTAlg will 
  // propagate these following additional logic
  output->removeObjectCollectionLinks( seedString() );

  return StatusCode::SUCCESS;
}


StatusCode TrigNavSlimmingMTAlg::propagateDecisionIDs(  
  const TrigCompositeUtils::Decision* input, 
  TrigCompositeUtils::Decision* output,
  const DecisionIDContainer& chainIDs) const
{

  // Get all DecisionIDs from the const input Decision*
  DecisionIDContainer fromInput;
  decisionIDs(input, fromInput);

  DecisionIDContainer toOutput;

  if (chainIDs.size()) {
    // Applying ChainsFilter to the set of DecisionIDs
    std::set_intersection(fromInput.begin(), fromInput.end(), chainIDs.begin(), chainIDs.end(),
      std::inserter(toOutput, toOutput.begin()));
  } else {
    // Copying all DecisionIDs from input to output
    toOutput.insert(fromInput.begin(), fromInput.end());
  }

  // Set the DecisionIDs into the mutable output Decision* 
  insertDecisionIDs(toOutput, output);

  return StatusCode::SUCCESS;
}


StatusCode TrigNavSlimmingMTAlg::repackLinks(
  TrigCompositeUtils::Decision* output,
  Outputs& outputContainers) const
{
  
  if (m_repackROIs) {
    ATH_CHECK( doRepack<TrigRoiDescriptorCollection>(output, outputContainers.rois, roiString()) );
    ATH_CHECK( doRepack<TrigRoiDescriptorCollection>(output, outputContainers.rois, initialRoIString()) );
  }


  if (m_repackFeatures) {
    // Debug printing. Look at the four-momentum of any feature before the repacking.
    // Note: Transiently we interact with the IParticle interface.
    if (msgLvl(MSG::DEBUG)) {
      if (output->hasObjectLink(featureString(), ClassID_traits<xAOD::IParticleContainer>::ID())) {
        ElementLink<xAOD::IParticleContainer> before = output->objectLink<xAOD::IParticleContainer>(featureString());
        if (before.isValid()) {
          const xAOD::IParticle& b = **before;
          ATH_MSG_DEBUG("--- --- ---");
          ATH_MSG_DEBUG("IParticle repacking debug. Before: "
            << "(pt:" << b.pt() << ",eta:" << b.eta() << ",phi:" << b.phi() << ",m:" << b.m() << ",e:" << b.e() << ")"
            << " from:" << before.dataID());
        }
      }
    }

    // Do any IParticle repacking
    ATH_CHECK( doRepack<xAOD::ParticleContainer>(output, outputContainers.particles, featureString()) );

    // Debug printing. Look at the four-momentum of any feature after the repacking (the stored link is re-written)
    if (msgLvl(MSG::DEBUG)) {
      if (output->hasObjectLink(featureString(), ClassID_traits<xAOD::IParticleContainer>::ID())) {
        ElementLink<xAOD::IParticleContainer> after = output->objectLink<xAOD::IParticleContainer>(featureString());
        if (after.isValid()) {
          const xAOD::IParticle& a = **after;
          ATH_MSG_DEBUG("IParticle repacking debug. After : " 
            << "(pt:" << a.pt() << ",eta:" << a.eta() << ",phi:" << a.phi() << ",m:" << a.m() << ",e:" << a.e() << ")"
            << " from:" << after.dataID());
        }
      }
    }

    // Some features do not support an IParticle interface. These need their own containers.
    // TODO. Apply some thinning?
    ATH_CHECK( doRepack<xAOD::TrigMissingETContainer>(output, outputContainers.mets, featureString()) );
  }


  return StatusCode::SUCCESS;
}
