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

#include <EventLoop/SlurmDriver.h>

#include <EventLoop/BatchJob.h>
#include <EventLoop/Job.h>
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
    m_b_partition = false;
    m_b_run_time = false;
    m_b_memory = false;
    m_b_constraint = false;

    RCU_NEW_INVARIANT (this);
  }
  //****************************************************
  std::string SlurmDriver :: batchInit () const
  {
    RCU_READ_INVARIANT (this);
    return "export PATH LD_LIBRARY_PATH PYTHONPATH";
  }
  //****************************************************
  void SlurmDriver :: batchSubmit (const std::string& location, const SH::MetaObject& options, std::size_t njob) const
  {
    auto all_set = m_b_job_name && m_b_account && m_b_partition && m_b_run_time && m_b_memory && m_b_constraint;
    if (!all_set)
    {
      std::cout << "Job Name" << m_job_name << std::endl;
      std::cout << "Account " << m_account << std::endl;
      std::cout << "Partition " << m_partition << std::endl;
      std::cout << "Run Time " << m_run_time << std::endl;
      std::cout << "Memory " << m_memory << std::endl;
      std::cout << "Constraint " << m_constraint << std::endl;

      RCU_THROW_MSG("All parameters need to be set before job can be submitted");
      return;
    }

    RCU_READ_INVARIANT (this);

    if(!options.castBool(Job::optBatchSharedFileSystem,true))
    {
      int status=gSystem->CopyFile("RootCore.par",(location+"/submit/RootCore.par").c_str());
      if(status != 0)
      RCU_THROW_MSG ("failed to copy RootCore.par");
    }

    {
      std::ofstream file ((location + "/submit/submit").c_str());

      file << "#!/bin/bash \n";
      file << "\n";
      file << "#SBATCH --job-name=" << m_job_name << "\n";
      file << "#SBATCH --output=slurm-%j.out\n";
      file << "#SBATCH --error=slurm-%j.err\n";
      file << "#SBATCH --account=" << m_account << "\n";
      file << "#SBATCH --partition=" << m_partition << "\n";
      file << "#SBATCH --time=" << m_run_time << "\n";
      file << "#SBATCH --mem=" << m_memory << "\n";
      file << "#SBATCH --constraint=" << m_constraint << "\n";
      file << "\n";
      file << options.castString(Job::optBatchSlurmExtraConfigLines) << "\n";
      file << "\n";
      //note: no "\n" at the of this string since this goes as pre-command to the execution of the next line
      file << options.castString(Job::optBatchSlurmWrapperExec); 
      file << "./run ${SLURM_ARRAY_TASK_ID}\n";
    }

    {
      std::ostringstream cmd;
      cmd << "cd " << location << "/submit && sbatch --array=0-" << njob-1 << " " << options.castString (Job::optSubmitFlags) << " submit";
      if (gSystem->Exec (cmd.str().c_str()) != 0)
      RCU_THROW_MSG (("failed to execute: " + cmd.str()).c_str());
    }
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
    m_b_partition = true;
    m_partition = partition;
  }
  void SlurmDriver :: SetRunTime(std::string run_time)
  {
    m_b_run_time = true;
    m_run_time = run_time;
  }
  void SlurmDriver :: SetMemory(std::string memory)
  {
    m_b_memory = true;
    m_memory = memory;
  }
  void SlurmDriver :: SetConstrain(std::string constraint)
  {
    m_b_constraint = true;
    m_constraint = constraint;
  }
  //****************************************************
}
