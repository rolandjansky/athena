/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_PROOF_DRIVER_HH
#define EVENT_LOOP_PROOF_DRIVER_HH

//          Copyright Nils Krumnack 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines a driver for running using PROOF.  The
/// interface provided in this class is intended for experts only.
/// The module is considered to be in the pre-alpha stage.



#include <EventLoop/Global.h>

#include <EventLoop/Driver.h>

namespace EL
{
  class ProofDriver : public Driver
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
    ProofDriver ();


    /// description: whether we want to return the data files to the
    ///   submission node instead of creating a PROOF dataset.
    /// rationale: this is to be used for PROOF-lite running
  public:
    bool returnFiles;


    /// description: the name of the proof server, if we have one
  public:
    std::string proofMaster;


    /// description: the maximum number of workers to use
    /// rationale: for short jobs you may not want to use the entire
    ///   proof cluster, since setting it up can take some time
  public:
    unsigned numWorkers;


    /// description: the options to pass to make_par.sh
  public:
    std::string makeParOptions;


    /// effects: remove the current .par file from the PROOF master
    /// guarantee: basic
    /// failures: out of memory II
    /// failures: PROOF errors
    /// rationale: this allows to remove the package in case we have
    ///   made deep changes that mean incremental updates are not
    ///   possible
  public:
    void removePar () const;



    /// description: the parameters to pass to proof
  public:
    SH::MetaObject proofParm;


    /// description: the name of the query result object in the
    ///   histogram file
  public:
    static const std::string queryResultName;


    /// description: the option for the name of the par file
  public:
    static const std::string optionParFile;



    //
    // interface inherited from Driver
    //

    /// effects: submit the given job with the given output location
    /// guarantee: strong
    /// failures: out of memory II
    /// failures: submission errors
    /// rationale: the virtual part of EL::Driver::submit
  private:
    virtual void
    doSubmit (const Job& job, const std::string& location) const override;



    //
    // private interface
    //

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Winconsistent-missing-override"
    ClassDef(ProofDriver, 1);
#pragma GCC diagnostic pop
  };
}

#endif
