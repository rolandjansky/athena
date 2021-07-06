/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ANA_ALGORITHM__I_ALGORITHM_WRAPPER_H
#define ANA_ALGORITHM__I_ALGORITHM_WRAPPER_H

#include <AnaAlgorithm/Global.h>

#include <AsgMessaging/StatusCode.h>
#include <memory>
#include <string_view>

namespace EL
{
  class Algorithm;

  /// \brief a wrapper around a generic algorithm for use within
  /// EventLoop
  ///
  /// Currently (11 Jan 21) we have two algorithm classes for use
  /// within EventLoop (`Algorithm` and `AnaAlgorithm`), and more
  /// algorithm classes (as well as other components like tools and
  /// services) are likely to come in the future.  Those need to be
  /// handled by EventLoop in a uniform manner, and to allow that each
  /// algorithm should be wrapped in an object that implements this
  /// interface and translates from this interface to the interface of
  /// the algorithm.
  ///
  /// This should generally be done as a wrapper (or otherwise as a
  /// private base class) to insulate the user from the existence of
  /// this interface and future changes to it.  Objects of this type
  /// should generally have a dictionary and allow full root
  /// serialization (writing to files and reading back) before
  /// `initialize` is called.
  ///
  /// \warn While users could develop their own algorithm class and
  /// wrap it in an object of this class, that is generally strongly
  /// discouraged.  And anybody who does this regardless of this
  /// warning should not expect this interface to remain unchanged
  /// from one version of EventLoop to the next.

  class IAlgorithmWrapper
  {
  public:

    /// \brief standard (virtual) destructor
    virtual ~IAlgorithmWrapper () noexcept = default;

    /// \brief get the name of this algorithm
    virtual std::string_view getName () const = 0;

    /// \brief whether this algorithm has the given name
    virtual bool hasName (const std::string& name) const = 0;

    /// \brief make a clone of this algorithm
    virtual std::unique_ptr<IAlgorithmWrapper> makeClone() const = 0;

    /// \brief get the legacy algorithm, if we wrap one
    virtual Algorithm *getLegacyAlg () {return nullptr;};

    /// \brief call initialize on the algorithm
    virtual StatusCode initialize (const AlgorithmWorkerData& workerData) = 0;

    /// \brief call execute on the algorithm
    virtual StatusCode execute () = 0;

    /// \brief call postExecute on the algorithm
    virtual StatusCode postExecute () {return StatusCode::SUCCESS;};

    /// \brief call finalize on the algorithm
    virtual StatusCode finalize () = 0;

    /// \brief call fileExecute on the algorithm
    virtual ::StatusCode fileExecute () = 0;

    /// \brief call beginInputFile on the algorithm
    virtual ::StatusCode beginInputFile () = 0;

    /// \brief call endInputFile on the algorithm
    virtual ::StatusCode endInputFile () = 0;
  };
}

#endif
