/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef QUICK_ANA__ZZ4l_MUON_TOOL_H
#define QUICK_ANA__ZZ4l_MUON_TOOL_H

// This module still needs to be documented.  The interface provided
// in this module is intended for experts only.  The module is
// considered to be in the pre-alpha stage.



#include <QuickAna/Global.h>

// #include <QuickAna/MuonTool.h>

#include <QuickAna/AnaToolSelect.h>
#include <AsgTools/AnaToolHandle.h>
#include <QuickAna/SelectionCut.h>

namespace CP
{
  class IMuonSelectionTool;
  class IIsolationSelectionTool;
}

namespace WPType{
   enum {
      _HZZ4l,
      _SMZZ4l,
      _ZHinv,
      _Hmumu
   };
}


namespace ana
{

  class HZZMuonTool : public AnaToolSelect<xAOD::MuonContainer>
  { // added by Cong for ZZ4l

  ASG_TOOL_CLASS (HZZMuonTool, ana::IAnaTool)


  public:
    HZZMuonTool (const std::string& name);


  public:
    StatusCode initialize() override;


  public:
    virtual StatusCode
    selectObject (xAOD::Muon& muon) override;

  public:
    xAOD::Muon::Quality m_quality;
    int m_wp;
    std::string m_isoStr;

  /// description: the selection tool
  public:
    asg::AnaToolHandle<CP::IMuonSelectionTool> m_selection;
    asg::AnaToolHandle<CP::IIsolationSelectionTool> m_isolationTool;

  private:
    SelectionCut cut_ID;
    SelectionCut cut_Pt;
    SelectionCut cut_D0;
    SelectionCut cut_Z0;

    SelectionCut cut_CB;
    SelectionCut cut_Eta;
    SelectionCut cut_Iso;

  };

  StatusCode makeHZZMuonTool(DefinitionArgs& args,
                          const xAOD::Muon::Quality& quality=xAOD::Muon::Loose,
                          int WP = WPType::_HZZ4l,
                          const std::string& isolationWPString = "FixedCutLoose" );



}

#endif
