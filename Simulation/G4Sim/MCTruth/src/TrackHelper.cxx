/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruth/TrackHelper.h"

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
