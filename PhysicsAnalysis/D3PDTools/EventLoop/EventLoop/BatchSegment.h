/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_BATCH_SEGMENT_H
#define EVENT_LOOP_BATCH_SEGMENT_H

//          Copyright Nils Krumnack 2012.
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

#include <TObject.h>

namespace EL
{
  struct BatchSegment : public TObject
  {
    //
    // public interface
    //

    /// effects: standard default constructor
    /// guarantee: no-fail
  public:
    BatchSegment ();


    /// effects: standard destructor
    /// guarantee: no-fail
  public:
    ~BatchSegment ();


    /// \brief the name of the sample for this segment
  public:
    std::string sampleName;


    /// \brief the name/id to use for this segment (including the sample name)
  public:
    std::string fullName;


    /// \brief the name/id to use for this segment (not
    /// including the sample name)
  public:
    std::string segmentName;


    /// description: the index of the sample we are using
  public:
    UInt_t sample;


    /// description: the job id of this segment
  public:
    UInt_t job_id;


    /// description: the starting and ending file and event number
  public:
    Long64_t begin_file, begin_event;
    Long64_t end_file, end_event;



    //
    // private interface
    //

    ClassDef(BatchSegment, 1);
  };
}

#endif
