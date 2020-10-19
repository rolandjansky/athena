/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "src/ZDC_StripSD.h"
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
   Athena_test::initGaudi("ZDC_SD/optionForTest.txt", m_svcLoc);
  }
  ISvcLocator* m_svcLoc = nullptr;
};
class ZDC_StripSDtest : public ::testing::Test {	
  protected:
    virtual void SetUp() override {
    }

    virtual void TearDown() override {
    }
};
//end of environment setting

TEST_F( ZDC_StripSDtest, Initialize )
{
  G4HCofThisEvent hce;
  ZDC_StripSD sd1( "name1", "name1" );
  sd1.Initialize(&hce);
  ASSERT_TRUE(sd1.m_HitColl.isValid());
}

TEST_F( ZDC_StripSDtest, ProcessHits )
{
  G4HCofThisEvent hce;
  G4Step sp;
  G4TouchableHistory th;

  G4double totalenergydeposit = 0.8;
  std::vector<G4String> physicalname = {"physicsTDQuarticBar[9]"};
  G4String logicalname = "BBBBBBBBBTubeGas";
  std::vector<G4int> copynos = {11000};
  G4ThreeVector preStepPos = G4ThreeVector(0,0,1);
  G4ThreeVector postStepPos = G4ThreeVector(0,0,2);
  G4double globaltime0 = 0.5;
  G4double kineticenergy0 = 1.5;
  G4double velocity0 = 2500;
  G4double globaltime = 5.0;
  G4double kineticenergy = 0.5;
  G4double globaltime1 = 0.5;
  G4double kineticenergy1 = 0.5;
  G4double velocity1 = 2500;
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

  ZDC_StripSD sd2("name2", "name2");
  sd2.Initialize(&hce);
  sd2.StartOfAthenaEvent();
  sd2.ProcessHits(&sp, &th);

  ASSERT_FLOAT_EQ(sd2.m_Edep_Cherenkov_Strip[0][0], 1.4782044e-05); //Based on my setting, Side=0, Zloc=0, so m_Edep_Cherenkov_Strip[0][0] was accumulated and finally its value should be 1.4782044e-05
  ASSERT_EQ(sd2.m_NPhoton_Cherenkov_Strip[0][0], 5); //Similarly, based on my setting, Side=0, Zloc=0, so m_NPhoton_Cherenkov_Strip[0][0] was accumulated for five times and finally its value should be 5
}

TEST_F( ZDC_StripSDtest, StartOfAthenaEvent )
{
  G4HCofThisEvent hce;
  ZDC_StripSD sd3( "name3", "name3" );
  sd3.Initialize(&hce);
  sd3.StartOfAthenaEvent();

  for (int I=0; I<2; I++) //test if the Hit info is the same with what the member function StartOfAthenaEvent() sets
    {
      for (int J=0; J<4; J++)
        {
          ASSERT_TRUE( sd3.m_Edep_Cherenkov_Strip   [I][J] == 0 );
          ASSERT_TRUE( sd3.m_NPhoton_Cherenkov_Strip[I][J] == 0 );
        }
    }
}

TEST_F( ZDC_StripSDtest, EndOfAthenaEvent )
{
  G4HCofThisEvent hce;
  ZDC_StripSD sd4( "name4", "name4" );
  sd4.Initialize(&hce);
  sd4.StartOfAthenaEvent();
  sd4.EndOfAthenaEvent();
  
  ZDC_SimStripHit_Collection * a = sd4.m_HitColl.ptr();
  ASSERT_TRUE(a->begin()->GetSide()==0); //test if the Side value is the same with what the member function EndOfAthenaEvent() sets, the same below
  ASSERT_TRUE((a->end()-1)->GetSide()==1);
  ASSERT_TRUE(a->begin()->GetMod()==0);
  ASSERT_TRUE((a->end()-1)->GetMod()==3);
  ASSERT_FLOAT_EQ(a->begin()->GetEdep(),0);
  ASSERT_FLOAT_EQ((a->end()-1)->GetEdep(),0);
  ASSERT_EQ(a->begin()->GetNPhotons(),0);
  ASSERT_EQ((a->end()-1)->GetNPhotons(),0);

}

TEST_F( ZDC_StripSDtest, AddHit )
{
  G4HCofThisEvent hce;
  ZDC_StripSD sd5( "name5", "name5" );
  sd5.Initialize(&hce);

  int i = 1;
  int j = 2;
  int NPhoton = 4;
  double energy = 5.0;
  sd5.AddHit( i, j, NPhoton, energy );

  ZDC_SimStripHit_Collection * a = sd5.m_HitColl.ptr();
  ASSERT_TRUE(a->begin()->GetSide()==1); //test if the Side value is same with my input value 1, the same below
  ASSERT_TRUE(a->begin()->GetMod()==2);
  ASSERT_TRUE(a->begin()->GetEdep()==5.0);
  ASSERT_TRUE(a->begin()->GetNPhotons()==4);

}

int main( int argc, char** argv ) {

  auto g=new GaudiEnvironment;
  ::testing::AddGlobalTestEnvironment(g);
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();

}

