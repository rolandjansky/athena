//
// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
//

// EventLoop include(s).
#include "AnaAlgorithm/AnaAlgorithmConfig.h"
#include "EventLoop/DirectDriver.h"
#include "EventLoop/Job.h"
#include "SampleHandler/SampleHandler.h"
#include "SampleHandler/SampleLocal.h"

// xAOD include(s).
#include "xAODRootAccess/Init.h"

// System include(s).
#include <memory>

int main() {

   // Set up the xAOD reading.
   xAOD::Init().ignore();

   // Set up the job object.
   EL::Job job;
   job.useXAOD();
   job.options()->setBool( EL::Job::optRemoveSubmitDir, true );

   // Set up the test sample to run on.
   SH::SampleHandler sh;
   auto sample = std::make_unique< SH::SampleLocal >( "data" );
   sample->add( "file://${ASG_TEST_FILE_DATA}" );
   sh.add( std::move( sample ) );
   job.sampleHandler( sh );

   // Set up the test algorithm('s configuration).
   EL::AnaAlgorithmConfig algconf( "EL::UnitTestAlg4/MetaDataCheckAlg" );
   job.algsAdd( algconf );

   // Run the test.
   EL::DirectDriver driver;
   driver.submit( job, "metaDataCheckJob" );

   // Return gracefully.
   return 0;
}
