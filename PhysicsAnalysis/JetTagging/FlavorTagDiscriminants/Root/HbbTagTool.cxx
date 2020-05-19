/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/HbbTagTool.h"
#include "FlavorTagDiscriminants/HbbTag.h"

namespace FlavorTagDiscriminants {

  HbbTagTool::HbbTagTool(const std::string& name):
    asg::AsgTool(name),
    m_props(),
    m_hbb(nullptr)
  {
    declareProperty("nnFile", m_props.nnFile);
  }
  HbbTagTool::~HbbTagTool() {}

  StatusCode HbbTagTool::initialize() {
    ATH_MSG_INFO("Initialize HbbTag from: " + m_props.nnFile);
    m_hbb.reset(new HbbTag(HbbTagConfig(m_props.nnFile)));
    return StatusCode::SUCCESS;
  }

  int HbbTagTool::decorate(const xAOD::JetContainer& jets) const {
    for (const xAOD::Jet* jet: jets) {
      m_hbb->decorate(*jet);
    }
    return 0; // 0 means success
  }

}
