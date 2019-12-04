/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "BTagging/BTagNNAlg.h"

#include "xAODJet/JetContainer.h"
#include "FlavorTagDiscriminants/DL2HighLevel.h"


namespace Analysis {

  BTagNNAlg::BTagNNAlg(
    const std::string& name, ISvcLocator* loc):
    AthAlgorithm(name, loc),
    m_dl2(nullptr)
  {
    declareProperty("nnPath", m_nnPath);
    declareProperty("jetCollections", m_jetCollections);
    declareProperty("variableRemapping", m_variableRemapping);
  }
  StatusCode BTagNNAlg::initialize() {
    namespace ftd = FlavorTagDiscriminants;
    auto ftc = ftd::FlipTagConfig::STANDARD;
    m_dl2.reset(new ftd::DL2HighLevel(m_nnPath, ftc, m_variableRemapping));
    return StatusCode::SUCCESS;
  }

  StatusCode BTagNNAlg::finalize() {
    return StatusCode::SUCCESS;
  }

  StatusCode BTagNNAlg::execute() {
    for (const auto& jcn: m_jetCollections) {
      const xAOD::JetContainer* jets = nullptr;
      CHECK( evtStore()->retrieve(jets, jcn) );
      for (const xAOD::Jet* jet: *jets) {
        m_dl2->decorate(*jet);
      }
    }
    return StatusCode::SUCCESS;
  }


}
