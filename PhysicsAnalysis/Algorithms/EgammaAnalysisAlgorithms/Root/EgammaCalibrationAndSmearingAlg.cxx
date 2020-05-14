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
    m_systematicsList.addHandle (m_egammaHandle);
    ANA_CHECK (m_systematicsList.addAffectingSystematics (m_calibrationAndSmearingTool->affectingSystematics()));
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_preselection.initialize());
    ANA_CHECK (m_outOfValidity.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode EgammaCalibrationAndSmearingAlg ::
  execute ()
  {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
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
        return StatusCode::SUCCESS;
      });
  }
}
