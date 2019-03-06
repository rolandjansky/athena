/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
    declareProperty ("efficiencyDecoration", m_efficiencyDecoration, "the decoration for the tau efficiency");
  }



  StatusCode DiTauEfficiencyCorrectionsAlg ::
  initialize ()
  {
    if (m_efficiencyDecoration.empty())
    {
      ANA_MSG_ERROR ("no efficiency decoration name set");
      return StatusCode::FAILURE;
    }
    m_efficiencyAccessor = std::make_unique<SG::AuxElement::Accessor<float> > (m_efficiencyDecoration);

    ANA_CHECK (m_efficiencyCorrectionsTool.retrieve());
    m_systematicsList.addHandle (m_tauHandle);
    ANA_CHECK (m_systematicsList.addAffectingSystematics (m_efficiencyCorrectionsTool->affectingSystematics()));
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_preselection.initialize());
    ANA_CHECK (m_outOfValidity.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode DiTauEfficiencyCorrectionsAlg ::
  execute ()
  {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
        ANA_CHECK (m_efficiencyCorrectionsTool->applySystematicVariation (sys));
        xAOD::DiTauJetContainer *taus = nullptr;
        ANA_CHECK (m_tauHandle.getCopy (taus, sys));
        for (xAOD::DiTauJet *tau : *taus)
        {
          if (m_preselection.getBool (*tau))
          {
            double eff = 0;
            ANA_CHECK_CORRECTION (m_outOfValidity, *tau, m_efficiencyCorrectionsTool->getEfficiencyScaleFactor (*tau, eff));
            (*m_efficiencyAccessor) (*tau) = eff;
          }
        }
        return StatusCode::SUCCESS;
      });
  }
}
