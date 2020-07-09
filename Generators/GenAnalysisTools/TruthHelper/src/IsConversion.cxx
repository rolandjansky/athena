/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthHelper/IsConversion.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"
#include <vector>

namespace TruthHelper {


  bool IsConversion::operator()(HepMC::ConstGenParticlePtr  part ) const {
    if (part->pdg_id() != 22) return false;
    if (!part->end_vertex()) return false;

    // Ensure that the children are an e+/e- pair
    int mult = 1;
#ifdef HEPMC3
    if (part->end_vertex()->particles_out().size() != 2) return false;
    for (auto child: part->end_vertex()->particles_out()) {
      if (std::abs(child->pdg_id()) != 11) return false;
      mult *= child->pdg_id();
    }
#else
    if (part->end_vertex()->particles_out_size() != 2) return false;
    HepMC::GenVertex::particle_iterator child = part->end_vertex()->particles_begin(HepMC::children);
    HepMC::GenVertex::particle_iterator childE = part->end_vertex()->particles_end(HepMC::children);
    for (; child != childE; ++child) {
      if (abs((*child)->pdg_id()) != 11) return false;
      mult *= (*child)->pdg_id();
    }
#endif
    if (mult != -11*11) return false;
    return true;
  }


  GenIMCselector* IsConversion::create() const {
    return new IsConversion(*this);
  }


}
