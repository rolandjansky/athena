/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_UNIT_TEST_ALG_H
#define EVENT_LOOP_UNIT_TEST_ALG_H

//          Copyright Nils Krumnack 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines an algorithm used for unit tests.  The
/// interface provided in this module is intended for experts only.
/// The module is considered to be in the pre-alpha stage.



#include <EventLoopTest/Global.h>

#include <EventLoop/Algorithm.h>

class TBranch;
class TH1;
class TTree;

namespace EL
{
  class UnitTestAlg : public Algorithm
  {
    //
    // public interface
    //

    /// effects: test the invariant of this object
    /// guarantee: no-fail
  public:
    void testInvariant () const;


    /// effects: standard constructor.  initialize with a branch of
    ///   type Int_t
    /// guarantee: strong
    /// failures: low level errors II
  public:
    UnitTestAlg (const std::string& branchName = "el_n");


    /// description: whether I create an output n-tuple
  public:
    bool makeOutput;



    //
    // interface inherited from Algorithm
    //

    /// effects: give the algorithm a chance to intialize the job with
    ///   anything this algorithm needs.  this method is automatically
    ///   called before the algorithm is actually added to the job.
    /// guarantee: basic
    /// failures: algorithm dependent
    /// rationale: this is currently used to give algorithms a chance
    ///   to register their output datasets, but can also be used for
    ///   other stuff.
  private:
    virtual StatusCode setupJob (Job& job) override;


    /// effects: do all changes to work with a new input file,
    ///   e.g. set new branch addresses.  if firstFile is set, this
    ///   method is called just before init() is called
    /// guarantee: basic
    /// failures: algorithm dependent
  private:
    virtual StatusCode changeInput (bool firstFile) override;


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
    virtual StatusCode initialize () override;

  private:
    virtual StatusCode histInitialize () override;


    /// effects: process the next event
    /// guarantee: basic
    /// failures: algorithm dependent
    /// rationale: the virtual part of Algorithm::process
  private:
    virtual StatusCode execute () override;


    /// effects: do all the processing that needs to be done once per
    ///   file
    /// guarantee: basic
    /// failures: algorithm dependent
    /// rationale: this is to read per-file accounting data, e.g. the
    ///   list of lumi-blocks processed
  private:
    virtual StatusCode fileExecute () override;


    /// effects: do everything that needs to be done after completing
    ///   work on this worker
    /// guarantee: basic
    /// failures: algorithm dependent
    /// rationale: currently there is no use foreseen, but this
    ///   routine is provided regardless
    /// rationale: the virtual part of Algorithm::process
  private:
    virtual StatusCode finalize () override;


    /// effects: this is a post-initialization routine that is called
    ///   after finalize has been called.
    /// guarantee: basic
    /// failures: algorithm dependent
    /// rationale: unlike finalize(), this method is called all the
    ///   time, even on empty input files.
  private:
    virtual StatusCode histFinalize () override;



    //
    // private interface
    //

    /// description: the name of the variable we are using
  private:
    std::string m_name;

    /// description: the branch we are using
  private:
    TBranch *m_branch; //!

    /// description: the value we are reading from the branch
  private:
    Int_t m_value; //!

    /// description: the histogram we are creating
  private:
    // TH1 *m_hist; //!

    /// description: the tree we are creating
  private:
    TTree *m_tree; //!

    /// \brief the name of the file (for testing changeInput)
  private:
    std::string m_fileName;

    /// \brief whether initialize has been called
  private:
    bool m_hasInitialize;

    /// \brief whether histInitialize has been called
  private:
    bool m_hasHistInitialize;

  private:
    unsigned m_calls = 0; //!

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Winconsistent-missing-override"
    ClassDef(UnitTestAlg, 1);
#pragma GCC diagnostic pop
  };
}

#endif
