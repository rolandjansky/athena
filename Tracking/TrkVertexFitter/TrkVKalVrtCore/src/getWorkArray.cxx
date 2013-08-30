/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVKalVrtCore/WorkArray.h"

namespace Trk {

vkalDynamicArrays::vkalDynamicArrays(long int NTrkMax ) {

    dphi   = new double[NTrkMax];
    deps   = new double[NTrkMax];
    drho   = new double[NTrkMax];
    dtet   = new double[NTrkMax];
    dzp    = new double[NTrkMax];
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
