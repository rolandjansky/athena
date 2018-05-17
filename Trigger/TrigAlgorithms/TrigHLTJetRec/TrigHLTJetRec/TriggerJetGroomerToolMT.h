// JtRecToolTrigger.h. This file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TriggerJetGroomerToolMT_H
#define TriggerJetGroomerToolMT_H

/// \class TriggerJetGroomerToolMT
///
/// This class contains the Run2 trigger specifics for JetGroomerToolMT

#include "TrigHLTJetRec/ITriggerJetGroomerToolMT.h"
#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetGroomerMT.h"
#include "JetInterface/IJetModifier.h"
#include "JetInterface/IJetPseudojetRetriever.h"

class PseudoJetContainer;

class TriggerJetGroomerToolMT : 
  public asg::AsgTool, virtual public ITriggerJetGroomerToolMT{
  ASG_TOOL_CLASS(TriggerJetGroomerToolMT, ITriggerJetGroomerToolMT)
  
  public:
  
  TriggerJetGroomerToolMT(const std::string& name);
  virtual ~TriggerJetGroomerToolMT() override {}
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  //
  virtual xAOD::JetContainer* groom(const xAOD::JetContainer* in,
                                    const PseudoJetContainer&) const override;
private:
  
  ToolHandle<IJetGroomerMT> m_groomer;
  ToolHandleArray<IJetModifier> m_modifiers;
  ToolHandle<IJetPseudojetRetriever> m_jetPseudoJetRetriever;
};

#endif
