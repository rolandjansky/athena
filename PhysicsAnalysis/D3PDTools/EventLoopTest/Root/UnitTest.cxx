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

#include <EventLoopTest/UnitTest.h>

#include <EventLoop/Driver.h>
#include <EventLoop/Job.h>
#include <EventLoop/OutputStream.h>
#include <EventLoopTest/UnitTestAlg.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/MetaFields.h>
#include <SampleHandler/MetaObject.h>
#include <SampleHandler/SampleGrid.h>
#include <SampleHandler/SampleLocal.h>
#include <SampleHandler/SamplePtr.h>
#include <SampleHandler/ToolsSplit.h>
#include <TFile.h>
#include <TH1.h>
#include <TSystem.h>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>

//
// method implementations
//

namespace EL
{
  UnitTest ::
  UnitTest (const std::string& val_name, std::string val_base_path)
    : name (val_name), base_path (val_base_path), cleanup (true), testOutput (true), outputDisk (0),
      testFileExecute (true)
  {
    if (base_path.empty())
      base_path = "$ROOTCOREBIN/data/EventLoop/";
  }



  int UnitTest ::
  run (const Driver& driver) const
  {
    std::vector<std::string> files;
    std::vector<TH1*> histos;

    for (unsigned iter = 0, end = 3; iter != end; ++ iter)
    {
      std::ostringstream path;
      path << base_path << "test_ntuple" << iter << ".root";
      TString input = path.str();
      gSystem->ExpandPathName (input);
      files.push_back (input.Data());
      std::unique_ptr<TFile> file (TFile::Open (input.Data(), "READ"));
      RCU_ASSERT_SOFT (file.get() != 0);
      TH1 *hist = dynamic_cast<TH1*>(file->Get ("hist_n"));
      RCU_ASSERT_SOFT (hist != 0);
      histos.push_back(hist);
      hist->SetDirectory (0);
    };
    std::string tree ("physics");


    SH::SampleHandler samples;

    std::vector<std::unique_ptr<SH::Sample> > mysamples;
    if (gridInput)
    {
      std::unique_ptr<SH::SampleGrid> mysample;
      mysample.reset (new SH::SampleGrid ("dataset0"));
      mysample->meta()->setString (SH::MetaFields::gridName, "user.krumnack:user.krumnack.EventLoopTest.2019-03-25.dataset0");
      mysample->meta()->setString (SH::MetaFields::gridFilter, SH::MetaFields::gridFilter_default);
      mysamples.push_back (std::move (mysample));
      mysample.reset (new SH::SampleGrid ("dataset1"));
      mysample->meta()->setString (SH::MetaFields::gridName, "user.krumnack:user.krumnack.EventLoopTest.2019-03-25.dataset1");
      mysample->meta()->setString (SH::MetaFields::gridFilter, SH::MetaFields::gridFilter_default);
      mysamples.push_back (std::move (mysample));
    } else
    {
      std::unique_ptr<SH::SampleLocal> mysample;
      mysample.reset (new SH::SampleLocal ("dataset0"));
      mysample->add (files[0]);
      mysamples.push_back (std::move (mysample));
      mysample.reset (new SH::SampleLocal ("dataset1"));
      mysample->add (files[1]);
      mysample->add (files[2]);
      mysamples.push_back (std::move (mysample));
    }
    {
      TH1 *hist = 0;
      hist = dynamic_cast<TH1*>(histos[0]->Clone ("hist"));
      mysamples[0]->meta()->addReplace (hist);

      hist = dynamic_cast<TH1*>(histos[1]->Clone ("hist"));
      hist->Add (histos[2]);
      mysamples[1]->meta()->addReplace (hist);
    }
    for (auto& mysample : mysamples)
      samples.add (mysample.release());

    samples.setMetaString (SH::MetaFields::treeName, tree);

    for (unsigned iter = 0, end = histos.size(); iter != end; ++ iter)
      delete histos[iter];

    RCU_ASSERT (samples.size() > 0);

    if (scanNEvents)
      SH::scanNEvents (samples);

    TString output = "/tmp";
    if (location.empty())
    {
      {
	const char *tmpdir = getenv ("TMPDIR");
	if (tmpdir)
	  output = tmpdir;
      }
      output += "/EventLoop-" + name + ".$$";
    } else output = location;
    gSystem->ExpandPathName (output);

    try
    {
      UnitTestAlg alg;
      alg.makeOutput = testOutput;

      EL::Job job;
      // job.options()->setDouble (Job::optD3PDPerfStats, 1);
      job.options()->setDouble ("jobOpt", 42);
      // job.options()->setDouble (Job::optPerfTree, 1);
      // job.options()->setDouble (EL::Job::optCacheSize, 10*1024*1024);
      // job.options()->setDouble (Job::optPrintPerFileStats, 1);
      {
	SH::SampleHandler sh = samples;
	sh.setMetaString ("mymeta", "test");
	job.sampleHandler (sh);
      }
      job.algsAdd (new UnitTestAlg (alg));
      bool outputDone = outputDisk == 0;
      for (Job::outputMIter outputStream = job.outputBegin(),
	     end = job.outputEnd(); outputStream != end; ++ outputStream)
      {
	if (!outputDone)
	{
	  outputStream->output (outputDisk);
	  outputDone = true;
	}
      }

      if (!cleanup)
	RCU_PRINT_MSG ("placing temporary files in: " + output);
      gSystem->Exec (("rm -rf " + output).Data());
      driver.submit (job, output.Data());

      for (std::size_t iter = 0, end = samples.size(); iter != end; ++ iter)
      {
	TH1 *ref_hist = dynamic_cast<TH1*>(samples[iter]->meta()->get ("hist"));
	if (ref_hist != 0)
	{
	  SH::SampleHandler sh;
	  sh.load ((output + "/hist").Data());
	  SH::SamplePtr sample = sh.get (samples[iter]->name());
	  if (sample.empty())
	    RCU_THROW_MSG ("could not find histogram sample " + samples[iter]->name());

	  if (testFileExecute)
	  {
	    TH1 *file_executes = dynamic_cast<TH1*>(sample->readHist ("file_executes"));
	    RCU_ASSERT_SOFT (file_executes != 0);
	    RCU_ASSERT_SOFT (file_executes->GetEntries() == samples[iter]->numFiles());
	  }

	  TH1 *hist = dynamic_cast<TH1*>(sample->readHist ("el_n"));
	  if (samples[iter]->getNumEntries() == 0)
	  {
	    RCU_ASSERT_SOFT (hist == 0);
	  } else
	  {
	    RCU_ASSERT_SOFT (dynamic_cast<TH1*>(sample->readHist ("el_n2")));
	    RCU_ASSERT_SOFT (dynamic_cast<TList*>(sample->readHist ("alpha")));
	    RCU_ASSERT_SOFT (sample->readHist ("beta/dir/hist"));
	    if (hist == 0)
	      RCU_THROW_MSG ("didn't find histogram el_n in sample " + sample->name());
	    if (hist->GetNbinsX() != ref_hist->GetNbinsX())
	      RCU_THROW_MSG ("bin missmatch between histograms");
	    for (int bin = 0, end = hist->GetNbinsX()+2; bin != end; ++ bin)
	    {
	      if (hist->GetBinContent (bin) != ref_hist->GetBinContent (bin))
	      {
		std::ostringstream str;
		str << "bin content missmatch in bin " << bin
		    << " found " << hist->GetBinContent (bin)
		    << " expected " << ref_hist->GetBinContent (bin);
		RCU_THROW_MSG (str.str());
	      }
	    }
	  }
	  TH1 *count = dynamic_cast<TH1*>(sample->readHist ("EventLoop_EventCount"));
	  if (samples[iter]->getNumEntries() > 0 && count == 0)
	    RCU_THROW_MSG ("didn't find histogram EventLoop_EventCount");
	}
	if (testOutput && samples[iter]->getNumEntries() > 0)
	{
	  SH::SampleHandler sh;
	  sh.load ((output + "/output-out").Data());
	  SH::Sample *const sample = sh.get (samples[iter]->name());
	  if (!sample)
	    RCU_THROW_MSG ("output dataset not found for " + samples[iter]->name());
	  sample->meta()->setString (SH::MetaFields::treeName, "tree");
	  if (ref_hist && samples[iter]->getNumEntries() != ref_hist->GetEntries())
	  {
	    std::ostringstream str;
	    str << "tree entries missmatch found " << samples[iter]->getNumEntries()
		<< " expected " << ref_hist->GetEntries();
	    RCU_THROW_MSG (str.str());
	  }
	}
      }
      if (cleanup)
	gSystem->Exec (("rm -rf " + output).Data());
      return EXIT_SUCCESS;
    } catch (std::exception& e)
    {
      std::cout << "exception caught in unit test: " << e.what() << std::endl;
      if (cleanup)
	gSystem->Exec (("rm -rf " + output).Data());
      return EXIT_FAILURE;
    } catch (std::string& e)
    {
      std::cout << "exception caught in unit test: " << e << std::endl;
      if (cleanup)
	gSystem->Exec (("rm -rf " + output).Data());
      return EXIT_FAILURE;
    } catch (...)
    {
      std::cout << "unknown exception caught in unit test" << std::endl;
      if (cleanup)
	gSystem->Exec (("rm -rf " + output).Data());
      return EXIT_FAILURE;
    }
  }
}
