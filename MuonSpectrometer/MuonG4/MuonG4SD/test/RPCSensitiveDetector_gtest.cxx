/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "src/RPCSensitiveDetector.h"

#include "gtest/gtest.h"

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
class RPCSensitiveDetectortest : public ::testing::Test {	
  protected:
    virtual void SetUp() override {
    }

    virtual void TearDown() override {
    }
};
//end of environment setting

TEST_F ( RPCSensitiveDetectortest, Initialize )
{
  G4HCofThisEvent hce;
  RPCSensitiveDetector sd1("name1", "name1" );
  sd1.Initialize( &hce );
  ASSERT_TRUE(sd1.m_myRPCHitColl.isValid()); //check if initialization of m_myRPCHitColl is successful

}

TEST_F ( RPCSensitiveDetectortest, ProcessHits )
{
  G4Step sp;
  G4TouchableHistory th; 
  G4HCofThisEvent hce;  

  G4double totalenergydeposit = 3.0;
  G4String physicalname = "physical_RPC_Muon::STN[2]_av___impr_Name";//set a proper name for the physical volume, which will decide which block of the tested class would be executed
  G4String logicalname = "BBBBBBBBBTubeGas";
  G4int copyno = 10100000;
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
  DerivedG4SensitiveDetectorTestSetting(sp, totalenergydeposit, physicalname, logicalname, copyno, preStepPos, postStepPos, globaltime0, kineticenergy0, velocity0, globaltime, kineticenergy, globaltime1, kineticenergy1, velocity1, steplength, charge, encoding, antiencoding, astring, atype, nop1, nop2, nop3);//invoking of this function aims to setting testing environment.

  RPCSensitiveDetector sd2("name2", "name2" );
  sd2.Initialize( &hce );//initialize the hit collection m_myRPCHitColl
  sd2.ProcessHits(&sp, &th );//invoke the tested member function

  int barcode = 0;
  HepMcParticleLink plink(barcode);

  RPCSimHitCollection* a = sd2.m_myRPCHitColl.ptr();

//test the first newly-generated hit that has been stored in the RPCSimHitCollection collector 
  ASSERT_EQ(a->begin()->RPCid(), -8577024); //test if the RPCid value of the Hit generated by this member function is right, the same below
  ASSERT_EQ(a->begin()->globalTime(), 0.5);
  ASSERT_EQ(a->begin()->preLocalPosition(), Amg::Vector3D(0,0,1));
  ASSERT_EQ(a->begin()->particleLink(), plink);
  ASSERT_EQ(a->begin()->postLocalPosition(), Amg::Vector3D(0,0,2));
  ASSERT_EQ(a->begin()->energyDeposit(), 3.0);
  ASSERT_EQ(a->begin()->stepLength(), 1);
  ASSERT_EQ(a->begin()->particleEncoding(), 22);
  ASSERT_EQ(a->begin()->kineticEnergy(), 1.5);

//test the second newly-generated hit that has been stored in the RPCSimHitCollection collector
  ASSERT_EQ(((a->begin())+1)->RPCid(), -188416); //test if the RPCid value of the Hit generated by this member function is right, the same below
  ASSERT_EQ(((a->begin())+1)->globalTime(), 0.5);
  ASSERT_EQ(((a->begin())+1)->preLocalPosition(), Amg::Vector3D(0,0,1));
  ASSERT_EQ(((a->begin())+1)->particleLink(), plink);
  ASSERT_EQ(((a->begin())+1)->postLocalPosition(), Amg::Vector3D(0,0,2));
  ASSERT_EQ(((a->begin())+1)->energyDeposit(), 3.0);
  ASSERT_EQ(((a->begin())+1)->stepLength(), 1);
  ASSERT_EQ(((a->begin())+1)->particleEncoding(), 22);
  ASSERT_EQ(((a->begin())+1)->kineticEnergy(), 1.5);
}

int main( int argc, char** argv ) {

  auto g=new GaudiEnvironment;
  ::testing::AddGlobalTestEnvironment(g);
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();

}

