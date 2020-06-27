/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigT1Result/JetEnergyResult.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "FSRoIsUnpackingTool.h"

FSRoIsUnpackingTool::FSRoIsUnpackingTool( const std::string& type,
					    const std::string& name,
					    const IInterface* parent )
  : RoIsUnpackingToolBase(type, name, parent),
    m_configSvc( "TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name )
{}

StatusCode FSRoIsUnpackingTool::initialize()
{
  ATH_CHECK( m_fsRoIKey.initialize() );

  ATH_CHECK(RoIsUnpackingToolBase::initialize());
  // for ( auto th2chains: m_thresholdToChainMapping ) {
  //   m_allFSChains.insert( th2chains.second.begin(), th2chains.second.end() );
  // }

  CHECK( m_configSvc.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode FSRoIsUnpackingTool::start() {
  ATH_CHECK( decodeMapping( [](const std::string& name ){ return
	  name.find("FS") == 0 or
	  name == ""; } ) );

  return StatusCode::SUCCESS;
}


StatusCode FSRoIsUnpackingTool::updateConfiguration() {
  using namespace TrigConf;
  m_allFSChains.clear();
  
  for ( auto thresholdToChain: m_thresholdToChainMapping ) {
    m_allFSChains.insert( thresholdToChain.second.begin(), thresholdToChain.second.end() );
  }

  for ( auto id: m_allFSChains ) {
    ATH_MSG_DEBUG( "FS Chain " << id  );
  }

  return StatusCode::SUCCESS;
}


StatusCode FSRoIsUnpackingTool::unpack( const EventContext& ctx,
					const ROIB::RoIBResult& /*roib*/,
					const HLT::IDSet& activeChains ) const {
  using namespace TrigCompositeUtils;
  SG::WriteHandle<DecisionContainer> handle = createAndStore(m_decisionsKey, ctx );
  auto decisionOutput = handle.ptr();

  HLT::IDSet activeFSchains;
  // see if any chain we care of is active
  std::set_intersection(activeChains.begin(), activeChains.end(),
  			m_allFSChains.begin(), m_allFSChains.end(),
			std::inserter(activeFSchains, activeFSchains.end() ) );

  auto decision  = TrigCompositeUtils::newDecisionIn( decisionOutput, "L1" ); // This "L1" denotes an initial node with no parents
  addChainsToDecision( HLT::Identifier( "FSNOSEED" ), decision, activeChains );

  ATH_MSG_DEBUG("Unpacking FS RoI for " << activeFSchains.size() << " chains: " << [&](){ 
      TrigCompositeUtils::DecisionIDContainer ids; 
      TrigCompositeUtils::decisionIDs( decision, ids ); 
      return std::vector<TrigCompositeUtils::DecisionID>( ids.begin(), ids.end() ); }() );
  

  std::unique_ptr<TrigRoiDescriptorCollection> fsRoIsColl = std::make_unique<TrigRoiDescriptorCollection>();
  TrigRoiDescriptor* fsRoI = new TrigRoiDescriptor( true ); // true == FS
  fsRoIsColl->push_back( fsRoI );

  auto roiHandle = SG::makeHandle( m_fsRoIKey, ctx );
  ATH_CHECK( roiHandle.record ( std::move( fsRoIsColl ) ) );

  ATH_MSG_DEBUG("Linking to FS RoI descriptor");
  decision->setObjectLink( initialRoIString(), ElementLink<TrigRoiDescriptorCollection>( m_fsRoIKey.key(), 0 ) );  

  return StatusCode::SUCCESS;
}
