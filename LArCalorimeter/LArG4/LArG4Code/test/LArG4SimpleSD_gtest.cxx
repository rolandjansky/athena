/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4Code/LArG4SimpleSD.h"

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
#include "LArSimEvent/LArHitContainer.h"

#include "G4AtlasTools/DerivedG4PhysicalVolume.h"
#include "LArG4Code/DerivedILArCalculatorSvcForTest.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"

//set environment
class GaudiEnvironment : public ::testing::Environment {
  protected:
  virtual void SetUp() override {
    Athena_test::initGaudi("LArG4Code/optionForTest.txt", m_svcLoc);
  }
  ISvcLocator* m_svcLoc = nullptr;
};
class LArG4SimpleSDtest : public ::testing::Test {	
  protected:
    virtual void SetUp() override {
    }

    virtual void TearDown() override {
    }

// Here I initialize 4 Identifier helper class objects, and they are used to convert a set of numbers stored in a LArG4Identifier object into a Identifier object.
// they can be used by all the TEST_F
LArEM_ID* m_EM = new LArEM_ID(); 
  LArFCAL_ID* m_FCAL = new LArFCAL_ID();
  LArHEC_ID* m_HEC = new LArHEC_ID();
  LArMiniFCAL_ID* m_mini = new LArMiniFCAL_ID();
};
//end

TEST_F( LArG4SimpleSDtest, ProcessHits )
{

}

TEST_F( LArG4SimpleSDtest, SimpleHit )
{
//this member function is used for fast simulations. it will decide if a new hitCollection element of m_timeBins need to be added according to the variable timeBin calculated by the member function getTimeBin. Based on current time value 1.0, there will be a new element inserted into m_timeBins, which can be tested
//in addition, a new hit can be generated in the tested member function, and it is stored in the m_timeBins. And as well it can be tested that if the hit was correctly generated.
// Add a bunch of numbers into the object a_ident and this kind of number setting is intented to run the "if(a_ident[0]==4) {if(a_ident[1]==1) ...}" block when the member function invokes the member function ConvertID.
  LArG4Identifier a_ident;
  a_ident.add(4);
  a_ident.add(2);
  a_ident.add(2);
  a_ident.add(3);
  a_ident.add(4);
  a_ident.add(5);
  a_ident.add(6);
  double time = 1.0;//define actual parameter for the tested function SimpleHit, the same below
  double energy = 1.0;

  DerivedILArCalculatorSvcForTest* calc = new DerivedILArCalculatorSvcForTest();//use the derived ILArCalculatorSvc class since ILArCalculatorSvc is abstact and can not be instantiated
  LArG4SimpleSD sd2("name2", calc);//instantiate the tested class LArG4SimpleSD
  sd2.setupHelpers(m_EM, m_FCAL, m_HEC, m_mini);//add helpers(m_EM, m_FCAL, m_HEC, m_mini), which can convert a set of numbers stored in LArG4Identifier object into a compact number stored in a Identifier object 
  sd2.SimpleHit(a_ident, time, energy);//invoke the tested member function SimpleHit, it will generated a new hit and stores it in the variable m_timeBins

  LArG4SimpleSD::hits_t* hitCollection = (*(sd2.m_timeBins.begin())).second;//firstly, get the hitCollection that was stored in the container m_timeBins
  unsigned long long compact_num = (*(hitCollection->begin()))->cellID().get_compact(); //get the compacted id of the hit, according to the previous setting it should be 7
  double theTime = (*(hitCollection->begin()))->time();//get the time info of the hit, it should be 1.0 as we set before
  double theEnergy = (*(hitCollection->begin()))->energy();//get the energy info of the hit, it should be as well 1.0 as we set before
  ASSERT_EQ(compact_num, 7);//test if the compact_num is 7
  ASSERT_EQ(theTime, 1.0);//test if theTime is 1.0
  ASSERT_EQ(theEnergy, 1.0);//test if theEnergy is 1.0
}

TEST_F( LArG4SimpleSDtest, EndOfAthenaEvent )
{
  Identifier id;
  double l_energy = 1.0;
  double l_time = 1.0;//define actual parameter for the LArHit object named hit
  LArHit hit(id, l_energy, l_time);//declare a LArHit object, which is essentially a hit
  LArG4SimpleSD::hits_t* hits = new LArG4SimpleSD::hits_t();//define a hit container hits
  hits->insert(&hit);//insert a hit into the hit container hits

  DerivedILArCalculatorSvcForTest* calc = new DerivedILArCalculatorSvcForTest();//use the derived ILArCalculatorSvc class since ILArCalculatorSvc is abstact and can not be instantiated
  LArG4SimpleSD sd3("name3", calc);//instantiate the tested class LArG4SimpleSD
  sd3.setupHelpers(m_EM, m_FCAL, m_HEC, m_mini);//add helpers(m_EM, m_FCAL, m_HEC, m_mini), which can convert a set of numbers stored in LArG4Identifier object into a compact number stored in a Identifier object 
  sd3.m_timeBins.insert( std::pair<G4int, LArG4SimpleSD::hits_t*>(0,hits) );//insert the hit container hits into the container of hit container
  LArHitContainer* hitContainer = new LArHitContainer("name");//define another hit container
  sd3.EndOfAthenaEvent(hitContainer);//invoke the tested member function EndOfAthenaEvent, which will put the hit stored in the container named m_timeBins into the hit container named hitContainer

  LArHit* a = *(hitContainer->begin());//access the hit defined before
  ASSERT_EQ(1.0, a->energy());//test if the energy is 1.0 as the value we just set
  ASSERT_EQ(1.0, a->time());//test if the time is 1.0 as the value we just set
}

TEST_F( LArG4SimpleSDtest, ConvertID )
{
  LArG4Identifier a_ident;
  a_ident.add(4);
  a_ident.add(2);
  a_ident.add(2);
  a_ident.add(3);
  a_ident.add(4);
  a_ident.add(5);
  a_ident.add(6);
//here I decorate the LArG4Identifier object a_ident with a set of numbers to make it complete.
//This kind of setting for a_ident will make the "else if(a_ident[1]==2)" block of the member function ConvertID be tested. you can change the setting to test other block

  DerivedILArCalculatorSvcForTest* calc = new DerivedILArCalculatorSvcForTest();//use the derived ILArCalculatorSvc class since ILArCalculatorSvc is abstact and can not be instantiated
  LArG4SimpleSD sd4("name4", calc);//instantiate the tested class LArG4SimpleSD
  sd4.setupHelpers(m_EM, m_FCAL, m_HEC, m_mini);//add helpers(m_EM, m_FCAL, m_HEC, m_mini), which can convert a set of numbers stored in LArG4Identifier object into a compact number stored in a Identifier object 
  Identifier id = sd4.ConvertID(a_ident); //generally speaking, a set of number was compact into a single number stored in id

  unsigned long long compact_num = id.get_compact();
  ASSERT_TRUE(id.is_valid()); //test if the Identifier object id is valid
  ASSERT_EQ(compact_num, 7); //Based on previous number setting, the compact number should equal to 15. Test it here
}

TEST_F( LArG4SimpleSDtest, getTimeBin )
{
  double aTime = 75.0;//set actual parameter for the tested member funtion getTimeBin (G4double time)
  
  DerivedILArCalculatorSvcForTest* calc = new DerivedILArCalculatorSvcForTest();//use the derived ILArCalculatorSvc class since ILArCalculatorSvc is abstact and can not be instantiated
  LArG4SimpleSD sd5("name5", calc);//instantiate the tested class LArG4SimpleSD
  sd5.setupHelpers(m_EM, m_FCAL, m_HEC, m_mini);//add helpers(m_EM, m_FCAL, m_HEC, m_mini), which can convert a set of numbers stored in LArG4Identifier object into a compact number stored in a Identifier object
  double output = sd5.getTimeBin(aTime);//invoke the tested member function, and its output should be 13 according to the member function definition

  ASSERT_EQ(output, 13);//test if the output is as we expected


}

int main( int argc, char** argv ) {

  auto g=new GaudiEnvironment;
  ::testing::AddGlobalTestEnvironment(g);
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();

}

