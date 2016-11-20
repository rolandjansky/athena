/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__TOPMUON_TOOL_H
#define QUICK_ANA__TOPMUON_TOOL_H


// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.

// It is flexible enough to provide:
//   Defintions corresponding to the default top group object selection(s)
//     (as implemented in the AnalysisTop packages)
//   Definitions for some other analyses that use slightly modified object selections
//     (e.g., Top/Exotics SS+bjets)



#include <QuickAna/Global.h>

#include <AsgTools/AnaToolHandle.h>
#include <QuickAna/AnaToolCorrect.h>
#include <QuickAna/AnaToolSelect.h>
#include <QuickAna/SelectionCut.h>

// #include <QuickAna/IsolationTools.h>

namespace CP
{
  class IMuonSelectionTool;
}

namespace ana
{
  /// This class is added so we can add additional variables that are common to
  class TopMuonToolPreCorrect : public AnaToolCorrect<xAOD::MuonContainer>
  {
    //
    // public interface
    //

    ASG_TOOL_CLASS (TopMuonToolPreCorrect, ana::IAnaTool)


    /// effects: standard constructor
    /// guarantee: strong
    /// failures: out of memory II
  public:
    TopMuonToolPreCorrect (const std::string& name);


    /// \copydoc IAnaTool::step
  public:
    virtual AnalysisStep step () const override;


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


  private:
    bool trackSelector(const xAOD::TrackParticle &trk);
  };



  class TopMuonToolSelect : public AnaToolSelect<xAOD::MuonContainer>
  {
    //
    // public interface
    //

    ASG_TOOL_CLASS (TopMuonToolSelect, ana::IAnaTool)


    /// effects: standard constructor
    /// guarantee: strong
    /// failures: out of memory II
  public:
    TopMuonToolSelect (const std::string& name);


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


    /// quality
  public:
    xAOD::Muon::Quality m_quality;

    /// author
  public:
    xAOD::Muon::Author m_author;

    /// islation tool
  public:
    std::unique_ptr<IsolationBase> m_isolation;

    /// |z0| cut
  public:
    double m_z0cut;

    /// |d0_significance| cut
  public:
    double m_sd0cut;

    /// the selection tool
  public:
    asg::AnaToolHandle<CP::IMuonSelectionTool> m_selection;

  private:
    SelectionCut cut_selection;
    SelectionCut cut_author;
    SelectionCut cut_minPt;
    SelectionCut cut_maxEta;
    SelectionCut cut_z0;
    SelectionCut cut_sd0;
    SelectionCut cut_isolation;
  };

  StatusCode makeTopMuonTool (DefinitionArgs& args,
                              xAOD::Muon::Quality quality = xAOD::Muon::Tight,
                              xAOD::Muon::Author author = xAOD::Muon::MuidCo,
                              IsolationBase* isolation = nullptr,
                              double z0cut = -1.,
                              double sd0cut = -1.);
}

#endif
