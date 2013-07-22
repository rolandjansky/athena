/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsActions/FadsStackingAction.h"
#include "FadsActions/StackingAction.h"
#include "G4RunManager.hh"

namespace FADS {

FadsStackingAction* FadsStackingAction::thePointer=0;

FadsStackingAction::FadsStackingAction():G4UserStackingAction(),applAction(0)
{
}
FadsStackingAction::~FadsStackingAction()
{
}
FadsStackingAction* FadsStackingAction::GetStackingAction()
{
	if (!thePointer) thePointer=new FadsStackingAction;
	return thePointer;
}
void FadsStackingAction::RegisterAction(StackingAction* ta)
{
	actionList.push_back(ta);
}
G4ClassificationOfNewTrack FadsStackingAction::ClassifyNewTrack(const G4Track* aTrack)
{
	if (applAction) return applAction->ClassifyNewTrack(aTrack);
	return fUrgent;
}
void FadsStackingAction::NewStage()
{
	if (applAction) applAction->NewStage();
	for (unsigned int i=0;i<actionList.size();i++)
		actionList[i]->NewStage();
}
void FadsStackingAction::PrepareNewEvent()
{
	if (applAction) applAction->PrepareNewEvent();
	for (unsigned int i=0;i<actionList.size();i++)
		actionList[i]->PrepareNewEvent();
}

}	// end namespace
