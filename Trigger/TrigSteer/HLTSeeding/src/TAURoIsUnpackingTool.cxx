/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "TAURoIsUnpackingTool.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "AthenaMonitoringKernel/Monitored.h"


TAURoIsUnpackingTool::TAURoIsUnpackingTool(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent)
  : RoIsUnpackingToolBase(type, name, parent) {}


StatusCode TAURoIsUnpackingTool::initialize() {
  ATH_CHECK( RoIsUnpackingToolBase::initialize() );
  ATH_CHECK( m_recRoIsKey.initialize() );
  return StatusCode::SUCCESS;
}


StatusCode TAURoIsUnpackingTool::start() {
  ATH_CHECK(decodeMapping([](const std::string& name ){
    return name.find("TAU") == 0 or name.find(getProbeThresholdName("TAU")) == 0;
  }));
  // For Taus, since there is threshold name change from HA to TAU we need to fill the mapping with same threshold
  // but prefixed by HA
  // TODO: Remove once L1 configuration switches to TAU
  for ( const auto& [threshold, chains] : m_thresholdToChainMapping ) {
    if ( threshold.name().find("TAU") != std::string::npos ) {
      std::string newThresholdName = threshold.name();
      newThresholdName.replace(threshold.name().find("TAU"), 3, "HA");
      ATH_MSG_INFO("Temporary fix due to renaming the HA to TAU thresholds, adding decoding of " << newThresholdName );
      m_thresholdToChainMapping[HLT::Identifier(newThresholdName)] = chains;
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode TAURoIsUnpackingTool::unpack(const EventContext& ctx,
                                        const ROIB::RoIBResult& roib,
                                        const HLT::IDSet& activeChains) const {
  using namespace TrigCompositeUtils;

  // create and record the collections needed
  SG::WriteHandle<TrigRoiDescriptorCollection> trigRoIs = createAndStoreNoAux(m_trigRoIsKey, ctx );
  SG::WriteHandle< DataVector<LVL1::RecEmTauRoI> > recRoIs = createAndStoreNoAux( m_recRoIsKey, ctx );
  SG::WriteHandle<DecisionContainer> decisionOutput = createAndStore(m_decisionsKey, ctx );
  SG::WriteHandle<DecisionContainer> decisionOutputProbe = createAndStore(m_decisionsKeyProbe, ctx );

  // Retrieve the L1 menu configuration
  SG::ReadHandle<TrigConf::L1Menu> l1Menu = SG::makeHandle(m_l1MenuKey, ctx);
  ATH_CHECK(l1Menu.isValid());
  std::optional<ThrVecRef> tauThresholds;
  ATH_CHECK(getL1Thresholds(*l1Menu, "TAU", tauThresholds));

  // RoIBResult contains vector of TAU fragments
  for ( const auto & emTauFragment : roib.eMTauResult() ) {
    for ( const auto & roi : emTauFragment.roIVec() ) {
      uint32_t roIWord = roi.roIWord();
      if ( not ( LVL1::TrigT1CaloDefs::TauRoIWordType == roi.roIType() ) )  {
        ATH_MSG_DEBUG( "Skipping RoI as it is not TAU threshold " << roIWord );
        continue;
      }

      recRoIs->push_back( std::make_unique<LVL1::RecEmTauRoI>(roIWord, l1Menu.cptr()) );
      const LVL1::RecEmTauRoI* recRoI = recRoIs->back();

      trigRoIs->push_back( std::make_unique<TrigRoiDescriptor>(
        roIWord, 0u ,0u,
        recRoI->eta(), recRoI->eta()-m_roIWidthEta, recRoI->eta()+m_roIWidthEta,
        recRoI->phi(), recRoI->phi()-m_roIWidthPhi, recRoI->phi()+m_roIWidthPhi) );

      ATH_MSG_DEBUG( "RoI word: 0x" << MSG::hex << std::setw( 8 ) << roIWord << MSG::dec );

      // The hltSeedingNodeName() denotes an initial node with no parents
      Decision* decisionMain = TrigCompositeUtils::newDecisionIn( decisionOutput.ptr(), hltSeedingNodeName() );
      Decision* decisionProbe = TrigCompositeUtils::newDecisionIn( decisionOutputProbe.ptr(), hltSeedingNodeName() );

      std::vector<TrigCompositeUtils::DecisionID> passedThresholdIDs;

      for (const auto& th : tauThresholds.value().get()) {
        ATH_MSG_VERBOSE( "Checking if the threshold " << th->name() << " passed" );
        if ( recRoI->passedThreshold( th->mapping() ) ) {
          passedThresholdIDs.push_back(HLT::Identifier(th->name()));
          const std::string thresholdProbeName = getProbeThresholdName(th->name());
          ATH_MSG_DEBUG( "Passed Threshold names " << th->name() << " and " << thresholdProbeName);
          addChainsToDecision( HLT::Identifier( th->name() ), decisionMain, activeChains );
          addChainsToDecision( HLT::Identifier( thresholdProbeName ), decisionProbe, activeChains );
        }
      }

      decisionMain->setDetail("thresholds", passedThresholdIDs);
      decisionMain->setObjectLink( initialRoIString(),
                                   ElementLink<TrigRoiDescriptorCollection>(m_trigRoIsKey.key(), trigRoIs->size()-1) );
      decisionMain->setObjectLink( initialRecRoIString(),
                                   ElementLink<DataVector<LVL1::RecEmTauRoI>>(m_recRoIsKey.key(), recRoIs->size()-1) );

      decisionProbe->setDetail("thresholds", passedThresholdIDs);
      decisionProbe->setObjectLink( initialRoIString(),
                                    ElementLink<TrigRoiDescriptorCollection>(m_trigRoIsKey.key(), trigRoIs->size()-1) );
      decisionProbe->setObjectLink( initialRecRoIString(),
                                    ElementLink<DataVector<LVL1::RecEmTauRoI>>(m_recRoIsKey.key(), recRoIs->size()-1) );
    }
  }

  for ( auto roi: *trigRoIs ) {
    ATH_MSG_DEBUG( "RoI Eta: " << roi->eta() << " Phi: " << roi->phi() << " RoIWord: " << roi->roiWord() );
  }

  // monitoring
  {
    auto RoIsCount = Monitored::Scalar( "count", trigRoIs->size() );
    auto RoIsPhi   = Monitored::Collection( "phi", *trigRoIs, &TrigRoiDescriptor::phi );
    auto RoIsEta   = Monitored::Collection( "eta", *trigRoIs, &TrigRoiDescriptor::eta );
    Monitored::Group( m_monTool,  RoIsCount, RoIsEta, RoIsPhi );
  }

  ATH_MSG_DEBUG( "Unpacked " <<  trigRoIs->size() << " RoIs" );

  return StatusCode::SUCCESS;
}
