/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AlignTRTHit.cxx
// Richard Hawkings, started 7/9/04

#include "InDetAlignTrkInfo/AlignTRTHit.h"

AlignTRTHit::AlignTRTHit() :
  m_det(0), m_mod(0), m_phimod(0), m_layer(0), m_straw(0),
  m_r(0.), m_phi(0.), m_z(0.), m_xdrift(0.), m_tdrift(0.), m_res(0.),
  m_highth(false)
{}
