/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_LOOP_BACKGROUND_TSELECTOR_H
#define EVENT_LOOP_BACKGROUND_TSELECTOR_H

//          Copyright Nils Krumnack 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.



#include <EventLoop/Global.h>

#include <TSelector.h>
#include <memory>

class TProofOutputFile;

namespace EL
{
  /// \brief a TSelector running the worker in a background process
  ///
  /// The main purpose of this is to allow running xAOD inputs on a
  /// full PROOF farm with the ProofDriver.

  class BackgroundTSelector : public TSelector
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
    BackgroundTSelector ();


    /// effects: standard destructor
    /// guarantee: no-fail
  public:
    ~BackgroundTSelector ();


    /// description: the interface inherited from TSelector
  public:
    // virtual void	Begin(TTree*) override;
    virtual void	SlaveBegin(TTree*) override;
    virtual void	Init(TTree*) override;
    virtual Bool_t	Notify() override;
    virtual Bool_t	Process(Long64_t) override;
    virtual void	SlaveTerminate() override;
    // virtual void	Terminate() override;
    virtual int	Version() const override;



    //
    // private interface
    //

    /// description: the list of arguments to the submission
  private:
    ProofArgs *m_args; //!

    /// \brief the socket we use for the background thread
  private:
    std::auto_ptr<BackgroundSocket> m_socket; //!

    /// \brief the current tree
  private:
    TTree *m_tree; //!

    /// description: whether we are current on the input
  private:
    bool m_currentInput; //!

    /// description: the output files we are using
  private:
    std::vector<TProofOutputFile*> m_outputFiles; //!

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Winconsistent-missing-override"
    ClassDef (BackgroundTSelector, 1);
#pragma GCC diagnostic pop
  };
}

#endif
