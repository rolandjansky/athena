/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrkExUnitTestBase.cxx, (c) ATLAS Detector software
//////////////////////////////////////////////////////////////////

//TrkExUnitTests
#include "TrkExUnitTests/TrkExUnitTestBase.h"

Trk::TrkExUnitTestBase::TrkExUnitTestBase(const std::string& name, ISvcLocator* pSvcLocator):
    AthAlgorithm(name,pSvcLocator),
    m_gaussDist(0),
    m_flatDist(0),
    m_numTests(100)
{
    declareProperty("NumberOfTestsPerEvent",   m_numTests);
}

Trk::TrkExUnitTestBase::~TrkExUnitTestBase()
{
    delete m_gaussDist;
    delete m_flatDist;
}

StatusCode Trk::TrkExUnitTestBase::initialize()
{
    ATH_MSG_INFO( "Creating random number services, call bookTree() and initializeTest()" );

    // intialize the random number generators
    m_gaussDist = new Rndm::Numbers(randSvc(), Rndm::Gauss(0.,1.));
    m_flatDist  = new Rndm::Numbers(randSvc(), Rndm::Flat(0.,1.));

    if  (bookTree().isFailure()){
        ATH_MSG_FATAL( "Could not book the TTree object" );
        return StatusCode::FAILURE;
    }
    
    if (initializeTest().isFailure()){
        ATH_MSG_FATAL( "Could not initialize the test" );
        return StatusCode::FAILURE;
    }
    
    return StatusCode::SUCCESS;
}
 
StatusCode Trk::TrkExUnitTestBase::execute()
{
    return runTest();
}

StatusCode Trk::TrkExUnitTestBase::finalize()
{

   ATH_MSG_INFO( "finalize()" );
   return StatusCode::SUCCESS;
}

StatusCode Trk::TrkExUnitTestBase::bookTree() { return StatusCode::SUCCESS; }

StatusCode Trk::TrkExUnitTestBase::initializeTest() { return StatusCode::SUCCESS; }

