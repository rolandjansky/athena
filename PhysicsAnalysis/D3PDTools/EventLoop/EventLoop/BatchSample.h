/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_BATCH_SAMPLE_HH
#define EVENT_LOOP_BATCH_SAMPLE_HH

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

#include <TObject.h>
#include <SampleHandler/MetaObject.h>

namespace EL
{
  struct BatchSample : public TObject
  {
    //
    // public interface
    //

    /// effects: standard default constructor
    /// guarantee: no-fail
  public:
    BatchSample ();


    /// effects: standard destructor
    /// guarantee: no-fail
  public:
    ~BatchSample ();


    /// description: the names of the sample
  public:
    std::string name;


    /// description: the sample meta-information
  public:
    SH::MetaObject meta;


    /// description: the list of files we are reading
  public:
    std::vector<std::string> files;


    /// description: the beginning and end of the segments for this
    ///   sample
  public:
    UInt_t begin_segments, end_segments;



    //
    // private interface
    //

    ClassDef(BatchSample, 1);
  };
}

#endif
