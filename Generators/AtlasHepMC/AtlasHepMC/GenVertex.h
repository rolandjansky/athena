/* Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
   Author: Andrii Verbytskyi andrii.verbytskyi@mpp.mpg.de
*/
#ifndef ATLASHEPMC_GENVERTEX_H
#define ATLASHEPMC_GENVERTEX_H
#ifdef HEPMC3
#include "HepMC3/GenVertex.h"
#include "HepMC3/PrintStreams.h"
namespace HepMC {
typedef HepMC3::GenVertexPtr GenVertexPtr;
typedef HepMC3::ConstGenVertexPtr ConstGenVertexPtr;
inline GenVertexPtr newGenVertexPtr(const HepMC3::FourVector &pos = HepMC3::FourVector::ZERO_VECTOR(),const int i=0) {
  GenVertexPtr v=std::make_shared<HepMC3::GenVertex>(pos);
  v->set_status(i);
  return v;
}
inline int barcode(GenVertexPtr p){ 
	std::shared_ptr<HepMC3::IntAttribute> barcode=p->attribute<HepMC3::IntAttribute>("barcode");
		 return barcode?(barcode->value()):0;
}
inline int barcode(ConstGenVertexPtr p){ 
	std::shared_ptr<HepMC3::IntAttribute> barcode=p->attribute<HepMC3::IntAttribute>("barcode");
		 return barcode?(barcode->value()):0;
}
inline int barcode(HepMC3::GenVertex p){ 
	std::shared_ptr<HepMC3::IntAttribute> barcode=p.attribute<HepMC3::IntAttribute>("barcode");
		 return barcode?(barcode->value()):0;
}

inline std::vector<HepMC3::GenVertexPtr> DESCENDANTS(HepMC3::GenVertexPtr endvtx)
{
return std::vector<HepMC3::GenVertexPtr>();	
}
inline std::vector<HepMC3::ConstGenVertexPtr> DESCENDANTS(HepMC3::ConstGenVertexPtr endvtx)
{
return std::vector<HepMC3::ConstGenVertexPtr>();	
}
inline void* raw_pointer(GenVertexPtr p){ return p.get();}
}
#else
#include "HepMC/GenVertex.h"
namespace HepMC {
typedef HepMC::GenVertex* GenVertexPtr;
typedef HepMC::GenVertex* const  ConstGenVertexPtr;
inline GenVertexPtr newGenVertexPtr(const HepMC::FourVector &pos = HepMC::FourVector(0.0,0.0,0.0,0.0), const int i=0) {
    return new HepMC::GenVertex(pos,i);
}
inline int barcode(ConstGenVertexPtr p){ return p->barcode();}
inline void* raw_pointer(GenVertexPtr p){ return p;}
}
#endif
#endif
