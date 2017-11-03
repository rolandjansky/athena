/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include <vector>
#include "tbb/parallel_reduce.h"
#include "tbb/blocked_range.h"
#include "TestTools/ParallelCallTest.h"

bool ParallelCallTest::run( size_t nrepeats ) {    
  firstCall();
  
  return  tbb::parallel_reduce( tbb::blocked_range<int>( 0, nrepeats, 1 ),
				true, // initial value
				[&]( tbb::blocked_range<int>, bool statusSoFar ) -> bool {
				  return callAndCompare() and statusSoFar;
				},
				[]( bool allCallsStatus, bool thisCallStatus ) -> bool { // result accumulation
				  return allCallsStatus and thisCallStatus; 
				} );
}


bool ParallelCallTest::launchTests( size_t nrepeats, std::initializer_list<ParallelCallTest*> testList ) {
  std::vector<ParallelCallTest*> tests( testList.begin(), testList.end() );
  return  tbb::parallel_reduce( tbb::blocked_range< std::vector<ParallelCallTest*>::iterator >( tests.begin(), tests.end() ),
				true, // initial value
				[&]( tbb::blocked_range< std::vector<ParallelCallTest*>::iterator > groupOfTests, bool statusSoFar ) -> bool {
				  bool success = true;
				  for ( auto test : groupOfTests ) {
				    success = test->run( nrepeats ) and success;
				  }
				  return statusSoFar and success;
				},
				[]( bool allCallsStatus, bool thisCallStatus ) -> bool { // result accumulation
				  return allCallsStatus and thisCallStatus; 
				} );    
}
