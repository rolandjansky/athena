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
  }



  StatusCode EgammaIsolationSelectionAlg ::
  initialize ()
  {
    ANA_CHECK (m_selectionTool.retrieve());
      
    ANA_CHECK (m_egammasHandle.initialize (m_systematicsList));
    ANA_CHECK (m_preselection.initialize (m_systematicsList, m_egammasHandle, SG::AllowEmpty));
    ANA_CHECK (m_selectionHandle.initialize (m_systematicsList, m_egammasHandle));
    ANA_CHECK (m_systematicsList.initialize());

    Root::TAccept blankAccept = m_selectionTool->getObjTAccept();
    // Just in case this isn't initially set up as a failure clear it this one
    // time. This only calls reset on the bitset
    blankAccept.clear();
    m_setOnFail = selectionFromAccept(blankAccept);

    return StatusCode::SUCCESS;
  }



  StatusCode EgammaIsolationSelectionAlg ::
  execute ()
  {
    for (const auto& sys : m_systematicsList.systematicsVector())
    {
      const xAOD::EgammaContainer *egammas = nullptr;
      ANA_CHECK (m_egammasHandle.retrieve (egammas, sys));
      for (const xAOD::Egamma *egamma : *egammas)
      {
        if (m_preselection.getBool (*egamma, sys))
        {
          m_selectionHandle.setBits
            (*egamma, selectionFromAccept (m_selectionTool->accept (*egamma)), sys);
        } else {
          m_selectionHandle.setBits (*egamma, m_setOnFail, sys);
        }
      }
    }
    return StatusCode::SUCCESS;
  }
}
