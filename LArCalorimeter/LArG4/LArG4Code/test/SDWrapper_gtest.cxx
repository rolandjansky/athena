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

TEST_F(SDWrappertest, ProcessHits)
{

}

TEST_F(SDWrappertest, EndOfAthenaEvent)
{

}

TEST_F(SDWrappertest, addSD)
{

}

TEST_F(SDWrappertest, addFastSimSD)
{

}

int main( int argc, char** argv ) {

  auto g=new GaudiEnvironment;
  ::testing::AddGlobalTestEnvironment(g);
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();

}

