/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EventLoop/Driver.h>

#include <EventLoop/BaseManager.h>
#include <EventLoop/DriverManager.h>
#include <EventLoop/Job.h>
#include <EventLoop/ManagerData.h>
#include <EventLoop/ManagerOrder.h>
#include <EventLoop/ManagerStep.h>
#include <EventLoop/MessageCheck.h>
#include <EventLoop/MetricsSvc.h>
#include <EventLoop/OutputStream.h>
#include <EventLoop/RetrieveManager.h>
#include <EventLoop/SubmitDirManager.h>
#include <EventLoop/SubmitManager.h>
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

using namespace EL::msgEventLoop;

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



  std::string Driver ::
  submit (const Job& job, const std::string& location) const
  {
    // no invariant used

    std::string actualLocation = submitOnly (job, location);
    ANA_MSG_DEBUG ("wait on: " << actualLocation);
    wait (actualLocation);
    return actualLocation;
  }



  std::string Driver ::
  submitOnly (const Job& job, const std::string& location) const
  {
    RCU_READ_INVARIANT (this);

    Detail::ManagerData data;
    data.addManager (std::make_unique<Detail::BaseManager> ());
    data.addManager (std::make_unique<Detail::SubmitDirManager> ());
    data.addManager (std::make_unique<Detail::DriverManager> ());
    data.addManager (std::make_unique<Detail::SubmitManager> ());

    Job myjob = job;
    data.driver = this;
    data.submitDir = location;
    data.job = &myjob;
    if (data.run().isFailure())
      throw std::runtime_error ("failed to submit job");
    return data.submitDir;
  }



  void Driver ::
  resubmit (const std::string& location,
            const std::string& option)
  {
    Detail::ManagerData data;
    data.addManager (std::make_unique<Detail::BaseManager> ());
    data.addManager (std::make_unique<Detail::SubmitDirManager> ());
    data.addManager (std::make_unique<Detail::DriverManager> ());
    data.addManager (std::make_unique<Detail::SubmitManager> ());
    data.submitDir = location;

    std::unique_ptr<TFile> file (TFile::Open ((location + "/driver.root").c_str(), "READ"));
    std::unique_ptr<Driver> driver (dynamic_cast<Driver*>(file->Get ("driver")));
    RCU_ASSERT2_SOFT (driver.get() != 0, "failed to read driver");
    data.driver = driver.get();

    data.resubmit = true;
    data.resubmitOption = option;
    if (data.run().isFailure())
      throw std::runtime_error ("failed to resubmit job");
  }



  bool Driver ::
  retrieve (const std::string& location)
  {
    Detail::ManagerData data;
    data.addManager (std::make_unique<Detail::BaseManager> ());
    data.addManager (std::make_unique<Detail::SubmitDirManager> ());
    data.addManager (std::make_unique<Detail::DriverManager> ());
    data.addManager (std::make_unique<Detail::RetrieveManager> ());
    data.submitDir = location;

    std::unique_ptr<TFile> file (TFile::Open ((location + "/driver.root").c_str(), "READ"));
    if (!file || file->IsZombie())
      throw std::runtime_error ("failed to open driver file");
    std::unique_ptr<Driver> driver (dynamic_cast<Driver*>(file->Get ("driver")));
    RCU_ASSERT2_SOFT (driver.get() != 0, "failed to read driver");
    data.driver = driver.get();

    if (data.run().isFailure())
      throw std::runtime_error ("failed to retrieve job");
    return data.completed;
  }



  bool Driver ::
  wait (const std::string& location, unsigned time)
  {
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



  void Driver ::
  mergedOutputSave (Detail::ManagerData& data)
  {
    for (Job::outputIter out = data.job->outputBegin(),
	   end = data.job->outputEnd(); out != end; ++ out)
    {
      const std::string name
	= data.submitDir + "/data-" + out->label();

      SH::SampleHandler sh;
      for (SH::SampleHandler::iterator sample = data.job->sampleHandler().begin(),
	     end = data.job->sampleHandler().end(); sample != end; ++ sample)
      {
	const std::string name2 = name + "/" + (*sample)->name() + ".root";
	std::unique_ptr<SH::SampleLocal> mysample
	  (new SH::SampleLocal ((*sample)->name()));
	mysample->add (name2);
	sh.add (mysample.release());
      }
      sh.fetch (data.job->sampleHandler());
      sh.save (data.submitDir + "/output-" + out->label());
    }
  }



  void Driver ::
  diskOutputSave (Detail::ManagerData& data)
  {
    SH::SampleHandler sh_hist;
    sh_hist.load (data.submitDir + "/hist");

    for (Job::outputIter out = data.job->outputBegin(),
	   end = data.job->outputEnd(); out != end; ++ out)
    {
      SH::SampleHandler sh;
      for (SH::SampleHandler::iterator sample = data.job->sampleHandler().begin(),
	     end = data.job->sampleHandler().end(); sample != end; ++ sample)
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
      sh.fetch (data.job->sampleHandler());
      sh.save (data.submitDir + "/output-" + out->label());
    }
  }



  ::StatusCode Driver ::
  doManagerStep (Detail::ManagerData& /*data*/) const
  {
    return ::StatusCode::SUCCESS;
  }
}
