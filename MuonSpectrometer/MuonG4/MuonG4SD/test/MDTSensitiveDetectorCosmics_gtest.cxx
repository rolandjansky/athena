/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "src/MDTSensitiveDetectorCosmics.h"

#include "gtest/gtest.h"

#include "TestTools/initGaudi.h"

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
#include "G4TrackStatus.hh"

#include "G4AtlasTools/DerivedG4PhysicalVolume.h"
#include "G4AtlasTools/DerivedG4SensitiveDetectorTestSetting.h"
#include "G4AtlasTools/DerivedG4Process.h"
#include "G4AtlasTools/DerivedG4SensitiveDetector.h"

//set environment
class GaudiEnvironment : public ::testing::Environment {
  protected:
  virtual void SetUp() override {
    Athena_test::initGaudi("MuonG4SD/optionForTest.txt", m_svcLoc);
  }
  ISvcLocator* m_svcLoc = nullptr;
};
class MDTSensitiveDetectorCosmicstest : public ::testing::Test {
  protected:
    virtual void SetUp() override {
    }

    virtual void TearDown() override {
    }
};
//end of environment setting

TEST_F ( MDTSensitiveDetectorCosmicstest, Initialize )
{
  G4HCofThisEvent hce;
  MDTSensitiveDetectorCosmics sd1("name1", "name1", 78); // all MDT/sMDT chambers used in Run1/2 have maximum 78 tubes
  sd1.Initialize( &hce );
  ASSERT_TRUE(sd1.m_MDTHitColl.isValid()); //check if initialization of m_MDTHitColl is successful
}

TEST_F ( MDTSensitiveDetectorCosmicstest, ProcessHits )
{
  G4Step sp;
  G4TouchableHistory th; 
  G4HCofThisEvent hce;  

  G4double totalenergydeposit = 3.0;
  G4String physicalname = "physicalName";
  G4String logicalname = "logicalName";
  G4int copyno = 1000;
  G4ThreeVector preStepPos = G4ThreeVector(0,0,1);
  G4ThreeVector postStepPos = G4ThreeVector(0,0,2);
  G4double globaltime0 = 0.5;
  G4double kineticenergy0 = 1.5;
  G4double velocity0 = 99.93100;
  G4double globaltime = 5.0;
  G4double kineticenergy = 0.5;
  G4double globaltime1 = 0.5;
  G4double kineticenergy1 = 0.5;
  G4double velocity1 = 99.93100;
  G4double steplength = 1.0;
  G4double charge = 1.0;
  G4int encoding = 22;
  G4int antiencoding = 22;
  G4String astring = "Cerenkov";
  G4ProcessType atype = (G4ProcessType)0;
  G4String nop1 = "opticalphoton";
  G4String nop2 = "opticalphoton";
  G4String nop3 = "photon";
  DerivedG4SensitiveDetectorTestSetting(sp, totalenergydeposit, physicalname, logicalname, copyno, preStepPos, postStepPos, globaltime0, kineticenergy0, velocity0, globaltime, kineticenergy, globaltime1, kineticenergy1, velocity1, steplength, charge, encoding, antiencoding, astring, atype, nop1, nop2, nop3);

//the following lines aims to set a G4StepPoint object for sp as a PreStepPoint since the setting for that in the function DerivedG4SensitiveDetectorTestSetting is not complete
  G4StepPoint* stepPoint0 = new G4StepPoint();
  stepPoint0->SetPosition(preStepPos);//para
  G4NavigationHistory* navigationHistory0 = new G4NavigationHistory();
  G4String boxName0 = "name";
  G4Box* box0 = new G4Box(boxName0, 1.0, 1.0, 1.0);
  G4NistManager* man = G4NistManager::Instance();
  G4Material* material0 = man->FindOrBuildMaterial("G4_AIR");
  G4String name0 = "logicalName";
  G4LogicalVolume* fLogical0 = new G4LogicalVolume(box0, material0, name0);
  G4MySensitiveDetector* SDetector = new G4MySensitiveDetector("SensitiveDetectorName");
  fLogical0->SetSensitiveDetector(SDetector);
  G4String PhysicalName0 = physicalname;//para
  G4VPhysicalVolume* pPhysical0 = nullptr;
  G4MyPhysicalVolume* physicalVolume0 = new G4MyPhysicalVolume(0, G4ThreeVector(0,0,0), PhysicalName0, fLogical0, pPhysical0);
  G4int CopyNo0 = copyno;
  physicalVolume0->SetCopyNo(CopyNo0);//para
  G4int nReplica0 = 2;
  navigationHistory0->SetFirstEntry(physicalVolume0);
  navigationHistory0->NewLevel(physicalVolume0, kNormal, nReplica0);
  navigationHistory0->NewLevel(physicalVolume0, kNormal, nReplica0);
  navigationHistory0->NewLevel(physicalVolume0, kNormal, nReplica0);
  navigationHistory0->NewLevel(physicalVolume0, kNormal, nReplica0);
  navigationHistory0->NewLevel(physicalVolume0, kNormal, nReplica0);
  G4TouchableHistory* touchableHistory0 = new G4TouchableHistory(*navigationHistory0);
  G4TouchableHandle touchableHandle0(touchableHistory0);
  stepPoint0->SetTouchableHandle(touchableHandle0);
  G4double GlobalTime00 = globaltime0;
  G4double KineticEnergy00 = kineticenergy0;
  G4double Velocity00 = velocity0;
  stepPoint0->SetGlobalTime(GlobalTime00);//para
  stepPoint0->SetKineticEnergy(KineticEnergy00);//para
  stepPoint0->SetVelocity(Velocity00);//para

  G4Material* material00 = man->FindOrBuildMaterial("G4_WATER");
  stepPoint0->SetMaterial(material00); 

  sp.SetPreStepPoint(stepPoint0);

//the following lines aims to set a G4StepPoint object for sp as a PostStepPoint since the setting for that in the function DerivedG4SensitiveDetectorTestSetting is not complete
  G4StepPoint* stepPoint = new G4StepPoint();
  stepPoint->SetPosition(preStepPos);//para
  G4NavigationHistory* navigationHistory = new G4NavigationHistory();
  G4String boxName = "name";
  G4Box* box = new G4Box(boxName, 1.0, 1.0, 1.0);
  G4Material* material = man->FindOrBuildMaterial("G4_AIR");
  G4String name = "logicalName";
  G4LogicalVolume* fLogical = new G4LogicalVolume(box, material, name);
  G4String PhysicalName = physicalname;//para
  G4VPhysicalVolume* pPhysical = nullptr;
  G4MyPhysicalVolume* physicalVolume = new G4MyPhysicalVolume(0, G4ThreeVector(0,0,0), PhysicalName, fLogical, pPhysical);
  G4int CopyNo = copyno;
  physicalVolume->SetCopyNo(CopyNo);//para
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

  G4Material* material1 = man->FindOrBuildMaterial("G4_WATER");
  stepPoint->SetMaterial(material1);

//  sp.SetPreStepPoint(stepPoint);
  sp.SetPostStepPoint(stepPoint);

//the following lines aims to set a G4Track object for sp since the setting in the function DerivedG4SensitiveDetectorTestSetting is not complete
  G4double Charge = charge;
  G4int Encoding = encoding;
  G4int Antiencoding = antiencoding;
  G4String NOP1 = "higgs";
  G4String NOP2 = "higgs";
  G4String NOP3 = "higgs";
  G4ParticleDefinition* particle = new G4ParticleDefinition(NOP1,         0.0*MeV,       0.0*MeV,         Charge,//para
                    2,              -1,            -1,
                    0,               0,             0,
              NOP2,               0,             0,          Encoding,//para
              true,               -1.0,          nullptr,
             false,           NOP3,          Antiencoding//para
              );
  G4ThreeVector aMomentumDirection(0,0,0);
  G4double aKineticEnergy = kineticenergy;//para
  G4DynamicParticle* dynamicPar = new G4DynamicParticle(particle, aMomentumDirection, aKineticEnergy);
  G4double aValueTime = 1;
  G4ThreeVector ValuePosition(1.0, 1.0, 1.0);
  G4Track* track = new G4Track(dynamicPar, aValueTime, ValuePosition);
  G4double globalTime = globaltime;
  track->SetGlobalTime(globalTime);//para
  int trackID = 3;
  track->SetTrackID(trackID);
  G4String boxName1 = "name";
  G4Box* box1 = new G4Box(boxName1, 1.0, 1.0, 1.0);
  G4Material* material2 = man->FindOrBuildMaterial("G4_AIR");
  G4String name2 = "logicalname1";//para
  G4LogicalVolume* fLogical1 = new G4LogicalVolume(box1, material2, name2);
  track->SetLogicalVolumeAtVertex(fLogical1);
  G4Step* stepForTrack = new G4Step();
  G4StepPoint* stepPoint2 = new G4StepPoint();
  stepForTrack->SetPreStepPoint(stepPoint2);
  track->SetStep(stepForTrack);
  G4ThreeVector threeVector = G4ThreeVector(1,1,1);
  track->SetVertexPosition(threeVector);

  G4ProcessType aType = atype; 
  G4String aString = astring;
  G4MyProcess* process = new G4MyProcess( aString, aType );//para para
  track->SetCreatorProcess( process );

  track->SetTouchableHandle(touchableHandle);
  track->SetTrackStatus(fStopAndKill);

  sp.SetTrack(track);

  MDTSensitiveDetectorCosmics sd2("name2", "name2", 78); // all MDT/sMDT chambers used in Run1/2 have maximum 78 tubes
  sd2.Initialize( &hce );
  sd2.ProcessHits(&sp, &th );//invoke the memberfunction that is being tested

  int barcode = 0;
  HepMcParticleLink plink(barcode);

  MDTSimHitCollection* a = sd2.m_MDTHitColl.ptr();
  ASSERT_EQ(a->begin()->MDTid(), -189440); //test if the MDTid value of the Hit generated by this member function is right, the same below
  ASSERT_EQ(a->begin()->globalTime(), 0.5);
  ASSERT_EQ(a->begin()->driftRadius(), 0);
  ASSERT_EQ(a->begin()->localPosition(), Amg::Vector3D(0,0,1));
  ASSERT_EQ(a->begin()->particleLink(), plink);
  ASSERT_EQ(a->begin()->stepLength(), 1);
  ASSERT_EQ(a->begin()->energyDeposit(), 3);
  ASSERT_EQ(a->begin()->particleEncoding(), 22);
  ASSERT_EQ(a->begin()->kineticEnergy(), 1.5);

}

TEST_F ( MDTSensitiveDetectorCosmicstest, GetIdentifier )
{
//define a G4HCofThisEvent object 
  G4HCofThisEvent hce; 

//the following line aims to define a complete G4TouchableHistory object
  G4NavigationHistory* navigationHistory = new G4NavigationHistory();
  G4String boxName = "name";
  G4Box* box = new G4Box(boxName, 1.0, 1.0, 1.0);
  G4NistManager* man = G4NistManager::Instance();
  G4Material* material = man->FindOrBuildMaterial("G4_AIR");
  G4String name = "logicalName";
  G4LogicalVolume* fLogical = new G4LogicalVolume(box, material, name);
  G4String PhysicalName = "physical_MDT_Muon::STN[2]_av___impr_Name";
  G4VPhysicalVolume* pPhysical = nullptr;
  G4MyPhysicalVolume* physicalVolume = new G4MyPhysicalVolume(0, G4ThreeVector(0,0,0), PhysicalName, fLogical, pPhysical);
  physicalVolume->SetCopyNo(10100000);
  G4int nReplica = 2;
  navigationHistory->SetFirstEntry(physicalVolume);
  navigationHistory->NewLevel(physicalVolume, kNormal, nReplica);
  navigationHistory->NewLevel(physicalVolume, kNormal, nReplica);
  navigationHistory->NewLevel(physicalVolume, kNormal, nReplica);
  navigationHistory->NewLevel(physicalVolume, kNormal, nReplica);
  navigationHistory->NewLevel(physicalVolume, kNormal, nReplica);
  G4TouchableHistory* touchableHistory = new G4TouchableHistory(*navigationHistory);

  MDTSensitiveDetectorCosmics sd3("name3", "name3", 78); // all MDT/sMDT chambers used in Run1/2 have maximum 78 tubes
  sd3.Initialize( &hce );
  int theID = sd3.GetIdentifier(touchableHistory);

  ASSERT_EQ(theID,-188416);//test if the ID is rightly calculated

}

int main( int argc, char** argv ) {

  auto g=new GaudiEnvironment;
  ::testing::AddGlobalTestEnvironment(g);
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();

}
