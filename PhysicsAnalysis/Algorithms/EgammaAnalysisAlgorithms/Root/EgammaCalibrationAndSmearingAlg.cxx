/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EgammaAnalysisAlgorithms/EgammaCalibrationAndSmearingAlg.h>

//
// method implementations
//

namespace CP
{
  EgammaCalibrationAndSmearingAlg ::
  EgammaCalibrationAndSmearingAlg (const std::string& name, 
                     ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_calibrationAndSmearingTool ("CP::EgammaCalibrationAndSmearingTool", this)
  {
    declareProperty ("calibrationAndSmearingTool", m_calibrationAndSmearingTool, "the smearing tool we apply");
  }



  StatusCode EgammaCalibrationAndSmearingAlg ::
  initialize ()
  {
    ANA_CHECK (m_calibrationAndSmearingTool.retrieve());
    ANA_CHECK (m_egammaHandle.initialize (m_systematicsList));
    ANA_CHECK (m_systematicsList.addSystematics (*m_calibrationAndSmearingTool));
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_preselection.initialize());
    ANA_CHECK (m_outOfValidity.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode EgammaCalibrationAndSmearingAlg ::
  execute ()
  {
    for (const auto& sys : m_systematicsList.systematicsVector())
    {
      ANA_CHECK (m_calibrationAndSmearingTool->applySystematicVariation (sys));
      xAOD::EgammaContainer *egammas = nullptr;
      ANA_CHECK (m_egammaHandle.getCopy (egammas, sys));
      for (xAOD::Egamma *egamma : *egammas)
      {
        if (m_preselection.getBool (*egamma))
        {
          ANA_CHECK_CORRECTION (m_outOfValidity, *egamma, m_calibrationAndSmearingTool->applyCorrection (*egamma));
        }
      }
    }
    return StatusCode::SUCCESS;
  }
}
