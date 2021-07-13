/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CopyPileupParticleTruthInfo.h"

#include "xAODTruth/TruthParticleAuxContainer.h"


CopyPileupParticleTruthInfo::CopyPileupParticleTruthInfo(const std::string &name, ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator) {}

StatusCode CopyPileupParticleTruthInfo::initialize()
{
  ATH_MSG_DEBUG("Initializing...");

  // Check and initialize keys
  ATH_CHECK( m_bkgInputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_bkgInputKey);
  ATH_CHECK( m_outputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_outputKey);

  return StatusCode::SUCCESS;
}

StatusCode CopyPileupParticleTruthInfo::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("execute() begin");

  // Reading the input timings
  ATH_MSG_VERBOSE("Retrieving input Truth particle containers");

  SG::ReadHandle<xAOD::TruthParticleContainer> bkgContainer(m_bkgInputKey, ctx);
  if (!bkgContainer.isValid()) {
    ATH_MSG_ERROR("Could not get background Truth particle container " << bkgContainer.name() << " from store " << bkgContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found background Truth particle container " << bkgContainer.name() << " in store " << bkgContainer.store());

  // Creating output Truth particle container
  SG::WriteHandle<xAOD::TruthParticleContainer> outputContainer(m_outputKey, ctx);
  ATH_CHECK(outputContainer.record(std::make_unique<xAOD::TruthParticleContainer>(), std::make_unique<xAOD::TruthParticleAuxContainer>()));
  if (!outputContainer.isValid()) {
    ATH_MSG_ERROR("Could not record output Truth particle container " << outputContainer.name() << " to store " << outputContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Recorded output Truth particle container " << outputContainer.name() << " in store " << outputContainer.store());

  outputContainer->reserve(bkgContainer->size());

  for (const xAOD::TruthParticle *bkgTruthParticle : *bkgContainer.cptr()) {
    xAOD::TruthParticle *xTruthParticle = new xAOD::TruthParticle();
    outputContainer->push_back(xTruthParticle);
    *xTruthParticle = *bkgTruthParticle;
  }

  ATH_MSG_DEBUG("execute() end");
  return StatusCode::SUCCESS;
}
