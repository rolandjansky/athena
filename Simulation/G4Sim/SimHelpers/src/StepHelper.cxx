/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "SimHelpers/StepHelper.h"

#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4TouchableHistory.hh"
#include "G4VProcess.hh"

StepHelper::StepHelper():m_theStep(0)
{
}
StepHelper::StepHelper(const G4Step* s):m_theStep(s)
{
}
void StepHelper::SetStep(const G4Step* s)
{
	m_theStep=s;
}
G4ThreeVector StepHelper::PreStepPosition() const
{
	assert (m_theStep!=0);
	return m_theStep->GetPreStepPoint()->GetPosition();
}
G4ThreeVector StepHelper::PostStepPosition() const
{
	assert (m_theStep!=0);
	return m_theStep->GetPostStepPoint()->GetPosition();
}
std::string StepHelper::ParticleName() const
{
	assert (m_theStep!=0);
	return m_theStep->GetTrack()->GetDefinition()->GetParticleName();
}
int StepHelper::ParticlePDGCode() const
{
	assert (m_theStep!=0);
	return m_theStep->GetTrack()->GetDefinition()->GetPDGEncoding();
}
double StepHelper::DepositedEnergy() const
{
	assert (m_theStep!=0);
	return m_theStep->GetTotalEnergyDeposit();
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
					m_theStep->GetPreStepPoint()->GetTouchable();
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
					m_theStep->GetPostStepPoint()->GetTouchable();
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
	return ((G4TouchableHistory *)m_theStep->GetPreStepPoint()->
				GetTouchable())->GetHistoryDepth();
}
int StepHelper::PostStepBranchDepth() const
{
	return ((G4TouchableHistory *)m_theStep->GetPostStepPoint()->
				GetTouchable())->GetHistoryDepth();
}
const G4VProcess* StepHelper::GetProcess() const
{
	return m_theStep->GetPostStepPoint()->GetProcessDefinedStep();
}

std::string StepHelper::GetProcessName() const
{
	return GetProcess()->GetProcessName();
}

G4int StepHelper::GetProcessSubType() const
{
	return GetProcess()->GetProcessSubType();
}

