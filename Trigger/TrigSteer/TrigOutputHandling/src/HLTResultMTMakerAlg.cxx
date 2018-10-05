/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "AthenaMonitoring/MonitoredScope.h"
#include "HLTResultMTMakerAlg.h"

HLTResultMTMakerAlg::HLTResultMTMakerAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{
}

HLTResultMTMakerAlg::~HLTResultMTMakerAlg()
{
}

StatusCode HLTResultMTMakerAlg::initialize()
{
  //ATH_MSG_DEBUG("Use macros for logging!");
  return StatusCode::SUCCESS;
}

StatusCode HLTResultMTMakerAlg::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode HLTResultMTMakerAlg::execute_r(const EventContext& context) const
{
  using namespace Monitored;


  auto hltResult = SG::makeHandle(m_resultKey, context);
  ATH_CHECK( hltResult.record(std::make_unique<HLT::HLTResultMT>()) );

  auto time =  MonitoredTimer::declare("TIME_build");
  for ( auto& maker: m_makerTools ) {
    maker->fill( *hltResult );
  }
  time.stop();
  // monitoring


  auto nstreams = MonitoredScalar::declare( "nstreams", hltResult->getStreamTags().size() );
  auto nfrags = MonitoredScalar::declare( "nfrags", hltResult->getSerialisedData().size() );
  auto sizeMain = MonitoredScalar::declare( "sizeMain", -1 );
  
  auto iter = hltResult->getSerialisedData().find( 0 ); // this is the main fragment of the HLT result
  if ( iter ==  hltResult->getSerialisedData().end() ) 
    sizeMain = iter->second.size();

  //auto sizeAll = MonitoredScalar::declare( "sizeAll", [&]() { return std::accumulate( hltResult->getSerialisedData().begin(), hltResult->getSerialisedData().end(), 0 ); } );
  
  MonitoredScope::declare( m_monTool,  time, nstreams,  nfrags, sizeMain );

  return StatusCode::SUCCESS;
}

