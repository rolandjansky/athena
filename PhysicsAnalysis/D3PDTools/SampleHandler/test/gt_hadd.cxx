/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgTesting/UnitTest.h>
#include <RootCoreUtils/Assert.h>
#include <SampleHandler/SampleHandler.h>
#include <SampleHandler/SampleLocal.h>
#include <SampleHandler/SamplePtr.h>
#include <SampleHandler/ToolsOther.h>
#include <TFile.h>
#include <TH1.h>

using namespace SH;
using namespace testing;

//
// unit test
//

struct MergeTest : public Test
{
  static SamplePtr getSample ()
  {
    static SamplePtr sample;
    if (sample.empty())
    {
      std::unique_ptr<SampleLocal> mysample (new SampleLocal ("test"));
      {
	mysample->add ("file1.root");
	TFile file ("file1.root", "RECREATE");
	TH1 *hist = new TH1F ("test", "test", 10, 0, 10);
	hist->SetBinContent (1, 1);
	file.Write ();
      }
      {
	mysample->add ("file2.root");
	TFile file ("file2.root", "RECREATE");
	TH1 *hist = new TH1F ("test", "test", 10, 0, 10);
	hist->SetBinContent (3, 1);
	file.Write ();
      }
      sample = SamplePtr (std::move (mysample));
    }
    return sample;
  }

  void checkMerger (const Sample& sample,
		    const std::string& fileName) const
  {
    auto files = sample.makeFileList();
    EXPECT_EQ (1u, files.size());
    EXPECT_TRUE (files.at(0).rfind ("/" + fileName) != std::string::npos);
    TFile file (fileName.c_str(), "READ");
    TH1 *hist = dynamic_cast<TH1*>(file.Get ("test"));
    ASSERT_TRUE (hist != nullptr);
    EXPECT_EQ (1, hist->GetBinContent (1));
    EXPECT_EQ (1, hist->GetBinContent (3));
  }
};

TEST_F (MergeTest, SLOW_sample)
{
  SamplePtr input = getSample ();
  std::unique_ptr<SH::SampleLocal> output
    = mergeFiles (*input, "output.root", false);
  checkMerger (*output, "output.root");
}

TEST_F (MergeTest, SLOW_sh)
{
  SampleHandler input;
  input.add (getSample ());
  SH::SampleHandler output
    = mergeFiles (input, "sh_output-", false);
  SamplePtr myoutput = output.get("test");
  ASSERT_TRUE (myoutput.get() != nullptr);
  checkMerger (*myoutput, "sh_output-test.root");
}

ATLAS_GOOGLE_TEST_MAIN
