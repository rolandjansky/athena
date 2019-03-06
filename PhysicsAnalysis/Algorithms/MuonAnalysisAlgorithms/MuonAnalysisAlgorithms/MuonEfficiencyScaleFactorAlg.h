/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef MUON_ANALYSIS_ALGORITHMS__MUON_EFFICIENCY_SCALE_FACTOR_ALG_H
#define MUON_ANALYSIS_ALGORITHMS__MUON_EFFICIENCY_SCALE_FACTOR_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <MuonAnalysisInterfaces/IMuonEfficiencyScaleFactors.h>
#include <SelectionHelpers/OutOfValidityHelper.h>
#include <SelectionHelpers/SelectionReadHandle.h>
#include <SystematicsHandles/SysCopyHandle.h>
#include <SystematicsHandles/SysListHandle.h>
#include <SystematicsHandles/SysReadHandle.h>
#include <xAODEventInfo/EventInfo.h>
#include <xAODMuon/MuonContainer.h>

namespace CP
{
  /// \brief an algorithm for calling \ref IMuonEfficiencyScaleFactorTool

  class MuonEfficiencyScaleFactorAlg final : public EL::AnaAlgorithm
  {
    /// \brief the standard constructor
  public:
    MuonEfficiencyScaleFactorAlg (const std::string& name, 
                                   ISvcLocator* pSvcLocator);


  public:
    StatusCode initialize () override;

  public:
    StatusCode execute () override;
    


    /// \brief the smearing tool
  private:
    ToolHandle<IMuonEfficiencyScaleFactors> m_efficiencyScaleFactorTool;

    /// \brief the systematics list we run
  private:
    SysListHandle m_systematicsList {this};

    /// \brief the muon collection we run on
  private:
    SysCopyHandle<xAOD::MuonContainer> m_muonHandle {
      this, "muons", "Muons", "the muon collection to run on"};

    /// \brief the EventInfo collection we use
  private:
    SysReadHandle<xAOD::EventInfo> m_eventInfoHandle {
      this, "eventInfo", "EventInfo", "the EventInfo we use"};

    /// \brief the preselection we apply to our input
  private:
    SelectionReadHandle m_preselection {
      this, "preselection", "", "the preselection to apply"};

    /// \brief the helper for OutOfValidity results
  private:
    OutOfValidityHelper m_outOfValidity {this};

    /// \brief the decoration for the muon efficiency
  private:
    std::string m_scaleFactorDecoration;

    /// \brief the decoration for the muon mc efficiency
  private:
    std::string m_mcEfficiencyDecoration;

    /// \brief the decoration for the muon data efficiency
  private:
    std::string m_dataEfficiencyDecoration;

    /// \brief the accessor for \ref m_scaleFactorDecoration
  private:
    std::unique_ptr<const SG::AuxElement::Accessor<float> > m_scaleFactorAccessor;

    /// \brief the accessor for \ref m_mcEfficiencyDecoration
  private:
    std::unique_ptr<const SG::AuxElement::Accessor<float> > m_mcEfficiencyAccessor;

    /// \brief the accessor for \ref m_dataEfficiencyDecoration
  private:
    std::unique_ptr<const SG::AuxElement::Accessor<float> > m_dataEfficiencyAccessor;
  };
}

#endif
