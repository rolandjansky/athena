/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EventLoopTest/UnitTestFixture.h>

#include <AsgTools/MessageCheck.h>
#include <EventLoop/Driver.h>
#include <AnaAlgorithm/AnaAlgorithmConfig.h>
#include <EventLoopTest/UnitTestAlg1.h>
#include <EventLoopTest/UnitTestConfig.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/DiskWriter.h>
#include <SampleHandler/MetaFields.h>
#include <SampleHandler/SampleHandler.h>
#include <SampleHandler/SampleLocal.h>
#include <SampleHandler/SamplePtr.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <TTree.h>
#include <TSystem.h>

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
  }




  std::map<std::shared_ptr<Driver>,std::string> UnitTestFixture::m_jobs;

  SH::SamplePtr UnitTestFixture ::
  getSample (const std::string& sampleName)
  {
    if (sampleName == "null")
    {
      static SH::SamplePtr result;
      if (result.empty())
      {
	std::unique_ptr<SH::SampleLocal> myresult (new SH::SampleLocal ("null"));
	result = myresult.release();
      }
      return result;
    }
    if (sampleName == "empty")
    {
      static SH::SamplePtr result;
      if (result.empty())
      {
	std::unique_ptr<SH::SampleLocal> myresult (new SH::SampleLocal ("empty"));
	myresult->add (makeFile ({}));
	result = myresult.release();
      }
      return result;
    }
    if (sampleName == "single")
    {
      static SH::SamplePtr result;
      if (result.empty())
      {
	std::unique_ptr<SH::SampleLocal> myresult (new SH::SampleLocal ("single"));
	std::vector<unsigned> entries;
	for (unsigned iter = 0; iter != 10000; ++ iter)
	  entries.push_back (iter % 10);
	myresult->add (makeFile (entries));
	result = myresult.release();
      }
      return result;
    }
    if (sampleName == "multi")
    {
      static SH::SamplePtr result;
      if (result.empty())
      {
	std::unique_ptr<SH::SampleLocal> myresult (new SH::SampleLocal ("multi"));
	for (unsigned jter = 0; jter != 100; ++ jter)
	{
	  std::vector<unsigned> entries;
	  for (unsigned iter = 0; iter != 10000; ++ iter)
	    entries.push_back (iter % 10);
	  myresult->add (makeFile (entries));
	}
	result = myresult.release();
      }
      return result;
    }
    RCU_THROW_MSG ("unknown sample: " + sampleName);
  }



  SH::SampleHandler UnitTestFixture ::
  getSH ()
  {
    SH::SampleHandler sh;
    sh.add (getSample ("null"));
    sh.add (getSample ("empty"));
    sh.add (getSample ("single"));
    sh.add (getSample ("multi"));
    sh.setMetaString (SH::MetaFields::treeName, "physics");
    return sh;
  }



  std::string UnitTestFixture ::
  getJob ()
  {
    using namespace asg::msgUserCode;
    ANA_MSG_INFO ("in EventLoopTest");

    std::shared_ptr<Driver> driver = GetParam().m_driver;
    auto iter = m_jobs.find (driver);
    if (iter != m_jobs.end())
      return iter->second;

    Job job;
    job.sampleHandler (getSH());
    GetParam().setupJob (job);
    {
      EL::AnaAlgorithmConfig config;
      config.setType ("EL::UnitTestAlg2");
      config.setName ("newAlg");
      config.setUseXAODs (false);
      ANA_CHECK_THROW (config.setProperty ("property", 42));
      job.algsAdd (config);
    }
    {
      std::unique_ptr<UnitTestAlg1> alg (new UnitTestAlg1);
      job.algsAdd (alg.release());
    }

    std::ostringstream submit;
    submit << "submit-" << driver.get();
    driver->submit (job, submit.str());
    driver->wait (submit.str());
    m_jobs[driver] = submit.str();
    return submit.str();
  }



  TObject *UnitTestFixture ::
  getTObject (const std::string& sampleName,
	      const std::string& objectName,
	      bool isMandatory)
  {
    SH::SampleHandler sh;
    sh.load (getJob() + "/hist");
    SH::Sample *sample = sh.get (sampleName);
    if (sample == nullptr)
      RCU_THROW_MSG ("couldn't find sample: " + sampleName);
    TObject *object = sample->readHist (objectName);
    if (isMandatory && object == nullptr)
      RCU_THROW_MSG ("couldn't find object: " + objectName);
    return object;
  }



  unsigned UnitTestFixture ::
  eventCount (const std::string& sampleName)
  {
    TH1 *hist = getHist<TH1> (sampleName, "EventLoop_EventCount", true);
    return hist->GetBinContent (1);
  }



  TH1 *UnitTestFixture ::
  getCallbacks (const std::string& sampleName)
  {
    return getHist<TH1> (sampleName, "callbacks", true);
  }



  void UnitTestFixture ::
  checkFileExecuted (const std::string& sampleName)
  {
    std::set<std::string> filesOut;
    {
      SH::SampleHandler sh;
      sh.load (getJob() + "/hist");
      SH::Sample *sample = sh.get (sampleName);
      if (sample == nullptr)
	RCU_THROW_MSG ("couldn't find sample: " + sampleName);
      auto vec = readVectorFromTree (sample, "EventLoop_FileExecuted", "file");
      filesOut.insert (vec.begin(), vec.end());
    }
    std::set<std::string> filesIn;
    for (auto& file : getSample (sampleName)->makeFileList())
    {
      auto split = file.rfind ('/');
      if (split == std::string::npos)
	split = 0;
      else
	++ split;
      std::string fileName = file.substr (split);
      ASSERT_TRUE (filesIn.find (fileName) == filesIn.end());
      filesIn.insert (fileName);
    }
    ASSERT_EQ (filesIn, filesOut);
  }



  std::string UnitTestFixture ::
  makeFile (const std::vector<unsigned>& entries)
  {
    static unsigned index = 0;
    std::ostringstream fileName;
    fileName << "file-" << ++ index << ".root";

    std::unique_ptr<SH::DiskWriter> file
      = GetParam().make_file_writer (fileName.str());
    {
      if (!entries.empty())
      {
	TTree *tree = new TTree ("physics", "physics");
	Int_t el_n = 0;
	tree->Branch ("el_n", &el_n, "el_n/I");
	for (auto entry : entries)
	{
	  el_n = entry;
	  tree->Fill ();
	}
      }
      file->file()->Write ();
      file->close ();
    }
    return file->path();
  }



  TEST_P (UnitTestFixture, null_eventCount)
  {
    ASSERT_EQ (eventCount ("null"), 0u);
  }



  TEST_P (UnitTestFixture, null_callbacks)
  {
    TH1 *callbacks = getCallbacks ("null");
    ASSERT_EQ (0, callbacks->GetBinContent (1 + UnitTestAlg1::CB_CHANGE_INPUT_FIRST));
    ASSERT_EQ (0, callbacks->GetBinContent (1 + UnitTestAlg1::CB_CHANGE_INPUT_OTHER));
    ASSERT_EQ (0, callbacks->GetBinContent (1 + UnitTestAlg1::CB_INITIALIZE));
    ASSERT_LE (1, callbacks->GetBinContent (1 + UnitTestAlg1::CB_HIST_INITIALIZE));
    ASSERT_EQ (0, callbacks->GetBinContent (1 + UnitTestAlg1::CB_EXECUTE));
    ASSERT_EQ (0, callbacks->GetBinContent (1 + UnitTestAlg1::CB_FILE_EXECUTE));
    ASSERT_EQ (0, callbacks->GetBinContent (1 + UnitTestAlg1::CB_FINALIZE));
    ASSERT_LE (1, callbacks->GetBinContent (1 + UnitTestAlg1::CB_HIST_FINALIZE));
  }



  TEST_P (UnitTestFixture, null_fileExecuted)
  {
    checkFileExecuted ("null");
  }



  TEST_P (UnitTestFixture, null_property)
  {
    ASSERT_EQ (getHist<TH1> ("null", "test_property", true)->GetBinContent(1), 42);
  }



  TEST_P (UnitTestFixture, empty_eventCount)
  {
    ASSERT_EQ (eventCount ("empty"), 0u);
  }



  TEST_P (UnitTestFixture, empty_callbacks)
  {
    TH1 *callbacks = getCallbacks ("empty");
    ASSERT_EQ (0, callbacks->GetBinContent (1 + UnitTestAlg1::CB_CHANGE_INPUT_FIRST));
    ASSERT_EQ (0, callbacks->GetBinContent (1 + UnitTestAlg1::CB_CHANGE_INPUT_OTHER));
    ASSERT_EQ (0, callbacks->GetBinContent (1 + UnitTestAlg1::CB_INITIALIZE));
    ASSERT_LE (1, callbacks->GetBinContent (1 + UnitTestAlg1::CB_HIST_INITIALIZE));
    ASSERT_EQ (0, callbacks->GetBinContent (1 + UnitTestAlg1::CB_EXECUTE));
    ASSERT_EQ (1, callbacks->GetBinContent (1 + UnitTestAlg1::CB_FILE_EXECUTE));
    ASSERT_EQ (0, callbacks->GetBinContent (1 + UnitTestAlg1::CB_FINALIZE));
    ASSERT_LE (1, callbacks->GetBinContent (1 + UnitTestAlg1::CB_HIST_FINALIZE));
  }



  TEST_P (UnitTestFixture, empty_fileExecuted)
  {
    checkFileExecuted ("empty");
  }



  TEST_P (UnitTestFixture, single_eventCount)
  {
    ASSERT_EQ (eventCount ("single"), 10000u);
  }



  TEST_P (UnitTestFixture, single_callbacks)
  {
    TH1 *callbacks = getCallbacks ("single");
    ASSERT_LE (1, callbacks->GetBinContent (1 + UnitTestAlg1::CB_CHANGE_INPUT_FIRST));
    ASSERT_LE (0, callbacks->GetBinContent (1 + UnitTestAlg1::CB_CHANGE_INPUT_OTHER));
    ASSERT_LE (1, callbacks->GetBinContent (1 + UnitTestAlg1::CB_INITIALIZE));
    ASSERT_LE (1, callbacks->GetBinContent (1 + UnitTestAlg1::CB_HIST_INITIALIZE));
    ASSERT_EQ (10000, callbacks->GetBinContent (1 + UnitTestAlg1::CB_EXECUTE));
    ASSERT_EQ (1, callbacks->GetBinContent (1 + UnitTestAlg1::CB_FILE_EXECUTE));
    ASSERT_LE (1, callbacks->GetBinContent (1 + UnitTestAlg1::CB_FINALIZE));
    ASSERT_LE (1, callbacks->GetBinContent (1 + UnitTestAlg1::CB_HIST_FINALIZE));
  }



  TEST_P (UnitTestFixture, single_fileExecuted)
  {
    checkFileExecuted ("single");
  }



  TEST_P (UnitTestFixture, multi_eventCount)
  {
    ASSERT_EQ (eventCount ("multi"), 1000000u);
  }



  TEST_P (UnitTestFixture, multi_callbacks)
  {
    TH1 *callbacks = getCallbacks ("multi");
    ASSERT_LE (1, callbacks->GetBinContent (1 + UnitTestAlg1::CB_CHANGE_INPUT_FIRST));
    ASSERT_LE (0, callbacks->GetBinContent (1 + UnitTestAlg1::CB_CHANGE_INPUT_OTHER));
    ASSERT_LE (1, callbacks->GetBinContent (1 + UnitTestAlg1::CB_INITIALIZE));
    ASSERT_LE (1, callbacks->GetBinContent (1 + UnitTestAlg1::CB_HIST_INITIALIZE));
    ASSERT_EQ (1000000, callbacks->GetBinContent (1 + UnitTestAlg1::CB_EXECUTE));
    ASSERT_EQ (100, callbacks->GetBinContent (1 + UnitTestAlg1::CB_FILE_EXECUTE));
    ASSERT_LE (1, callbacks->GetBinContent (1 + UnitTestAlg1::CB_FINALIZE));
    ASSERT_LE (1, callbacks->GetBinContent (1 + UnitTestAlg1::CB_HIST_FINALIZE));
  }



  TEST_P (UnitTestFixture, multi_fileExecuted)
  {
    checkFileExecuted ("multi");
  }



  TEST_P (UnitTestFixture, multi_out_empty)
  {
    SH::SampleHandler sh;
    sh.load (getJob() + "/output-out_empty");
    SH::Sample *sample = sh.get ("multi");
    ASSERT_TRUE (sample != nullptr);

    for (auto fileName : sample->makeFileList())
    {
      std::unique_ptr<TFile> file (TFile::Open (fileName.c_str(), "READ"));
    }
  }
}
