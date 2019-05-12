/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef ELECTRON_ANALYSIS_ALGORITHMS__ELECTRON_EFFICIENCY_SCALE_FACTOR_ALG_H
#define ELECTRON_ANALYSIS_ALGORITHMS__ELECTRON_EFFICIENCY_SCALE_FACTOR_ALG_H

#include <xAODEgamma/ElectronContainer.h>
#include <AnaAlgorithm/AnaAlgorithm.h>
#include <EgammaAnalysisInterfaces/IAsgElectronEfficiencyCorrectionTool.h>
#include <SelectionHelpers/OutOfValidityHelper.h>
#include <SelectionHelpers/SelectionReadHandle.h>
#include <SystematicsHandles/SysCopyHandle.h>
#include <SystematicsHandles/SysDecorationHandle.h>
#include <SystematicsHandles/SysListHandle.h>
#include <SystematicsHandles/SysReadHandle.h>

namespace CP
{
  /// \brief an algorithm for calling \ref IElectronEfficiencyCorrectionTool

  class ElectronEfficiencyCorrectionAlg final : public EL::AnaAlgorithm
  {
    /// \brief the standard constructor
  public:
    ElectronEfficiencyCorrectionAlg (const std::string& name, 
                                   ISvcLocator* pSvcLocator);


  public:
    StatusCode initialize () override;

  public:
    StatusCode execute () override;
    


    /// \brief the smearing tool
  private:
    ToolHandle<IAsgElectronEfficiencyCorrectionTool> m_efficiencyCorrectionTool;

    /// \brief the systematics list we run
  private:
    SysListHandle m_systematicsList {this};

    /// \brief the electron collection we run on
  private:
    SysCopyHandle<xAOD::ElectronContainer> m_electronHandle {
      this, "electrons", "Electrons", "the electron collection to run on"};

    /// \brief the preselection we apply to our input
  private:
    SelectionReadHandle m_preselection {
      this, "preselection", "", "the preselection to apply"};

    /// \brief the helper for OutOfValidity results
  private:
    OutOfValidityHelper m_outOfValidity {this};

    /// \brief the decoration for the electron scale factor
  private:
    SysDecorationHandle<float> m_scaleFactorDecoration {
      this, "scaleFactorDecoration", "", "the decoration for the electron efficiency scale factor"};
  };
}

#endif
