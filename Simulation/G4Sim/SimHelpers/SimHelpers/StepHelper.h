/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SIMHELPERS_StepHelper_H
#define SIMHELPERS_StepHelper_H

#include "G4Step.hh"
#include "G4EmProcessSubType.hh"
#include "G4HadronicProcessType.hh"
#include "G4DecayProcessType.hh"
#include <string>
#include <vector>

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4VProcess;

/// @brief helper functions to avoid having to play with the G4Step to
/// retrieve relevant quantities.
///
/// TODO: These should be standalone functions rather than a class.
///
namespace G4StepHelper {
  /// @brief TODO
  G4ThreeVector preStepPosition(const G4Step* theStep);
  /// @brief TODO
  G4ThreeVector postStepPosition(const G4Step* theStep);
  /// @brief TODO
  std::string particleName(const G4Step* theStep);
  /// @brief TODO
  int particlePDGCode(const G4Step* theStep);
  /// @brief TODO
  double depositedEnergy(const G4Step* theStep);
  /// @brief TODO
  G4LogicalVolume* getPreStepLogicalVolume(const G4Step* theStep, int iLevel=0);
  /// @brief TODO
  std::string getPreStepLogicalVolumeName(const G4Step* theStep, int iLevel=0);
  /// @brief TODO
  G4VPhysicalVolume* getPreStepPhysicalVolume(const G4Step* theStep, int iLevel=0);
  /// @brief TODO
  G4LogicalVolume* getPostStepLogicalVolume(const G4Step* theStep, int iLevel=0);
  /// @brief TODO
  std::string getPostStepLogicalVolumeName(const G4Step* theStep, int iLevel=0);
  /// @brief TODO
  G4VPhysicalVolume* getPostStepPhysicalVolume(const G4Step* theStep, int iLevel=0);
  /// @brief TODO
  int preStepBranchDepth(const G4Step* theStep);
  /// @brief TODO
  int postStepBranchDepth(const G4Step* theStep);
  /// @brief TODO
  const G4VProcess* getProcess(const G4Step* theStep);
  /// @brief TODO
  std::string getProcessName(const G4Step* theStep);
  /// @brief TODO
  G4int getProcessSubType(const G4Step* theStep);
}

#endif
