/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
   */

//////////////////////////////////////////////////////////////////////////////
// TruthParticlesInConeTool
//
//  (c) ATLAS software
//////////////////////////////////////////////////////////////////////////////

#include "TruthParticlesInConeTool.h"

namespace xAOD {

TruthParticlesInConeTool::TruthParticlesInConeTool(const std::string& type,
                                                   const std::string& name,
                                                   const IInterface* parent)
  : AthAlgTool(type, name, parent)
{
  declareInterface<ITruthParticlesInConeTool>(this);
}


StatusCode
TruthParticlesInConeTool::initialize()
{
  ATH_CHECK(m_truthParticleLocation.initialize());
  const std::string tableName = name() + "LookUpTable";
  m_LookUpTableCacheRead = tableName;
  m_LookUpTableCacheWrite = tableName;
  ATH_CHECK(m_LookUpTableCacheRead.initialize());
  ATH_CHECK(m_LookUpTableCacheWrite.initialize());
  return StatusCode::SUCCESS;
}

const TruthParticlesInConeTool::LookUpTable*
TruthParticlesInConeTool::getTable(const EventContext& ctx) const
{
  SG::ReadHandle<LookUpTable> rh(m_LookUpTableCacheRead, ctx);
  if (rh.isValid()) {
    return &*rh;
  }
  SG::ReadHandle<TruthParticleContainer> truthParticles(m_truthParticleLocation,
                                                        ctx);
  if (!truthParticles.isValid()) {
    ATH_MSG_ERROR("Could not open a TruthParticle container with key "
                  << m_truthParticleLocation.key());
    return nullptr;
  }
  auto lut = std::make_unique<LookUpTable>();
  lut->init(*truthParticles);
  SG::WriteHandle<LookUpTable> wh(m_LookUpTableCacheWrite, ctx);
  return wh.put(std::move(lut), true);
}

bool
TruthParticlesInConeTool::particlesInCone(
  const EventContext& ctx,
  float eta,
  float phi,
  float dr,
  std::vector<const TruthParticle*>& output) const
{
  /// initialize if needed
  const LookUpTable* lut = getTable(ctx);
  return lut->iParticlesInCone(eta, phi, dr, output);
}
} // end of namespace
