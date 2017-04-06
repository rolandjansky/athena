/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruth/TrackHelper.h"
#include "ISF_Event/ISFParticle.h"

TrackHelper::TrackHelper(const G4Track* t)
{
  m_trackInfo=static_cast<TrackInformation *>(t->GetUserInformation());
}
bool TrackHelper::IsPrimary() const
{
  if (m_trackInfo==0) return false;
  return m_trackInfo->GetClassification()==Primary;
}
bool TrackHelper::IsRegeneratedPrimary() const
{
  if (m_trackInfo==0) return false;
  return m_trackInfo->GetClassification()==RegeneratedPrimary;
}
bool TrackHelper::IsRegisteredSecondary() const
{
  if (m_trackInfo==0) return false;
  return m_trackInfo->GetClassification()==RegisteredSecondary;
}
bool TrackHelper::IsSecondary() const
{
  if (m_trackInfo==0) return true;
  return m_trackInfo->GetClassification()==Secondary;
}
int TrackHelper::GetBarcode() const
{
  if (m_trackInfo==0 || m_trackInfo->GetHepMCParticle()==0) return 0;
  return m_trackInfo->GetParticleBarcode();
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
