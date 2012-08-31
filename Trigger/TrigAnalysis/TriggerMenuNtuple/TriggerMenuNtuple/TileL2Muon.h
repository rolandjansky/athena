/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**  @file TileL2Muon.h
*
*    authors: Akimasa Ishikawa (akimasa@phys.sci.kobe-u.ac.jp)             
*             Arantxa Ruiz Martinez (aranzazu.ruis.martines@cern.ch)
*/
#ifndef __Tile_L2_Muon_H__
#define __Tile_L2_Muon_H__

#include <vector>

//class Rec::TrackParticle

class TileL2Muon 
{
public:
  TileL2Muon ();
  virtual ~TileL2Muon ();

  //set functions
  void setTileL2Muon( std::vector<float> s_eta, std::vector<float> s_phi, 
                      std::vector<float> s_energy0, std::vector<float> s_energy1, std::vector<float> s_energy2,
                      std::vector<float> s_quality );

  //get functions

  //clear functions
  void clear();

public:
  //MuonSpectorometer
  std::vector<float> eta;
  std::vector<float> phi;
  std::vector<float> energy0;
  std::vector<float> energy1;
  std::vector<float> energy2;
  std::vector<float> quality;


};

#endif
