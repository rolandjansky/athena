/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "MURoIsUnpackingTool.h"
#include "TrigT1Result/RoIBResult.h"
#include "AthenaMonitoring/MonitoredScope.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
MURoIsUnpackingTool::MURoIsUnpackingTool( const std::string& type, 
					  const std::string& name, 
					  const IInterface* parent )
  : RoIsUnpackingToolBase  ( type, name, parent ),
    m_configSvc( "TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name ),
    m_recRpcRoISvc( "LVL1RPC::RPCRecRoiSvc/LVL1RPC::RPCRecRoiSvc", name ),
    m_recTgcRoISvc( "LVL1TGC::TGCRecRoiSvc/LVL1TGC::TGCRecRoiSvc", name )
{
}


StatusCode MURoIsUnpackingTool::initialize() {

  CHECK( RoIsUnpackingToolBase::initialize() );
  CHECK( m_configSvc.retrieve() );
  CHECK( m_trigRoIsKey.initialize() );
  CHECK( m_recRoIsKey.initialize() );
  CHECK( m_recRpcRoISvc.retrieve() );
  CHECK( m_recTgcRoISvc.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode MURoIsUnpackingTool::updateConfiguration() {
  using namespace TrigConf;
  const ThresholdConfig* thresholdConfig = m_configSvc->thresholdConfig();
  for ( TriggerThreshold * th : thresholdConfig->getThresholdVector( L1DataDef::MUON ) ) {
    if ( th != nullptr ) {
      ATH_MSG_DEBUG( "Found threshold in the configuration: " << th->name() << " of ID: " << HLT::Identifier( th->name() ).numeric() ); 
      m_muonThresholds.push_back( th );    
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode MURoIsUnpackingTool::unpack( const EventContext& ctx,
                                        const ROIB::RoIBResult& roib,
                                        const HLT::IDSet& activeChains ) const {
  using namespace TrigCompositeUtils;
  auto decisionOutput = std::make_unique<DecisionContainer>();
  auto decisionAux    = std::make_unique<DecisionAuxContainer>();
  decisionOutput->setStore( decisionAux.get() );

  auto trigRoIs = std::make_unique< TrigRoiDescriptorCollection >();
  auto recRoIs  = std::make_unique< DataVector<LVL1::RecMuonRoI> >();

  for ( auto& roi : roib.muCTPIResult().roIVec() ) {    
    const uint32_t roIWord = roi.roIWord();
    int thresholdNumber = roi.pt();
    ATH_MSG_DEBUG( "MUON RoI with the threshold number: " << thresholdNumber );
    if ( thresholdNumber < 1 or thresholdNumber > 6 ) {
      ATH_MSG_WARNING( "Incorrect threshold number, should be between 1 and 6 but is: "
		       << thresholdNumber << ", force setting it to 1" );
      thresholdNumber = 1;
    }
    LVL1::RecMuonRoI* recRoI = new LVL1::RecMuonRoI( roIWord, m_recRpcRoISvc.get(), m_recTgcRoISvc.get(), &m_muonThresholds );
    recRoIs->push_back( recRoI );
    auto trigRoI = new TrigRoiDescriptor( roIWord, 0u ,0u,
					  recRoI->eta(), recRoI->eta()-m_roIWidth, recRoI->eta()+m_roIWidth,
					  recRoI->phi(), recRoI->phi()-m_roIWidth, recRoI->phi()+m_roIWidth );
    trigRoIs->push_back( trigRoI );
    
    ATH_MSG_DEBUG( "RoI word: 0x" << MSG::hex << std::setw( 8 ) << roIWord );
    
    auto decision  = TrigCompositeUtils::newDecisionIn( decisionOutput.get() );
    decision->setObjectLink( "initialRoI", ElementLink<TrigRoiDescriptorCollection>( m_trigRoIsKey.key(), trigRoIs->size()-1 ) );
    decision->setObjectLink( "initialRecRoI", ElementLink<DataVector<LVL1::RecMuonRoI>>( m_recRoIsKey.key(), recRoIs->size()-1 ) );
    
    for ( auto th: m_muonThresholds ) {
      if ( th->thresholdNumber() <= thresholdNumber )  { // TODO verify if here should be <= or <
	// this code suggests <= https://gitlab.cern.ch/atlas/athena/blob/master/Trigger/TrigSteer/TrigSteering/src/Lvl1ResultAccessTool.cxx#L654
	ATH_MSG_DEBUG( "Threshold passed: " << th->name() );
	addChainsToDecision( HLT::Identifier( th->name() ), decision, activeChains );
	ATH_MSG_DEBUG( "Labeled object with chains: " << [&](){ 
	    TrigCompositeUtils::DecisionIDContainer ids; 
	    TrigCompositeUtils::decisionIDs( decision, ids ); 
	    return std::vector<TrigCompositeUtils::DecisionID>( ids.begin(), ids.end() ); }() );
	
      }
    }

  }
  // monitoring
  {
    using namespace Monitored;
    auto RoIsCount = MonitoredScalar::declare( "count", trigRoIs->size() );
    auto RoIsPhi   = MonitoredCollection::declare( "phi", *trigRoIs.get(), &TrigRoiDescriptor::phi );
    auto RoIsEta   = MonitoredCollection::declare( "eta", *trigRoIs.get(), &TrigRoiDescriptor::eta );
    MonitoredScope::declare( m_monTool,  RoIsCount, RoIsEta, RoIsPhi );
  }
  // recording
  {
    auto handle = SG::makeHandle( m_trigRoIsKey, ctx );
    CHECK( handle.record( std::move( trigRoIs ) ) );
  }
  {
    auto handle = SG::makeHandle( m_recRoIsKey, ctx );
    CHECK( handle.record( std::move( recRoIs ) ) );
  }
  {
    auto handle = SG::makeHandle(  m_decisionsKey, ctx );
    CHECK( handle.record( std::move( decisionOutput ), std::move( decisionAux ) ) );
  }
  return StatusCode::SUCCESS;
}



