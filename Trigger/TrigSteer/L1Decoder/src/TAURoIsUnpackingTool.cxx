/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
// L1Decoder includes
#include "TAURoIsUnpackingTool.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "TrigConfL1Data/CTPConfig.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
TAURoIsUnpackingTool::TAURoIsUnpackingTool( const std::string& type, 
              const std::string& name, 
              const IInterface* parent ) 
  : RoIsUnpackingToolBase(type, name, parent),
    m_configSvc( "TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name ){
}


StatusCode TAURoIsUnpackingTool::initialize() {

  ATH_CHECK( RoIsUnpackingToolBase::initialize() );
  ATH_CHECK( m_configSvc.retrieve() );
  ATH_CHECK( m_trigRoIsKey.initialize() );
  ATH_CHECK( m_recRoIsKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode TAURoIsUnpackingTool::start() {
  ATH_CHECK( decodeMapping( [](const std::string& name ){ return name.find("TAU") == 0 or name.find(getProbeThresholdName("TAU")) == 0; } ) );
  // for taus, since there is threshold name change from HA to TAU we need to fill up mapping wiht same threshold but prefixed by HA
  // TODO remove once L1 configuration switches to TAU
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


StatusCode TAURoIsUnpackingTool::updateConfiguration() {
  using namespace TrigConf;

  m_tauThresholds.clear();
  ATH_CHECK( copyThresholds(m_configSvc->thresholdConfig()->getThresholdVector( L1DataDef::TAU ), m_tauThresholds ) );
  return StatusCode::SUCCESS;
}


StatusCode TAURoIsUnpackingTool::finalize() {
  return StatusCode::SUCCESS;
}


StatusCode TAURoIsUnpackingTool::unpack( const EventContext& ctx,
           const ROIB::RoIBResult& roib,
           const HLT::IDSet& activeChains ) const {
  using namespace TrigCompositeUtils;
  SG::WriteHandle<TrigRoiDescriptorCollection> handle1 = createAndStoreNoAux(m_trigRoIsKey, ctx ); 
  auto trigRoIs = handle1.ptr();
  SG::WriteHandle< DataVector<LVL1::RecEmTauRoI> > handle2 = createAndStoreNoAux( m_recRoIsKey, ctx );
  auto recRoIs = handle2.ptr();
  SG::WriteHandle<DecisionContainer> handle3 = createAndStore(m_decisionsKey, ctx ); 
  auto decisionOutput = handle3.ptr();
  SG::WriteHandle<DecisionContainer> handle4 = createAndStore(m_decisionsKeyProbe, ctx ); 
  auto decisionOutputProbe = handle4.ptr();
  
  // RoIBResult contains vector of TAU fragments
  for ( auto& emTauFragment : roib.eMTauResult() ) {
    for ( auto& roi : emTauFragment.roIVec() ) {
      uint32_t roIWord = roi.roIWord();      
      if ( not ( LVL1::TrigT1CaloDefs::TauRoIWordType == roi.roIType() ) )  {
        ATH_MSG_DEBUG( "Skipping RoI as it is not TAU threshold " << roIWord );
        continue;
      }
      
      auto recRoI = new LVL1::RecEmTauRoI( roIWord, &m_tauThresholds );
      recRoIs->push_back( recRoI );
      
      auto trigRoI = new TrigRoiDescriptor( roIWord, 0u ,0u,
              recRoI->eta(), recRoI->eta()-m_roIWidth, recRoI->eta()+m_roIWidth,
              recRoI->phi(), recRoI->phi()-m_roIWidth, recRoI->phi()+m_roIWidth );
      trigRoIs->push_back( trigRoI );
        
      ATH_MSG_DEBUG( "RoI word: 0x" << MSG::hex << std::setw( 8 ) << roIWord << MSG::dec );      
      
      // The l1DecoderNodeName() denotes an initial node with no parents
      Decision* decisionMain = TrigCompositeUtils::newDecisionIn( decisionOutput, l1DecoderNodeName() ); 
      Decision* decisionProbe = TrigCompositeUtils::newDecisionIn( decisionOutputProbe, l1DecoderNodeName() );

      for ( auto th: m_tauThresholds ) {
        ATH_MSG_VERBOSE( "Checking if the threshold " << th->name() << " passed" );
        if ( recRoI->passedThreshold( th->thresholdNumber() ) ) {
          const std::string thresholdProbeName = getProbeThresholdName(th->name());
          ATH_MSG_DEBUG( "Passed Threshold names " << th->name() << " and " << thresholdProbeName);
          addChainsToDecision( HLT::Identifier( th->name() ), decisionMain, activeChains );
          addChainsToDecision( HLT::Identifier( thresholdProbeName ), decisionProbe, activeChains );
        }
      }
      
      // TODO would be nice to have this. Requires modifying the TC class: decision->setDetail( "Thresholds", passedThresholds ); // record passing threshold names ( for easy debugging )            
      decisionMain->setObjectLink( initialRoIString(), ElementLink<TrigRoiDescriptorCollection>( m_trigRoIsKey.key(), trigRoIs->size()-1 ) );
      decisionMain->setObjectLink( initialRecRoIString(), ElementLink<DataVector<LVL1::RecEmTauRoI>>( m_recRoIsKey.key(), recRoIs->size()-1 ) );

      decisionProbe->setObjectLink( initialRoIString(), ElementLink<TrigRoiDescriptorCollection>( m_trigRoIsKey.key(), trigRoIs->size()-1 ) );
      decisionProbe->setObjectLink( initialRecRoIString(), ElementLink<DataVector<LVL1::RecEmTauRoI>>( m_recRoIsKey.key(), recRoIs->size()-1 ) );
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
