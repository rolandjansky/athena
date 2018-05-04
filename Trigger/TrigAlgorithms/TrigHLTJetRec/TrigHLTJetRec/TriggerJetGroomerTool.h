// JtRecToolTrigger.h. This file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TriggerJetGroomerTool_H
#define TriggerJetGroomerTool_H

/// \class TriggerJetGroomerTool
///
/// This class contains the Run2 trigger specifics for JetGroomerTool

#include "TrigHLTJetRec/ITriggerJetGroomerTool.h"
#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetGroomer.h"
#include "JetInterface/IJetModifier.h"
#include "JetInterface/IJetPseudojetRetriever.h"

class TriggerJetGroomerTool : 
  public asg::AsgTool, virtual public ITriggerJetGroomerTool{
  ASG_TOOL_CLASS(TriggerJetGroomerTool, ITriggerJetGroomerTool)
  
  public:
  
  TriggerJetGroomerTool(const std::string& name);
  virtual ~TriggerJetGroomerTool() override {}
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual xAOD::JetContainer* groom(const xAOD::JetContainer*) const override;
private:
  
  ToolHandle<IJetGroomer> m_groomer;
  ToolHandleArray<IJetModifier> m_modifiers;
  ToolHandle<IJetPseudojetRetriever> m_jetPseudoJetRetriever;
};

#endif
