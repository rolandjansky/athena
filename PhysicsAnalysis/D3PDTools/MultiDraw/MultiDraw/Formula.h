/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MULTI_DRAW_FORMULA_H
#define MULTI_DRAW_FORMULA_H

//          - 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines a class that manages an individual formula.
/// The interface provided in this module is intended for experts
/// only.  The module is considered to be in the pre-alpha stage.



//protect
#include <MultiDraw/Global.h>

#include <string>
#include <Rtypes.h>

class TTree;
class TTreeFormula;
class TTreeFormulaManager;

namespace MD
{
  class Formula
  {
    //
    // public interface
    //

    /// effects: test the invariant of this object
    /// guarantee: no-fail
  public:
    void testInvariant () const;


    /// effects: standard default constructor
    /// guarantee: no-fail
  public:
    Formula ();


    /// effects: standard constructor
    /// guarantee: strong
    /// failures: out of memory II
  public:
    Formula (const std::string& name, const std::string& formula, TTree *tree);


    /// effects: standard destructor
    /// guarantee: no-fail
  public:
    ~Formula ();


    /// effects: reset this formula to a new tree
    /// guarantee: strong
    /// failures: out of memory II
    /// failures: TTreeFormula error
    /// requires: !formula().empty()
  public:
    void reset (TTree *tree);


    /// description: the formula used
    /// guarantee: no-fail
  public:
    const std::string& formula () const;


    /// returns: whether the formula is valid
    /// guarantee: no-fail
  public:
    bool valid () const;


    /// returns: the number of unspecified array dimensions for the
    ///   given formula
    /// guarantee: no-fail
    /// requires: valid()
  public:
    int ndim () const;


    /// returns: the number of data entries for the formula with the
    ///   given index
    /// guarantee: strong
    /// failures: TTreeFormula errors
    /// requires: valid()
  public:
    std::size_t ndata () const;


    /// returns: the result of evaluating the formula with the given
    ///   index for the given data entry
    /// guarantee: strong
    /// failures: TTreeFormula errors
    /// requires: valid()
    /// requires: data < getNData()
  public:
    Double_t value (std::size_t data) const;



    //
    // private interface
    //

    /// description: members directly corresponding to accessors
  private:
    std::string m_formula;

    /// description: the name we use for the formula
  private:
    std::string m_name;

    /// description: the tree we are connected to
  private:
    TTree *m_tree;

    /// description the formula used
  private:
    TTreeFormula *m_form;

    /// description: the manager used
  private:
    TTreeFormulaManager *m_manager;

    /// description: the number of array dimensions we need to loop
    ///   over, or -1 if we are in error
  private:
    int m_ndim;

    /// description: the last entry we read
  private:
    mutable Long64_t m_entry;

    /// description: the number of data entries
  private:
    mutable Int_t m_ndata;

    /// description: the cache of data entries
  private:
    mutable std::vector<Double_t> m_cache;

    /// description: whether we read the given data entry
  private:
    mutable std::vector<bool> m_read;


    /// rationale: to avoid broken objects
  private:
    Formula (const Formula& that);
    Formula& operator = (const Formula& that);
  };
}

#endif
