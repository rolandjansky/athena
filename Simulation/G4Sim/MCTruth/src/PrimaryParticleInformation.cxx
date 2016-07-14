/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruth/PrimaryParticleInformation.h"

PrimaryParticleInformation::PrimaryParticleInformation() 
  : theParticle(0),theISFParticle(0),regenerationNr(0),barcode(-1)
{
}

PrimaryParticleInformation::PrimaryParticleInformation(const HepMC::GenParticle *p, const ISF::ISFParticle* isp):theParticle(p),theISFParticle(isp),regenerationNr(0),barcode(-1)
{
}

const HepMC::GenParticle* PrimaryParticleInformation::GetHepMCParticle() const
{
	return theParticle;
}

const ISF::ISFParticle* PrimaryParticleInformation::GetISFParticle() const
{
	return theISFParticle;
}

void PrimaryParticleInformation::SuggestBarcode(int bc)
{
  barcode=bc;
  if (theParticle) {
    std::cout<<"ERROR: PrimaryParticleInformation::SuggestBarcode() should be only called if no HepMC::Particle is available"<<std::endl;
    //theParticle->suggest_barcode(bc);
  }
}

int PrimaryParticleInformation::GetParticleBarcode() const
{
	return theParticle?theParticle->barcode():barcode;
}

void PrimaryParticleInformation::SetParticle(const HepMC::GenParticle* p)
{
	theParticle=p;
}

void PrimaryParticleInformation::SetISFParticle(const ISF::ISFParticle* p)
{
	theISFParticle=p;
}
