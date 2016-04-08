/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AlignTrk.cxx
// Richard Hawkings, started 13/4/04

#include <iostream>
#include "InDetAlignTrkInfo/AlignTrk.h"

AlignTrk::AlignTrk() :
  m_nscat(0), m_irun(0), m_ievent(0), m_itrack(0), m_nholes(0), m_nshared(0),m_noverlapPIX(0), m_noverlapSCT(0),
  m_ikine(0), m_ivtx(0)
  
{
  for (int i=0;i<5;++i) {
    m_trkpar[i]=0.;
    m_truetrkpar[i]=-999.;
  }
  for (int i=0;i<15;++i) m_trkcov[i]=0.;
  for (int i=0;i<3;++i) {
   m_trpvtx[i]=0.;
   m_beampos[i]=0.;
   m_beamsigma[i]=0.;
   if (i<2) m_beamtilt[i]=0.;
  }
  m_hits.clear();
  m_trthits.clear();
  //**
  m_overlaphits.clear(); // All elements in m_overlaphits vector are cleared.
}

void AlignTrk::print() const {
  std::cout << "Printing AlignTrk parameters " << "nhits" << m_hits.size() <<
  " a0 " << m_trkpar[0] << " z0 " << m_trkpar[1] << " phi0 " << m_trkpar[2] << 
    " cottheta " << m_trkpar[3] << " Q/pt " << m_trkpar[4] << std::endl;
}

