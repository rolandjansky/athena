/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetWidthTool.h

#ifndef JetMomentTools_JetWidthTool_H
#define JetMomentTools_JetWidthTool_H

/// Steven Schramm and David Adams \n
/// February 2014
///
/// Tool to calculate the jet width.

#include "JetInterface/IJetDecorator.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "StoreGate/WriteDecorHandleKey.h"

#include "PFlowUtils/IWeightPFOTool.h"

class JetWidthTool : public asg::AsgTool,
                     virtual public IJetDecorator {
  ASG_TOOL_CLASS(JetWidthTool, IJetDecorator)

public:

  // Constructor from tool name.
  JetWidthTool(std::string myname);

  virtual StatusCode initialize();

  // Inherited method to decorate a jet container.
  // Calls width and puts the result on the jets.
  virtual StatusCode decorate(const xAOD::JetContainer& jets) const;

  // Local method to calculate and return the width.
  //double width(const xAOD::Jet& jet) const;
  double width(const xAOD::Jet& jet, double& widthEta, double& widthPhi) const;

private:

  std::string m_jetContainerName;
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_widthKey;
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_widthPhiKey;
};

#endif
