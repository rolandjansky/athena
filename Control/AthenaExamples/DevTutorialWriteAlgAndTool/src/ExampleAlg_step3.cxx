/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "ExampleAlg_step3.h"

StatusCode ExampleAlg_step3::initialize()
{
  ATH_CHECK( m_trackParticleKey.initialize());
  return StatusCode::SUCCESS;
}

StatusCode ExampleAlg_step3::execute(const EventContext& ctx) const
{
  ATH_MSG_VERBOSE("Events processed: "<<ctx.evt());
  SG::ReadHandle<xAOD::TrackParticleContainer> handle(m_trackParticleKey, ctx);
  ATH_MSG_VERBOSE("Got back  "<<handle->size());
  return StatusCode::SUCCESS;
}
