/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EgammaAnalysisAlgorithms/PhotonShowerShapeFudgeAlg.h>

//
// method implementations
//

namespace CP
{
  PhotonShowerShapeFudgeAlg ::
  PhotonShowerShapeFudgeAlg (const std::string& name, 
                     ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_showerShapeFudgeTool ("ElectronPhotonShowerShapeFudgeTool", this)
  {
    declareProperty ("showerShapeFudgeTool", m_showerShapeFudgeTool, "the smearing tool we apply");
  }



  StatusCode PhotonShowerShapeFudgeAlg ::
  initialize ()
  {
    ANA_CHECK (m_showerShapeFudgeTool.retrieve());
    m_systematicsList.addHandle (m_photonHandle);
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_preselection.initialize());
    ANA_CHECK (m_outOfValidity.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode PhotonShowerShapeFudgeAlg ::
  execute ()
  {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
        xAOD::PhotonContainer *photons = nullptr;
        ANA_CHECK (m_photonHandle.getCopy (photons, sys));
        for (xAOD::Photon *photon : *photons)
        {
          if (m_preselection.getBool (*photon))
          {
            ANA_CHECK_CORRECTION (m_outOfValidity, *photon, m_showerShapeFudgeTool->applyCorrection (*photon));
          }
        }
        return StatusCode::SUCCESS;
      });
  }
}
