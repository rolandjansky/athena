/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrkDigUnitTestBase.cxx, (c) ATLAS Detector software
//////////////////////////////////////////////////////////////////

//TrkDigUnitTests
#include "TrkDigUnitTests/TrkDigUnitTestBase.h"

Trk::TrkDigUnitTestBase::TrkDigUnitTestBase(const std::string& name, ISvcLocator* pSvcLocator):
    AthAlgorithm(name,pSvcLocator),
    m_gaussDist(0),
    m_flatDist(0),
    m_numTests(100)
{
    declareProperty("NumberOfTestsPerEvent",   m_numTests);
}

Trk::TrkDigUnitTestBase::~TrkDigUnitTestBase()
{
    delete m_gaussDist;
    delete m_flatDist;
}

StatusCode Trk::TrkDigUnitTestBase::initialize()
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
 
StatusCode Trk::TrkDigUnitTestBase::execute()
{
    return runTest();
}

StatusCode Trk::TrkDigUnitTestBase::finalize()
{

   ATH_MSG_INFO( "finalize()" );
   return StatusCode::SUCCESS;
}

StatusCode Trk::TrkDigUnitTestBase::bookTree() { return StatusCode::SUCCESS; }

StatusCode Trk::TrkDigUnitTestBase::initializeTest() { return StatusCode::SUCCESS; }

