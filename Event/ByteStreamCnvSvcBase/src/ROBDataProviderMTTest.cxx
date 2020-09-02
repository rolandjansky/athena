/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include <algorithm>
#include "Gaudi/Property.h"
#include "TestTools/ParallelCallTest.h"
#include "TestTools/random.h"
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

class AskForROBs : public ParallelCallTest {
public:
  AskForROBs( const EventContext& context, const ServiceHandle<IROBDataProviderSvc>& svc, uint32_t lvl1ID, const std::vector<eformat::read::ROBFragment>& robs, size_t frac=8 )
    : m_lvl1ID( lvl1ID ),
      m_context( context ),
      m_svc( svc ) {
    // pick some fraction of robs at random
    for ( size_t el = 0; el < robs.size()/frac; ++el ) {
      m_ROBIDs.push_back( robs[el].rob_source_id() );
    }
  }
  size_t nrobs() const { return m_ROBIDs.size(); }
  virtual void firstCall() override {}
  virtual bool callAndCompare() const override {
    IROBDataProviderSvc::VROBFRAG robs;
    m_svc->getROBData( m_context, m_ROBIDs, robs ); 
    for ( auto& rob: robs ) {
      if ( rob->rod_lvl1_id() != m_lvl1ID ) 
	return false;
    }
    return true;
  }

private:
  uint32_t m_lvl1ID;
  std::vector<uint32_t> m_ROBIDs;
  EventContext m_context;
  const ServiceHandle<IROBDataProviderSvc>& m_svc;
};



StatusCode ROBDataProviderMTTest::execute( const EventContext& context ) const
{  
  // will be asking for many ROBs from the list and check if all of them come from the same physical event
  const RawEvent* ev = m_robDataProvider->getEvent( context );
  ATH_MSG_DEBUG( "Obtained event, global id: " << ev->global_id() << " lvl1 id: " << ev->lvl1_id() );
  uint32_t lvl1ID = ev->lvl1_id();

  
  // we try now the other method, getROBData, in 8 steps, concurrently
  std::vector<ParallelCallTest*> requests;  
  std::vector<eformat::read::ROBFragment> robs;
  ev->robs( robs );

  Athena_test::URNG rng;
  rng.seed = context.evt();
  for ( size_t i = 0; i < 8; ++i ) { 
    std::shuffle( robs.begin(), robs.end(), rng );
    auto r = new AskForROBs( context, m_robDataProvider, lvl1ID, robs );
    ATH_MSG_DEBUG( "Prepared parallel request with " << r->nrobs() << " robs" );  
    requests.push_back( r );
  }
 

  bool status = ParallelCallTest::launchTests( 100, requests );
  if ( status == false ) 
  ATH_MSG_ERROR( "Concurrent ROB requests resulted in inconsistent information" );      

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


