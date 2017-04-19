/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_PROOF_ARGS_HH
#define EVENT_LOOP_PROOF_ARGS_HH

//          Copyright Nils Krumnack 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines the arguments passed from the PROOF driver to
/// the PROOF worker.  The interface provided in this class is
/// intended for experts only.  The module is considered to be in the
/// pre-alpha stage.



#include <EventLoop/Global.h>

#include <vector>
#include <TObject.h>
#include <SampleHandler/MetaObject.h>

namespace EL
{
  struct ProofArgs : public TObject
  {
    //
    // public interface
    //

    /// effects: standard default constructor
    /// guarantee: no-fail
  public:
    ProofArgs ();


    /// effects: standard destructor
    /// guarantee: no-fail
  public:
    ~ProofArgs ();


    /// description: a copy of the PROOF driver we are using
  public:
    ProofDriver *driver;


    /// description: the list of algorithms we are using
  public:
    std::vector<Algorithm*> algs;


    /// description: the list of output streams we are using
  public:
    std::vector<OutputStream> output;


    /// description: the sample name
  public:
    std::string sample_name;


    /// description: the sample meta-information
  public:
    SH::MetaObject sample_meta;


    /// description: the directory where we put our output
    /// rationale: this is used for directly writing the output files
    ///   in PROOF lite
  public:
    std::string output_dir;



    //
    // private interface
    //

    ClassDef(ProofArgs, 1);
  };
}

#endif
