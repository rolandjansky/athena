/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/BTagMuonAugmenterTool.h"

namespace FlavorTagDiscriminants {

  BTagMuonAugmenterTool::BTagMuonAugmenterTool(const std::string& name):
    asg::AsgTool(name),
    m_aug(nullptr)
  {
    // you'll probably have to initialize some accessors here (or above)
  }
  BTagMuonAugmenterTool::~BTagMuonAugmenterTool() {}

  StatusCode BTagMuonAugmenterTool::initialize() {
    return StatusCode::SUCCESS;
  }
  StatusCode BTagMuonAugmenterTool::finalize() {
    return StatusCode::SUCCESS;
  }

  int BTagMuonAugmenterTool::modifyJet(xAOD::Jet& jet) const {
    m_aug->augment(jet);
    return 0; // 0 means success
  }

}
