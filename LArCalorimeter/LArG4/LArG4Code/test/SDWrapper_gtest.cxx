/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#define SDWRAPPER_TEST//define the macro name to use it in the tested class SDWrapper. Using the macro name to do conditional compiling on the "private:" statement in the file SDWrapper.h

#include "LArG4Code/SDWrapper.h"

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

#include "LArSimEvent/LArHitContainer.h"
#include "LArG4Code/LArG4SimpleSD.h"
#include "LArG4Code/DerivedILArCalculatorSvcForTest.h"
#include <memory>
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArMiniFCAL_ID.h"
#include "CaloIdentifier/CaloDM_ID.h"

//set environment
class GaudiEnvironment : public ::testing::Environment {
  protected:
  virtual void SetUp() override {
    Athena_test::initGaudi("LArG4Code/optionForTest.txt", m_svcLoc);
  }
  ISvcLocator* m_svcLoc = nullptr;
};
class SDWrappertest : public ::testing::Test {	
  protected:
    virtual void SetUp() override {
    }

    virtual void TearDown() override {
    }
  LArEM_ID* m_EM = new LArEM_ID();
  LArFCAL_ID* m_FCAL = new LArFCAL_ID();
  LArHEC_ID* m_HEC = new LArHEC_ID();
  LArMiniFCAL_ID* m_mini = new LArMiniFCAL_ID();
};
//end

TEST_F(SDWrappertest, Initialize)
{
  std::string name = "name1";//define the actual parameter for instantiating class, the same below
  std::string hitCollectionName = "name11";
  std::string deadHitCollectionName = "name111";
  LArG4::detail::SDWrapper<LArG4SimpleSD, LArHitContainer> sd1(name, hitCollectionName, deadHitCollectionName);//initialize the class SDWrapper as an object sd1, it will deal with hit using LArG4SimpleSD and LArHitContainer

  G4HCofThisEvent hce;
  sd1.Initialize(&hce);//invoke the member function that needs to be tested
  
  ASSERT_TRUE(sd1.m_hitColl.isValid());//test if the template class WriteHandle object m_hitColl is initialized well
  ASSERT_TRUE(sd1.m_deadHitColl.isValid());//test if the template class WriteHandle object m_deadHitColl is initialized well
}


TEST_F(SDWrappertest, EndOfAthenaEvent)
{
  std::string name = "name5";//define the actual parameter for instantiating class, the same below
  std::string hitCollectionName = "name55";
  std::string deadHitCollectionName = "name555";
  LArG4::detail::SDWrapper<LArG4SimpleSD, LArHitContainer> sd5(name, hitCollectionName, deadHitCollectionName);//initialize the class SDWrapper as an object sd5, it will deal with hit using LArG4SimpleSD and LArHitContainer

  G4HCofThisEvent hce;
  sd5.Initialize(&hce);//here invoking the member function aims to initialize the class, after this, the initialized class can invoke other member functions  

  Identifier id;
  double l_energy = 1.0;
  double l_time = 1.0;//define actual parameter for the LArHit object named hit
  LArHit hit(id, l_energy, l_time);//declare a LArHit object, which is essentially a hit
  LArG4SimpleSD::hits_t* hits = new LArG4SimpleSD::hits_t();//define a hit container hits
  hits->insert(&hit);//insert a hit into the hit container hits, which will be used by a std::unique_ptr<LArG4SimpleSD> object p

  DerivedILArCalculatorSvcForTest* calc = new DerivedILArCalculatorSvcForTest();//use the derived ILArCalculatorSvc class since ILArCalculatorSvc is abstact and can not be instantiated
  auto p = std::make_unique<LArG4SimpleSD>("name", calc);//instantiate a LArG4SimpleSD object and wrap it in std::unique_ptr<LArG4SimpleSD> object p
  p.get()->setupHelpers(m_EM, m_FCAL, m_HEC, m_mini);//add helpers(m_EM, m_FCAL, m_HEC, m_mini), which can convert a set of numbers stored in LArG4Identifier object into a compact number stored in a Identifier object 
  p.get()->m_timeBins.insert( std::pair<G4int, LArG4SimpleSD::hits_t*>(0,hits) );//insert the hit container hits into the container of hit container

  sd5.m_sdList.push_back( std::move(p) );//insert the std::unique_ptr<LArG4SimpleSD> object p into the container m_sdList
  sd5.EndOfAthenaEvent();//invoke the tested member function, which will put the hit in another container m_hitColl. Next few lines will test if the hit was placed well

  LArHit* a = *((sd5.m_hitColl)->begin());//access the hit defined before
  ASSERT_EQ(1.0, a->energy());//test if the energy is 1.0 as the value we just set
  ASSERT_EQ(1.0, a->time());//test if the time is 1.0 as the value we just set
}

TEST_F(SDWrappertest, addSD)
{
  std::string name = "name4";//define the actual parameter for instantiating class, the same below
  std::string hitCollectionName = "name44";
  std::string deadHitCollectionName = "name444";
  LArG4::detail::SDWrapper<LArG4SimpleSD, LArHitContainer> sd4(name, hitCollectionName, deadHitCollectionName);//initialize the class SDWrapper as an object sd4, it will deal with hit using LArG4SimpleSD and LArHitContainer

  DerivedILArCalculatorSvcForTest* calc = new DerivedILArCalculatorSvcForTest();//use the derived ILArCalculatorSvc class since ILArCalculatorSvc is abstact and can not be instantiated
  auto p = std::make_unique<LArG4SimpleSD>("name", calc);
//  sd4.m_sdList.push_back( std::move(p) );
//  sd4.addSD(p);//this member function can not be invoked here correctly(i.e. error arises when compiling), I think there is something wrong with the definition of the member function
}

TEST_F(SDWrappertest, addFastSimSD)
{
  std::string name = "name3";//define the actual parameter for instantiating class, the same below
  std::string hitCollectionName = "name33";
  std::string deadHitCollectionName = "name333";
  LArG4::detail::SDWrapper<LArG4SimpleSD, LArHitContainer> sd3(name, hitCollectionName, deadHitCollectionName);//initialize the class SDWrapper as an object sd3, it will deal with hit using LArG4SimpleSD and LArHitContainer

  std::string sdName = "sdName";
  sd3.addFastSimSD(sdName);//invoke the tested member function addFastSimSD, and it will assign the variable sdName to m_fastSimSDName.

  ASSERT_TRUE("sdName" == sd3.m_fastSimSDName);//test if the variable sdName was correctly assigned to m_fastSimSDName
}

TEST_F(SDWrappertest, ProcessHits)
{ 
  std::string name = "name2";//define the actual parameter for instantiating class, the same below
  std::string hitCollectionName = "name22";
  std::string deadHitCollectionName = "name222";
  LArG4::detail::SDWrapper<LArG4SimpleSD, LArHitContainer> sd2(name, hitCollectionName, deadHitCollectionName);//initialize the class SDWrapper as an object sd2, it will deal with hit using LArG4SimpleSD and LArHitContainer
  
  G4HCofThisEvent hce;
  sd2.Initialize(&hce);//here invoking the member function aims to initialize the class, after this, the initialized class can invoke other member functions
  
  G4Step aStep;
  G4TouchableHistory th;
//  sd2.ProcessHits(&aStep, &th);//please at the time when you want to invoke the member function ProcessHits, uncomment this line. According to the comments in the tested class SDWrapper, the member function should not be invoked. Once it is invoked, it will return "ProcessHits: this SD shouldn't be assigned to volumes!" and its test will fail
}

int main( int argc, char** argv ) {

  auto g=new GaudiEnvironment;
  ::testing::AddGlobalTestEnvironment(g);
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();

}

