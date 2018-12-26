/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigT1Result/JetEnergyResult.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigConfL1Data/CTPConfig.h"
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


  CHECK( m_configSvc.retrieve() );
  
  return StatusCode::SUCCESS;
}

StatusCode METRoIsUnpackingTool::updateConfiguration( const IRoIsUnpackingTool::SeedingMap& seeding ) {
  using namespace TrigConf;  
  ATH_CHECK( decodeMapping( [](const TriggerThreshold* th){ return L1DataDef::TE == th->ttype() or L1DataDef::XE == th->ttype() or  L1DataDef::XS == th->ttype(); }, 
			    m_configSvc->ctpConfig()->menu().itemVector(),
			    seeding ) );

  for ( auto thresholdToChain: m_thresholdToChainMapping ) {
    m_allMETChains.insert( thresholdToChain.second.begin(), thresholdToChain.second.end() );
  }

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
  SG::WriteHandle<DecisionContainer> handle = createAndStore(m_decisionsKey, ctx ); 
  auto decisionOutput = handle.ptr();

  HLT::IDSet activeMETchains;
  // see if any chain we care of is active
  std::set_intersection(activeChains.begin(), activeChains.end(), 
  			m_allMETChains.begin(), m_allMETChains.end(),
			std::inserter(activeMETchains, activeMETchains.end() ) );
			
  auto decision  = TrigCompositeUtils::newDecisionIn( decisionOutput );
  for ( auto c: activeMETchains ) addDecisionID( c, decision );

  ATH_MSG_DEBUG("Unpacking MET RoI for " << activeMETchains.size() << " chains");

  ATH_MSG_DEBUG("Linking to FS RoI descriptor");  
  decision->setObjectLink( "initialRoI", ElementLink<TrigRoiDescriptorCollection>( m_fsRoIKey, 0 ) );
  
  // check the MET RoI, TODO unpack and create L1 MET object (only if turns out to be needed)
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
  return StatusCode::SUCCESS;
}
