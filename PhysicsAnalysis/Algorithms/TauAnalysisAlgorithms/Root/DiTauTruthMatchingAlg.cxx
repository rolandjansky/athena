/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <TauAnalysisAlgorithms/DiTauTruthMatchingAlg.h>

//
// method implementations
//

namespace CP
{
  DiTauTruthMatchingAlg ::
  DiTauTruthMatchingAlg (const std::string& name, 
                     ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_matchingTool ("TauAnalysisTools::DiTauTruthMatchingTool", this)
  {
    declareProperty ("matchingTool", m_matchingTool, "the matching tool we apply");
  }



  StatusCode DiTauTruthMatchingAlg ::
  initialize ()
  {
    ANA_CHECK (m_matchingTool.retrieve());
    ANA_CHECK (m_tauHandle.initialize (m_systematicsList));
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_preselection.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode DiTauTruthMatchingAlg ::
  execute ()
  {
    for (const auto& sys : m_systematicsList.systematicsVector())
    {
      const xAOD::DiTauJetContainer *taus = nullptr;
      ANA_CHECK (m_tauHandle.retrieve (taus, sys));
      for (const xAOD::DiTauJet *tau : *taus)
      {
        if (m_preselection.getBool (*tau))
        {
          m_matchingTool->applyTruthMatch (*tau);
        }
      }
    }
    return StatusCode::SUCCESS;
  }
}
