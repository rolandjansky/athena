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
    declareProperty("trackAssociator", m_trackAssociator);
  }
  BTagAugmenterTool::~BTagAugmenterTool() {}

  StatusCode BTagAugmenterTool::initialize() {
    m_aug.reset(
      new BTagJetAugmenter(m_trackAssociator,
        flipTagConfigFromString(m_flipTagConfig)));
    return StatusCode::SUCCESS;
  }

  void BTagAugmenterTool::decorate(const xAOD::BTagging& btag) const {
    m_aug->augment(btag);
  }

  std::set<std::string> BTagAugmenterTool::getDecoratorKeys() const {
    return m_aug->getDecoratorKeys();
  }

  std::set<std::string> BTagAugmenterTool::getAuxInputKeys() const {
    return m_aug->getAuxInputKeys();
  }
  std::set<std::string> BTagAugmenterTool::getConstituentAuxInputKeys() const {
    return {};
  }

}
