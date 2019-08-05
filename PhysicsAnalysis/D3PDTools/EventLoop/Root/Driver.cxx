/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EventLoop/Driver.h>

#include <EventLoop/Job.h>
#include <EventLoop/ManagerData.h>
#include <EventLoop/ManagerStep.h>
#include <EventLoop/MessageCheck.h>
#include <EventLoop/MetricsSvc.h>
#include <EventLoop/OutputStream.h>
#include <RootCoreUtils/RootUtils.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/DiskListLocal.h>
#include <SampleHandler/Sample.h>
#include <SampleHandler/SampleHist.h>
#include <SampleHandler/SampleLocal.h>
#include <TFile.h>
#include <TObjString.h>
#include <TSystem.h>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <memory>
#include <signal.h>

//
// method implementations
//

ClassImp (EL::Driver)

namespace EL
{
  bool EL::Driver::abortRetrieve(false); 



  void Driver ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  Driver ::
  Driver ()
  {
    RCU_NEW_INVARIANT (this);
  }



  SH::MetaObject *Driver ::
  options ()
  {
    RCU_READ_INVARIANT (this);
    return &m_options;
  }



  const SH::MetaObject *Driver ::
  options () const
  {
    RCU_READ_INVARIANT (this);
    return &m_options;
  }



  void Driver ::
  submit (const Job& job, const std::string& location) const
  {
    // no invariant used

    submitOnly (job, location);
    wait (location);
  }



  void Driver ::
  submitOnly (const Job& job, const std::string& location) const
  {
    RCU_READ_INVARIANT (this);
    using namespace msgEventLoop;

    Job myjob = job;
    Detail::ManagerData data;
    data.submitDir = location;
    data.job = &myjob;
    for (unsigned stepIter = unsigned (Detail::ManagerStep::initial);
         stepIter != unsigned (Detail::ManagerStep::final) + 1;
         stepIter += 1)
    {
      if (doManagerStep (data, Detail::ManagerStep (stepIter)).isFailure())
      {
        ANA_MSG_ERROR ("while performing submission step " << stepIter);
        throw std::runtime_error ("submission error in step " + std::to_string (stepIter));
      }
    }
  }



  void Driver ::
  resubmit (const std::string& location,
            const std::string& option)
  {
    std::unique_ptr<TFile> file (TFile::Open ((location + "/driver.root").c_str(), "READ"));
    std::unique_ptr<Driver> driver (dynamic_cast<Driver*>(file->Get ("driver")));
    RCU_ASSERT2_SOFT (driver.get() != 0, "failed to read driver");

    Detail::ManagerData data;
    data.submitDir = location;
    data.resubmit = true;
    data.resubmitOption = option;
    driver->doResubmit (data);
  }



  bool Driver ::
  retrieve (const std::string& location)
  {
    std::unique_ptr<TFile> file (TFile::Open ((location + "/driver.root").c_str(), "READ"));
    std::unique_ptr<Driver> driver (dynamic_cast<Driver*>(file->Get ("driver")));
    RCU_ASSERT2_SOFT (driver.get() != 0, "failed to read driver");

    return driver->doRetrieve (location);
  }



  bool Driver ::
  wait (const std::string& location, unsigned time)
  {
    using namespace msgEventLoop;

    // no invariant used

    struct SigTrap {
      static void handler (int) 
      { 
	EL::Driver::abortRetrieve = true; 
	ANA_MSG_INFO ("\nAborting...");
      }
      SigTrap() { signal (SIGINT, &handler); }
      ~SigTrap() { signal (SIGINT, SIG_DFL); EL::Driver::abortRetrieve = false; }
    } sigTrap;

    while (!retrieve (location))
    {
      if (abortRetrieve) { return false; }
      ANA_MSG_INFO ("not all worker jobs finished yet, waiting " << time << " seconds");
      for (unsigned i = 0; i != time; ++i) 
      {
	if (abortRetrieve) { return false; }
	sleep (1);
      }
      ANA_MSG_INFO ("rechecking jobs");
    }
    return true;
  }



  void Driver ::
  updateLocation (const std::string& location)
  {
    std::string from;
    {
      std::ifstream file ((location + "/location").c_str());
      if (!std::getline (file, from))
	RCU_THROW_MSG ("failed to read submit location from " + location + "/location");
    }
    std::string to = location;
    while (!to.empty() && to[to.size()-1] == '/')
      to = to.substr (0,to.size()-1);
    {
      SH::SampleHandler sh;
      sh.load (location + "/hist");
      sh.updateLocation (from, to);
      sh.save (location + "/hist");
    }
    SH::DiskListLocal list (location);
    while (list.next())
    {
      if (list.fileName().find ("output-") == 0)
      {
	SH::SampleHandler sh;
	sh.load (list.path());
	sh.updateLocation (from, to);
	sh.save (list.path());
      }
    }
    {
      std::ofstream file ((location + "/location").c_str());
      file << to << std::endl;
    }
  }



  std::string Driver ::
  mergedOutputName (const std::string& location, const OutputStream& output,
		    const std::string& sample)
  {
    return location + "/data-" + output.label() + "/" + sample + ".root";
  }



  void Driver ::
  mergedOutputMkdir (const std::string& location, const Job& job)
  {
    for (Job::outputIter out = job.outputBegin(), end = job.outputEnd();
	 out != end; ++ out)
    {
      const std::string dir = location + "/data-" + out->label();
      if (gSystem->MakeDirectory (dir.c_str()) != 0)
	RCU_THROW_MSG ("failed to create directory " + dir);
    }
  }



  void Driver ::
  mergedOutputSave (const std::string& location, const Job& job)
  {
    for (Job::outputIter out = job.outputBegin(),
	   end = job.outputEnd(); out != end; ++ out)
    {
      const std::string name
	= location + "/data-" + out->label();

      SH::SampleHandler sh;
      for (SH::SampleHandler::iterator sample = job.sampleHandler().begin(),
	     end = job.sampleHandler().end(); sample != end; ++ sample)
      {
	const std::string name2 = name + "/" + (*sample)->name() + ".root";
	std::unique_ptr<SH::SampleLocal> mysample
	  (new SH::SampleLocal ((*sample)->name()));
	mysample->add (name2);
	sh.add (mysample.release());
      }
      sh.fetch (job.sampleHandler());
      sh.save (location + "/output-" + out->label());
    }
  }



  void Driver ::
  diskOutputSave (const std::string& location, const Job& job)
  {
    SH::SampleHandler sh_hist;
    sh_hist.load (location + "/hist");

    for (Job::outputIter out = job.outputBegin(),
	   end = job.outputEnd(); out != end; ++ out)
    {
      SH::SampleHandler sh;
      for (SH::SampleHandler::iterator sample = job.sampleHandler().begin(),
	     end = job.sampleHandler().end(); sample != end; ++ sample)
      {
	SH::Sample *histSample = sh_hist.get ((*sample)->name());
	RCU_ASSERT (histSample != 0);
	std::unique_ptr<SH::SampleLocal> mysample
	  (new SH::SampleLocal ((*sample)->name()));
	TList *list = dynamic_cast<TList*>(histSample->readHist ("EventLoop_OutputStream_" + out->label()));
	if (list != 0)
	{
	  TObject *obj = 0;
	  for (TIter iter (list); (obj = iter.Next ()); )
	  {
	    TObjString *str = dynamic_cast<TObjString*>(obj);
	    RCU_ASSERT (str != 0);
	    mysample->add (str->GetString().Data());
	  }
	}
	mysample->meta()->fetch (*out->options());
	sh.add (mysample.release());
      }
      sh.fetch (job.sampleHandler());
      sh.save (location + "/output-" + out->label());
    }
  }



  ::StatusCode Driver ::
  doManagerStep (Detail::ManagerData& data,
                Detail::ManagerStep step) const
  {
    using namespace msgEventLoop;

    switch (step)
    {
    case Detail::ManagerStep::updateSubmitDir:
      {
        if (data.submitDir[0] != '/')
          data.submitDir = gSystem->WorkingDirectory () + ("/" + data.submitDir);
        if (data.submitDir.find ("/pnfs/") == 0)
        {
          ANA_MSG_ERROR ("can not place submit directory on pnfs: " + data.submitDir);
          return ::StatusCode::FAILURE;
        }
      }
      break;

    case Detail::ManagerStep::fillOptions:
      {
        data.options = *data.job->options();
        data.options.fetchDefaults (*options());
      }
      break;

    case Detail::ManagerStep::addSystemAlgs:
      {
        if (data.options.castBool (Job::optDisableMetrics, false))
          if (!data.job->algsHas (MetricsSvc::name))
            data.job->algsAdd (new MetricsSvc);
      }
      break;

    case Detail::ManagerStep::createSubmitDir:
      {
        if (data.options.castBool (Job::optRemoveSubmitDir, false))
          gSystem->Exec (("rm -rf " + data.submitDir).c_str());
        if (gSystem->MakeDirectory (data.submitDir.c_str()) != 0)
        {
          ANA_MSG_ERROR ("could not create output directory " + data.submitDir);
          return ::StatusCode::FAILURE;
        }
      }
      break;

    case Detail::ManagerStep::prepareSubmitDir:
      {
        {
          std::unique_ptr<TFile> file (TFile::Open ((data.submitDir + "/driver.root").c_str(), "RECREATE"));
          file->WriteObject (this, "driver");
          file->Close ();
        }
        data.job->sampleHandler().save (data.submitDir + "/input");
        {
          std::ofstream file ((data.submitDir + "/location").c_str());
          file << data.submitDir << "\n";
        }

        SH::SampleHandler sh_hist;
        for (SH::SampleHandler::iterator sample = data.job->sampleHandler().begin(),
               end = data.job->sampleHandler().end(); sample != end; ++ sample)
        {
          const std::string histfile
            = data.submitDir + "/hist-" + (*sample)->name() + ".root";
          std::unique_ptr<SH::SampleHist> hist
            (new SH::SampleHist ((*sample)->name(), histfile));
          hist->meta()->fetch (*(*sample)->meta());
          sh_hist.add (hist.release());
        }
        sh_hist.save (data.submitDir + "/hist");
      }
      break;

    case Detail::ManagerStep::submitJob:
      {
        ANA_MSG_INFO ("submitting job in " << data.submitDir);
      }
      break;

    case Detail::ManagerStep::postSubmit:
      {
        if (!data.submitted)
        {
          ANA_MSG_FATAL ("Driver::submit not implemented in class " << typeid(*this).name());
          std::abort ();
        }

        // this particular file can be checked to see if a job has
        // been submitted successfully.
        std::ofstream ((data.submitDir + "/submitted").c_str());
      }
      break;

    default:
      (void) true; // safe to do nothing
    }
    return ::StatusCode::SUCCESS;
  }



  void Driver ::
  doResubmit (Detail::ManagerData& /*info*/) const
  {
    RCU_READ_INVARIANT (this);
    RCU_THROW_MSG ("job resubmission not supported for this driver");
  }



  bool Driver ::
  doRetrieve (const std::string& location) const
  {
    RCU_READ_INVARIANT (this);

    if (gSystem->AccessPathName ((location + "/submitted").c_str()) != 0)
      RCU_THROW_MSG ("job submission was unsuccessful");
    return true;
  }
}
