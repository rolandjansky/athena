/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsActions/FadsTrackingAction.h"
#include "FadsActions/TrackingAction.h"
#include "G4RunManager.hh"
#include "G4VTrajectory.hh"

namespace FADS {

FadsTrackingAction* FadsTrackingAction::thePointer=0;
FadsTrackingAction::FadsTrackingAction():G4UserTrackingAction(),applAction(0)
{
}
FadsTrackingAction::~FadsTrackingAction()
{
}
FadsTrackingAction* FadsTrackingAction::GetTrackingAction()
{
	if (!thePointer) thePointer=new FadsTrackingAction;
	return thePointer;
}
void FadsTrackingAction::RegisterAction(TrackingAction* ta)
{
	actionList.push_back(ta);
}
void FadsTrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
	if (applAction) applAction->PreUserTrackingAction(aTrack);
	for (unsigned int i=0;i<actionList.size();i++)
		actionList[i]->PreUserTrackingAction(aTrack);
}
void FadsTrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
	if (applAction) applAction->PostUserTrackingAction(aTrack);
	for (unsigned int i=0;i<actionList.size();i++)
	{
		actionList[i]->PostUserTrackingAction(aTrack);
	} 
}
void FadsTrackingAction::SetTraj(G4VTrajectory *t)
{
	fpTrackingManager->SetStoreTrajectory(true);
	fpTrackingManager->SetTrajectory(t);
}
G4VTrajectory * FadsTrackingAction::GetTraj()
{
	return fpTrackingManager->GimmeTrajectory();
}
void FadsTrackingAction::ResetTraj()
{
	fpTrackingManager->SetStoreTrajectory(false);
}

}	// end namespace
