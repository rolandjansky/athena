/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVKalVrtCore/WorkArray.h"

namespace Trk {

vkalDynamicArrays::vkalDynamicArrays(long int NTrkMax ) {
    m_NSIZE  = NTrkMax;
    m_dphi   = new double[NTrkMax];
    m_deps   = new double[NTrkMax];
    m_drho   = new double[NTrkMax];
    m_dtet   = new double[NTrkMax];
    m_dzp    = new double[NTrkMax];
}

vkalDynamicArrays::vkalDynamicArrays(const vkalDynamicArrays & src ) {  //copy
    m_NSIZE=src.m_NSIZE;
    m_dphi   = new double[m_NSIZE];
    m_deps   = new double[m_NSIZE];
    m_drho   = new double[m_NSIZE];
    m_dtet   = new double[m_NSIZE];
    m_dzp    = new double[m_NSIZE];
    for(int i=0; i<m_NSIZE; i++){
      m_dphi[i]=src.m_dphi[i];
      m_deps[i]=src.m_deps[i];
      m_drho[i]=src.m_drho[i];
      m_dtet[i]=src.m_dtet[i];
      m_dzp[i] =src.m_dzp[i];
    }
}

vkalDynamicArrays& vkalDynamicArrays::operator= (const vkalDynamicArrays & src ) {  //assignment
    if (this!=&src) {
      m_NSIZE=src.m_NSIZE;
      if(m_dphi) delete[] m_dphi;
      if(m_deps) delete[] m_deps;
      if(m_drho) delete[] m_drho;
      if(m_dtet) delete[] m_dtet;
      if(m_dzp)  delete[] m_dzp;
      m_dphi   = new double[m_NSIZE];
      m_deps   = new double[m_NSIZE];
      m_drho   = new double[m_NSIZE];
      m_dtet   = new double[m_NSIZE];
      m_dzp    = new double[m_NSIZE];
      for(int i=0; i<m_NSIZE; i++){
	m_dphi[i]=src.m_dphi[i];
	m_deps[i]=src.m_deps[i];
	m_drho[i]=src.m_drho[i];
	m_dtet[i]=src.m_dtet[i];
	m_dzp[i] =src.m_dzp[i];
      }
    }
    return *this;
}

vkalDynamicArrays::~vkalDynamicArrays() {
    delete[] m_dphi;
    delete[] m_deps;
    delete[] m_drho;
    delete[] m_dtet;
    delete[] m_dzp;
}


double *vkalDynamicArrays::get_dphi() const { return m_dphi;}
double *vkalDynamicArrays::get_deps() const { return m_deps;}
double *vkalDynamicArrays::get_drho() const { return m_drho;}
double *vkalDynamicArrays::get_dtet() const { return m_dtet;}
double *vkalDynamicArrays::get_dzp()  const { return m_dzp;}



}
