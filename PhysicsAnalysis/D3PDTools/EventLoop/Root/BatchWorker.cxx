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

#include <EventLoop/BatchWorker.h>

#include <EventLoop/Algorithm.h>
#include <EventLoop/BatchJob.h>
#include <EventLoop/BatchSample.h>
#include <EventLoop/BatchSegment.h>
#include <EventLoop/Driver.h>
#include <EventLoop/EventRange.h>
#include <EventLoop/Job.h>
#include <EventLoop/MessageCheck.h>
#include <EventLoop/OutputStream.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/DiskOutput.h>
#include <SampleHandler/DiskWriter.h>
#include <SampleHandler/ToolsOther.h>
#include <TFile.h>
#include <TSystem.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

//
// method implementations
//

namespace EL
{
  void BatchWorker ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
    RCU_INVARIANT (m_job != 0);
    RCU_INVARIANT (m_sample != 0);
    RCU_INVARIANT (m_segment != 0);
  }



  BatchWorker ::
  BatchWorker (const BatchJob *val_job,
	       const BatchSample *val_sample,
	       const BatchSegment *val_segment)
    : m_job (val_job), m_sample (val_sample), m_segment (val_segment)
  {
    using namespace msgEventLoop;

    RCU_NEW_INVARIANT (this);
  }



  void BatchWorker ::
  run (const BatchJob *val_job)
  {
    using namespace msgEventLoop;

    RCU_CHANGE_INVARIANT (this);

    setJobConfig (JobConfig (val_job->job.jobConfig()));

    for (Job::outputIter out = m_job->job.outputBegin(),
	   end = m_job->job.outputEnd(); out != end; ++ out)
    {
      Detail::OutputStreamData data;
      data.m_writer =
        out->output()->makeWriter (m_segment->sampleName, m_segment->segmentName, ".root");
      ANA_CHECK_THROW (addOutputStream (out->label(), std::move (data)));
    }
 
    Long64_t beginFile = m_segment->begin_file;
    Long64_t endFile   = m_segment->end_file;
    Long64_t lastFile  = m_segment->end_file;
    RCU_ASSERT (beginFile <= endFile);
    Long64_t beginEvent = m_segment->begin_event;
    Long64_t endEvent   = m_segment->end_event;
    if (endEvent > 0) endFile += 1;

    ANA_CHECK_THROW (initialize ());

    for (Long64_t file = beginFile; file != endFile; ++ file)
    {
      RCU_ASSERT (std::size_t(file) < m_sample->files.size());
      EventRange eventRange;
      eventRange.m_url = m_sample->files[file];
      eventRange.m_beginEvent = (file == beginFile ? beginEvent : 0);
      eventRange.m_endEvent = (file == lastFile ? endEvent : EventRange::eof);
      ANA_CHECK_THROW (processEvents (eventRange));
    }
    ANA_CHECK_THROW (finalize ());
  }



  void BatchWorker ::
  execute (unsigned job_id, const char *confFile)
  {
    try
    {
      std::unique_ptr<TFile> file (TFile::Open (confFile, "READ"));
      RCU_ASSERT_SOFT (file.get() != 0);
      std::unique_ptr<BatchJob> job (dynamic_cast<BatchJob*>(file->Get ("job")));
      RCU_ASSERT_SOFT (job.get() != 0);
      RCU_ASSERT_SOFT (job_id < job->segments.size());
      BatchSegment *segment = &job->segments[job_id];
      RCU_ASSERT_SOFT (segment->job_id == job_id);
      RCU_ASSERT_SOFT (segment->sample < job->samples.size());
      BatchSample *sample = &job->samples[segment->sample];

      gSystem->Exec ("pwd");
      gSystem->MakeDirectory ("output");

      BatchWorker worker (job.get(), sample, segment);
      worker.setMetaData (&sample->meta);
      worker.setOutputHist (job->location + "/fetch");
      worker.setSegmentName (segment->fullName);
      worker.run (job.get());

      std::ostringstream job_name;
      job_name << job_id;
      std::ofstream completed ((job->location + "/status/completed-" + job_name.str()).c_str());
    } catch (...)
    {
      Detail::report_exception ();
      exit (EXIT_FAILURE);
    }
  }
}
