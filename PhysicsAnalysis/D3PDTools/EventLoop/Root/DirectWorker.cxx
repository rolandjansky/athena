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
#include <EventLoop/Driver.h>
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
  DirectWorker (const SH::SamplePtr& sample,
		const std::string& location)
    : m_sample (sample), m_location (location)
  {
    using namespace msgEventLoop;

    RCU_NEW_INVARIANT (this);
  }



  void DirectWorker ::
  run (const SH::SamplePtr& sample, const Job& job)
  {
    using namespace msgEventLoop;

    RCU_CHANGE_INVARIANT (this);

    setJobConfig (JobConfig (job.jobConfig()));

    for (Job::outputIter out = job.outputBegin(),
	   end = job.outputEnd(); out != end; ++ out)
    {
      Detail::OutputStreamData data {
        out->output()->makeWriter (sample->name(), "", ".root")};
      ANA_CHECK_THROW (addOutputStream (out->label(), std::move (data)));
    }

    ANA_CHECK_THROW (initialize ());

    Long64_t maxEvents
      = metaData()->castDouble (Job::optMaxEvents, -1);
    Long64_t skipEvents
      = metaData()->castDouble (Job::optSkipEvents, 0);

    std::vector<std::string> files = m_sample->makeFileList();
    for (const std::string& fileName : files)
    {
      EventRange eventRange;
      eventRange.m_url = fileName;
      if (skipEvents == 0 && maxEvents == -1)
      {
        ANA_CHECK_THROW (processEvents (eventRange));
      } else
      {
        // just open the input file to inspect it
        ANA_CHECK_THROW (openInputFile (fileName));
        eventRange.m_endEvent = inputFileNumEntries();

        if (skipEvents >= eventRange.m_endEvent)
        {
          skipEvents -= eventRange.m_endEvent;
          continue;
        }
        eventRange.m_beginEvent = skipEvents;
        skipEvents = 0;

        if (maxEvents != -1)
        {
          if (eventRange.m_endEvent > eventRange.m_beginEvent + maxEvents)
            eventRange.m_endEvent = eventRange.m_beginEvent + maxEvents;
          maxEvents -= eventRange.m_endEvent - eventRange.m_beginEvent;
          assert (maxEvents >= 0);
        }
        ANA_CHECK_THROW (processEvents (eventRange));
        if (maxEvents == 0)
          break;
      }
    }
    ANA_CHECK_THROW (finalize ());
  }



  void DirectWorker ::
  execute (const SH::SamplePtr& sample, const Job& job,
           const std::string& location, const SH::MetaObject& options)
  {
    using namespace msgEventLoop;

    SH::MetaObject meta (*sample->meta());
    meta.fetchDefaults (*job.options());
    meta.fetchDefaults (options);

    DirectWorker worker (sample, location);
    worker.setMetaData (&meta);
    worker.setOutputHist (location);
    worker.setSegmentName (sample->name());

    ANA_MSG_INFO ("Running sample: " << sample->name());
    worker.run (sample, job);
  }
}
