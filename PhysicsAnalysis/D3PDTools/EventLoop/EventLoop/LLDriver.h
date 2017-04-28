/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_LL_DRIVER_HH
#define EVENT_LOOP_LL_DRIVER_HH

//          Copyright Nils Krumnack 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines a driver class for running on IBM LL batch
/// systems.  The interface provided in this class is intended for
/// experts only.  The module is considered to be in the pre-alpha
/// stage.

// LoadLeveler Driver contributed by Morten Dam Joergensen <mdj@nbi.dk> 2012

#include <EventLoop/Global.h>

#include <EventLoop/BatchDriver.h>
#include <SampleHandler/Global.h>

namespace EL
{
  class LLDriver : public BatchDriver
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
    LLDriver ();


    /// description: the queue we run on
  public:
    std::string queue;
    


    //
    // interface inherited from BatchDriver
    //

    /// returns: the name of the submission script to use.  if this
    ///   contains {JOBID} it will create one script for each job id
    /// guarantee: strong
    /// failures: out of memory II
    /// rationale: some batch systems are picky about names.  others
    ///   don't allow passing arguments into submission scripts
  private:
    virtual std::string batchName () const override;


    /// returns: any additional code we need for setting up the batch
    ///   job.  if multiple files are used, {JOBID} will be replaced
    ///   with the index of the current file.
    /// guarantee: strong
    /// failures: out of memory II
    /// rationale: some batch systems need extra lines when making
    ///   there submission scripts, which can either be specially
    ///   formatted option lines or just some special commands
  private:
    virtual std::string batchInit () const override;


    /// effects: perform the actual torque submission with njob jobs
    /// guarantee: strong
    /// failures: submission errors
    /// rationale: the virtual part of batch submission
  private:
    virtual void
    batchSubmit (const std::string& location, const SH::MetaObject& options,
		 std::size_t njob) const override;



    //
    // private interface
    //

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Winconsistent-missing-override"
    ClassDef(LLDriver, 1);
#pragma GCC diagnostic pop
  };
}

#endif
