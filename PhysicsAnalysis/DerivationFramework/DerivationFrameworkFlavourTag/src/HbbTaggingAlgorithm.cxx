/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "HbbTaggingAlgorithm.h"

#include "JetCalibTools/JetCalibrationTool.h"
#include "BoostedJetTaggers/HbbTaggerDNN.h"

#include "xAODJet/JetContainer.h"
#include "xAODCore/ShallowCopy.h"


HbbTaggingAlgorithm::HbbTaggingAlgorithm(
  const std::string& name, ISvcLocator* loc):
  AthAlgorithm(name, loc),
  m_decoration_name(""),
  m_dec_jet("HbbScore")
{
  declareProperty("jetCollectionName", m_jet_collection);
  declareProperty("decorationName", m_decoration_name);
  declareProperty("minPt", m_min_pt);
  declareProperty("maxEta", m_max_eta);
  declareProperty("tagger", m_tagger);
  declareProperty("calibrationTool", m_jet_calib_tool);
}

StatusCode HbbTaggingAlgorithm::initialize() {
  CHECK(m_tagger.retrieve());
  CHECK(m_jet_calib_tool.retrieve());
  if (m_decoration_name.size() > 0) {
    m_dec_jet = SG::AuxElement::Decorator<double>(m_decoration_name);
  }
  return StatusCode::SUCCESS;
}

StatusCode HbbTaggingAlgorithm::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode HbbTaggingAlgorithm::execute() {
  const xAOD::JetContainer *rawjets = 0;
  CHECK( evtStore()->retrieve(rawjets, m_jet_collection) );
  auto shallow_jets = xAOD::shallowCopyContainer (*rawjets);
  std::unique_ptr<xAOD::JetContainer> jets(shallow_jets.first);
  std::unique_ptr<xAOD::ShallowAuxContainer> aux(shallow_jets.second);
  for (auto jet: *jets) {
    m_jet_calib_tool->applyCorrection(*jet);
    if (jet->pt() < m_min_pt) continue;
    if (std::abs(jet->eta()) > m_max_eta) continue;
    const xAOD::Jet* raw_jet = rawjets->at(jet->index());
    double score = m_tagger->getScore(*jet);
    m_dec_jet(*raw_jet) = score;
  }
  return StatusCode::SUCCESS;
}
