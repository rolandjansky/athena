/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruth/VTrackInformation.h"

VTrackInformation::VTrackInformation(TrackClassification tc):classify(tc)
{
}

const HepMC::GenParticle* VTrackInformation::GetHepMCParticle() const
{
  return 0;
}

const ISF::ISFParticle* VTrackInformation::GetISFParticle() const
{
  return 0;
}

void VTrackInformation::SetParticle(const HepMC::GenParticle* /*p*/)
{
  // you should not call this, perhaps throw an exception?
  std::cerr<<"ERROR  VTrackInformation::SetParticle() not supported  "<<std::endl;
 
}

void VTrackInformation::SetISFParticle(const ISF::ISFParticle* /*p*/)
{
  // you should not call this, perhaps throw an exception?
  std::cerr<<"ERROR  VTrackInformation::SetISFParticle() not supported  "<<std::endl;
 
}
