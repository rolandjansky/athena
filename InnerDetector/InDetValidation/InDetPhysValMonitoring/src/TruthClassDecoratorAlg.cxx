/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TruthClassDecoratorAlg.cxx
 * @author shaun roe
 * 27 August 2014
 **/

#include "TruthClassDecoratorAlg.h"
#include "safeDecorator.h"


TruthClassDecoratorAlg::TruthClassDecoratorAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name,pSvcLocator)
{
}

StatusCode
TruthClassDecoratorAlg::initialize() {
  ATH_CHECK(m_truthClassifier.retrieve());
  ATH_CHECK( m_truthSelectionTool.retrieve( EnableTool { not m_truthSelectionTool.name().empty() } ) );
  ATH_CHECK( m_truthParticleName.initialize() );

  std::vector<std::string> decor_names(kNDecorators);
  decor_names[kDecorTruthType]="truthType";
  decor_names[kDecorTruthOrigin]="truthOrigin";
  IDPVM::createDecoratorKeys(*this, m_truthParticleName, m_prefix.value(), decor_names, m_decor);
  assert( m_decor.size() == kNDecorators);
  return StatusCode::SUCCESS;
}

StatusCode
TruthClassDecoratorAlg::finalize() {
  return StatusCode::SUCCESS;
}


// to migrate to AthReentrantAlgorithm later
StatusCode
TruthClassDecoratorAlg::execute(const EventContext &ctx) const {
  SG::ReadHandle<xAOD::TruthParticleContainer> ptruth(m_truthParticleName, ctx);
  if ((not ptruth.isValid())) {
    return StatusCode::FAILURE;
  }

  std::vector< SG::WriteDecorHandle<xAOD::TruthParticleContainer,int> >
    int_decor( IDPVM::createDecorators<xAOD::TruthParticleContainer,int>(m_decor, ctx) );

  if (m_truthSelectionTool.get()) {
    for (const xAOD::TruthParticle *truth_particle : *ptruth) {
      if (not m_truthSelectionTool->accept(truth_particle)) continue;
      if (not decorateTruth(*truth_particle, int_decor) ) return StatusCode::FAILURE;
    }
  }
  else {
    for (const xAOD::TruthParticle *truth_particle : *ptruth) {
      if (not decorateTruth(*truth_particle, int_decor) ) return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

bool
TruthClassDecoratorAlg::decorateTruth(const xAOD::TruthParticle& particle,
                                       std::vector< SG::WriteDecorHandle<xAOD::TruthParticleContainer,int> > &int_decor) const {
  bool success(false);

  if (not m_truthClassifier.empty()) {
    auto truthClass = m_truthClassifier->particleTruthClassifier(&particle);
    int type = static_cast<int>(truthClass.first);
    int origin = static_cast<int>(truthClass.second);
    int_decor[kDecorTruthType](particle)   = type;
    int_decor[kDecorTruthOrigin](particle) = origin;
    success = true;
  }
  return success;
}
