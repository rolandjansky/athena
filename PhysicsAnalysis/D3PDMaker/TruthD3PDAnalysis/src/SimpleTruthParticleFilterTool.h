/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHD3PDANALYSIS_SIMPLETRUTHPARTICLEFILTERTOOL_H
#define TRUTHD3PDANALYSIS_SIMPLETRUTHPARTICLEFILTERTOOL_H

#include "TruthD3PDAnalysis/TruthParticleFilterBase.h"
#include "AtlasHepMC/GenParticle_fwd.h"

namespace D3PD {

/**
 * @brief Filter truth particles for writing to D3PD.
 *
 * This is used as part of @c TruthParticleFaker.
 *
 * This tool retrieves a @c McEventCollection, filters it, and
 * writes a new one.
 *
 * The @c TruthParticleBuilder will then turn the filtered
 * @c McEventCollection into a @c TruthParticleContainer.
 */
class SimpleTruthParticleFilterTool
  : public TruthParticleFilterBase
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  SimpleTruthParticleFilterTool (const std::string& type,
                           const std::string& name,
                           const IInterface* parent);

  /// Test to see if we want to keep a particle.
  virtual bool isAccepted (const HepMC::GenParticle* p);

private:
  /// Parameter: PDG ID to filter on
  int m_filterID;

  /// Parameter: min pT to keep particle
  float m_minPt;
};

} // namespace D3PD

#endif // not TRUTHD3PDANALYSIS_SIMPLETRUTHPARTICLEFILTERTOOL_H
