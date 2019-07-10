/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/BTagAugmenterTool.h"
#include "FlavorTagDiscriminants/BTagJetAugmenter.h"

namespace FlavorTagDiscriminants {

  BTagAugmenterTool::BTagAugmenterTool(const std::string& name):
    asg::AsgTool(name),
    m_schema(""),
    m_flipTagConfig("STANDARD"),
    m_aug(nullptr)
  {
    declareProperty("schema", m_schema);
    declareProperty("flipTagConfig", m_flipTagConfig);
  }
  BTagAugmenterTool::~BTagAugmenterTool() {}

  StatusCode BTagAugmenterTool::initialize() {
    m_aug.reset(
      new BTagJetAugmenter(
        enumFromString(m_schema),
        flipTagConfigFromString(m_flipTagConfig)));
    return StatusCode::SUCCESS;
  }
  StatusCode BTagAugmenterTool::finalize() {
    return StatusCode::SUCCESS;
  }

  int BTagAugmenterTool::modifyJet(xAOD::Jet& jet) const {
    m_aug->augment(jet);
    return 0; // 0 means success
  }

}
