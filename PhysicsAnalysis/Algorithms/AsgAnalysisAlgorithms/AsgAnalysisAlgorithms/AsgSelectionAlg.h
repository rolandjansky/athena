/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ASG_ANALYSIS_ALGORITHMS__ASG_SELECTION_ALG_H
#define ASG_ANALYSIS_ALGORITHMS__ASG_SELECTION_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <PATCore/IAsgSelectionTool.h>
#include <SelectionHelpers/ISelectionAccessor.h>
#include <SelectionHelpers/SelectionReadHandle.h>
#include <SystematicsHandles/SysCopyHandle.h>
#include <SystematicsHandles/SysListHandle.h>

namespace CP
{
  /// \brief an algorithm for calling \ref IAsgSelectionTool
  ///
  /// There are two ways to implement this: The way I chose is to call
  /// the IAsgSelectionTool on the members of an IParticleContainer.
  /// Among several issues this has some inefficiencies, because it
  /// relies on dynamic_cast inside the IAsgSelectionTool.  Also,
  /// currently \ref CopyHandle only supports specific xAOD types for
  /// IParticleContainer (though in practice this only matters if
  /// systematics are applied in this algorithm).
  ///
  /// The alternative would be to implement this as a template, with
  /// one template argument for the tool interface type and a second
  /// template argument for the container type to run on.  Besides
  /// improving some of the efficiences, it would also allow using
  /// selector tools that don't implement the \ref IAsgSelectionTool
  /// interface (though arguably they should support the
  /// IAsgSelectionTool interface).  The main downside of that
  /// approach would be the need to support a templated algorithm,
  /// which I'm not even sure we have dual-use support for at the
  /// moment (19 Mar 18).

  class AsgSelectionAlg final : public EL::AnaAlgorithm
  {
    /// \brief the standard constructor
  public:
    AsgSelectionAlg (const std::string& name, 
                                   ISvcLocator* pSvcLocator);


  public:
    StatusCode initialize () override;

  public:
    StatusCode execute () override;
    


    /// \brief the smearing tool
  private:
    ToolHandle<IAsgSelectionTool> m_selectionTool;

    /// \brief the smearing tool cast to an ISystematicsTool
    ///
    /// Normally selection tools don't have systematics, but I believe
    /// I have seen one or two with systematics, so I added that
    /// possibility in.
  private:
    ISystematicsTool *m_systematicsTool {nullptr};

    /// \brief the systematics list we run
  private:
    SysListHandle m_systematicsList {this};

    /// \brief the particle continer we run on
  private:
    SysCopyHandle<xAOD::IParticleContainer> m_particlesHandle {
      this, "particles", "", "the asg collection to run on"};

    /// \brief the preselection we apply to our input
  private:
    SelectionReadHandle m_preselection {
      this, "preselection", "", "the preselection to apply"};

    /// \brief the decoration for the asg selection
  private:
    std::string m_selectionDecoration;

    /// \brief the accessor for \ref m_selectionDecoration
  private:
    std::unique_ptr<ISelectionAccessor> m_selectionAccessor;

    /// \brief the bits to set for an object failing the preselection
  private:
    SelectionType m_setOnFail;
  };
}

#endif
