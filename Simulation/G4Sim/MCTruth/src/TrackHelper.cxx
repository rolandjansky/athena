/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruth/TrackHelper.h"
#include "ISF_Event/ISFParticle.h"

TrackHelper::TrackHelper(const G4Track* t): theTrack(t)
{
  trackInfo=static_cast<TrackInformation *>(t->GetUserInformation());
}
bool TrackHelper::IsPrimary() const
{
  if (trackInfo==0) return false;
  return trackInfo->GetClassification()==Primary;
}
bool TrackHelper::IsRegeneratedPrimary() const
{
  if (trackInfo==0) return false;
  return trackInfo->GetClassification()==RegeneratedPrimary;
}
bool TrackHelper::IsRegisteredSecondary() const
{
  if (trackInfo==0) return false;
  return trackInfo->GetClassification()==RegisteredSecondary;
}
bool TrackHelper::IsSecondary() const
{
  if (trackInfo==0) return true;
  return trackInfo->GetClassification()==Secondary;
}
int TrackHelper::GetBarcode() const
{
  if (trackInfo==0 || trackInfo->GetHepMCParticle()==0) return 0;
  return trackInfo->GetParticleBarcode();
}

HepMcParticleLink TrackHelper::GetParticleLink()
{
  int barcode = this->GetBarcode();
  return HepMcParticleLink(barcode);
  //HepMcParticleLink * originalParticleLink(nullptr);
  //if (this->GetTrackInformation() && this->GetTrackInformation()->GetBaseISFParticle())
  //  {
  //    originalParticleLink = this->GetTrackInformation()->GetBaseISFParticle()->getParticleLink();
  //  }
  //return (originalParticleLink) ? HepMcParticleLink(barcode, originalParticleLink->eventIndex(), originalParticleLink->getEventCollection()) : HepMcParticleLink(barcode);
}
