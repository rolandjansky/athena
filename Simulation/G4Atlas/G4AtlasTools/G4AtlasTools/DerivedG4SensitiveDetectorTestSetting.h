/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"

#include "G4Track.hh"
#include "G4StepPoint.hh"
#include "G4DynamicParticle.hh"
#include "G4ThreeVector.hh"
#include "G4Box.hh"
#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4VPhysicalVolume.hh"
#include "G4SystemOfUnits.hh"

#include "DerivedG4Process.h"

//In this function, the reasons why I use 'new' to define objects instead of using smart pointers are as follows:
// 1. this function aims to set environment for test code, which means the objects that were defined in the function should exist throughout the whole code running. Therefore, defining the objects with 'new' can meet the requirement. If I define them with smart pointer, they will be destruct once they are out of scope.
// 2. this function will only be used in the unit test code for testing the sensitive detector classes based on G4VSensitiveDetector. So it will never be responsible for memory leaks in production jobs.
void DerivedG4SensitiveDetectorTestSetting(G4Step& sp, G4double& totalEnergyDeposit, std::vector<G4String>& physicalNames, G4String& logicalName1, std::vector<G4int>& copyNos, G4ThreeVector& preStepPos, G4ThreeVector& postStepPos, G4double& globalTime0/*for preSP*/, G4double& kineticEnergy0/*for preSP*/, G4double& velocity0/*for preSP*/, G4double& globalTime/*for track*/, G4double& kineticEnergy/*for track*/, G4double& globalTime1/*for postSP*/, G4double& kineticEnergy1/*for postSP*/, G4double& velocity1/*for postSP*/, G4double& steplength, G4double& charge, G4int& encoding, G4int& antiencoding, G4String& astring, G4ProcessType& atype, G4String& nop1, G4String& nop2, G4String& nop3)
{

  if (copyNos.size()!=physicalNames.size()) { throw std::runtime_error("ERROR: physicalNames and copyNos must have the same length."); }
//decorate sp with the variable called TotalEnergyDeposit
  G4double TotalEnergyDeposit = totalEnergyDeposit;//para(i.e. there is a parameter in this line)
  sp.SetTotalEnergyDeposit( TotalEnergyDeposit );
//end

//decorate sp with a G4StepPoint object
  G4StepPoint* stepPoint = new G4StepPoint();
  stepPoint->SetPosition(preStepPos);//para
  G4NavigationHistory* navigationHistory = new G4NavigationHistory();
  G4String boxName = "name";
  G4Box* box = new G4Box(boxName, 1.0, 1.0, 1.0);
  G4NistManager* man = G4NistManager::Instance();
  G4Material* material = man->FindOrBuildMaterial("G4_AIR");
  G4String name = "logicalName";
  G4LogicalVolume* fLogical = new G4LogicalVolume(box, material, name);
  G4String PhysicalName = physicalNames[0];//para
  G4VPhysicalVolume* pPhysical = nullptr;
  G4MyPhysicalVolume* physicalVolume = new G4MyPhysicalVolume(0, G4ThreeVector(0,0,0), PhysicalName, fLogical, pPhysical);
  G4int CopyNo = copyNos[0];
  physicalVolume->SetCopyNo(CopyNo);//para
  G4int nReplica = 2;
  navigationHistory->SetFirstEntry(physicalVolume);
  if (physicalNames.size()==1) {
    // temporary workaround
    navigationHistory->NewLevel(physicalVolume, kNormal, nReplica);
    navigationHistory->NewLevel(physicalVolume, kNormal, nReplica);
    navigationHistory->NewLevel(physicalVolume, kNormal, nReplica);
    navigationHistory->NewLevel(physicalVolume, kNormal, nReplica);
    navigationHistory->NewLevel(physicalVolume, kNormal, nReplica);
  }
  else {
    bool skip(true);
    size_t idx(0);
    for( const auto& name : physicalNames) {
      if (skip) { skip=false; ++idx; continue; }//skip first entry
      G4MyPhysicalVolume* parentPhysVol = new G4MyPhysicalVolume(0, G4ThreeVector(0,0,0), name, fLogical, pPhysical);
      parentPhysVol->SetCopyNo(copyNos[idx]);
      ++idx;
      navigationHistory->NewLevel(parentPhysVol, kNormal, nReplica);
    }
  }
  G4TouchableHistory* touchableHistory = new G4TouchableHistory(*navigationHistory);
  G4TouchableHandle touchableHandle(touchableHistory);
  stepPoint->SetTouchableHandle(touchableHandle);
  G4double GlobalTime0 = globalTime0;
  G4double KineticEnergy0 = kineticEnergy0;
  G4double Velocity0 = velocity0;
  stepPoint->SetGlobalTime(GlobalTime0);//para
  stepPoint->SetKineticEnergy(KineticEnergy0);//para
  stepPoint->SetVelocity(Velocity0);//para

  sp.SetPreStepPoint(stepPoint);
//end

//decorate sp with another G4StepPoint object
  G4StepPoint* stepPoint1 = new G4StepPoint();
  stepPoint1->SetPosition(postStepPos);//para
  G4double GlobalTime1 = globalTime1;
  G4double KineticEnergy1 = kineticEnergy1;
  G4double Velocity1 = velocity1;
  stepPoint1->SetGlobalTime(GlobalTime1);//para
  stepPoint1->SetKineticEnergy(KineticEnergy1);//para
  stepPoint1->SetVelocity(Velocity1);//para

  sp.SetPostStepPoint(stepPoint1);
//end

//set step length for the step
  G4double StepLength = steplength;
  sp.SetStepLength(StepLength);//para
//end

//decorate sp with a G4Track object
  G4double Charge = charge;
  G4int Encoding = encoding;
  G4int Antiencoding = antiencoding;
  G4String NOP1 = nop1;
  G4String NOP2 = nop2;
  G4String NOP3 = nop3;
  G4ParticleDefinition* particle = new G4ParticleDefinition(NOP1,         0.0*MeV,       0.0*MeV,         Charge,//para
                    2,              -1,            -1,
                    0,               0,             0,
              NOP2,               0,             0,          Encoding,//para
              true,               -1.0,          nullptr,
             false,           NOP3,          Antiencoding//para
              );
  G4ThreeVector aMomentumDirection(0,0,0);
  G4double aKineticEnergy = kineticEnergy;//para
  G4DynamicParticle* dynamicPar = new G4DynamicParticle(particle, aMomentumDirection, aKineticEnergy);
  G4double aValueTime = 1;
  G4ThreeVector ValuePosition(1.0, 1.0, 1.0);
  G4Track* track = new G4Track(dynamicPar, aValueTime, ValuePosition);
  //G4double globalTime = globalTime;
  track->SetGlobalTime(globalTime);//para
  int trackID = 3;
  track->SetTrackID(trackID);
  G4String boxName1 = "name";
  G4Box* box1 = new G4Box(boxName1, 1.0, 1.0, 1.0);
  G4NistManager* man1 = G4NistManager::Instance();
  G4Material* material1 = man1->FindOrBuildMaterial("G4_AIR");
  G4String name2 = logicalName1;//para
  G4LogicalVolume* fLogical1 = new G4LogicalVolume(box1, material1, name2);
  track->SetLogicalVolumeAtVertex(fLogical1);
  G4Step* stepForTrack = new G4Step();
  G4StepPoint* stepPoint2 = new G4StepPoint();
  G4Material* mat1 = man1->FindOrBuildMaterial("G4_AIR");
  G4MaterialPropertiesTable* propertiesTable = new G4MaterialPropertiesTable();
  G4double Energies[5] = {1,2,3,4,5};
  G4double Values[5] = {1,2,3,4,5};
  size_t VectorLength = 5;
  G4MaterialPropertyVector* mpv = new G4MaterialPropertyVector(Energies, Values, VectorLength);
  G4MaterialPropertyVector* mpv1 = new G4MaterialPropertyVector(Energies, Values, VectorLength);
  propertiesTable->AddProperty("RINDEX", mpv);
  propertiesTable->AddProperty("ABSLENGTH", mpv1);
  mat1->SetMaterialPropertiesTable(propertiesTable);
  stepPoint2->SetMaterial(mat1);
  stepForTrack->SetPreStepPoint(stepPoint2);
  track->SetStep(stepForTrack);

  G4ProcessType aType = atype; 
  G4String aString = astring;
  G4MyProcess* process = new G4MyProcess( aString, aType );//para para
  track->SetCreatorProcess( process );

  track->SetTouchableHandle(touchableHandle);

  sp.SetTrack(track);
//end

  return;
}
