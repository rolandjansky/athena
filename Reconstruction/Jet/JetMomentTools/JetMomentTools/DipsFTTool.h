/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// DipsFTTool.h

#ifndef JetMomentTools_DipsFTTool_H
#define JetMomentTools_DipsFTTool_H

/// Marco Valente \n
/// December 2021
///
/// Tool to calculate the jet width.

#include "JetInterface/IJetDecorator.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/PropertyWrapper.h"
#include "AsgTools/ToolHandle.h"
#include "AsgDataHandles/WriteDecorHandleKey.h"


class DipsFTTool : public asg::AsgTool,
                     virtual public IJetDecorator {
  ASG_TOOL_CLASS(DipsFTTool, IJetDecorator)

public:

  // Constructor from tool name.
  DipsFTTool(std::string myname);

  virtual StatusCode initialize() override;

  // Inherited method to decorate a jet container.
  // Calls width and puts the result on the jets.
  virtual StatusCode decorate(const xAOD::JetContainer& jets) const override;

  // Local method to calculate and return the width.
  float score(const xAOD::Jet& jet) const;

private:

  Gaudi::Property<std::string> m_jetContainerName{this, "JetContainer", "",
      "SG key for the input jet container"};

  SG::WriteDecorHandleKey<xAOD::JetContainer> m_scoreKey{this, "ScoreKey", "DipzScore",
      "SG key for width decoration (not including jet container name)"};
};

#endif
