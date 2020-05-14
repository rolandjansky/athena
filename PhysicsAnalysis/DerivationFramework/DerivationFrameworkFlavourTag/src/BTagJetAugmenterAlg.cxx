/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BTagJetAugmenterAlg.h"

#include "xAODJet/JetContainer.h"

BTagJetAugmenterAlg::BTagJetAugmenterAlg(
  const std::string& name, ISvcLocator* loc):
  AthAlgorithm(name, loc),
  m_jet_collection(),
  m_augmenter(nullptr)
{
  declareProperty("JetCollectionName", m_jet_collection);
}

StatusCode BTagJetAugmenterAlg::initialize() {

  m_augmenter.reset(new BTagJetAugmenter(""));
  return StatusCode::SUCCESS;
}

StatusCode BTagJetAugmenterAlg::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode BTagJetAugmenterAlg::execute() {
  const xAOD::JetContainer *jets = nullptr;
  CHECK( evtStore()->retrieve(jets, m_jet_collection) );
  for (const auto* jet: *jets) {
    m_augmenter->augment(*jet);
  }
  return StatusCode::SUCCESS;
}
