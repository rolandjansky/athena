/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_SLURM_DRIVER_HH
#define EVENT_LOOP_SLURM_DRIVER_HH

//          Copyright Nils Krumnack 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines a driver class for running on slurm batch
/// systems.  The interface provided in this class is intended for
/// experts only.  The module is considered to be in the pre-alpha
/// stage.



#include <EventLoop/Global.h>

#include <EventLoop/BatchDriver.h>
#include <SampleHandler/Global.h>

#include <iostream>

namespace EL
{
  class SlurmDriver : public BatchDriver
  {
    //
    // public interface
    //

    /// effects: test the invariant of this object
    /// guarantee: no-fail
  public:
    void testInvariant () const;


    /// effects: standard default constructor
    /// guarantee: strong
    /// failures: low level errors I
  public:
    SlurmDriver ();

    void SetJobName(std::string job_name);
    void SetAccount(std::string account);
    void SetPartition(std::string partition);
    void SetRunTime(std::string run_time);
    void SetMemory(std::string memory);
    void SetConstrain(std::string constraint);

    //
    // interface inherited from BatchDriver
    //

    /// effects: perform the actual slurm submission with njob jobs
    /// guarantee: strong
    /// failures: submission errors
    /// rationale: the virtual part of batch submission
  private:
    virtual void
    batchSubmit (const std::string& location, const SH::MetaObject& options,
 		 const std::vector<std::size_t>& jobIndices, bool resubmit)
      const override;

    /// effects: special initialization for slurm scripts: export PATH
    /// guarantee: strong
    /// failures: none
    /// rationale: slurm jobs do not export PATH, needed by RootCore setup scripts
  private:
    virtual std::string batchInit () const override;


    std::string m_job_name;
    std::string m_account;
    std::string m_partition;
    std::string m_run_time;
    std::string m_memory;
    std::string m_constraint;

    bool m_b_job_name;
    bool m_b_account;
    bool m_b_run_time;

    //
    // private interface
    //

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Winconsistent-missing-override"
    ClassDef(SlurmDriver, 1);
#pragma GCC diagnostic pop
  };
}

#endif
