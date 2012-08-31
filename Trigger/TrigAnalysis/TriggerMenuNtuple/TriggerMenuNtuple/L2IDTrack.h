/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**  @file L2IDTrack.h
*
*    authors: Akimasa Ishikawa (akimasa@phys.sci.kobe-u.ac.jp)
*/
#ifndef __L2_ID_Track_H__
#define __L2_ID_Track_H__


class L2IDTrack
{
public:
  L2IDTrack ();
  virtual ~L2IDTrack ();

  //set functions
  // to avoid dependencies with other classes all variables are explicitly passed.
  void setL2IDTrack( int s_algo, float s_pt, float s_ept, float s_eta, float s_eeta, float s_phi0, float s_ephi0,
                     float s_z0, float s_ez0, float s_a0, float s_ea0, float s_chi2,
                     int s_nPIX, int s_nSCT, int s_hitPattern, int s_nStrawHits, int s_Straw, int s_StrawTime, int s_nTRHits);

  //get functions

  //clear functions
  void clear();

public:
  int   algo;
  float pt;
  float ept;
  float eta;
  float eeta;
  float phi0;
  float ephi0;
  float z0;
  float ez0;
  float a0;
  float ea0;
  float chi2;
  int   nPIX;
  int   nSCT;
  int   hitPattern; // Hit pattern of SCT space points
  int   nStrawHits; // Number of TRT hits associated with Track
  int   nStraw;     // Number of TRT straws intersected by track
  int   nStrawTime; // Number of TRT starws with valid drift time intersected by track
  int   nTRHits;        // Number of high-threshold TRT hits associated with track

};

#endif
