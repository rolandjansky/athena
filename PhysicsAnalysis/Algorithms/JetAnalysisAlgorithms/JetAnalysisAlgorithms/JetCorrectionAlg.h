/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JET_ANALYSIS_ALGORITHMS_JET_CORRECTION_ALG_H
#define JET_ANALYSIS_ALGORITHMS_JET_CORRECTION_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <JetCPInterfaces/ICPJetCorrectionTool.h>
#include <SelectionHelpers/OutOfValidityHelper.h>
#include <SelectionHelpers/SelectionReadHandle.h>
#include <SystematicsHandles/SysCopyHandle.h>
#include <SystematicsHandles/SysListHandle.h>


namespace CP
{
  /// \brief an algorithm for calling \ref ICPJetCorrectionTool

  class JetCorrectionAlg final : public EL::AnaAlgorithm
  {
    /// \brief the standard constructor
  public:
    JetCorrectionAlg (const std::string& name, 
                      ISvcLocator* pSvcLocator);


  public:
    StatusCode initialize () override;

  public:
    StatusCode execute () override;



    /// \brief the uncertainties tool
  private:
    ToolHandle<ICPJetCorrectionTool> m_correctionTool;

    /// \brief the systematics list we run
  private:
    SysListHandle m_systematicsList {this};

    /// \brief the jet collection we run on
  private:
    SysCopyHandle<xAOD::JetContainer> m_jetHandle {
      this, "jets", "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets", "the jet collection to run on"};

    /// \brief the preselection we apply to our input
  private:
    SelectionReadHandle m_preselection {
      this, "preselection", "", "the preselection to apply"};

    /// \brief the helper for OutOfValidity results
  private:
    OutOfValidityHelper m_outOfValidity {this};
  };
}

#endif
