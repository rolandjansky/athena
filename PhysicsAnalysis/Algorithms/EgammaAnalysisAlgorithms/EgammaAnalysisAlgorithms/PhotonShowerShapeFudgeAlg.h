/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EGAMMA_ANALYSIS_ALGORITHMS__ELECTRON_PHOTON_SHOWER_SHAPE_FUDGE_ALG_H
#define EGAMMA_ANALYSIS_ALGORITHMS__ELECTRON_PHOTON_SHOWER_SHAPE_FUDGE_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <EgammaAnalysisInterfaces/IElectronPhotonShowerShapeFudgeTool.h>
#include <EgammaAnalysisAlgorithms/CopyHelpers.h>
#include <SelectionHelpers/OutOfValidityHelper.h>
#include <SelectionHelpers/SelectionReadHandle.h>
#include <SystematicsHandles/SysCopyHandle.h>
#include <SystematicsHandles/SysListHandle.h>
#include <xAODEgamma/PhotonContainer.h>

namespace CP
{
  /// \brief an algorithm for calling \ref
  /// IElectronPhotonShowerShapeFudgeTool for photons

  class PhotonShowerShapeFudgeAlg final : public EL::AnaAlgorithm
  {
    /// \brief the standard constructor
  public:
    PhotonShowerShapeFudgeAlg (const std::string& name, 
                                     ISvcLocator* pSvcLocator);


  public:
    StatusCode initialize () override;

  public:
    StatusCode execute () override;
    


    /// \brief the smearing tool
  private:
    ToolHandle<IElectronPhotonShowerShapeFudgeTool> m_showerShapeFudgeTool;

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
  };
}

#endif
