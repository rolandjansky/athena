/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**  @file TileMuon.h
*
*    authors: Akimasa Ishikawa (akimasa@phys.sci.kobe-u.ac.jp)             
*             Arantxa Ruiz Martinez (aranzazu.ruis.martines@cern.ch)
*/
#ifndef __Tile_Muon_H__
#define __Tile_Muon_H__

#include <vector>

//class Rec::TrackParticle

class TileMuon 
{
public:
  TileMuon ();
  virtual ~TileMuon ();

  //set functions
  void setTileMuon( float s_eta, float s_phi, 
                    std::vector<float> s_energy, float s_quality );

  //get functions

  //clear functions
  void clear();

public:
  //MuonSpectorometer
  float eta;
  float phi;
  std::vector<float> energy;
  float quality;


};

#endif
