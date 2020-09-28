/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/* Author: Andrii Verbytskyi andrii.verbytskyi@mpp.mpg.de */

#ifndef ATLASHEPMC_OPERATORS_H
#define ATLASHEPMC_OPERATORS_H
#ifdef HEPMC3
// This suboptimal piece of code is here because of the googletest suite.
namespace HepMC3 {
bool operator ==(const HepMC3::GenParticle* a, const std::shared_ptr<HepMC3::GenParticle> b ) {
    return a==b.get();
}
bool operator ==(const std::shared_ptr<HepMC3::GenParticle> a, const HepMC3::GenParticle* const b) {
    return a.get()==b;
}
bool operator ==(const std::shared_ptr<const HepMC3::GenParticle> a, const HepMC3::GenParticle* const b) {
    return a.get()==b;
}
bool operator ==(const HepMC3::GenParticle a, const HepMC3::GenParticle b) {
    return a.id()==b.id();
}
bool operator !=(const std::shared_ptr<HepMC3::GenParticle> a, const HepMC3::GenParticle* const b) {
    return a.get()!=b;
}
bool operator !=(const std::shared_ptr<const HepMC3::GenParticle> a, const HepMC3::GenParticle* const b) {
    return a.get()!=b;
}
bool operator !=(const HepMC3::GenParticle a, const HepMC3::GenParticle b) {
    return a.id()!=b.id();
}
}
#endif
#endif
