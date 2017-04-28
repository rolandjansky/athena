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

#include <EventLoop/BackgroundWorker.h>

#include <EventLoop/Algorithm.h>
#include <EventLoop/BackgroundJob.h>
#include <EventLoop/BackgroundSocket.h>
#include <EventLoop/OutputStream.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ShellExec.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/DiskOutput.h>
#include <SampleHandler/MetaFields.h>
#include <TFile.h>

//
// method implementations
//

namespace EL
{
  void BackgroundWorker ::
  execute (int val_socket)
  {
    BackgroundSocket socket (val_socket);
    TList output;
    std::string outputFile;
    std::auto_ptr<BackgroundJob> job;
    std::auto_ptr<BackgroundWorker> worker;
    std::auto_ptr<TFile> file;

    while (true)
    {
      BackgroundSocket::Type type = BackgroundSocket::T_INVALID;
      std::string payload;
      socket.read (type, payload);
      switch (type)
      {
      case BackgroundSocket::T_START_JOB:
	RCU_ASSERT_SOFT (worker.get() == 0);
	{
	  std::auto_ptr<TFile> file (TFile::Open ((payload + "/job.root").c_str(), "READ"));
	  if (file.get() == 0)
	    RCU_THROW_MSG ("failed to open file: " + payload);
	  TObject *object = file->Get ("job");
	  if (object == 0)
	    RCU_THROW_MSG ("failed to read job object from file: " + payload);
	  job.reset (dynamic_cast<BackgroundJob*>(object));
	  if (job.get() == 0)
	    RCU_THROW_MSG ("job object not of Job type in file: " + payload + " " + object->ClassName());
	  worker.reset (new BackgroundWorker (*job, &output));
	}
	RCU::Shell::exec ("rm -rf " + payload);
	break;
      case BackgroundSocket::T_SET_INPUT_FILE:
	RCU_ASSERT_SOFT (worker.get() != 0);
	if (file.get() != 0)
	{
	  worker->algsEndOfFile ();
	  file.reset ();
	}
	file.reset (TFile::Open (payload.c_str(), "READ"));
	worker->setInputFile (file.get());
	worker->algsChangeInput ();
	break;
      case BackgroundSocket::T_SET_ENTRY:
	{
	  RCU_ASSERT_SOFT (worker.get() != 0);
	  Long64_t entry = *reinterpret_cast<const Long64_t*>(payload.data());
	  worker->treeEntry (entry);
	}
	break;
      case BackgroundSocket::T_EXECUTE:
	RCU_ASSERT_SOFT (worker.get() != 0);
	worker->algsExecute ();
	socket.write (BackgroundSocket::T_EXECUTE_CONFIRM, "");
	break;
      case BackgroundSocket::T_END_JOB:
	if (worker.get() != 0)
	{
	  if (file.get() != 0)
	  {
	    worker->algsEndOfFile ();
	    file.reset ();
	  }
	  worker->algsFinalize ();
	  std::auto_ptr<TFile> file (TFile::Open (payload.c_str(), "RECREATE"));
	  file->WriteObject (&output, "hist");
	}
	socket.write (BackgroundSocket::T_END_JOB_CONFIRM, "");
	return;
      case BackgroundSocket::T_EXECUTE_CONFIRM:
      case BackgroundSocket::T_END_JOB_CONFIRM:
      case BackgroundSocket::T_INVALID:
	RCU_THROW_MSG ("read invalid message");
	break; //compiler dummy
      }
    }
  }



  void BackgroundWorker ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  BackgroundWorker ::
  BackgroundWorker (const BackgroundJob& job, TList *output)
    : Worker (&job.meta, output)
  {
    const std::string name =
      job.meta.castString (SH::MetaFields::sampleName);

    for (std::vector<OutputStream>::const_iterator out = job.output.begin(),
	   end = job.output.end(); out != end; ++ out)
    {
      addOutputWriter (out->label(), out->output()->makeWriter (name, "", -1, ".root"));
    }

    for (std::vector<Algorithm*>::const_iterator alg = job.algs.begin(),
	   end = job.algs.end(); alg != end; ++ alg)
      addAlg (dynamic_cast<Algorithm*>((*alg)->Clone ()));

    RCU_NEW_INVARIANT (this);
  }
}
