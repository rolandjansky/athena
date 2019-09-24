/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "ExampleAlg_step4.h"

StatusCode ExampleAlg_step4::initialize()
{
  ATH_CHECK( m_trackParticleKey.initialize());
  ATH_CHECK( m_algTool.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode ExampleAlg_step4::execute(const EventContext& ctx) const
{
  ATH_MSG_VERBOSE("Event slot: "<<ctx.evt());
  SG::ReadHandle<xAOD::TrackParticleContainer> handle(m_trackParticleKey, ctx);
  ATH_MSG_VERBOSE("Got back  "<<handle->size()<<" trackparticles");
  return StatusCode::SUCCESS;
}
