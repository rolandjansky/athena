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
#include "HepMC3/GenPdfInfo.h"
#include "HepMC3/PrintStreams.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/SimpleVector.h"
namespace HepMC3 {
inline std::vector<HepMC3::GenParticlePtr>::const_iterator  begin(HepMC3::GenEvent& e) { return e.particles().begin(); }
inline std::vector<HepMC3::GenParticlePtr>::const_iterator  end(HepMC3::GenEvent& e) { return e.particles().end(); }
inline std::vector<HepMC3::ConstGenParticlePtr>::const_iterator  begin(const HepMC3::GenEvent& e) { return e.particles().begin(); }
inline std::vector<HepMC3::ConstGenParticlePtr>::const_iterator  end(const HepMC3::GenEvent& e) { return e.particles().end(); }
}

namespace HepMC {
using Print=HepMC3::Print;
using GenHeavyIon=HepMC3::GenHeavyIon;
using GenEvent=HepMC3::GenEvent;

inline std::vector<HepMC3::GenParticlePtr>::const_iterator  begin(HepMC3::GenEvent& e) { return e.particles().begin(); }
inline std::vector<HepMC3::GenParticlePtr>::const_iterator  end(HepMC3::GenEvent& e) { return e.particles().end(); }
inline std::vector<HepMC3::ConstGenParticlePtr>::const_iterator  begin(const HepMC3::GenEvent& e) { return e.particles().begin(); }
inline std::vector<HepMC3::ConstGenParticlePtr>::const_iterator  end(const HepMC3::GenEvent& e) { return e.particles().end(); }

inline GenEvent* newGenEvent(const int signal_process_id, const int event_number ) {
    GenEvent* e= new GenEvent();
    std::shared_ptr<HepMC3::IntAttribute> signal_process_id_A = std::make_shared<HepMC3::IntAttribute>(signal_process_id);
    e->add_attribute("signal_process_id",signal_process_id_A);
    e->set_event_number(event_number);
    return e;
}

inline GenEvent* copyemptyGenEvent(const GenEvent* inEvt) {
  GenEvent* e= new GenEvent();
  e->set_event_number(inEvt->event_number());
  e->weights()=inEvt->weights();
  auto a_mpi = inEvt->attribute<HepMC3::IntAttribute>("mpi"); 
  if (a_mpi) e->add_attribute("mpi",std::make_shared<HepMC3::IntAttribute>(*a_mpi));
  auto a_signal_process_id = inEvt->attribute<HepMC3::IntAttribute>("signal_process_id");
  if (a_signal_process_id) e->add_attribute("signal_process_id",std::make_shared<HepMC3::IntAttribute>(*a_signal_process_id));
  auto a_event_scale = inEvt->attribute<HepMC3::DoubleAttribute>("event_scale");
  if (a_event_scale) e->add_attribute("event_scale",std::make_shared<HepMC3::DoubleAttribute>(*a_event_scale));
  auto a_alphaQCD = inEvt->attribute<HepMC3::DoubleAttribute>("alphaQCD");
  if (a_alphaQCD) e->add_attribute("alphaQCD",std::make_shared<HepMC3::DoubleAttribute>(*a_alphaQCD));
  auto a_alphaQED = inEvt->attribute<HepMC3::DoubleAttribute>("alphaQED");
  if (a_alphaQED) e->add_attribute("alphaQED",std::make_shared<HepMC3::DoubleAttribute>(*a_alphaQED));
  auto a_pi = inEvt->pdf_info(); 
  if (a_pi) e->set_pdf_info(std::make_shared<HepMC3::GenPdfInfo>(*a_pi));
  auto a_hi = inEvt->heavy_ion(); 
  if (a_hi) e->set_heavy_ion(std::make_shared<HepMC3::GenHeavyIon>(*a_hi));
  auto a_random_states = inEvt->attribute<HepMC3::VectorLongIntAttribute>("random_states");
  if (a_random_states) e->add_attribute("random_states",std::make_shared<HepMC3::VectorLongIntAttribute>(*a_random_states));
  return e;
}

inline ConstGenVertexPtr  barcode_to_vertex(const GenEvent* e, int id ) {
    auto vertices=e->vertices();
    for (auto v: vertices) {
        auto barcode_attr=e->attribute<HepMC3::IntAttribute>("barcode");
        if (!barcode_attr) continue;
        if (barcode_attr->value()==id) return v;
    }
    if (-id>0&&-id<=(int)vertices.size()) return vertices[-id-1];
    return  HepMC3::ConstGenVertexPtr();
}

inline ConstGenParticlePtr  barcode_to_particle(const GenEvent* e, int id ) {
    auto particles=e->particles();
    for (auto p: particles) {
        auto barcode_attr=p->attribute<HepMC3::IntAttribute>("barcode");
        if (!barcode_attr) continue;
        if (barcode_attr->value()==id) return p;
    }
    if (id>0&&id<=(int)particles.size()) return particles[id-1];
    return  HepMC3::ConstGenParticlePtr();
}

inline GenVertexPtr  barcode_to_vertex(GenEvent* e, int id ) {
    auto vertices=e->vertices();
    for (auto v: vertices) {
        auto barcode_attr=v->attribute<HepMC3::IntAttribute>("barcode");
        if (!barcode_attr) continue;
        if (barcode_attr->value()==id) return v;
    }
    if (-id>0&&-id<=(int)vertices.size()) return vertices[-id-1];
    return  HepMC3::GenVertexPtr();
}

inline GenParticlePtr  barcode_to_particle(GenEvent* e, int id ) {
    auto particles=e->particles();
    for (auto p: particles) {
        auto barcode_attr=p->attribute<HepMC3::IntAttribute>("barcode");
        if (!barcode_attr) continue;
        if (barcode_attr->value()==id) return p;
    }
    if (id>0&&id<=(int)particles.size()) return particles[id-1];
    return  HepMC3::GenParticlePtr();
}

inline int mpi(const GenEvent evt) {
    std::shared_ptr<HepMC3::IntAttribute> A_mpi=evt.attribute<HepMC3::IntAttribute>("mpi");
    return A_mpi?(A_mpi->value()):0;
}
inline int mpi(const GenEvent* evt) {
    std::shared_ptr<HepMC3::IntAttribute> A_mpi=evt->attribute<HepMC3::IntAttribute>("mpi");
    return A_mpi?(A_mpi->value()):0;
}

inline int signal_process_id(const GenEvent evt) {
    std::shared_ptr<HepMC3::IntAttribute> A_signal_process_id=evt.attribute<HepMC3::IntAttribute>("signal_process_id");
    return A_signal_process_id?(A_signal_process_id->value()):0;
}
inline int signal_process_id(const GenEvent* evt) {
    std::shared_ptr<HepMC3::IntAttribute> A_signal_process_id=evt->attribute<HepMC3::IntAttribute>("signal_process_id");
    return A_signal_process_id?(A_signal_process_id->value()):0;
}
inline void set_signal_process_id(GenEvent* e, const int i=0) {
    std::shared_ptr<HepMC3::IntAttribute> signal_process_id = std::make_shared<HepMC3::IntAttribute>(i);
    e->add_attribute("signal_process_id",signal_process_id);
}
inline void set_mpi(GenEvent* e, const int i=0) {
    std::shared_ptr<HepMC3::IntAttribute> mpi = std::make_shared<HepMC3::IntAttribute>(i);
    e->add_attribute("mpi",mpi);
}
inline void set_random_states(GenEvent* e, std::vector<long int>& a) {
    e->add_attribute("random_states",std::make_shared<HepMC3::VectorLongIntAttribute>(a));
}
template <class T> void set_signal_process_vertex(GenEvent* e, T v) {
    if (!v || !e) return;
/* AV: HepMC2 adds the vertex to event */
    e->add_vertex(v);
    v->add_attribute("signal_process_vertex",std::make_shared<HepMC3::IntAttribute>(1));
}
inline ConstGenVertexPtr signal_process_vertex(const GenEvent* e) { for (auto v: e->vertices()) if (v->attribute<HepMC3::IntAttribute>("signal_process_vertex")) return v; return nullptr; }
inline      GenVertexPtr signal_process_vertex(GenEvent* e) { for (auto v: e->vertices()) if (v->attribute<HepMC3::IntAttribute>("signal_process_vertex")) return v; return nullptr; }
inline bool valid_beam_particles(const GenEvent* e) { if (!e) return false; if  (e->beams().size()!=2) return false; return true;}
}
#else
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "AtlasHepMC/GenVertex.h"
namespace HepMC {
inline GenEvent::particle_iterator  begin(HepMC::GenEvent& e) { return e.particles_begin(); }
inline GenEvent::particle_iterator  end(HepMC::GenEvent& e) { return e.particles_end(); }
inline GenEvent::particle_const_iterator  begin(const HepMC::GenEvent& e) { return e.particles_begin(); }
inline GenEvent::particle_const_iterator  end(const HepMC::GenEvent& e) { return e.particles_end(); }
inline GenEvent* newGenEvent(const int a, const int b ) { return new GenEvent(a,b); }
inline GenVertex* signal_process_vertex(const GenEvent* e) { return e->signal_process_vertex(); }
inline GenVertex* barcode_to_vertex(const GenEvent* e, int id ) {return  e->barcode_to_vertex(id);}
inline GenParticle* barcode_to_particle(const GenEvent* e, int id ) {return  e->barcode_to_particle(id);}
inline int mpi(const GenEvent e) {
    return e.mpi();
}
inline int mpi(const GenEvent* e) {
    return e->mpi();
}
inline int signal_process_id(const GenEvent e) {
    return e.signal_process_id();
}
inline int signal_process_id(const GenEvent* e) {
    return e->signal_process_id();
}
inline void set_signal_process_id(GenEvent* e, const int i) {
    e->set_signal_process_id(i);
}
inline void set_mpi(GenEvent* e, const int i) {
    e->set_mpi(i);
}
template <class T> void set_random_states(GenEvent* e, std::vector<T> a) {
    e->set_random_states(a);
}
template <class T> void set_signal_process_vertex(GenEvent* e, T v) {
    e->set_signal_process_vertex(v);
}
inline GenEvent* copyemptyGenEvent(const GenEvent* inEvt) {
    HepMC::GenEvent* outEvt = new HepMC::GenEvent( inEvt->signal_process_id(),  inEvt->event_number() );
    outEvt->set_mpi  ( inEvt->mpi() );
    outEvt->set_event_scale  ( inEvt->event_scale() );
    outEvt->set_alphaQCD     ( inEvt->alphaQCD() );
    outEvt->set_alphaQED     ( inEvt->alphaQED() );
    outEvt->weights() =        inEvt->weights();
    outEvt->set_random_states( inEvt->random_states() );
    if ( nullptr != inEvt->heavy_ion() ) {
      outEvt->set_heavy_ion    ( *inEvt->heavy_ion() );
    }
    if ( nullptr != inEvt->pdf_info() ) {
      outEvt->set_pdf_info     ( *inEvt->pdf_info() );
    }
    return outEvt;
}

namespace Print {
inline void line(std::ostream& os,const GenEvent& e) {e.print(os);}
inline void line(std::ostream& os,const GenEvent* e) {e->print(os);}
}
inline bool valid_beam_particles(const GenEvent* e) {return e->valid_beam_particles();}
}
#include "AtlasHepMC/SimpleVector.h"
#endif
#endif
