/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_BATCH_WORKER_HH
#define EVENT_LOOP_BATCH_WORKER_HH

//          Copyright Nils Krumnack 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines the Worker class for BatchDriver.  The
/// interface provided in this module is intended for experts only.
/// The module is considered to be in the pre-alpha stage.



#include <EventLoop/Global.h>

#include <string>
#include <EventLoop/Worker.h>
#include <SampleHandler/SamplePtr.h>

namespace EL
{
  class BatchWorker : public Worker
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
    /// requires: val_job != 0
    /// requires: val_sample != 0
    /// requires: val_segment != 0
  public:
    BatchWorker (const BatchJob *val_job,
		 const BatchSample *val_sample,
		 const BatchSegment *val_segment);


    /// effects: run the job
    /// guarantee: strong
    /// failures: out of memory II
    /// failures: job failures
  public:
    void run (const BatchJob *val_job);


    /// effects: do what is needed to execute the given job segment
    /// guarantee: basic
    /// failures: job specific
  public:
    static void execute (unsigned job_id, const char *confFile);



    //
    // interface inherited from Worker
    //



    //
    // private interface
    //

    /// description: constructor arguments
  private:
    const BatchJob *m_job;
  private:
    const BatchSample *m_sample;
  private:
    const BatchSegment *m_segment;

    /// description: the index of this subjob
  private:
    // unsigned m_index_old;
  };
}

#endif
