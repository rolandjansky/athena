/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#import "FlavorTagDiscriminants/DL2Tool.h"

#include "PathResolver/PathResolver.h"

namespace FlavorTagDiscriminants {

  DL2Tool::DL2Tool(const std::string& name):
    asg::AsgTool(name),
    m_props(),
    m_dl2(nullptr)
  {
    declareProperty("nnFile", m_props.nnFile)
  }

  StatusCode initialize() {
    std::string nn_path = PathResolverFindCalibFile(m_props.nnFile);
    m_dl2.reset(new DL2HighLevel(nn_path));
  }
  StatusCode finalize() {

  }

  int DL2Tool::modifyJet(xAOD::Jet& jet) const {
    m_dl2->decorate(jet);
  }

}
