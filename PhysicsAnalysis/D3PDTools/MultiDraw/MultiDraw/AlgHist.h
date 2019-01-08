/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MULTI_DRAW_ALG_HIST_H
#define MULTI_DRAW_ALG_HIST_H

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines an "algorithm" that fills a TH1 object.  The
/// interface provided in this module is intended for experts only.
/// The module is considered to be in the pre-alpha stage.



//protect
#include <MultiDraw/Global.h>

#include <EventLoop/Algorithm.h>

class TH1;

namespace MD
{
  class AlgHist : public EL::Algorithm
  {
    //
    // public interface
    //

    /// effects: test the invariant of this object
    /// guarantee: no-fail
  public:
    void testInvariant () const;


    /// effects: default constructor
    /// guarantee: no-fail
    /// rationale: this should only be called by serialization
  public:
    AlgHist ();


    /// effects: constructor for the given histogram spec, value,
    ///   and weight
    /// guarantee: strong
    /// failures: out of memory II
    /// failures: insufficient values passed
    /// requires: val_hist_swallow != 0
  public:
    AlgHist (TH1 *val_hist_swallow,
	    const std::string& val_value0,
	    const std::string& val_value1 = "",
	    const std::string& val_value2 = "",
	    const std::string& val_value3 = "");


    /// effects: destructor
    /// guarantee: no-fail
    /// rationale: this is needed to delete the template histogram
  public:
    ~AlgHist ();


    //
    // interface inherited from EL::Algorithm
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
    virtual EL::StatusCode setupJob (EL::Job& job);


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
    virtual EL::StatusCode initialize ();


    /// effects: process the next event
    /// guarantee: basic
    /// failures: algorithm dependent
    /// rationale: the virtual part of Algorithm::process
  private:
    virtual EL::StatusCode execute ();



    //
    // private interface
    //

    /// description: the histogram we use as a template
  private:
    TH1 *m_hist;

    /// description: the type of histogram used
  private:
    int m_type;

    /// description: the list of formulas used
  private:
    std::vector<std::string> m_formulas;

    /// description: the value of the formulas
  private:
    std::vector<Double_t> m_values;

    /// description: the number of values without the weight/cut
  private:
    std::size_t m_valnum;

    /// description: whether we use a cut
  private:
    bool m_cut;

    /// description: the indices to the formulas
  private:
    std::vector<const Formula*> m_index;

    /// description: the histogram we are filling
  private:
    TH1 *m_hist2; //!

    /// description: the formula service we are using
  private:
    FormulaSvc *m_formSvc; //!

    ClassDef (AlgHist, 1);
  };
}

#endif
