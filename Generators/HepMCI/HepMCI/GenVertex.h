#ifndef HEPMCIGENVERTEX_H
#define HEPMCIGENVERTEX_H
#include "HepMC/GenVertex.h"
namespace HepMC {
typedef HepMC::GenVertex* GenVertexPtr;
typedef (HepMC::GenVertex* const)  ConstGenVertexPtr;
inline GenVertexPtr newGenVertexPtr(const HepMC::FourVector &pos = HepMC::FourVector(0.0,0.0,0.0,0.0), int i=0) {
  return new HepMC::GenVertex(pos,i);
}
inline int barcode(GenVertexPtr p){ p->barcode();}
inline int barcode(ConstGenVertexPtr p){ p->barcode();}
}
#endif
