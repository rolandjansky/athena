// for text editors: this file is -*- C++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VR_JET_OVERLAP_DECORATOR_TOOL_H
#define VR_JET_OVERLAP_DECORATOR_TOOL_H

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetDecorator.h"

class VRJetOverlapDecorator;

namespace FlavorTagDiscriminants {

  class VRJetOverlapDecoratorTool : public asg::AsgTool, virtual public IJetDecorator
  {
    ASG_TOOL_CLASS(VRJetOverlapDecoratorTool, IJetDecorator )
  public:
    VRJetOverlapDecoratorTool(const std::string& name);
    ~VRJetOverlapDecoratorTool();

    StatusCode initialize() override;

    // returns 0 for success
    StatusCode decorate(const xAOD::JetContainer& jets) const override;
  private:
    std::unique_ptr<VRJetOverlapDecorator> m_dec;
  };

}
#endif
