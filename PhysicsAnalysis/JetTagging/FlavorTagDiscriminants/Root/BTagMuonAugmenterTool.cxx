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
    declareProperty("MuonAssociationName", m_muonAssociationName="Muons");
    declareProperty("MuonMinDR", m_muonMinDR=0.4);
    declareProperty("MuonMinpT", m_muonMinpT=4.0*1e3);



  }
  BTagMuonAugmenterTool::~BTagMuonAugmenterTool() {}

  StatusCode BTagMuonAugmenterTool::initialize() {
    m_aug.reset(new BTagMuonAugmenter(m_muonAssociationName, m_muonMinDR, m_muonMinpT));
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
