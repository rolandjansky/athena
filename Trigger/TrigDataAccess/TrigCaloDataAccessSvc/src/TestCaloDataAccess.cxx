/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include <iostream>
#include "tbb/parallel_reduce.h"
#include "tbb/blocked_range.h"
#include "TestTools/expect.h"
#include "TestTools/ParallelCallTest.h"
#include "GaudiKernel/Property.h"
#include "TestCaloDataAccess.h"
#include <sys/time.h>


TestCaloDataAccess::TestCaloDataAccess( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthReentrantAlgorithm( name, pSvcLocator )
{

}

TestCaloDataAccess::~TestCaloDataAccess() {}

StatusCode TestCaloDataAccess::initialize() {
  return StatusCode::SUCCESS;
}


StatusCode TestCaloDataAccess::execute_r( const EventContext& context ) const {  
  return StatusCode::SUCCESS;
}


