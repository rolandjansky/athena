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

// #include <QuickAna/ElectronTool.h>

#include <QuickAna/AnaToolSelect.h>
#include <AsgTools/AnaToolHandle.h>
#include <QuickAna/SelectionCut.h>

namespace CP
{
  class IIsolationSelectionTool;
}
class IAsgElectronLikelihoodTool;

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

  class HZZElectronTool : public AnaToolSelect<xAOD::ElectronContainer>
  { // added by Cong for ZZ4l

  ASG_TOOL_CLASS (HZZElectronTool, ana::IAnaTool)


  public:
    HZZElectronTool (const std::string& name);


  public:
    StatusCode initialize() override;


  public:
    virtual StatusCode
    selectObject (xAOD::Electron& electron) override;

  public:
    std::string m_selection;
    int m_wp;
    std::string m_isoStr;

  /// description: the selection tool
  public:
    asg::AnaToolHandle<IAsgElectronLikelihoodTool> m_selectionTool;
    asg::AnaToolHandle<CP::IIsolationSelectionTool> m_isolationTool;

  private:
    SelectionCut cut_Pt;
    SelectionCut cut_Eta;
    SelectionCut cut_OQ;
    SelectionCut cut_selectionTool;
    SelectionCut cut_Z0;
    SelectionCut cut_D0;
    SelectionCut cut_Iso;
  };

  StatusCode makeHZZElectronTool(DefinitionArgs& args,
                                 const std::string& selection = "LooseLLH",
                                 int WP = WPType::_HZZ4l,
                                 const std::string& isolationWPString = "FixedCutLoose" );



}

#endif
