/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruth/PrimaryParticleInformation.h"

PrimaryParticleInformation::PrimaryParticleInformation() 
  : m_theParticle(0),m_theISFParticle(0),m_regenerationNr(0),m_barcode(-1)
{
}

PrimaryParticleInformation::PrimaryParticleInformation(const HepMC::GenParticle *p, const ISF::ISFParticle* isp):m_theParticle(p),m_theISFParticle(isp),m_regenerationNr(0),m_barcode(-1)
{
}

const HepMC::GenParticle* PrimaryParticleInformation::GetHepMCParticle() const
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
    //theParticle->suggest_barcode(bc);
  }
}

int PrimaryParticleInformation::GetParticleBarcode() const
{
	return m_theParticle?m_theParticle->barcode():m_barcode;
}

void PrimaryParticleInformation::SetParticle(const HepMC::GenParticle* p)
{
	m_theParticle=p;
}

void PrimaryParticleInformation::SetISFParticle(const ISF::ISFParticle* p)
{
	m_theISFParticle=p;
}
