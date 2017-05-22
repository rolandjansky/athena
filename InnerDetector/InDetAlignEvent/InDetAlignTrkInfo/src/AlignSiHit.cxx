/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AlignSiHit.cxx
// Richard Hawkings, stared 7/9/04

#include "InDetAlignTrkInfo/AlignSiHit.h"

AlignSiHit::AlignSiHit() :
  m_index(-1), m_chan1(0), m_chan2(0), m_clusw(0), m_clusl(0), m_cluss(0),
  m_status(0),
  //new variables to AlignSiHit - in progress
  m_isOverlap(false),
  m_stereo(0.), m_rphi_resid(0.), m_z_resid(0.), m_erphi_resid(0.), 
  m_ez_resid(0.), m_trkExtrapLocalXError(0.), m_trkExtrapLocalYError(0.),
  m_r(0.), m_phi(0.), m_z(0.), m_delzr(0.), m_sinlocal(0.), m_trklen(0.)
 
{
  // default constructor - set all to safe values via init list except arrays
  for (int i=0;i<2;++i) {
    m_locang[i]=0.;
    m_locdel[i]=0.;
    m_locedel[i]=0.;
    m_globang[i]=0.;
    m_globdel[i]=0.;
    m_globedel[i]=0.;
  }
  for (int i=0;i<5;++i) {
    m_dtpar1[i]=0.;
    m_dtpar2[i]=0.;
  }
}
