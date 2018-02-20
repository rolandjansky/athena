/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_BATCH_DRIVER_HH
#define EVENT_LOOP_BATCH_DRIVER_HH

//          Copyright Nils Krumnack 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines a driver base class for running on batch
/// systems.  The interface provided in this class is intended for
/// experts only.  The module is considered to be in the pre-alpha
/// stage.



#include <EventLoop/Global.h>

#include <EventLoop/Driver.h>
#include <SampleHandler/Global.h>

namespace EL
{
  class BatchDriver : public Driver
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
    BatchDriver ();


    /// description: these shell commands are run verbatim on each
    ///   worker node before execution
  public:
    std::string shellInit;



    //
    // interface inherited from Driver
    //

    /// effects: update the job before it is submitted
    /// guarantee: basic
    /// failures: out of memory II
    /// failures: job specifications unfulfillable
  private:
    virtual void
    doUpdateJob (Job& job, const std::string& location) const override;


    /// effects: submit the given job with the given output location
    /// guarantee: strong
    /// failures: out of memory II
    /// failures: submission errors
    /// rationale: the virtual part of EL::Driver::submit
  private:
    virtual void
    doSubmit (const Job& job, const std::string& location) const override;


    /// effects: retrieve all the output for the job in the given
    ///   location
    /// returns: whether the job completed successfully
    /// guarantee: basic, may partially retrieve
    /// failures: out of memory III
    /// failures: job failures
    /// failures: job can't be read
    /// failures: job was made with different driver
    /// rationale: while job failures will cause this method to fail
    ///   you can typically retry it multiple times if you can use
    ///   partial results.
    /// rationale: the virtual part of EL::Driver::retrieve
  private:
    virtual bool
    doRetrieve (const std::string& location) const override;



    //
    // virtual interface
    //

    /// returns: the name of the submission script to use.  if this
    ///   contains {JOBID} it will create one script for each job id
    /// guarantee: strong
    /// failures: out of memory II
    /// rationale: some batch systems are picky about names.  others
    ///   don't allow passing arguments into submission scripts
  private:
    virtual std::string batchName () const;


    /// returns: any additional code we need for setting up the batch
    ///   job.  if multiple files are used, {JOBID} will be replaced
    ///   with the index of the current file.
    /// guarantee: strong
    /// failures: out of memory II
    /// rationale: some batch systems need extra lines when making
    ///   there submission scripts, which can either be specially
    ///   formatted option lines or just some special commands
  private:
    virtual std::string batchInit () const;


    /// returns: the code needed for setting EL_JOBID
    /// guarantee: strong
    /// failures: out of memory II
    /// rationale: normally one can just pass the index of the job as
    ///   the first argument to the execution script, but some systems
    ///   instead use environment variables
    /// rationale: this is not used if we have separate execution
    ///   scripts from the job
  private:
    virtual std::string batchJobId () const;


    /// \brief the code for setting up the release
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II\n
    ///   failed to read environment variables
  private:
    virtual std::string batchReleaseSetup (bool sharedFileSystem) const;


    /// effects: perform the actual batch submission with njob jobs
    /// guarantee: strong
    /// failures: submission errors
    /// rationale: the virtual part of batch submission
  private:
    virtual void
    batchSubmit (const std::string& location, const SH::MetaObject& options,
 		 std::size_t njob) const = 0;



    //
    // private interface
    //

    /// effects: create the run script to be used
    /// guarantee: basic, may create a partial script
    /// failures: out of memory II
    /// failures: i/o errors
  private:
    void makeScript (const std::string& location, std::size_t njobs, bool sharedFileSystem) const;


    /// effects: merge the fetched histograms
    /// returns: wether all histograms have been fetched
    /// guarantee: strong
    /// failures: out of memory II
    /// failures: i/o errors
  private:
    static bool mergeHists (const std::string& location, const BatchJob& config);

    /// effects: determine location for writing output
    /// returns: path to directory for writing
    /// guarantee: strong
  private:
    const std::string getWriteLocation(const std::string& location) const;

    /// effects: determine location with input configuration
    /// returns: path to directory with input configuration
    /// guarantee: strong
  private:
    const std::string getSubmitLocation(const std::string& location) const;

#ifndef USE_CMAKE
    /// effects: determine location of rootcorebin on node
    /// returns: path to directory of rootcorebin on node
    /// guarantee: strong
  private:
    const std::string getRootCoreBin() const;
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Winconsistent-missing-override"
    ClassDef(BatchDriver, 1);
#pragma GCC diagnostic pop
  };
}

#endif
