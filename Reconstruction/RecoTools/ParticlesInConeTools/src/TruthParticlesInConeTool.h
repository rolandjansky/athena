/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef PARTICLESINCONETOOLS_TRUTHPARTICLESINCONETOOL_H
#define PARTICLESINCONETOOLS_TRUTHPARTICLESINCONETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/ReadHandleKey.h"

#include "IParticlesLookUpTable.h"
#include "ParticlesInConeTools/ITruthParticlesInConeTool.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"

namespace xAOD {

class TruthParticlesInConeTool
  : public AthAlgTool
  , virtual public ITruthParticlesInConeTool
{
public:
  /** constructor */
  TruthParticlesInConeTool(const std::string& type,
                           const std::string& name,
                           const IInterface* parent);

  /** destructor */
  virtual ~TruthParticlesInConeTool() = default;

  /** initialize */
  virtual StatusCode initialize() override final;

  /**ITruthParticlesInConeTool interface */
  virtual bool particlesInCone(
    const EventContext& ctx,
    float eta,
    float phi,
    float dr,
    std::vector<const TruthParticle*>& output) const override final;

  typedef IParticlesLookUpTable<TruthParticle> LookUpTable;

private:
  // init look-up table
  const LookUpTable* getTable(const EventContext& ctx) const;

  /** Truth Particle collection name */
  SG::ReadHandleKey<TruthParticleContainer> m_truthParticleLocation{
    this,
    "TruthParticleLocation",
    "TruthParticles"
  };

  SG::ReadHandleKey<LookUpTable> m_LookUpTableCacheRead{ "LookUpTable" };
  SG::WriteHandleKey<LookUpTable> m_LookUpTableCacheWrite{ "LookUpTable" };
};
} // end of namespace

CLASS_DEF(xAOD::TruthParticlesInConeTool::LookUpTable, 151156931, 0)
#endif

