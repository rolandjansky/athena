/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef HLTSEEDING_ROISUNPACKINGTOOLPHASE1_H
#define HLTSEEDING_ROISUNPACKINGTOOLPHASE1_H

// Local includes
#include "RoIsUnpackingToolBase.h"
#include "HLTSeedingRoIToolDefs.h"

// Athena includes
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadDecorHandleKey.h"
#include "StoreGate/ReadDecorHandle.h"

/**
 * @class RoIsUnpackingToolPhase1
 * Class template for all phase-1 L1 RoI unpacking tools
 */
template<typename T_RoI, typename T_RoIContainer, auto F_RoIWordGetter, const char* N_RoIContainer, const char* N_ThresholdType>
class RoIsUnpackingToolPhase1 : public RoIsUnpackingToolBase {
private:
  Gaudi::Property<std::string> m_l1RoILinkName {
    this, "L1RoILinkName", N_RoIContainer,
    "Name of the link to read from L1TriggerResult for L1 xAOD RoI container"};

  SG::ReadDecorHandleKey<T_RoIContainer> m_thresholdPatternsKey {
    this, "L1RoIThresholdPatternsKey", std::string(N_RoIContainer)+".thresholdPatterns",
    "Name of the L1 xAOD RoI container decoration for the threshold patterns"};

  Gaudi::Property<float> m_roiHalfWidthEta{
    this, "RoIHalfWidthEta", 0.1, "Size of RoI in eta"};

  Gaudi::Property<float> m_roiHalfWidthPhi{
    this, "RoIHalfWidthPhi", 0.1, "Size of RoI in phi"};

public:
  // ===========================================================================
  RoIsUnpackingToolPhase1(const std::string& type, const std::string& name, const IInterface* parent)
    : RoIsUnpackingToolBase(type, name, parent) {}

  // ===========================================================================
  virtual StatusCode initialize() override {
    ATH_CHECK(RoIsUnpackingToolBase::initialize());
    ATH_CHECK(m_l1MenuKey.initialize());
    ATH_CHECK(m_trigRoIsKey.initialize());
    ATH_CHECK(m_thresholdPatternsKey.initialize());
    return StatusCode::SUCCESS;
  }

  // ===========================================================================
  virtual StatusCode start() override {
    ATH_CHECK(decodeMapping([](const std::string& name){
      return name.find(N_ThresholdType)==0 or name.find(getProbeThresholdName(N_ThresholdType))==0;
    }));
    return StatusCode::SUCCESS;
  }

  // ===========================================================================
  using RoIsUnpackingToolBase::unpack;
  virtual StatusCode unpack(const EventContext& ctx,
                            const xAOD::TrigComposite& l1TriggerResult,
                            const HLT::IDSet& activeChains) const override {
    using namespace TrigCompositeUtils;
    const bool doProbe = !m_decisionsKeyProbe.empty();

    // Create and record RoI descriptor and decision containers
    SG::WriteHandle<TrigRoiDescriptorCollection> roiDescriptors = createAndStoreNoAux(m_trigRoIsKey, ctx);
    SG::WriteHandle<DecisionContainer> decisionsMain = createAndStore(m_decisionsKey, ctx);
    SG::WriteHandle<DecisionContainer> decisionsProbe;
    if (doProbe) decisionsProbe = createAndStore(m_decisionsKeyProbe, ctx);

    // Retrieve the xAOD RoI container from L1TriggerResult
    if (!l1TriggerResult.hasObjectLink(m_l1RoILinkName, ClassID_traits<T_RoIContainer>::ID())) {
      ATH_MSG_DEBUG("No " << N_ThresholdType << " RoIs in this event");
      auto monRoIsCount = Monitored::Scalar("count", 0);
      Monitored::Group(m_monTool, monRoIsCount);
      return StatusCode::SUCCESS;
    }
    ElementLink<T_RoIContainer> roisLink = l1TriggerResult.objectLink<T_RoIContainer>(m_l1RoILinkName);
    ATH_CHECK(roisLink.isValid());
    const T_RoIContainer& rois = roisLink.getStorableObjectRef();

    // Create threshold patterns decoration accessor
    auto thrPatternAcc = SG::makeHandle<uint64_t>(m_thresholdPatternsKey, ctx);
    ATH_CHECK(thrPatternAcc.isPresent());

    // Retrieve the L1 menu configuration
    SG::ReadHandle<TrigConf::L1Menu> l1Menu = SG::makeHandle(m_l1MenuKey, ctx);
    ATH_CHECK(l1Menu.isValid());
    std::optional<ThrVecRef> thresholds;
    ATH_CHECK(getL1Thresholds(*l1Menu, N_ThresholdType, thresholds));

    size_t linkIndex{0};
    for (const T_RoI* roi : rois) {
      // Create new RoI descriptor
      roiDescriptors->push_back(std::make_unique<TrigRoiDescriptor>(
        static_cast<unsigned int>((roi->*F_RoIWordGetter)()), 0u, 0u,
        roi->eta(), roi->eta()-m_roiHalfWidthEta, roi->eta()+m_roiHalfWidthEta,
        roi->phi(), roi->phi()-m_roiHalfWidthPhi, roi->phi()+m_roiHalfWidthPhi
      ));

      // Create new decision and link the RoI objects
      Decision* decisionMain = TrigCompositeUtils::newDecisionIn(decisionsMain.ptr(), hltSeedingNodeName());
      decisionMain->setObjectLink(initialRoIString(),
                                  ElementLink<TrigRoiDescriptorCollection>(m_trigRoIsKey.key(), linkIndex, ctx));
      decisionMain->setObjectLink(initialRecRoIString(),
                                  ElementLink<T_RoIContainer>(m_l1RoILinkName, linkIndex, ctx));

      Decision* decisionProbe{nullptr};
      if (doProbe) {
        decisionProbe = TrigCompositeUtils::newDecisionIn(decisionsProbe.ptr(), hltSeedingNodeName());
        decisionProbe->setObjectLink(initialRoIString(),
                                    ElementLink<TrigRoiDescriptorCollection>(m_trigRoIsKey.key(), linkIndex, ctx));
        decisionProbe->setObjectLink(initialRecRoIString(),
                                    ElementLink<T_RoIContainer>(m_l1RoILinkName, linkIndex, ctx));
      }

      std::vector<TrigCompositeUtils::DecisionID> passedThresholdIDs;

      // Add positive decisions for chains to be activated by this RoI object
      uint64_t thresholdPattern = thrPatternAcc(*roi);
      ATH_MSG_DEBUG("RoI #" << linkIndex << " threshold pattern: " << thresholdPattern);
      for (const std::shared_ptr<TrigConf::L1Threshold>& thr : thresholds.value().get()) {
        if ((thresholdPattern & (1 << thr->mapping())) == 0u) {continue;}
        passedThresholdIDs.push_back(HLT::Identifier(thr->name()));
        ATH_MSG_DEBUG("RoI #" << linkIndex << " passed threshold number " << thr->mapping()
                      << " name" << (doProbe ? "s " : " ") << thr->name()
                      << (doProbe ? " and "+getProbeThresholdName(thr->name()) : ""));
        addChainsToDecision(HLT::Identifier(thr->name()), decisionMain, activeChains);
        if (doProbe) addChainsToDecision(HLT::Identifier(getProbeThresholdName(thr->name())), decisionProbe, activeChains);
      }

      decisionMain->setDetail("thresholds", passedThresholdIDs);
      if (doProbe) decisionProbe->setDetail("thresholds", passedThresholdIDs);

      ++linkIndex;
    }

    // Monitor the TrigRoiDescriptorCollection
    {
      auto monRoIsCount = Monitored::Scalar("count", roiDescriptors->size());
      auto monRoIsEta = Monitored::Collection("eta", *roiDescriptors, &TrigRoiDescriptor::eta);
      auto monRoIsPhi = Monitored::Collection("phi", *roiDescriptors, &TrigRoiDescriptor::phi);
      Monitored::Group(m_monTool, monRoIsCount, monRoIsEta, monRoIsPhi);
    }

    return StatusCode::SUCCESS;
  }

}; // class RoIsUnpackingToolPhase1


/** @name RoIsUnpackingToolPhase1_impl
 *  @brief Concrete implementations of the RoIsUnpackingToolPhase1 template, one per threshold type
 */
/// @{
class eFexEMRoIsUnpackingTool : public HLTSeedingRoIToolDefs::eFexEM::UnpackingBaseClass {
public:
  eFexEMRoIsUnpackingTool(const std::string& type, const std::string& name, const IInterface* parent)
  : HLTSeedingRoIToolDefs::eFexEM::UnpackingBaseClass(type, name, parent) {}
};

class eFexTauRoIsUnpackingTool : public HLTSeedingRoIToolDefs::eFexTau::UnpackingBaseClass {
public:
  eFexTauRoIsUnpackingTool(const std::string& type, const std::string& name, const IInterface* parent)
  : HLTSeedingRoIToolDefs::eFexTau::UnpackingBaseClass(type, name, parent) {}
};

class jFexTauRoIsUnpackingTool : public HLTSeedingRoIToolDefs::jFexTau::UnpackingBaseClass {
public:
  jFexTauRoIsUnpackingTool(const std::string& type, const std::string& name, const IInterface* parent)
  : HLTSeedingRoIToolDefs::jFexTau::UnpackingBaseClass(type, name, parent) {}
};

class cTauRoIsUnpackingTool : public HLTSeedingRoIToolDefs::cTau::UnpackingBaseClass {
public:
  cTauRoIsUnpackingTool(const std::string& type, const std::string& name, const IInterface* parent)
  : HLTSeedingRoIToolDefs::cTau::UnpackingBaseClass(type, name, parent) {}
};

class jFexSRJetRoIsUnpackingTool : public HLTSeedingRoIToolDefs::jFexSRJet::UnpackingBaseClass {
public:
  jFexSRJetRoIsUnpackingTool(const std::string& type, const std::string& name, const IInterface* parent)
  : HLTSeedingRoIToolDefs::jFexSRJet::UnpackingBaseClass(type, name, parent) {}
};

class jFexLRJetRoIsUnpackingTool : public HLTSeedingRoIToolDefs::jFexLRJet::UnpackingBaseClass {
public:
  jFexLRJetRoIsUnpackingTool(const std::string& type, const std::string& name, const IInterface* parent)
  : HLTSeedingRoIToolDefs::jFexLRJet::UnpackingBaseClass(type, name, parent) {}
};

class gFexSRJetRoIsUnpackingTool : public HLTSeedingRoIToolDefs::gFexSRJet::UnpackingBaseClass {
public:
  gFexSRJetRoIsUnpackingTool(const std::string& type, const std::string& name, const IInterface* parent)
  : HLTSeedingRoIToolDefs::gFexSRJet::UnpackingBaseClass(type, name, parent) {}
};

class gFexLRJetRoIsUnpackingTool : public HLTSeedingRoIToolDefs::gFexLRJet::UnpackingBaseClass {
public:
  gFexLRJetRoIsUnpackingTool(const std::string& type, const std::string& name, const IInterface* parent)
  : HLTSeedingRoIToolDefs::gFexLRJet::UnpackingBaseClass(type, name, parent) {}
};

class MuonRoIsUnpackingTool : public HLTSeedingRoIToolDefs::Muon::UnpackingBaseClass {
public:
  MuonRoIsUnpackingTool(const std::string& type, const std::string& name, const IInterface* parent)
  : HLTSeedingRoIToolDefs::Muon::UnpackingBaseClass(type, name, parent) {}
};
/// @}

#endif //> !HLTSEEDING_ROISUNPACKINGTOOLPHASE1_H
