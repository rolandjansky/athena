/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/BTagAugmenterTool.h"
#include "FlavorTagDiscriminants/BTagJetAugmenter.h"

namespace FlavorTagDiscriminants {

  BTagAugmenterTool::BTagAugmenterTool(const std::string& name):
    asg::AsgTool(name),
    m_flipTagConfig("STANDARD"),
    m_aug(nullptr)
  {
    declareProperty("flipTagConfig", m_flipTagConfig);
  }
  BTagAugmenterTool::~BTagAugmenterTool() {}

  StatusCode BTagAugmenterTool::initialize() {
    m_aug.reset(
      new BTagJetAugmenter("BTagTrackToJetAssociator",
        flipTagConfigFromString(m_flipTagConfig)));
    return StatusCode::SUCCESS;
  }

  void BTagAugmenterTool::decorate(const xAOD::Jet& jet) const {
    m_aug->augment(jet);
  }

}
