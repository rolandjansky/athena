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

#include <EventLoop/Driver.h>

#include <EventLoop/Job.h>
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

    std::string mylocation = location;
    if (location[0] != '/')
      mylocation = gSystem->WorkingDirectory () + ("/" + location);
    if (location.find ("/pnfs/") == 0)
      RCU_THROW_MSG ("can not place submit directory on pnfs: " + mylocation);
    if (job.options()->castBool (Job::optRemoveSubmitDir, false))
      gSystem->Exec (("rm -rf " + mylocation).c_str());
    if (gSystem->MakeDirectory (mylocation.c_str()) != 0)
      RCU_THROW_MSG ("could not create output directory " + mylocation);

    Job myjob = job;
    doUpdateJob (myjob, mylocation);

    if (job.options()->castBool (Job::optDisableMetrics, false))
      if (!myjob.algsHas (MetricsSvc::name))
     	myjob.algsAdd (new MetricsSvc);

    {
      TFile file ((mylocation + "/driver.root").c_str(), "RECREATE");
      file.WriteObject (this, "driver");
      file.Close ();
    }
    myjob.sampleHandler().save (mylocation + "/input");
    {
      std::ofstream file ((mylocation + "/location").c_str());
      file << mylocation << "\n";
    }

    SH::SampleHandler sh_hist;
    for (SH::SampleHandler::iterator sample = myjob.sampleHandler().begin(),
	   end = myjob.sampleHandler().end(); sample != end; ++ sample)
    {
      const std::string histfile
	= mylocation + "/hist-" + (*sample)->name() + ".root";
      std::auto_ptr<SH::SampleHist> hist
	(new SH::SampleHist ((*sample)->name(), histfile));
      hist->meta()->fetch (*(*sample)->meta());
      sh_hist.add (hist.release());
    }
    sh_hist.save (mylocation + "/hist");

    doSubmit (myjob, mylocation);

    // rationale: this particular file can be checked to see if a job
    //   has been submitted successfully.
    std::ofstream ((mylocation + "/submitted").c_str());
  }



  bool Driver ::
  retrieve (const std::string& location)
  {
    TFile file ((location + "/driver.root").c_str(), "READ");
    std::auto_ptr<Driver> driver (dynamic_cast<Driver*>(file.Get ("driver")));
    RCU_ASSERT2_SOFT (driver.get() != 0, "failed to read driver");

    return driver->doRetrieve (location);
  }



  bool Driver ::
  wait (const std::string& location, unsigned time)
  {
    // no invariant used

    struct SigTrap {
      static void handler (int) 
      { 
	EL::Driver::abortRetrieve = true; 
	std::cout << "\nAborting..." << std::endl;
      }
      SigTrap() { signal (SIGINT, &handler); }
      ~SigTrap() { signal (SIGINT, SIG_DFL); EL::Driver::abortRetrieve = false; }
    } sigTrap;

    while (!retrieve (location))
    {
      if (abortRetrieve) { return false; }
      std::cout << "not all worker jobs finished yet, waiting " << time << " seconds" << std::endl;
      for (unsigned i = 0; i != time; ++i) 
      {
	if (abortRetrieve) { return false; }
	sleep (1);
      }
      std::cout << "rechecking jobs" << std::endl;
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
  saveOutput (const std::string& location, const std::string& name,
	      TList& output)
  {
    try
    {
      TFile file ((location + "/hist-" + name + ".root").c_str(), "RECREATE");
      TIter iter (&output);
      TObject *object = 0;
      while ((object = iter.Next()))
      {
	TDirectory *dir = &file;
	std::string path = object->GetName();
	std::string name;
	std::string::size_type split = path.rfind ("/");
	if (split == std::string::npos)
	{
	  name = path;
	} else
	{
	  std::string dirname = path.substr (0, split);
	  name = path.substr (split + 1);
	  TNamed *named = dynamic_cast<TNamed*>(object);
	  if (named)
	    named->SetName (name.c_str());
	  dir = dynamic_cast<TDirectory*>(file.Get (dirname.c_str()));
	  if (!dir)
	  {
	    file.mkdir (dirname.c_str());
	    dir = dynamic_cast<TDirectory*>(file.Get (dirname.c_str()));
	  }
	  RCU_ASSERT (dir != 0);
	}

	if (!RCU::SetDirectory (object, dir))
	  dir->WriteObject (object, name.c_str());
      }
      file.Write ();
      output.Clear ("nodelete");
    } catch (...)
    {
      output.Clear ("nodelete");
      throw;
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
	std::auto_ptr<SH::SampleLocal> mysample
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
	std::auto_ptr<SH::SampleLocal> mysample
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



  void Driver ::
  doUpdateJob (Job& /*job*/, const std::string& /*location*/) const
  {
    RCU_READ_INVARIANT (this);
  }



  void Driver ::
  doSubmit (const Job& /*job*/, const std::string& /*location*/) const
  {
    RCU_READ_INVARIANT (this);

    RCU_THROW_MSG (std::string ("Driver::doSubmit not overridden in class ") + typeid(*this).name());
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
