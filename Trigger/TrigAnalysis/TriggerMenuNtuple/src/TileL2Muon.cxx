/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**  @file TileL2Muon.cxx
*
*    authors: Akimasa Ishikawa (akimasa@phys.sci.kobe-u.ac.jp)             
*             Arantxa Ruiz Martinez (aranzazu.ruis.martines@cern.ch)
*/
#include "TriggerMenuNtuple/TileL2Muon.h"

TileL2Muon::TileL2Muon()
{
  clear();
}

TileL2Muon::~TileL2Muon()
{
}

void
TileL2Muon::setTileL2Muon( std::vector<float> s_eta, std::vector<float> s_phi, 
                           std::vector<float> s_energy0, std::vector<float> s_energy1, std::vector<float> s_energy2,
                           std::vector<float> s_quality )
{
  eta = s_eta;
  phi = s_phi;
  energy0 = s_energy0;
  energy1 = s_energy1;
  energy2 = s_energy2;
  quality = s_quality;
}

void
TileL2Muon::clear()
{
  eta.clear();
  phi.clear();
  energy0.clear();
  energy1.clear();
  energy2.clear();
  quality.clear();
}
