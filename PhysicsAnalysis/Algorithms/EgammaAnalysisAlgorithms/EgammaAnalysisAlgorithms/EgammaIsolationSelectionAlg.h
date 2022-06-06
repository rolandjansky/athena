/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef EGAMMA_ANALYSIS_ALGORITHMS__EGAMMA_ISOLATION_SELECTION_ALG_H
#define EGAMMA_ANALYSIS_ALGORITHMS__EGAMMA_ISOLATION_SELECTION_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <IsolationSelection/IIsolationSelectionTool.h>
#include <EgammaAnalysisAlgorithms/CopyHelpers.h>
#include <SelectionHelpers/OutOfValidityHelper.h>
#include <SelectionHelpers/SysReadSelectionHandle.h>
#include <SelectionHelpers/SysWriteSelectionHandle.h>
#include <SystematicsHandles/SysListHandle.h>
#include <SystematicsHandles/SysReadHandle.h>

namespace CP
{
  /// \brief an algorithm for calling \ref IIsolationSelectionTool

  class EgammaIsolationSelectionAlg final : public EL::AnaAlgorithm
  {
    /// \brief the standard constructor
  public:
    EgammaIsolationSelectionAlg (const std::string& name, 
                                 ISvcLocator* pSvcLocator);


  public:
    StatusCode initialize () override;

  public:
    StatusCode execute () override;
    


    /// \brief the selection tool
  private:
    ToolHandle<IIsolationSelectionTool> m_selectionTool;

    /// \brief the systematics list we run
  private:
    SysListHandle m_systematicsList {this};

    /// \brief the preselection we apply to our input
  private:
    SysReadSelectionHandle m_preselection {
      this, "preselection", "", "the preselection to apply"};

    /// \brief the particle continer we run on
  private:
    SysReadHandle<xAOD::EgammaContainer> m_egammasHandle {
      this, "egammas", "Electrons", "the egamma collection to run on"};

    /// \brief the decoration for the asg selection
  private:
    SysWriteSelectionHandle m_selectionHandle {
      this, "selectionDecoration", "isolated", "the decoration for the asg selection"};

    /// \brief the bits to set for an object failing the preselection
  private:
    SelectionType m_setOnFail;
  };
}

#endif
