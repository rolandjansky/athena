/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef StepHelper_H
#define StepHelper_H

#include "G4Step.hh"
#include "G4EmProcessSubType.hh"
#include "G4HadronicProcessType.hh"
#include "G4DecayProcessType.hh"
#include <string>
#include <vector>

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4VProcess;

/// @brief helper class which avoids having to play with the G4Step to retrieve
/// relevant quantities.
///
/// TODO: These should be standalone functions rather than a class.
///
class StepHelper
{
public:
  StepHelper();
  StepHelper(const G4Step*);
  void SetStep(const G4Step*);
  G4ThreeVector PreStepPosition() const;
  G4ThreeVector PostStepPosition() const;
  std::string ParticleName() const;
  int ParticlePDGCode() const;
  double DepositedEnergy() const;
  G4LogicalVolume* GetPreStepLogicalVolume(int iLevel=0) const;
  std::string GetPreStepLogicalVolumeName(int iLevel=0) const;
  G4VPhysicalVolume* GetPreStepPhysicalVolume(int iLevel=0) const;
  G4LogicalVolume* GetPostStepLogicalVolume(int iLevel=0) const;
  std::string GetPostStepLogicalVolumeName(int iLevel=0) const;
  G4VPhysicalVolume* GetPostStepPhysicalVolume(int iLevel=0) const;
  int PreStepBranchDepth() const;
  int PostStepBranchDepth() const;
  const G4VProcess* GetProcess() const;
  std::string GetProcessName() const;
  G4int GetProcessSubType() const;
private:
  const G4Step *m_theStep;
};

#endif
