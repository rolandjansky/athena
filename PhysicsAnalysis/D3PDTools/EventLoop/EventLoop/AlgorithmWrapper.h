/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP__ALGORITHM_WRAPPER_H
#define EVENT_LOOP__ALGORITHM_WRAPPER_H

#include <EventLoop/Global.h>

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <AnaAlgorithm/AnaAlgorithmConfig.h>
#include <AnaAlgorithm/IAlgorithmWrapper.h>

namespace EL
{
  /// \brief an \ref IAlgorithmWrapper for \ref AnaAlgorithm

  class AlgorithmWrapper final : public IAlgorithmWrapper
  {
  public:

    /// Public Members
    /// ==============

    /// \brief test the invariant of this object
    void testInvariant () const;

    /// \brief standard default constructor for serialization
    AlgorithmWrapper () {};

    /// \brief standard constructor
    AlgorithmWrapper (std::unique_ptr<Algorithm>&& val_algorithm);



    /// Inherited Members
    /// =================

    virtual std::string_view getName () const override;

    virtual bool hasName (const std::string& name) const override;

    virtual std::unique_ptr<IAlgorithmWrapper> makeClone() const override;

    virtual Algorithm *getLegacyAlg () override;

    virtual StatusCode initialize (const AlgorithmWorkerData& workerData) override;

    virtual StatusCode execute () override;

    virtual StatusCode postExecute () override;

    virtual StatusCode finalize () override;

    virtual ::StatusCode fileExecute () override;

    virtual ::StatusCode beginInputFile () override;

    virtual ::StatusCode endInputFile () override;



    /// Private Members
    /// ===============

  private:

    /// \brief the actual algorithm
    std::unique_ptr<Algorithm> m_algorithm;

    /// \brief whether this is the first file we encounter
    bool m_firstFile {true};

    /// \brief whether \ref Algorithm::initialize has been called
    bool m_isInitialized {false};
  };
}

#endif
