/* Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
   Author: Andrii Verbytskyi andrii.verbytskyi@mpp.mpg.de
*/
#ifndef ATLASHEPMC_GENPARTICLE_H
#define ATLASHEPMC_GENPARTICLE_H
#include "HepMC/GenParticle.h"
namespace HepMC {
typedef GenParticle* GenParticlePtr;
typedef GenParticle* ConstGenParticlePtr;
inline GenParticlePtr newGenParticlePtr(const HepMC::FourVector &mom = HepMC::FourVector(0.0,0.0,0.0,0.0), int pid = 0, int status = 0) {
    return new HepMC::GenParticle(mom,pid,status);
}
inline int barcode(GenParticle p) {   return    p.barcode(); }
template <class T> inline int barcode(T p) {   return    p->barcode(); }

}
#endif
