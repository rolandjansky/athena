/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak

//
// includes
//

#include <MuonAnalysisAlgorithms/MuonSelectionAlg.h>
#include <PATCore/IAsgSelectionTool.h>

//
// method implementations
//

namespace CP
{
  MuonSelectionAlgV2 ::
  MuonSelectionAlgV2 (const std::string& name, 
                      ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_selectionTool ("", this)
  {
    declareProperty ("selectionTool", m_selectionTool, "the selection tool we apply");
    declareProperty ("selectionDecoration", m_selectionDecoration, "the decoration for the quality selection");
    declareProperty ("badMuonVetoDecoration", m_badMuonVetoDecoration, "the decoration for the bad muon veto");
  }



  StatusCode MuonSelectionAlgV2 ::
  initialize ()
  {
    if (m_selectionDecoration.empty())
    {
      ANA_MSG_ERROR ("no selection decoration name set");
      return StatusCode::FAILURE;
    }
    ANA_CHECK (makeSelectionAccessor (m_selectionDecoration, m_selectionAccessor));
    ANA_CHECK (m_selectionTool.retrieve());

    if (!m_badMuonVetoDecoration.empty())
    {
      ANA_CHECK (makeSelectionAccessor (m_badMuonVetoDecoration, m_badMuonVetoAccessor));
      ANA_CHECK (m_selectionTool.retrieve());
    }

    m_systematicsList.addHandle (m_muonsHandle);
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_preselection.initialize());

    auto *selectionTool = dynamic_cast<IAsgSelectionTool *>(&*m_selectionTool);
    asg::AcceptData blankAccept {&selectionTool->getAcceptInfo()};
    m_setOnFail = selectionFromAccept(blankAccept);

    return StatusCode::SUCCESS;
  }



  StatusCode MuonSelectionAlgV2 ::
  execute ()
  {
    return m_systematicsList.foreach ([&] (const CP::SystematicSet& sys) -> StatusCode
    {
      xAOD::MuonContainer *muons = nullptr;
      ANA_CHECK (m_muonsHandle.getCopy (muons, sys));
      for (xAOD::Muon *muon : *muons)
      {
        if (m_preselection.getBool (*muon))
        {
          m_selectionAccessor->setBits
            (*muon, selectionFromAccept (m_selectionTool->accept (*muon)));
          
          if (m_badMuonVetoAccessor != nullptr)
          {
            m_badMuonVetoAccessor->setBool (*muon, m_selectionTool->isBadMuon (*muon));
          }
        } else {
          m_selectionAccessor->setBits (*muon, m_setOnFail);

          if (m_badMuonVetoAccessor != nullptr)
          {
            m_badMuonVetoAccessor->setBool (*muon, false);
          }
        }
      }
      return StatusCode::SUCCESS;
    });
  }
}
