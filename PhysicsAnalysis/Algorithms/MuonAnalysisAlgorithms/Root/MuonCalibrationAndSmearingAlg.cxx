/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <MuonAnalysisAlgorithms/MuonCalibrationAndSmearingAlg.h>

#include <MuonAnalysisInterfaces/IMuonCalibrationAndSmearingTool.h>
#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace CP
{
  MuonCalibrationAndSmearingAlg ::
  MuonCalibrationAndSmearingAlg (const std::string& name, 
                     ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_calibrationAndSmearingTool ("MuonCalibrationAndSmearingTool", this)
  {
    declareProperty ("calibrationAndSmearingTool", m_calibrationAndSmearingTool, "the calibration and smearing tool we apply");
  }



  StatusCode MuonCalibrationAndSmearingAlg ::
  initialize ()
  {
    ANA_CHECK (m_calibrationAndSmearingTool.retrieve());
    m_systematicsList.addHandle (m_muonHandle);
    ANA_CHECK (m_systematicsList.addAffectingSystematics (m_calibrationAndSmearingTool->affectingSystematics()));
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_outOfValidity.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode MuonCalibrationAndSmearingAlg ::
  execute ()
  {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
        ANA_CHECK (m_calibrationAndSmearingTool->applySystematicVariation (sys));
        xAOD::MuonContainer *muons = nullptr;
        ANA_CHECK (m_muonHandle.getCopy (muons, sys));
        for (xAOD::Muon *muon : *muons)
        {
          ANA_CHECK_CORRECTION (m_outOfValidity, *muon, m_calibrationAndSmearingTool->applyCorrection (*muon));
        }
        return StatusCode::SUCCESS;
      });
  }
}
