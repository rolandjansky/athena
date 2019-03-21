/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef QUICK_ANA__MUON_TOOL_H
#define QUICK_ANA__MUON_TOOL_H

// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.



#include <QuickAna/Global.h>

#include <QuickAna/AnaToolCorrect.h>
#include <AsgTools/AnaToolHandle.h>
#include <QuickAna/AnaToolSelect.h>
#include <QuickAna/AnaToolWeight.h>
#include <QuickAna/SelectionCut.h>

namespace CP
{
  class IMuonCalibrationAndSmearingTool;
  class IMuonSelectionTool;
  class IIsolationSelectionTool;
  class IMuonEfficiencyScaleFactors;
  class IPileupReweightingTool;
}

namespace ana
{
  class MuonToolCorrect : public AnaToolCorrect<xAOD::MuonContainer>
  {
    //
    // public interface
    //

    ASG_TOOL_CLASS (MuonToolCorrect, ana::IAnaTool)


    /// effects: standard constructor
    /// guarantee: strong
    /// failures: out of memory II
  public:
    MuonToolCorrect (const std::string& name);


    /// effects: initialize this tool
    /// guarantee: basic
    /// failures: configuration errors
  public:
    StatusCode initialize() override;


    /// effects: run the calibration on this event
    /// returns: whether we accepted the event
    /// guarantee: basic
    /// failures: tool dependent
  public:
    virtual StatusCode
    correctObject (xAOD::Muon& muon) override;


    /// description: the calibration/smearing tool
  private:
//    asg::AnaToolHandle<CP::IMuonCalibrationAndSmearingTool> m_calib_smear;
    asg::AnaToolHandle<CP::IMuonCalibrationAndSmearingTool> m_calib_smear_16;
    asg::AnaToolHandle<CP::IMuonCalibrationAndSmearingTool> m_calib_smear_17;


  private:
    SelectionCut cut_calib_tool;

  };



  class MuonToolSelect : public AnaToolSelect<xAOD::MuonContainer>
  {
    //
    // public interface
    //

    ASG_TOOL_CLASS (MuonToolSelect, ana::IAnaTool)


    /// effects: standard constructor
    /// guarantee: strong
    /// failures: out of memory II
  public:
    MuonToolSelect (const std::string& name);


    /// effects: initialize this tool
    /// guarantee: basic
    /// failures: configuration errors
  public:
    StatusCode initialize() override;


    /// returns: whether the object passes object selection
    /// guarantee: basic
    /// failures: definition dependent
  public:
    virtual StatusCode
    selectObject (xAOD::Muon& muon) override;


  public:
    /// description: desired quality
    xAOD::Muon::Quality m_quality;

  private:

    // Configuration

    /// Use isolation cut
    bool m_isolationOn;
    /// Isolation working point
    std::string m_isolationWP;
    /// Apply z0/d0/isolation cuts after MET and OR?
    bool m_inclusiveMET;
    /// Apply looser impact parameter cuts
    bool m_looseImpactCut;
    /// Point above which to apply the high pT cuts
    float m_high_pT;
    /// Don't apply a TRT cut -- only for specific derivations 
    bool m_noTRT;
    /// Apply the high-pT bad muon veto
    bool m_doBadMuVeto;

    // CP tools

    /// description: the selection tool
    asg::AnaToolHandle<CP::IMuonSelectionTool> m_selection;
    /// description: the selection tool for high-pT working point
    asg::AnaToolHandle<CP::IMuonSelectionTool> m_selectionHPT;
    /// description: the isolation selection tool
    asg::AnaToolHandle<CP::IIsolationSelectionTool> m_isolationTool;

    SelectionCut cut_selection;
    SelectionCut cut_d0;
    SelectionCut cut_z0;
    SelectionCut cut_high_pT;
    SelectionCut cut_isolationTool;
  };





  class MuonToolWeight : public AnaToolWeight<xAOD::MuonContainer>
  {
    //
    // public interface
    //

    ASG_TOOL_CLASS (MuonToolWeight, ana::IAnaTool)


    /// effects: standard constructor
    /// guarantee: strong
    /// failures: out of memory II
  public:
    MuonToolWeight (const std::string& name);


    /// effects: initialize this tool
    /// guarantee: basic
    /// failures: configuration errors
  public:
    StatusCode initialize() override;


    /// effects: calculate the weight for this event
    /// returns: the calculated weight
    /// guarantee: basic
    /// failures: tool dependent
  public:
    virtual StatusCode
    objectWeight (const xAOD::Muon& muon, float& weight) override;


  private:
    // CP tools

    /// description: the reco efficiency correction tool
    asg::AnaToolHandle<CP::IMuonEfficiencyScaleFactors> m_efficiency_scale;
    /// description: the reco efficiency correction tool for high-pT
    asg::AnaToolHandle<CP::IMuonEfficiencyScaleFactors> m_efficiency_scaleHPT;

    /// description: the TTVA efficiency correction tool
    asg::AnaToolHandle<CP::IMuonEfficiencyScaleFactors> m_ttva_efficiency_scale;

    /// description: the isolation efficiency correction tool
    asg::AnaToolHandle<CP::IMuonEfficiencyScaleFactors> m_isolation_scale;

    /// description: the bad muon veto efficiency correction tool
    asg::AnaToolHandle<CP::IMuonEfficiencyScaleFactors> m_badmuonveto_scale;

  private:
    /// Configuration:

    /// Isolation working point string
    std::string m_isolationWP;
    /// Point above which to apply the high pT cuts
    float m_high_pT;
    /// Don't apply a TRT cut -- only for specific derivations 
    bool m_noTRT;
    /// Apply the high-pT bad muon veto
    bool m_doBadMuVeto;

  public:
    /// description: desired quality
    xAOD::Muon::Quality m_quality;
  };


  StatusCode makeMuonTool(DefinitionArgs& args,
                          const xAOD::Muon::Quality& quality,
                          const bool& isolationOn = false,
                          const std::string& isolationWPString = "Gradient",
                          const bool& inclusiveMET=false,
                          const bool& looseImpactCut=false,
                          const float& high_pT=-1.,
                          const bool noTRT=false,
                          const bool badMuVeto=false);

}

#endif
