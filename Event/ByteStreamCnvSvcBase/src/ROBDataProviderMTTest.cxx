/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include <algorithm>
#include "GaudiKernel/Property.h"
#include "TestTools/ParallelCallTest.h"
#include "ROBDataProviderMTTest.h"



ROBDataProviderMTTest::ROBDataProviderMTTest( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthReentrantAlgorithm( name, pSvcLocator ) {
}

ROBDataProviderMTTest::~ROBDataProviderMTTest() {}

StatusCode ROBDataProviderMTTest::initialize() {
  CHECK( m_robDataProvider.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode ROBDataProviderMTTest::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode ROBDataProviderMTTest::execute_r( const EventContext& context ) const
{  
  // will be asking for many ROBs from the list and check if all of them come from the same physical event
  const RawEvent* ev = m_robDataProvider->getEvent( context );
  ATH_MSG_DEBUG( "Obtained event, global id: " << ev->global_id() << " lvl1 id: " << ev->lvl1_id() );
  uint32_t lvl1ID = ev->lvl1_id();
  std::vector<uint32_t> robIDs;
  
  {
    //    std::vector<ROBFragment> robs
    std::vector<eformat::read::ROBFragment> robs;
    ev->robs( robs );
    for ( auto& rob: robs ) {
      robIDs.push_back( rob.rob_source_id() );
    }
  }

  std::random_shuffle( robIDs.begin(), robIDs.end() );

  // we try now the other method, getROBData, in 8 steps
  std::vector<std::vector<uint32_t>> requests(8);
  
  for ( size_t index = 0; index < robIDs.size(); ++index ) {
    requests[index % requests.size() ].push_back( robIDs[index] );
  }
  for ( auto & reqPart: requests ) {
    //    ATH_MSG_DEBUG( "Performing getROBData request for this robs: " << reqPart);
    IROBDataProviderSvc::VROBFRAG robs;
    m_robDataProvider->getROBData( context, reqPart, robs ); 
    for ( auto& rob: robs ) {
      if ( rob->rod_lvl1_id() != lvl1ID ) 
	ATH_MSG_ERROR( "ROB of ID "<< rob->rob_source_id()  << "  Lvl1 ID missmatch, in ROB " 
		       <<  rob->rod_lvl1_id() << " in Event header " << lvl1ID );      
    }
  }
  size_t count = 0;
  m_robDataProvider->processCachedROBs( context, 
					[&]( const IROBDataProviderSvc::ROBF* ){ count++; } );
  ATH_MSG_DEBUG( "Size of the cache " << count );

  m_robDataProvider->processCachedROBs( context, 
					[&]( const IROBDataProviderSvc::ROBF* rob ){ 
					  if ( 1120005 == rob->rob_source_id() ) 
					    ATH_MSG_DEBUG( "rob in the cache " << rob->rob_source_id() ); } 
					);

  return StatusCode::SUCCESS;
}


