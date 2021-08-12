/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "MURoIsUnpackingTool.h"
#include "TrigT1Result/RoIBResult.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "StoreGate/ReadDecorHandle.h"

MURoIsUnpackingTool::MURoIsUnpackingTool( const std::string& type, 
            const std::string& name, 
            const IInterface* parent )
  : RoIsUnpackingToolBase  ( type, name, parent ) {}

StatusCode MURoIsUnpackingTool::initialize() {
  CHECK( RoIsUnpackingToolBase::initialize() );
  CHECK( m_recRoIsKey.initialize(SG::AllowEmpty) );
  CHECK( m_thresholdPatternsKey.initialize(!m_thresholdPatternsKey.empty()) );
  CHECK( m_recRpcRoITool.retrieve() );
  CHECK( m_recTgcRoITool.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode MURoIsUnpackingTool::start() {
  ATH_CHECK( decodeMapping( [](const std::string& name ){ return name.find("MU") == 0 or name.find(getProbeThresholdName("MU")) == 0;  } ) );
  return StatusCode::SUCCESS;
}

StatusCode MURoIsUnpackingTool::unpack( const EventContext& ctx,
                                        const ROIB::RoIBResult& roib,
                                        const HLT::IDSet& activeChains ) const {
  using namespace TrigCompositeUtils;

  // create and record the collections needed
  SG::WriteHandle<TrigRoiDescriptorCollection> trigRoIs = createAndStoreNoAux(m_trigRoIsKey, ctx ); 
  SG::WriteHandle< DataVector<LVL1::RecMuonRoI> > recRoIs = createAndStoreNoAux( m_recRoIsKey, ctx );
  SG::WriteHandle<DecisionContainer> decisionOutput = createAndStore(m_decisionsKey, ctx ); 
  SG::WriteHandle<DecisionContainer> decisionOutputProbe = createAndStore(m_decisionsKeyProbe, ctx ); 

  // Retrieve the L1 menu configuration
  SG::ReadHandle<TrigConf::L1Menu> l1Menu = SG::makeHandle(m_l1MenuKey, ctx);
  ATH_CHECK(l1Menu.isValid());
  std::optional<ThrVecRef> muThresholds;
  ATH_CHECK(getL1Thresholds(*l1Menu, "MU", muThresholds));

  for ( auto& roi : roib.muCTPIResult().roIVec() ) {    
    const uint32_t roIWord = roi.roIWord();
    unsigned int thresholdNumber = roi.pt();
    ATH_MSG_DEBUG( "MUON RoI with the threshold number: " << thresholdNumber );
    if ( thresholdNumber < 1 or thresholdNumber > 6 ) {
      ATH_MSG_WARNING( "Incorrect threshold number, should be between 1 and 6 but is: "
           << thresholdNumber << ", force setting it to 1" );
      thresholdNumber = 1;
    }

    recRoIs->push_back( std::make_unique<LVL1::RecMuonRoI>(roIWord, m_recRpcRoITool.get(), m_recTgcRoITool.get(), l1Menu.cptr()) );
    const LVL1::RecMuonRoI* recRoI = recRoIs->back();
    
    trigRoIs->push_back( std::make_unique<TrigRoiDescriptor>(
      roIWord, 0u ,0u,
      recRoI->eta(), recRoI->eta()-m_roIWidth, recRoI->eta()+m_roIWidth,
      recRoI->phi(), recRoI->phi()-m_roIWidth, recRoI->phi()+m_roIWidth) );
    
    ATH_MSG_DEBUG( "RoI word: 0x" << MSG::hex << std::setw( 8 ) << roIWord );
    
    // The hltSeedingNodeName() denotes an initial node with no parents
    Decision* decisionMain  = TrigCompositeUtils::newDecisionIn( decisionOutput.ptr(), hltSeedingNodeName() );
    decisionMain->setObjectLink( initialRoIString(), ElementLink<TrigRoiDescriptorCollection>( m_trigRoIsKey.key(), trigRoIs->size()-1 ) );
    decisionMain->setObjectLink( initialRecRoIString(), ElementLink<DataVector<LVL1::RecMuonRoI>>( m_recRoIsKey.key(), recRoIs->size()-1 ) );

    Decision* decisionProbe  = TrigCompositeUtils::newDecisionIn( decisionOutputProbe.ptr(), hltSeedingNodeName() );
    decisionProbe->setObjectLink( initialRoIString(), ElementLink<TrigRoiDescriptorCollection>( m_trigRoIsKey.key(), trigRoIs->size()-1 ) );
    decisionProbe->setObjectLink( initialRecRoIString(), ElementLink<DataVector<LVL1::RecMuonRoI>>( m_recRoIsKey.key(), recRoIs->size()-1 ) );
    
    for (const auto th : muThresholds.value().get()) {
      if ( th->mapping() < thresholdNumber )  {
        //th->thresholdNumber() is defined to be [0,5] and thresholdNumber [0,6]
        const std::string thresholdProbeName = getProbeThresholdName(th->name());
        ATH_MSG_DEBUG( "Passed Threshold names " << th->name() << " and " << thresholdProbeName);
        addChainsToDecision( HLT::Identifier( th->name() ), decisionMain, activeChains );
        addChainsToDecision( HLT::Identifier(thresholdProbeName ), decisionProbe, activeChains );
      }
    }

  }

  // monitoring
  {
    auto RoIsCount = Monitored::Scalar( "count", trigRoIs->size() );
    auto RoIsPhi   = Monitored::Collection( "phi", *trigRoIs, &TrigRoiDescriptor::phi );
    auto RoIsEta   = Monitored::Collection( "eta", *trigRoIs, &TrigRoiDescriptor::eta );
    Monitored::Group( m_monTool,  RoIsCount, RoIsEta, RoIsPhi );
  }
  return StatusCode::SUCCESS;
}

StatusCode MURoIsUnpackingTool::unpack(const EventContext& ctx,
                                       const xAOD::TrigComposite& l1TriggerResult,
                                       const HLT::IDSet& activeChains) const {
  using namespace TrigCompositeUtils;

  // Create and record RoI descriptor and decision containers
  SG::WriteHandle<TrigRoiDescriptorCollection> roiDescriptors = createAndStoreNoAux(m_trigRoIsKey, ctx );
  SG::WriteHandle<DecisionContainer> decisions = createAndStore(m_decisionsKey, ctx );
  SG::WriteHandle<DecisionContainer> decisionsProbe = createAndStore(m_decisionsKeyProbe, ctx );

  // Retrieve the xAOD RoI container from L1TriggerResult
  if (!l1TriggerResult.hasObjectLink(m_muRoILinkName, ClassID_traits<xAOD::MuonRoIContainer>::ID())) {
    ATH_MSG_DEBUG("No muon RoIs in this event");
    return StatusCode::SUCCESS;
  }
  ElementLink<xAOD::MuonRoIContainer> roisLink = l1TriggerResult.objectLink<xAOD::MuonRoIContainer>(m_muRoILinkName);
  ATH_CHECK(roisLink.isValid());
  const xAOD::MuonRoIContainer& rois = roisLink.getStorableObjectRef();

  // Create threshold patterns decoration accessor
  auto thrPatternAcc = SG::makeHandle<uint64_t>(m_thresholdPatternsKey, ctx);
  ATH_CHECK(thrPatternAcc.isPresent());

  // Retrieve the L1 menu configuration
  SG::ReadHandle<TrigConf::L1Menu> l1Menu = SG::makeHandle(m_l1MenuKey, ctx);
  ATH_CHECK(l1Menu.isValid());
  std::optional<ThrVecRef> muThresholds;
  ATH_CHECK(getL1Thresholds(*l1Menu, "MU", muThresholds));

  size_t linkIndex{0};
  for (const xAOD::MuonRoI* roi : rois) {
    // Create new RoI descriptor
    roiDescriptors->push_back(std::make_unique<TrigRoiDescriptor>(
      roi->roiWord(), 0u, 0u,
      roi->eta(), roi->eta()-m_roIWidth, roi->eta()+m_roIWidth,
      roi->phi(), roi->phi()-m_roIWidth, roi->phi()+m_roIWidth
    ));

    // Create new decision and link the RoI objects
    Decision* decision = TrigCompositeUtils::newDecisionIn(decisions.ptr(), hltSeedingNodeName());
    decision->setObjectLink(initialRoIString(),
                            ElementLink<TrigRoiDescriptorCollection>(m_trigRoIsKey.key(), linkIndex, ctx));
    decision->setObjectLink(initialRecRoIString(),
                            ElementLink<xAOD::MuonRoIContainer>(m_muRoILinkName, linkIndex, ctx));

    Decision* decisionProbe = TrigCompositeUtils::newDecisionIn(decisionsProbe.ptr(), hltSeedingNodeName());
    decisionProbe->setObjectLink(initialRoIString(),
                                 ElementLink<TrigRoiDescriptorCollection>(m_trigRoIsKey.key(), linkIndex, ctx));
    decisionProbe->setObjectLink(initialRecRoIString(),
                                 ElementLink<xAOD::MuonRoIContainer>(m_muRoILinkName, linkIndex, ctx));

    // Add positive decisions for chains above the threshold
    uint64_t thresholdPattern = thrPatternAcc(*roi);
    ATH_MSG_DEBUG("RoI #" << linkIndex << " threshold pattern: " << thresholdPattern);
    for (const auto thr : muThresholds.value().get()) {
      if (not (thresholdPattern & (1 << thr->mapping()))) {continue;}
      const std::string thresholdProbeName = getProbeThresholdName(thr->name());
      ATH_MSG_DEBUG("RoI #" << linkIndex << " passed threshold number " << thr->mapping()
                    << " names " << thr->name() << " and " << thresholdProbeName);
      addChainsToDecision(HLT::Identifier(thr->name()), decision, activeChains);
      addChainsToDecision(HLT::Identifier(thresholdProbeName), decisionProbe, activeChains);
      if (msgLvl(MSG::DEBUG)) {
        DecisionIDContainer ids;
        decisionIDs(decision, ids);
        ATH_MSG_DEBUG("Activated chains: " << std::vector<DecisionID>(ids.begin(), ids.end()));
      }
    }

    ++linkIndex;
  }
  return StatusCode::SUCCESS;
}
