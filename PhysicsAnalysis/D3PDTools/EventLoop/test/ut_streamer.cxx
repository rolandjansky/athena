/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//          Copyright Nils Krumnack 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <EventLoop/Global.h>

#include <EventLoop/BatchJob.h>
#include <RootCoreUtils/Assert.h>
#include <SampleHandler/SampleLocal.h>
#include <TFile.h>

#include <memory>

//
// main program
//

using namespace EL;

int main ()
{
  {
    SH::SampleHandler sh;
    sh.add (new SH::SampleLocal ("test1"));
    Job job;
    job.sampleHandler (sh);
    std::auto_ptr<BatchJob> batch_job (new BatchJob);
    batch_job->job = job;
    TFile file ("batch_job.root", "RECREATE");
    file.WriteObject (batch_job.get(), "job");
  }
  {
    TFile file ("batch_job.root", "READ");
    std::auto_ptr<BatchJob> job (dynamic_cast<BatchJob*>(file.Get ("job")));
    RCU_ASSERT (job->job.sampleHandler().size() == 1);
  }

  return 0;
}
