/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruth/TrackInformation.h"
#include "HepMC/GenEvent.h"

TrackInformation::TrackInformation():regenerationNr(0),theParticle(0),theISFParticle(0),returnedToISF(false)
{
}

TrackInformation::TrackInformation(const HepMC::GenParticle *p,const ISF::ISFParticle* isp):regenerationNr(0),theParticle(p),theISFParticle(isp),returnedToISF(false)
{
}

const HepMC::GenParticle* TrackInformation::GetHepMCParticle() const
{
  return theParticle;
}

const ISF::ISFParticle* TrackInformation::GetISFParticle() const
{
  return theISFParticle;
}

int TrackInformation::GetParticleBarcode() const
{
  //return theParticle?theParticle->barcode():-1;
  if (!theParticle) {
    std::cout<<"*AS* ERROR TrackInformation::GetParticleBarcode() without particle "<<std::endl;
    return -1;
  }
  return theParticle->barcode();
}

void TrackInformation::SetParticle(const HepMC::GenParticle* p)
{
  theParticle=p;
}

void TrackInformation::SetISFParticle(const ISF::ISFParticle* p)
{
  theISFParticle=p;
}
