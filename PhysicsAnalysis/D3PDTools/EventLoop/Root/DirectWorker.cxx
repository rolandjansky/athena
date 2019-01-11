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
#include <EventLoop/Job.h>
#include <EventLoop/OutputStream.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/DiskOutput.h>
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
    setJobConfig (JobConfig (job.jobConfig()));

    for (Job::outputIter out = job.outputBegin(),
	   end = job.outputEnd(); out != end; ++ out)
    {
      addOutputWriter (out->label(), out->output()->makeWriter (sample->name(), "", -1, ".root"));
    }

    RCU_NEW_INVARIANT (this);
  }



  void DirectWorker ::
  run ()
  {
    RCU_CHANGE_INVARIANT (this);

    Long64_t count = 0;
    const Long64_t maxEvents
      = metaData()->castDouble (Job::optMaxEvents, -1);
    Long64_t skipEvents
      = metaData()->castDouble (Job::optSkipEvents, 0);

    std::vector<std::string> files = m_sample->makeFileList();
    for (std::vector<std::string>::const_iterator fileName = files.begin(),
	   end = files.end();
	 fileName != end && count != maxEvents; ++ fileName)
    {
      std::cout << "Processing File: " << *fileName << std::endl;
      std::unique_ptr<TFile> inFile = SH::openFile (*fileName, *metaData());
      setInputFile (inFile.get());
      Long64_t inTreeEntries = inputFileNumEntries();

      if (skipEvents > inTreeEntries)
      {
	skipEvents -= inTreeEntries;
	continue;
      }

      treeEntry (0);
      algsChangeInput ();
      for (Long64_t entry = skipEvents, num = inTreeEntries;
	   entry != num && count != maxEvents; ++ entry)
      {
	treeEntry (entry);
	algsExecute ();

	if (++ count % 10000 == 0)
	  std::cout << "Processed " << count << " events" << std::endl;
      }
      algsEndOfFile ();
      skipEvents = 0;
    }
    algsFinalize ();

    // Perform a memory leak check in case at least one event was processed.
    if (count > 0) {

       // Extract the limits for producing an error.
       const int absResidentLimit =
          metaData()->castInteger (Job::optMemResidentIncreaseLimit,
                                   10000);
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
}
