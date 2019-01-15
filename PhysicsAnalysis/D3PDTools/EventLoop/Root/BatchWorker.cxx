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
#include <EventLoop/Job.h>
#include <EventLoop/OutputStream.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/DiskOutput.h>
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
	       const BatchSegment *val_segment,
	       TList *output)
    : Worker ((RCU_REQUIRE (val_job != 0),
	       RCU_REQUIRE (val_sample != 0),
	       RCU_REQUIRE (val_segment != 0),
	       RCU_REQUIRE (output != 0),
	       &val_sample->meta), output),
      m_job (val_job), m_sample (val_sample), m_segment (val_segment)
  {
    setJobConfig (JobConfig (val_job->job.jobConfig()));

    for (Job::outputIter out = m_job->job.outputBegin(),
	   end = m_job->job.outputEnd(); out != end; ++ out)
    {
      addOutputWriter (out->label(), out->output()->makeWriter ("", m_segment->name, -1, ".root"));
    }

    RCU_NEW_INVARIANT (this);
  }



  void BatchWorker ::
  run ()
  {
    RCU_CHANGE_INVARIANT (this);
 
    Long64_t beginFile = m_segment->begin_file;
    Long64_t endFile   = m_segment->end_file;
    Long64_t lastFile  = m_segment->end_file;
    RCU_ASSERT (beginFile <= endFile);
    Long64_t beginEvent = m_segment->begin_event;
    Long64_t endEvent   = m_segment->end_event;
    if (endEvent > 0) endFile += 1;
    Long64_t count = 0;

    for (Long64_t file = beginFile; file != endFile; ++ file)
    {
      RCU_ASSERT (std::size_t(file) < m_sample->files.size());
      std::cout << "Processing File " << m_sample->files[file].c_str() << std::endl;
      std::unique_ptr<TFile> inFile = SH::openFile (m_sample->files[file], *metaData());
      if (inFile.get() == 0)
	RCU_THROW_MSG (std::string ("failed to open file ") + m_sample->files[file]);
      setInputFile (inFile.get());

      const Long64_t firstEvent = (file == beginFile ? beginEvent : 0);
      const Long64_t lastEvent
	= (file == lastFile ? endEvent : inputFileNumEntries());

      treeEntry (firstEvent);
      algsChangeInput ();
      for (Long64_t entry = firstEvent; entry != lastEvent; ++ entry)
      {
	treeEntry (entry);
	algsExecute ();
	++count;
      }
      algsEndOfFile ();
    }
    algsFinalize ();

    // Perform a memory leak check in case at least one event was processed.
    if (count > 0) {

       // Extract the limits for producing an error.
       const int absResidentLimit =
          metaData()->castInteger (Job::optMemResidentIncreaseLimit,
                                   1000);
       const int absVirtualLimit =
          metaData()->castInteger (Job::optMemVirtualIncreaseLimit,
                                   0);
       const int perEvResidentLimit =
          metaData()->castInteger (Job::optMemResidentPerEventIncreaseLimit,
                                   10);
       const int perEvVirtualLimit =
          metaData()->castInteger (Job::optMemVirtualPerEventIncreaseLimit,
                                   0);

       // Calculate and print the memory increase of the job.
       const Long_t resLeak = memIncreaseResident();
       const Double_t resLeakPerEv =
          (static_cast< Double_t > (resLeak) /
           static_cast< Double_t > (count));
       const Long_t virtLeak = memIncreaseVirtual();
       const Double_t virtLeakPerEv =
          (static_cast< Double_t > (virtLeak) /
           static_cast< Double_t > (count) );
       std::cout << "Memory increase/change during the job:" << std::endl;
       std::cout << "  - resident: " << resLeakPerEv << " kB/event ("
                 << resLeak << " kB total)" << std::endl;
       std::cout << "  - virtual : " << virtLeakPerEv << " kB/event ("
                 << virtLeak << " kB total)" << std::endl;

       // Decide if this acceptable or not.
       if ((resLeak > absResidentLimit) &&
           (resLeakPerEv > perEvResidentLimit) &&
           (virtLeak > absVirtualLimit) &&
           (virtLeakPerEv > perEvVirtualLimit)) {

          // If not, decide what to do about it.
          if (metaData()->castBool (Job::optMemFailOnLeak, false)) {
             RCU_THROW_MSG ("A significant memory leak was detected");
          } else {
             std::cerr << "WARNING *" << std::endl
                       << "WARNING * A significant memory leak was detected"
                       << std::endl
                       << "WARNING *" << std::endl;
          }
       }
    }
  }



  void BatchWorker ::
  execute (unsigned job_id, const char *confFile)
  {
    try
    {
      std::auto_ptr<TFile> file (TFile::Open (confFile, "READ"));
      RCU_ASSERT_SOFT (file.get() != 0);
      std::auto_ptr<BatchJob> job (dynamic_cast<BatchJob*>(file->Get ("job")));
      RCU_ASSERT_SOFT (job.get() != 0);
      RCU_ASSERT_SOFT (job_id < job->segments.size());
      BatchSegment *segment = &job->segments[job_id];
      RCU_ASSERT_SOFT (segment->job_id == job_id);
      RCU_ASSERT_SOFT (segment->sample < job->samples.size());
      BatchSample *sample = &job->samples[segment->sample];

      gSystem->Exec ("pwd");
      gSystem->MakeDirectory ("output");

      TList output;
      BatchWorker worker (job.get(), sample, segment,
			  &output);
      worker.run ();

      std::ostringstream job_name;
      job_name << job_id;
      Driver::saveOutput (job->location + "/fetch", segment->name, output);
      std::ofstream completed ((job->location + "/status/completed-" + job_name.str()).c_str());
    } catch (std::exception& e)
    {
      std::cout << "exception caught: " << e.what() << std::endl;
      exit (EXIT_FAILURE);
    } catch (std::string& s)
    {
      std::cout << "exception caught: " << s << std::endl;
      exit (EXIT_FAILURE);
    }
  }
}
