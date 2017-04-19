/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//          Copyright Nils Krumnack 2011, Morten Dam Joergensen 2012.
//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <EventLoop/LLDriver.h>

#include <EventLoop/Job.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <TSystem.h>
#include <sstream>

//
// method implementations
//

std::string convertInt(int number)
{
   std::stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}

ClassImp(EL::LLDriver)


namespace EL
{
  void LLDriver ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  LLDriver ::
  LLDriver ()
  {
    RCU_NEW_INVARIANT (this);
  }



  std::string LLDriver ::
  batchName () const
  {
    RCU_READ_INVARIANT (this);
    return "run{JOBID}.cmd";
  }



  std::string LLDriver ::
  batchInit () const
  {
    RCU_READ_INVARIANT (this);
    std::ostringstream result;
    result << "#\n";
    result << "# @ job_name       = EventLoopAnalysis{JOBID}\n";
    result << "# @ job_type       = serial\n";
    result << "# @ error          = $(Cluster).err\n";
    result << "# @ output         = $(Cluster).out\n";
    result << "# @ class          = " << queue << "\n";
    result << "# @ resources = ConsumableCpus(1) ConsumableMemory(4gb)\n";
    result << "# @ wall_clock_limit = 00:20:00\n";
    result << "# @ queue\n";
    return result.str();
  }



  void LLDriver ::
  batchSubmit (const std::string& location, const SH::MetaObject& options,
	       std::size_t njob) const
  {
    RCU_READ_INVARIANT (this);

    // Submit n jobs with loadleveler
    for (unsigned iter = 0, end = njob; iter != end; ++ iter) {
        
      // Submit!

      {
	std::ostringstream cmd;
	cmd << "cd " << location << "/submit && llsubmit "
	    << options.castString (Job::optSubmitFlags)
	    << " run"<<iter<<".cmd";

	if (gSystem->Exec (cmd.str().c_str()) != 0)
	  RCU_THROW_MSG (("failed to execute: " + cmd.str()).c_str());
      }
    }    
  }
}
