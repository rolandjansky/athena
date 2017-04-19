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

#include <EventLoop/BackgroundTSelector.h>

#include <EventLoop/BackgroundJob.h>
#include <EventLoop/BackgroundSocket.h>
#include <EventLoop/OutputStream.h>
#include <EventLoop/ProofArgs.h>
#include <EventLoop/ProofDriver.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ShellExec.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/DiskWriter.h>
#include <TFile.h>
#include <TProofOutputFile.h>
#include <TTree.h>
#include <stdlib.h>
#include <unistd.h>

//
// method implementations
//

ClassImp (EL::BackgroundTSelector)

namespace EL
{
  namespace
  {
    struct MyWriter : public SH::DiskWriter
    {
      /// description: this is a custom writer for the old-school
      ///   drivers that don't use an actual writer

    public:
      TFile *m_file;

      explicit MyWriter (TFile *val_file)
	: m_file (val_file)
      {}

      ~MyWriter ()
      {
	if (m_file != 0)
	  close();
      }

      std::string getPath () const
      {
	return "";
      }

      TFile *getFile ()
      {
	RCU_REQUIRE2_SOFT (m_file != 0, "file already closed");
	return m_file;
      }

      void doClose ()
      {
	RCU_REQUIRE2_SOFT (m_file != 0, "file already closed");
	m_file->Write ();
	m_file = 0;
      }
    };
  }



  void BackgroundTSelector ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  BackgroundTSelector ::
  BackgroundTSelector ()
    : m_args (0),
      m_tree (0),
      m_currentInput (false)
  {
    RCU_NEW_INVARIANT (this);
  }



  BackgroundTSelector ::
  ~BackgroundTSelector ()
  {
    RCU_DESTROY_INVARIANT (this);
  }



  void BackgroundTSelector ::
  SlaveBegin (TTree *tree)
  {
    RCU_CHANGE_INVARIANT (this);

    RCU_ASSERT (m_args == 0);
    {
      TIter iter (fInput);
      TObject *object = 0;
      while ((object = iter.Next()) && !m_args)
	m_args = dynamic_cast<ProofArgs*>(object);
    }
    RCU_ASSERT (m_args != 0);

    BackgroundJob job;
    for (std::vector<Algorithm*>::const_iterator alg = m_args->algs.begin(),
	   end = m_args->algs.end(); alg != end; ++ alg)
      job.algs.push_back (*alg);

    for (std::vector<OutputStream>::const_iterator out = m_args->output.begin(),
	   end = m_args->output.end(); out != end; ++ out)
    {
      RCU_ASSERT0_NOIMP ("don't have output yet");
      TProofOutputFile *output = 0;

      if (!m_args->driver->returnFiles)
      {
	output = new TProofOutputFile
	  ((out->label() + ".root").c_str(), TProofOutputFile::kDataset,
	   TProofOutputFile::kRegister | TProofOutputFile::kVerify);
	output->SetOutputFileName ((out->label() + ".root").c_str());
      } else
      {
	output = new TProofOutputFile
	  ((out->label() + ".root").c_str(), TProofOutputFile::kMerge,
	   TProofOutputFile::kRemote);
	const std::string name = Driver::mergedOutputName
	  (m_args->output_dir, *out, m_args->sample_name);
	output->SetOutputFileName (name.c_str());
      }
      m_outputFiles.push_back (output);
      OutputStream stream (out->label());
      // stream.output (new MyWriter (output->OpenFile ("RECREATE")));
      job.output.push_back (stream);
      // output->Print ("all");
      // m_worker->addOutputWriter (out->label(),
      // 				 );
      // output->Print ("all");
    }

    job.meta = m_args->sample_meta;

    m_socket = BackgroundSocket::startWorker (job);
    m_currentInput = false;
    m_tree = tree;

    /*
    m_worker = new ProofWorker (&m_args->sample_meta, fOutput, this);
    */
  }



  void BackgroundTSelector ::
  Init (TTree* tree)
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE (m_socket.get() != 0);

    m_currentInput = false;
    m_tree = tree;
  }



  Bool_t BackgroundTSelector ::
  Notify()
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE (m_socket.get() != 0);

    m_currentInput = false;
    return kTRUE;
  }



  Bool_t BackgroundTSelector ::
  Process (Long64_t entry)
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE (m_socket.get() != 0);

    try
    {
      m_socket->write (BackgroundSocket::T_SET_ENTRY, std::string (reinterpret_cast<const char*>(&entry), sizeof (Long64_t)));
      if (!m_currentInput)
      {
	m_socket->write (BackgroundSocket::T_SET_INPUT_FILE, m_tree->GetCurrentFile()->GetName());
      }
      m_socket->write (BackgroundSocket::T_EXECUTE, "");
      {
	BackgroundSocket::Type type;
	std::string payload;
	m_socket->read (type, payload);
	if (type != BackgroundSocket::T_EXECUTE_CONFIRM)
	  RCU_THROW_MSG ("no confirmation of execution");
      }
    } catch (std::string& e)
    {
      Abort (("caught exception in process: " + e).c_str());
    } catch (std::exception& e)
    {
      Abort ((std::string ("caught exception in process: ") + e.what()).c_str());
    } catch (...)
    {
      Abort ("caught unknown exception in process");
    }

    return kFALSE;
  }



  void BackgroundTSelector ::
  SlaveTerminate ()
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE (m_socket.get() != 0);

    try
    {
      char tmpdir [30] = "/tmp/el_bg_output_XXXXXX";
      if (!mkdtemp (tmpdir))
	RCU_THROW_MSG ("failed to create temporary directory");
      std::string histLocation = std::string (tmpdir) + "/hist.root";
      m_socket->write (BackgroundSocket::T_END_JOB, histLocation);
      {
	BackgroundSocket::Type type;
	std::string payload;
	m_socket->read (type, payload);
	if (type != BackgroundSocket::T_END_JOB_CONFIRM)
	  RCU_THROW_MSG ("no confirmation of end job");
      }
      {
	std::auto_ptr<TFile> file (TFile::Open (histLocation.c_str(), "READ"));
	std::auto_ptr<TList> list;
	if (file.get())
	  list.reset (dynamic_cast<TList*>(file->Get ("hist")));
	if (list.get())
	{
	  TIter iter (list.get());
	  TObject *object = 0;
	  while ((object = iter.Next()))
	  {
	    fOutput->Add (object);
	  }
	  list->Clear ("nodelete");
	}
      }
      for (std::vector<TProofOutputFile*>::const_iterator
	     out = m_outputFiles.begin(), end = m_outputFiles.end();
	   out != end; ++ out)
      {
	(*out)->Print ("all");
	fOutput->Add (*out);
      }
      RCU::Shell::exec (std::string ("rm -rf ") + tmpdir);
    } catch (std::string& e)
    {
      Abort (("caught exception in terminate: " + e).c_str());
    } catch (std::exception& e)
    {
      Abort ((std::string ("caught exception in terminate: ") + e.what()).c_str());
    } catch (...)
    {
      Abort ("caught unknown exception in terminate");
    }
  }



  int BackgroundTSelector ::
  Version () const
  {
    RCU_CHANGE_INVARIANT (this);
    return 1;
  }
}
