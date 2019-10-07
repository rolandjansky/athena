#include "src/ZDC_PixelSD.h"

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

#include "G4AtlasTools/DerivedG4PhysicalVolume.h"
#include "G4AtlasTools/DerivedG4SensitiveDetectorTestSetting.h"

//set environment
ISvcLocator* g_svcLoc = nullptr;
class GaudiEnvironment : public ::testing::Environment {
  protected:
  virtual void SetUp() override {
    Athena_test::initGaudi("ZDC_SD/optionForTest.txt", g_svcLoc);
  }
};
class ZDC_PixelSDtest : public ::testing::Test {	
  protected:
    virtual void SetUp() override {
//...
    }

    virtual void TearDown() override {
//...
    }
//...
};
//end of environment setting

TEST_F( ZDC_PixelSDtest, Initialize )
{
  G4HCofThisEvent hce;
  ZDC_PixelSD sd1( "name1", "name1" );
  sd1.Initialize(&hce);
  ASSERT_TRUE(sd1.m_HitColl.isValid());
}

TEST_F( ZDC_PixelSDtest, ProcessHits )
{
  G4HCofThisEvent hce;
  G4Step sp;
  G4TouchableHistory th;

  G4double totalenergydeposit = 0.8;
  G4String physicalname = "physicsTDQuarticBar[9]";
  G4String logicalname = "BBBBBBBBBTubeGas";
  G4int copyno = 21978;
  G4ThreeVector preStepPos = G4ThreeVector(0,0,1);
  G4ThreeVector postStepPos = G4ThreeVector(0,0,2);
  G4double globaltime0 = 0.5;
  G4double kineticenergy0 = 1.5;
  G4double velocity0 = 250;
  G4double globaltime = 5.0;
  G4double kineticenergy = 0.5;
  G4double globaltime1 = 0.5;
  G4double kineticenergy1 = 0.5;
  G4double velocity1 = 250;
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

/*
//decorate sp with the variable called TotalEnergyDeposit
  sp.SetTotalEnergyDeposit( 0.8 );
//end

//decorate sp with a G4StepPoint object
  G4StepPoint* stepPoint = new G4StepPoint();
  G4ThreeVector preStepPos(0, 0, 1);
  stepPoint->SetPosition(preStepPos);
  G4NavigationHistory* navigationHistory = new G4NavigationHistory();
  G4String boxName = "name";
  G4Box* box = new G4Box(boxName, 1.0, 1.0, 1.0);
  G4NistManager* man = G4NistManager::Instance();
  G4Material* material = man->FindOrBuildMaterial("G4_AIR");
  G4String name = "logicalName";
  G4LogicalVolume fLogical(box, material, name);
  G4String name1 = "physicsTDQuarticBar[9]";
  G4VPhysicalVolume* pPhysical = NULL;
  G4MyPhysicalVolume physicalVolume(0, G4ThreeVector(0,0,0), name1, &fLogical, pPhysical);
  physicalVolume.SetCopyNo(21978);
  G4int nReplica = 2;
  navigationHistory->SetFirstEntry(&physicalVolume);
  navigationHistory->NewLevel(&physicalVolume, kNormal, nReplica);
  navigationHistory->NewLevel(&physicalVolume, kNormal, nReplica);
  navigationHistory->NewLevel(&physicalVolume, kNormal, nReplica);
  navigationHistory->NewLevel(&physicalVolume, kNormal, nReplica);
  navigationHistory->NewLevel(&physicalVolume, kNormal, nReplica);
  G4TouchableHistory* touchableHistory = new G4TouchableHistory(*navigationHistory);
  G4TouchableHandle touchableHandle(touchableHistory);
  stepPoint->SetTouchableHandle(touchableHandle);
  stepPoint->SetGlobalTime(0.5);
  stepPoint->SetKineticEnergy(1.5);
  stepPoint->SetVelocity(250);

  sp.SetPreStepPoint(stepPoint);
//end

//decorate sp with another G4StepPoint object
  G4StepPoint* stepPoint1 = new G4StepPoint();
  G4ThreeVector postStepPos(0, 0, 2);
  stepPoint1->SetPosition(postStepPos);
  stepPoint1->SetVelocity(250);

  sp.SetPostStepPoint(stepPoint1);
//end

//set step length for the step
  sp.SetStepLength(1.0);
//end

//decorate sp with a G4Track object
 G4ParticleDefinition particle("anyon",         0.0*MeV,       0.0*MeV,         1.0, 
		    2,              -1,            -1,          
		    0,               0,             0,             
	      "anyon",               0,             0,          22,
	      true,               -1.0,          NULL,
             false,           "anyon",          22
	      );
  G4ThreeVector aMomentumDirection(0,0,0);
  G4double aKineticEnergy = 0.5;
  G4DynamicParticle* dynamicPar = new G4DynamicParticle(&particle, aMomentumDirection, aKineticEnergy);
  G4double aValueTime = 1;
  G4ThreeVector ValuePosition(1.0, 1.0, 1.0);
  G4Track* track = new G4Track(dynamicPar, aValueTime, ValuePosition);
  G4double globalTime = 5.0;
  track->SetGlobalTime(globalTime);
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

  sp.SetTrack(track);
//end
*/

  ZDC_PixelSD sd2("name2", "name2");
  sd2.Initialize(&hce);
  sd2.StartOfAthenaEvent();
  sd2.ProcessHits(&sp, &th);

  ASSERT_FLOAT_EQ(sd2.m_Edep_Cherenkov_Pixel[1][1][78], 2.2452348e-05);
  ASSERT_EQ(sd2.m_NPhoton_Cherenkov_Pixel[1][1][78], 7);
}

TEST_F( ZDC_PixelSDtest, StartOfAthenaEvent )
{
  G4HCofThisEvent hce;
  ZDC_PixelSD sd3( "name3", "name3" );
  sd3.Initialize(&hce);
  sd3.StartOfAthenaEvent();

  for (int I=0; I<=1; I++)
    {
      for (int J=0; J<=1; J++)
        {
          for (int K=0; K<80; K++)
            {
              ASSERT_TRUE(sd3.m_Edep_Cherenkov_Pixel   [I][J][K] == 0);
              ASSERT_TRUE(sd3.m_NPhoton_Cherenkov_Pixel[I][J][K] == 0);
            }
        }
    }
}

TEST_F( ZDC_PixelSDtest, EndOfAthenaEvent )
{
  G4HCofThisEvent hce;
  ZDC_PixelSD sd4( "name4", "name4" );
  sd4.Initialize(&hce);
  sd4.StartOfAthenaEvent();
  sd4.EndOfAthenaEvent();

  ZDC_SimPixelHit_Collection * a = sd4.m_HitColl.ptr();
  ASSERT_TRUE(a->begin()->GetSide()==0);
  ASSERT_TRUE((a->end()-1)->GetSide()==1);
  ASSERT_TRUE(a->begin()->GetMod()==0);
  ASSERT_TRUE((a->end()-1)->GetMod()==1);
  ASSERT_EQ(a->begin()->GetPix(),0);
  ASSERT_EQ((a->end()-1)->GetPix(),23);
  ASSERT_FLOAT_EQ(a->begin()->GetEdep(),0);
  ASSERT_FLOAT_EQ((a->end()-1)->GetEdep(),0);
  ASSERT_EQ(a->begin()->GetNPhotons(),0);
  ASSERT_EQ((a->end()-1)->GetNPhotons(),0);
}

TEST_F( ZDC_PixelSDtest, AddHit )
{
  G4HCofThisEvent hce;
  ZDC_PixelSD sd5( "name5", "name5" );
  sd5.Initialize(&hce);

  int i = 1;
  int j = 2;
  int k = 3;
  int NPhoton = 4;
  double energy = 5.0;
  sd5.AddHit( i, j, k, NPhoton, energy );

  ZDC_SimPixelHit_Collection * a = sd5.m_HitColl.ptr();
  ASSERT_TRUE(a->begin()->GetSide()==1);
  ASSERT_TRUE(a->begin()->GetMod()==2);
  ASSERT_TRUE(a->begin()->GetPix()==3);
  ASSERT_TRUE(a->begin()->GetEdep()==5.0);
  ASSERT_TRUE(a->begin()->GetNPhotons()==4);
}

int main( int argc, char** argv ) {

  auto g=new GaudiEnvironment;
  ::testing::AddGlobalTestEnvironment(g);
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();

}


