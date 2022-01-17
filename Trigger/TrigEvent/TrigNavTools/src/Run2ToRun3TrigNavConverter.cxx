/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "Run2ToRun3TrigNavConverter.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigNavStructure/TriggerElement.h"
#include "xAODTrigger/TriggerMenu.h"
#include "xAODTrigger/TriggerMenuContainer.h"
#include "TrigConfHLTUtils/HLTUtils.h"
#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"
#include <AsgTools/AsgMetadataTool.h>
#include "TrigNavStructure/StandaloneNavigation.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODTracking/versions/TrackParticle_v1.h"
#include "xAODTracking/TrackParticleContainer.h"
#include <functional>
#include <random>


Run2ToRun3TrigNavConverter::Run2ToRun3TrigNavConverter(const std::string& name, ISvcLocator* pSvcLocator) : AthReentrantAlgorithm(name, pSvcLocator){}

Run2ToRun3TrigNavConverter::~Run2ToRun3TrigNavConverter(){}

StatusCode Run2ToRun3TrigNavConverter::initialize() {

  ATH_CHECK(m_trigNavWriteKey.initialize());
  ATH_CHECK(m_configSvc.retrieve());
  ATH_CHECK(m_clidSvc.retrieve());
  ATH_CHECK(m_tdt.empty() != m_trigNavKey.key().empty()); //either of the two has to be enabled but not both
  if (!m_tdt.empty()) {
    ATH_CHECK(m_tdt.retrieve());
    ATH_MSG_INFO("Will use Trigger Navigation from TrigDecisionTool");
  }
  else {
    ATH_CHECK(m_trigNavKey.initialize(SG::AllowEmpty));
    ATH_MSG_INFO("Will use Trigger Navigation decoded from TrigNavigation object");
  }
  // retrievig CLID from names and storing to set
  for (const auto& name : m_collectionsToSave)
  {
    CLID id{ 0 };
    ATH_CHECK(m_clidSvc->getIDOfTypeName(name, id));
    m_setCLID.insert(id);
  }

  //m_chainsToSave - m_setChainName
  m_setChainName.insert(begin(m_chainsToSave), end(m_chainsToSave));

  //m_roisToSave - m_setRoiName
  for (const auto& name : m_roisToSave) {
    m_setRoiName.push_back(name);
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
  ATH_CHECK(m_clidSvc->getIDOfTypeName("xAOD::TrackParticleAuxContainer", m_TrackParticleAuxContainerCLID));
  ATH_CHECK(m_clidSvc->getIDOfTypeName("xAOD::TauTrackContainer", m_TauTrackContainerCLID));
  ATH_CHECK(m_clidSvc->getIDOfTypeName("xAOD::TauTrackAuxContainer", m_TauTrackAuxContainerCLID));

  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverter::finalize() {
  return StatusCode::SUCCESS;
}


StatusCode Run2ToRun3TrigNavConverter::execute(const EventContext& context) const
{
  #define FEATURESELECTOR true 

  const HLT::TrigNavStructure* run2NavigationPtr = nullptr;
  HLT::StandaloneNavigation standaloneNav;
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


  // string identifier for TE
  std::map< int, std::string > hash2string;
  for (auto p : m_configSvc->chains()) {
    for (auto s_iter = p->signatures().begin(); s_iter != p->signatures().end(); ++s_iter) {
      for (auto te : (*s_iter)->outputTEs()) {
        int hash = TrigConf::HLTUtils::string2hash(te->name());
        hash2string[hash] = te->name();
      }
    }
  }

  if (m_onlyFeaturePriting)
    return printFeatures(run2Navigation);

  SG::WriteHandle<TrigCompositeUtils::DecisionContainer> outputNavigation = TrigCompositeUtils::createAndStore(m_trigNavWriteKey, context);
  auto decisionOutput = outputNavigation.ptr();
  TrigCompositeUtils::Decision* passRawOutput = TrigCompositeUtils::newDecisionIn(decisionOutput, "HLTPassRaw");

  if (m_doPrint) {
    std::string dump;
    run2Navigation.printASCIIArt(dump);
    ATH_MSG_INFO(dump);
  }

  // auxiliary random objects
  
  std::random_device rd;
  std::mt19937_64 gen(rd());
  std::uniform_int_distribution<uint32_t> dis;
  

  TEDecisionMap mapTEtoHNodes;                          // TE - Decision (xAOD::TrigComposite)
  TEDecisionMap mapTEtoIMNodes;                    // TE Active - Decision (xAOD::TrigComposite)
  std::vector<TrigCompositeUtils::Decision*> decisionLast; // storing "last" decision in a chain

  std::map<TrigCompositeUtils::Decision*, TrigCompositeUtils::Decision*> decisionLastSF; // map of key: "last H decision" value: SF
  DecisionObjMap decisionObj;
  #if FEATURESELECTOR==true
    DecisionObjMap decisionObjFeatureless;
  #else 
    DecisionObjStringMap decisionObjFeatureless;
  #endif
  
  L1ObjMap l1Obj;
  std::set<TrigCompositeUtils::Decision*> currentChainDecisions; // to veto decision nodes to be reused for the same chain

  std::vector<HLT::TriggerElement::FeatureAccessHelper> featureRoI; // for keeping predecessing TE with RoI

  // @@@@@@@@@@@@@@@@@@@@@@@@@@ getDecisionObject @@@@@@@@@@@@@@@@@@@@@@@@@@
  auto getDecisionObject = [&](const HLT::TriggerElement::FeatureAccessHelper& elemFE,
    size_t idx,
    size_t& kIn,
    HLT::TriggerElement* ptrTE = nullptr) {
      // kIn: flag with the information about the decision objects (DO)
      // 0: DO pair has been found and returned (no new pair created)
      // 1: H node created for a feature unit from collection, IM retrieved (not created)
      // 2: a new DO pair created (that is: H and IM node)

      auto [sgKey, sgCLID, sgName] = getSgKey(run2Navigation, elemFE);
      auto preExistingDecsionsIt = decisionObj.find(sgKey + idx);
      if (preExistingDecsionsIt != decisionObj.end() ) {
        auto [hNode, imNode] = preExistingDecsionsIt->second;
        if ( currentChainDecisions.count(hNode) == 0  and currentChainDecisions.count(imNode) == 0 ) {
          kIn = 0;
          ATH_MSG_DEBUG("Reusing = " << ptrTE << " IM " << imNode->index() << " H " << hNode->index());
          return preExistingDecsionsIt->second;
        }
      }

      kIn = 2;
      if (idx == elemFE.getIndex().objectsBegin()) {
        auto imNode = TrigCompositeUtils::newDecisionIn(decisionOutput); // IM
        currentChainDecisions.insert(imNode);
        imNode->setName(TrigCompositeUtils::inputMakerNodeName()); // for debugging: imNode->setName( hash2string[ ptrTE->getId() ] );
        auto hNode = TrigCompositeUtils::newDecisionIn(decisionOutput); // H
        currentChainDecisions.insert(hNode);
        hNode->setName(TrigCompositeUtils::hypoAlgNodeName());
        TrigCompositeUtils::linkToPrevious(hNode, imNode, context); // H low IM up
        ATH_MSG_DEBUG("TE FE link creator H lower IM upper = " << ptrTE << " " << hNode << " " << imNode);
        ATH_MSG_DEBUG("REGTEST " << "[TE feature] " << "[H creation] " << "(" << hNode->index() << ")");
        ATH_MSG_DEBUG("REGTEST " << "[TE feature] " << "[IM creation] " << "(" << imNode->index() << ")");
        ATH_MSG_DEBUG("REGTEST " << "[TE feature] " << "[H -> IM seed link] " << "(" << hNode->index() << " -> " << imNode->index() << ")");
        return std::make_pair(hNode, imNode);
      }

      kIn = 1;
      auto hNode = TrigCompositeUtils::newDecisionIn(decisionOutput); // H
      currentChainDecisions.insert(hNode);
      hNode->setName(TrigCompositeUtils::hypoAlgNodeName());
      auto imNode = (decisionObj.find(sgKey + elemFE.getIndex().objectsBegin())->second).second;
      ATH_MSG_DEBUG("TE FE link creator H lower IM upper = " << ptrTE << " " << hNode << " " << imNode);
      TrigCompositeUtils::linkToPrevious(hNode, imNode, context); // H low IM up
      ATH_MSG_DEBUG("REGTEST " << "[TE feature] " << "[H creation] " << "(" << hNode->index() << ")");
      ATH_MSG_DEBUG("REGTEST " << "[TE feature] " << "[H -> IM seed link] " << "(" << hNode->index() << " -> " << imNode->index() << ")");
      return std::make_pair(hNode, imNode);
  };

  // @@@@@@@@@@@@@@@@@@@@@@@@@@ getL1Object @@@@@@@@@@@@@@@@@@@@@@@@@@
  auto getL1Object = [&](const HLT::TriggerElement::FeatureAccessHelper& elemFE, size_t idx, TrigCompositeUtils::DecisionContainer* dOutput, size_t& kIn) {
    auto [sgKey, sgCLID, sgName] = getSgKey(run2Navigation, elemFE);
    auto it = l1Obj.find(sgKey + idx);
    if (it != l1Obj.end()) {
      kIn = 0;
      return it->second;
    }

    kIn = 1;
    auto d1 = TrigCompositeUtils::newDecisionIn(dOutput);
    d1->setName(TrigCompositeUtils::hltSeedingNodeName());
    return d1;
  };

  // @@@@@@@@@@@@@@@@@@@@@@@@@@ getSFObject @@@@@@@@@@@@@@@@@@@@@@@@@@
  auto getSFObject = [&](TrigCompositeUtils::Decision *dLast, TrigCompositeUtils::DecisionContainer *dOutput, size_t &kIn) -> TrigCompositeUtils::Decision *
  {
    auto pairLastSF = decisionLastSF.find(dLast);
    if (pairLastSF != decisionLastSF.end())
    { // last H node found
      kIn = 0;
      return pairLastSF->second; // return existing SF
    }

    kIn = 1;
    auto sf_decision = TrigCompositeUtils::newDecisionIn(dOutput);
    sf_decision->setName("SF");
    decisionLastSF[dLast] = sf_decision; // insert new last H - SF pair
    TrigCompositeUtils::linkToPrevious(sf_decision, dLast, context);

    return sf_decision;
  };

  // @@@@@@@@@@@@@@@@@@@@@@@@@@ ordered_sorter @@@@@@@@@@@@@@@@@@@@@@@@@@
  auto rois_priority_order = [&](const auto& left, const auto& right) {
    return std::find(cbegin(m_setRoiName), cend(m_setRoiName), left) < std::find(cbegin(m_setRoiName), cend(m_setRoiName), right);
  };


  // @@@@@@@@@@@@@@@@@@@@@@@@@@ begin of chain loop @@@@@@@@@@@@@@@@@@@@@@@@@@
  for (auto p : m_configSvc->chains())
  {
    if (m_convertAllChains || m_setChainName.find(p->name()) != m_setChainName.end())
    {
      std::string chainName = p->name();
      auto c = p;
      HLT::Identifier chainId = HLT::Identifier(chainName);
      currentChainDecisions.clear();

      ATH_MSG_DEBUG(" CHAIN name " << chainName);
      bool kTErejection{ false };
      for (auto s_iter = c->signatures().begin(), first_s_iter = s_iter; s_iter != c->signatures().end(); ++s_iter)
      {
        if (kTErejection) break;
        for (auto te : (*s_iter)->outputTEs())
        {
          std::vector<HLT::TriggerElement*> tes;
          run2Navigation.getAllOfType(te->id(), tes, false);
          ATH_MSG_DEBUG(" TE size " << tes.size() << " name " << hash2string[te->id()]);
          if (tes.empty()) {
            kTErejection = true;
            break;
          } else {
            decisionLast.clear();
          }

          for (auto teptr : tes)
          {
            std::vector<TrigCompositeUtils::Decision*> hNodesVector;

            auto vectorTEfeatures_ptr = getTEfeatures(teptr, run2Navigation);
            auto vectorTEROIfeatures_ptr = getTEROIfeatures(teptr, run2Navigation);
            auto vectorTRACKfeatures_ptr = getTRACKfeatures(teptr);

            std::map<std::string, HLT::TriggerElement::FeatureAccessHelper, decltype(rois_priority_order)> mp(rois_priority_order);

            // @@@@@@@@@@@@@@@@@@@@@@@@@@ roiFinder @@@@@@@@@@@@@@@@@@@@@@@@@@
            std::function<void(HLT::TriggerElement*)> roiFinder = [&](HLT::TriggerElement* ptrTE) {
              auto vecTEpred = run2Navigation.getDirectPredecessors(ptrTE);
              if (vecTEpred.empty()) {
                return nullptr;
              } else {
                for (auto pred : vecTEpred) { // vector predecessors TE
                  auto vectorTEROIfeatures_ptr = getTEROIfeatures(pred, run2Navigation);
                  if (vectorTEROIfeatures_ptr.empty() == false) {
                    auto [sgKey, sgCLID, sgName] = getSgKey(run2Navigation, vectorTEROIfeatures_ptr.front());
                    mp[sgName] = vectorTEROIfeatures_ptr.front();
                  }
                }
                if (mp.empty()) {
                  for (auto pred : vecTEpred) { // vector predecessors TE
                    roiFinder(pred);
                  }
                }
              }
              return nullptr;
            };

            // @@@@@@@@@@@@@@@@@@@@@@@@@@ featureFinder @@@@@@@@@@@@@@@@@@@@@@@@@@
            std::function<uint32_t(HLT::TriggerElement*, uint32_t&)> featureFinder = [&](HLT::TriggerElement* ptrTE, uint32_t& deepLvl) {
              auto vecTEpred = run2Navigation.getDirectPredecessors(ptrTE);
              if (vecTEpred.empty() == false) {
                for (auto pred : vecTEpred) { // vector predecessors TE
                  auto vectorTEfeatures_ptr = getTEfeatures(pred, run2Navigation /*, false */ ); // filtered feature collection, if any: uncomment false
                  if (vectorTEfeatures_ptr.size() == 1 && (vectorTEfeatures_ptr.front().getIndex().objectsBegin() == vectorTEfeatures_ptr.front().getIndex().objectsEnd())) {
                    break; // this is empty FE, treat it is as zero FE
                  }
                  if (vectorTEfeatures_ptr.empty() == false) {                  
                    auto featureBegin = 10000 * vectorTEfeatures_ptr.front().getIndex().objectsBegin();
                    auto [sgKey, sgCLID, sgName] = getSgKey(run2Navigation, vectorTEfeatures_ptr.front());
                    return sgKey + featureBegin + deepLvl; // first found 
                  }
                }
                // if no success after scan of TE predecessors we go one level up
                ++deepLvl;
                uint32_t keySearch{ 0 };
                for (auto pred : vecTEpred) {
                  keySearch = featureFinder(pred, deepLvl); // recursive call
                  if (keySearch != 0) return keySearch; // first found
                }
              }
              return (uint32_t)0; // no TE predecessor, no FE at all or recursive search failed
            };

            // @@@@@@@@@@@@@@@@@@@@@@@@@@ getTEObject @@@@@@@@@@@@@@@@@@@@@@@@@@
            auto getTEObject = [&](HLT::TriggerElement* ptrTE, uint32_t& deepLevel, TrigCompositeUtils::DecisionContainer* dOutput) -> DecisionPair& {
              // this is featureless case
              #if FEATURESELECTOR==false
              std::string teName = hash2string[ptrTE->getId()];
              if (teName != "") {
                auto it = decisionObjFeatureless.find(teName);
                if (it != decisionObjFeatureless.end()) return it->second;
              }

              deepLevel = 0; // in this option obsolte
              #endif

                // THIS BLOCK is left intetionally here as an alternative approach.
                #if FEATURESELECTOR==true
                uint32_t sgKeyProxy = featureFinder(ptrTE,deepLevel);
                if (sgKeyProxy != 0) {
                  auto it = decisionObjFeatureless.find(sgKeyProxy);
                  if (it != decisionObjFeatureless.end()) return it->second;
                }
                if (sgKeyProxy==0) {
                  ATH_MSG_DEBUG("ZERO sgKeyProxy");
                  sgKeyProxy = 1000000 + dis(gen); // let us random it
                }
                #endif

              auto d2 = TrigCompositeUtils::newDecisionIn(dOutput); // IM
              d2->setName(TrigCompositeUtils::inputMakerNodeName()); // debug: d2->setName( hash2string[ ptrTE->getId() ] );
              auto d1 = TrigCompositeUtils::newDecisionIn(dOutput); // H
              d1->setName(TrigCompositeUtils::hypoAlgNodeName());
              TrigCompositeUtils::linkToPrevious(d1, d2, context);
              ATH_MSG_DEBUG("TE link creator H lower IM upper = " << ptrTE << " " << d1 << " " << d2);
              ATH_MSG_DEBUG("REGTEST " << "[TE featureless] " << "[H creation] " << "(" << d1->index() << ")");
              ATH_MSG_DEBUG("REGTEST " << "[TE featureless] " << "[IM creation] " << "(" << d2->index() << ")");
              ATH_MSG_DEBUG("REGTEST " << "[TE featureless] " << "[H -> IM seed link] " << "(" << d1->index() << " -> " << d2->index() << ")");
              #if FEATURESELECTOR==true
                return decisionObjFeatureless[sgKeyProxy] = std::make_pair(d1, d2); 
              #else
                return decisionObjFeatureless[teName] = std::make_pair(d1, d2);
              #endif
            };







            if (vectorTEROIfeatures_ptr.empty()) {
              roiFinder(teptr);
            }
            else {
              auto [sgKey, sgCLID, sgName] = getSgKey(run2Navigation, vectorTEROIfeatures_ptr.front());
              mp[sgName] = vectorTEROIfeatures_ptr.front();
            }


            // @@@@@@@@@@@@@@@@@@@@@@@@@@ vector TE - EMPTY @@@@@@@@@@@@@@@@@@@@@@@@@@
            if (vectorTEfeatures_ptr.empty() || vectorTEfeatures_ptr[0].getIndex().objectsBegin() == vectorTEfeatures_ptr[0].getIndex().objectsEnd()) {


              uint32_t dLevel{ 1 };
              auto [hNode, imNode] = getTEObject(teptr, dLevel, decisionOutput); // H IM
              ATH_MSG_DEBUG("JUST AFTER " << "[TE] " << teptr << " [H] " << hNode->index() << " [IM] " << imNode->index() << " dLevel=" << dLevel);
              auto decision = imNode; // IM

              hNodesVector.push_back(hNode);

              if (teptr->getActiveState()) {
                TrigCompositeUtils::addDecisionID(chainId, hNode); // H
                ATH_MSG_DEBUG("REGTEST " << "[TE featureless] " << "[H Index] (" << hNode->index() << ") [Chain ID] " << chainId);
              }
              TrigCompositeUtils::addDecisionID(chainId, decision); // IM
              ATH_MSG_DEBUG("REGTEST " << "[TE featureless] " << "[IM Index] (" << decision->index() << ") [Chain ID] " << chainId);

              ElementLink<xAOD::TrigCompositeContainer> linkToSelf = TrigCompositeUtils::decisionToElementLink(hNode, context);
              hNode->setObjectLink<xAOD::TrigCompositeContainer>(TrigCompositeUtils::featureString(), linkToSelf);
              ATH_CHECK(insertDecisionToTEMap(hNode, teptr,  mapTEtoHNodes));
              ATH_CHECK(insertDecisionToTEMap(imNode, teptr, mapTEtoIMNodes));

              TrigCompositeUtils::Decision* l1_decision{ nullptr };
              if (s_iter == first_s_iter)
              {
                l1_decision = TrigCompositeUtils::newDecisionIn(decisionOutput);
                l1_decision->setName(TrigCompositeUtils::hltSeedingNodeName());
                TrigCompositeUtils::addDecisionID(chainId, l1_decision);
                TrigCompositeUtils::linkToPrevious(decision, l1_decision, context);
                ATH_MSG_DEBUG("REGTEST " << "[TE featureless] " << "[L1 creation] " << "(" << l1_decision->index() << ") [Chain ID] " << chainId);
                ATH_MSG_DEBUG("REGTEST " << "[TE featureless] " << "[IM -> L1 seed link] " << "(" << decision->index() << " -> " << l1_decision->index() << ")");
              }


              if (mp.empty() == false) {
                ATH_CHECK(addTEROIfeatures(run2Navigation, (mp.begin())->second, decision));
              }

              for (const auto& rNodes : HLT::TrigNavStructure::getRoINodes(teptr))
              {
                if (HLT::TrigNavStructure::isRoINode(rNodes))
                {
                  //////////////////auto vectorROIfeatures_ptr = getROIfeatures(rNodes, run2Navigation);
                  auto vectorROIfeatures_ptr = getTEROIfeatures(rNodes, run2Navigation);
                  for (auto featureRoI : vectorROIfeatures_ptr)
                  {
                    if (mp.empty()) {
                      ATH_CHECK(addTEROIfeatures(run2Navigation, featureRoI, decision)); // roi link to initialRoi
                    }
                  }

                  //### TRACKS ### check for track features and connect the same RoI
                  if (!vectorROIfeatures_ptr.empty())
                    for (const auto& elemTRACK : vectorTRACKfeatures_ptr)
                    {
                      for (size_t i{ elemTRACK.getIndex().objectsBegin() }; i < elemTRACK.getIndex().objectsEnd(); ++i)
                      {
                        ElementLink<TrigRoiDescriptorCollection> ROIElementLink = decision->objectLink<TrigRoiDescriptorCollection>(TrigCompositeUtils::roiString());
                        if (ROIElementLink.isValid())
                        {
                          ATH_CHECK(addTRACKfeatures(run2Navigation, elemTRACK, decision, ROIElementLink));
                        }
                      }
                    }
                }
              }
            }


            // @@@@@@@@@@@@@@@@@@@@@@@@@@ vector TE - FULL @@@@@@@@@@@@@@@@@@@@@@@@@@
            if (vectorTEfeatures_ptr.empty() == false &&
              vectorTEfeatures_ptr[0].getIndex().objectsBegin() < vectorTEfeatures_ptr[0].getIndex().objectsEnd())
            {

              for (const auto& elemFE : vectorTEfeatures_ptr)
              {
                TrigCompositeUtils::Decision* decision{ nullptr };

                for (size_t objectIndex{ elemFE.getIndex().objectsBegin() }; objectIndex < elemFE.getIndex().objectsEnd(); ++objectIndex)
                {
                  size_t kInsert{ 0 };
                  auto [hNode, imNode] = getDecisionObject(elemFE, objectIndex, kInsert, teptr); // H IM
                  decision = imNode; // IM

                  if (mp.empty() == false) {
                    ATH_CHECK(addTEROIfeatures(run2Navigation, (mp.begin())->second, imNode)); // updated roi on the way
                  }


                  if (teptr->getActiveState()) {
                    TrigCompositeUtils::addDecisionID(chainId, hNode); // H
                    ATH_MSG_DEBUG("REGTEST " << "[TE feature] " << "[H Index] (" << hNode->index() << ") [Chain ID] " << chainId);
                  }
                  TrigCompositeUtils::addDecisionID(chainId, imNode); // IM
                  ATH_MSG_DEBUG("REGTEST " << "[TE feature] " << "[IM Index] (" << imNode->index() << ") [Chain ID] " << chainId);


                  hNodesVector.push_back(hNode);

                  if (kInsert)
                  {
                    auto mp = std::make_pair(hNode, imNode);
                    ATH_CHECK(addTEfeatures(run2Navigation, elemFE, mp, objectIndex, &decisionObj));
                  }
                  ATH_CHECK(insertDecisionToTEMap(hNode, teptr, mapTEtoHNodes));
                  ATH_CHECK(insertDecisionToTEMap(imNode, teptr, mapTEtoIMNodes));
                }

                for (const auto& rNodes : HLT::TrigNavStructure::getRoINodes(teptr))
                {
                  if (HLT::TrigNavStructure::isRoINode(rNodes))
                  {
                    ///////////////auto vectorROIfeatures_ptr = getROIfeatures(rNodes,run2Navigation);
                    auto vectorROIfeatures_ptr = getTEROIfeatures(rNodes, run2Navigation);
                    for (auto featureRoI : vectorROIfeatures_ptr)
                    {
                      if (s_iter == first_s_iter)
                      {
                        size_t kInsert{ 0 };
                        auto l1_decision = getL1Object(featureRoI, featureRoI.getIndex().objectsBegin(), decisionOutput, kInsert);
                        TrigCompositeUtils::addDecisionID(chainId, l1_decision);
                        if (kInsert)
                        {
                          ATH_CHECK(addROIfeatures(run2Navigation, featureRoI, l1_decision, -1, &l1Obj)); // test coding for initialRoi
                        }
                        // check for updated roi
                        if (mp.empty()) {
                          ATH_CHECK(addTEROIfeatures(run2Navigation, featureRoI, decision)); // roi link to initialRoi
                        }
                        TrigCompositeUtils::linkToPrevious(decision, l1_decision, context);
                        ATH_MSG_DEBUG("REGTEST " << "[TE feature] " << "[L1 creation] " << "(" << l1_decision->index() << ") [Chain ID] " << chainId);
                        ATH_MSG_DEBUG("REGTEST " << "[TE feature] " << "[IM -> L1 seed link] " << "(" << decision->index() << " -> " << l1_decision->index() << ")");
                      }
                      else
                      {
                        // check for updated roi
                        if (mp.empty()) {
                          ATH_CHECK(addTEROIfeatures(run2Navigation, featureRoI, decision)); // roi link to initialRoi
                        }
                      }
                    }
                    //### TRACKS ### check for track features and connect the same RoI
                    if (!vectorROIfeatures_ptr.empty())
                      for (const auto& elemTRACK : vectorTRACKfeatures_ptr)
                      {
                        for (size_t i{ elemTRACK.getIndex().objectsBegin() }; i < elemTRACK.getIndex().objectsEnd(); ++i)
                        {
                          if (mp.empty()) {
                            ElementLink<TrigRoiDescriptorCollection> ROIElementLink = decision->objectLink<TrigRoiDescriptorCollection>(TrigCompositeUtils::roiString());
                            if (ROIElementLink.isValid())
                            {
                              ATH_CHECK(addTRACKfeatures(run2Navigation, elemTRACK, decision, ROIElementLink));
                            }
                          }
                        }
                      }
                  }
                }
              }
            } // eof if - TE full

            decisionLast.insert(decisionLast.end(), hNodesVector.begin(), hNodesVector.end());
          }
        }
      }

      size_t kInsert{0};
      if (decisionLast.empty() == false) {
        auto last = decisionLast.back();
        auto sf_decision = getSFObject(last, decisionOutput, kInsert);

        TrigCompositeUtils::addDecisionID(chainId, sf_decision);
        if (kInsert) { // link if new SF created
          TrigCompositeUtils::linkToPrevious(passRawOutput, sf_decision, context);
          ATH_MSG_DEBUG("REGTEST " << "[SF creation] " << "(" << sf_decision->index() << ") [Chain ID] " << chainId);
          ATH_MSG_DEBUG("REGTEST " << "[SF -> H seed link] " << "(" << sf_decision->index() << " -> " << last->index() << ")");
          ATH_MSG_DEBUG("REGTEST " << "[RAW -> SF seed link] " << "(" << passRawOutput->index() << " -> " << sf_decision->index() << ")");
        }
      }

      TrigCompositeUtils::decisionIDs(passRawOutput).push_back(chainId);
    }
  }


  for (auto& [teptr, decisions] : mapTEtoIMNodes) {
    if (decisions.size() == 1) {
      ATH_MSG_DEBUG("TE IMs = " << teptr << " IM Index=" << decisions[0]->index() << " " << decisions);
    }
    else {
      ATH_MSG_DEBUG("TE IMs = " << teptr << " IM " << decisions);
      for (const auto& p : decisions) ATH_MSG_DEBUG("TE IMs ... IM Index=" << p->index());
    }
  }

  for (auto& [teptr, decisions] : mapTEtoHNodes) {
    if (decisions.size() == 1) {
      ATH_MSG_DEBUG("TE Hs = " << teptr << " H Index=" << decisions[0]->index() << " " << decisions);
    }
    else {
      ATH_MSG_DEBUG("TE Hs = " << teptr << " H " << decisions);
      for (const auto& p : decisions) ATH_MSG_DEBUG("TE Hs ... H Index=" << p->index());
    }
  }

  for (auto& [teptr, decisions] : mapTEtoIMNodes) {
    for (auto prep_ptr : run2Navigation.getDirectPredecessors(teptr)) {
      ATH_MSG_DEBUG("TE predTE (from IM) = " << teptr << " " << prep_ptr);
    }
  }

  std::set<std::string> regtestSorted;
  for (auto& [tePtr, imNodes] : mapTEtoIMNodes)
  { // loop over all IM nodes

    for (auto predtePtr : run2Navigation.getDirectPredecessors(tePtr))
    {
      if (tePtr != predtePtr)
        if (mapTEtoHNodes.find(predtePtr) != mapTEtoHNodes.end())
        {
          for (auto& imNode : imNodes)
          { // this is IM loop
            ATH_MSG_DEBUG("TE corresponding to IM = "
              << tePtr << " " << hash2string[tePtr->getId()]
              << " seeded by TE " << predtePtr << " " << hash2string[predtePtr->getId()]
              << " IM node " << imNode);
             ATH_MSG_DEBUG("ADDTEST IM node " << imNode->name()); 
            for (auto& hNode : mapTEtoHNodes[predtePtr])
            { // this is H loop
              ATH_MSG_DEBUG("ADDTEST H node " << hNode->name()); 
              TrigCompositeUtils::linkToPrevious(imNode, hNode, context);
              ATH_MSG_DEBUG("TE predTE linking IM lower H upper = " << tePtr << " " << predtePtr << " " << imNode << " " << hNode);
              regtestSorted.insert("REGTEST [IM -> H seed link] (" + std::to_string(imNode->index()) + " -> " + std::to_string(hNode->index()) + ")");
            }
          }
        }
    } // for (auto prep_ptr : run2Navigation....
  }
  for (const auto& p : regtestSorted) {
    ATH_MSG_DEBUG(p);
  }



  return StatusCode::SUCCESS;
}

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ getSgKey @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
std::tuple<uint32_t, CLID, std::string> Run2ToRun3TrigNavConverter::getSgKey(const HLT::TrigNavStructure& navigationDecoder, const HLT::TriggerElement::FeatureAccessHelper& helper) const
{
  const std::string hltLabel = navigationDecoder.label(helper.getCLID(), helper.getIndex().subTypeIndex());


  const CLID saveCLID = [&](const CLID& clid) {
    if (clid == m_roIDescriptorCLID) return m_roIDescriptorCollectionCLID;
    if (clid == m_TrigEMClusterCLID) return m_TrigEMClusterContainerCLID;
    if (clid == m_TrigRingerRingsCLID) return m_TrigRingerRingsContainerCLID;
    return clid;
  }(helper.getCLID());

  std::string type_name;
  if (m_clidSvc->getTypeNameOfID(saveCLID, type_name).isFailure())
  {
    return { 0, 0, "" };
  }

  const auto sgStringKey = HLTNavDetails::formatSGkey("HLT", type_name, hltLabel);
  const bool isAvailable = evtStore()->contains(saveCLID, sgStringKey);
  ATH_MSG_DEBUG(" Objects presence " << helper << " " << sgStringKey << (isAvailable ? " present" : " absent"));
  if (!isAvailable) {
    return { 0, 0, "" };
  }

  const auto sgIntKey = evtStore()->stringToKey(sgStringKey, saveCLID);

  ATH_MSG_DEBUG(" getSgKey: sgKey, sgCLID, sgName " << sgIntKey << " " << saveCLID << " " << hltLabel);
  return { sgIntKey, saveCLID, hltLabel }; // sgKey, sgCLID, sgName
}

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ addTEROIfeatures @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
StatusCode Run2ToRun3TrigNavConverter::addTEROIfeatures(const HLT::TrigNavStructure& navigationDecoder, const HLT::TriggerElement::FeatureAccessHelper& helper, TrigCompositeUtils::Decision*& decisionPtr) const
{
  auto [sgKey, sgCLID, sgName] = getSgKey(navigationDecoder, helper);
  ATH_MSG_DEBUG("In addTEROIfeatures sgKey, sgCLID, sgName index-begin " << sgKey << " " << sgCLID << " " << sgName << " " << helper.getIndex().objectsBegin());
  if (sgKey != 0) {
    decisionPtr->typelessSetObjectLink(TrigCompositeUtils::roiString(), sgKey, sgCLID, helper.getIndex().objectsBegin());
  }

  return StatusCode::SUCCESS;
}

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ addROIfeatures @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
StatusCode Run2ToRun3TrigNavConverter::addROIfeatures(const HLT::TrigNavStructure& navigationDecoder, const HLT::TriggerElement::FeatureAccessHelper& helper, TrigCompositeUtils::Decision*& decisionPtr, int idx, L1ObjMap* om) const
{
  auto [sgKey, sgCLID, sgName] = getSgKey(navigationDecoder, helper);

  if (sgKey != 0) {
    if (idx == -1)
    {
      if (om)
      {
        auto fHash = sgKey + helper.getIndex().objectsBegin();
        (*om)[fHash] = decisionPtr;
      }
      decisionPtr->typelessSetObjectLink(TrigCompositeUtils::initialRoIString(), sgKey, sgCLID, helper.getIndex().objectsBegin());
    }
    else
    {
      decisionPtr->typelessSetObjectLink(TrigCompositeUtils::roiString(), sgKey, sgCLID, helper.getIndex().objectsBegin());
    }
  }

  return StatusCode::SUCCESS;
}

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ addTRACKfeatures @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
StatusCode Run2ToRun3TrigNavConverter::addTRACKfeatures(const HLT::TrigNavStructure& navigationDecoder, const HLT::TriggerElement::FeatureAccessHelper& helper, TrigCompositeUtils::Decision*& decisionPtr, ElementLink<TrigRoiDescriptorCollection>& rLink) const
{
  SG::AuxElement::Decorator<ElementLink<TrigRoiDescriptorCollection>> viewBookkeeper("viewIndex");
  auto [sgKey, sgCLID, sgName] = getSgKey(navigationDecoder, helper);
  ATH_MSG_DEBUG("sgKey, sgCLID, sgName " << sgKey << " " << sgCLID << " " << sgName);
  if (decisionPtr != nullptr && sgCLID == 1287425431) // CLASS_DEF( xAOD::TrackParticleContainer, 1287425431, 1 )
  {
    decisionPtr->typelessSetObjectLink("TEMP_TRACKS", sgKey, sgCLID, helper.getIndex().objectsBegin(), helper.getIndex().objectsEnd());
    ElementLinkVector<xAOD::TrackParticleContainer> tracks = decisionPtr->objectCollectionLinks<xAOD::TrackParticleContainer>("TEMP_TRACKS");
    decisionPtr->removeObjectCollectionLinks("TEMP_TRACKS");
    for (const ElementLink<xAOD::TrackParticleContainer> track : tracks)
    {
      if (track.isValid())
      {
        const xAOD::TrackParticle* t = *track;
        viewBookkeeper(*t) = rLink;
      }
    }
  }
  return StatusCode::SUCCESS;
}

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ addTEfeatures @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
StatusCode Run2ToRun3TrigNavConverter::addTEfeatures(const HLT::TrigNavStructure& navigationDecoder, const HLT::TriggerElement::FeatureAccessHelper& helper, std::pair<TrigCompositeUtils::Decision*, TrigCompositeUtils::Decision*>& decisionPtr, int idx, DecisionObjMap* om) const
{
  auto [sgKey, sgCLID, sgName] = getSgKey(navigationDecoder, helper);

  if (idx == -1)
  {
    auto fHash = sgKey + helper.getIndex().objectsBegin();
    (*om)[fHash] = decisionPtr;

    decisionPtr.first->typelessSetObjectLink(TrigCompositeUtils::featureString(), sgKey, sgCLID, helper.getIndex().objectsBegin(), helper.getIndex().objectsEnd());
  }
  else
  {
    auto fHash = sgKey + idx;
    (*om)[fHash] = decisionPtr;
    decisionPtr.first->typelessSetObjectLink(TrigCompositeUtils::featureString(), sgKey, sgCLID, idx, idx + 1);
  }

  return StatusCode::SUCCESS;
}

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ getTEfeatures @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
const std::vector<HLT::TriggerElement::FeatureAccessHelper> Run2ToRun3TrigNavConverter::getTEfeatures(const HLT::TriggerElement* te_ptr, const HLT::TrigNavStructure& navigationDecoder, bool filterOnCLID) const
{
  std::vector<HLT::TriggerElement::FeatureAccessHelper> ptrFAHelper;
  for (HLT::TriggerElement::FeatureAccessHelper helper : te_ptr->getFeatureAccessHelpers())
  {
    auto [sgKey, sgCLID, sgName] = getSgKey(navigationDecoder, helper);
    if (sgKey==0) return ptrFAHelper; 
    if (filterOnCLID && m_setCLID.find(helper.getCLID()) == m_setCLID.end())
    {
     // continue;
    }
    ptrFAHelper.push_back(helper);
  }
  return ptrFAHelper;
}

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ getTEROIfeatures @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
const std::vector<HLT::TriggerElement::FeatureAccessHelper> Run2ToRun3TrigNavConverter::getTEROIfeatures(const HLT::TriggerElement* te_ptr, const HLT::TrigNavStructure& navigationDecoder) const
{
  // @@@@@@@@@@@@@@@@@@@@@@@@@@ ordered_sorter @@@@@@@@@@@@@@@@@@@@@@@@@@
  auto ordered_sorter = [&](const auto& left, const auto& right) -> bool {
    return std::find(cbegin(m_setRoiName), cend(m_setRoiName), left) < std::find(cbegin(m_setRoiName), cend(m_setRoiName), right);
  };

  std::map<std::string, HLT::TriggerElement::FeatureAccessHelper, decltype(ordered_sorter)> mp(ordered_sorter);


  for (HLT::TriggerElement::FeatureAccessHelper helper : te_ptr->getFeatureAccessHelpers())
  {
    auto [sgKey, sgCLID, sgName] = getSgKey(navigationDecoder, helper);
    ATH_MSG_DEBUG(" getTEROIfeatures name " << sgName);
    if (std::find(m_setRoiName.begin(), m_setRoiName.end(), sgName) == m_setRoiName.end()) {
      // do not filter continue;
      continue;
    }
    ATH_MSG_DEBUG(" getTEROIfeatures name accepted " << sgName);
    mp[sgName] = helper;
  }

  for (const auto& p : mp) {
    auto [sgKey, sgCLID, sgName] = getSgKey(navigationDecoder, p.second);
    ATH_MSG_DEBUG("CHECK getTEROIfeatures name accepted " << sgName);
  }

  std::vector<HLT::TriggerElement::FeatureAccessHelper> ptrFAHelper;
  std::transform(cbegin(mp), cend(mp), back_inserter(ptrFAHelper),
    [](const std::map<std::string, HLT::TriggerElement::FeatureAccessHelper>::value_type& p) {return p.second;});

  return ptrFAHelper;
}

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ getTRACKfeatures @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
const std::vector<HLT::TriggerElement::FeatureAccessHelper> Run2ToRun3TrigNavConverter::getTRACKfeatures(const HLT::TriggerElement* te_ptr) const
{
  std::vector<HLT::TriggerElement::FeatureAccessHelper> ptrFAHelper;
  for (HLT::TriggerElement::FeatureAccessHelper helper : te_ptr->getFeatureAccessHelpers())
  {
    if (helper.getCLID() == m_TrackParticleContainerCLID || 
        helper.getCLID() == m_TrackParticleAuxContainerCLID || 
        helper.getCLID() == m_TauTrackContainerCLID || 
        helper.getCLID() == m_TauTrackAuxContainerCLID )
    {
      ptrFAHelper.push_back(helper);
    }
  }
  return ptrFAHelper;
}

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ getROIfeatures @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
const std::vector<HLT::TriggerElement::FeatureAccessHelper> Run2ToRun3TrigNavConverter::getROIfeatures(const HLT::TriggerElement* te_ptr, const HLT::TrigNavStructure& navigationDecoder) const
{

  std::vector<HLT::TriggerElement::FeatureAccessHelper> ptrFAHelper;
  for (HLT::TriggerElement::FeatureAccessHelper helper : te_ptr->getFeatureAccessHelpers())
  {

    auto [sgKey, sgCLID, sgName] = getSgKey(navigationDecoder, helper);

    if (helper.getCLID() != m_roIDescriptorCLID)
    {
      continue;
    }
    ptrFAHelper.push_back(helper);
  }
  return ptrFAHelper;
}

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ printFeatures @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
StatusCode Run2ToRun3TrigNavConverter::printFeatures(const HLT::TrigNavStructure& nav) const
{
  std::set<std::string> totset;

  for (const auto chain : m_configSvc->chains())
  {
    std::set<std::string> fset;
    for (auto signature : chain->signatures())
    {
      for (auto configTE : signature->outputTEs())
      {
        std::vector<HLT::TriggerElement*> tes;
        nav.getAllOfType(configTE->id(), tes, false);
        for (auto te : tes)
        {
          for (auto featureAccessHelper : te->getFeatureAccessHelpers())
          {
            std::string type;
            if (m_clidSvc->getTypeNameOfID(featureAccessHelper.getCLID(), type).isFailure())
            {
              ATH_MSG_WARNING("CLID " << featureAccessHelper.getCLID() << " is not known");
            }
            const std::string info = type + "#" + nav.label(featureAccessHelper.getCLID(), featureAccessHelper.getIndex().subTypeIndex());
            fset.insert(info);
            totset.insert(info);
          }
        }
      }
    }
    const std::vector<std::string> fvec(fset.begin(), fset.end());
    ATH_MSG_INFO("chain " << chain->name() << " features " << fvec.size() << " " << fvec);
  }
  const std::vector totvec(totset.begin(), totset.end());
  ATH_MSG_INFO("all event features " << totvec.size() << " " << totvec);

  return StatusCode::SUCCESS;
}


StatusCode  Run2ToRun3TrigNavConverter::insertDecisionToTEMap( TrigCompositeUtils::Decision* d, const HLT::TriggerElement* te, TEDecisionMap& mapToFill) const { 
  auto& entryToFill = mapToFill[te];
  auto iter = std::find(entryToFill.begin(), entryToFill.end(), d);
  if (iter == entryToFill.end() )
    entryToFill.push_back(d);
  return StatusCode::SUCCESS;
}
