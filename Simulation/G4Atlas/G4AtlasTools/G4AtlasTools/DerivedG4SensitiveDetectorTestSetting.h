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

#include "G4MyProcess.h"

void DerivedG4SensitiveDetectorTestSetting(G4Step& sp, G4double& totalenergydeposit, G4String& physicalname, G4int& copyno, G4ThreeVector& preStepPos, G4ThreeVector& postStepPos, G4double& globaltime0, G4double& kineticenergy0, G4double& velocity0, G4double& globaltime1, G4double& kineticenergy1, G4double& velocity1, G4double& steplength, G4double& charge, G4int& encoding, G4int& antiencoding, G4String& astring, G4ProcessType& atype)
{
//decorate sp with the variable called TotalEnergyDeposit
  G4double TotalEnergyDeposit = totalenergydeposit;//para
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
  G4LogicalVolume fLogical(box, material, name);
  G4String PhysicalName = physicalname;//para
  G4VPhysicalVolume* pPhysical = NULL;
  G4MyPhysicalVolume* physicalVolume = new G4MyPhysicalVolume(0, G4ThreeVector(0,0,0), PhysicalName, &fLogical, pPhysical);
  G4int CopyNo = copyno;
  physicalVolume->SetCopyNo(copyno);//para
  G4int nReplica = 2;
  navigationHistory->SetFirstEntry(physicalVolume);
  navigationHistory->NewLevel(physicalVolume, kNormal, nReplica);
  navigationHistory->NewLevel(physicalVolume, kNormal, nReplica);
  navigationHistory->NewLevel(physicalVolume, kNormal, nReplica);
  navigationHistory->NewLevel(physicalVolume, kNormal, nReplica);
  navigationHistory->NewLevel(physicalVolume, kNormal, nReplica);
  G4TouchableHistory* touchableHistory = new G4TouchableHistory(*navigationHistory);
  G4TouchableHandle touchableHandle(touchableHistory);
  stepPoint->SetTouchableHandle(touchableHandle);
  G4double GlobalTime0 = globaltime0;
  G4double KineticEnergy0 = kineticenergy0;
  G4double Velocity0 = velocity0;
  stepPoint->SetGlobalTime(GlobalTime0);//para
  stepPoint->SetKineticEnergy(KineticEnergy0);//para
  stepPoint->SetVelocity(Velocity0);//para

  sp.SetPreStepPoint(stepPoint);
//end

//decorate sp with another G4StepPoint object
  G4StepPoint* stepPoint1 = new G4StepPoint();
//  G4ThreeVector postStepPos(0, 0, 2);
  stepPoint1->SetPosition(postStepPos);//para
  G4double Velocity1 = velocity1;
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
G4ParticleDefinition* particle = new G4ParticleDefinition("anyon",         0.0*MeV,       0.0*MeV,         Charge,//para
                    2,              -1,            -1,
                    0,               0,             0,
              "anyon",               0,             0,          Encoding,//para
              true,               -1.0,          NULL,
             false,           "anyon",          Antiencoding//para
              );
  G4ThreeVector aMomentumDirection(0,0,0);
  G4double aKineticEnergy = kineticenergy1;//para
  G4DynamicParticle* dynamicPar = new G4DynamicParticle(particle, aMomentumDirection, aKineticEnergy);
  G4double aValueTime = 1;
  G4ThreeVector ValuePosition(1.0, 1.0, 1.0);
  G4Track* track = new G4Track(dynamicPar, aValueTime, ValuePosition);
  G4double globalTime = globaltime1;
  track->SetGlobalTime(globalTime);//para
  int trackID = 3;
  track->SetTrackID(trackID);
  G4String boxName1 = "name";
  G4Box* box1 = new G4Box(boxName1, 1.0, 1.0, 1.0);
  G4NistManager* man1 = G4NistManager::Instance();
  G4Material* material1 = man1->FindOrBuildMaterial("G4_AIR");
  G4String name2 = "name2";
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

  sp.SetTrack(track);
//end

  return;
}
