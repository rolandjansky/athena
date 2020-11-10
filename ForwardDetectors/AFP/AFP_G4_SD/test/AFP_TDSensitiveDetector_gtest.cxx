/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "src/AFP_TDSensitiveDetector.cxx"

#include "gtest/gtest.h"

#include <vector> 

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

#include "G4AtlasTools/DerivedG4PhysicalVolume.h"
#include "G4AtlasTools/DerivedG4SensitiveDetectorTestSetting.h"

//set environment
class GaudiEnvironment : public ::testing::Environment {
  protected:
  virtual void SetUp() override {
    Athena_test::initGaudi("AFP_G4_SD/optionForTest.txt", m_svcLoc);
  }
  ISvcLocator* m_svcLoc = nullptr;
};
class AFP_TDSensitiveDetectortest : public ::testing::Test {	
  protected:
    virtual void SetUp() override {
    }

    virtual void TearDown() override {
    }
};
//end of environment setting

TEST_F( AFP_TDSensitiveDetectortest, Initialize )
{
  G4HCofThisEvent hce;
  AFP_TDSensitiveDetector sd1( "name1", "name1" );
  sd1.Initialize(&hce);
  ASSERT_TRUE(sd1.m_HitColl.isValid());
}

TEST_F( AFP_TDSensitiveDetectortest, ProcessHits )
{
  G4HCofThisEvent hce;
  G4Step sp;
  G4TouchableHistory th;

  G4double totalenergydeposit = 0.8;
  std::vector<G4String> physicalname = {"phy01csTDQuarticBar[9]"};
  G4String logicalname = "BBBBBBBBBTubeGas";
  std::vector<G4int> copynos = {2009};
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
  DerivedG4SensitiveDetectorTestSetting(sp, totalenergydeposit, physicalname, logicalname, copynos, preStepPos, postStepPos, globaltime0, kineticenergy0, velocity0, globaltime, kineticenergy, globaltime1, kineticenergy1, velocity1, steplength, charge, encoding, antiencoding, astring, atype, nop1, nop2, nop3);

  AFP_TDSensitiveDetector sd2("name2", "name2");
  sd2.Initialize(&hce);
  sd2.ProcessHits(&sp, &th);

  AFP_TDSimHitCollection * a = sd2.m_HitColl.ptr();
  ASSERT_TRUE(a->begin()->m_nHitID==0); //test the HitID value of the Hit, the same below
  ASSERT_TRUE(a->begin()->m_nTrackID==3);
  ASSERT_TRUE(a->begin()->m_nParticleEncoding==22);
  ASSERT_FLOAT_EQ(a->begin()->m_fKineticEnergy,1.5);
  ASSERT_FLOAT_EQ(a->begin()->m_fEnergyDeposit,0.8);
  ASSERT_FLOAT_EQ(a->begin()->m_fWaveLength,0.00026688879);
  ASSERT_FLOAT_EQ(a->begin()->m_fPreStepX,0);
  ASSERT_FLOAT_EQ(a->begin()->m_fPreStepY,0);
  ASSERT_FLOAT_EQ(a->begin()->m_fPreStepZ,1.29266);
  ASSERT_FLOAT_EQ(a->begin()->m_fPostStepX,0.18366571);
  ASSERT_FLOAT_EQ(a->begin()->m_fPostStepY,-0.74110413);
  ASSERT_FLOAT_EQ(a->begin()->m_fPostStepZ,1.9384398);
  ASSERT_FLOAT_EQ(a->begin()->m_fGlobalTime,1286.6235);
  ASSERT_TRUE(a->begin()->m_nStationID==1);
  ASSERT_TRUE(a->begin()->m_nDetectorID==9);
  ASSERT_TRUE(a->begin()->m_nSensitiveElementID==-1);
}

TEST_F( AFP_TDSensitiveDetectortest, StartOfAthenaEvent )
{
  G4HCofThisEvent hce;
  AFP_TDSensitiveDetector sd3("name3","name3");
  sd3.Initialize(&hce);
  sd3.StartOfAthenaEvent();

//the following lines aim to test the values of the array(m_nNOfTDSimHits) and member variable(m_nNumberOfTDSimHits) initialized by the member function StartOfAthenaEvent()
  ASSERT_TRUE(sd3.m_nNumberOfTDSimHits==0);
  for( int i=0; i < 4; i++)
    {
      for( int j=0; j < 32; j++)
        {
          ASSERT_TRUE(sd3.m_nNOfTDSimHits[i][j] == 0);
        }
    }
}

TEST_F( AFP_TDSensitiveDetectortest, EndOfAthenaEvent )
{
  G4HCofThisEvent hce;
  AFP_TDSensitiveDetector sd4( "name4", "name4" );
  sd4.Initialize(&hce);
  sd4.EndOfAthenaEvent(); 

//the following lines aim to test the values of the array(m_nNOfTDSimHits) and member variable(m_nEventNumber) assigned by the member function EndOfAthenaEvent()
  ASSERT_TRUE( sd4.m_nEventNumber == 1 );
  ASSERT_TRUE( sd4.m_nNumberOfTDSimHits == 0 );
  for( int i=0; i < 4; i++){
    for( int j=0; j < 32; j++){
      ASSERT_TRUE( sd4.m_nNOfTDSimHits[i][j] == 0);
    }
  }
}

TEST_F( AFP_TDSensitiveDetectortest, AddHit )
{
  G4HCofThisEvent hce;
  AFP_TDSensitiveDetector sd5( "name5", "name5" );
  sd5.Initialize(&hce);

  int HitID = 1;
  int TrackID = 1;
  int ParticleEncoding = 10;
  float KineticEnergy = 5.0;
  float EnergyDeposit = 5.0;
  float WaveLength = 50.0;
  float PreStepX = 1.0;
  float PreStepY = 1.0;
  float PreStepZ = 1.0;
  float PostStepX = 2.0;
  float PostStepY = 2.0;
  float PostStepZ = 2.0;
  float GlobalTime = 5.0;
  int StationID = 4;
  int DetectorID = 9;
  int SensitiveElementID = 5;
  sd5.AddHit( HitID, TrackID, ParticleEncoding, KineticEnergy, EnergyDeposit, WaveLength, PreStepX, PreStepY, PreStepZ, PostStepX, PostStepY, PostStepZ, GlobalTime, StationID, DetectorID, SensitiveElementID);

  AFP_TDSimHitCollection * a = sd5.m_HitColl.ptr();
  ASSERT_EQ( a->begin()->m_nHitID, 1 ); //test the HitID value of the Hit added by the member function AddHit, the same below
  ASSERT_EQ( a->begin()->m_nTrackID, 1 );
  ASSERT_EQ( a->begin()->m_nParticleEncoding, 10 );
  ASSERT_EQ( a->begin()->m_fKineticEnergy, 5.0 );
  ASSERT_EQ( a->begin()->m_fEnergyDeposit, 5.0 );
  ASSERT_EQ( a->begin()->m_fWaveLength, 50.0 );
  ASSERT_EQ( a->begin()->m_fPreStepX, 1.0 );
  ASSERT_EQ( a->begin()->m_fPreStepY, 1.0 );
  ASSERT_EQ( a->begin()->m_fPreStepZ, 1.0 );
  ASSERT_EQ( a->begin()->m_fPostStepX, 2.0 );
  ASSERT_EQ( a->begin()->m_fPostStepY, 2.0 );
  ASSERT_EQ( a->begin()->m_fPostStepZ, 2.0 );
  ASSERT_EQ( a->begin()->m_fGlobalTime, 5.0 );
  ASSERT_EQ( a->begin()->m_nStationID, 4 );
  ASSERT_EQ( a->begin()->m_nDetectorID, 9 );
  ASSERT_EQ( a->begin()->m_nSensitiveElementID, 5 );
}

int main( int argc, char** argv ) {

  auto g=new GaudiEnvironment;
  ::testing::AddGlobalTestEnvironment(g);
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();

}
