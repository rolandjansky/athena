/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MULTI_DRAW_FORMULA_SVC_H
#define MULTI_DRAW_FORMULA_SVC_H

//          Copyright Nils Krumnack 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines a class that manages a list of formulas and
/// their application to the data.  The interface provided in this
/// module is intended for experts only.  The module is considered to
/// be in the pre-alpha stage.



//protect
#include <MultiDraw/Global.h>

#include <EventLoop/Algorithm.h>

class TTree;

namespace MD
{
  /// returns: the debugging info of this object
  /// guarantee: strong
  /// failures: out of memory II
  std::string dbg (const FormulaSvc& obj, unsigned verbosity = 0);


  /// effects: register the formula service for this job
  /// guarantee: strong
  /// failures: out of memory I
  void useFormulas (EL::Job& job);


  /// returns: the formula service for this worker
  /// guarantee: strong
  /// failures: formula service not configured
  /// requires: worker != 0
  FormulaSvc *formulas (EL::IWorker *worker);



  class FormulaSvc : public EL::Algorithm
  {
    //
    // public interface
    //

    /// description: the name of the service
  public:
    static const std::string name;


    /// effects: test the invariant of this object
    /// guarantee: no-fail
  public:
    void testInvariant () const;


    /// effects: standard default constructor
    /// guarantee: no-fail
  public:
    FormulaSvc ();


    /// effects: standard destructor
    /// guarantee: no-fail
  public:
    ~FormulaSvc ();


    /// effects: add another formula
    /// returns: the formula
    /// guarantee: strong
    /// failures: out of memory II
    /// failures: not in initialization stage
    /// requires: !formula.empty()
  public:
    const Formula *addForm (const std::string& formula);



    //
    // methods inherited from EL::Algorithm
    //

    /// effects: return the name of this algorithm
    /// guarantee: no-fail
  private:
    virtual const char *GetName () const;


    /// effects: do all changes to work with a new input file,
    ///   e.g. set new branch addresses.  if firstFile is set, this
    ///   method is called just before init() is called
    /// guarantee: basic
    /// failures: algorithm dependent
  private:
    virtual EL::StatusCode changeInput (bool firstFile);


    /// effects: process the next event
    /// guarantee: basic
    /// failures: algorithm dependent
    /// rationale: the virtual part of Algorithm::process
  private:
    virtual EL::StatusCode execute ();



    //
    // private interface
    //

    /// description: the tree we are connected to
  private:
    TTree *m_tree; //!

    /// description: the list of variables used
  private:
    std::vector<Formula*> m_vars; //!

    ClassDef (FormulaSvc, 1);
  };
}

#endif
