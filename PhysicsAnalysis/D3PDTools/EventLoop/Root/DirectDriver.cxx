/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


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
  doManagerStep (Detail::ManagerData& data) const
  {
    using namespace msgEventLoop;
    ANA_CHECK (Driver::doManagerStep (data));
    switch (data.step)
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
        diskOutputSave (data);
      }
      break;

    case Detail::ManagerStep::doRetrieve:
      {
        data.retrieved = true;
        data.completed = true;
      }
      break;

    default:
      (void) true; // safe to do nothing
    }
    return ::StatusCode::SUCCESS;
  }
}
