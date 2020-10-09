/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "AsgTools/PropertyWrapper.h"
#include "AsgTools/ToolHandle.h"
#include "AsgDataHandles/WriteDecorHandleKey.h"

#include "PFlowUtils/IWeightPFOTool.h"

class JetWidthTool : public asg::AsgTool,
                     virtual public IJetDecorator {
  ASG_TOOL_CLASS(JetWidthTool, IJetDecorator)

public:

  // Constructor from tool name.
  JetWidthTool(std::string myname);

  virtual StatusCode initialize() override;

  // Inherited method to decorate a jet container.
  // Calls width and puts the result on the jets.
  virtual StatusCode decorate(const xAOD::JetContainer& jets) const override;

  // Local method to calculate and return the width.
  float width(const xAOD::Jet& jet, float& widthEta, float& widthPhi) const;

private:

  Gaudi::Property<std::string> m_jetContainerName{this, "JetContainer", "",
      "SG key for the input jet container"};

  SG::WriteDecorHandleKey<xAOD::JetContainer> m_widthKey{this, "WidthKey", "Width",
      "SG key for width decoration (not including jet container name)"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_widthPhiKey{this, "WidthPhiKey", "WidthPhi",
      "SG key for width phi decoration (not including jet container name)"};
};

#endif
