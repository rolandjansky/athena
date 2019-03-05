/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <SampleHandler/ToolsOther.h>

#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/hadd.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/MetaNames.h>
#include <SampleHandler/MetaObject.h>
#include <SampleHandler/SampleHandler.h>
#include <SampleHandler/SampleLocal.h>
#include <TFile.h>
#include <TRandom.h>
#include <TSystem.h>
#include <iostream>
#include <thread>
#include <chrono>

//
// method implementations
//

namespace SH
{
  std::unique_ptr<TFile>
  openFile (const std::string& name, const MetaObject& options)
  {
    const unsigned retries
      = options.castInteger (MetaNames::openRetries(),
			     MetaNames::openRetries_default());
    const double wait
      = options.castDouble (MetaNames::openRetriesWait(),
			    MetaNames::openRetriesWait_default());
    if (wait < 0)
      RCU_THROW_MSG ("negative values not allowed for: " + MetaNames::openRetriesWait());

    std::unique_ptr<TFile> result;

    for (unsigned tries = 0; tries <= retries; ++ tries)
    {
      if (tries > 0)
      {
	unsigned mywait = 1000 * gRandom->Uniform (wait/2, wait);
	std::cout << "open failed, waiting " << (mywait/1000.) << " seconds: "
		  << name << std::endl;
	std::this_thread::sleep_for (std::chrono::milliseconds (mywait));
      }
      result.reset (TFile::Open (name.c_str(), "READ"));
      if (result != nullptr)
	return result;
    }
    RCU_THROW_MSG ("failed to open file: " + name);
    throw; //compiler dummy
  }



  std::unique_ptr<SampleLocal>
  mergeFiles (const Sample& sample, const std::string& location,
	      bool overwrite)
  {
    std::unique_ptr<SampleLocal> result (new SampleLocal (sample.name()));
    result->add (location);
    *result->meta() = *sample.meta();
    if (overwrite || gSystem->AccessPathName (location.c_str()) != 0)
    {
      std::vector<std::string> files = sample.makeFileList();
      RCU::hadd (location, files);
    }
    return result;
  }



  SampleHandler
  mergeFiles (const SampleHandler& sh, const std::string& location,
	      bool overwrite)
  {
    SampleHandler result;
    for (auto& sample : sh)
    {
      std::string sublocation = location + sample->name() + ".root";
      result.add (mergeFiles (*sample, sublocation, overwrite));
    }
    return result;
  }
}
