/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigT1Result/JetEnergyResult.h"
#include "TrigT1Result/RoIBResult.h"
#include "METRoIsUnpackingTool.h"

METRoIsUnpackingTool::METRoIsUnpackingTool( const std::string& type, 
					    const std::string& name, 
					    const IInterface* parent ) 
  : RoIsUnpackingToolBase(type, name, parent),
    m_configSvc( "TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name )
{}

StatusCode METRoIsUnpackingTool::initialize()
{
  ATH_CHECK(RoIsUnpackingToolBase::initialize());
  for ( auto th2chains: m_thresholdToChainMapping ) {
    m_allMETChains.insert( th2chains.second.begin(), th2chains.second.end() );
  }


  CHECK( m_trigRoIsKey.initialize() );
  CHECK( m_configSvc.retrieve() );
  
  return StatusCode::SUCCESS;
}

StatusCode METRoIsUnpackingTool::updateConfiguration() {
  using namespace TrigConf;
  const ThresholdConfig* thresholdConfig = m_configSvc->thresholdConfig();
  std::vector<const TriggerThreshold*> filtered;
  {
    auto insert = [&](  const std::vector<TriggerThreshold*>& src ) -> void { filtered.insert(filtered.end(),  src.begin(), src.end());  };
    insert( thresholdConfig->getThresholdVector( L1DataDef::TE ) );
    insert( thresholdConfig->getThresholdVector( L1DataDef::XE ) );
    insert( thresholdConfig->getThresholdVector( L1DataDef::XS ) );
  }

  for ( auto th: filtered ) {
    if ( th != nullptr ) {
      ATH_MSG_INFO( "Found threshold in the configuration: " << th->name() << " of ID: " << HLT::Identifier( th->name() ).numeric() ); 
      m_thresholds.push_back( th );      
    } else {
      ATH_MSG_DEBUG( "Nullptr to the threshold" ); 
    }
  }
  
  if ( m_thresholds.empty() ) {
    ATH_MSG_INFO("No MET thresholds configured");
  }

  return StatusCode::SUCCESS;
}


StatusCode METRoIsUnpackingTool::unpack( const EventContext& ctx,
					const ROIB::RoIBResult& roib,
					const HLT::IDSet& activeChains ) const {
  using namespace TrigCompositeUtils;
  auto decisionOutput = std::make_unique<DecisionContainer>();
  auto decisionAux    = std::make_unique<DecisionAuxContainer>();
  decisionOutput->setStore( decisionAux.get() );  
  auto trigRoIs = std::make_unique< TrigRoiDescriptorCollection >();
  HLT::IDSet activeMETchains;
  // see if any chain we care of is active
  std::set_intersection(activeChains.begin(), activeChains.end(), 
			m_allMETChains.begin(), m_allMETChains.end(), 
			std::inserter(activeMETchains, activeMETchains.end() ) );
			
  auto decision  = TrigCompositeUtils::newDecisionIn( decisionOutput.get() );
  for ( auto c: activeMETchains ) addDecisionID( c, decision );
  ATH_MSG_DEBUG("Unpacking MET RoI for " << activeMETchains.size() << " chains");
  trigRoIs->push_back( new TrigRoiDescriptor(true) );
  ATH_MSG_DEBUG("Created FS RoI descriptor");  
  decision->setObjectLink( "initialRoI", ElementLink<TrigRoiDescriptorCollection>( m_trigRoIsKey.key(), 0 ) );
	
  // check the MET RoI, TODO unpack anc create L1 MET object (only if turns out to be needed)
  bool foundMETRoI = false;
  const std::vector<ROIB::JetEnergyResult>& jetEnergyResult = roib.jetEnergyResult();
  for ( const ROIB::JetEnergyResult& jeResult: jetEnergyResult ) {
    for ( const ROIB::JetEnergyRoI& roi: jeResult.roIVec() ) {
      uint32_t roIWord = roi.roIWord();
      LVL1::TrigT1CaloDefs::RoIType roiType = m_jepDecoder.roiType( roIWord );
      if ( roiType == LVL1::TrigT1CaloDefs::EnergyRoIWordType0 ) {
	foundMETRoI = true;
	break;	
      }
    }
  }

    // inconsistency, active MET chains, yet missing MET RoI
  if ( (not activeMETchains.empty()) and not foundMETRoI) { 
    ATH_MSG_WARNING( "" << activeMETchains.size() << " active MET chains while missing  MET RoI" );
  }
  {
    auto handle = SG::makeHandle( m_trigRoIsKey, ctx );
    CHECK( handle.record ( std::move( trigRoIs ) ) );
  }
  {
    auto handle = SG::makeHandle( m_decisionsKey, ctx );
    CHECK ( handle.record( std::move( decisionOutput ), std::move( decisionAux )  ) );
  }

  return StatusCode::SUCCESS;
}
