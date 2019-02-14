/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EventLoop/EmptyFileHandler.h>

#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ShellExec.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <RootCoreUtils/hadd.h>
#include <EventLoop/DirectDriver.h>
#include <EventLoop/Job.h>
#include <EventLoop/OutputStream.h>
#include <SampleHandler/MetaFields.h>
#include <SampleHandler/MetaObject.h>
#include <SampleHandler/SampleHandler.h>
#include <SampleHandler/SampleLocal.h>
#include <TList.h>
#include <TChain.h>
#include <TFile.h>
#include <TObjString.h>
#include <memory>

#include <iostream>

//
// method implementations
//

namespace EL
{
  namespace
  {
    std::vector<std::string>
    readVectorFromTree (SH::Sample *sample, const std::string& treeName,
			const std::string& branchName)
    {
      std::vector<std::string> result;
      auto fileNames = sample->makeFileList();
      TString *var = nullptr;
      for (auto& fileName : fileNames)
      {
	std::unique_ptr<TFile> file (TFile::Open (fileName.c_str(), "READ"));
	if (file == nullptr)
	  RCU_THROW_MSG ("failed to open file: " + fileName);
	TTree *tree = dynamic_cast<TTree*>(file->Get (treeName.c_str()));
	Long64_t nentries = 0;
	if (tree != nullptr && (nentries = tree->GetEntries()) > 0)
	{
	  TBranch *branch = nullptr;
	  tree->SetBranchAddress (branchName.c_str(), &var, &branch);
	  for (Long64_t entry = 0; entry < nentries; ++ entry)
	  {
	    if (branch->GetEntry(entry) <= 0)
	      RCU_THROW_MSG ("failed to read entry from branch");
	    result.push_back (var->Data());
	  }
	}
      }
      return result;
    }



    SH::SampleHandler makeReprocessSamples (const std::string& submitdir)
    {
      /// \brief the input sample handler
      SH::SampleHandler shInput;
      shInput.load (submitdir + "/input");

      /// \brief the histogram sample handler
      SH::SampleHandler shHist;
      shHist.load (submitdir + "/hist");
      shHist.setMetaString (SH::MetaFields::treeName, "EventLoop_FileExecuted");

      /// \brief the list of samples we need to reprocess
      SH::SampleHandler shReprocess;

      for (SH::Sample *sampleInput : shInput)
      {
	SH::Sample *sampleHist = shHist.get (sampleInput->name());
	RCU_ASSERT (sampleHist != nullptr);
	std::vector<std::string> filesUsedVector
	  = readVectorFromTree (sampleHist, "EventLoop_FileExecuted", "file");
	std::set<std::string> filesUsed
	  (filesUsedVector.begin(), filesUsedVector.end());
	RCU_ASSERT (filesUsed.size() == filesUsedVector.size());

	std::unique_ptr<SH::SampleLocal> sampleReprocess;
	std::vector<std::string> filesInput (sampleInput->makeFileList ());
	  
	for (auto fileInput : filesInput)
	{
	  std::string fileName = fileInput;
	  auto split = fileName.rfind ('/');
	  if (split != std::string::npos)
	    fileName = fileName.substr (split + 1);

	  if (filesUsed.find (fileName) == filesUsed.end())
	  {
	    if (sampleReprocess == nullptr)
	    {
	      sampleReprocess.reset (new SH::SampleLocal (sampleInput->name()));
	      *sampleReprocess->meta() = *sampleInput->meta();
	    }
	    sampleReprocess->add (fileInput);
	  }
	}
	if (filesInput.empty())
	{
	  sampleReprocess.reset (new SH::SampleLocal (sampleInput->name()));
	  *sampleReprocess->meta() = *sampleInput->meta();
	}
	if (sampleReprocess != nullptr)
	{
	  RCU_ASSERT (sampleReprocess->makeFileList().size() + filesUsed.size() == filesInput.size());
	  shReprocess.add (sampleReprocess.release());
	}
      }
      return shReprocess;
    }



    void mergeHists (const std::string& mainDir,
		     const std::string& extraDir,
		     const SH::SampleHandler& samples)
    {
      for (auto& sample : samples)
      {
	std::string tmp = extraDir + "/hist2-" + sample->name() + ".root";
	std::string target = mainDir + "/hist-" + sample->name() + ".root";

	RCU::hadd (tmp, {target, extraDir + "/hist-" + sample->name() + ".root"});
	RCU::Shell::exec ("mv -f " + RCU::Shell::quote (tmp) + " " + RCU::Shell::quote (target));
      }
    }



    void mergeNtuple (const std::string& mainDir,
		      const std::string& extraDir)
    {
      SH::SampleHandler mainSH;
      mainSH.load (mainDir);
      SH::SampleHandler extraSH;
      extraSH.load (extraDir);
      SH::SampleHandler newSH;

      for (SH::Sample *extraSample : extraSH)
      {
	SH::Sample *mainSample = mainSH.get (extraSample->name());
	RCU_ASSERT (mainSample != nullptr);
	std::unique_ptr<SH::SampleLocal> newSample
	  (new SH::SampleLocal (extraSample->name()));
	*newSample->meta() = *mainSample->meta();

	for (auto& file : mainSample->makeFileList())
	  newSample->add (file);
	for (auto& file : extraSample->makeFileList())
	  newSample->add (file);
	newSH.add (newSample.release());
      }

      for (SH::Sample *mainSample : mainSH)
      {
	if (newSH.get (mainSample->name()) == nullptr)
	  newSH.add (mainSample);
      }

      mainSH.save (mainDir);
    }
  }

  void processEmptyFiles (const std::string& submitdir,
			  const Job& job)
  {
    SH::SampleHandler shReprocess = makeReprocessSamples (submitdir);

    if (shReprocess.size() > 0)
    {
      std::string mysubmitdir = submitdir + "/emptyFiles";
      Job myjob = job;
      myjob.sampleHandler (shReprocess);

      DirectDriver driver;
      driver.submit (myjob, mysubmitdir);

      mergeHists (submitdir, mysubmitdir, shReprocess);
      for (auto output = job.outputBegin(),
	     end = job.outputEnd(); output != end; ++ output)
      {
	mergeNtuple (submitdir + "/output-" + output->label(),
		     mysubmitdir + "/output-" + output->label());
      }
    }
  }
}
