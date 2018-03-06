/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef MUON_ANALYSIS_ALGORITHMS__MUON_ISOLATION_ALG_H
#define MUON_ANALYSIS_ALGORITHMS__MUON_ISOLATION_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <SelectionHelpers/SelectionHelpers.h>
#include <SystematicsHandles/SysCopyHandle.h>
#include <SystematicsHandles/SysListHandle.h>
#include <xAODMuon/MuonContainer.h>

namespace CP
{
  class IIsolationSelectionTool;

  /// \brief an algorithm for calling \ref IMuonSelectionTool

  class MuonIsolationAlg final : public EL::AnaAlgorithm
  {
    /// \brief the standard constructor
  public:
    MuonIsolationAlg (const std::string& name, 
                                   ISvcLocator* pSvcLocator);


  public:
    StatusCode initialize () override;

  public:
    StatusCode execute () override;
    


    /// \brief the smearing tool
  private:
    ToolHandle<IIsolationSelectionTool> m_isolationTool;

    /// \brief the systematics list we run
  private:
    SysListHandle m_systematicsList {this};

    /// \brief the muon collection we run on
  private:
    SysCopyHandle<xAOD::MuonContainer> m_muonHandle {
      this, "muons", "Muons", "the muon collection to run on"};

    /// \brief the decoration for the muon isolation
  private:
    std::string m_isolationDecoration;

    /// \brief the accessor for \ref m_isolationDecoration
  private:
    std::unique_ptr<const SG::AuxElement::Accessor<SelectionType> > m_isolationAccessor;
  };
}

#endif
