/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_DIRECT_WORKER_HH
#define EVENT_LOOP_DIRECT_WORKER_HH

//          Copyright Nils Krumnack 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines the Worker class for DirectDriver.  The
/// interface provided in this module is intended for experts only.
/// The module is considered to be in the pre-alpha stage.



#include <EventLoop/Global.h>

#include <string>
#include <EventLoop/Worker.h>
#include <SampleHandler/SamplePtr.h>

namespace EL
{
  class DirectWorker : public Worker
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
    /// requires: sample.get() != 0
    /// requires: output != 0
  public:
    DirectWorker (const SH::SamplePtr& sample,
                  const std::string& location);


    /// effects: run the job
    /// guarantee: strong
    /// failures: out of memory II
    /// failures: job failures
  public:
    void run (const SH::SamplePtr& sample, const Job& job);


    /// \brief run the job
    /// \par Guarantee
    ///   basic
  public:
    static void execute (const SH::SamplePtr& sample, const Job& job,
                         const std::string& location, const SH::MetaObject& options);



    //
    // interface inherited from Worker
    //



    //
    // private interface
    //

    /// description: the sample we are working on
  private:
    SH::SamplePtr m_sample;

    /// description: the location we store data at
  private:
    std::string m_location;
  };
}

#endif
