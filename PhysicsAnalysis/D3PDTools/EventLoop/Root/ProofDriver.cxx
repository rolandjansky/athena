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

#include <EventLoop/ProofDriver.h>

#include <EventLoop/Algorithm.h>
#include <EventLoop/EmptyFileHandler.h>
#include <EventLoop/Job.h>
#include <EventLoop/OutputStream.h>
#include <EventLoop/ProofArgs.h>
#include <RootCoreUtils/ranLoadPackages.h>
#include <RootCoreUtils/ShellExec.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/MetaData.h>
#include <SampleHandler/Sample.h>
#include <SampleHandler/SampleLocal.h>
#include <TChain.h>
#include <TDirectory.h>
#include <TFile.h>
#include <TFileCollection.h>
#include <TFileInfo.h>
#include <THashList.h>
#include <TList.h>
#include <TProof.h>
#include <TProofLog.h>
#include <TQueryResult.h>
#include <TSystem.h>  
#include <TString.h>  
#include <TEnv.h> 
#include <iostream>
#include <memory>
#include <sstream>

//
// method implementations
//

ClassImp(EL::ProofDriver)

namespace EL
{
  namespace
  {
    /// effects: print the worker logs
    /// guarantee: basic
    /// failures: proof dependent
    /// rationale: inspired by http://sourceforge.net/p/sframe/code/HEAD/tree/SFrame/trunk/core/src/SProofManager.cxx#l271
    void printWorkerLogs (TProof& proof)
    {
      TList* slaveInfos = proof.GetListOfSlaveInfos();

      std::auto_ptr<TProofLog> log (proof.GetManager()->GetSessionLogs());
      TList* logList = log->GetListOfLogs();
      for (Int_t iter = 0; iter < logList->GetSize(); ++ iter)
      {
	// Access the log of a single node:

	TProofLogElem* element
	  = dynamic_cast<TProofLogElem*>(logList->At (iter));
	RCU_ASSERT (element != 0);

	// Find "the name" of the node. TProofLogElem objects only know that
	// they came from node "0.2" for instance. This small loop matches
	// these identifiers to the proper node names in the slaveInfos list.
	//
	// If the identifier is not found in the list, then it has to be the
	// master...
	//
	std::string nodeName;
	if (strcmp (element->GetName(), "0") == 0)
	  nodeName = proof.GetMaster();
	for (Int_t jter = 0; jter < slaveInfos->GetSize() && nodeName.empty();
	     ++ jter)
	{
	  // Access the TSlaveInfo object:
	  TSlaveInfo* info =
	    dynamic_cast<TSlaveInfo*>(slaveInfos->At (jter));
	  RCU_ASSERT (info != 0);

	  // Check if this TSlaveInfo describes the source of the log:
	  if (strcmp (element->GetName(), info->GetOrdinal()) == 0)
	    nodeName = info->GetName();
	}
	if (nodeName.empty())
	  nodeName = element->GetName();

	//
	// Print the log. Note that we don't need to redirect the log lines
	// to m_logger. The log lines of the nodes will already be formatted,
	// so printing them through SLogger would just look ugly.
	//
	std::cout << "Output from node: " << nodeName << " ("
                  << element->GetName() << ")" << std::endl;
	element->GetMacro()->Print();
	std::cout << std::endl;
      }
    }
  }



  const std::string ProofDriver::queryResultName = "EventLoop_TQueryResult";
  const std::string ProofDriver::optionParFile = "EventLoop_ParFile";



  void ProofDriver ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  ProofDriver ::
  ProofDriver ()
    : returnFiles (true), numWorkers (0), makeParOptions ("--lite")
  {
    proofParm.setDouble ("PROOF_UseTreeCache", 0);

    RCU_NEW_INVARIANT (this);
  }



  void ProofDriver ::
  removePar () const
  {
    RCU_READ_INVARIANT (this);

    std::auto_ptr<TProof> proof (TProof::Open (proofMaster.c_str()));
    if (proof->ClearPackage ("RootCore") == -1)
      RCU_THROW_MSG ("failed to clear package RootCore");
  }



  void ProofDriver ::
  doSubmit (const Job& job, const std::string& location) const
  {
    RCU_READ_INVARIANT (this);

    const std::string sourceDir = gSystem->WorkingDirectory ();
    const std::string tmpDir = location + "/tmp";

    try
    {
      if (gSystem->MakeDirectory (tmpDir.c_str()) != 0)
	RCU_THROW_MSG ("failed to create directory " + tmpDir);
      if (gSystem->ChangeDirectory (tmpDir.c_str()) == false)
	RCU_THROW_MSG ("failed to create directory " + tmpDir);
      
      std::string server = proofMaster;
      if (server.empty())
      {
	server = "lite://";
      } else
      {
	if (server.find ("://") >= server.find ("/"))
	  server = "proof://" + server;
	if (server.find ("//", server.find ("://")+3) == std::string::npos)
	  server = server + "//";
        TProof::AddEnvVar("PROOF_INITCMD", gSystem->GetFromPipe("source $ROOTCOREBIN/../rcSetup.sh -P"));
      }
      std::ostringstream worker_options;
      if (numWorkers > 0)
      {
	worker_options << "workers=" << numWorkers;
        TProof::AddEnvVar("PROOF_NWORKERS", TString::Format("%d",numWorkers) );
      }
      TProof::AddEnvVar("PROOF_INTWAIT", "30000");
      gEnv->SetValue("Proof.SocketActivityTimeout",30000);
      std::auto_ptr<TProof> proof (TProof::Open (server.c_str() ) );
      
//       proof->SetLogLevel (15);
      {
	TObject *object = 0;
	for (TIter iter (&proofParm); (object = iter.Next()); )
	{
	  const SH::MetaData<std::string> *pstring = 0;
	  const SH::MetaData<double> *pdouble = 0;
	  if ((pstring = dynamic_cast<const SH::MetaData<std::string>*>(object)))
	  {
	    proof->SetParameter (object->GetName(), pstring->value.c_str());
	  } else if ((pdouble = dynamic_cast<const SH::MetaData<double>*>(object)))
	  {
	    proof->SetParameter (object->GetName(), pdouble->value);
	  } else
	  {
	    RCU_THROW_MSG ("don't know the type of parameter: " + std::string (object->GetName()));
	  }
	}
      }

      SH::MetaObject jobMeta = *job.options();
      jobMeta.fetchDefaults (*options());
      std::string parPath = jobMeta.castString (optionParFile, location + "/RootCore.par");
      RCU::Shell::exec ("$ROOTCOREDIR/scripts/make_par.sh " + makeParOptions + " " + parPath);
      // proof->SetQueryMode (TProof::kSync);
      if (proof->UploadPackage (parPath.c_str()) == -1)
      {
	printWorkerLogs (*proof);
	RCU_THROW_MSG (("failed to upload package " + parPath).c_str());
      }
      if (proof->EnablePackage ("RootCore", RCU::ranLoadPackages()) == -1)
	RCU_THROW_MSG ("failed to upload package RootCore");
       proof->ShowEnabledPackages ();

      std::map<std::string,SH::SampleHandler> sh_out;
      if (returnFiles)
      {
	mergedOutputMkdir (location, job);
	mergedOutputSave (location, job);
      }

      for (std::size_t sampleIndex = 0, end = job.sampleHandler().size();
	   sampleIndex != end; ++ sampleIndex)
      {
	const SH::Sample *const sample = job.sampleHandler()[sampleIndex];

	proof->ClearInput ();
	if (job.options()->castBool (Job::optPerfTree, false))
	  proof->SetPerfTree ();
	std::auto_ptr<ProofArgs> args (new ProofArgs);
	args->driver = static_cast<ProofDriver*>(Clone());
	for (Job::algsIter alg = job.algsBegin(), end = job.algsEnd();
	     alg != end; ++ alg)
	{
	  args->algs.push_back (0);
	  args->algs.back() = dynamic_cast<Algorithm*>((*alg)->Clone());
	}
	for (Job::outputIter out = job.outputBegin(), end = job.outputEnd();
	     out != end; ++ out)
	  args->output.push_back (*out);
	args->sample_name = sample->name();
	args->sample_meta = *sample->meta();
	args->sample_meta.fetchDefaults (*job.options());
	args->sample_meta.fetchDefaults (*options());
	args->output_dir = location;
	const Long64_t maxEvents
	  = Long64_t (args->sample_meta.castDouble (Job::optMaxEvents, -1));
	const Long64_t skipEvents
	  = Long64_t (args->sample_meta.castDouble (Job::optSkipEvents, 0));
	std::string selector = "EL::ProofTSelector";
	if (args->sample_meta.castBool (Job::optBackgroundProcess))
	  selector = "EL::BackgroundTSelector";

	proof->AddInput (args.release());
	proof->AddInput (sample->Clone());

	std::auto_ptr<TDSet> dset (sample->makeTDSet());
	Long64_t result = proof->Process (dset.get(), selector.c_str(), "", maxEvents, skipEvents);
	// proof->ShowQueries ();
 	if (result == -1)
	{
	  std::auto_ptr<TChain> chain (sample->makeTChain());
	  if (chain->GetEntries() != 0)
	    RCU_THROW_MSG ("PROOF processing of sample " + sample->name() + " failed");
	  RCU_WARN_MSG ("empty sample " + sample->name() + " ignored for proof processing");
	}

	TList *outputList = proof->GetOutputList();

	if (result != -1)
	{
	  TQueryResult *query = proof->GetQueryResult ();
	  query = dynamic_cast<TQueryResult*>(query->Clone (queryResultName.c_str()));
	  outputList->Add (query);
	  if (job.options()->castBool (Job::optPerfTree, false))
	    proof->SavePerfTree ((location + "/perftree-" + sample->name() + ".root").c_str());
	}
	if (!returnFiles)
	{
	  TDirectory *origDir = gDirectory;
	  for (Job::outputIter out = job.outputBegin(),
		 end = job.outputEnd(); out != end; ++ out)
	  {
	    std::auto_ptr<SH::SampleLocal> mysample
	      (new SH::SampleLocal (sample->name()));
	    std::string outputName = out->label() + ".root";
	    TFileCollection *coll = dynamic_cast<TFileCollection*>
	      (proof->GetOutputList()->FindObject ((out->label() + ".root").c_str()));
	    RCU_ASSERT (coll);
	    TIter iter (coll->GetList ());
	    TObject *obj = 0;
	    while ((obj = iter.Next()))
	    {
	      TFileInfo *info = dynamic_cast<TFileInfo*>(obj);
	      RCU_ASSERT (info);
	      std::string fileName = info->GetCurrentUrl()->GetUrl();
	      std::auto_ptr<TFile> myfile
		(TFile::Open (fileName.c_str(), "READ"));
	      TIter myiter (myfile->GetListOfKeys());
	      if (myiter.Next())
		mysample->add (info->GetCurrentUrl()->GetUrl());
	    }
	    sh_out[out->label()].add (mysample.release());
	  }
	gDirectory = origDir;
	}

	saveOutput (location, sample->name(), *outputList);
	// printWorkerLogs (*proof);
      }
      if (!returnFiles)
      {
	for (Job::outputIter out = job.outputBegin(),
	       end = job.outputEnd(); out != end; ++ out)
	{
	  sh_out[out->label()].save (location + "/output-" + out->label());
	}
      }
      gSystem->ChangeDirectory (sourceDir.c_str());
    } catch (...)
    {
      gSystem->ChangeDirectory (sourceDir.c_str());
      throw;
    }

    processEmptyFiles (location, job);
  }
}
