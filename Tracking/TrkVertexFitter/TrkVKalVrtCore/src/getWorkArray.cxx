/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVKalVrtCore/WorkArray.h"

namespace Trk {

vkalDynamicArrays::vkalDynamicArrays(long int NTrkMax ) {
    NSIZE  = NTrkMax;
    dphi   = new double[NTrkMax];
    deps   = new double[NTrkMax];
    drho   = new double[NTrkMax];
    dtet   = new double[NTrkMax];
    dzp    = new double[NTrkMax];
}

vkalDynamicArrays::vkalDynamicArrays(const vkalDynamicArrays & src ) {  //copy
    NSIZE=src.NSIZE;
    dphi   = new double[NSIZE];
    deps   = new double[NSIZE];
    drho   = new double[NSIZE];
    dtet   = new double[NSIZE];
    dzp    = new double[NSIZE];
    for(int i=0; i<NSIZE; i++){
      dphi[i]=src.dphi[i];
      deps[i]=src.deps[i];
      drho[i]=src.drho[i];
      dtet[i]=src.dtet[i];
      dzp[i] =src.dzp[i];
    }
}

vkalDynamicArrays& vkalDynamicArrays::operator= (const vkalDynamicArrays & src ) {  //assignment
    if (this!=&src) {
      NSIZE=src.NSIZE;
      if(dphi) delete[] dphi;
      if(deps) delete[] deps;
      if(drho) delete[] drho;
      if(dtet) delete[] dtet;
      if(dzp)  delete[] dzp;
      dphi   = new double[NSIZE];
      deps   = new double[NSIZE];
      drho   = new double[NSIZE];
      dtet   = new double[NSIZE];
      dzp    = new double[NSIZE];
      for(int i=0; i<NSIZE; i++){
	dphi[i]=src.dphi[i];
	deps[i]=src.deps[i];
	drho[i]=src.drho[i];
	dtet[i]=src.dtet[i];
	dzp[i] =src.dzp[i];
      }
    }
    return *this;
}

vkalDynamicArrays::~vkalDynamicArrays() {
    delete[] dphi;
    delete[] deps;
    delete[] drho;
    delete[] dtet;
    delete[] dzp;
}


double *vkalDynamicArrays::get_dphi() const { return dphi;}
double *vkalDynamicArrays::get_deps() const { return deps;}
double *vkalDynamicArrays::get_drho() const { return drho;}
double *vkalDynamicArrays::get_dtet() const { return dtet;}
double *vkalDynamicArrays::get_dzp()  const { return dzp;}



}
