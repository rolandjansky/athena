/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "src/sTGCSensitiveDetector.h"

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
class sTGCSensitiveDetectortest : public ::testing::Test {	
  protected:
    virtual void SetUp() override {
    }

    virtual void TearDown() override {
    }
};
//end of environment setting

TEST_F ( sTGCSensitiveDetectortest, Initialize )
{
  G4HCofThisEvent hce;
  sTGCSensitiveDetector sd1("name1", "name1" );
  sd1.Initialize( &hce );
  ASSERT_TRUE(sd1.m_sTGCSimHitCollection.isValid()); //check if initialization of m_sTGCSimHitCollection is successful
}

TEST_F ( sTGCSensitiveDetectortest, ProcessHits )
{
  G4Step sp;
  G4TouchableHistory th; 
  G4HCofThisEvent hce;

  G4double totalenergydeposit = 3.0;
  std::vector<G4String> physicalname = {"x","Muon::sTGC_1-QS2P"};
  G4String logicalname = "BBBBBBBBBTubeGas";
  std::vector<G4int> copynos = {1,11815};
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
  G4String nop1 = "gamma";//set particle name as gamma
  G4String nop2 = "gamma";
  G4String nop3 = "gamma";
  DerivedG4SensitiveDetectorTestSetting(sp, totalenergydeposit, physicalname, logicalname, copynos, preStepPos, postStepPos, globaltime0, kineticenergy0, velocity0, globaltime, kineticenergy, globaltime1, kineticenergy1, velocity1, steplength, charge, encoding, antiencoding, astring, atype, nop1, nop2, nop3);//invoking of this function aims to setting testing environment.

//the following lines aims to set a G4StepPoint object for sp as a PreStepPoint since the setting for that in the function DerivedG4SensitiveDetectorTestSetting is not complete
  G4StepPoint* stepPoint0 = new G4StepPoint();
  stepPoint0->SetPosition(preStepPos);//para
  G4NavigationHistory* navigationHistory0 = new G4NavigationHistory();
  G4String boxName0 = "name";
  G4Box* box0 = new G4Box(boxName0, 1.0, 1.0, 1.0);
  G4NistManager* man = G4NistManager::Instance();
  G4Material* material0 = man->FindOrBuildMaterial("G4_AIR");
  G4String name0 = "logical-TLNPame";
  G4LogicalVolume* fLogical0 = new G4LogicalVolume(box0, material0, name0);
  G4String PhysicalName0 = physicalname[0];//para
  G4VPhysicalVolume* pPhysical0 = nullptr;
  G4MyPhysicalVolume* physicalVolume0 = new G4MyPhysicalVolume(0, G4ThreeVector(0,0,0), PhysicalName0, fLogical0, pPhysical0);
  G4int CopyNo0 = copynos[0];
  physicalVolume0->SetCopyNo(CopyNo0);//para
  G4int nReplica = 2;
  navigationHistory0->SetFirstEntry(physicalVolume0);
  if (physicalname.size()==1) {
    // temporary workaround
    navigationHistory0->NewLevel(physicalVolume0, kNormal, nReplica);
    navigationHistory0->NewLevel(physicalVolume0, kNormal, nReplica);
    navigationHistory0->NewLevel(physicalVolume0, kNormal, nReplica);
    navigationHistory0->NewLevel(physicalVolume0, kNormal, nReplica);
    navigationHistory0->NewLevel(physicalVolume0, kNormal, nReplica);
  }
  else {
    if(copynos.size()<physicalname.size()) {
      copynos.resize(physicalname.size());
    }
    bool skip(true);
    size_t idx(0);
    for( const auto& name : physicalname) {
      if (skip) { skip=false; ++idx; continue; }//skip first entry
      G4MyPhysicalVolume* parentPhysVol = new G4MyPhysicalVolume(0, G4ThreeVector(0,0,0), name, fLogical0, pPhysical0);
      parentPhysVol->SetCopyNo(copynos[idx]);
      ++idx;
      navigationHistory0->NewLevel(parentPhysVol, kNormal, nReplica);
    }
  }
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

  sTGCSensitiveDetector sd2("name2", "name2" );
  sd2.Initialize( &hce );//initialize the hit collection m_sTGCSimHitCollection
  sd2.ProcessHits(&sp, &th );//invoke the tested member function

  int barcode = 0;
  HepMcParticleLink plink(barcode);

  sTGCSimHitCollection* a = sd2.m_sTGCSimHitCollection.ptr();
  ASSERT_EQ(a->begin()->sTGCId(), -364542); //test if the sTGCId value of the Hit generated by this member function is right, the same below
  ASSERT_EQ(a->begin()->globalTime(), 0.5);
  ASSERT_EQ(a->begin()->globalPosition(), Amg::Vector3D(0,0,2));
  ASSERT_EQ(a->begin()->particleEncoding(), 22);
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

