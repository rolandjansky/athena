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

#include <EventLoop/ProofTSelector.h>

#include <EventLoop/Algorithm.h>
#include <EventLoop/OutputStream.h>
#include <EventLoop/ProofArgs.h>
#include <EventLoop/ProofDriver.h>
#include <EventLoop/ProofWorker.h>
#include <RootCoreUtils/Assert.h>
#include <SampleHandler/DiskWriter.h>
#include <TFile.h>
#include <TProofOutputFile.h>

//
// method implementations
//

ClassImp (EL::ProofTSelector)

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



  void ProofTSelector ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  ProofTSelector ::
  ProofTSelector ()
    : m_args (0),
      m_worker (0),
      m_currentInput (false)
  {
    RCU_NEW_INVARIANT (this);
  }



  ProofTSelector ::
  ~ProofTSelector ()
  {
    RCU_DESTROY_INVARIANT (this);

    delete m_worker;
  }



  void ProofTSelector ::
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

    m_worker = new ProofWorker (&m_args->sample_meta, fOutput, this);

    for (std::vector<Algorithm*>::const_iterator alg = m_args->algs.begin(),
	   end = m_args->algs.end(); alg != end; ++ alg)
      m_worker->addAlg (dynamic_cast<EL::Algorithm*>((*alg)->Clone()));
      
    for (std::vector<OutputStream>::const_iterator out = m_args->output.begin(),
	   end = m_args->output.end(); out != end; ++ out)
    {
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
      // output->Print ("all");
      m_worker->addOutputWriter (out->label(),
				 new MyWriter (output->OpenFile ("RECREATE")));
      // output->Print ("all");
      m_outputFiles.push_back (output);
    }

    m_currentInput = false;
    m_worker->setTreeProofOnly (tree);
  }



  void ProofTSelector ::
  Init (TTree* tree)
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE (m_worker != 0);

    m_currentInput = false;
    m_worker->setTreeProofOnly (tree);
  }



  Bool_t ProofTSelector ::
  Notify()
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE (m_worker != 0);

    m_currentInput = false;
    return kTRUE;
  }



  Bool_t ProofTSelector ::
  Process (Long64_t entry)
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE (m_worker != 0);

    try
    {
      m_worker->treeEntry (entry);
      while (!m_currentInput)
      {
	m_currentInput = true;
	m_worker->algsChangeInput ();
      }
      m_worker->algsExecute ();
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



  void ProofTSelector ::
  SlaveTerminate ()
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE (m_worker != 0);

    try
    {
      for (std::vector<TProofOutputFile*>::const_iterator
	     out = m_outputFiles.begin(), end = m_outputFiles.end();
	   out != end; ++ out)
      {
	(*out)->Print ("all");
	fOutput->Add (*out);
      }
      m_worker->algsFinalize ();
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



  int ProofTSelector ::
  Version () const
  {
    RCU_CHANGE_INVARIANT (this);
    return 1;
  }
}
