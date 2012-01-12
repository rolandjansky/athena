/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauPerformNtuple/TrigTauMCParticleInCluster.h"

ClassImp(TrigTauMCParticleInCluster) 

TrigTauMCParticleInCluster::TrigTauMCParticleInCluster()
  : TObject()
  , m_pdgid(0)
  , m_barcode(0)
  , m_e(0)
  , m_eta(0)
  , m_phi(0)
  , m_x(0)
  , m_y(0)
  , m_z(0)
  , m_time(0)
{
  //reset();
}

TrigTauMCParticleInCluster::TrigTauMCParticleInCluster( const TrigTauMCParticleInCluster & other)
  : TObject()
  , m_pdgid(other.m_pdgid)
  , m_barcode(other.m_barcode)
  , m_e(other.m_e)
  , m_eta(other.m_eta)
  , m_phi(other.m_phi)
  , m_x(other.m_x)
  , m_y(other.m_y)
  , m_z(other.m_z)
  , m_time(other.m_time)
{}

TrigTauMCParticleInCluster::~TrigTauMCParticleInCluster() {}
  
void TrigTauMCParticleInCluster::reset()
{
  m_pdgid = 0;  
  m_barcode = 0;  
  m_e = 0;  
  m_phi = 0;
  m_eta = 0;
  m_x = 0;
  m_y = 0;
  m_z = 0;
  m_time = 0;
}
