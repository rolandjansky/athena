/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TruthClassDecoratorTool.cxx
 * @author shaun roe
 * 27 August 2014
 **/

#include "TruthClassDecoratorTool.h"
#include "xAODTracking/TrackParticle.h"



TruthClassDecoratorTool::TruthClassDecoratorTool(const std::string& type, const std::string& name,
                                                 const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_truthClassifier("MCTruthClassifier/MCTruthClassifier") {
  declareInterface<IInDetPhysValDecoratorTool>(this);
}

StatusCode
TruthClassDecoratorTool::initialize() {
  ATH_CHECK(AlgTool::initialize());
  ATH_CHECK(m_truthClassifier.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode
TruthClassDecoratorTool::finalize() {
  return StatusCode::SUCCESS;
}

bool
TruthClassDecoratorTool::decorateTruth(const xAOD::TruthParticle& particle, const std::string& prefix) {
  bool success(false);

  if (not m_truthClassifier.empty()) {
    auto truthClass = m_truthClassifier->particleTruthClassifier(&particle);
    int type = static_cast<int>(truthClass.first);
    int origin = static_cast<int>(truthClass.second);
    particle.auxdecor<int>(prefix + "truthType") = type;
    particle.auxdecor<int>(prefix + "truthOrigin") = origin;
    success = true;
  }
  return success;
}
