/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**  @file L2IDTrack.cxx
*
*    authors: Akimasa Ishikawa (akimasa@phys.sci.kobe-u.ac.jp)
*/
#include "TriggerMenuNtuple/L2IDTrack.h"

L2IDTrack::L2IDTrack()
{
  clear();
}

L2IDTrack::~L2IDTrack()
{
}

void
L2IDTrack::setL2IDTrack( int s_algo, float s_pt, float s_ept, float s_eta, float s_eeta, float s_phi0, float s_ephi0,
                     float s_z0, float s_ez0, float s_a0, float s_ea0, float s_chi2,
                     int s_nPIX, int s_nSCT, int s_hitPattern, int s_nStrawHits, int s_nStraw, int s_nStrawTime, int s_nTRHits)
{
  algo = s_algo;
  pt = s_pt;
  ept = s_ept;
  eta = s_eta;
  eeta = s_eeta;
  phi0 = s_phi0;
  ephi0 = s_ephi0;
  z0 = s_z0;
  ez0 = s_ez0;
  a0 = s_a0;
  ea0 = s_ea0;
  chi2 = s_chi2;
  nPIX = s_nPIX;
  nSCT = s_nSCT;
  hitPattern = s_hitPattern;
  nStrawHits = s_nStrawHits;
  nStraw = s_nStraw;
  nStrawTime = s_nStrawTime;
  nTRHits = s_nTRHits;
}

void
L2IDTrack::clear()
{
  algo = -1;
  pt = -1;
  ept = -1;
  eta = -1;
  eeta = -1;
  phi0 = -1;
  ephi0 = -1;
  z0 = -1;
  ez0 = -1;
  a0 = -1;
  ea0 = -1;
  chi2 = -1;
  nPIX = -1;
  nSCT = -1;
  hitPattern = -1;
  nStrawHits = -1;
  nStraw = -1;
  nStrawTime = -1;
  nTRHits = -1;
}
