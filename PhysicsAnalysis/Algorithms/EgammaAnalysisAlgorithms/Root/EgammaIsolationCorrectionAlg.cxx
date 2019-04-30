/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EgammaAnalysisAlgorithms/EgammaIsolationCorrectionAlg.h>

//
// method implementations
//

namespace CP
{
  EgammaIsolationCorrectionAlg ::
  EgammaIsolationCorrectionAlg (const std::string& name, 
                     ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_isolationCorrectionTool ("CP::IsolationCorrectionTool", this)
  {
    declareProperty ("isolationCorrectionTool", m_isolationCorrectionTool, "the smearing tool we apply");
  }



  StatusCode EgammaIsolationCorrectionAlg ::
  initialize ()
  {
    ANA_CHECK (m_isolationCorrectionTool.retrieve());
    m_systematicsList.addHandle (m_egammaHandle);
    ANA_CHECK (m_systematicsList.addAffectingSystematics (m_isolationCorrectionTool->affectingSystematics()));
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_preselection.initialize());
    ANA_CHECK (m_outOfValidity.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode EgammaIsolationCorrectionAlg ::
  execute ()
  {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
        ANA_CHECK (m_isolationCorrectionTool->applySystematicVariation (sys));
        xAOD::EgammaContainer *egammas = nullptr;
        ANA_CHECK (m_egammaHandle.getCopy (egammas, sys));
        for (xAOD::Egamma *egamma : *egammas)
        {
          if (m_preselection.getBool (*egamma))
          {
            ANA_CHECK_CORRECTION (m_outOfValidity, *egamma, m_isolationCorrectionTool->applyCorrection (*egamma));
          }
        }
        return StatusCode::SUCCESS;
      });
  }
}
