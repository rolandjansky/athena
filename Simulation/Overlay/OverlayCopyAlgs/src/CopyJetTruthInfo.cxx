/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CopyJetTruthInfo.h"

#include "xAODJet/JetAuxContainer.h"


CopyJetTruthInfo::CopyJetTruthInfo(const std::string &name, ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator) {}

StatusCode CopyJetTruthInfo::initialize()
{
  ATH_MSG_DEBUG("Initializing...");

  // Check and initialize keys
  ATH_CHECK( m_bkgInputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_bkgInputKey);
  ATH_CHECK( m_outputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_outputKey);

  return StatusCode::SUCCESS;
}

StatusCode CopyJetTruthInfo::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("execute() begin");

  // Reading the input timings
  ATH_MSG_VERBOSE("Retrieving input jets containers");

  SG::ReadHandle<xAOD::JetContainer> bkgContainer(m_bkgInputKey, ctx);
  if (!bkgContainer.isValid()) {
    ATH_MSG_ERROR("Could not get background jets container " << bkgContainer.name() << " from store " << bkgContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found background jets container " << bkgContainer.name() << " in store " << bkgContainer.store());

  // Creating output jets container
  SG::WriteHandle<xAOD::JetContainer> outputContainer(m_outputKey, ctx);
  ATH_CHECK(outputContainer.record(std::make_unique<xAOD::JetContainer>(), std::make_unique<xAOD::JetAuxContainer>()));
  if (!outputContainer.isValid()) {
    ATH_MSG_ERROR("Could not record output jet container " << outputContainer.name() << " to store " << outputContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Recorded output jet container " << outputContainer.name() << " in store " << outputContainer.store());

  outputContainer->reserve(bkgContainer->size());

  for (const xAOD::Jet *bkgJet : *bkgContainer.cptr()) {
    xAOD::Jet *jet = new xAOD::Jet();
    outputContainer->push_back(jet);
    *jet = *bkgJet;
  }

  ATH_MSG_DEBUG("execute() end");
  return StatusCode::SUCCESS;
}
