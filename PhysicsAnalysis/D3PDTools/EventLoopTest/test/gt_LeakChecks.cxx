//
// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
//

// Framework include(s).
#include "AsgTesting/UnitTest.h"
#include "AnaAlgorithm/AnaAlgorithmConfig.h"
#include "EventLoop/DirectDriver.h"
#include "EventLoop/LocalDriver.h"
#include "EventLoop/Job.h"
#include "SampleHandler/SampleHandler.h"
#include "SampleHandler/SampleLocal.h"

// GTest include(s).
#include <gtest/gtest.h>

// System include(s).
#include <memory>
#include <sstream>

/// Test configuration parameters
struct TestParams {
   int leakBytes; ///< Number of bytes to leak per event
   int enableChecks; ///< Whether or not to enable the memory check(s)
};

/// Fixture for the leak check tests
///
/// It sets up an EventLoop job that runs an instance of @c EL::UnitTestAlg3,
/// set up based on the properties received through the @c TestParams object
/// passed to the fixture.
///
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
///
class LeakCheckTests : public ::testing::TestWithParam< TestParams > {

protected:
   /// Function setting up the test fixture
   virtual void SetUp() override {

      // Set up the sample to run on.
      SH::SampleHandler sh;
      sh.setMetaString( "nc_tree", "CollectionTree" );
      std::unique_ptr< SH::Sample > sample( new SH::SampleLocal( "mc" ) );
      SH::SampleLocal* samplePtr =
         dynamic_cast< SH::SampleLocal* >( sample.get() );
      ASSERT_NE( samplePtr, nullptr );
      samplePtr->add( "file://${ASG_TEST_FILE_MC}" );
      sh.add( std::move( sample ) );
      m_job.sampleHandler( sh );

      // Set up the test algorithm.
      EL::AnaAlgorithmConfig algconf( "EL::UnitTestAlg3/LeakCheckAlg" );
      ASSERT_SUCCESS( algconf.setProperty( "LeakBytes",
                                           GetParam().leakBytes ) );
      m_job.algsAdd( algconf );

      // Set up the job.
      m_job.options()->setBool( EL::Job::optRemoveSubmitDir, true );
      m_job.options()->setBool( EL::Job::optMemFailOnLeak,
                                GetParam().enableChecks );
   }

   /// EventLoop job to be run
   EL::Job m_job;

}; // class LeakCheckTests

TEST_P( LeakCheckTests, direct ) {

   // Construct the submission directory name, and make sure that the directory
   // does not exist.
   std::ostringstream outputDir;
   outputDir << "submit-direct-" << GetParam().leakBytes << "-"
             << GetParam().enableChecks;

   // Set up the direct driver.
   EL::DirectDriver driver;
   if( ( GetParam().leakBytes < 10000 ) || ( GetParam().enableChecks == 0 ) ) {
      ASSERT_NO_THROW( driver.submit( m_job, outputDir.str() ) );
   } else {
      // Here we catch the exception thrown by the memory leak check. So we
      // can set some expectations for the text of the exception.
      ASSERT_ANY_THROW( driver.submit( m_job, outputDir.str() ) );
   }
}

TEST_P( LeakCheckTests, batch ) {

   // Construct the submission directory name, and make sure that the directory
   // does not exist.
   std::ostringstream outputDir;
   outputDir << "submit-batch-" << GetParam().leakBytes << "-"
             << GetParam().enableChecks;

   const bool shouldSucceed =
     ( GetParam().leakBytes < 10000 ) || ( GetParam().enableChecks == 0 );

   // Set up the local driver.
   EL::LocalDriver driver;
   try
   {
     driver.submit( m_job, outputDir.str() );
     driver.retrieve( outputDir.str() );
     if (shouldSucceed == false)
       FAIL() << "job succeeded when it should have failed";
   } catch (...)
   {
     if (shouldSucceed == true)
       FAIL() << "job failed when it should have succeeded";
   }
}

// Perform the test with a number of different settings.
INSTANTIATE_TEST_CASE_P( AllLeakCheckTests, LeakCheckTests,
                         ::testing::Values( TestParams{ 0, 0 },
                                            TestParams{ 1000, 0 },
                                            TestParams{ 5000, 0 },
                                            TestParams{ 20000, 0 },
                                            TestParams{ 0, 1 },
                                            TestParams{ 1000, 1 },
                                            TestParams{ 5000, 1 },
                                            TestParams{ 20000, 1 } ), );

// Declare the main() function.
ATLAS_GOOGLE_TEST_MAIN
