/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__TRUTH_JET_TOOLS_H
#define QUICK_ANA__TRUTH_JET_TOOLS_H

#include <QuickAna/Global.h>

#include <QuickAna/AnaToolCorrect.h>
#include <QuickAna/AnaToolSelect.h>
// #include <xAODJet/JetContainer.h>

namespace ana
{

  /// @brief Correction tool for truth jets
  ///
  /// Doesn't really do anything currently besides set the
  /// preselection flag to true, but in the future it may
  /// contain some truth-to-reco smearing functionality.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class TruthJetToolCorrect : public AnaToolCorrect<xAOD::JetContainer>
  {

    ASG_TOOL_CLASS (TruthJetToolCorrect, ana::IAnaTool)

  public:

    /// @brief Standard constructor
    TruthJetToolCorrect (const std::string& name);

    /// @brief Initialize this tool
    StatusCode initialize() override;

    /// @brief Apply selection to a truth jet
    /// @param jet the truth jet
    /// @param accept boolean selection flag passed by reference
    virtual StatusCode correctObject (xAOD::Jet& jet) override;

  private:

  }; // class TruthJetToolCorrect


  /// @brief Selection tool for truth jets
  ///
  /// Contains the slimmed down functionality necessary to select truth jets.
  /// There is no reco-like cleaning tool.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class TruthJetToolSelect : public AnaToolSelect<xAOD::JetContainer>
  {

    ASG_TOOL_CLASS (TruthJetToolSelect, ana::IAnaTool)

  public:

    /// @brief Standard constructor
    TruthJetToolSelect (const std::string& name);

    /// @brief Initialize this tool
    StatusCode initialize() override;

    /// @brief Apply selection to a truth jet
    /// @param jet the truth jet
    /// @param accept boolean selection flag passed by reference
    virtual StatusCode selectObject (xAOD::Jet& jet) override;

  private:

  }; // class TruthJetToolSelect

} // namespace ana

#endif
