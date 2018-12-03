/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruth/TrackInformation.h"
#include "HepMC/GenEvent.h"

TrackInformation::TrackInformation():m_regenerationNr(0),m_theParticle(0),m_theBaseISFParticle(0),m_returnedToISF(false)
{
}

TrackInformation::TrackInformation(const HepMC::GenParticle *p,const ISF::ISFParticle* baseIsp):
    m_regenerationNr(0),
    m_theParticle(p),
    m_theBaseISFParticle(baseIsp),
    m_returnedToISF(false)
{
}

const HepMC::GenParticle* TrackInformation::GetHepMCParticle() const
{
  return m_theParticle;
}

const ISF::ISFParticle* TrackInformation::GetBaseISFParticle() const
{
  return m_theBaseISFParticle;
}

int TrackInformation::GetParticleBarcode() const
{
  return ( m_theParticle ? m_theParticle->barcode() : 0 );
}

void TrackInformation::SetParticle(const HepMC::GenParticle* p)
{
  m_theParticle=p;
}

void TrackInformation::SetBaseISFParticle(const ISF::ISFParticle* p)
{
  m_theBaseISFParticle=p;
}
