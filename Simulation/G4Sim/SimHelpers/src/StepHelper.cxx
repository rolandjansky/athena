/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "SimHelpers/StepHelper.h"

#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4TouchableHistory.hh"
#include "G4VProcess.hh"

namespace G4StepHelper {
G4ThreeVector StepHelper::preStepPosition(const G4Step* theStep)
{
  assert (theStep!=nullptr);
  return theStep->GetPreStepPoint()->GetPosition();
}
G4ThreeVector StepHelper::postStepPosition(const G4Step* theStep)
{
  assert (theStep!=nullptr);
  return theStep->GetPostStepPoint()->GetPosition();
}
std::string StepHelper::particleName(const G4Step* theStep)
{
  assert (theStep!=nullptr);
  return theStep->GetTrack()->GetDefinition()->GetParticleName();
}
int StepHelper::particlePDGCode(const G4Step* theStep)
{
  assert (theStep!=nullptr);
  return theStep->GetTrack()->GetDefinition()->GetPDGEncoding();
}
double StepHelper::depositedEnergy(const G4Step* theStep)
{
  assert (theStep!=nullptr);
  return theStep->GetTotalEnergyDeposit();
}
G4LogicalVolume* StepHelper::getPreStepLogicalVolume(const G4Step* theStep, int iLevel)
{
  return getPreStepPhysicalVolume(theStep, iLevel)->GetLogicalVolume();
}
std::string StepHelper::getPreStepLogicalVolumeName(const G4Step* theStep, int iLevel)
{
  return getPreStepLogicalVolume(theStep, iLevel)->GetName();
}
G4VPhysicalVolume* StepHelper::getPreStepPhysicalVolume(const G4Step* theStep, int iLevel)
{
  G4TouchableHistory *history
    = static_cast<G4TouchableHistory*>(theStep->GetPreStepPoint()->GetTouchable());
  if (iLevel<=0)
    {
      return history->GetVolume(std::abs(iLevel));
    }
  const int nLev=history->GetHistoryDepth();
  return history->GetVolume(nLev-iLevel);
}
G4LogicalVolume* StepHelper::getPostStepLogicalVolume(const G4Step* theStep, int iLevel)
{
  return getPostStepPhysicalVolume(theStep, iLevel)->GetLogicalVolume();
}
std::string StepHelper::getPostStepLogicalVolumeName(const G4Step* theStep, int iLevel)
{
  return getPostStepLogicalVolume(theStep, iLevel)->GetName();
}
G4VPhysicalVolume* StepHelper::getPostStepPhysicalVolume(const G4Step* theStep, int iLevel)
{
  G4TouchableHistory *history
    = static_cast<G4TouchableHistory*>(theStep->GetPostStepPoint()->GetTouchable());
  if (iLevel<=0)
    {
      return history->GetVolume(std::abs(iLevel));
    }
  const int nLev=history->GetHistoryDepth();
  return history->GetVolume(nLev-iLevel);
}
int StepHelper::preStepBranchDepth(const G4Step* theStep)
{
  return static_cast<G4TouchableHistory*>(theStep->GetPreStepPoint()->
          GetTouchable())->GetHistoryDepth();
}
int StepHelper::postStepBranchDepth(const G4Step* theStep)
{
  return static_cast<G4TouchableHistory*>(theStep->GetPostStepPoint()->
          GetTouchable())->GetHistoryDepth();
}
const G4VProcess* StepHelper::getProcess(const G4Step* theStep)
{
  return theStep->GetPostStepPoint()->GetProcessDefinedStep();
}

std::string StepHelper::getProcessName(const G4Step* theStep)
{
  return getProcess(theStep)->GetProcessName();
}

G4int StepHelper::getProcessSubType(const G4Step* theStep)
{
  return getProcess(theStep)->GetProcessSubType();
}
}
