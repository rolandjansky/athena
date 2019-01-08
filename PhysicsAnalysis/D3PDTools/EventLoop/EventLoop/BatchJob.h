/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_BATCH_JOB_HH
#define EVENT_LOOP_BATCH_JOB_HH

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines the arguments passed from the BATCH driver to
/// the BATCH worker.  The interface provided in this class is
/// intended for experts only.  The module is considered to be in the
/// pre-alpha stage.



#include <EventLoop/Global.h>

#include <vector>
#include <TObject.h>
#include <EventLoop/Job.h>
#include <SampleHandler/MetaObject.h>

class TChain;

namespace EL
{
  struct BatchJob : public TObject
  {
    //
    // public interface
    //

    /// effects: standard default constructor
    /// guarantee: no-fail
  public:
    BatchJob ();


    /// effects: standard destructor
    /// guarantee: no-fail
  public:
    ~BatchJob ();


    /// description: the job we are using
  public:
    Job job;


    /// description: the location of the submission directory, if it
    ///   is shared
    /// rationale: this allows to place the output files directly
    ///   where they belong, instead of copying them around
  public:
    std::string location;


    /// description: the number of jobs per sample
  public:
    std::vector<unsigned> njobs_old;


    /// description: the list of samples
  public:
    std::vector<BatchSample> samples;

    /// description: the list of segments
  public:
    std::vector<BatchSegment> segments;



    //
    // private interface
    //

    ClassDef(BatchJob, 1);
  };
}

#endif
