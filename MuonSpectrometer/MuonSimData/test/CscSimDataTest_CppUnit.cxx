/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CscSimDataTest_CppUnit.cxx

#include "MuonSimData/CscSimData.h"

#include<cppunit/extensions/HelperMacros.h>
#include<cppunit/Exception.h>

// Class to test CscSimData.
//
// David Adams
// March 2007

class CscSimDataTest : public CppUnit::TestFixture {
  // Definition of the unit test suite
  CPPUNIT_TEST_SUITE( CscSimDataTest );
  CPPUNIT_TEST( testCscMcData );
  CPPUNIT_TEST( testCscSimData );
  CPPUNIT_TEST_SUITE_END();
private:
  CscSimData* m_pdat;
public:
  void setUp() {
    m_pdat = 0;
  }
  void tearDown() {
    delete m_pdat;
  }
  void testCscMcData() {
    float energy = 0.00123;
    float ypos = -12.34;
    float zpos = 24.68;
    float charge = 123456.7;
    CscMcData dat1(energy, ypos, zpos);
    CPPUNIT_ASSERT( dat1.energy() == energy );
    CPPUNIT_ASSERT( dat1.ypos() == ypos );
    CPPUNIT_ASSERT( dat1.zpos() == zpos );
    CPPUNIT_ASSERT( dat1.charge() == 0.0 );
    dat1.setCharge(charge);
    CPPUNIT_ASSERT( dat1.charge() == charge );
  }
  void testCscSimData() {
    std::vector<CscSimData::Deposit> deps;
    // Create and store first deposit.
    float e1 = 0.00123;
    float y1 = -12.34;
    float z1 = 24.68;
    float c1 = 123456.7;
    CscMcData dat1(e1, y1, z1);
    HepMcParticleLink l1(11,1);
    CscSimData::Deposit dep1(l1, dat1);
    deps.push_back(dep1);
    // Create and store second deposit.
    dat1.setCharge(c1);
    float e2 = 0.00124;
    float y2 = -12.35;
    float z2 = 24.69;
    float c2 = 223456.7;
    CscMcData dat2(e2, y2, z2);
    dat2.setCharge(c2);
    HepMcParticleLink l2(12,1);
    CscSimData::Deposit dep2(l2, dat2);
    deps.push_back(dep2);
    // Create data object.
    CscSimData sdat(deps);
    CPPUNIT_ASSERT( sdat.getdeposits().size() == 2 );
  }
};

// Registration of the test suite "ComplexNumberTest", so it
// will be recognized by the main test program which drives the
// different tests.
CPPUNIT_TEST_SUITE_REGISTRATION(CscSimDataTest);

// CppUnit test-driver common for all the cppunit test classes.
// In ATLAS sw release it is located in TestPolicy package.
#include <TestPolicy/CppUnit_testdriver.cxx>
