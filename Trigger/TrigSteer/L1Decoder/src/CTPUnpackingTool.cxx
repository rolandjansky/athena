/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "DecisionHandling/HLTIdentifier.h"
#include "TrigT1Result/RoIBResult.h"
#include "AthenaMonitoring/Monitored.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/TriggerItem.h"
#include "CTPUnpackingTool.h"

using namespace HLT;


CTPUnpackingTool::CTPUnpackingTool( const std::string& type,
                                    const std::string& name, 
                                    const IInterface* parent ) 
  : CTPUnpackingToolBase(type, name, parent),
    m_configSvc( "TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name ) {
}


StatusCode CTPUnpackingTool::initialize() 
{   
  CHECK( m_configSvc.retrieve() );

  CHECK( CTPUnpackingToolBase::initialize() );
  
  return StatusCode::SUCCESS;
}


StatusCode CTPUnpackingTool::updateConfiguration( const std::map<std::string, std::string>& seeding )  {
  ATH_MSG_DEBUG( "Updating CTP configuration with " << seeding.size() << " seeding mapping");
  // iterate over all items and obtain the CPT ID for each item. Then, package that in the map: name -> CTP ID
  std::map<std::string, size_t> toCTPID;
  for ( const TrigConf::TriggerItem* item:   m_configSvc->ctpConfig()->menu().itemVector() ) {
    toCTPID[item->name()] = item->ctpId();
  }
  
  for ( auto seedingHLTtoL1: seeding ) {
    ATH_MSG_DEBUG( "Seeding " << seedingHLTtoL1.first << " " << seedingHLTtoL1.second );
    CHECK( toCTPID.find( seedingHLTtoL1.second ) != toCTPID.end() ); 
    size_t l1ItemID = toCTPID [ seedingHLTtoL1.second ];
    m_ctpToChain[ l1ItemID ].push_back( HLT::Identifier( seedingHLTtoL1.first ) ); 
  }
  for ( auto ctpIDtoChain: m_ctpToChain ) {
    for ( auto chain: ctpIDtoChain.second ) {
      ATH_MSG_DEBUG( "CTP seed of " << ctpIDtoChain.first << " enables chains " << chain );
    }
  }
    
  return StatusCode::SUCCESS;  
}


StatusCode CTPUnpackingTool::decode( const ROIB::RoIBResult& roib,  HLT::IDVec& enabledChains ) const {
  auto nTAVItems = Monitored::Scalar( "TAVItems", 0 );
  auto nChains   = Monitored::Scalar( "Chains", 0 );
  auto monitorit = Monitored::Group( m_monTool, nTAVItems, nChains );
  auto tav = roib.cTPResult().TAV();
  const size_t tavSize = tav.size();

  for ( size_t wordCounter = 0; wordCounter < tavSize; ++wordCounter ) {
    for ( size_t bitCounter = 0;  bitCounter < 32; ++bitCounter ) {
      const size_t ctpIndex = 32*wordCounter + bitCounter;
      const bool decision = ( tav[wordCounter].roIWord() & (1 << bitCounter) ) > 0;

      if ( decision == true or m_forceEnable ) {
	if ( decision ) {
	  nTAVItems = nTAVItems + 1;	  
	  ATH_MSG_DEBUG( "L1 item " << ctpIndex << " active, enabling chains " 
			 << (m_forceEnable ? " due to the forceEnable flag" : " due to the seed"));
	}

	auto itr = m_ctpToChain.find( ctpIndex );
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


