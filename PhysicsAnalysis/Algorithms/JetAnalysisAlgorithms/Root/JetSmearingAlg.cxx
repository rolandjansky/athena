/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <JetAnalysisAlgorithms/JetSmearingAlg.h>

//
// method implementations
//

namespace CP
{
  JetSmearingAlg ::
  JetSmearingAlg (const std::string& name, 
                     ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_smearingTool ("JERSmearingTool", this)
  {
    declareProperty ("smearingTool", m_smearingTool, "the smearing tool we apply");
  }



  StatusCode JetSmearingAlg ::
  initialize ()
  {
    ANA_CHECK (m_smearingTool.retrieve());
    ANA_CHECK (m_jetHandle.initialize (m_systematicsList));
    ANA_CHECK (m_systematicsList.addSystematics (*m_smearingTool));
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_preselection.initialize());
    ANA_CHECK (m_outOfValidity.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode JetSmearingAlg ::
  execute ()
  {
    for (const auto& sys : m_systematicsList.systematicsVector())
    {
      ANA_CHECK (m_smearingTool->applySystematicVariation (sys));
      xAOD::JetContainer *jets = nullptr;
      ANA_CHECK (m_jetHandle.getCopy (jets, sys));
      for (xAOD::Jet *jet : *jets)
      {
        if (m_preselection.getBool (*jet))
        {
          ANA_CHECK_CORRECTION (m_outOfValidity, *jet, m_smearingTool->applyCorrection (*jet));
        }
      }
    }
    return StatusCode::SUCCESS;
  }
}
