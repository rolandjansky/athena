/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AthLinks/ElementLinkVector.h"
#include "TrigConfHLTUtils/HLTUtils.h"
#include "xAODTrigger/TrigPassBitsContainer.h"
#include "AthenaKernel/ClassID_traits.h"
#include "TrigNavStructure/TriggerElement.h"

#include "Run2ToRun3TrigNavConverterV2.h"

namespace TCU = TrigCompositeUtils;

// helper class
ConvProxy::ConvProxy(const HLT::TriggerElement *t) : te{t}
{
  teIDs.push_back(te->getId());
}

bool ConvProxy::mergeAllowed(const ConvProxy *other) const
{
  if (this == other)
    return false; // no merging with self
  // never merge children with parents
  for (auto c : children)
  {
    if (other == c)
      return false;
  }
  for (auto p : parents)
  {
    if (other == p)
      return false;
  }
  return true;
}

void ConvProxy::merge(ConvProxy *other)
{
  if (other == this)
  {
    return;
  }
  // copy over chains
  runChains.insert(other->runChains.begin(), other->runChains.end());
  passChains.insert(other->passChains.begin(), other->passChains.end());
  teIDs.push_back(other->te->getId());
  /* the intention of the code below is following.
  Intial structure like is like this (the line is always bidirectional):
  P1 P2 P3 <- parents
   | | /
  T1 T2 <- "this" and "other"
   | |
  C1 C2 <- children
  1) Lets assume that the first proxies we treat are B1 & B2 ath they are mergable. The resulting structure should look like this:
  P1 P2 P3
   |/__/
  T1 T2
   |\
  C1 C2

  */
  auto add = [](ConvProxy *toadd, std::vector<ConvProxy *> &coll)
  {
    if (std::find(coll.begin(), coll.end(), toadd) == coll.end())
    {
      coll.push_back(toadd);
    }
  };

  auto remove = [](ConvProxy *torem, std::vector<ConvProxy *> &coll)
  {
    auto place = std::find(coll.begin(), coll.end(), torem);
    if (place != coll.end())
    {
      coll.erase(place);
    }
  };

  // this is T <-> C connection
  for (auto otherChild : other->children)
  {
    add(otherChild, children);
    add(this, otherChild->parents);
  }
  // this is T <-> P connection rewiring
  for (auto otherParent : other->parents)
  {
    add(otherParent, parents);
    add(this, otherParent->children);
  }

  // now need to remove links back to the "other"
  for (auto otherParent : other->parents)
  {
    remove(other, otherParent->children);
  }

  for (auto otherChild : other->children)
  {
    remove(other, otherChild->parents);
  }
  other->children.clear();
  other->parents.clear();
}

std::string ConvProxy::description() const
{
  std::string ret;
  ret += " N parents: " + std::to_string(parents.size());
  ret += " N children: " + std::to_string(children.size());
  ret += " feaHash: " + std::to_string(feaHash);
  ret += " N run chains: " + std::to_string(runChains.size());
  return ret;
}

// the algorithm
Run2ToRun3TrigNavConverterV2::Run2ToRun3TrigNavConverterV2(const std::string &name, ISvcLocator *pSvcLocator) : AthReentrantAlgorithm(name, pSvcLocator)
{
}

Run2ToRun3TrigNavConverterV2::~Run2ToRun3TrigNavConverterV2()
{
}

StatusCode Run2ToRun3TrigNavConverterV2::initialize()
{
  ATH_CHECK(m_trigOutputNavKey.initialize());
  ATH_CHECK(m_tdt.empty() != m_trigNavKey.key().empty()); // either of the two has to be enabled but not both
  if (!m_tdt.empty())
  {
    ATH_CHECK(m_tdt.retrieve());
    ATH_MSG_INFO("Will use Trigger Navigation from TrigDecisionTool");
  }
  else
  {
    ATH_CHECK(m_trigNavKey.initialize(SG::AllowEmpty));
    ATH_MSG_INFO("Will use Trigger Navigation decoded from TrigNavigation object");
  }

  ATH_CHECK(m_configSvc.retrieve());
  ATH_CHECK(m_clidSvc.retrieve());

  // configured collections can be either just type name, or type#key
  // decoding takes this into account, if only the type is configured then empty string is places in the decoded lookup map
  // else CLID + a name is placed

  for (const auto &name : m_collectionsToSave)
  {
    std::string typeName = name;
    std::string collName;
    size_t delimeterIndex = name.find('#');
    if (delimeterIndex != std::string::npos)
    {
      ATH_MSG_ERROR("Not support for specific collection name yet");
      return StatusCode::FAILURE;
      typeName = name.substr(0, delimeterIndex);
      collName = name.substr(delimeterIndex + 1);
    }
    CLID id{0};
    ATH_CHECK(m_clidSvc->getIDOfTypeName(typeName, id));
    ATH_MSG_DEBUG("Will be linking collection type " << typeName << " name (empty==all) " << collName);
    m_collectionsToSaveDecoded[id].insert(collName);
  }

  for (const auto &name : m_roisToSave)
  {
    m_setRoiName.push_back(name);
  }

  // sanity check, i.e. if there is at least one entry w/o the coll name no other enties are needed for a given clid
  for (auto [clid, keysSet] : m_collectionsToSaveDecoded)
  {
    if (keysSet.size() > 1 and keysSet.count("") != 0)
    {
      ATH_MSG_ERROR("Bad configuration for CLID " << clid << " reuested saving of all (empty coll name configures) collections, yet there are also specific keys");
      return StatusCode::FAILURE;
    }
  }

  ATH_CHECK(m_clidSvc->getIDOfTypeName("TrigRoiDescriptor", m_roIDescriptorCLID));
  ATH_CHECK(m_clidSvc->getIDOfTypeName("TrigRoiDescriptorCollection", m_roIDescriptorCollectionCLID));
  ATH_CHECK(m_clidSvc->getIDOfTypeName("xAOD::TrigRingerRings", m_TrigRingerRingsCLID));
  ATH_CHECK(m_clidSvc->getIDOfTypeName("xAOD::TrigRingerRingsContainer", m_TrigRingerRingsContainerCLID));
  ATH_CHECK(m_clidSvc->getIDOfTypeName("xAOD::TrigEMCluster", m_TrigEMClusterCLID));
  ATH_CHECK(m_clidSvc->getIDOfTypeName("xAOD::TrigEMClusterContainer", m_TrigEMClusterContainerCLID));
  ATH_CHECK(m_clidSvc->getIDOfTypeName("xAOD::CaloCluster", m_CaloClusterCLID));
  ATH_CHECK(m_clidSvc->getIDOfTypeName("xAOD::CaloClusterContainer", m_CaloClusterContainerCLID));
  ATH_CHECK(m_clidSvc->getIDOfTypeName("xAOD::TrackParticleContainer", m_TrackParticleContainerCLID));
  ATH_CHECK(m_clidSvc->getIDOfTypeName("xAOD::TauTrackContainer", m_TauTrackContainerCLID));

  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::execute(const EventContext &context) const
{
  {
    // configuration reading could not be done before the event loop
    // it needs to be done only once though
    std::scoped_lock lock(m_configUpdateMutex);
    if (m_allTEIdsToChains.empty())
      ATH_CHECK(extractTECtoChainMapping(const_cast<TEIdToChainsMap_t &>(m_allTEIdsToChains), const_cast<TEIdToChainsMap_t &>(m_finalTEIdsToChains)));
  }

  ConvProxySet_t convProxies;
  HLT::StandaloneNavigation standaloneNav; // needed to keep TEs around, so it is out of the scope where it is filled and referenced
  const HLT::TrigNavStructure *run2NavigationPtr = nullptr;
  if (!m_trigNavKey.key().empty())
  {
    SG::ReadHandle navReadHandle(m_trigNavKey, context);
    ATH_CHECK(navReadHandle.isValid());
    standaloneNav.deserialize(navReadHandle->serialized());
    run2NavigationPtr = &standaloneNav;
  }
  else
  {
    run2NavigationPtr = m_tdt->ExperimentalAndExpertMethods().getNavigation();
  }

  ATH_CHECK(mirrorTEsStructure(convProxies, *run2NavigationPtr));

  if (m_doSelfValidation)
    ATH_CHECK(allProxiesConnected(convProxies));

  ATH_CHECK(associateChainsToProxies(convProxies, m_allTEIdsToChains));
  ATH_CHECK(cureUnassociatedProxies(convProxies));
  ATH_MSG_DEBUG("Proxies to chains mapping done");

  if (not m_chainsToSave.empty())
  {
    ATH_CHECK(removeUnassociatedProxies(convProxies));
    ATH_MSG_DEBUG("Removed proxies to chains that are not converted, remaining number of elements " << convProxies.size());
  }
  if (m_doSelfValidation)
  {
    ATH_CHECK(allProxiesHaveChain(convProxies));
  }
  if (m_doCompression)
  {
    ATH_CHECK(doCompression(convProxies, *run2NavigationPtr));
  }

  SG::WriteHandle<TrigCompositeUtils::DecisionContainer> outputNavigation = TrigCompositeUtils::createAndStore(m_trigOutputNavKey, context);
  auto decisionOutput = outputNavigation.ptr();
  TrigCompositeUtils::newDecisionIn(decisionOutput, "HLTPassRaw"); // we rely on the fact that the 1st element is the top

  if (m_doLinkFeatures)
  {
    ATH_CHECK(fillRelevantFeatures(convProxies, *run2NavigationPtr));
    ATH_CHECK(fillRelevantRois(convProxies, *run2NavigationPtr));
    ATH_CHECK(fillRelevantTracks(convProxies));
    ATH_MSG_DEBUG("Features to link found");
  }

  ATH_CHECK(createIMHNodes(convProxies, *decisionOutput, context));
  if (m_doSelfValidation)
  {
    ATH_CHECK(numberOfHNodesPerProxyNotExcessive(convProxies));
  }

  ATH_CHECK(createSFNodes(convProxies, *decisionOutput, m_finalTEIdsToChains, context));
  ATH_CHECK(createL1Nodes(convProxies, *decisionOutput, context));
  ATH_CHECK(linkFeaNode(convProxies, *decisionOutput, *run2NavigationPtr, context));
  ATH_CHECK(linkRoiNode(convProxies, *run2NavigationPtr));
  ATH_CHECK(linkTrkNode(convProxies, *run2NavigationPtr));
  ATH_MSG_DEBUG("Conversion done, from " << convProxies.size() << " elements to " << decisionOutput->size() << " elements");

  // dispose temporaries
  for (auto proxy : convProxies)
  {
    delete proxy;
  }

  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::extractTECtoChainMapping(TEIdToChainsMap_t &allTEs, TEIdToChainsMap_t &finalTEs) const
{
  // port chains iteration code from previous version
  ATH_CHECK(not m_configSvc->chains().empty());
  for (auto ptrChain : m_configSvc->chains())
  {
    std::string chainName = ptrChain->name();
    if (not m_chainsToSave.empty())
    {
      auto found = std::find(m_chainsToSave.begin(), m_chainsToSave.end(), chainName);
      if (found == m_chainsToSave.end())
      {
        continue;
      }
    }

    HLT::Identifier chainId = HLT::Identifier(chainName);
    ATH_MSG_DEBUG(" CHAIN name " << chainName << " CHAIN Id " << chainId);
    for (auto ptrHLTSignature : ptrChain->signatures())
    {
      for (auto ptrHLTTE : ptrHLTSignature->outputTEs())
      {
        unsigned int trigConfDataId = ptrHLTTE->id();
        allTEs[trigConfDataId].insert(chainId);
        if (ptrHLTSignature == ptrChain->signatures().back())
        {
          finalTEs[trigConfDataId].insert(chainId);
        }
      }
    }
  }
  ATH_MSG_DEBUG("Recognised " << allTEs.size() << " kinds of TEs and among them " << finalTEs.size() << " final types");
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::mirrorTEsStructure(ConvProxySet_t &convProxies, const HLT::TrigNavStructure &run2Nav) const
{

  // iterate over the TEs, for each make the ConvProxy and build connections
  std::map<const HLT::TriggerElement *, ConvProxy *> teToProxy;
  ATH_MSG_DEBUG("TrigNavStructure with " << run2Nav.getAllTEs().size() << " TEs acquired");
  for (auto te : run2Nav.getAllTEs())
  {
    // skip event seed node
    if (HLT::TrigNavStructure::isInitialNode(te))
      continue;
    auto proxy = new ConvProxy(te);
    convProxies.insert(proxy);
    teToProxy[te] = proxy;
    // add linking
    for (auto predecessor : HLT::TrigNavStructure::getDirectPredecessors(te))
    {
      ConvProxy *predecessorProxy = teToProxy[predecessor];
      if (predecessorProxy != nullptr)
      { // because we skip some
        proxy->parents.push_back(predecessorProxy);
        predecessorProxy->children.push_back(proxy);
      }
    }
  }

  if (m_doSelfValidation)
  {
    int counter = -1;
    for (auto proxy : convProxies)
    {
      counter++;
      ATH_MSG_WARNING("Proxy " << counter << " " << proxy->description() << "ptr " << proxy);
      for (auto p : proxy->children)
        ATH_MSG_DEBUG("Child ptr " << p);
      for (auto p : proxy->parents)
        ATH_MSG_DEBUG("Parent ptr " << p);

      for (auto p : proxy->parents)
      {
        for (auto pp : p->parents)
        {
          if (pp == proxy)
          {
            ATH_MSG_WARNING("Weird, proxy is in parents list of parents");
          }
        }
      }
      for (auto c : proxy->children)
      {
        for (auto cc : c->children)
        {
          if (cc == proxy)
          {
            ATH_MSG_WARNING("Weird, proxy is in children list of children");
          }
        }
      }
    }
  }

  ATH_MSG_DEBUG("Created " << convProxies.size() << " proxy objects");
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::associateChainsToProxies(ConvProxySet_t &convProxies, const TEIdToChainsMap_t &allTEs) const
{

  for (auto &ptrConvProxy : convProxies)
  {
    auto teId = ptrConvProxy->te->getId();
    bool teActive = ptrConvProxy->te->getActiveState();
    auto iter = allTEs.find(teId);
    if (iter != allTEs.end())
    {
      ptrConvProxy->runChains.insert(iter->second.begin(), iter->second.end());
      if (teActive)
      {
        ptrConvProxy->passChains.insert(iter->second.begin(), iter->second.end());
      }
    }

    for (auto &objTeIdToChain : allTEs)
    {
      if (teId == objTeIdToChain.first)
      {
        for (auto &chainId : objTeIdToChain.second)
        {
          (ptrConvProxy->runChains).insert(chainId);
        }
        break;
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::cureUnassociatedProxies(ConvProxySet_t &convProxies) const
{
  // propagate up (towards L1) chain IDs if they are not in proxies
  // technically each proxy looks at the children proxies and inserts from it all unseen chains
  // procedure is repeated until, no single proxy needs an update (tedious - we may be smarter in future)

  while (true)
  {
    size_t numberOfUpdates = 0;
    for (auto p : convProxies)
    {
      for (auto child : p->children)
      {
        size_t startSize = p->runChains.size();
        p->runChains.insert(std::begin(child->runChains), std::end(child->runChains));

        if (startSize != p->runChains.size())
        { // some chain needed to be inserted
          numberOfUpdates++;
          // if update was need, it means set of chains that passed need update as well
          p->passChains.insert(std::begin(child->runChains), std::end(child->runChains));
        }
      }
    }
    ATH_MSG_DEBUG("Needed to propagate chains from " << numberOfUpdates << " child(ren)");
    if (numberOfUpdates == 0)
    {
      break;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::removeUnassociatedProxies(ConvProxySet_t &convProxies) const
{

  auto rem = [](auto &collection, auto element)
  {
    collection.erase(std::remove(collection.begin(), collection.end(), element), collection.end());
  };
  // remove proxies that have no chains
  for (auto i = std::begin(convProxies); i != std::end(convProxies);)
  {
    if ((*i)->runChains.empty())
    {
      const ConvProxy *toDel = *i;
      // remove it from parents/children
      for (auto parent : toDel->parents)
      {
        rem(parent->children, toDel);
      }
      for (auto child : toDel->children)
      {
        rem(child->parents, toDel);
      }
      delete toDel;
      i = convProxies.erase(i);
    }
    else
    {
      ++i;
    }
  }
  ATH_MSG_DEBUG("After eliminating proxies not associated to chainsof intereset left with " << convProxies.size());
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::doCompression(ConvProxySet_t &convProxies, const HLT::TrigNavStructure &run2Nav) const
{

  ATH_CHECK(collapseFeaturesProxies(convProxies, run2Nav));
  ATH_CHECK(collapseFeaturelessProxies(convProxies));
  if (m_doSelfValidation)
  {
    ATH_CHECK(allProxiesHaveChain(convProxies));
    ATH_CHECK(allProxiesConnected(convProxies));
  }
  ATH_MSG_DEBUG("Compression done");

  return StatusCode::SUCCESS;
}

template <typename MAP>
StatusCode Run2ToRun3TrigNavConverterV2::collapseProxies(ConvProxySet_t &convProxies, MAP &keyToProxyMap) const
{
  // collapse proxies based on the mapping in the map argument(generic) and clean proxiesSet
  std::vector<ConvProxy *> todelete;
  for (auto &[key, proxies] : keyToProxyMap)
  {
    if (proxies.size() > 1)
    {
      ATH_MSG_DEBUG("Merging " << proxies.size() << " similar proxies");
      for (auto p : proxies)
      {
        if (p->mergeAllowed(*proxies.begin()))
        {
          (*proxies.begin())->merge(p);
          todelete.push_back(p);
        }
        // TODO consider scanning proxies another time if merge is not allowed, it may be allowed with other proxies here
      }
    }
  }
  for (auto proxy : todelete)
  {
    convProxies.erase(proxy);
    delete proxy;
  }
  // remove from proxies set all elements that are now unassociated (remember to delete after)
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::collapseFeaturesProxies(ConvProxySet_t &convProxies, const HLT::TrigNavStructure &run2Nav) const
{

  const size_t beforeCount = convProxies.size();
  std::map<uint64_t, ConvProxySet_t> feaToProxyMap;
  for (auto proxy : convProxies)
  {
    proxy->feaHash = feaToHash(proxy->te->getFeatureAccessHelpers(), proxy->te, run2Nav);
    if (proxy->feaHash != ConvProxy::MissingFEA)
      feaToProxyMap[proxy->feaHash].insert(proxy);

    ATH_MSG_VERBOSE("TE " << TrigConf::HLTUtils::hash2string(proxy->te->getId()) << " FEA hash " << proxy->feaHash);
    for (auto fea : proxy->te->getFeatureAccessHelpers())
    {
      ATH_MSG_VERBOSE("FEA: " << fea);
    }
  }
  if (m_doSelfValidation)
  {
    for (auto [feaHash, proxies] : feaToProxyMap)
    {
      auto first = *proxies.begin();
      for (auto p : proxies)
      {
        if (not feaEqual(p->te->getFeatureAccessHelpers(), first->te->getFeatureAccessHelpers()))
        {
          ATH_MSG_ERROR("Proxies grouped by FEA hash have actually distinct features (specific FEAs are different)");
          return StatusCode::FAILURE;
        }
      }
    }
  }

  ATH_CHECK(collapseProxies(convProxies, feaToProxyMap));
  ATH_MSG_DEBUG("Proxies with features collapsing reduces size from " << beforeCount << " to " << convProxies.size());

  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::collapseFeaturelessProxies(ConvProxySet_t &convProxies) const
{
  // merge proxies bases on the parent child relation (this has to run after feature based collapsing)
  struct ParentChildCharacteristics
  {
    ConvProxy *parent = nullptr;
    ConvProxy *child = nullptr;
    size_t distanceFromParent = 0;
    bool operator<(const ParentChildCharacteristics &rhs) const
    {
      if (parent != rhs.parent)
        return parent < rhs.parent;
      if (child != rhs.child)
        return child < rhs.child;
      return distanceFromParent < rhs.distanceFromParent;
    }
  };
  const size_t beforeCount = convProxies.size();
  std::map<ParentChildCharacteristics, ConvProxySet_t> groupedProxies;
  for (auto proxy : convProxies)
  {
    if (proxy->feaHash == ConvProxy::MissingFEA)
    {
      ATH_MSG_VERBOSE("Featureless proxy to deal with: " << proxy->description());
      /* the canonical case
        merged parent
         / |  | \
        C1 C2 C3 C4 <-- proxies to merge
         \ |  | /
        merged child
      */
      if (proxy->children.size() == 1 and
          proxy->children[0]->feaHash != ConvProxy::MissingFEA and
          proxy->parents.size() == 1 and
          proxy->parents[0]->feaHash != ConvProxy::MissingFEA)
      {
        ATH_MSG_VERBOSE("Proxy to possibly merge: " << proxy->description());
        groupedProxies[{proxy->parents[0], proxy->children[0], 0}].insert(proxy);
        // TODO expand it to cover longer featureless sequences
      }
      else
      {
        ATH_MSG_VERBOSE("Featureless proxy in noncanonical situation " << proxy->description());
        ATH_MSG_VERBOSE("parents ");
        for (auto pp : proxy->parents)
        {
          ATH_MSG_VERBOSE(pp->description());
        }
        ATH_MSG_VERBOSE("children ");
        for (auto cp : proxy->children)
        {
          ATH_MSG_VERBOSE(cp->description());
        }
      }
    }
  }

  ATH_CHECK(collapseProxies(convProxies, groupedProxies));
  ATH_MSG_DEBUG("Proxies without features collapsing reduces size from " << beforeCount << " to " << convProxies.size());
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::fillRelevantFeatures(ConvProxySet_t &convProxies, const HLT::TrigNavStructure &run2Nav) const
{
  // from all FEAs of the associated TE pick those objects that are to be linked
  for (auto &proxy : convProxies)
  {
    if (proxy->te != nullptr)
    {

      for (HLT::TriggerElement::FeatureAccessHelper helper : proxy->te->getFeatureAccessHelpers())
      {
        auto [sgKey, sgCLID, sgName] = getSgKey(run2Nav, helper);
        if (sgKey != 0)
        {
          if (feaToSave(helper) == true)
          {
            proxy->features.push_back(helper);
          }
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::fillRelevantRois(ConvProxySet_t &convProxies, const HLT::TrigNavStructure &run2Nav) const
{
  for (auto &proxy : convProxies)
  {
    // TODO need check & handling of case when there is more RoIs, now overwriting
    if (HLT::TrigNavStructure::getRoINodes(proxy->te).size() > 1)
    {
      ATH_MSG_DEBUG("Several RoIs pointing to a proxy, taking latest one for now");
    }
    proxy->rois = getTEROIfeatures(proxy->te, run2Nav);
  }

  // roiPropagator
  std::function<void(std::vector<ConvProxy *> &, std::vector<HLT::TriggerElement::FeatureAccessHelper> &)> roiPropagator = [&](std::vector<ConvProxy *> &convProxyChildren, std::vector<HLT::TriggerElement::FeatureAccessHelper> &roiParent)
  {
    for (auto &proxyChild : convProxyChildren)
    {
      if (proxyChild->rois.empty())
      { // no roi update, copy from parent
        proxyChild->rois = roiParent;
        if (proxyChild->children.empty() == false)
        {
          roiPropagator(proxyChild->children, roiParent);
        }
      }
    }
  };

  for (auto &proxy : convProxies)
  {
    roiPropagator(proxy->children, proxy->rois);
  }

  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::fillRelevantTracks(ConvProxySet_t &convProxies) const
{
  for (auto &proxy : convProxies)
  {
    proxy->tracks = getTRACKfeatures(proxy->te);
  }

  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::createIMHNodes(ConvProxySet_t &convProxies, xAOD::TrigCompositeContainer &decisions, const EventContext &context) const
{
  for (auto &proxy : convProxies)
  {
    proxy->imNode = TrigCompositeUtils::newDecisionIn(&decisions, TrigCompositeUtils::inputMakerNodeName()); // IM
    for (auto chainId : proxy->runChains)
    {
      TrigCompositeUtils::addDecisionID(chainId, proxy->imNode);
    }
    proxy->hNode.push_back(TrigCompositeUtils::newDecisionIn(&decisions, TrigCompositeUtils::hypoAlgNodeName())); // H
    for (auto chainId : proxy->passChains)
    {
      TrigCompositeUtils::addDecisionID(chainId, proxy->hNode.back());
    }

    TrigCompositeUtils::linkToPrevious(proxy->hNode.front(), proxy->imNode, context); // H low IM up
  }
  // connecting current IM to all Hs in parent proxies
  for (auto &proxy : convProxies)
  {
    for (auto &parentProxy : proxy->parents)
    {
      TrigCompositeUtils::linkToPrevious(proxy->imNode, parentProxy->hNode.front(), context); // IM low H up (in parent)
    }
  }
  ATH_MSG_DEBUG("IM & H nodes made, output nav elements " << decisions.size());
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::createSFNodes(const ConvProxySet_t &convProxies, xAOD::TrigCompositeContainer &decisions,
                                                       const TEIdToChainsMap_t &terminalIds, const EventContext &context) const
{
  // make node & link it properly
  auto makeSFNode = [&decisions, &context](auto lastDecisionNode, auto chainIds)
  {
    auto sfNode = TrigCompositeUtils::newDecisionIn(&decisions);
    sfNode->setName("SF");
    TrigCompositeUtils::linkToPrevious(decisions.at(0), sfNode, context);
    TrigCompositeUtils::linkToPrevious(sfNode, lastDecisionNode, context);
    for (auto chainId : chainIds)
    {
      TrigCompositeUtils::addDecisionID(chainId, sfNode);
      TrigCompositeUtils::addDecisionID(chainId, decisions.at(0));
    }

    return sfNode;
  };

  for (auto proxy : convProxies)
  {
    // associate terminal nodes to filter nodes,
    if (proxy->children.empty())
    { // the H modes are terminal
      if (proxy->hNode.empty())
      { // nothing has passed, so link to the IM node
        // TODO make sure it needs to be done like that
        makeSFNode(proxy->imNode, proxy->runChains);
      }
      else
      {
        // makeSFNode(proxy->hNode[0], TCU::decisionIDs(proxy->hNode[0])); // not using passChains as there may be additional filtering
        for (auto &hNode : proxy->hNode)
        {
          makeSFNode(hNode, proxy->passChains); // using passChains
        }
      }
    }
    else
    {
      // likely need more iterations
      // nonterminal nodes that are nevertheless terminal for a given chain
      std::vector<TCU::DecisionID> toRetain;
      for (auto teId : proxy->teIDs)
      {
        auto whereInMap = terminalIds.find(teId);
        if (whereInMap != terminalIds.end())
        {
          toRetain.insert(toRetain.begin(), whereInMap->second.begin(), whereInMap->second.end());
        }
      }
      if (not toRetain.empty())
      {
        makeSFNode(proxy->imNode, toRetain);
      }
    }
  }
  // associate all nodes designated as final one with the filter nodes

  ATH_MSG_DEBUG("SF nodes made, output nav elements " << decisions.size());
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::createL1Nodes(const ConvProxySet_t &convProxies, xAOD::TrigCompositeContainer &decisions,
                                                       const EventContext &context) const
{

  auto makeL1Node = [&decisions, &context](auto firstDecisionNode, auto chainIds)
  {
    auto L1Node = TrigCompositeUtils::newDecisionIn(&decisions);
    L1Node->setName(TrigCompositeUtils::hltSeedingNodeName()); // L1
    for (auto chainId : chainIds)
    {
      TrigCompositeUtils::addDecisionID(chainId, L1Node);
    }
    TrigCompositeUtils::linkToPrevious(firstDecisionNode, L1Node, context); // IM -> L1

    return L1Node;
  };

  for (auto &proxy : convProxies)
  {
    // associate initial node to filter nodes,
    if (proxy->parents.empty())
    {                                                                             // the IM node is initial
      proxy->l1Node = makeL1Node(proxy->imNode, TCU::decisionIDs(proxy->imNode)); // not using passChains as there may be additional filtering
    }
  }

  ATH_MSG_DEBUG("L1 nodes made, output nav elements ");
  return StatusCode::SUCCESS;
}

std::size_t Run2ToRun3TrigNavConverterV2::getFeaSize(const ConvProxy &proxy) const
{
  size_t feaCount{0};
  if (proxy.features.empty())
  { // no features
    ++feaCount;
  }
  for (const auto &fea : proxy.features)
  {
    if (fea.getIndex().objectsBegin() == fea.getIndex().objectsEnd())
    {
      ++feaCount;
    }
    for (auto n = fea.getIndex().objectsBegin(); n < fea.getIndex().objectsEnd(); ++n)
    {
      ++feaCount;
    }
  }
  // 1 means a deafult H node created is enough, no need to expand H nodes
  return feaCount;
}

StatusCode Run2ToRun3TrigNavConverterV2::linkFeaNode(ConvProxySet_t &convProxies, xAOD::TrigCompositeContainer &decisions, const HLT::TrigNavStructure &run2Nav, const EventContext &context) const
{
  // from all FEAs of the associated TE pick those objects that are to be linked
  for (const auto &proxy : convProxies)
  {
    auto feaN = getFeaSize(*proxy);
    if (feaN > 1)
    { // expand for more H nodes and connect them
      while (--feaN)
      {
        proxy->hNode.push_back(TrigCompositeUtils::newDecisionIn(&decisions, TrigCompositeUtils::hypoAlgNodeName())); // H
        // connecting to upeer IM node
        TrigCompositeUtils::linkToPrevious(proxy->hNode.back(), proxy->imNode, context); // H low IM up
        // connecting created H to IM in children proxies
        for (auto &childProxy : proxy->children)
        {
          TrigCompositeUtils::linkToPrevious(childProxy->imNode, proxy->hNode.back(), context); // IM child H up (just created))
        }
      }
    }

    if (proxy->features.empty())
    { // no features attached, self link
      ElementLink<xAOD::TrigCompositeContainer> linkToSelf = TrigCompositeUtils::decisionToElementLink(proxy->hNode.front(), context);
      proxy->hNode.front()->setObjectLink<xAOD::TrigCompositeContainer>(TrigCompositeUtils::featureString(), linkToSelf);
    }

    auto hNodeIter = proxy->hNode.begin();
    for (auto &fea : proxy->features)
    {
      auto [sgKey, sgCLID, sgName] = getSgKey(run2Nav, fea);
      if (fea.getIndex().objectsBegin() == fea.getIndex().objectsEnd())
      {
        ElementLink<xAOD::TrigCompositeContainer> linkToSelf = TrigCompositeUtils::decisionToElementLink(*hNodeIter, context);
        (*hNodeIter)->setObjectLink<xAOD::TrigCompositeContainer>(TrigCompositeUtils::featureString(), linkToSelf);
        ++hNodeIter;
      }
      for (auto n = fea.getIndex().objectsBegin(); n < fea.getIndex().objectsEnd(); ++n)
      {
        // connecting feature
        (*hNodeIter)->typelessSetObjectLink(TrigCompositeUtils::featureString(), sgKey, sgCLID, n, n + 1);
        ++hNodeIter;
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::linkRoiNode(ConvProxySet_t &convProxies, const HLT::TrigNavStructure &run2Nav) const
{
  // from all Rois of the associated TE pick those objects that are to be linked
  for (auto &proxy : convProxies)
  {
    for (auto &roi : proxy->rois)
    {
      auto [sgKey, sgCLID, sgName] = getSgKey(run2Nav, roi);
      if (proxy->l1Node)
      {
        proxy->l1Node->typelessSetObjectLink(TrigCompositeUtils::initialRoIString(), sgKey, sgCLID, roi.getIndex().objectsBegin());
      }
      if (proxy->rois.empty() == false)
      {
        proxy->imNode->typelessSetObjectLink(TrigCompositeUtils::roiString(), sgKey, sgCLID, roi.getIndex().objectsBegin());
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::linkTrkNode(ConvProxySet_t &convProxies, const HLT::TrigNavStructure &run2Nav) const
{

  for (auto &proxy : convProxies)
  {
    for (auto &trk : proxy->tracks)
    {
      if (proxy->imNode->hasObjectLink(TrigCompositeUtils::roiString()))
      {
        ElementLink<TrigRoiDescriptorCollection> ROIElementLink = proxy->imNode->objectLink<TrigRoiDescriptorCollection>(TrigCompositeUtils::roiString());
        if (ROIElementLink.isValid())
        {
          SG::AuxElement::Decorator<ElementLink<TrigRoiDescriptorCollection>> viewBookkeeper("viewIndex");
          auto [sgKey, sgCLID, sgName] = getSgKey(run2Nav, trk);
          if (sgCLID == m_TrackParticleContainerCLID || sgCLID == m_TauTrackContainerCLID)
          {
            const char *tName = sgCLID == m_TrackParticleContainerCLID ? "TEMP_TRACKS" : "TEMP_TAU_TRACKS";
            auto d = std::make_unique<TrigCompositeUtils::Decision>();
            d->makePrivateStore();
            d->typelessSetObjectLink(tName, sgKey, sgCLID, trk.getIndex().objectsBegin());
            if (sgCLID == m_TrackParticleContainerCLID)
            {
              for (const ElementLink<xAOD::TrackParticleContainer> &track : d->objectCollectionLinks<xAOD::TrackParticleContainer>(tName))
              {
                if (track.isValid())
                {
                  const xAOD::TrackParticle *t = *track;
                  viewBookkeeper(*t) = ROIElementLink;
                }
              }
            }
            if (m_includeTauTrackFeatures == false && sgCLID == m_TauTrackContainerCLID)
            {
              for (const ElementLink<xAOD::TauTrackContainer> &track : d->objectCollectionLinks<xAOD::TauTrackContainer>(tName))
              {
                if (track.isValid())
                {
                  const xAOD::TauTrack_v1 *t = *track;
                  viewBookkeeper(*t) = ROIElementLink;
                }
              }
            }
          }
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

// does not need to be a method, so kept as local
bool feaToSkip(const HLT::TriggerElement::FeatureAccessHelper &fea)
{
  CLID thePassBitsCLID = ClassID_traits<xAOD::TrigPassBits>::ID();
  CLID thePassBitsContCLID = ClassID_traits<xAOD::TrigPassBitsContainer>::ID();

  return fea.getCLID() == thePassBitsCLID or fea.getCLID() == thePassBitsContCLID;
}

uint64_t Run2ToRun3TrigNavConverterV2::feaToHash(const std::vector<HLT::TriggerElement::FeatureAccessHelper> &feaVector, const HLT::TriggerElement *te_ptr, const HLT::TrigNavStructure &navigationDecoder) const
{
  // FEA vectors hashing

  uint64_t hash = 0;
  for (auto fea : feaVector)
  {
    if (feaToSkip(fea))
    {
      ATH_MSG_VERBOSE("Skipping TrigPassBits in FEA hash calculation");
      continue;
    }

    auto [sgKey, sgCLID, sgName] = getSgKey(navigationDecoder, fea);

    if (sgKey == 0)
    {
      ATH_MSG_VERBOSE("Skipping TrigPassBits in FEA hash calculation - CLID not present: " << sgCLID);
      continue;
    }

    ATH_MSG_DEBUG("feature CLID: " << fea.getCLID() << " te Id: " << te_ptr->getId());
    uint64_t repr64 = static_cast<uint64_t>(fea.getCLID()) << 32 | static_cast<uint64_t>(fea.getIndex().subTypeIndex()) << 24 | (fea.getIndex().objectsBegin() << 16 ^ fea.getIndex().objectsEnd());
    hash ^= repr64;
  }
  return hash;
}

bool Run2ToRun3TrigNavConverterV2::feaEqual(const std::vector<HLT::TriggerElement::FeatureAccessHelper> &a,
                                            const std::vector<HLT::TriggerElement::FeatureAccessHelper> &b) const
{
  if (a.size() != b.size())
    return false;

  for (size_t i = 0; i < a.size(); ++i)
  {
    if (feaToSkip(a[i]) and feaToSkip(b[i]))
    {
      ATH_MSG_VERBOSE("Skipping TrigPassBits in FEA comparison");
      continue;
    }
    if (not(a[i] == b[i]))
      return false;
  }
  return true;
}

bool Run2ToRun3TrigNavConverterV2::feaToSave(const HLT::TriggerElement::FeatureAccessHelper &fea) const
{
  if (m_collectionsToSaveDecoded.find(fea.getCLID()) != m_collectionsToSaveDecoded.end())
  {
    return true; // feature accepted for saving
  }

  return false;
}

bool Run2ToRun3TrigNavConverterV2::roiToSave(const HLT::TrigNavStructure &run2Nav, const HLT::TriggerElement::FeatureAccessHelper &roi) const
{
  auto [sgKey, sgCLID, sgName] = getSgKey(run2Nav, roi);
  if (std::find(m_setRoiName.begin(), m_setRoiName.end(), sgName) != m_setRoiName.end())
  {
    return true;
  }

  return false;
}

StatusCode Run2ToRun3TrigNavConverterV2::allProxiesHaveChain(const ConvProxySet_t &proxies) const
{
  for (auto p : proxies)
  {
    if (p->runChains.empty())
    {
      ATH_MSG_ERROR("Proxy with no chains");
      return StatusCode::FAILURE;
    }
  }
  ATH_MSG_DEBUG("CHECK OK, no proxies w/o a chain");
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::allProxiesConnected(const ConvProxySet_t &proxies) const
{
  for (auto p : proxies)
  {
    if (p->children.empty() and p->parents.empty())
    {
      ATH_MSG_ERROR("Orphanted proxy");
      return StatusCode::FAILURE;
    }
  }
  ATH_MSG_DEBUG("CHECK OK, no orphanted proxies");
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::numberOfHNodesPerProxyNotExcessive(const ConvProxySet_t &) const
{
  ATH_MSG_DEBUG("CHECK OK, no excessive number of H nodes per proxy");
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverterV2::noUnconnectedHNodes(const xAOD::TrigCompositeContainer &decisions) const
{
  // build map of all links to H nodes from IMs and FS
  std::set<const TrigCompositeUtils::Decision *> linkedHNodes;
  for (auto d : decisions)
  {
    if (d->name() == "IM" or d->name() == "FS")
    {
      for (auto el : TCU::getLinkToPrevious(d))
      {
        linkedHNodes.insert(*el); // dereferences to bare pointer
      }
    }
  }
  for (auto d : decisions)
  {
    if (d->name() == "H")
    {
      if (linkedHNodes.count(d) == 0)
      {
        ATH_MSG_ERROR("Orphanted H node");
        return StatusCode::FAILURE;
      }
    }
  }
  ATH_MSG_DEBUG("CHECK OK, all H modes are connected");

  return StatusCode::SUCCESS;
}

std::tuple<uint32_t, CLID, std::string> Run2ToRun3TrigNavConverterV2::getSgKey(const HLT::TrigNavStructure &navigationDecoder, const HLT::TriggerElement::FeatureAccessHelper &helper) const
{
  const std::string hltLabel = navigationDecoder.label(helper.getCLID(), helper.getIndex().subTypeIndex());

  const CLID saveCLID = [&](const CLID &clid)
  {
    if (clid == m_roIDescriptorCLID)
      return m_roIDescriptorCollectionCLID;
    if (clid == m_TrigEMClusterCLID)
      return m_TrigEMClusterContainerCLID;
    if (clid == m_TrigRingerRingsCLID)
      return m_TrigRingerRingsContainerCLID;
    return clid;
  }(helper.getCLID());

  std::string type_name;
  if (m_clidSvc->getTypeNameOfID(saveCLID, type_name).isFailure())
  {
    return {0, 0, ""};
  }

  const auto sgStringKey = HLTNavDetails::formatSGkey("HLT", type_name, hltLabel);
  const bool isAvailable = evtStore()->contains(saveCLID, sgStringKey);
  ATH_MSG_DEBUG(" Objects presence " << helper << " " << sgStringKey << (isAvailable ? " present" : " absent"));
  if (!isAvailable)
  {
    return {0, saveCLID, ""};
  }

  return {evtStore()->stringToKey(sgStringKey, saveCLID), saveCLID, hltLabel}; // sgKey, sgCLID, sgName
}

const std::vector<HLT::TriggerElement::FeatureAccessHelper> Run2ToRun3TrigNavConverterV2::getTEROIfeatures(const HLT::TriggerElement *te_ptr, const HLT::TrigNavStructure &navigationDecoder) const
{
  // ordered_sorter
  auto ordered_sorter = [&](const auto &left, const auto &right) -> bool
  {
    return std::find(cbegin(m_setRoiName), cend(m_setRoiName), left) < std::find(cbegin(m_setRoiName), cend(m_setRoiName), right);
  };

  std::map<std::string, HLT::TriggerElement::FeatureAccessHelper, decltype(ordered_sorter)> mp(ordered_sorter);

  for (HLT::TriggerElement::FeatureAccessHelper helper : te_ptr->getFeatureAccessHelpers())
  {
    auto [sgKey, sgCLID, sgName] = getSgKey(navigationDecoder, helper);
    if (std::find(m_setRoiName.begin(), m_setRoiName.end(), sgName) == m_setRoiName.end())
    {
      // do not filter continue;
      continue;
    }
    mp[sgName] = helper;
  }

  std::vector<HLT::TriggerElement::FeatureAccessHelper> ptrFAHelper;
  std::transform(cbegin(mp), cend(mp), back_inserter(ptrFAHelper),
                 [](const std::map<std::string, HLT::TriggerElement::FeatureAccessHelper>::value_type &p)
                 { return p.second; });

  return ptrFAHelper;
}

const std::vector<HLT::TriggerElement::FeatureAccessHelper> Run2ToRun3TrigNavConverterV2::getTRACKfeatures(const HLT::TriggerElement *te_ptr) const
{
  std::vector<HLT::TriggerElement::FeatureAccessHelper> ptrFAHelper;
  for (HLT::TriggerElement::FeatureAccessHelper helper : te_ptr->getFeatureAccessHelpers())
  {
    if (helper.getCLID() == m_TrackParticleContainerCLID || helper.getCLID() == m_TauTrackContainerCLID)
    {
      ptrFAHelper.push_back(helper);
    }
  }
  return ptrFAHelper;
}
