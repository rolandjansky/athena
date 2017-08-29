/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_UNIT_TEST_ALG2_H
#define EVENT_LOOP_UNIT_TEST_ALG2_H

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

#include <AnaAlgorithm/AnaAlgorithm.h>

class TBranch;
class TH1;
class TTree;

namespace EL
{
  class UnitTestAlg2 final : public AnaAlgorithm
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
    UnitTestAlg2 (const std::string& name, 
                  ISvcLocator* pSvcLocator);


    /// description: whether I create an output n-tuple
  public:
    bool makeOutput;


    /// \brief the various callbacks we use
  public:
    enum CallBack
    {
      CB_INITIALIZE,
      CB_EXECUTE,
      CB_FINALIZE
    };



    //
    // interface inherited from Algorithm
    //

  private:
    virtual ::StatusCode initialize () override;

  private:
    virtual ::StatusCode execute () override;

  private:
    virtual ::StatusCode finalize () override;



    //
    // private interface
    //

    /// \brief the float property our owner sets
  public:
    float m_property = 0;

    /// \brief the string property our owner sets
  public:
    std::string m_string_property;

    /// description: the name of the variable we are using
  private:
    std::string m_name;

    /// description: the branch we are using
  private:
    TBranch *m_branch = nullptr;

    /// description: the value we are reading from the branch
  private:
    Int_t m_value;

    /// description: the histogram we are creating
  private:
    // TH1 *m_hist;

    /// description: the tree we are creating
  private:
    TTree *m_tree;

    /// \brief the name of the file (for testing changeInput)
  private:
    std::string m_fileName;

    /// \brief whether initialize has been called
  private:
    bool m_hasInitialize;

  private:
    unsigned m_calls = 0;

  private:
    TH1 *m_callbacks = nullptr;

  private:
    TH1 *getCallbacks ();
  };
}

#endif
