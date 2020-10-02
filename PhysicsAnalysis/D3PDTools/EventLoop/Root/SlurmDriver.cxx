/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <EventLoop/SlurmDriver.h>

#include <AsgMessaging/StatusCode.h>
#include <EventLoop/BatchJob.h>
#include <EventLoop/Job.h>
#include <EventLoop/ManagerData.h>
#include <EventLoop/MessageCheck.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <TSystem.h>
#include <fstream>
#include <memory>
#include <sstream>

//
// method implementations
//

ClassImp(EL::SlurmDriver)

namespace EL
{
  //****************************************************
  void SlurmDriver :: testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }
  //****************************************************
  SlurmDriver :: SlurmDriver ()
  {
    m_b_job_name = false;
    m_b_account = false;
    m_b_run_time = false;

    RCU_NEW_INVARIANT (this);
  }
  //****************************************************
  ::StatusCode SlurmDriver ::
  doManagerStep (Detail::ManagerData& data) const
  {
    RCU_READ_INVARIANT (this);
    using namespace msgEventLoop;
    ANA_CHECK (BatchDriver::doManagerStep (data));
    switch (data.step)
    {
    case Detail::ManagerStep::batchScriptVar:
      {
        data.batchInit = "export PATH LD_LIBRARY_PATH PYTHONPATH";
      }
      break;

    case Detail::ManagerStep::submitJob:
    case Detail::ManagerStep::doResubmit:
      {
        auto all_set = m_b_job_name && m_b_account && m_b_run_time;
        if (!all_set)
        {
          ANA_MSG_INFO ("Job Name" << m_job_name);
          ANA_MSG_INFO ("Account " << m_account);
          ANA_MSG_INFO ("Run Time " << m_run_time);

          ANA_MSG_ERROR("All parameters need to be set before job can be submitted");
          return ::StatusCode::FAILURE;
        }

        RCU_READ_INVARIANT (this);

        if (data.resubmit)
          RCU_THROW_MSG ("resubmission not supported for this driver");

        assert (!data.batchJobIndices.empty());
        assert (data.batchJobIndices.back() + 1 == data.batchJobIndices.size());
        const std::size_t njob = data.batchJobIndices.size();

        if(!data.options.castBool(Job::optBatchSharedFileSystem,true))
        {
          int status=gSystem->CopyFile("RootCore.par",(data.submitDir+"/submit/RootCore.par").c_str());
          if(status != 0)
            RCU_THROW_MSG ("failed to copy RootCore.par");
        }

        {
          std::ofstream file ((data.submitDir + "/submit/submit").c_str());

          file << "#!/bin/bash \n";
          file << "\n";
          file << "#SBATCH --job-name=" << m_job_name << "\n";
          file << "#SBATCH --output=slurm-%j.out\n";
          file << "#SBATCH --error=slurm-%j.err\n";
          file << "#SBATCH --account=" << m_account << "\n";
          if(!m_partition .empty()) file << "#SBATCH --partition=" << m_partition << "\n";
          file << "#SBATCH --time=" << m_run_time << "\n";
          if(!m_memory    .empty()) file << "#SBATCH --mem=" << m_memory << "\n";
          if(!m_constraint.empty()) file << "#SBATCH --constraint=" << m_constraint << "\n";
          file << "\n";
          file << data.options.castString(Job::optBatchSlurmExtraConfigLines) << "\n";
          file << "\n";
          //note: no "\n" at the of this string since this goes as pre-command to the execution of the next line
          file << data.options.castString(Job::optBatchSlurmWrapperExec);
          file << "./run ${SLURM_ARRAY_TASK_ID}\n";
        }

        {
          std::ostringstream cmd;
          cmd << "cd " << data.submitDir << "/submit && sbatch --array=0-" << njob-1 << " " << data.options.castString (Job::optSubmitFlags) << " submit";
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

  //****************************************************
  void SlurmDriver :: SetJobName(std::string job_name)
  {
    m_b_job_name = true;
    m_job_name = job_name;
  }
  void SlurmDriver :: SetAccount(std::string account)
  {
    m_b_account = true;
    m_account = account;
  }
  void SlurmDriver :: SetPartition(std::string partition)
  {
    m_partition = partition;
  }
  void SlurmDriver :: SetRunTime(std::string run_time)
  {
    m_b_run_time = true;
    m_run_time = run_time;
  }
  void SlurmDriver :: SetMemory(std::string memory)
  {
    m_memory = memory;
  }
  void SlurmDriver :: SetConstrain(std::string constraint)
  {
    m_constraint = constraint;
  }
  //****************************************************
}
