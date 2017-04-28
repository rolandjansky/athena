/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__ELECTRON_TOOL_H
#define QUICK_ANA__ELECTRON_TOOL_H

// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.

#include <QuickAna/Global.h>

#include <QuickAna/AnaToolCorrect.h>
#include <AsgTools/AnaToolHandle.h>
#include <QuickAna/AnaToolSelect.h>
#include <QuickAna/AnaToolWeight.h>
#include <QuickAna/SelectionCut.h>
#include <QuickAna/xAODInclude.h>

/// Forward declarations
class IAsgElectronIsEMSelector;
class IAsgElectronLikelihoodTool;
namespace CP{
  class IEgammaCalibrationAndSmearingTool;
  class IIsolationSelectionTool;
  class IIsolationCorrectionTool;
}
class IAsgElectronEfficiencyCorrectionTool;

namespace ana
{
  /// @brief Tool for the pre-correction of electrons
  ///
  /// Applies corrections independent on systematics
  ///
  class ElectronToolPreCorrect : virtual public AnaToolCorrect<xAOD::ElectronContainer>
  {

    /// This macro generates a proper constructor for Athena
    ASG_TOOL_CLASS (ElectronToolPreCorrect, ana::IAnaTool)

  public:
    // Public interface methods

    /// Standard constructor
    ElectronToolPreCorrect (const std::string& name);

    /// \copydoc IAnaTool::step
  public:
    virtual AnalysisStep step () const override;

    /// Initialize this tool
    StatusCode initialize() override;

    /// Run the correction on this event
    virtual StatusCode
    correctObject (xAOD::Electron& electron) override;

  private:
    // CP tools

    /// The likelihood selection tool
    asg::AnaToolHandle<IAsgElectronLikelihoodTool> m_likelihoodTool_loose;
    asg::AnaToolHandle<IAsgElectronLikelihoodTool> m_likelihoodTool_medium;
    asg::AnaToolHandle<IAsgElectronLikelihoodTool> m_likelihoodTool_tight;
  };



  /// @brief Tool for the correction of electrons
  ///
  /// Applies correction-related systematics as well
  ///
  class ElectronToolCorrect : virtual public AnaToolCorrect<xAOD::ElectronContainer>
  {

    /// This macro generates a proper constructor for Athena
    ASG_TOOL_CLASS (ElectronToolCorrect, ana::IAnaTool)

  public:
    // Public interface methods

    /// Standard constructor
    ElectronToolCorrect (const std::string& name);

    /// Initialize this tool
    StatusCode initialize() override;

    /// Run the correction on this event
    virtual StatusCode
    correctObject (xAOD::Electron& electron) override;

  private:
    // Configuration

    /// Is this AF2?
    bool m_isAF2;
    /// Is this data?
    bool m_isData;

    // CP tools

    /// The calibration tool
    asg::AnaToolHandle<CP::IEgammaCalibrationAndSmearingTool> m_calibration;
    /// The CP isolation correction tool
    asg::AnaToolHandle<CP::IIsolationCorrectionTool> m_isoCorrTool;

    SelectionCut cut_correction;
    SelectionCut cut_isoCorr_tool;
  };



  /// @brief Tool for selection of analysis electrons
  ///
  /// Applies common electron selections
  ///
  class ElectronToolSelect : virtual public AnaToolSelect<xAOD::ElectronContainer>
  {

    /// This macro generates a proper constructor for Athena
    ASG_TOOL_CLASS (ElectronToolSelect, ana::IAnaTool)

  public:
    // Public interface methods

    /// Standard constructor
    ElectronToolSelect (const std::string& name);

    /// Initialize this tool
    StatusCode initialize() override;

    /// Apply selection on this object
    virtual StatusCode
    selectObject (xAOD::Electron& electron) override;

  private:

    // Configuration

    /// The selection string
    std::string m_idStr;
    /// Minimum PT requirement
    float m_ptMin;
    /// Maximum |eta| requirement
    float m_etaMax;

    /// Use isolation cut
    bool m_isolationOn;
    /// Isolation working point
    std::string m_isolationWP;

    /// Use a different ID at analysis level
    std::string m_idAna;
    /// Apply looser impact parameter cuts
    bool m_looseImpactCut;


    // CP tools

    /// The isolation selection tool
    asg::AnaToolHandle<CP::IIsolationSelectionTool> m_isolationTool;

    // the different cuts
    SelectionCut cut_id;
    SelectionCut cut_OQ;
    SelectionCut cut_ptMin;
    SelectionCut cut_clusEta;
    SelectionCut cut_d0;
    SelectionCut cut_z0;
    SelectionCut cut_isolationTool;
    SelectionCut cut_idAna;
  };



  /// @brief Tool for calculating weights for analysis electrons
  ///
  /// Applies the electron reco and ID efficiency scale factors
  ///
  class ElectronToolWeight : virtual public AnaToolWeight<xAOD::ElectronContainer>
  {

    /// This macro generates a proper constructor for Athena
    ASG_TOOL_CLASS (ElectronToolWeight, ana::IAnaTool)

  public:
    // Public interface methods

    /// Standard constructor
    ElectronToolWeight (const std::string& name);

    /// Initialize this tool
    StatusCode initialize() override;

    /// Calculate the weight for this event
    virtual StatusCode
    objectWeight (const xAOD::Electron& elecron, float& weight) override;

  public:
    // CP tools

    /// The efficiency correction tools - currently need one for each of
    ///  ID, reco, and trigger efficiencies.
    asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool> m_efficiencyTool_reco;
    asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool> m_efficiencyTool_id;
    asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool> m_efficiencyTool_iso;

  private:
    // Configuration

    /// The selection string
    std::string m_idStr;
    /// The isolation WP
    std::string m_isoStr;
    /// Use tight ID at analysis level
    std::string m_idAna;
    /// Is this AF2?
    bool m_isAF2;
  };


  StatusCode makeElectronTool(DefinitionArgs& args,
                              const std::string& id,
                              const bool isolationOn = false,
                              const std::string& isolationWP = "Gradient",
                              const std::string& idAna = "",
                              const bool looseImpactCut=false);

}

#endif
