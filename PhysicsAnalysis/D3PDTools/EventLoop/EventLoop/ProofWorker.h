/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_PROOF_WORKER_HH
#define EVENT_LOOP_PROOF_WORKER_HH

//          Copyright Nils Krumnack 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines the Worker class for ProofDriver.  The
/// interface provided in this module is intended for experts only.
/// The module is considered to be in the pre-alpha stage.



#include <EventLoop/Global.h>

#include <string>
#include <EventLoop/Worker.h>
#include <SampleHandler/SamplePtr.h>

namespace EL
{
  class ProofWorker : public Worker
  {
    //
    // public interface
    //

    /// effects: test the invariant of this object
    /// guarantee: no-fail
  public:
    void testInvariant () const;


    /// effects: standard constructor
    /// guarantee: strong
    /// failures: low level errors I
    /// requires: val_metaData != 0
    /// requires: output != 0
    /// requires: input != 0
    /// requires: selector != 0
  public:
    ProofWorker (const SH::MetaObject *val_metaData, TList *output,
		 ProofTSelector *selector);



    //
    // interface inherited from Worker
    //



    //
    // private interface
    //

    /// rationale: I split this into two classes, because TSelector
    ///   needs to be serializable and Worker is not.
    friend class ProofTSelector;

    /// description: the selector we use
  private:
    ProofTSelector *m_selector;
  };
}

#endif
