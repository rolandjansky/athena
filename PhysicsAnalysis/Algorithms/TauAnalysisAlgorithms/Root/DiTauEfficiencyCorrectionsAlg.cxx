/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <TauAnalysisAlgorithms/DiTauEfficiencyCorrectionsAlg.h>

//
// method implementations
//

namespace CP
{
  DiTauEfficiencyCorrectionsAlg ::
  DiTauEfficiencyCorrectionsAlg (const std::string& name, 
                     ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_efficiencyCorrectionsTool ("TauAnalysisTools::DiTauEfficiencyCorrectionsTool", this)
  {
    declareProperty ("efficiencyCorrectionsTool", m_efficiencyCorrectionsTool, "the calibration and smearing tool we apply");
  }



  StatusCode DiTauEfficiencyCorrectionsAlg ::
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



  StatusCode DiTauEfficiencyCorrectionsAlg ::
  execute ()
  {
    for (const auto& sys : m_systematicsList.systematicsVector())
    {
      ANA_CHECK (m_efficiencyCorrectionsTool->applySystematicVariation (sys));
      const xAOD::DiTauJetContainer *taus = nullptr;
      ANA_CHECK (m_tauHandle.retrieve (taus, sys));
      for (const xAOD::DiTauJet *tau : *taus)
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
