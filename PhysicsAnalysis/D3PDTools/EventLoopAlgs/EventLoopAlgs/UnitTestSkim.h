/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_ALGS_UNIT_TEST_SKIM_H
#define EVENT_LOOP_ALGS_UNIT_TEST_SKIM_H

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines an algorithm used for unit tests of the
/// skimming algorithm.  The interface provided in this module is
/// intended for the experts only.  The module is considered to be in
/// the pre-alpha stage.



#include <EventLoopAlgs/Global.h>

#include <EventLoop/Algorithm.h>

namespace EL
{
  class UnitTestSkim : public Algorithm
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
    /// failures: out of memory I
  public:
    UnitTestSkim ();



    //
    // interface inherited from Algorithm
    //

    /// effects: do all changes to work with a new input file,
    ///   e.g. set new branch addresses.  if firstFile is set, this
    ///   method is called just before init() is called
    /// guarantee: basic
    /// failures: algorithm dependent
  private:
    virtual StatusCode changeInput (bool firstFile);


    /// effects: do everything that needs to be done before running
    ///   the algorithm, e.g. create output n-tuples and histograms.
    ///   this method is called only once right after
    ///   changeInput(true) is called
    /// guarantee: basic
    /// failures: algorithm dependent
    /// rationale: in principle all this work could be done on
    ///   changeInput(true).  However, providing this method should
    ///   make it easier for the user to set up all his outputs and to
    ///   do so only once.
  private:
    virtual StatusCode initialize ();


    /// effects: process the next event
    /// guarantee: basic
    /// failures: algorithm dependent
    /// rationale: the virtual part of Algorithm::process
  private:
    virtual StatusCode execute ();



    //
    // private interface
    //

    /// description: the output algorithm we use
  private:
    NTupleSvc *m_skim; //!

    /// description: the member we read into
  private:
    Int_t el_n; //!

    /// description: the member we write from
  private:
    Int_t el_n2; //!

    ClassDef(UnitTestSkim, 1);
  };
}

#endif
