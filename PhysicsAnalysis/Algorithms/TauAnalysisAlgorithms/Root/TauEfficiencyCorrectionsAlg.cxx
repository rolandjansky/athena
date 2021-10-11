/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <TauAnalysisAlgorithms/TauEfficiencyCorrectionsAlg.h>

//
// method implementations
//

namespace CP
{
  TauEfficiencyCorrectionsAlg ::
  TauEfficiencyCorrectionsAlg (const std::string& name, 
                     ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_efficiencyCorrectionsTool ("TauAnalysisTools::TauEfficiencyCorrectionsTool", this)
  {
    declareProperty ("efficiencyCorrectionsTool", m_efficiencyCorrectionsTool, "the calibration and smearing tool we apply");
  }



  StatusCode TauEfficiencyCorrectionsAlg ::
  initialize ()
  {
    if (m_scaleFactorDecoration.empty())
    {
      ANA_MSG_ERROR ("no scale factor decoration name set");
      return StatusCode::FAILURE;
    }

    ANA_CHECK (m_efficiencyCorrectionsTool.retrieve());
    ANA_CHECK (m_tauHandle.initialize (m_systematicsList));
    ANA_CHECK (m_scaleFactorDecoration.initialize (m_systematicsList, m_tauHandle));
    ANA_CHECK (m_systematicsList.addSystematics (*m_efficiencyCorrectionsTool));
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_preselection.initialize());
    ANA_CHECK (m_outOfValidity.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode TauEfficiencyCorrectionsAlg ::
  execute ()
  {
    for (const auto& sys : m_systematicsList.systematicsVector())
    {
      ANA_CHECK (m_efficiencyCorrectionsTool->applySystematicVariation (sys));
      const xAOD::TauJetContainer *taus = nullptr;
      ANA_CHECK (m_tauHandle.retrieve (taus, sys));
      for (const xAOD::TauJet *tau : *taus)
      {
        if (m_preselection.getBool (*tau))
        {
          double sf = 0;
          ANA_CHECK_CORRECTION (m_outOfValidity, *tau, m_efficiencyCorrectionsTool->getEfficiencyScaleFactor (*tau, sf));
          m_scaleFactorDecoration.set (*tau, sf, sys);
        } else {
          m_scaleFactorDecoration.set (*tau, invalidScaleFactor(), sys);
        }
      }
    }
    return StatusCode::SUCCESS;
  }
}
