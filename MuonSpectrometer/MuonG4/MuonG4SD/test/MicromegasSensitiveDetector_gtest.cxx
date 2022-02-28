/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "src/MicromegasSensitiveDetector.h"

#include "gtest/gtest.h"

#include <vector>

#include "TestTools/initGaudi.h"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"

#include "G4AtlasTools/DerivedG4PhysicalVolume.h"
#include "G4AtlasTools/DerivedG4SensitiveDetectorTestSetting.h"

//set environment
class GaudiEnvironment : public ::testing::Environment {
  protected:
  virtual void SetUp() override {
    Athena_test::initGaudi("MuonG4SD/optionForTest.txt", m_svcLoc);
  }
  ISvcLocator* m_svcLoc = nullptr;
};
class MicromegasSensitiveDetectortest : public ::testing::Test {	
  protected:
    virtual void SetUp() override {
    }

    virtual void TearDown() override {
    }
};
//end of environment setting

TEST_F( MicromegasSensitiveDetectortest, Initialize )
{
  G4HCofThisEvent hce;
  MicromegasSensitiveDetector sd1("name1", "name1" );
  sd1.Initialize( &hce );
  ASSERT_TRUE(sd1.m_MMSimHitCollection.isValid()); //check if initialization of m_MMSimHitCollection is successful
}

TEST_F( MicromegasSensitiveDetectortest, ProcessHits )
{
  G4Step sp;
  G4TouchableHistory th; 
  G4HCofThisEvent hce;

  G4double totalenergydeposit = 3.0;
  std::vector<G4String> physicalname = {"av_407_impr_3_Muon::MM_1-M2L1_pv_16","MM_1muo::ArCO2"};
  G4String logicalname = "BBBBBBBBBTubeGas";
  std::vector<G4int> copynos = {117,1};
  G4ThreeVector preStepPos = G4ThreeVector(0,0,1);
  G4ThreeVector postStepPos = G4ThreeVector(0.1,0,2); // x>0 to compute \phi
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
  G4String nop1 = "gamma";//set particle name as gamma
  G4String nop2 = "gamma";
  G4String nop3 = "gamma";
  DerivedG4SensitiveDetectorTestSetting(sp, totalenergydeposit, physicalname, logicalname, copynos, preStepPos, postStepPos, globaltime0, kineticenergy0, velocity0, globaltime, kineticenergy, globaltime1, kineticenergy1, velocity1, steplength, charge, encoding, antiencoding, astring, atype, nop1, nop2, nop3);//invoking of this function aims to setting testing environment

//decorate sp with a G4StepPoint object. Require to set logical name correctly, since the function above sets it to a non-mutatable (hardcoded) string
  G4StepPoint* stepPoint = new G4StepPoint();
  stepPoint->SetPosition(preStepPos);//para
  G4NavigationHistory* navigationHistory = new G4NavigationHistory();
  G4String boxName = "name";
  G4Box* box = new G4Box(boxName, 1.0, 1.0, 1.0);
  G4NistManager* man = G4NistManager::Instance();
  G4Material* material = man->FindOrBuildMaterial("G4_AIR");
  G4String name = "Muon::MM_1-M2L1"; // Logical name required by MM SensitiveDetector
  G4LogicalVolume* fLogical = new G4LogicalVolume(box, material, name);
  G4String PhysicalName = physicalname[0];//para
  G4VPhysicalVolume* pPhysical = nullptr;
  G4MyPhysicalVolume* physicalVolume = new G4MyPhysicalVolume(0, G4ThreeVector(0,0,0), PhysicalName, fLogical, pPhysical);
  G4int CopyNo = copynos[0];
  physicalVolume->SetCopyNo(CopyNo);//para
  G4int nReplica = 2;
  navigationHistory->SetFirstEntry(physicalVolume);
  if (physicalname.size()==1) {
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
    for( const auto& name : physicalname) {
      if (skip) { skip=false; ++idx; continue; }//skip first entry
      G4MyPhysicalVolume* parentPhysVol = new G4MyPhysicalVolume(0, G4ThreeVector(0,0,0), name, fLogical, pPhysical);
      parentPhysVol->SetCopyNo(copynos[idx]);
      ++idx;
      navigationHistory->NewLevel(parentPhysVol, kNormal, nReplica);
    }
  }
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

//set G4Track for the step since it was not done in the previous function
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
  stepForTrack->SetTotalEnergyDeposit(5.0);
  track->SetStep(stepForTrack);
  G4ThreeVector threeVector = G4ThreeVector(1,1,1);
  track->SetVertexPosition(threeVector);

  sp.SetTrack(track);

  MicromegasSensitiveDetector sd2("name2", "name2" );
  sd2.Initialize( &hce );
  sd2.ProcessHits(&sp, &th );//invoke the tested member function

  int barcode = 0;
  HepMcParticleLink plink(barcode);

  MMSimHitCollection* a = sd2.m_MMSimHitCollection.ptr();
  ASSERT_EQ(a->begin()->MMId(), 528404);//test if the MMId alue of the Hit generated by this member function is right, the same below
  ASSERT_EQ(a->begin()->globalTime(), 0.5);
  ASSERT_EQ(a->begin()->globalPosition(), Amg::Vector3D(0.1,0,2));
  ASSERT_EQ(a->begin()->particleEncoding(), 22);
  ASSERT_EQ(a->begin()->kineticEnergy(), 0.5);
  ASSERT_EQ(a->begin()->globalDirection(), Amg::Vector3D(0,0,0));
  ASSERT_EQ(a->begin()->depositEnergy(), 5);
  ASSERT_EQ(a->begin()->particleLink(), plink);

}

int main( int argc, char** argv ) {

  auto g=new GaudiEnvironment;
  ::testing::AddGlobalTestEnvironment(g);
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();

}

