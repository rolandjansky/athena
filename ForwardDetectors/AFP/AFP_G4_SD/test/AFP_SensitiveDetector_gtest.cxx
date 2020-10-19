/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "src/AFP_SensitiveDetector.h"

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
#include <assert.h>

//set environment
class GaudiEnvironment : public ::testing::Environment {
  protected:
  virtual void SetUp() override {
    Athena_test::initGaudi("AFP_G4_SD/optionForTest.txt", m_svcLoc);
  }
  ISvcLocator* m_svcLoc = nullptr;
};
class AFP_SensitiveDetectortest : public ::testing::Test {	
  protected:
    virtual void SetUp() override {
    }

    virtual void TearDown() override {
    }
};
//end of environment setting

TEST_F( AFP_SensitiveDetectortest, Initialize )
{
  G4HCofThisEvent hce;
  AFP_SensitiveDetector sd1("name1", "name1", "name1");
  sd1.Initialize(&hce);
  ASSERT_TRUE(sd1.m_pTDSimHitCollection.isValid());
  ASSERT_TRUE(sd1.m_pSIDSimHitCollection.isValid());
}

/*TEST_F( AFP_SensitiveDetectortest, ProcessHits1 )//this test fixture aims to test the specific block of ProcessHits(i.e.,if ( (bRes=VolumeName.contains("TDQuarticBar[")) ) )
{
  G4HCofThisEvent hce;
  G4Step sp;
  G4TouchableHistory th;

  G4double totalenergydeposit = 0.8;
  std::vector<G4String> physicalname = {"physicsTDQuarticBar[9]"};
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

  AFP_SensitiveDetector sd21("name21", "name21", "name21");
  sd21.Initialize(&hce);
  sd21.ProcessHits(&sp, &th);

  AFP_TDSimHitCollection* a = sd21.m_pTDSimHitCollection.ptr();
  ASSERT_TRUE(a->begin()->m_nHitID==0); //test the HitID value of the 1st Hit, the same below
  ASSERT_TRUE((a->end()-1)->m_nHitID==0); // test the HitID value of the last Hit, the same below
  ASSERT_TRUE(a->begin()->m_nTrackID==3);
  ASSERT_TRUE((a->end()-1)->m_nTrackID==3);
  ASSERT_TRUE(a->begin()->m_nParticleEncoding==22);
  ASSERT_TRUE((a->end()-1)->m_nParticleEncoding==22);
  ASSERT_FLOAT_EQ(a->begin()->m_fKineticEnergy,1.5);
  ASSERT_FLOAT_EQ((a->end()-1)->m_fKineticEnergy,1.5);
  ASSERT_FLOAT_EQ(a->begin()->m_fEnergyDeposit,0.8);
  ASSERT_FLOAT_EQ((a->end()-1)->m_fEnergyDeposit,0.8);
  ASSERT_FLOAT_EQ(a->begin()->m_fWaveLength,0.00030956);
  ASSERT_FLOAT_EQ((a->end()-1)->m_fWaveLength,0.00030079353);
  ASSERT_FLOAT_EQ(a->begin()->m_fPreStepX,0);
  ASSERT_FLOAT_EQ((a->end()-1)->m_fPreStepX,0);
  ASSERT_FLOAT_EQ(a->begin()->m_fPreStepY,0);
  ASSERT_FLOAT_EQ((a->end()-1)->m_fPreStepY,0);
  ASSERT_FLOAT_EQ(a->begin()->m_fPreStepZ,1.8150871);
  ASSERT_FLOAT_EQ((a->end()-1)->m_fPreStepZ,1.1110719);
  ASSERT_FLOAT_EQ(a->begin()->m_fPostStepX,-0.44258225);
  ASSERT_FLOAT_EQ((a->end()-1)->m_fPostStepX,-0.17657205);
  ASSERT_FLOAT_EQ(a->begin()->m_fPostStepY,0.49302673);
  ASSERT_FLOAT_EQ((a->end()-1)->m_fPostStepY,0.66264898);
  ASSERT_FLOAT_EQ(a->begin()->m_fPostStepZ,2.564117);
  ASSERT_FLOAT_EQ((a->end()-1)->m_fPostStepZ,1.8388897);
  ASSERT_FLOAT_EQ(a->begin()->m_fGlobalTime,508.15649);
  ASSERT_FLOAT_EQ((a->end()-1)->m_fGlobalTime,1570.1189);
  ASSERT_TRUE(a->begin()->m_nStationID==727);
  ASSERT_TRUE((a->end()-1)->m_nStationID==727);
  ASSERT_TRUE(a->begin()->m_nDetectorID==9);
  ASSERT_TRUE((a->end()-1)->m_nDetectorID==9);
  ASSERT_TRUE(a->begin()->m_nSensitiveElementID==-1);
  ASSERT_TRUE((a->end()-1)->m_nSensitiveElementID==-1);
}*/

TEST_F( AFP_SensitiveDetectortest, ProcessHits2 )//this test aims to test the specific block of ProcessHits(i.e.,else if (VolumeName.contains("SIDSensor") || (bIsSIDAuxVSID=VolumeName.contains("SIDVacuumSensor"))))
{
  G4HCofThisEvent hce;
  G4Step sp;
  G4TouchableHistory th;

  G4double totalenergydeposit = 8;
  std::vector<G4String> physicalname = {"phy01lSIDSensor[9]"};
  G4String logicalname = "BBBBBBBBBTubeGas";
  std::vector<G4int> copynos = {2009};
  G4ThreeVector preStepPos = G4ThreeVector(1,1,1);
  G4ThreeVector postStepPos = G4ThreeVector(2,2,2);
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

  AFP_SensitiveDetector sd22("name22", "name22", "name22");
  sd22.Initialize(&hce);
  sd22.ProcessHits(&sp, &th);

  AFP_SIDSimHitCollection* a = sd22.m_pSIDSimHitCollection.ptr();
  ASSERT_TRUE(a->begin()->m_nHitID==0); //test the HitID value of the 1st Hit, the same below
  ASSERT_TRUE((a->end()-1)->m_nHitID==0); //test the HitID value of the last Hit, the same below
  ASSERT_TRUE(a->begin()->m_nTrackID==3);
  ASSERT_TRUE((a->end()-1)->m_nTrackID==3);
  ASSERT_TRUE(a->begin()->m_nParticleEncoding==22);
  ASSERT_TRUE((a->end()-1)->m_nParticleEncoding==22);
  ASSERT_FLOAT_EQ(a->begin()->m_fKineticEnergy,1.5);
  ASSERT_FLOAT_EQ((a->end()-1)->m_fKineticEnergy,1.5);
  ASSERT_FLOAT_EQ(a->begin()->m_fEnergyDeposit,0.);
  ASSERT_FLOAT_EQ((a->end()-1)->m_fEnergyDeposit,0.39999962);
  ASSERT_FLOAT_EQ(a->begin()->m_fPreStepX,1);
  ASSERT_FLOAT_EQ((a->end()-1)->m_fPreStepX,1);
  ASSERT_FLOAT_EQ(a->begin()->m_fPreStepY,1);
  ASSERT_FLOAT_EQ((a->end()-1)->m_fPreStepY,1);
  ASSERT_FLOAT_EQ(a->begin()->m_fPreStepZ,1);
  ASSERT_FLOAT_EQ((a->end()-1)->m_fPreStepZ,1);
  ASSERT_FLOAT_EQ(a->begin()->m_fPostStepX,2);
  ASSERT_FLOAT_EQ((a->end()-1)->m_fPostStepX,2);
  ASSERT_FLOAT_EQ(a->begin()->m_fPostStepY,2);
  ASSERT_FLOAT_EQ((a->end()-1)->m_fPostStepY,2);
  ASSERT_FLOAT_EQ(a->begin()->m_fPostStepZ,2);
  ASSERT_FLOAT_EQ((a->end()-1)->m_fPostStepZ,2);
  ASSERT_FLOAT_EQ(a->begin()->m_fGlobalTime,500);
  ASSERT_FLOAT_EQ((a->end()-1)->m_fGlobalTime,500);
  ASSERT_TRUE(a->begin()->m_nStationID==1);
  ASSERT_TRUE((a->end()-1)->m_nStationID==1);
  ASSERT_TRUE(a->begin()->m_nDetectorID==9);
  ASSERT_TRUE((a->end()-1)->m_nDetectorID==9);
  ASSERT_TRUE(a->begin()->m_bIsAuxVSID==0);
  ASSERT_TRUE((a->end()-1)->m_bIsAuxVSID==0);
  ASSERT_EQ(a->begin()->m_nPixelRow,7);
  ASSERT_EQ((a->end()-1)->m_nPixelRow,7);
  ASSERT_EQ(a->begin()->m_nPixelCol,0);
  ASSERT_EQ((a->end()-1)->m_nPixelCol,20);
  ASSERT_EQ(sd22.m_nNumberOfSIDSimHits,21);
  ASSERT_EQ(sd22.m_nNOfSIDSimHits[1],21);
}

TEST_F( AFP_SensitiveDetectortest, StartOfAthenaEvent)
{
  G4HCofThisEvent hce;
  AFP_SensitiveDetector sd3("name3", "name3", "name3");
  sd3.Initialize(&hce);
  sd3.StartOfAthenaEvent();

//the following lines aim to test the values of the arrays(m_nNOfSIDSimHits and m_nNOfTDSimHits) initialized by the member function StartOfAthenaEvent()
  for (int i=0;i<4;i++)
  {
    ASSERT_TRUE(sd3.m_nNOfSIDSimHits[i]==0);
  }
  for( int i=0; i < 4; i++)
  {
    for( int j=0; j < 32; j++)
    {
      ASSERT_TRUE(sd3.m_nNOfTDSimHits[i][j]==0);
    }
  }
}

TEST_F( AFP_SensitiveDetectortest, EndOfAthenaEvent)
{
  G4HCofThisEvent hce;
  AFP_SensitiveDetector sd4("name4", "name4", "name4");
  sd4.Initialize(&hce);
  sd4.EndOfAthenaEvent();

//the following line saim to test the values of the arrays(m_nNOfSIDSimHits and m_nNOfTDSimHits) assigned by the member function EndOfAthenaEvent()
  for (int i=0;i<4;i++)
  {
    ASSERT_TRUE(sd4.m_nNOfSIDSimHits[i]==0);
  }
  for( int i=0; i < 4; i++)
  {
    for( int j=0; j < 32; j++)
    {
      ASSERT_TRUE(sd4.m_nNOfTDSimHits[i][j]==0);
    }
  }
}

int main( int argc, char** argv ) {

  auto g=new GaudiEnvironment;
  ::testing::AddGlobalTestEnvironment(g);
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();

}
