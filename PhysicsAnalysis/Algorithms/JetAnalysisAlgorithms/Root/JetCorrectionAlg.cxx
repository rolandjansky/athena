/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
// includes
//

#include <JetAnalysisAlgorithms/JetCorrectionAlg.h>

//
// method implementations
//

namespace CP
{
  JetCorrectionAlg ::
  JetCorrectionAlg (const std::string& name, 
                    ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_correctionTool ("JetCorrectionTool", this)
  {
    declareProperty ("correctionTool", m_correctionTool, "the correction tool we apply");
  }



  StatusCode JetCorrectionAlg ::
  initialize ()
  {
    ANA_CHECK (m_correctionTool.retrieve());
    ANA_CHECK (m_jetHandle.initialize (m_systematicsList));
    ANA_CHECK (m_systematicsList.addSystematics (*m_correctionTool));
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_preselection.initialize());
    ANA_CHECK (m_outOfValidity.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode JetCorrectionAlg ::
  execute ()
  {
    for (const auto& sys : m_systematicsList.systematicsVector())
    {
      ANA_CHECK (m_correctionTool->applySystematicVariation (sys));
      xAOD::JetContainer *jets = nullptr;
      ANA_CHECK (m_jetHandle.getCopy (jets, sys));
      for (xAOD::Jet *jet : *jets)
      {
        if (m_preselection.getBool (*jet))
        {
          ANA_CHECK_CORRECTION (m_outOfValidity, *jet, m_correctionTool->applyCorrection (*jet));
        }
      }
    }
    return StatusCode::SUCCESS;
  }
}
