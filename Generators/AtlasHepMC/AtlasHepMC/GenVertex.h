/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/* Author: Andrii Verbytskyi andrii.verbytskyi@mpp.mpg.de */

#ifndef ATLASHEPMC_GENVERTEX_H
#define ATLASHEPMC_GENVERTEX_H
#ifdef HEPMC3
#include "HepMC3/GenVertex.h"
#include "HepMC3/PrintStreams.h"
namespace HepMC3
{
inline std::vector<HepMC3::ConstGenParticlePtr>::const_iterator  begin(const HepMC3::GenVertex& v){ return v.particles_out().begin(); }
inline std::vector<HepMC3::ConstGenParticlePtr>::const_iterator  end(const HepMC3::GenVertex& v){ return v.particles_out().end(); }
}
namespace HepMC {
typedef HepMC3::GenVertexPtr GenVertexPtr;
typedef HepMC3::ConstGenVertexPtr ConstGenVertexPtr;
inline GenVertexPtr newGenVertexPtr(const HepMC3::FourVector &pos = HepMC3::FourVector::ZERO_VECTOR(),const int i=0) {
  GenVertexPtr v=std::make_shared<HepMC3::GenVertex>(pos);
  v->set_status(i);
  return v;
}
inline int barcode(GenVertexPtr p){
	if (!p) return 0; 
	std::shared_ptr<HepMC3::IntAttribute> barcode=p->attribute<HepMC3::IntAttribute>("barcode");
		 return barcode?(barcode->value()):p->id();
}
inline int barcode(ConstGenVertexPtr p){ 
	if (!p) return 0;
	std::shared_ptr<HepMC3::IntAttribute> barcode=p->attribute<HepMC3::IntAttribute>("barcode");
		 return barcode?(barcode->value()):p->id();
}
inline int barcode(HepMC3::GenVertex p){ 
	std::shared_ptr<HepMC3::IntAttribute> barcode=p.attribute<HepMC3::IntAttribute>("barcode");
		 return barcode?(barcode->value()):p.id();
}

inline void* raw_pointer(GenVertexPtr p){ return p.get();}
inline const void* raw_pointer(ConstGenVertexPtr p){ return p.get();}
using HepMC3::GenVertex;
}
#else
#include "HepMC/GenVertex.h"
namespace HepMC {
typedef HepMC::GenVertex* GenVertexPtr;
typedef const HepMC::GenVertex* ConstGenVertexPtr;
inline GenVertex::particles_out_const_iterator  begin(const HepMC::GenVertex& v){ return v.particles_out_const_begin(); }
inline GenVertex::particles_out_const_iterator  end(const HepMC::GenVertex& v){ return v.particles_out_const_end(); }

inline GenVertexPtr newGenVertexPtr(const HepMC::FourVector &pos = HepMC::FourVector(0.0,0.0,0.0,0.0), const int i=0) {
    return new HepMC::GenVertex(pos,i);
}
namespace Print {
inline void line(std::ostream& os,const GenVertex& v){v.print(os);}
inline void line(std::ostream& os,const GenVertex* v){v->print(os);}
}
inline int barcode(ConstGenVertexPtr p){ return p->barcode();}
inline void* raw_pointer(GenVertexPtr p){ return p;}
inline const void* raw_pointer(ConstGenVertexPtr p){ return p;}
inline std::ostream& operator<<( std::ostream& os, const GenVertex* v ) { if (v) return os<<(*v); else return os;} 
}
#endif
#endif
