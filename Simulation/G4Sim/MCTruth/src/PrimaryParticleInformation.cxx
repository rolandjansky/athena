/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruth/PrimaryParticleInformation.h"

PrimaryParticleInformation::PrimaryParticleInformation()
{
}

PrimaryParticleInformation::PrimaryParticleInformation(HepMC::ConstGenParticlePtr p, const ISF::ISFParticle* isp):m_theParticle(p),m_theISFParticle(isp)
{
}

HepMC::ConstGenParticlePtr PrimaryParticleInformation::GetHepMCParticle() const
{
  return m_theParticle;
}

const ISF::ISFParticle* PrimaryParticleInformation::GetISFParticle() const
{
  return m_theISFParticle;
}

void PrimaryParticleInformation::SuggestBarcode(int bc)
{
  m_barcode=bc;
  if (m_theParticle) {
    std::cout<<"ERROR: PrimaryParticleInformation::SuggestBarcode() should be only called if no HepMC::Particle is available"<<std::endl;
  }
}

int PrimaryParticleInformation::GetParticleBarcode() const
{
  return m_theParticle?HepMC::barcode(m_theParticle):m_barcode;
}

void PrimaryParticleInformation::SetParticle(HepMC::ConstGenParticlePtr p)
{
  m_theParticle=p;
}

void PrimaryParticleInformation::SetISFParticle(const ISF::ISFParticle* p)
{
  m_theISFParticle=p;
}
