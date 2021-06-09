/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "Run2ToRun3TrigNavConverter.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigNavStructure/TriggerElement.h"
#include "TrigNavStructure/StandaloneNavigation.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODTracking/versions/TrackParticle_v1.h"
#include "xAODTracking/TrackParticleContainer.h"
#include <functional>
#include <random>

Run2ToRun3TrigNavConverter::Run2ToRun3TrigNavConverter(const std::string &name, ISvcLocator *pSvcLocator) : AthReentrantAlgorithm(name, pSvcLocator)
{
}

Run2ToRun3TrigNavConverter::~Run2ToRun3TrigNavConverter()
{
}

StatusCode Run2ToRun3TrigNavConverter::initialize()
{


  ATH_CHECK(m_trigNavWriteKey.initialize());
  ATH_CHECK(m_trigSummaryWriteKey.initialize());
  ATH_CHECK(m_configSvc.retrieve());
  ATH_CHECK(m_clidSvc.retrieve());
  ATH_CHECK( m_tdt.empty() != m_trigNavKey.key().empty() ); //either of the two has to be enabled but not both
  if ( !m_tdt.empty() ) {
    ATH_CHECK(m_tdt.retrieve());
    m_tdt->ExperimentalAndExpertMethods()->enable();
    ATH_MSG_INFO( "Will use Trigger Navigation from TrigDecisionTool");
  } else { 
    ATH_CHECK(m_trigNavKey.initialize(SG::AllowEmpty));
    ATH_MSG_INFO( "Will use Trigger Navigation decoded from TrigNavigation object");
  }
  // retrievig CLID from names and storing to set
  for (const auto &name : m_collectionsToSave)
  {
    CLID id{0};
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
  ATH_CHECK(m_clidSvc->getIDOfTypeName("xAOD::TauTrackContainer", m_TauTrackContainerCLID));

  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverter::finalize()
{
  return StatusCode::SUCCESS;
}


StatusCode Run2ToRun3TrigNavConverter::execute(const EventContext &context) const
{
  const HLT::TrigNavStructure* navDecoderPtr = nullptr;
  HLT::StandaloneNavigation standaloneNav;
  if (!m_trigNavKey.key().empty()) {    
    SG::ReadHandle navReadHandle(m_trigNavKey, context);
    ATH_CHECK(navReadHandle.isValid());
    standaloneNav.deserialize(navReadHandle->serialized());
    navDecoderPtr = &standaloneNav;
  } else {
    navDecoderPtr = m_tdt->ExperimentalAndExpertMethods()->getNavigation();
  }
  const HLT::TrigNavStructure& navDecoder = *navDecoderPtr;

  if (m_onlyFeaturePriting)
    return printFeatures(navDecoder);

  SG::WriteHandle<TrigCompositeUtils::DecisionContainer> outputNavigation = TrigCompositeUtils::createAndStore(m_trigNavWriteKey, context);
  auto decisionOutput = outputNavigation.ptr();
  SG::WriteHandle<TrigCompositeUtils::DecisionContainer> outputSummary = TrigCompositeUtils::createAndStore(m_trigSummaryWriteKey, context);
  auto decisionSummary = outputSummary.ptr();
  TrigCompositeUtils::Decision *passRawOutput = TrigCompositeUtils::newDecisionIn(decisionSummary, "HLTPassRaw");

  if (m_doPrint)
  {
    std::string dump;
    navDecoder.printASCIIArt(dump);
    ATH_MSG_INFO(dump);
  }


  std::vector<HLT::TriggerElement *> tes;
  TE_Decision_map mapTEtoDecision;                          // TE - Decision (xAOD::TrigComposite)
  TE_Decision_map mapTEtoDecisionActive;                    // TE Active - Decision (xAOD::TrigComposite)
  std::vector<TrigCompositeUtils::Decision *> decisionLast; // storing "last" decision in a chain

  DecisionObjMap decisionObj;
  DecisionObjMap decisionObjFeatureless;
  L1ObjMap l1Obj;
  TEObjMap teObj;
  FElessObjMap feObj; // this is only for FEless H nodes -> pair(IM node, TEptr)
  TEMap teMap;        // TEptr - bool, true if FE

  std::vector<HLT::TriggerElement::FeatureAccessHelper> featureRoI; // for keeping predecessing TE with RoI

  // @@@@@@@@@@@@@@@@@@@@@@@@@@ getDecisionObject @@@@@@@@@@@@@@@@@@@@@@@@@@
  auto getDecisionObject = [&](const HLT::TriggerElement::FeatureAccessHelper &elemFE, size_t idx, TrigCompositeUtils::DecisionContainer *dOutput, size_t &kIn) {
  // kIn: flag with the information about the decision objects (DO)
  // 0: DO pair has been found and returned (no new pair created)
  // 1: H node created for a feature unit from collection, IM retrieved (not created)
  // 2: a new DO pair created (that is: H and IM node)
    auto [sgKey, sgCLID, sgName] = getSgKey(navDecoder, elemFE);
    auto it = decisionObj.find(sgKey + idx);
    if (it != decisionObj.end())
    {
      kIn = 0;
      return it->second;
    }
    kIn = 2;

    if (idx == elemFE.getIndex().objectsBegin())
    {
      auto d1 = TrigCompositeUtils::newDecisionIn(dOutput);
      d1->setName(TrigCompositeUtils::hypoAlgNodeName());
      auto d2 = TrigCompositeUtils::newDecisionIn(dOutput);
      d2->setName(TrigCompositeUtils::inputMakerNodeName());
      return std::make_pair(d1, d2);
    }

    kIn = 1;
    auto d1 = TrigCompositeUtils::newDecisionIn(dOutput);
    d1->setName(TrigCompositeUtils::hypoAlgNodeName());
    return std::make_pair(d1, (decisionObj.find(sgKey + elemFE.getIndex().objectsBegin())->second).second);
  };

  // @@@@@@@@@@@@@@@@@@@@@@@@@@ getL1Object @@@@@@@@@@@@@@@@@@@@@@@@@@
  auto getL1Object = [&](const HLT::TriggerElement::FeatureAccessHelper &elemFE, size_t idx, TrigCompositeUtils::DecisionContainer *dOutput, size_t &kIn) {
    auto [sgKey, sgCLID, sgName] = getSgKey(navDecoder, elemFE);
    auto it = l1Obj.find(sgKey + idx);
    if (it != l1Obj.end())
    {
      kIn = 0;
      return it->second;
    }
    kIn = 1;

    auto d1 = TrigCompositeUtils::newDecisionIn(dOutput);
    d1->setName(TrigCompositeUtils::l1DecoderNodeName());
    return d1;
  };

  // @@@@@@@@@@@@@@@@@@@@@@@@@@ begin of chain loop @@@@@@@@@@@@@@@@@@@@@@@@@@
  for (auto p : m_configSvc->chains())
  {
    if (m_convertAllChains || m_setChainName.find(p->name()) != m_setChainName.end())
    {
      std::string chainName = p->name();
      auto c = p;
      HLT::Identifier chainId = HLT::Identifier(chainName);

        // @@@@@@@@@@@@@@@@@@@@@@@@@@ ordered_sorter @@@@@@@@@@@@@@@@@@@@@@@@@@
        auto ordered_sorter = [&](const auto& left, const auto& right) {
           return std::find(cbegin(m_setRoiName), cend(m_setRoiName), left) < std::find(cbegin(m_setRoiName), cend(m_setRoiName), right);
        };
        std::map<std::string, HLT::TriggerElement::FeatureAccessHelper, decltype(ordered_sorter)> mp(ordered_sorter);
        // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

      for (auto s_iter = c->signatures().begin(), first_s_iter = s_iter; s_iter != c->signatures().end(); ++s_iter)
      {

        std::vector<TrigCompositeUtils::Decision *> tempDecisionVector;

        for (auto te : (*s_iter)->outputTEs())
        {
          tes.clear();
          navDecoder.getAllOfType(te->id(), tes, false);
          if (tes.empty() == false)
            decisionLast.clear();

          for (auto teptr : tes)
          {

            tempDecisionVector.clear();

            auto vectorTEfeatures_ptr = getTEfeatures(teptr, navDecoder);
            auto vectorTEROIfeatures_ptr = getTEROIfeatures( teptr, navDecoder );
            auto vectorTRACKfeatures_ptr = getTRACKfeatures(teptr);

            mp.clear();

            // @@@@@@@@@@@@@@@@@@@@@@@@@@ roiFinder @@@@@@@@@@@@@@@@@@@@@@@@@@
            std::function<void(HLT::TriggerElement*)> roiFinder = [&](HLT::TriggerElement* ptrTE) {
                auto vecTEpred = navDecoder.getDirectPredecessors(ptrTE);
                if (vecTEpred.empty()) {
                  return;
                } else {
                  for (auto pred : vecTEpred) { // vector predecessors TE
                    auto vectorTEROIfeatures_ptr = getTEROIfeatures( pred, navDecoder );
                    if (vectorTEROIfeatures_ptr.empty()==false) {
                      auto [sgKey, sgCLID, sgName] = getSgKey(navDecoder,vectorTEROIfeatures_ptr.front());
                      mp[sgName] = vectorTEROIfeatures_ptr.front();
                    } 
                  }        
                  if (mp.empty()) {
                    for (auto pred : vecTEpred) { // vector predecessors TE
                      roiFinder(pred);
                    }
                  }
                }
                return; 
            };

            // @@@@@@@@@@@@@@@@@@@@@@@@@@ featureFinder @@@@@@@@@@@@@@@@@@@@@@@@@@
            std::function<uint32_t(HLT::TriggerElement*,uint32_t&)> featureFinder = [&](HLT::TriggerElement* ptrTE, uint32_t& deepLvl) {
                auto  vecTEpred = navDecoder.getDirectPredecessors(ptrTE);
                if (vecTEpred.empty()) // no TE predecessor, no FE at all                    
                { 
                  return (uint32_t)0;
                } else {
                  for (auto pred : vecTEpred) { // vector predecessors TE
                    auto vectorTEfeatures_ptr = getTEfeatures( pred, navDecoder );
                    if (vectorTEfeatures_ptr.size()==1 && (vectorTEfeatures_ptr.front().getIndex().objectsBegin()==vectorTEfeatures_ptr.front().getIndex().objectsEnd())) {
                      break; // this is empty FE, treat it is as zero FE
                    }
                    if (vectorTEfeatures_ptr.empty()==false) 
                    {
                      auto featureBegin = 1000*vectorTEfeatures_ptr.front().getIndex().objectsBegin();
                      auto [sgKey, sgCLID, sgName] = getSgKey(navDecoder,vectorTEfeatures_ptr.front());
                      return sgKey+featureBegin+deepLvl; // first found 
                    } 
                  }
                  // if no success after scan of TE predecessors we go one level up
                  ++deepLvl;
                  uint32_t keySearch { 0 };
                  for (auto pred : vecTEpred) { 
                    keySearch = featureFinder(pred,deepLvl); // recursive call
                    if (keySearch != 0) return keySearch; // first found
                  }
                }
                return (uint32_t)0; 
            };

            // @@@@@@@@@@@@@@@@@@@@@@@@@@ getTEObject @@@@@@@@@@@@@@@@@@@@@@@@@@
            auto getTEObject = [&](HLT::TriggerElement* ptrTE, uint32_t& deepLevel, TrigCompositeUtils::DecisionContainer *dOutput) {
                // this is featureless case
                uint32_t sgKeyProxy = featureFinder(ptrTE,deepLevel);
                if (sgKeyProxy != 0) {
                  auto it = decisionObjFeatureless.find(sgKeyProxy);
                  if (it != decisionObjFeatureless.end()) return it->second;
                }
                
                auto d1 = TrigCompositeUtils::newDecisionIn(dOutput);
                d1->setName(TrigCompositeUtils::hypoAlgNodeName());
                auto d2 = TrigCompositeUtils::newDecisionIn(dOutput);
                d2->setName(TrigCompositeUtils::inputMakerNodeName());
                if (sgKeyProxy==0) { // let us random it
                  std::random_device rd;
                  std::mt19937_64 gen(rd());
                  std::uniform_int_distribution<uint32_t> dis;
                  sgKeyProxy = 1000000 + dis(gen);
                }
                return decisionObjFeatureless[sgKeyProxy] = std::make_pair(d1, d2);
            };


            if (vectorTEROIfeatures_ptr.empty()) {
              roiFinder(teptr);
            } else {
              auto [sgKey, sgCLID, sgName] = getSgKey(navDecoder,vectorTEROIfeatures_ptr.front());
              mp[sgName] = vectorTEROIfeatures_ptr.front();
            }


            // @@@@@@@@@@@@@@@@@@@@@@@@@@ vector TE - EMPTY @@@@@@@@@@@@@@@@@@@@@@@@@@
            if ( vectorTEfeatures_ptr.empty() || 
               ( vectorTEfeatures_ptr.size() == 1 && 
                 (vectorTEfeatures_ptr[0].getIndex().objectsBegin() == vectorTEfeatures_ptr[0].getIndex().objectsEnd()) )
               ) 
            {


              uint32_t dLevel { 0 };
              auto [ decisionFeature, decisionPtr ] = getTEObject(teptr,dLevel,decisionOutput);
              auto decision = decisionPtr;

              tempDecisionVector.push_back(decisionFeature);

              if (teptr->getActiveState())
                TrigCompositeUtils::addDecisionID(chainId, decisionFeature);
              ElementLink<xAOD::TrigCompositeContainer> linkToSelf = TrigCompositeUtils::decisionToElementLink(decisionFeature, context);
              decisionFeature->setObjectLink<xAOD::TrigCompositeContainer>(TrigCompositeUtils::featureString(), linkToSelf);
              mapTEtoDecision[teptr].push_back(decisionFeature);
              TrigCompositeUtils::addDecisionID(chainId, decision);

              TrigCompositeUtils::Decision *l1_decision{nullptr};
              if (s_iter == first_s_iter)
              {
                l1_decision = TrigCompositeUtils::newDecisionIn(decisionOutput);
                l1_decision->setName(TrigCompositeUtils::l1DecoderNodeName());
                TrigCompositeUtils::addDecisionID(chainId, l1_decision);
                TrigCompositeUtils::linkToPrevious(decision, l1_decision, context);
              }


              if (mp.empty() == false) {
                ATH_CHECK(addTEROIfeatures(navDecoder, (mp.begin())->second, decision)); 
              }


              for (const auto &rNodes : HLT::TrigNavStructure::getRoINodes(teptr))
              {
                if (HLT::TrigNavStructure::isRoINode(rNodes))
                {
                  auto vectorROIfeatures_ptr = getROIfeatures(rNodes, navDecoder);
                  for (auto featureRoI : vectorROIfeatures_ptr)
                  {
                    if (mp.empty()) {
                      ATH_CHECK(addTEROIfeatures(navDecoder, featureRoI, decision)); // roi link to initialRoi
                    }
                  }

                  // ### TRACKS ### check for track features and connect the same RoI
                  if (!vectorROIfeatures_ptr.empty())
                    for (const auto &elemTRACK : vectorTRACKfeatures_ptr)
                    {
                      for (size_t i{elemTRACK.getIndex().objectsBegin()}; i < elemTRACK.getIndex().objectsEnd(); ++i)
                      {

                        ElementLink<TrigRoiDescriptorCollection> ROIElementLink = decision->objectLink<TrigRoiDescriptorCollection>(TrigCompositeUtils::roiString());
                        if (ROIElementLink.isValid())
                        {
                          ATH_CHECK(addTRACKfeatures(navDecoder, elemTRACK, decision, ROIElementLink));
                        }
                      }
                    }
                }
              }

              for (const auto &p : tempDecisionVector)
              {
                TrigCompositeUtils::linkToPrevious(p, decision, context);
              }

              mapTEtoDecisionActive[teptr].push_back(decision);
            }


            for (const auto &elemFE : vectorTEfeatures_ptr)
            {
              TrigCompositeUtils::Decision *decision{nullptr};

              for (size_t i{elemFE.getIndex().objectsBegin()}; i < elemFE.getIndex().objectsEnd(); ++i)
              {
                size_t kInsert{0};
                auto [decisionFeature, decisionPtr] = getDecisionObject(elemFE, i, decisionOutput, kInsert);
                decision = decisionPtr;

                if (mp.empty() == false) {
                      ATH_CHECK(addTEROIfeatures(navDecoder, (mp.begin())->second, decision)); // updated roi on the way
                }


                if (teptr->getActiveState()) TrigCompositeUtils::addDecisionID(chainId, decisionFeature);
                TrigCompositeUtils::addDecisionID(chainId, decision);
                TrigCompositeUtils::linkToPrevious(decisionFeature, decision, context);

                tempDecisionVector.push_back(decisionFeature);
                if (kInsert)
                {
                  auto mp = std::make_pair(decisionFeature, decision);
                  ATH_CHECK(addTEfeatures(navDecoder, elemFE, mp, i, &decisionObj));
                }
                mapTEtoDecision[teptr].push_back(decisionFeature);
                mapTEtoDecisionActive[teptr].push_back(decision);
              }

              for (const auto &rNodes : HLT::TrigNavStructure::getRoINodes(teptr))
              {
                if (HLT::TrigNavStructure::isRoINode(rNodes))
                {
                  auto vectorROIfeatures_ptr = getROIfeatures(rNodes,navDecoder);
                  for (auto featureRoI : vectorROIfeatures_ptr)
                  {
                    if (s_iter == first_s_iter)
                    {
                      size_t kInsert{0};
                      auto l1_decision = getL1Object(featureRoI, featureRoI.getIndex().objectsBegin(), decisionOutput, kInsert);
                      TrigCompositeUtils::addDecisionID(chainId, l1_decision);
                      if (kInsert)
                      {
                        ATH_CHECK(addROIfeatures(navDecoder, featureRoI, l1_decision, -1, &l1Obj)); // test coding for initialRoi
                      }
                      // check for updated roi
                      if (mp.empty()) {
                        ATH_CHECK(addTEROIfeatures(navDecoder, featureRoI, decision)); // roi link to initialRoi
                      }
                      TrigCompositeUtils::linkToPrevious(decision, l1_decision, context);
                    }
                    else
                    {
                      // check for updated roi
                      if (mp.empty()) {
                        ATH_CHECK(addTEROIfeatures(navDecoder, featureRoI, decision)); // roi link to initialRoi
                      }
                    }
                  }
                  // ### TRACKS ### check for track features and connect the same RoI
                  if (!vectorROIfeatures_ptr.empty())
                  for (const auto &elemTRACK : vectorTRACKfeatures_ptr)
                  {
                    for (size_t i{elemTRACK.getIndex().objectsBegin()}; i < elemTRACK.getIndex().objectsEnd(); ++i)
                    {
                        if (mp.empty()) {
                            ElementLink<TrigRoiDescriptorCollection> ROIElementLink = decision->objectLink<TrigRoiDescriptorCollection>(TrigCompositeUtils::roiString());
                            if (ROIElementLink.isValid())
                            {
                                ATH_CHECK(addTRACKfeatures(navDecoder, elemTRACK, decision, ROIElementLink));
                            }
                        }
                    }
                  }
                }
              }
            }

            decisionLast.insert(decisionLast.end(), tempDecisionVector.begin(), tempDecisionVector.end());
          }
        }
      }

      for (auto &[teptr, decisions] : mapTEtoDecisionActive)
      { // loop over all IM nodes

        for (auto prep_ptr : navDecoder.getDirectPredecessors(teptr))
        {

          if (mapTEtoDecision.find(prep_ptr) != mapTEtoDecision.end())
          {
            for (auto d : decisions)
            { // this is IM loop

              for (auto pd : mapTEtoDecision[prep_ptr])
              { // this is H loop
                TrigCompositeUtils::linkToPrevious(d, pd, context);
              }
            }
          }
        } // for (auto prep_ptr : navDecoder....
      }

      std::set<TrigCompositeUtils::Decision *> decisionLastUnique(decisionLast.begin(), decisionLast.end());
      decisionLast.assign(decisionLastUnique.begin(), decisionLastUnique.end());

      for (const auto &last : decisionLast)
      {
        auto sf_decision = TrigCompositeUtils::newDecisionIn(decisionOutput);
        sf_decision->setName("SF");
        TrigCompositeUtils::addDecisionID(chainId, sf_decision);
        TrigCompositeUtils::linkToPrevious(sf_decision, last, context);
        TrigCompositeUtils::linkToPrevious(passRawOutput, sf_decision, context);
      }
      TrigCompositeUtils::decisionIDs(passRawOutput).push_back(chainId);
    }
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
  const bool isAvailable = evtStore()->contains( saveCLID,  sgStringKey);
  ATH_MSG_DEBUG(" Objects presence " << helper << " " << sgStringKey <<  (isAvailable? " present" : " absent"));
  if ( ! isAvailable ) {
    return { 0, 0, "" };
  }

  const auto sgIntKey = evtStore()->stringToKey(sgStringKey, saveCLID);

  return { sgIntKey, saveCLID, hltLabel }; // sgKey, sgCLID, sgName
}

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ addTEROIfeatures @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
StatusCode Run2ToRun3TrigNavConverter::addTEROIfeatures(const HLT::TrigNavStructure &navigationDecoder, const HLT::TriggerElement::FeatureAccessHelper &helper, TrigCompositeUtils::Decision *&decisionPtr) const
{
  auto [sgKey, sgCLID, sgName] = getSgKey(navigationDecoder, helper);

  decisionPtr->typelessSetObjectLink(TrigCompositeUtils::roiString(), sgKey, sgCLID, helper.getIndex().objectsBegin());

  return StatusCode::SUCCESS;
}

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ addROIfeatures @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
StatusCode Run2ToRun3TrigNavConverter::addROIfeatures(const HLT::TrigNavStructure &navigationDecoder, const HLT::TriggerElement::FeatureAccessHelper &helper, TrigCompositeUtils::Decision *&decisionPtr, int idx, L1ObjMap *om) const
{
  auto [sgKey, sgCLID, sgName] = getSgKey(navigationDecoder, helper);

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

  return StatusCode::SUCCESS;
}

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ addTRACKfeatures @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
StatusCode Run2ToRun3TrigNavConverter::addTRACKfeatures(const HLT::TrigNavStructure &navigationDecoder, const HLT::TriggerElement::FeatureAccessHelper &helper, TrigCompositeUtils::Decision *&decisionPtr, ElementLink<TrigRoiDescriptorCollection> &rLink) const
{
  SG::AuxElement::Decorator<ElementLink<TrigRoiDescriptorCollection>> viewBookkeeper("viewIndex");
  auto [sgKey, sgCLID, sgName] = getSgKey(navigationDecoder, helper);

  if (sgKey != 0)
  {
    decisionPtr->typelessSetObjectLink("TEMP_TRACKS", sgKey, sgCLID, helper.getIndex().objectsBegin(), helper.getIndex().objectsEnd());
    if (decisionPtr != nullptr)
    {
      ElementLinkVector<xAOD::TrackParticleContainer> tracks = decisionPtr->objectCollectionLinks<xAOD::TrackParticleContainer>("TEMP_TRACKS");

      decisionPtr->removeObjectCollectionLinks("TEMP_TRACKS");
      for (const ElementLink<xAOD::TrackParticleContainer> &track : tracks)
      {
        if (track.isValid())
        {
          const xAOD::TrackParticle *t = *track;
          viewBookkeeper(*t) = rLink;
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ addTEfeatures @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
StatusCode Run2ToRun3TrigNavConverter::addTEfeatures(const HLT::TrigNavStructure &navigationDecoder, const HLT::TriggerElement::FeatureAccessHelper &helper, std::pair<TrigCompositeUtils::Decision *, TrigCompositeUtils::Decision *> &decisionPtr, int idx, DecisionObjMap *om) const
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
const std::vector<HLT::TriggerElement::FeatureAccessHelper> Run2ToRun3TrigNavConverter::getTEfeatures(const HLT::TriggerElement *te_ptr, const HLT::TrigNavStructure &navigationDecoder) const
{
  std::vector<HLT::TriggerElement::FeatureAccessHelper> ptrFAHelper;
  for (HLT::TriggerElement::FeatureAccessHelper helper : te_ptr->getFeatureAccessHelpers())
  {
    auto [sgKey, sgCLID, sgName] = getSgKey(navigationDecoder, helper);
    if (m_setCLID.find(helper.getCLID()) == m_setCLID.end())
    {
      continue;
    }
    ptrFAHelper.push_back(helper);
  }
  return ptrFAHelper;
}

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ getTEROIfeatures @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
const std::vector<HLT::TriggerElement::FeatureAccessHelper> Run2ToRun3TrigNavConverter::getTEROIfeatures(const HLT::TriggerElement *te_ptr, const HLT::TrigNavStructure &navigationDecoder) const
{
  // @@@@@@@@@@@@@@@@@@@@@@@@@@ ordered_sorter @@@@@@@@@@@@@@@@@@@@@@@@@@
  auto ordered_sorter = [&](const auto& left, const auto& right) -> bool {
      return std::find(cbegin(m_setRoiName), cend(m_setRoiName), left) < std::find(cbegin(m_setRoiName), cend(m_setRoiName), right);
  };

  std::map<std::string, HLT::TriggerElement::FeatureAccessHelper, decltype(ordered_sorter)> mp(ordered_sorter);


  for (HLT::TriggerElement::FeatureAccessHelper helper : te_ptr->getFeatureAccessHelpers())
  {
    auto [sgKey, sgCLID, sgName] = getSgKey(navigationDecoder,helper);
       if (std::find(m_setRoiName.begin(), m_setRoiName.end(), sgName) == m_setRoiName.end()) {
        continue;
       }
      mp[sgName] = helper;
  }

  std::vector<HLT::TriggerElement::FeatureAccessHelper> ptrFAHelper;
  std::transform(cbegin(mp),cend(mp),back_inserter(ptrFAHelper),
  [](const std::map<std::string, HLT::TriggerElement::FeatureAccessHelper>::value_type& p ){return p.second;});

  return ptrFAHelper;
}

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ getTRACKfeatures @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
const std::vector<HLT::TriggerElement::FeatureAccessHelper> Run2ToRun3TrigNavConverter::getTRACKfeatures(const HLT::TriggerElement *te_ptr) const
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

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ getROIfeatures @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
const std::vector<HLT::TriggerElement::FeatureAccessHelper> Run2ToRun3TrigNavConverter::getROIfeatures(const HLT::TriggerElement *te_ptr, const HLT::TrigNavStructure &navigationDecoder) const
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
StatusCode Run2ToRun3TrigNavConverter::printFeatures(const HLT::TrigNavStructure &nav) const
{
  std::set<std::string> totset;

  for (const auto chain : m_configSvc->chains())
  {
    std::set<std::string> fset;
    for (auto signature : chain->signatures())
    {
      for (auto configTE : signature->outputTEs())
      {
        std::vector<HLT::TriggerElement *> tes;
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
