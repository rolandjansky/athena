/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_ALGS__ALG_SELECT_H
#define EVENT_LOOP_ALGS__ALG_SELECT_H

//          - 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines an "algorithm" that fills a TH1 object.  The
/// interface provided in this module is intended for experts only.
/// The module is considered to be in the pre-alpha stage.



//protect
#include <EventLoopAlgs/Global.h>

#include <EventLoop/Algorithm.h>
#include <MultiDraw/Global.h>

class TH1;

namespace EL
{
  class AlgSelect : public Algorithm
  {
    //
    // public interface
    //

    /// effects: test the invariant of this object
    /// guarantee: no-fail
  public:
    void testInvariant () const;


    /// effects: create a new selector for the given output.  if a cut
    ///   it specified it will be the first cut in the list.
    /// guarantee: strong
    /// failures: out of memory II
    /// requires: !val_outputStream.empty()
  public:
    AlgSelect (const std::string& val_outputStream,
	       const std::string& cut = "");


    /// description: the name of the output stream
    /// guarantee: no-fail
    /// invariant: !outputStream.empty()
  public:
    const std::string& outputStream () const;


    /// description: the histogram name
    /// guarantee: no-fail / strong
    /// failures: out of memory II
  public:
    const std::string& histName () const;
    void histName (const std::string& val_histName);


    /// effects: add another cut
    /// guarantee: strong
    /// failures: out of memory II
    /// requires: !cut.empty()
  public:
    void addCut (const std::string& cut);



    //
    // semi-public interface
    //

    /// effects: standard default constructor
    /// guarantee: strong
    /// failures: out of memory II
    /// rationale: this should only be called by serialization
  public:
    AlgSelect ();



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
    virtual StatusCode setupJob (Job& job);


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

    /// description: members directly corresponding to accessors
  private:
    std::string m_outputStream;

    /// description: the list of formulas used
  private:
    std::vector<std::string> m_cuts;

    /// description: the name of the cut-flow histogram
  private:
    std::string m_histName;

    /// description: the indices to the formulas
  private:
    std::vector<const MD::Formula*> m_index; //!

    /// description: the histogram we are filling
  private:
    TH1 *m_hist; //!

    /// description: the formula service we are using
  private:
    MD::FormulaSvc *m_formSvc; //!

    /// description: the skimming service we are using
  private:
    NTupleSvc *m_skim; //!

    ClassDef (AlgSelect, 1);
  };
}

#endif
