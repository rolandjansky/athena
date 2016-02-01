/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruth/TrackInformation.h"
#include "HepMC/GenEvent.h"

TrackInformation::TrackInformation():regenerationNr(0),theParticle(0),theBaseISFParticle(0),returnedToISF(false)
{
}

TrackInformation::TrackInformation(const HepMC::GenParticle *p,const ISF::ISFParticle* baseIsp):
    regenerationNr(0),
    theParticle(p),
    theBaseISFParticle(baseIsp),
    returnedToISF(false)
{
}

const HepMC::GenParticle* TrackInformation::GetHepMCParticle() const
{
  return theParticle;
}

const ISF::ISFParticle* TrackInformation::GetBaseISFParticle() const
{
  return theBaseISFParticle;
}

int TrackInformation::GetParticleBarcode() const
{
  return ( theParticle ? theParticle->barcode() : 0 );
}

void TrackInformation::SetParticle(const HepMC::GenParticle* p)
{
  theParticle=p;
}

void TrackInformation::SetBaseISFParticle(const ISF::ISFParticle* p)
{
  theBaseISFParticle=p;
}
