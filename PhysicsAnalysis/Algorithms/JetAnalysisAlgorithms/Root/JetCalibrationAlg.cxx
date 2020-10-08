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
    return StatusCode::SUCCESS;
  }



  StatusCode JetCalibrationAlg ::
  execute ()
  {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
        xAOD::JetContainer *jets = nullptr;
        ANA_CHECK (m_jetHandle.getCopy (jets, sys));
        ANA_CHECK (m_calibrationTool->applyCalibration(*jets));
        return StatusCode::SUCCESS;
      });
  }
}
