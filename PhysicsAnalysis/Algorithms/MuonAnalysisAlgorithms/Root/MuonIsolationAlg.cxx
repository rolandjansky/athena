/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <MuonAnalysisAlgorithms/MuonIsolationAlg.h>

#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace CP
{
  MuonIsolationAlg ::
  MuonIsolationAlg (const std::string& name, 
                     ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_isolationTool ("CP::IsolationSelectionTool", this)
  {
    declareProperty ("isolationTool", m_isolationTool, "the isolation tool we apply");
    declareProperty ("isolationDecoration", m_isolationDecoration, "the decoration for the muon isolation");
  }



  StatusCode MuonIsolationAlg ::
  initialize ()
  {
    if (m_isolationDecoration.empty())
    {
      ANA_MSG_ERROR ("no isolation decoration name set");
      return StatusCode::FAILURE;
    }
    ANA_CHECK (makeSelectionAccessor (m_isolationDecoration, m_isolationAccessor));

    ANA_CHECK (m_isolationTool.retrieve());
    ANA_CHECK (m_muonHandle.initialize (m_systematicsList));
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_preselection.initialize());

    asg::AcceptData blankAccept {&m_isolationTool->getObjAcceptInfo()};
    m_setOnFail = selectionFromAccept(blankAccept);

    return StatusCode::SUCCESS;
  }



  StatusCode MuonIsolationAlg ::
  execute ()
  {
    for (const auto& sys : m_systematicsList.systematicsVector())
    {
      const xAOD::MuonContainer *muons = nullptr;
      ANA_CHECK (m_muonHandle.retrieve (muons, sys));
      for (const xAOD::Muon *muon : *muons)
      {
        if (m_preselection.getBool (*muon))
        {
          m_isolationAccessor->setBits
            (*muon, selectionFromAccept (m_isolationTool->accept (*muon)));
        } else {
          m_isolationAccessor->setBits (*muon, m_setOnFail);
        }
      }
    }
    return StatusCode::SUCCESS;
  }
}
