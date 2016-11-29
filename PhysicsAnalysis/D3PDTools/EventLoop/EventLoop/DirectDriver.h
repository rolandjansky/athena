/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_DIRECT_DRIVER_HH
#define EVENT_LOOP_DIRECT_DRIVER_HH

//          Copyright Nils Krumnack 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines a driver for running directly in the job.  The
/// interface provided in this class is intended for experts only.
/// The module is considered to be in the pre-alpha stage.



#include <EventLoop/Global.h>

#include <EventLoop/Driver.h>

namespace EL
{
  class DirectDriver : public Driver
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
    DirectDriver ();



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



    //
    // private interface
    //

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Winconsistent-missing-override"
    ClassDef(DirectDriver, 1);
#pragma GCC diagnostic pop
  };
}

#endif
