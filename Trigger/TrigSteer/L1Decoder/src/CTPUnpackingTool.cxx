/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "DecisionHandling/HLTIdentifier.h"
#include "TrigT1Result/RoIBResult.h"
#include "CTPUnpackingTool.h"

using namespace HLT;


CTPUnpackingTool::CTPUnpackingTool( const std::string& type,
				    const std::string& name, 
				    const IInterface* parent ) 
  : AthAlgTool(type, name, parent) {
  declareProperty("CTPToChainMapping", m_ctpToChainProperty, "Mapping of the form: '34:HLT_x', '35:HLT_y', ..., both CTP ID and chain may appear many times");
  declareProperty("ForceEnableAllChains", m_forceEnable=false, "Enables all chains in each event, testing mode");
  declareProperty("MonTool", m_monTool=VoidMonitoringTool(this), "Basic Monitoring");
}

CTPUnpackingTool::~CTPUnpackingTool()
{}

StatusCode CTPUnpackingTool::initialize() {   
  if ( !m_monTool.empty() ) CHECK( m_monTool.retrieve() );
  CHECK( decodeCTPToChainMapping() ); 
  if ( m_ctpToChain.empty() ) {
    ATH_MSG_WARNING( "Empty CTP to chains mapping " );
    ATH_MSG_WARNING( "Property confoguring it: " << m_ctpToChainProperty );
  } 
  for ( auto m: m_ctpToChain ) {
    ATH_MSG_INFO( "Mapping of CTP bit: " << m.first << " to chains " << m.second );
  }
  return StatusCode::SUCCESS;
}



StatusCode CTPUnpackingTool::decode( const ROIB::RoIBResult& roib,  HLT::IDVec& enabledChains ) const {
  using namespace Monitored;
  auto nTAVItems = MonitoredScalar::declare("TAVItems", 0);
  auto nChains   = MonitoredScalar::declare("Chains", 0);
  auto monitorit = MonitoredScope::declare(m_monTool, nTAVItems, nChains);
  auto tav = roib.cTPResult().TAV();
  const size_t tavSize = tav.size();

  for ( size_t wordCounter = 0; wordCounter < tavSize; ++wordCounter ) {
    for ( size_t bitCounter = 0;  bitCounter < 32; ++bitCounter ) {
      const size_t ctpIndex = 32*wordCounter + bitCounter;
      const bool decision = ( tav[wordCounter].roIWord() & (1 << bitCounter) ) > 0;

      if ( decision == true or m_forceEnable ) {
	if ( decision ) {
	  nTAVItems = nTAVItems + 1;	  
	  ATH_MSG_DEBUG( "L1 item " << ctpIndex << " active, enabling chains");
	}

	auto itr = m_ctpToChain.find(ctpIndex);
	if ( itr != m_ctpToChain.end() ) {
	  enabledChains.insert( enabledChains.end(), itr->second.begin(), itr->second.end() );

	}
      }
    }    
  }
  nChains = enabledChains.size();
  for ( auto chain: enabledChains ) {
    ATH_MSG_DEBUG( "Enabling chain: " << chain );
  }
  if ( nTAVItems == 0 ) {
    ATH_MSG_ERROR( "All CTP bits were disabled, this event shoudl not have shown here" );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


