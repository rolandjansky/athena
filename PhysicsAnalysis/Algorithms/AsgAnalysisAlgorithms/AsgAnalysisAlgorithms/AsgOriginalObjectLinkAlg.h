/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak


#ifndef ASG_ANALYSIS_ALGORITHMS__ASG_ORIGINAL_OBJECT_LINK_ALG_H
#define ASG_ANALYSIS_ALGORITHMS__ASG_ORIGINAL_OBJECT_LINK_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <SystematicsHandles/SysCopyHandle.h>
#include <SystematicsHandles/SysListHandle.h>
#include <xAODBase/IParticleContainer.h>


namespace CP
{
  /// \brief an algorithm for relinking a shallow copy with it's base container
  /// when this was not done originally
  ///
  /// In some cases we work with shallow copy containers that originally did
  /// not have proper original object linking done when created.
  /// Currently the client are b-tagging calibration shallow copies.

  class AsgOriginalObjectLinkAlg final : public EL::AnaAlgorithm
  {
    /// \brief the standard constructor
  public:
    AsgOriginalObjectLinkAlg (const std::string& name, 
                              ISvcLocator* pSvcLocator);


  public:
    StatusCode initialize () override;

  public:
    StatusCode execute () override;

    /// \brief base container name
  private:
    std::string m_baseContainerName {""};

    /// \brief the systematics list we run
  private:
    SysListHandle m_systematicsList {this};

    /// \brief the particle container we run on
  private:
    SysCopyHandle<xAOD::IParticleContainer> m_particleHandle {
      this, "particles", "", "the particle container to run on"};
  };
}

#endif
