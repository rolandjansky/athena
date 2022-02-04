/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "PFTrackPreselAlg.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "AthContainers/ConstDataVector.h"
#include <memory>

namespace {
  const SG::AuxElement::Decorator<char> decPass("passPFTrackPresel");
}

PFTrackPreselAlg::PFTrackPreselAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{
}

PFTrackPreselAlg::~PFTrackPreselAlg() {}

StatusCode PFTrackPreselAlg::initialize()
{
  ATH_CHECK( m_inputTracksKey.initialize() );
  ATH_CHECK( m_outputTracksKey.initialize() );
  ATH_CHECK( m_trackSelTool.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode PFTrackPreselAlg::execute(const EventContext &ctx) const
{
  auto input = SG::makeHandle(m_inputTracksKey, ctx);
  if (!input.isValid())
  {
    ATH_MSG_ERROR("Failed to retrieve " << m_inputTracksKey);
    return StatusCode::FAILURE;
  }
  auto output = std::make_unique<ConstDataVector<xAOD::TrackParticleContainer>>(SG::VIEW_ELEMENTS);
  for (const xAOD::TrackParticle* itrk : *input)
  {
    if (itrk->pt() > m_upperPtCut || !m_trackSelTool->accept(*itrk))
    {
      decPass(*itrk) = false;
      continue;
    }
    decPass(*itrk) = true;
    output->push_back(itrk);
  }
  auto outputHandle = SG::makeHandle(m_outputTracksKey, ctx);
  ATH_CHECK(outputHandle.put(std::move(output)) != nullptr);
  return StatusCode::SUCCESS;
}
