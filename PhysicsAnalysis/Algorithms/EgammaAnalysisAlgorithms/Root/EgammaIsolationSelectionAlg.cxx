/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EgammaAnalysisAlgorithms/EgammaIsolationSelectionAlg.h>

//
// method implementations
//

namespace CP
{
  EgammaIsolationSelectionAlg ::
  EgammaIsolationSelectionAlg (const std::string& name, 
                     ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_selectionTool ("", this)
  {
    declareProperty ("selectionTool", m_selectionTool, "the selection tool we apply");
    declareProperty ("selectionDecoration", m_selectionDecoration, "the decoration for the asg selection");
  }



  StatusCode EgammaIsolationSelectionAlg ::
  initialize ()
  {
    if (m_selectionDecoration.empty())
    {
      ANA_MSG_ERROR ("no selection decoration name set");
      return StatusCode::FAILURE;
    }
    ANA_CHECK (makeSelectionAccessor (m_selectionDecoration, m_selectionAccessor));
    ANA_CHECK (m_selectionTool.retrieve());
      
    m_systematicsList.addHandle (m_egammasHandle);
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_preselection.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode EgammaIsolationSelectionAlg ::
  execute ()
  {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
        xAOD::EgammaContainer *egammas = nullptr;
        ANA_CHECK (m_egammasHandle.getCopy (egammas, sys));
        for (xAOD::Egamma *egamma : *egammas)
        {
          if (m_preselection.getBool (*egamma))
          {
            m_selectionAccessor->setBits
              (*egamma, selectionFromAccept (m_selectionTool->accept (*egamma)));
          }
        }
        return StatusCode::SUCCESS;
      });
  }
}
