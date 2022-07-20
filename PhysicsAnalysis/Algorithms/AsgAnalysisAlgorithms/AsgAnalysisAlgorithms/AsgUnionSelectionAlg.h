/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak

#ifndef ASG_ANALYSIS_ALGORITHMS__UNION_SELECTION_ALG_H
#define ASG_ANALYSIS_ALGORITHMS__UNION_SELECTION_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <SelectionHelpers/SysReadSelectionHandle.h>
#include <SelectionHelpers/ISelectionWriteAccessor.h>
#include <SystematicsHandles/SysReadHandle.h>
#include <SystematicsHandles/SysListHandle.h>
#include <xAODBase/IParticleContainer.h>


namespace CP
{
  /// \brief an algorithm for selection an union of objects that pass
  /// at least one systematics

  class AsgUnionSelectionAlg final : public EL::AnaAlgorithm
  {
    /// \brief the standard constructor
  public:
    AsgUnionSelectionAlg (const std::string& name,
                          ISvcLocator* pSvcLocator);


  public:
    virtual StatusCode initialize () override;

  public:
    virtual StatusCode execute () override;
    


    /// \brief the systematics list we run and have containers
  private:
    SysListHandle m_systematicsList {this};

    /// \brief the particle continer we run on
  private:
    SysReadHandle<xAOD::IParticleContainer> m_particlesHandle {
      this, "particles", "", "the asg collection to run on"};

    /// \brief the preselection we apply to our input
  private:
    SysReadSelectionHandle m_preselection {
      this, "preselection", "", "the preselection to apply"};

    /// \brief the decoration of the selection
  private:
    std::string m_selectionDecoration;

    /// \brief the accessor for \ref m_selectionDecoration
  private:
    std::unique_ptr<ISelectionWriteAccessor> m_selectionAccessor;
  };

} // namespace CP

#endif
