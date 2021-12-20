/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "MURoIsUnpackingTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigT1Result/RoIBResult.h"

#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadDecorHandle.h"


MURoIsUnpackingTool::MURoIsUnpackingTool( const std::string& type,
            const std::string& name,
            const IInterface* parent )
  : RoIsUnpackingToolBase  ( type, name, parent ) {}


StatusCode MURoIsUnpackingTool::initialize() {
  CHECK( RoIsUnpackingToolBase::initialize() );
  CHECK( m_recRoIsKey.initialize(SG::AllowEmpty) );
  CHECK( m_recRpcRoITool.retrieve() );
  CHECK( m_recTgcRoITool.retrieve() );
  return StatusCode::SUCCESS;
}


StatusCode MURoIsUnpackingTool::start() {
  ATH_CHECK( decodeMapping([](const std::string& name){
    return name.find("MU") == 0 or name.find(getProbeThresholdName("MU")) == 0;
  }));
  return StatusCode::SUCCESS;
}


StatusCode MURoIsUnpackingTool::unpack(const EventContext& ctx,
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

  for ( const auto & roi : roib.muCTPIResult().roIVec() ) {
    const uint32_t roIWord = roi.roIWord();
    unsigned int thresholdNumber = roi.pt();
    ATH_MSG_DEBUG( "MUON RoI with the threshold number: " << thresholdNumber );
    if ( thresholdNumber < 1 or thresholdNumber > 6 ) {
      ATH_MSG_WARNING( "Incorrect threshold number, should be between 1 and 6 but is: "
           << thresholdNumber << ", force setting it to 1" );
      thresholdNumber = 1;
    }

    recRoIs->push_back(std::make_unique<LVL1::RecMuonRoI>(roIWord,
                                                          m_recRpcRoITool.get(),
                                                          m_recTgcRoITool.get(),
                                                          l1Menu.cptr()));
    const LVL1::RecMuonRoI* recRoI = recRoIs->back();

    trigRoIs->push_back( std::make_unique<TrigRoiDescriptor>(
      roIWord, 0u ,0u,
      recRoI->eta(), recRoI->eta()-m_roIWidth, recRoI->eta()+m_roIWidth,
      recRoI->phi(), recRoI->phi()-m_roIWidth, recRoI->phi()+m_roIWidth) );

    ATH_MSG_DEBUG( "RoI word: 0x" << MSG::hex << std::setw( 8 ) << roIWord );

    // The hltSeedingNodeName() denotes an initial node with no parents
    Decision* decisionMain  = TrigCompositeUtils::newDecisionIn( decisionOutput.ptr(), hltSeedingNodeName() );
    decisionMain->setObjectLink( initialRoIString(),
                                 ElementLink<TrigRoiDescriptorCollection>(m_trigRoIsKey.key(), trigRoIs->size()-1) );
    decisionMain->setObjectLink( initialRecRoIString(),
                                 ElementLink<DataVector<LVL1::RecMuonRoI>>(m_recRoIsKey.key(), recRoIs->size()-1) );

    Decision* decisionProbe  = TrigCompositeUtils::newDecisionIn( decisionOutputProbe.ptr(), hltSeedingNodeName() );
    decisionProbe->setObjectLink( initialRoIString(),
                                  ElementLink<TrigRoiDescriptorCollection>(m_trigRoIsKey.key(), trigRoIs->size()-1) );
    decisionProbe->setObjectLink( initialRecRoIString(),
                                  ElementLink<DataVector<LVL1::RecMuonRoI>>(m_recRoIsKey.key(), recRoIs->size()-1) );

    for (const auto& th : muThresholds.value().get()) {
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
