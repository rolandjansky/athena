/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <TauAnalysisAlgorithms/DiTauSmearingAlg.h>

//
// method implementations
//

namespace CP
{
  DiTauSmearingAlg ::
  DiTauSmearingAlg (const std::string& name, 
                     ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_smearingTool ("TauAnalysisTools::CommonDiTauSmearingTool", this)
  {
    declareProperty ("smearingTool", m_smearingTool, "the calibration and smearing tool we apply");
  }



  StatusCode DiTauSmearingAlg ::
  initialize ()
  {
    ANA_CHECK (m_smearingTool.retrieve());
    m_systematicsList.addHandle (m_tauHandle);
    ANA_CHECK (m_systematicsList.addAffectingSystematics (m_smearingTool->affectingSystematics()));
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_outOfValidity.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode DiTauSmearingAlg ::
  execute ()
  {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
        ANA_CHECK (m_smearingTool->applySystematicVariation (sys));
        xAOD::DiTauJetContainer *taus = nullptr;
        ANA_CHECK (m_tauHandle.getCopy (taus, sys));
        for (xAOD::DiTauJet *tau : *taus)
        {
          ANA_CHECK_CORRECTION (m_outOfValidity, *tau, m_smearingTool->applyCorrection (*tau));
        }
        return StatusCode::SUCCESS;
      });
  }
}
