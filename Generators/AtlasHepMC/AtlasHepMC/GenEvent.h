/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/* Author: Andrii Verbytskyi andrii.verbytskyi@mpp.mpg.de */

#ifndef ATLASHEPMC_GENEVENT_H
#define ATLASHEPMC_GENEVENT_H
#ifdef HEPMC3
#undef private
#undef protected
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenHeavyIon.h"
#include "HepMC3/PrintStreams.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/SimpleVector.h"
namespace HepMC3
{
inline std::vector<HepMC3::GenParticlePtr>::const_iterator  begin(HepMC3::GenEvent& e){ return e.particles().begin(); }
inline std::vector<HepMC3::GenParticlePtr>::const_iterator  end(HepMC3::GenEvent& e){ return e.particles().end(); }
inline std::vector<HepMC3::ConstGenParticlePtr>::const_iterator  begin(const HepMC3::GenEvent& e){ return e.particles().begin(); }
inline std::vector<HepMC3::ConstGenParticlePtr>::const_iterator  end(const HepMC3::GenEvent& e){ return e.particles().end(); }
}

namespace HepMC {
using Print=HepMC3::Print;
using GenHeavyIon=HepMC3::GenHeavyIon;
using GenEvent=HepMC3::GenEvent;

inline std::vector<HepMC3::GenParticlePtr>::const_iterator  begin(HepMC3::GenEvent& e){ return e.particles().begin(); }
inline std::vector<HepMC3::GenParticlePtr>::const_iterator  end(HepMC3::GenEvent& e){ return e.particles().end(); }
inline std::vector<HepMC3::ConstGenParticlePtr>::const_iterator  begin(const HepMC3::GenEvent& e){ return e.particles().begin(); }
inline std::vector<HepMC3::ConstGenParticlePtr>::const_iterator  end(const HepMC3::GenEvent& e){ return e.particles().end(); }

inline GenEvent* newGenEvent(const int signal_process_id, const int event_number ){ GenEvent* e= new GenEvent(); 
std::shared_ptr<HepMC3::IntAttribute> signal_process_id_A = std::make_shared<HepMC3::IntAttribute>(signal_process_id);
e->add_attribute("signal_process_id",signal_process_id_A); 
e->set_event_number(event_number);
return e;
}

inline GenVertexPtr  barcode_to_vertex(const GenEvent* e, int id ){
auto vertices=((GenEvent*)e)->vertices();
for (auto v: vertices)  
{
auto barcode_attr=e->attribute<HepMC3::IntAttribute>("barcode");
if (!barcode_attr) continue;
if (barcode_attr->value()==id) return v;
}
if (-id>0&&-id<=(int)vertices.size()) return vertices[-id-1];
return  HepMC3::GenVertexPtr(); 
}

inline GenParticlePtr  barcode_to_particle(const GenEvent* e, int id ){
auto particles=((GenEvent*)e)->particles();
for (auto p: particles)  
{
auto barcode_attr=p->attribute<HepMC3::IntAttribute>("barcode");
if (!barcode_attr) continue;
if (barcode_attr->value()==id) return p;
}
if (id>0&&id<=(int)particles.size()) return particles[id-1];
return  HepMC3::GenParticlePtr(); 
}


inline int signal_process_id(const GenEvent evt) {
std::shared_ptr<HepMC3::IntAttribute> A_signal_process_id=evt.attribute<HepMC3::IntAttribute>("signal_process_id");
 return A_signal_process_id?(A_signal_process_id->value()):0;
}
inline int signal_process_id(const GenEvent* evt) {
std::shared_ptr<HepMC3::IntAttribute> A_signal_process_id=evt->attribute<HepMC3::IntAttribute>("signal_process_id");
 return A_signal_process_id?(A_signal_process_id->value()):0;
}
inline void set_signal_process_id(GenEvent* e, const int i=0) {     std::shared_ptr<HepMC3::IntAttribute> signal_process_id = std::make_shared<HepMC3::IntAttribute>(i);
                                                                    e->add_attribute("signal_process_id",signal_process_id);  }
inline void set_random_states(GenEvent* e, std::vector<long int>& a)  { 
 e->add_attribute("random_states",std::make_shared<HepMC3::VectorLongIntAttribute>(a));
}
template <class T> void set_signal_process_vertex(GenEvent* e, T v){
if (!v) return;
if (v->parent_event()!=e) return;
v->add_attribute("signal_process_vertex",std::make_shared<HepMC3::IntAttribute>(1));
}
inline ConstGenVertexPtr signal_process_vertex(const GenEvent* e) { for (auto v: e->vertices()) if (v->attribute<HepMC3::IntAttribute>("signal_process_vertex")) return v; return nullptr; }
inline      GenVertexPtr signal_process_vertex(GenEvent* e) { for (auto v: e->vertices()) if (v->attribute<HepMC3::IntAttribute>("signal_process_vertex")) return v; return nullptr; }
inline bool valid_beam_particles(const GenEvent* e){ if (!e) return false; if  (e->beams().size()!=2) return false; return true;}
}
#else
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "AtlasHepMC/GenVertex.h"
namespace HepMC {
inline GenEvent::particle_iterator  begin(HepMC::GenEvent& e){ return e.particles_begin(); }
inline GenEvent::particle_iterator  end(HepMC::GenEvent& e){ return e.particles_end(); }
inline GenEvent::particle_const_iterator  begin(const HepMC::GenEvent& e){ return e.particles_begin(); }
inline GenEvent::particle_const_iterator  end(const HepMC::GenEvent& e){ return e.particles_end(); }
inline GenEvent* newGenEvent(const int a, const int b ){ return new GenEvent(a,b); }
inline GenVertex* signal_process_vertex(const GenEvent* e) { return e->signal_process_vertex(); }
inline GenVertex* barcode_to_vertex(const GenEvent* e, int id ){return  e->barcode_to_vertex(id);}
inline GenParticle* barcode_to_particle(const GenEvent* e, int id ){return  e->barcode_to_particle(id);}
inline int signal_process_id(const GenEvent e) {
    return e.signal_process_id();
}
inline int signal_process_id(const GenEvent* e) {
    return e->signal_process_id();
}
inline void set_signal_process_id(GenEvent* e, const int i) {
    e->set_signal_process_id(i);
}
template <class T> void set_random_states(GenEvent* e, std::vector<T> a) {
    e->set_random_states(a);
}
template <class T> void set_signal_process_vertex(GenEvent* e, T v) {
    e->set_signal_process_vertex(v);
}
namespace Print {
inline void line(std::ostream& os,const GenEvent& e){e.print(os);}
inline void line(std::ostream& os,const GenEvent* e){e->print(os);}
}
inline bool valid_beam_particles(const GenEvent* e){return e->valid_beam_particles();}
}
#include "AtlasHepMC/SimpleVector.h"
#endif
#endif
