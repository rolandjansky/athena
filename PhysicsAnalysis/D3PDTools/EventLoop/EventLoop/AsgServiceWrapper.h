/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EVENT_LOOP_ASG_SERVICE_WRAPPER_H
#define EVENT_LOOP_ASG_SERVICE_WRAPPER_H

#include <EventLoop/Global.h>

#include <AsgServices/AsgService.h>
#include <AsgServices/AsgServiceConfig.h>
#include <AnaAlgorithm/IAlgorithmWrapper.h>

namespace EL
{
  /// \brief an \ref IAlgorithmWrapper for \ref AsgService

  class AsgServiceWrapper final : public IAlgorithmWrapper
  {
  public:

    /// Public Members
    /// ==============

    /// \brief test the invariant of this object
    void testInvariant () const;

    /// \brief standard default constructor for serialization
    AsgServiceWrapper () {};

    /// \brief standard constructor
    AsgServiceWrapper (asg::AsgServiceConfig val_config);



    /// Inherited Members
    /// =================

    virtual std::string_view getName () const override;

    virtual bool hasName (const std::string& name) const override;

    virtual std::unique_ptr<IAlgorithmWrapper> makeClone() const override;

    virtual StatusCode initialize (const AlgorithmWorkerData& workerData) override;

    virtual StatusCode execute () override;

    virtual StatusCode finalize () override;

    virtual ::StatusCode fileExecute () override;

    virtual ::StatusCode beginInputFile () override;

    virtual ::StatusCode endInputFile () override;



    /// Private Members
    /// ===============

  private:

    /// \brief the algorithm configuration
    asg::AsgServiceConfig m_config;

    /// \brief the actual service (once instantiated)
    ///
    /// Please note (and do not remove) the comment at the end of the
    /// line.  This specifically excludes this member from
    /// serialization.
    std::shared_ptr<asg::AsgService> m_service; //!
  };
}

#endif
