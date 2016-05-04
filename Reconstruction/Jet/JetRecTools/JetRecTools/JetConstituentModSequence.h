/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//JetConstituentModSequence.h

#ifndef JETRECTOOLS_JETCONSTITUENTMODSEQUENCE_H
#define JETRECTOOLS_JETCONSTITUENTMODSEQUENCE_H

//
// // Michael Nelson, CERN & Univesity of Oxford
// // February, 2016

#include <string>
#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetExecuteTool.h"
#include "JetInterface/IJetConstituentModifier.h"
#include "AsgTools/ToolHandleArray.h"
#include "xAODCore/ShallowCopy.h"


class JetConstituentModSequence: public asg::AsgTool, virtual public IJetExecuteTool { // Changed from IJetExecute
ASG_TOOL_CLASS(JetConstituentModSequence, IJetExecuteTool)
public:
  JetConstituentModSequence(const std::string &name); // MEN: constructor 
  StatusCode initialize();
  int execute() const;

protected:
  std::string m_inputContainer = "";
  std::string m_outputContainer = "";

  // P-A : a property defining the type name of the input constituent
  std::string m_inputTypeName = "CaloCluster"; // MEN: Check this 
  // P-A : the actual type
  xAOD::Type::ObjectType m_inputType; // 
   

  ToolHandleArray<IJetConstituentModifier> m_modifiers;
};

#endif
