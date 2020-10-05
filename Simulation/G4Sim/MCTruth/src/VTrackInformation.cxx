/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruth/VTrackInformation.h"

VTrackInformation::VTrackInformation(TrackClassification tc):m_classify(tc)
{
}

HepMC::ConstGenParticlePtr VTrackInformation::GetPrimaryHepMCParticle() const
{
  return m_thePrimaryParticle;
}

void VTrackInformation::SetPrimaryHepMCParticle(HepMC::ConstGenParticlePtr p)
{
  m_thePrimaryParticle=p;
}


HepMC::ConstGenParticlePtr VTrackInformation::GetHepMCParticle() const
{
  return 0;
}

const ISF::ISFParticle* VTrackInformation::GetBaseISFParticle() const
{
  return 0;
}

bool VTrackInformation::GetReturnedToISF() const
{
  return false;
}

void VTrackInformation::SetParticle(HepMC::ConstGenParticlePtr /*p*/)
{
  // you should not call this, perhaps throw an exception?
  std::cerr<<"ERROR  VTrackInformation::SetParticle() not supported  "<<std::endl;
 
}

void VTrackInformation::SetBaseISFParticle(const ISF::ISFParticle* /*p*/)
{
  // you should not call this, perhaps throw an exception?
  std::cerr<<"ERROR  VTrackInformation::SetBaseISFParticle() not supported  "<<std::endl;
 
}

void VTrackInformation::SetReturnedToISF(bool)
{
  std::cerr<<"ERROR  VTrackInformation::SetReturnedToISF() not supported  "<<std::endl;
}
