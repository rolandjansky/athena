/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrkDetDescrUnitTestBase.cxx, (c) ATLAS Detector software
//////////////////////////////////////////////////////////////////

//TrkDetDescrUnitTests
#include "TrkDetDescrUnitTests/TrkDetDescrUnitTestBase.h"

Trk::TrkDetDescrUnitTestBase::TrkDetDescrUnitTestBase(const std::string& name, ISvcLocator* pSvcLocator):
    AthAlgorithm(name,pSvcLocator),
    m_gaussDist(nullptr),
    m_flatDist(nullptr),
    m_numTests(100)
{
    declareProperty("NumberOfTestsPerEvent",   m_numTests);
}

Trk::TrkDetDescrUnitTestBase::~TrkDetDescrUnitTestBase()
{
    delete m_gaussDist;
    delete m_flatDist;
}

StatusCode Trk::TrkDetDescrUnitTestBase::initialize()
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
 
StatusCode Trk::TrkDetDescrUnitTestBase::execute()
{
    return runTest();
}

StatusCode Trk::TrkDetDescrUnitTestBase::finalize()
{

   ATH_MSG_INFO( "finalize()" );
   return StatusCode::SUCCESS;
}

StatusCode Trk::TrkDetDescrUnitTestBase::bookTree() { return StatusCode::SUCCESS; }

StatusCode Trk::TrkDetDescrUnitTestBase::initializeTest() { return StatusCode::SUCCESS; }

