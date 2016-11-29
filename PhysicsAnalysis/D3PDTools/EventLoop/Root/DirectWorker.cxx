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
    for (Job::outputIter out = job.outputBegin(),
	   end = job.outputEnd(); out != end; ++ out)
    {
      addOutputWriter (out->label(), out->output()->makeWriter (sample->name(), "", -1, ".root"));
    }

    for (Job::algsIter alg = job.algsBegin(),
	   end = job.algsEnd(); alg != end; ++ alg)
      addAlg (dynamic_cast<Algorithm*>((*alg)->Clone ()));

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
  }
}
