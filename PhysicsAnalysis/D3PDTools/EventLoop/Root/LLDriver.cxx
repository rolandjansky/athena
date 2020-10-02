/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Morten Dam Joergensen 2012.
/// @author Nils Krumnack


//
// includes
//

#include <EventLoop/LLDriver.h>

#include <AsgMessaging/StatusCode.h>
#include <EventLoop/Job.h>
#include <EventLoop/ManagerData.h>
#include <EventLoop/MessageCheck.h>
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



  ::StatusCode LLDriver ::
  doManagerStep (Detail::ManagerData& data) const
  {
    RCU_READ_INVARIANT (this);
    using namespace msgEventLoop;
    ANA_CHECK (BatchDriver::doManagerStep (data));
    switch (data.step)
    {
    case Detail::ManagerStep::batchScriptVar:
      {
        data.batchName = "run{JOBID}.cmd";
        data.batchInit =
          "#\n"
          "# @ job_name       = EventLoopAnalysis{JOBID}\n"
          "# @ job_type       = serial\n"
          "# @ error          = $(Cluster).err\n"
          "# @ output         = $(Cluster).out\n"
          "# @ class          = " + queue + "\n"
          "# @ resources = ConsumableCpus(1) ConsumableMemory(4gb)\n"
          "# @ wall_clock_limit = 00:20:00\n"
          "# @ queue\n";
      }
      break;

    case Detail::ManagerStep::submitJob:
    case Detail::ManagerStep::doResubmit:
      {
        // safely ignoring: resubmit

        // Submit n jobs with loadleveler
        for (std::size_t iter : data.batchJobIndices)
        {
          // Submit!

          std::ostringstream cmd;
          cmd << "cd " << data.submitDir << "/submit && llsubmit "
              << data.options.castString (Job::optSubmitFlags)
              << " run"<<iter<<".cmd";

          if (gSystem->Exec (cmd.str().c_str()) != 0)
            RCU_THROW_MSG (("failed to execute: " + cmd.str()).c_str());
        }    
        data.submitted = true;
      }
      break;

    default:
      break;
    }
    return ::StatusCode::SUCCESS;
  }
}
