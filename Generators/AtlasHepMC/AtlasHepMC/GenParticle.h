#ifndef ATLASHEPMC_GENPARTICLE_H
#define ATLASHEPMC_GENPARTICLE_H
#include "HepMC/GenParticle.h"
namespace HepMC {
typedef GenParticle* GenParticlePtr;
typedef (GenParticle* const) ConstGenParticlePtr;
inline GenParticlePtr newGenParticlePtr(const HepMC::FourVector &mom = HepMC::FourVector(0.0,0.0,0.0,0.0), int pid = 0, int status = 0) {
    return new HepMC::GenParticle(mom,pid,status);
}
inline int barcode(GenParticlePtr p) {
    p->barcode();
}
inline int barcode(ConstGenParticlePtr p) {
    p->barcode();
}
}
#endif
