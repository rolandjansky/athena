/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "AthenaMonitoring/MonitoredScope.h"
#include "HLTResultMTMakerAlg.h"

HLTResultMTMakerAlg::HLTResultMTMakerAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator) {}

HLTResultMTMakerAlg::~HLTResultMTMakerAlg() {}

StatusCode HLTResultMTMakerAlg::initialize() {
  ATH_CHECK( m_resultKey.initialize() );
  ATH_CHECK( m_monTool.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode HLTResultMTMakerAlg::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode HLTResultMTMakerAlg::execute_r(const EventContext& context) const {
  using namespace Monitored;
  auto hltResult = std::make_unique<HLT::HLTResultMT>();
  
  auto time =  MonitoredTimer::declare( "TIME_build" );
  for ( auto& maker: m_makerTools ) {
    ATH_CHECK( maker->fill( *hltResult ) );
  }
  time.stop();
  // // monitoring


  auto nstreams = MonitoredScalar::declare( "nstreams", hltResult->getStreamTags().size() );
  auto nfrags   = MonitoredScalar::declare( "nfrags", hltResult->getSerialisedData().size() );
  auto sizeMain = MonitoredScalar::declare( "sizeMain", -1. );  
  auto iter = hltResult->getSerialisedData().find( 0 ); // this is the main fragment of the HLT result
  if ( iter !=  hltResult->getSerialisedData().end() ) 
    sizeMain = double(iter->second.size()*sizeof(uint32_t))/1024;
  
  auto bitWords = MonitoredScalar::declare( "bitWords", hltResult->getHltBits().size()  ); 
  MonitoredScope::declare( m_monTool,  time, nstreams,  nfrags, sizeMain, bitWords );
  

  auto hltResultHandle = SG::makeHandle( m_resultKey, context );
  ATH_CHECK( hltResultHandle.record( std::move( hltResult ) ) );
  return StatusCode::SUCCESS;
}

