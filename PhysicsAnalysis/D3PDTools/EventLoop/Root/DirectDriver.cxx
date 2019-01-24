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
#include <EventLoop/DirectWorker.h>
#include <EventLoop/OutputStream.h>
#include <RootCoreUtils/Assert.h>
#include <SampleHandler/DiskOutputLocal.h>
#include <SampleHandler/Sample.h>
#include <SampleHandler/SampleHandler.h>
#include <SampleHandler/SampleLocal.h>

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



  void DirectDriver ::
  doUpdateJob (Job& job, const std::string& location) const
  {
    RCU_READ_INVARIANT (this);

    for (Job::outputMIter out = job.outputBegin(),
	   end = job.outputEnd(); out != end; ++ out)
    {
      if (out->output() == 0)
      {
	out->output (new SH::DiskOutputLocal
		     (location + "/data-" + out->label() + "/"));
      }
    }
  }



  void DirectDriver ::
  doSubmit (const Job& job, const std::string& location) const
  {
    RCU_READ_INVARIANT (this);

    for (SH::SampleHandler::iterator sample = job.sampleHandler().begin(),
	   end = job.sampleHandler().end(); sample != end; ++ sample)
    {
      DirectWorker::execute (*sample, job, location, *options());
    }
    diskOutputSave (location, job);
  }
}
