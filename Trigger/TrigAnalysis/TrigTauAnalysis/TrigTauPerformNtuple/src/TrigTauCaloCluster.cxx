/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauPerformNtuple/TrigTauCaloCluster.h"

ClassImp(TrigTauCaloCluster)

TrigTauCaloCluster::TrigTauCaloCluster()
  : TObject()
  , m_e(0)
  , m_eta(0)
  , m_phi(0)
  , m_m(0)
  , m_radius(0)
  , m_containsBadCells(false)
{
  reset();
}

TrigTauCaloCluster::TrigTauCaloCluster( const TrigTauCaloCluster & other)
  : TObject()
  , m_e(other.m_e)
  , m_eta(other.m_eta)
  , m_phi(other.m_phi)
  , m_m(other.m_m)
  , m_radius(other.m_radius)
  , m_containsBadCells(other.m_containsBadCells)
  , r_cells(other.r_cells)
{
  for(std::vector<TrigTauMCParticleInCluster*>::const_iterator itr = other.g4Particles.begin(); itr != other.g4Particles.end(); ++itr)
    g4Particles.push_back(new TrigTauMCParticleInCluster(**itr));
  
}

TrigTauCaloCluster::~TrigTauCaloCluster() {reset();}

void TrigTauCaloCluster::reset()
{
  m_e   = 0;
  m_phi = 0;
  m_eta = 0;
  m_m   = 0;
  m_radius = 0;
  m_containsBadCells = false;
  resetCells();
  for(std::vector<TrigTauMCParticleInCluster*>::iterator itr =g4Particles .begin(); itr != g4Particles.end(); ++itr)
    delete *itr;
  g4Particles.erase(g4Particles.begin(), g4Particles.end() );
}
