/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef QUICK_ANA__FAT_JET_TOOL_H
#define QUICK_ANA__FAT_JET_TOOL_H

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

class IJetCalibrationTool;
class ICPJetUncertaintiesTool;
class IJERTool;
class IJERSmearingTool;
class IJetSelector;
class IBTaggingEfficiencyTool;

namespace ana
{
  /// TODO: this class needs documentation
  class FatJetToolCorrect : public AnaToolCorrect<xAOD::JetContainer>
  {
    //
    // public interface
    //

    ASG_TOOL_CLASS (FatJetToolCorrect, ana::IAnaTool)

  public:

    /// effects: standard constructor
    /// guarantee: strong
    /// failures: out of memory II
    FatJetToolCorrect (const std::string& name);


    /// effects: initialize this tool
    /// guarantee: basic
    /// failures: configuration errors
    StatusCode initialize() override;


    /// effects: run the calibration on this event
    /// returns: whether we accepted the event
    /// guarantee: basic
    /// failures: tool dependent
    virtual StatusCode
    correctObject (xAOD::Jet& jet) override;

  private:

    /// description: the calibration tool
    asg::AnaToolHandle<IJetCalibrationTool> m_calibration_tool;

    /// description: the uncertainties tool
    asg::AnaToolHandle<ICPJetUncertaintiesTool> m_uncertainties_tool;

    /// description: the resolution tool
    asg::AnaToolHandle<IJERTool> m_resolution_tool;

    /// description: the smearing tool
    asg::AnaToolHandle<IJERSmearingTool> m_smearing_tool;
  };



  /// TODO: this class needs documentation
  class FatJetToolSelect : public AnaToolSelect<xAOD::JetContainer>
  {
    //
    // public interface
    //

    ASG_TOOL_CLASS (FatJetToolSelect, ana::IAnaTool)

  public:

    /// effects: standard constructor
    /// guarantee: strong
    /// failures: out of memory II
    FatJetToolSelect (const std::string& name);


    /// effects: initialize this tool
    /// guarantee: basic
    /// failures: configuration errors
    StatusCode initialize() override;


    /// returns: whether the object passes object selection
    /// guarantee: basic
    /// failures: definition dependent
    virtual StatusCode
    selectObject (xAOD::Jet& jet) override;

  private:

    SelectionCut cut_eta;
    SelectionCut cut_pt;
  };



  class FatJetToolWeight : public AnaToolWeight<xAOD::JetContainer>
  {
    //
    // public interface
    //

    ASG_TOOL_CLASS (FatJetToolWeight, ana::IAnaTool)

  public:

    /// effects: standard constructor
    /// guarantee: strong
    /// failures: out of memory II
    FatJetToolWeight (const std::string& name);


    /// effects: initialize this tool
    /// guarantee: basic
    /// failures: configuration errors
    StatusCode initialize() override;


    /// effects: calculate the weight for this event
    /// returns: the calculated weight
    /// guarantee: basic
    /// failures: tool dependent
    virtual StatusCode
    objectWeight (const xAOD::Jet& jet, float& weight) override;

  private:

    /// description: the b-tagging efficiency tool
    // asg::AnaToolHandle<IBTaggingEfficiencyTool> m_btagging_eff_tool;
  };
}

#endif
