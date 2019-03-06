/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
    declareProperty ("efficiencyDecoration", m_efficiencyDecoration, "the decoration for the photon efficiency");
  }



  StatusCode PhotonEfficiencyCorrectionAlg ::
  initialize ()
  {
    if (m_efficiencyDecoration.empty())
    {
      ANA_MSG_ERROR ("no efficiency decoration name set");
      return StatusCode::FAILURE;
    }
    m_efficiencyAccessor = std::make_unique<SG::AuxElement::Accessor<float> > (m_efficiencyDecoration);

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
            double eff = 0;
            ANA_CHECK_CORRECTION (m_outOfValidity, *photon, m_efficiencyCorrectionTool->getEfficiencyScaleFactor (*photon, eff));
            (*m_efficiencyAccessor) (*photon) = eff;
          }
        }
        return StatusCode::SUCCESS;
      });
  }
}
