/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "AthLinks/ElementLinkVector.h"
#include "TrigConfHLTUtils/HLTUtils.h"

#include "Run2ToRun3TrigNavConverterV2.h"

namespace TCU = TrigCompositeUtils;

// helper class 
ConvProxy::ConvProxy(const HLT::TriggerElement* t) : te{ t } {}

void ConvProxy::merge(ConvProxy* /*other*/) {
  // copy over chains & links

  // prune the other
}





// the algorithm
Run2ToRun3TrigNavConverterV2::Run2ToRun3TrigNavConverterV2(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{
}

Run2ToRun3TrigNavConverterV2::~Run2ToRun3TrigNavConverterV2()
{
}

StatusCode Run2ToRun3TrigNavConverterV2::initialize()
{
  ATH_CHECK(m_trigOutputNavKey.initialize());
  ATH_CHECK(m_tdt.empty() != m_trigNavKey.key().empty()); //either of the two has to be enabled but not both
  if (!m_tdt.empty()) {
    ATH_CHECK(m_tdt.retrieve());
    ATH_MSG_INFO("Will use Trigger Navigation from TrigDecisionTool");
  }
  else {
    ATH_CHECK(m_trigNavKey.initialize(SG::AllowEmpty));
    ATH_MSG_INFO("Will use Trigger Navigation decoded from TrigNavigation object");
  }

  ATH_CHECK(m_configSvc.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::execute(const EventContext& context) const
{
  // need to check if this step works in start() or initialize, if so we need to it once
  // and remember this mapping 
  TEIdToChainsMap_t allTEIdsToChains, finalTEIdsToChains;
  ATH_CHECK(extractTECtoChainMapping(allTEIdsToChains, finalTEIdsToChains));

  ConvProxySet_t convProxies;
  HLT::StandaloneNavigation standaloneNav; // needed to keep TEs around
  ATH_CHECK(mirrorTEsStructure(convProxies, standaloneNav, context));

  if (m_doSelfValidation)
    ATH_CHECK(allProxiesConnected(convProxies));

  ATH_CHECK(associateChainsToProxies(convProxies, allTEIdsToChains));
  ATH_CHECK(cureUnassociatedProxies(convProxies));
  ATH_MSG_DEBUG("Proxies to chains mapping done");

  if (not m_chainsToSave.empty()) {
    ATH_CHECK(removeUnassociatedProxies(convProxies));
    ATH_MSG_DEBUG("Removed proxies to chains that are not converted, remaining number of elements " << convProxies.size());
  }
  if (m_doSelfValidation) {
    ATH_CHECK(allProxiesHaveChain(convProxies));
  }
  if (m_doCompression) {
    ATH_CHECK(doCompression(convProxies));
  }
  if (m_doLinkFeatures) {
    ATH_CHECK(fillRelevantFeatures(convProxies));
    ATH_MSG_DEBUG("Features to link found");
  }
  SG::WriteHandle<TrigCompositeUtils::DecisionContainer> outputNavigation = TrigCompositeUtils::createAndStore(m_trigOutputNavKey, context);
  auto decisionOutput = outputNavigation.ptr();
  TrigCompositeUtils::newDecisionIn(decisionOutput, "HLTPassRaw"); // we rely on the fact that the 1st element is the top

  ATH_CHECK(createIMHNodes(convProxies, *decisionOutput));
  if (m_doSelfValidation) {
    ATH_CHECK(numberOfHNodesPerProxyNotExcessive(convProxies));
  }
  ATH_CHECK(createFSNodes(convProxies, *decisionOutput, finalTEIdsToChains));
  ATH_CHECK(linkTopNode(*decisionOutput));
  ATH_MSG_DEBUG("Conversion done, from " << convProxies.size() << " elements to " << decisionOutput->size() << " elements");

  // dispose temporaries
  for (auto proxy : convProxies) {
    delete proxy;
  }

  return StatusCode::SUCCESS;
}




StatusCode Run2ToRun3TrigNavConverterV2::extractTECtoChainMapping(TEIdToChainsMap_t& allTEs, TEIdToChainsMap_t& finalTEs) const {
  // port chains iteration code from previous version
  ATH_MSG_DEBUG("Recognised " << allTEs.size() << " kinds of TEs and among them " << finalTEs.size() << " final types");

  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::mirrorTEsStructure(ConvProxySet_t& convProxies, HLT::StandaloneNavigation& standaloneNav, const EventContext& context) const {
  const HLT::TrigNavStructure* run2NavigationPtr = nullptr;

  if (!m_trigNavKey.key().empty()) {
    SG::ReadHandle navReadHandle(m_trigNavKey, context);
    ATH_CHECK(navReadHandle.isValid());
    standaloneNav.deserialize(navReadHandle->serialized());
    run2NavigationPtr = &standaloneNav;
  }
  else {
    run2NavigationPtr = m_tdt->ExperimentalAndExpertMethods().getNavigation();
  }
  const HLT::TrigNavStructure& run2Navigation = *run2NavigationPtr;

  // iterate over the TEs, for each make the ConvProxy and build connections
  std::map<const HLT::TriggerElement*, ConvProxy*> teToProxy;
  ATH_MSG_DEBUG("TrigNavStructure with " << run2Navigation.getAllTEs().size() << " TEs acquired");
  for (auto te : run2Navigation.getAllTEs()) {
    // skip event seed node
    if (HLT::TrigNavStructure::isInitialNode(te)) continue;
    auto proxy = new ConvProxy(te);
    convProxies.insert(proxy);
    // add linking 
    for (auto predecessor : HLT::TrigNavStructure::getDirectPredecessors(te)) {
      ConvProxy* predecessorProxy = teToProxy[predecessor];
      if (predecessorProxy != nullptr) { // because we skip some
        proxy->parents.push_back(predecessorProxy);
        predecessorProxy->children.push_back(proxy);
      }
    }
  }
  ATH_MSG_DEBUG("Created " << convProxies.size() << " proxy objects");
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::associateChainsToProxies(ConvProxySet_t&, const TEIdToChainsMap_t&) const {
  // using map chain IDs mapping 

  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::cureUnassociatedProxies(ConvProxySet_t& convProxies) const {
  // propagate up (towards L1) chain IDs if they are not in proxies
  // technically each proxy looks at the children proxies and inserts from it all unseen chains
  // procedure is repeated until, no single proxy needs an update (tedious - we may be smarter in future)

  while (true) {
    size_t numberOfUpdates = 0;
    for (auto p : convProxies) {
      for (auto child : p->children) {
        size_t startSize = p->runChains.size();
        p->runChains.insert(std::begin(child->runChains), std::end(child->runChains));

        if (startSize != p->runChains.size()) { // some chain needed to be inserted
          numberOfUpdates++;
          // if update was need, it means set of chains that passed need update as well
          p->passChains.insert(std::begin(child->runChains), std::end(child->runChains));
        }
      }
    }
    ATH_MSG_DEBUG("Needed to propagate chains from " << numberOfUpdates << " child(ren)");
    if (numberOfUpdates == 0) {
      break;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::removeUnassociatedProxies(ConvProxySet_t& convProxies) const {
  // remove proxies that have no chains
  for (auto i = std::begin(convProxies); i != std::end(convProxies);) {
    if ((*i)->runChains.empty()) {
      // TODO we may need to deregister it from it's children & parents
      delete* i;
      i = convProxies.erase(i);
    }
    else {
      ++i;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::doCompression(ConvProxySet_t& convProxies) const {
  ATH_CHECK(fillFEAHashes(convProxies));
  FEAToConvProxySet_t sharedFeaturesMap;
  ATH_CHECK(findSharedFEAHashes(convProxies, sharedFeaturesMap));
  if (m_doSelfValidation) {
    ATH_CHECK(allFEAHashesAreUnique(sharedFeaturesMap));
  }
  ATH_CHECK(collapseConvProxies(convProxies, sharedFeaturesMap));
  ATH_CHECK(cureFeaturelessProxies(convProxies));
  if (m_doSelfValidation) {
    ATH_CHECK(allProxiesHaveChain(convProxies));
    ATH_CHECK(allProxiesConnected(convProxies));
  }
  ATH_MSG_DEBUG("Compression done");

  return StatusCode::SUCCESS;
}



StatusCode Run2ToRun3TrigNavConverterV2::fillFEAHashes(ConvProxySet_t& convProxies) const {
  // calculate fea hash for each vector<FEA> from te associated to proxy
  for ( auto proxy: convProxies ) {
    proxy->feaHash = feaToHash(proxy->te->getFeatureAccessHelpers());
    ATH_MSG_VERBOSE("TE " << TrigConf::HLTUtils::hash2string(proxy->te->getId()) << " FEA hash " << proxy->feaHash);    
    for ( auto fea : proxy->te->getFeatureAccessHelpers()) {
      ATH_MSG_VERBOSE( "FEA: " << fea);
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::findSharedFEAHashes(const ConvProxySet_t& convProxies, FEAToConvProxySet_t& feaToProxyMap) const {
  // ceate feahash -> [te1, te2...] mapping, these tes are candidates for merging/compression
  for ( auto proxy: convProxies ) {
    feaToProxyMap[proxy->feaHash].insert(proxy);
  }
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::collapseConvProxies(ConvProxySet_t&, const FEAToConvProxySet_t&) const {
  // this for all entries in the FEA to TES map, use the first TE as a "primary" and merge all others to it


  // remove from proxies set all elements that are now unassociated (remember to delete after)
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::cureFeaturelessProxies(ConvProxySet_t&) const {
  // heuristically compres proxies that mirror TEs w/o any feature
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::fillRelevantFeatures(ConvProxySet_t&) const {
  // from all FEAs of the associated TE pick those objects that are to be linked
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::createIMHNodes(ConvProxySet_t&, xAOD::TrigCompositeContainer& decisions) const {
  // create nodes of ne navigation for relevant features
  ATH_MSG_DEBUG("IM & H nodes made, output nav elements " << decisions.size());
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::createFSNodes(const ConvProxySet_t&, xAOD::TrigCompositeContainer& decisions, const TEIdToChainsMap_t&) const {
  // associate terminal nodes to filter nodes, 
  // associate all nodes designated as final one with the filter nodes
  ATH_MSG_DEBUG("FS nodes made, output nav elements " << decisions.size());
  return StatusCode::SUCCESS;
}
StatusCode Run2ToRun3TrigNavConverterV2::linkTopNode(xAOD::TrigCompositeContainer& decisions) const {
  // simply link all filter nodes to the HLTPassRaw (the 1st element)
  ATH_CHECK((*decisions.begin())->name() == "HLTPassRaw");
  return StatusCode::SUCCESS;
}


uint64_t Run2ToRun3TrigNavConverterV2::feaToHash(const std::vector<HLT::TriggerElement::FeatureAccessHelper>& feaVector) const {
  // FEA vectors hashing
  uint64_t hash = 0;
  for ( auto fea:  feaVector) {
    // we may need filtering here in the future
    uint64_t repr64 = static_cast<uint64_t>(fea.getCLID()) << 32 | static_cast<uint64_t>(fea.getIndex().subTypeIndex())<<24 | (fea.getIndex().objectsBegin() << 16 ^ fea.getIndex().objectsEnd());
    hash ^= repr64;
  }
  return hash;

}

StatusCode Run2ToRun3TrigNavConverterV2::allProxiesHaveChain(const ConvProxySet_t& proxies) const {
  for (auto p : proxies) {
    if (p->runChains.empty()) {
      ATH_MSG_ERROR("Proxy with no chains");
      return StatusCode::FAILURE;
    }
  }
  ATH_MSG_DEBUG("CHECK OK, no proxies w/o a chain");
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::allProxiesConnected(const ConvProxySet_t& proxies) const {
  for (auto p : proxies) {
    if (p->children.empty() and p->parents.empty()) {
      ATH_MSG_ERROR("Orphanted proxy");
      return StatusCode::FAILURE;
    }
  }
  ATH_MSG_DEBUG("CHECK OK, no orphanted proxies");
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::allFEAHashesAreUnique(const FEAToConvProxySet_t& feaToProxies) const {
  for (auto [feaHash, proxies] : feaToProxies) {
    auto first = *proxies.begin();
    for (auto p : proxies) {
      if (p->te->getFeatureAccessHelpers() != first->te->getFeatureAccessHelpers()) {
        ATH_MSG_ERROR("Proxies grouped in FEA to Proxies map entries have distinct features");
        return StatusCode::FAILURE;
      }
    }
  }
  ATH_MSG_DEBUG("CHECK OK, FE hashes unique");
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::numberOfHNodesPerProxyNotExcessive(const ConvProxySet_t& proxies) const {
  for (auto p : proxies) {
    if (p->hNodes.size() > m_hNodesPerProxyThreshold) {
      ATH_MSG_ERROR("Too many H nodes per proxy");
      return StatusCode::FAILURE;
    }
  }
  ATH_MSG_DEBUG("CHECK OK, no excessive number of H nodes per proxy");
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::noUnconnectedHNodes(const xAOD::TrigCompositeContainer& decisions)const {
  // build map of all links to H nodes from IMs and FS
  std::set<const TrigCompositeUtils::Decision*> linkedHNodes;
  for (auto d : decisions) {
    if (d->name() == "IM" or d->name() == "FS") {
      for (auto el : TCU::getLinkToPrevious(d)) {
        linkedHNodes.insert(*el); // dereferences to bare pointer
      }
    }
  }
  for (auto d : decisions) {
    if (d->name() == "H") {
      if (linkedHNodes.count(d) == 0) {
        ATH_MSG_ERROR("Orphanted H node");
        return StatusCode::FAILURE;
      }
    }
  }
  ATH_MSG_DEBUG("CHECK OK, all H modes are connected");

  return StatusCode::SUCCESS;
}