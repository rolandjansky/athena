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

#include <EventLoop/DirectDriver.h>

#include <iostream>
#include <memory>
#include <TSystem.h>
#include <EventLoop/Job.h>
#include <EventLoop/ManagerData.h>
#include <EventLoop/ManagerStep.h>
#include <EventLoop/MessageCheck.h>
#include <EventLoop/Worker.h>
#include <EventLoop/OutputStream.h>
#include <RootCoreUtils/Assert.h>
#include <SampleHandler/DiskOutputLocal.h>
#include <SampleHandler/Sample.h>
#include <SampleHandler/SampleHandler.h>
#include <SampleHandler/SampleLocal.h>
#include <SampleHandler/SamplePtr.h>

//
// method implementations
//

ClassImp(EL::DirectDriver)

namespace EL
{
  void DirectDriver ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  DirectDriver ::
  DirectDriver ()
  {
    RCU_NEW_INVARIANT (this);
  }



  ::StatusCode DirectDriver ::
  doManagerStep (Detail::ManagerData& data,
                Detail::ManagerStep step) const
  {
    using namespace msgEventLoop;
    ANA_CHECK (Driver::doManagerStep (data, step));
    switch (step)
    {
    case Detail::ManagerStep::updateOutputLocation:
      {
        for (Job::outputMIter out = data.job->outputBegin(),
               end = data.job->outputEnd(); out != end; ++ out)
        {
          if (out->output() == 0)
          {
            out->output (new SH::DiskOutputLocal
                         (data.submitDir + "/data-" + out->label() + "/"));
          }
        }
      }
      break;

    case Detail::ManagerStep::submitJob:
      {
        for (SH::SampleHandler::iterator sample = data.job->sampleHandler().begin(),
               end = data.job->sampleHandler().end(); sample != end; ++ sample)
        {
          Worker worker;
          ANA_CHECK (worker.directExecute (*sample, *data.job, data.submitDir, data.options));
        }
        data.submitted = true;
      }
      break;

    case Detail::ManagerStep::directSaveOutput:
      {
        diskOutputSave (data.submitDir, *data.job);
      }
      break;

    default:
      (void) true; // safe to do nothing
    }
    return ::StatusCode::SUCCESS;
  }
}
