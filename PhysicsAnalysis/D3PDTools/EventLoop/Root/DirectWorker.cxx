/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#include <EventLoop/DirectWorker.h>

#include <EventLoop/Algorithm.h>
#include <EventLoop/EventRange.h>
#include <EventLoop/Job.h>
#include <EventLoop/MessageCheck.h>
#include <EventLoop/OutputStream.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/DiskOutput.h>
#include <SampleHandler/DiskWriter.h>
#include <SampleHandler/Sample.h>
#include <SampleHandler/ToolsOther.h>
#include <TFile.h>
#include <iostream>
#include <memory>

//
// method implementations
//

namespace EL
{
  void DirectWorker ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  DirectWorker ::
  DirectWorker (const SH::SamplePtr& sample, TList *output,
		const Job& job, const std::string& location,
		const SH::MetaObject *meta)
    : Worker (meta, output), m_sample (sample), m_location (location)
  {
    using namespace msgEventLoop;

    setJobConfig (JobConfig (job.jobConfig()));

    for (Job::outputIter out = job.outputBegin(),
	   end = job.outputEnd(); out != end; ++ out)
    {
      ANA_CHECK_THROW (addOutputWriter (out->label(), std::unique_ptr<SH::DiskWriter> (out->output()->makeWriter (sample->name(), "", -1, ".root"))));
    }

    RCU_NEW_INVARIANT (this);
  }



  void DirectWorker ::
  run ()
  {
    using namespace msgEventLoop;

    RCU_CHANGE_INVARIANT (this);

    ANA_CHECK_THROW (initialize ());

    const Long64_t maxEvents
      = metaData()->castDouble (Job::optMaxEvents, -1);
    Long64_t skipEvents
      = metaData()->castDouble (Job::optSkipEvents, 0);

    std::vector<std::string> files = m_sample->makeFileList();
    for (std::vector<std::string>::const_iterator fileName = files.begin(),
	   end = files.end();
	 fileName != end && Long64_t (eventsProcessed()) != maxEvents; ++ fileName)
    {
      EventRange eventRange;
      eventRange.m_url = *fileName;
      if (skipEvents == 0 && maxEvents == -1)
      {
        ANA_CHECK_THROW (processEvents (eventRange));
      } else
      {
        // just open the input file to inspect it
        ANA_CHECK_THROW (openInputFile (*fileName));
        const Long64_t inTreeEntries = inputFileNumEntries();

        if (skipEvents >= inTreeEntries)
        {
          skipEvents -= inTreeEntries;
          continue;
        }
        eventRange.m_beginEvent = skipEvents;

        if (maxEvents != -1 &&
            inTreeEntries > maxEvents + eventRange.m_beginEvent)
          eventRange.m_endEvent = maxEvents + eventRange.m_beginEvent;
        ANA_CHECK_THROW (processEvents (eventRange));
      }
    }
    ANA_CHECK_THROW (finalize ());
  }
}
