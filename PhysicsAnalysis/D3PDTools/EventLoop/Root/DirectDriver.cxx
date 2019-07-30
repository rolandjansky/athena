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
#include <EventLoop/JobSubmitInfo.h>
#include <EventLoop/JobSubmitStep.h>
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
  doSubmitStep (Detail::JobSubmitInfo& info,
                Detail::JobSubmitStep step) const
  {
    using namespace msgEventLoop;
    ANA_CHECK (Driver::doSubmitStep (info, step));
    switch (step)
    {
    case Detail::JobSubmitStep::updateOutputLocation:
      {
        for (Job::outputMIter out = info.job->outputBegin(),
               end = info.job->outputEnd(); out != end; ++ out)
        {
          if (out->output() == 0)
          {
            out->output (new SH::DiskOutputLocal
                         (info.submitDir + "/data-" + out->label() + "/"));
          }
        }
      }
      break;

    default:
      (void) true; // safe to do nothing
    }
    return ::StatusCode::SUCCESS;
  }






  void DirectDriver ::
  doSubmit (Detail::JobSubmitInfo& info) const
  {
    RCU_READ_INVARIANT (this);
    using namespace msgEventLoop;

    for (SH::SampleHandler::iterator sample = info.job->sampleHandler().begin(),
	   end = info.job->sampleHandler().end(); sample != end; ++ sample)
    {
      Worker worker;
      ANA_CHECK_THROW (worker.directExecute (*sample, *info.job, info.submitDir, info.options));
    }
    diskOutputSave (info.submitDir, *info.job);
  }
}
