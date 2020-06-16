/* Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
   Author: Andrii Verbytskyi andrii.verbytskyi@mpp.mpg.de
*/
#ifndef ATLASHEPMC_GENPARTICLE_H
#define ATLASHEPMC_GENPARTICLE_H
#ifdef HEPMC3
#include "HepMC3/GenParticle.h"
#include "HepMC3/PrintStreams.h"
#include "AtlasHepMC/Polarization.h"
#include "AtlasHepMC/Flow.h"
namespace HepMC {
typedef HepMC3::GenParticlePtr GenParticlePtr;
typedef HepMC3::ConstGenParticlePtr ConstGenParticlePtr;
inline GenParticlePtr newGenParticlePtr(const HepMC3::FourVector &mom = HepMC3::FourVector::ZERO_VECTOR(), int pid = 0, int status = 0) {
  return std::make_shared<HepMC3::GenParticle>(mom,pid,status);
}
inline GenParticlePtr newGenParticlePtr(const HepMC3::FourVector &mom, int pid , int status , HepMC::Flow fl, HepMC::Polarization pol) {
  return std::make_shared<HepMC3::GenParticle>(mom,pid,status);
}
inline int barcode(GenParticlePtr p){ 
	std::shared_ptr<HepMC3::IntAttribute> barcode=p->attribute<HepMC3::IntAttribute>("barcode");
		 return barcode?(barcode->value()):0;
}
inline int barcode(ConstGenParticlePtr p){ 
	std::shared_ptr<HepMC3::IntAttribute> barcode=p->attribute<HepMC3::IntAttribute>("barcode");
		 return barcode?(barcode->value()):0;
}
inline int barcode(const HepMC3::GenParticle p){ 
	std::shared_ptr<HepMC3::IntAttribute> barcode=p.attribute<HepMC3::IntAttribute>("barcode");
		 return barcode?(barcode->value()):0;
}
template <class T> void suggest_barcode(T p, int i){p->add_attribute("barcode",std::make_shared<HepMC3::IntAttribute>(i));}
}
#else
#include "HepMC/GenParticle.h"
namespace HepMC {
typedef GenParticle* GenParticlePtr;
typedef GenParticle* ConstGenParticlePtr;
inline GenParticlePtr newGenParticlePtr(const HepMC::FourVector &mom = HepMC::FourVector(0.0,0.0,0.0,0.0), int pid = 0, int status = 0) {
    return new HepMC::GenParticle(mom,pid,status);
}
inline int barcode(GenParticle p) {   return    p.barcode(); }
template <class T> inline int barcode(T p) {   return    p->barcode(); }
template <class T> void suggest_barcode(T p, int i){p->suggest_barcode(i);}
inline void suggest_barcode(GenParticle p, int i){p.suggest_barcode(i);}
template <class T> void suggest_barcode(T* p, int i){p->suggest_barcode(i);}
namespace Print {
inline void line(std::ostream& os,const GenParticle& p){p.print(os);}
inline void line(std::ostream& os,const GenParticle* p){p->print(os);}
}
}
#endif
#endif
