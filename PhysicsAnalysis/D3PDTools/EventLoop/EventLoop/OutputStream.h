/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_OUTPUT_STREAM_HH
#define EVENT_LOOP_OUTPUT_STREAM_HH

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines a class that describes everything that needs
/// to be known about an output dataset.  The interface provided in
/// this module is intended for the general user.  The module is
/// considered to be in the pre-alpha stage.



#include <EventLoop/Global.h>

#include <vector>
#include <TObject.h>
#include <SampleHandler/MetaObject.h>
#include <SampleHandler/SampleHandler.h>

namespace EL
{
  class OutputStream : public TObject
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
    OutputStream ();


    /// effects: create a named output dataset
    /// guarantee: strong
    /// failures: low level errors II
    /// requires: !val_label.empty()
  public:
    OutputStream (const std::string& val_label);


    /// effects: create a named output dataset
    /// guarantee: strong
    /// failures: low level errors II
    /// requires: !val_label.empty()
  public:
    OutputStream (const std::string& val_label, const std::string& val_type);


    /// effects: standard copy constructor
    /// guarantee: strong
    /// failures: out of memory II
  public:
    OutputStream (const OutputStream& that);


    /// effects: standard destructor
    /// guarantee: no-fail
  public:
    ~OutputStream ();


    /// effects: standard assignment operator
    /// guarantee: strong
    /// failures: out of memory II
  public:
    OutputStream& operator = (OutputStream that);


    /// effects: standard swap function
    /// guarantee: no-fail
  public:
    void swap (OutputStream& that);


    /// description: the label for this output dataset.
    /// invariant: !label.empty()
    /// rationale: this is mainly meant to allow for producing
    ///   multiple output datasets from a single job.
  public:
    const std::string& label () const;
    void label (const std::string& val_label);


    /// description: the list of options to the job
    /// guarantee: no-fail
    /// postcondition: result != 0
  public:
    SH::MetaObject *options ();
    const SH::MetaObject *options () const;


    /// description: the output disk, if we don't use the default one.
    /// guarantee: no-fail
    /// warning: this is not supported for all drivers
  public:
    const SH::DiskOutput *output() const;
    void output (SH::DiskOutput *output_swallow);


  public:
    static const std::string optType;
    static const std::string optMergeCmd;
    static const std::string optContainerSuffix;


    //
    // private interface
    //

    /// description: members directly corresponding to accessors
  private:
    std::string m_label;
  private:
    SH::MetaObject m_options;
  private:
    SH::DiskOutput *m_output;

    ClassDef (OutputStream, 2);
  };
}

#endif
