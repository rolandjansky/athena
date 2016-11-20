/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__TOP_ELECTRON_TOOL_H
#define QUICK_ANA__TOP_ELECTRON_TOOL_H


// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.

// It is flexible enough to provide:
//   Defintions corresponding to the default top group object selection(s)
//     (as implemented in the AnalysisTop packages)
//   Definitions for some other analyses that use slightly modified object selections
//     (e.g., Top/Exotics SS+bjets)



#include <QuickAna/Global.h>

#include <QuickAna/AnaToolCorrect.h>
#include <QuickAna/AnaToolSelect.h>

// #include <QuickAna/IsolationTools.h>
#include <QuickAna/SelectionCut.h>

namespace ana
{
  /// @brief Tool for the correction of electrons
  ///
  /// Applies correction-related systematics as well
  ///
  class TopElectronToolPreCorrect : virtual public AnaToolCorrect<xAOD::ElectronContainer>
  {

    /// This macro generates a proper constructor for Athena
    ASG_TOOL_CLASS (TopElectronToolPreCorrect, ana::IAnaTool)

  public:
    // Public interface methods

    /// Standard constructor
    TopElectronToolPreCorrect (const std::string& name);

    /// \copydoc IAnaTool::step
  public:
    virtual AnalysisStep step () const override;

    /// Initialize this tool
    StatusCode initialize() override;

    /// Run the correction on this event
    virtual StatusCode
    correctObject (xAOD::Electron& electron) override;

  private:
    bool trackSelector(const xAOD::TrackParticle &trk);
  };


  /// @brief Tool for selection of analysis electrons
  ///
  /// Applies top default electron selections
  ///
  class TopElectronToolSelect : virtual public AnaToolSelect<xAOD::ElectronContainer>
  {

    /// This macro generates a proper constructor for Athena
    ASG_TOOL_CLASS (TopElectronToolSelect, ana::IAnaTool)

  public:
    // Public interface methods

    /// Standard constructor
    TopElectronToolSelect (const std::string& name);

    /// Initialize this tool
    StatusCode initialize() override;

    /// Apply selection on this object
    virtual StatusCode
    selectObject (xAOD::Electron& electron) override;

  private:

    // Configuration

    /// quality string
    std::string m_idStr;

    /// minimum pT
    double m_ptMin;
    /// maximum |eta|
    double m_etaMax; // Note: this is not configurable in TopObjectSelectionTools
    /// veto the crack region?
    bool m_vetoCrack;

    /// |z0| cut
    double m_z0cut;

  public:
    /// isolation tool
    std::unique_ptr<IsolationBase> m_isolation;

    SelectionCut cut_author;
    SelectionCut cut_pt;
    SelectionCut cut_id;
    SelectionCut cut_etaMax;
    SelectionCut cut_etaCrack;
    SelectionCut cut_z0;
    SelectionCut cut_isolation;
  };


  StatusCode makeTopElectronTool (DefinitionArgs& args,
                                  const std::string& quality_str,
                                  IsolationBase* isolation=nullptr,
                                  double z0cut=-1.,
                                  bool vetoCrack=true);
}

#endif
