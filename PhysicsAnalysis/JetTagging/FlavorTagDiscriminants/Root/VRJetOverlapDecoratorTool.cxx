/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/VRJetOverlapDecoratorTool.h"
#include "FlavorTagDiscriminants/VRJetOverlapDecorator.h"

namespace FlavorTagDiscriminants {

  VRJetOverlapDecoratorTool::VRJetOverlapDecoratorTool(const std::string& name):
    asg::AsgTool(name),
    m_dec(nullptr)
  {
  }
  VRJetOverlapDecoratorTool::~VRJetOverlapDecoratorTool() {}

  StatusCode VRJetOverlapDecoratorTool::initialize() {
    m_dec.reset(new VRJetOverlapDecorator);
    return StatusCode::SUCCESS;
  }

  StatusCode VRJetOverlapDecoratorTool::decorate(const xAOD::JetContainer& jets) const {
    m_dec->decorate(jets);
    return StatusCode::SUCCESS;
  }

}
