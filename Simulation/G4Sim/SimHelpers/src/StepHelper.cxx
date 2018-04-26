/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "SimHelpers/StepHelper.h"

#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4TouchableHistory.hh"
#include "G4VProcess.hh"

namespace G4StepHelper {
G4ThreeVector preStepPosition(const G4Step* theStep)
{
  assert (theStep!=nullptr);
  return theStep->GetPreStepPoint()->GetPosition();
}
G4ThreeVector postStepPosition(const G4Step* theStep)
{
  assert (theStep!=nullptr);
  return theStep->GetPostStepPoint()->GetPosition();
}
std::string particleName(const G4Step* theStep)
{
  assert (theStep!=nullptr);
  return theStep->GetTrack()->GetDefinition()->GetParticleName();
}
int particlePDGCode(const G4Step* theStep)
{
  assert (theStep!=nullptr);
  return theStep->GetTrack()->GetDefinition()->GetPDGEncoding();
}
double depositedEnergy(const G4Step* theStep)
{
  assert (theStep!=nullptr);
  return theStep->GetTotalEnergyDeposit();
}
G4LogicalVolume* getPreStepLogicalVolume(const G4Step* theStep, int iLevel)
{
  return getPreStepPhysicalVolume(theStep, iLevel)->GetLogicalVolume();
}
std::string getPreStepLogicalVolumeName(const G4Step* theStep, int iLevel)
{
  return getPreStepLogicalVolume(theStep, iLevel)->GetName();
}
G4VPhysicalVolume* getPreStepPhysicalVolume(const G4Step* theStep, int iLevel)
{
  const G4TouchableHistory *history
    = static_cast<const G4TouchableHistory*>(theStep->GetPreStepPoint()->GetTouchable());
  if (iLevel<=0)
    {
      return history->GetVolume(std::abs(iLevel));
    }
  const int nLev=history->GetHistoryDepth();
  return history->GetVolume(nLev-iLevel);
}
G4LogicalVolume* getPostStepLogicalVolume(const G4Step* theStep, int iLevel)
{
  return getPostStepPhysicalVolume(theStep, iLevel)->GetLogicalVolume();
}
std::string getPostStepLogicalVolumeName(const G4Step* theStep, int iLevel)
{
  return getPostStepLogicalVolume(theStep, iLevel)->GetName();
}
G4VPhysicalVolume* getPostStepPhysicalVolume(const G4Step* theStep, int iLevel)
{
  const G4TouchableHistory *history
    = static_cast<const G4TouchableHistory*>(theStep->GetPostStepPoint()->GetTouchable());
  if (iLevel<=0)
    {
      return history->GetVolume(std::abs(iLevel));
    }
  const int nLev=history->GetHistoryDepth();
  return history->GetVolume(nLev-iLevel);
}
int preStepBranchDepth(const G4Step* theStep)
{
  return static_cast<const G4TouchableHistory*>(theStep->GetPreStepPoint()->
          GetTouchable())->GetHistoryDepth();
}
int postStepBranchDepth(const G4Step* theStep)
{
  return static_cast<const G4TouchableHistory*>(theStep->GetPostStepPoint()->
          GetTouchable())->GetHistoryDepth();
}
const G4VProcess* getProcess(const G4Step* theStep)
{
  return theStep->GetPostStepPoint()->GetProcessDefinedStep();
}

std::string getProcessName(const G4Step* theStep)
{
  return getProcess(theStep)->GetProcessName();
}

G4int getProcessSubType(const G4Step* theStep)
{
  return getProcess(theStep)->GetProcessSubType();
}
}
