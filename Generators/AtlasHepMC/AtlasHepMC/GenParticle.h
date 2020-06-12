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
inline void suggest_barcode(GenParticle p, int i){p.suggest_barcode(i);}
template <class T> void suggest_barcode(T* p, int i){p->suggest_barcode(i);}
namespace Print {
inline void line(std::ostream& os,const GenParticle& p){p.print(os);}
inline void line(std::ostream& os,const GenParticle* p){p->print(os);}
}
}
#endif
