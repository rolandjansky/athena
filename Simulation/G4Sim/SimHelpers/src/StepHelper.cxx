/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "SimHelpers/StepHelper.h"

#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4TouchableHistory.hh"
#include "G4VProcess.hh"

StepHelper::StepHelper():theStep(0)
{
}
StepHelper::StepHelper(const G4Step* s):theStep(s)
{
}
void StepHelper::SetStep(const G4Step* s)
{
	theStep=s;
}
G4ThreeVector StepHelper::PreStepPosition() const
{
	assert (theStep!=0);
	return theStep->GetPreStepPoint()->GetPosition();
}
G4ThreeVector StepHelper::PostStepPosition() const
{
	assert (theStep!=0);
	return theStep->GetPostStepPoint()->GetPosition();
}
std::string StepHelper::ParticleName() const
{
	assert (theStep!=0);
	return theStep->GetTrack()->GetDefinition()->GetParticleName();
}
int StepHelper::ParticlePDGCode() const
{
	assert (theStep!=0);
	return theStep->GetTrack()->GetDefinition()->GetPDGEncoding();
}
double StepHelper::DepositedEnergy() const
{
	assert (theStep!=0);
	return theStep->GetTotalEnergyDeposit();
}
G4LogicalVolume* StepHelper::GetPreStepLogicalVolume(int iLevel) const
{
	return GetPreStepPhysicalVolume(iLevel)->GetLogicalVolume();
}
std::string StepHelper::GetPreStepLogicalVolumeName(int iLevel) const
{
	return GetPreStepLogicalVolume(iLevel)->GetName();
}
G4VPhysicalVolume* StepHelper::GetPreStepPhysicalVolume(int iLevel) const
{
	G4TouchableHistory *history=(G4TouchableHistory *)
					theStep->GetPreStepPoint()->GetTouchable();
	if (iLevel<=0)
	{
		return history->GetVolume(abs(iLevel));
	}
	else
	{
		int nLev=history->GetHistoryDepth();
		return history->GetVolume(nLev-iLevel);
	}
}
G4LogicalVolume* StepHelper::GetPostStepLogicalVolume(int iLevel) const
{
	return GetPostStepPhysicalVolume(iLevel)->GetLogicalVolume();
}
std::string StepHelper::GetPostStepLogicalVolumeName(int iLevel) const
{
	return GetPostStepLogicalVolume(iLevel)->GetName();
}
G4VPhysicalVolume* StepHelper::GetPostStepPhysicalVolume(int iLevel) const
{
	G4TouchableHistory *history=(G4TouchableHistory *)
					theStep->GetPostStepPoint()->GetTouchable();
	if (iLevel<=0)
	{
		return history->GetVolume(abs(iLevel));
	}
	else
	{
		int nLev=history->GetHistoryDepth();
		return history->GetVolume(nLev-iLevel);
	}
}
int StepHelper::PreStepBranchDepth() const
{
	return ((G4TouchableHistory *)theStep->GetPreStepPoint()->
				GetTouchable())->GetHistoryDepth();
}
int StepHelper::PostStepBranchDepth() const
{
	return ((G4TouchableHistory *)theStep->GetPostStepPoint()->
				GetTouchable())->GetHistoryDepth();
}
const G4VProcess* StepHelper::GetProcess() const
{
	return theStep->GetPostStepPoint()->GetProcessDefinedStep();
}

std::string StepHelper::GetProcessName() const
{
	return GetProcess()->GetProcessName();
}

G4int StepHelper::GetProcessSubType() const
{
	return GetProcess()->GetProcessSubType();
}

