/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef ELECTRON_ANALYSIS_ALGORITHMS__PHOTON_EFFICIENCY_SCALE_FACTOR_ALG_H
#define ELECTRON_ANALYSIS_ALGORITHMS__PHOTON_EFFICIENCY_SCALE_FACTOR_ALG_H

#include <xAODEgamma/PhotonContainer.h>
#include <AnaAlgorithm/AnaAlgorithm.h>
#include <EgammaAnalysisInterfaces/IAsgPhotonEfficiencyCorrectionTool.h>
#include <SelectionHelpers/OutOfValidityHelper.h>
#include <SelectionHelpers/SelectionReadHandle.h>
#include <SystematicsHandles/SysCopyHandle.h>
#include <SystematicsHandles/SysListHandle.h>
#include <SystematicsHandles/SysReadHandle.h>

namespace CP
{
  /// \brief an algorithm for calling \ref IPhotonEfficiencyCorrectionTool

  class PhotonEfficiencyCorrectionAlg final : public EL::AnaAlgorithm
  {
    /// \brief the standard constructor
  public:
    PhotonEfficiencyCorrectionAlg (const std::string& name, 
                                   ISvcLocator* pSvcLocator);


  public:
    StatusCode initialize () override;

  public:
    StatusCode execute () override;
    


    /// \brief the smearing tool
  private:
    ToolHandle<IAsgPhotonEfficiencyCorrectionTool> m_efficiencyCorrectionTool;

    /// \brief the systematics list we run
  private:
    SysListHandle m_systematicsList {this};

    /// \brief the photon collection we run on
  private:
    SysCopyHandle<xAOD::PhotonContainer> m_photonHandle {
      this, "photons", "Photons", "the photon collection to run on"};

    /// \brief the preselection we apply to our input
  private:
    SelectionReadHandle m_preselection {
      this, "preselection", "", "the preselection to apply"};

    /// \brief the helper for OutOfValidity results
  private:
    OutOfValidityHelper m_outOfValidity {this};

    /// \brief the decoration for the photon efficiency
  private:
    std::string m_efficiencyDecoration;

    /// \brief the accessor for \ref m_efficiencyDecoration
  private:
    std::unique_ptr<const SG::AuxElement::Accessor<float> > m_efficiencyAccessor;
  };
}

#endif
