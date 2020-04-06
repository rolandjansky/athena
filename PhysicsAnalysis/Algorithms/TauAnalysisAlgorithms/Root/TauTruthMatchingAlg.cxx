/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <TauAnalysisAlgorithms/TauTruthMatchingAlg.h>

//
// method implementations
//

namespace CP
{
  TauTruthMatchingAlg ::
  TauTruthMatchingAlg (const std::string& name, 
                     ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_matchingTool ("TauAnalysisTools::TauTruthMatchingTool", this)
  {
    declareProperty ("matchingTool", m_matchingTool, "the matching tool we apply");
  }



  StatusCode TauTruthMatchingAlg ::
  initialize ()
  {
    ANA_CHECK (m_matchingTool.retrieve());
    m_systematicsList.addHandle (m_tauHandle);
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_preselection.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode TauTruthMatchingAlg ::
  execute ()
  {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
        xAOD::TauJetContainer *taus = nullptr;
        ANA_CHECK (m_tauHandle.getCopy (taus, sys));
        for (xAOD::TauJet *tau : *taus)
        {
          if (m_preselection.getBool (*tau))
          {
            m_matchingTool->applyTruthMatch (*tau);
          }
        }
        return StatusCode::SUCCESS;
      });
  }
}
