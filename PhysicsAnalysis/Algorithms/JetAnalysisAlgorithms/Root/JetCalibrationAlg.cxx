/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <JetAnalysisAlgorithms/JetCalibrationAlg.h>

//
// method implementations
//

namespace CP
{
  JetCalibrationAlg ::
  JetCalibrationAlg (const std::string& name, 
                     ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_calibrationTool ("JetCalibrationTool", this)
  {
    declareProperty ("calibrationTool", m_calibrationTool, "the calibration tool we apply");
  }



  StatusCode JetCalibrationAlg ::
  initialize ()
  {
    ANA_CHECK (m_calibrationTool.retrieve());
    m_systematicsList.addHandle (m_jetHandle);
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_preselection.initialize());
    ANA_CHECK (m_outOfValidity.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode JetCalibrationAlg ::
  execute ()
  {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
        xAOD::JetContainer *jets = nullptr;
        ANA_CHECK (m_jetHandle.getCopy (jets, sys));
        for (xAOD::Jet *jet : *jets)
        {
          if (m_preselection.getBool (*jet))
          {
            ANA_CHECK_CORRECTION (m_outOfValidity, *jet, m_calibrationTool->applyCorrection (*jet));
          }
        }
        return StatusCode::SUCCESS;
      });
  }
}
