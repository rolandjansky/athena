/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**  @file TileMuon.cxx
*
*    authors: Akimasa Ishikawa (akimasa@phys.sci.kobe-u.ac.jp)             
*             Arantxa Ruiz Martinez (aranzazu.ruis.martines@cern.ch)
*/
#include "TriggerMenuNtuple/TileMuon.h"

TileMuon::TileMuon()
{
  clear();
}

TileMuon::~TileMuon()
{
}

void
TileMuon::setTileMuon( float s_eta, float s_phi, 
                       std::vector<float> s_energy, float s_quality )
{
  eta = s_eta;
  phi = s_phi;
  energy = s_energy;
  quality = s_quality;
}

void
TileMuon::clear()
{
  eta = 0;
  phi = 0;
  energy.clear();
  quality = 0;
}
