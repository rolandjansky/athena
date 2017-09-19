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
  if (m_trackInfo==nullptr) return false;
  return m_trackInfo->GetClassification()==Primary;
}

bool TrackHelper::IsRegeneratedPrimary() const
{
  if (m_trackInfo==nullptr) return false;
  return m_trackInfo->GetClassification()==RegeneratedPrimary;
}

bool TrackHelper::IsRegisteredSecondary() const
{
  if (m_trackInfo==nullptr) return false;
  return m_trackInfo->GetClassification()==RegisteredSecondary;
}

bool TrackHelper::IsSecondary() const
{
  if (m_trackInfo==nullptr) return true;
  return m_trackInfo->GetClassification()==Secondary;
}

int TrackHelper::GetBarcode() const
{
  if (m_trackInfo==nullptr || m_trackInfo->GetHepMCParticle()==nullptr) return 0;
  return m_trackInfo->GetParticleBarcode();
}

HepMcParticleLink TrackHelper::GetParticleLink()
{
  const HepMcParticleLink *originalParticleLink(nullptr);
  if (this->GetTrackInformation() && this->GetTrackInformation()->GetBaseISFParticle())
    {
      originalParticleLink = this->GetTrackInformation()->GetBaseISFParticle()->getParticleLink();
    }
  return (originalParticleLink) ? HepMcParticleLink(this->GetBarcode(), originalParticleLink->eventIndex(), originalParticleLink->getEventCollection()) : HepMcParticleLink(this->GetBarcode());
}
