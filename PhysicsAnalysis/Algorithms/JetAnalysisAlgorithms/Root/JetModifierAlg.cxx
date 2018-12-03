/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <JetAnalysisAlgorithms/JetModifierAlg.h>

//
// method implementations
//

namespace CP
{
  JetModifierAlg ::
  JetModifierAlg (const std::string& name, 
                     ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_modifierTool ("JetForwardJvtTool", this)
  {
    declareProperty ("modifierTool", m_modifierTool, "the modifier tool we apply");
  }



  StatusCode JetModifierAlg ::
  initialize ()
  {
    ANA_CHECK (m_modifierTool.retrieve());
    m_systematicsList.addHandle (m_jetHandle);
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_outOfValidity.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode JetModifierAlg ::
  execute ()
  {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
        xAOD::JetContainer *jets = nullptr;
        ANA_CHECK (m_jetHandle.getCopy (jets, sys));
        if (m_modifierTool->modify (*jets) != 0)
        {
          ANA_MSG_ERROR ("Failed to call \"m_modifierTool->modify (*jets)\"");
          return StatusCode::FAILURE;
        }
        return StatusCode::SUCCESS;
      });
  }
}
