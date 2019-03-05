/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef TAU_ANALYSIS_ALGORITHMS__DI_TAU_EFFICIENCY_CORRECTIONS_ALG_H
#define TAU_ANALYSIS_ALGORITHMS__DI_TAU_EFFICIENCY_CORRECTIONS_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <TauAnalysisTools/IDiTauEfficiencyCorrectionsTool.h>
#include <SelectionHelpers/OutOfValidityHelper.h>
#include <SelectionHelpers/SelectionReadHandle.h>
#include <SystematicsHandles/SysCopyHandle.h>
#include <SystematicsHandles/SysListHandle.h>
#include <SystematicsHandles/SysReadHandle.h>
#include <xAODTau/DiTauJetContainer.h>

namespace CP
{
  /// \brief an algorithm for calling \ref IDiTauEfficiencyCorrectionsTool

  class DiTauEfficiencyCorrectionsAlg final : public EL::AnaAlgorithm
  {
    /// \brief the standard constructor
  public:
    DiTauEfficiencyCorrectionsAlg (const std::string& name, 
                                   ISvcLocator* pSvcLocator);


  public:
    StatusCode initialize () override;

  public:
    StatusCode execute () override;
    


    /// \brief the smearing tool
  private:
    ToolHandle<TauAnalysisTools::IDiTauEfficiencyCorrectionsTool> m_efficiencyCorrectionsTool;

    /// \brief the systematics list we run
  private:
    SysListHandle m_systematicsList {this};

    /// \brief the tau collection we run on
  private:
    SysCopyHandle<xAOD::DiTauJetContainer> m_tauHandle {
      this, "taus", "DiTauJets", "the tau collection to run on"};

    /// \brief the preselection we apply to our input
  private:
    SelectionReadHandle m_preselection {
      this, "preselection", "", "the preselection to apply"};

    /// \brief the helper for OutOfValidity results
  private:
    OutOfValidityHelper m_outOfValidity {this};

    /// \brief the decoration for the tau efficiency
  private:
    std::string m_efficiencyDecoration;

    /// \brief the accessor for \ref m_efficiencyDecoration
  private:
    std::unique_ptr<const SG::AuxElement::Accessor<float> > m_efficiencyAccessor;
  };
}

#endif
