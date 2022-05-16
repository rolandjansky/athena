/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EgammaAnalysisAlgorithms/PhotonEfficiencyCorrectionAlg.h>

//
// method implementations
//

namespace CP
{
  PhotonEfficiencyCorrectionAlg ::
  PhotonEfficiencyCorrectionAlg (const std::string& name, 
                     ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_efficiencyCorrectionTool ("AsgPhotonEfficiencyCorrectionTool", this)
  {
    declareProperty ("efficiencyCorrectionTool", m_efficiencyCorrectionTool, "the efficiency correction tool we apply");
  }



  StatusCode PhotonEfficiencyCorrectionAlg ::
  initialize ()
  {
    if (m_scaleFactorDecoration.empty())
    {
      ANA_MSG_ERROR ("no scale factor decoration name set");
      return StatusCode::FAILURE;
    }

    ANA_CHECK (m_efficiencyCorrectionTool.retrieve());
    ANA_CHECK (m_photonHandle.initialize (m_systematicsList));
    ANA_CHECK (m_preselection.initialize (m_systematicsList, m_photonHandle, SG::AllowEmpty));
    ANA_CHECK (m_scaleFactorDecoration.initialize(m_systematicsList, m_photonHandle));
    ANA_CHECK (m_systematicsList.addSystematics (*m_efficiencyCorrectionTool));
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_outOfValidity.initialize());

    return StatusCode::SUCCESS;
  }



  StatusCode PhotonEfficiencyCorrectionAlg ::
  execute ()
  {
    for (const auto& sys : m_systematicsList.systematicsVector())
    {
      ANA_CHECK (m_efficiencyCorrectionTool->applySystematicVariation (sys));
      const xAOD::PhotonContainer *photons = nullptr;
      ANA_CHECK (m_photonHandle.retrieve (photons, sys));
      for (const xAOD::Photon *photon : *photons)
      {
        if (m_preselection.getBool (*photon, sys))
        {
          double sf = 0;
          ANA_CHECK_CORRECTION (m_outOfValidity, *photon, m_efficiencyCorrectionTool->getEfficiencyScaleFactor (*photon, sf));
          m_scaleFactorDecoration.set (*photon, sf, sys);
        } else {
          m_scaleFactorDecoration.set (*photon, invalidScaleFactor(), sys);
        }
      }
    }
    return StatusCode::SUCCESS;
  }
}
