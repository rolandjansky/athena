/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <MuonAnalysisAlgorithms/MuonIsolationAlg.h>

#include <IsolationSelection/IIsolationSelectionTool.h>
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
    , m_isolationTool ("MuonIsolationTool", this)
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
    m_isolationAccessor = std::make_unique<SG::AuxElement::Accessor<SelectionType> > (m_isolationDecoration);

    ANA_CHECK (m_isolationTool.retrieve());
    m_systematicsList.addHandle (m_muonHandle);
    ANA_CHECK (m_systematicsList.initialize());
    return StatusCode::SUCCESS;
  }



  StatusCode MuonIsolationAlg ::
  execute ()
  {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode {
        xAOD::MuonContainer *muons = nullptr;
        ANA_CHECK (m_muonHandle.getCopy (muons, sys));
        for (xAOD::Muon *muon : *muons)
        {
          (*m_isolationAccessor) (*muon)
            = selectionFromAccept (m_isolationTool->accept (*muon));
        }
        return StatusCode::SUCCESS;
      });
  }
}
