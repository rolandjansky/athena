/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP__ANA_ALGORITHM_WRAPPER_H
#define EVENT_LOOP__ANA_ALGORITHM_WRAPPER_H

#include <EventLoop/Global.h>

#include <EventLoop/Algorithm.h>
#include <AnaAlgorithm/AnaAlgorithm.h>
#include <AnaAlgorithm/AnaAlgorithmConfig.h>

namespace EL
{
  /// \brief a class wrapping a \ref AnaAlgorithm in a \ref Algorithm
  ///
  /// This is mostly meant to be used internally to EventLoop to allow
  /// it to mix old and new style algorithms in a single job.

  class AnaAlgorithmWrapper : public Algorithm
  {
    //
    // public interface
    //

    /// \brief test the invariant of this object
    /// \par Guarantee
    ///   no-fail
  public:
    void testInvariant () const;


    /// \brief standard default constructor for streaming
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory I
  public:
    AnaAlgorithmWrapper () = default;


    /// \brief standard constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory I
  public:
    AnaAlgorithmWrapper (const AnaAlgorithmConfig& val_config);



    //
    // inherited interface
    //

  private:
    virtual StatusCode setupJob (Job& job);

  private:
    virtual StatusCode histInitialize ();

  private:
    virtual StatusCode initialize ();

  private:
    virtual StatusCode fileExecute ();

  private:
    virtual StatusCode changeInput (bool firstFile);

  private:
    virtual StatusCode execute ();

  private:
    virtual StatusCode histFinalize ();



    //
    // private interface
    //

    /// \brief the config object we use
  private:
    AnaAlgorithmConfig m_config;

    /// \brief the AnaAlgorithm (once we are running)
  private:
    std::unique_ptr<AnaAlgorithm> m_algorithm; //!

    ClassDef(AnaAlgorithmWrapper, 1);
  };
}

#endif
