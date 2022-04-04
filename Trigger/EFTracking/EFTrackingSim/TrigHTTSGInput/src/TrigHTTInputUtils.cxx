/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTSGInput/TrigHTTInputUtils.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/GenParticle.h"
namespace TrigHTTInputUtils {

  const ParentBitmask  construct_truth_bitmap(HepMC::ConstGenParticlePtr particle)
  {
    ParentBitmask result;
    result.reset();
    typedef std::pair<HepMC::ConstGenParticlePtr, unsigned int> Parent;
    std::vector<Parent> parents;
    parents.push_back(Parent(particle, 0));
    while (!parents.empty()) {
      HepMC::ConstGenParticlePtr p = parents.back().first;
      const unsigned int level = parents.back().second;
      if (std::abs(p->pdg_id()) == 15) { result.set(TAU_PARENT_BIT, 1); }
      if (std::abs(p->pdg_id()) == 5) { result.set(B_PARENT_BIT, 1); }
      if (std::abs(p->pdg_id()) == 211) { result.set(PION_PARENT_BIT, 1); }
      if (std::abs(p->pdg_id()) == 211 && level <= 1) { result.set(PION_IMMEDIATE_PARENT_BIT, 1); }
      if (result.count() == NBITS) { break; }
      parents.pop_back();
      if (!(p->production_vertex())) { continue; }
#ifdef HEPMC3
      for (HepMC::ConstGenParticlePtr i: p->production_vertex()->particles_in() ) {
        parents.push_back(Parent(i, level + 1));
      }
#else
      for (HepMC::GenVertex::particle_iterator i = p->production_vertex()->particles_begin(HepMC::parents), f = p->production_vertex()->particles_end(HepMC::parents); i != f; ++i) {
        parents.push_back(Parent(*i, level + 1));
      }

#endif
    }
    return result;
  }


}
