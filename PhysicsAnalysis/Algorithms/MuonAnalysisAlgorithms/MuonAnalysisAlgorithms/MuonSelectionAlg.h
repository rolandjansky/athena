/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak

#ifndef MUON_ANALYSIS_ALGORITHMS__MUON_SELECTION_ALG_H
#define MUON_ANALYSIS_ALGORITHMS__MUON_SELECTION_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <MuonAnalysisInterfaces/IMuonSelectionTool.h>
#include <SelectionHelpers/ISelectionAccessor.h>
#include <SelectionHelpers/SelectionReadHandle.h>
#include <SystematicsHandles/SysCopyHandle.h>
#include <SystematicsHandles/SysListHandle.h>
#include <xAODMuon/MuonContainer.h>

namespace CP
{
  /// \brief an algorithm for calling \ref IMuonSelectionTool

  class MuonSelectionAlgV2 final : public EL::AnaAlgorithm
  {
    /// \brief the standard constructor
  public:
    MuonSelectionAlgV2 (const std::string& name, 
                        ISvcLocator* pSvcLocator);


  public:
    StatusCode initialize () override;

  public:
    StatusCode execute () override;
    


    /// \brief the selection tool
  private:
    ToolHandle<IMuonSelectionTool> m_selectionTool;

    /// \brief the systematics list we run
  private:
    SysListHandle m_systematicsList {this};

    /// \brief the preselection we apply to our input
  private:
    SelectionReadHandle m_preselection {
      this, "preselection", "", "the preselection to apply"};

    /// \brief the particle continer we run on
  private:
    SysCopyHandle<xAOD::MuonContainer> m_muonsHandle {
      this, "muons", "Muons", "the muons collection to run on"};

    /// \brief the decoration for the quality selection
  private:
    std::string m_selectionDecoration;

    /// \brief the accessor for \ref m_selectionDecoration
  private:
    std::unique_ptr<ISelectionAccessor> m_selectionAccessor;

    /// \brief the decoration for the bad muon veto
  private:
    std::string m_badMuonVetoDecoration;

    /// \brief the accessor for \ref m_selectionDecoration
  private:
    std::unique_ptr<ISelectionAccessor> m_badMuonVetoAccessor;

    /// \brief the bits to set for an object failing the preselection
  private:
    SelectionType m_setOnFail;
  };
}

#endif
