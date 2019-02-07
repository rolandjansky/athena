/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "MURoIsUnpackingTool.h"
#include "TrigT1Result/RoIBResult.h"
#include "AthenaMonitoring/Monitored.h"
#include "TrigConfL1Data/CTPConfig.h"

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

StatusCode MURoIsUnpackingTool::updateConfiguration( const IRoIsUnpackingTool::SeedingMap& seeding ) {
  using namespace TrigConf;

  ATH_CHECK( decodeMapping( [](const TriggerThreshold* th){ return th->ttype() == L1DataDef::MUON; }, 
			    m_configSvc->ctpConfig()->menu().itemVector(),
			    seeding ) );

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
  // create and record the collections needed
  SG::WriteHandle<TrigRoiDescriptorCollection> handle1 = createAndStoreNoAux(m_trigRoIsKey, ctx ); 
  auto trigRoIs = handle1.ptr();
  SG::WriteHandle< DataVector<LVL1::RecMuonRoI> > handle2 = createAndStoreNoAux( m_recRoIsKey, ctx );
  auto recRoIs = handle2.ptr();
  SG::WriteHandle<DecisionContainer> handle3 = createAndStore(m_decisionsKey, ctx ); 
  auto decisionOutput = handle3.ptr();

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
    
    auto decision  = TrigCompositeUtils::newDecisionIn( decisionOutput );
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
    auto RoIsCount = Monitored::Scalar( "count", trigRoIs->size() );
    auto RoIsPhi   = Monitored::Collection( "phi", *trigRoIs, &TrigRoiDescriptor::phi );
    auto RoIsEta   = Monitored::Collection( "eta", *trigRoIs, &TrigRoiDescriptor::eta );
    Monitored::Group( m_monTool,  RoIsCount, RoIsEta, RoIsPhi );
  }
  return StatusCode::SUCCESS;
}



