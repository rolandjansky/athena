/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef EVENT_LOOP_SLURM_DRIVER_HH
#define EVENT_LOOP_SLURM_DRIVER_HH

#include <EventLoop/Global.h>

#include <EventLoop/BatchDriver.h>
#include <SampleHandler/Global.h>

#include <iostream>

namespace EL
{
  /// \brief a \ref Driver for running on SLURM batch systems

  class SlurmDriver final : public BatchDriver
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
    // interface inherited from BatchDriver
    //

  protected:
    virtual ::StatusCode
    doManagerStep (Detail::ManagerData& data) const override;

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
