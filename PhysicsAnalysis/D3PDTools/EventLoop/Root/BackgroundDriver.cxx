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

#include <EventLoop/BackgroundDriver.h>

#include <EventLoop/BackgroundJob.h>
#include <EventLoop/BackgroundSocket.h>
#include <EventLoop/Job.h>
#include <EventLoop/OutputStream.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/DiskOutputLocal.h>
#include <SampleHandler/MetaFields.h>
#include <SampleHandler/Sample.h>
#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <sys/wait.h>

//
// method implementations
//

ClassImp(EL::BackgroundDriver)

namespace EL
{
  void BackgroundDriver ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  BackgroundDriver ::
  BackgroundDriver ()
  {
    RCU_NEW_INVARIANT (this);
  }



  void BackgroundDriver ::
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



  void BackgroundDriver ::
  doSubmit (const Job& job, const std::string& location) const
  {
    RCU_READ_INVARIANT (this);

    for (SH::SampleHandler::iterator sample = job.sampleHandler().begin(),
	   end = job.sampleHandler().end(); sample != end; ++ sample)
    {
      std::cout << "Running sample: " << (*sample)->name() << std::endl;
      SH::MetaObject meta (*(*sample)->meta());
      meta.fetchDefaults (*job.options());
      meta.fetchDefaults (*options());

      std::auto_ptr<BackgroundSocket> socket;

      {
	BackgroundJob myjob;
	for (Job::algsIter iter = job.algsBegin(),
	       end = job.algsEnd(); iter != end; ++ iter)
	  myjob.algs.push_back (*iter);
	for (Job::outputIter iter = job.outputBegin(),
	       end = job.outputEnd(); iter != end; ++ iter)
	  myjob.output.push_back (*iter);
	myjob.meta = meta;
	socket = BackgroundSocket::startWorker (myjob);
      }

      Long64_t count = 0;
      const Long64_t maxEvents
	= meta.castDouble (Job::optMaxEvents, -1);
      Long64_t skipEvents
	= meta.castDouble (Job::optSkipEvents, 0);

      std::vector<std::string> files = (*sample)->makeFileList();
      for (std::vector<std::string>::const_iterator fileName = files.begin(),
	     end = files.end();
	   fileName != end && count != maxEvents; ++ fileName)
      {
	std::auto_ptr<TFile> inFile (TFile::Open (fileName->c_str()));
	if (inFile.get() == 0)
	  RCU_THROW_MSG ("failed to open file " + *fileName);
	Long64_t inTreeEntries = 0;
	{
	  std::string treeName
	    = meta.castString (SH::MetaFields::treeName, SH::MetaFields::treeName_default);
	  TTree *inputTree = dynamic_cast<TTree*>(inFile->Get(treeName.c_str()));
	  if (inputTree)
	    inTreeEntries = inputTree->GetEntries();
	}

	if (skipEvents > inTreeEntries)
	{
	  skipEvents -= inTreeEntries;
	  continue;
	}


	{
	  Long64_t entry = 0;
	  socket->write (BackgroundSocket::T_SET_ENTRY, std::string (reinterpret_cast<const char*>(&entry), sizeof (Long64_t)));
	}
	socket->write (BackgroundSocket::T_SET_INPUT_FILE, *fileName);

	for (Long64_t entry = skipEvents, num = inTreeEntries;
	     entry != num && count != maxEvents; ++ entry)
	{
	  socket->write (BackgroundSocket::T_SET_ENTRY, std::string (reinterpret_cast<const char*>(&entry), sizeof (Long64_t)));
	  socket->write (BackgroundSocket::T_EXECUTE, "");
	  {
	    BackgroundSocket::Type type;
	    std::string payload;
	    socket->read (type, payload);
	    if (type != BackgroundSocket::T_EXECUTE_CONFIRM)
	      RCU_THROW_MSG ("no confirmation of execution");
	  }

	  if (++ count % 10000 == 0)
	    std::cout << "Processed " << count << " events" << std::endl;
	}
	skipEvents = 0;
      }
      std::string histLocation
	= location + "/fetch-" + ((*sample)->name()) + ".root";
      socket->write (BackgroundSocket::T_END_JOB, histLocation);
      {
	BackgroundSocket::Type type;
	std::string payload;
	socket->read (type, payload);
	if (type != BackgroundSocket::T_END_JOB_CONFIRM)
	  RCU_THROW_MSG ("no confirmation of end job");
      }
      {
	std::auto_ptr<TFile> file (TFile::Open (histLocation.c_str(), "READ"));
	std::auto_ptr<TList> list;
	if (file.get())
	  list.reset (dynamic_cast<TList*>(file->Get ("hist")));
	if (list.get())
	  Driver::saveOutput (location, (*sample)->name(), *list);
      }
      int status = 0;
      if (::wait (&status) == -1)
	RCU_THROW_MSG ("process wait failed");
      if (WIFSIGNALED (status) || WEXITSTATUS (status) != 0)
	RCU_THROW_MSG ("subprocess failed");
    }
    diskOutputSave (location, job);
  }
}
