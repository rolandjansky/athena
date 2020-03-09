/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4Code/LArG4CalibSD.h"

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
#include "G4RunManager.hh"

#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArMiniFCAL_ID.h"
#include "CaloIdentifier/CaloDM_ID.h"

#include "G4AtlasTools/DerivedG4PhysicalVolume.h"
#include "LArG4Code/DerivedILArCalibCalculatorSvcForTest.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"

//set environment
class GaudiEnvironment : public ::testing::Environment {
  protected:
  virtual void SetUp() override {
    Athena_test::initGaudi("LArG4Code/optionForTest.txt", m_svcLoc);
  }
  ISvcLocator* m_svcLoc = nullptr;
};
class LArG4CalibSDtest : public ::testing::Test {	
  protected:
    virtual void SetUp() override {
    }

    virtual void TearDown() override {
    }

// Here I initialize 6 Identifier helper class objects, and they are used to convert a set of numbers stored in a LArG4Identifier object into a Identifier object.
// they can be used by all the TEST_F
  LArEM_ID* m_EM = new LArEM_ID(); 
  LArFCAL_ID* m_FCAL = new LArFCAL_ID();
  LArHEC_ID* m_HEC = new LArHEC_ID();
  LArMiniFCAL_ID* m_mini = new LArMiniFCAL_ID();
  CaloDM_ID* m_caloDm = new CaloDM_ID();
  AtlasDetectorID* m_Id_helper = new AtlasDetectorID();
};
//end

TEST_F( LArG4CalibSDtest, ProcessHits )
{
  G4Step* aStep = new G4Step();
  G4TouchableHistory* th = new G4TouchableHistory();

  DerivedILArCalibCalculatorSvcForTest* calc = new DerivedILArCalibCalculatorSvcForTest();//use the derived ILArCalibCalculatorSvc class since ILArCalibCalculatorSvc is abstact and can not be instantiated
  LArG4CalibSD sd1("name1", calc, false);
  sd1.setupHelpers(m_EM, m_FCAL, m_HEC, m_mini, m_caloDm);//add helpers(m_EM, m_FCAL, m_HEC, m_mini, m_caloDm), which can convert a set of numbers stored in LArG4Identifier object into a compact number stored in a Identifier object
  sd1.addDetectorHelper(m_Id_helper);//same with setupHelpers 
  sd1.ProcessHits(aStep, th);

//now that the newly-generated hit has already been stored in the hit collection calibrationHits, here I will test that. According to previous setting, the compact id stored in the hit should be 7
  unsigned long long compact_num = (*(sd1.m_calibrationHits.begin()))->cellID().get_compact();//get the compact id 
  ASSERT_EQ(compact_num , 7u);//test if the compact id is as we expected
}

TEST_F( LArG4CalibSDtest, EndOfAthenaEvent )
{
// define two actual parameters for the tested member function EndOfAthenaEvent
  CaloCalibrationHitContainer* hitContainer = new CaloCalibrationHitContainer();//this kind of instantiation will make the expression "(hitContainer)" be 1
  CaloCalibrationHitContainer* deadHitContainer = new CaloCalibrationHitContainer();

// define aStep and energies as the actual parameters of the member function SpecialHit 
  G4Step* aStep = new G4Step();
  std::vector<G4double> energies = {1., 2., 3., 4.};

  DerivedILArCalibCalculatorSvcForTest* calc = new DerivedILArCalibCalculatorSvcForTest();//use the derived ILArCalibCalculatorSvc class since ILArCalibCalculatorSvc is abstact and can not be instantiated
  LArG4CalibSD sd2("name2", calc, false);
  sd2.setupHelpers(m_EM, m_FCAL, m_HEC, m_mini, m_caloDm);//add helpers(m_EM, m_FCAL, m_HEC, m_mini, m_caloDm), which can convert a set of numbers stored in LArG4Identifier object into a compact number stored in a Identifier object
  sd2.addDetectorHelper(m_Id_helper);//same with setupHelpers
  sd2.SpecialHit(aStep, energies);//this member function is intended to store a newly-generated hit in to the hit collection calibrationHits with a kind of specific order

//so far the newly-generated hit has already been stored in the object m_calibrationHits, so I can invoke the member function EndOfAthenaEvent to move the hits into the object hitContainer that was just defined
  sd2.EndOfAthenaEvent(hitContainer, deadHitContainer);

  unsigned long long compact_num = (*(hitContainer->begin()))->cellID().get_compact();//get the compact id
  ASSERT_EQ(compact_num, 7u);//test if it is 7, which is the expected value according to previous setting
}

TEST_F( LArG4CalibSDtest, SpecialHit )
{
// the member function SpecialHit aims to generate a hit and store it in a hits container
// define aStep and energies as the actual parameters of the member function SpecialHit
  G4Step* aStep = new G4Step(); 
  std::vector<G4double> energies = {1., 2., 3., 4.}; 

  DerivedILArCalibCalculatorSvcForTest* calc = new DerivedILArCalibCalculatorSvcForTest();//use the derived ILArCalibCalculatorSvc class since ILArCalibCalculatorSvc is abstact and can not be instantiated
  LArG4CalibSD sd5("name5", calc, false);
  sd5.setupHelpers(m_EM, m_FCAL, m_HEC, m_mini, m_caloDm);//add helpers(m_EM, m_FCAL, m_HEC, m_mini, m_caloDm), which can convert a set of numbers stored in LArG4Identifier object into a compact number stored in a Identifier object
  sd5.addDetectorHelper(m_Id_helper);//same with setupHelpers
  sd5.SpecialHit(aStep, energies);//this member function is intended to store a newly-generated hit in to the hit collection calibrationHits with a kind of specific order
  
//now that the newly-generated hit has already been stored in the hit collection calibrationHits, here I will test that. According to previous setting, the compact id stored in the hit should be 7
  unsigned long long compact_num = (*(sd5.m_calibrationHits.begin()))->cellID().get_compact();//get the compact id 
  ASSERT_EQ(compact_num , 7u);//test if the compact id is as we expected
}

TEST_F( LArG4CalibSDtest, SimpleHit )
{
// the member function SimpleHit aims to generate a hit and store it in a hit container
// Add a bunch of numbers into the object a_ident and this kind of number setting is intented to run the "if(a_ident[0]==4) {if(a_ident[1]==1) ...}" block
  LArG4Identifier a_ident;
  a_ident.add(4);
  a_ident.add(1);
  a_ident.add(2);
  a_ident.add(3);
  a_ident.add(4);
  a_ident.add(5);
  a_ident.add(6);

// Set energies
  std::vector<double> energies = {1., 2., 3., 4.};

  DerivedILArCalibCalculatorSvcForTest* calc = new DerivedILArCalibCalculatorSvcForTest();
  LArG4CalibSD sd6("name6", calc, false);
  sd6.setupHelpers(m_EM, m_FCAL, m_HEC, m_mini, m_caloDm); 
  sd6.addDetectorHelper(m_Id_helper);
  LArG4CalibSD::m_calibrationHits_t calibrationHits; //it is actually a hit collection, since there is no previous hit in it, it will execute the "if (bookmark == calibrationHits.end() || !(*bookmark)->Equals(hit)) {if (calibrationHits.empty() || bookmark == calibrationHits.begin()) ...}" block of the member function SimpleHit
  sd6.SimpleHit(a_ident, energies, calibrationHits); //this member function is intended to store a newly-generated hit in to the hit collection calibrationHits with a kind of specific order

// Since there is a new hit added in the hit collection, I will test if the hit is stored correctly in the hit collection calibrationHits
  unsigned long long compact_num = (*(calibrationHits.begin()))->cellID().get_compact(); //get the compacted id of the hit, according to the previous setting it should be 7
  ASSERT_TRUE(compact_num == 7u); 

}

TEST_F( LArG4CalibSDtest, ConvertID )
{
//the member function ConvertID aims to convert a set of numbers stored in a LArG4Identifier object into a Identifier object with the identifier helper class objects, such as:
//const LArEM_ID*       m_larEmID;
//const LArFCAL_ID*     m_larFcalID;
//const LArHEC_ID*      m_larHecID;
//const LArMiniFCAL_ID* m_larMiniFcalID;
//const CaloDM_ID*      m_caloDmID;
//const AtlasDetectorID* m_id_helper;

  LArG4Identifier a_ident;
  a_ident.add(10);
  a_ident.add(1);
  a_ident.add(2);
  a_ident.add(3);
  a_ident.add(4);
  a_ident.add(5);
  a_ident.add(6);
//here I decorate the LArG4Identifier object a_ident with a set of numbers to make it complete.//This kind of setting for a_ident will make the "else if(a_ident[0]==10)" block of the member function ConvertID be tested. you can change the setting to test other block

  DerivedILArCalibCalculatorSvcForTest* calc = new DerivedILArCalibCalculatorSvcForTest();
  LArG4CalibSD sd7("name7", calc, false);
  sd7.setupHelpers(m_EM, m_FCAL, m_HEC, m_mini, m_caloDm); //To set the identifier helper class objects m_larEmID using these objects: EM, FCAL, HEC, mini, caloDm
  sd7.addDetectorHelper(m_Id_helper); //To set the identifier helper class object m_id_helper using the object: id_helper
  Identifier id = sd7.ConvertID(a_ident); //generally speaking, a set of number was compact into a single number stored in id

  unsigned long long compact_num = id.get_compact();
  ASSERT_TRUE(id.is_valid()); //test if the Identifier object id is valid
  ASSERT_TRUE(compact_num == 15u); //Based on previous number setting, the compact number should equal to 15. Test it here
}

int main( int argc, char** argv ) {

  auto g=new GaudiEnvironment;
  ::testing::AddGlobalTestEnvironment(g);
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();

}

