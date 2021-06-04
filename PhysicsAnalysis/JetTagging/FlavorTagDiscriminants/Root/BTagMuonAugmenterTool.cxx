/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/BTagMuonAugmenterTool.h"
#include "FlavorTagDiscriminants/BTagMuonAugmenter.h"

namespace FlavorTagDiscriminants {

  BTagMuonAugmenterTool::BTagMuonAugmenterTool(const std::string& name):
    asg::AsgTool(name),
    m_aug(nullptr)
  {
    declareProperty("MuonAssociationName", m_muonAssociationName="Muons");
    declareProperty("MuonMinDR", m_muonMinDR=defaults::MUON_MIN_DR);
    declareProperty("MuonMinpT", m_muonMinpT=defaults::MUON_MIN_PT);
    declareProperty("flipTagConfig", m_flipTagConfig = "STANDARD");
  }
  BTagMuonAugmenterTool::~BTagMuonAugmenterTool() {}

  StatusCode BTagMuonAugmenterTool::initialize() {
    m_aug.reset(new BTagMuonAugmenter(
                  m_muonAssociationName,
                  m_muonMinDR,
                  m_muonMinpT,
                  flipTagConfigFromString(m_flipTagConfig)));
    return StatusCode::SUCCESS;
  }

  void BTagMuonAugmenterTool::decorate(const xAOD::BTagging& btag) const {
    m_aug->augment(btag);
  }

  std::set<std::string> BTagMuonAugmenterTool::getDecoratorKeys() const {
    return m_aug->getDecoratorKeys();
  }
  std::set<std::string> BTagMuonAugmenterTool::getAuxInputKeys() const {
    return m_aug->getAuxInputKeys();
  }
  std::set<std::string> BTagMuonAugmenterTool::getConstituentAuxInputKeys()
    const {
    return m_aug->getConstituentAuxInputKeys();
  }

}
