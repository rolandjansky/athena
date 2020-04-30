/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
    declareProperty ("efficiencyCorrectionTool", m_efficiencyCorrectionTool, "the calibration and smearing tool we apply");
    declareProperty ("scaleFactorDecoration", m_scaleFactorDecoration, "the decoration for the photon scale factor");
  }



  StatusCode PhotonEfficiencyCorrectionAlg ::
  initialize ()
  {
    if (m_scaleFactorDecoration.empty())
    {
      ANA_MSG_ERROR ("no scale factor decoration name set");
      return StatusCode::FAILURE;
    }
    m_scaleFactorAccessor = std::make_unique<SG::AuxElement::Accessor<float> > (m_scaleFactorDecoration);

    ANA_CHECK (m_efficiencyCorrectionTool.retrieve());
    m_systematicsList.addHandle (m_photonHandle);
    ANA_CHECK (m_systematicsList.addAffectingSystematics (m_efficiencyCorrectionTool->affectingSystematics()));
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_preselection.initialize());
    ANA_CHECK (m_outOfValidity.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode PhotonEfficiencyCorrectionAlg ::
  execute ()
  {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
        ANA_CHECK (m_efficiencyCorrectionTool->applySystematicVariation (sys));
        xAOD::PhotonContainer *photons = nullptr;
        ANA_CHECK (m_photonHandle.getCopy (photons, sys));
        for (xAOD::Photon *photon : *photons)
        {
          if (m_preselection.getBool (*photon))
          {
            double sf = 0;
            ANA_CHECK_CORRECTION (m_outOfValidity, *photon, m_efficiencyCorrectionTool->getEfficiencyScaleFactor (*photon, sf));
            (*m_scaleFactorAccessor) (*photon) = sf;
          }
        }
        return StatusCode::SUCCESS;
      });
  }
}
