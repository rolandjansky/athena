/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// Class to test the ChargeFilter class
/// Author : S.Binet<binet@cern.ch>

// CppUnit includes
#include<cppunit/extensions/HelperMacros.h>
#include<cppunit/Exception.h>

// STL includes
#include <iostream>
#include <stdexcept>
#include <cmath>

// CLHEP includes
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Units/SystemOfUnits.h"

// ElectronPhotonIDEvent includes
#include "egammaEvent/Electron.h"

// JetEvent includes
#include "JetEvent/Jet.h"

// AnalysisUtils includes
#include "AnalysisUtils/ChargeFilter.h"

class ChargeFilterTest : public CppUnit::TestFixture
{
  /// Definition of the unit test suite "ChargeFilterTest"
  CPPUNIT_TEST_SUITE( ChargeFilterTest );

  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testCopyChargeFilter );
  CPPUNIT_TEST( testSettersAndGetters );

  /// end the definition test suite
  CPPUNIT_TEST_SUITE_END();

private:
    Jet        m_b;
    Jet        m_bbar;
    Analysis::Electron m_ele;
  
public:

  /// Set up the data members
  void setUp()
  {
    m_b.set4Mom( P4PxPyPzE( 100*GeV, 100*GeV, 100*GeV, 200*GeV ) );
    m_b.set_pdgId( PDG::b );
    m_b.set_charge( -1./3. );

    m_bbar.set4Mom( P4PxPyPzE( 100*GeV, 100*GeV, 100*GeV, 200*GeV ) );
    m_bbar.set_pdgId( PDG::anti_b );
    m_bbar.set_charge( 1./3. );

    m_ele.set4Mom( P4PxPyPzE( 100*GeV, 100*GeV, 100*GeV, 200*GeV ) );
    m_ele.set_pdgId( PDG::e_minus );
    m_ele.set_charge( -1. );
  }

  /// destroy any on-the-heap-created data member
  void tearDown()
  {
  }

  /// Test the ChargeFilter constructors
  void testConstructor()
  {
    ChargeFilter<IParticle> filter;
    CPPUNIT_ASSERT( filter.isAccepted(&m_b) );
    CPPUNIT_ASSERT( filter.isAccepted(&m_bbar) );
    CPPUNIT_ASSERT( filter.isAccepted(&m_ele) );

    try {
      CPPUNIT_ASSERT( filter.charge() );
      CPPUNIT_ASSERT( false );
    } catch( std::exception& e ) {
      const std::string error = "No charge selection defined";
      CPPUNIT_ASSERT( error == e.what() );
    }
  }

  /// Test that ChargeFilter cuts are well copied
  void testCopyChargeFilter()
  {
    ChargeFilter<IParticle> orig;
    orig.setCharge( -1./3. );
    CPPUNIT_ASSERT(  orig.isAccepted(&m_b) );
    CPPUNIT_ASSERT( !orig.isAccepted(&m_bbar) );

    {
      ChargeFilter<IParticle> filter(orig);
      CPPUNIT_ASSERT(  filter.isAccepted(&m_b) );
      CPPUNIT_ASSERT( !filter.isAccepted(&m_bbar) );
    }
    {
      ChargeFilter<IParticle> filter = orig;
      CPPUNIT_ASSERT(  filter.isAccepted(&m_b) );
      CPPUNIT_ASSERT( !filter.isAccepted(&m_bbar) );
    }
    {
      ChargeFilter<IParticle> filter;
      CPPUNIT_ASSERT( filter.isAccepted(&m_b) );
      CPPUNIT_ASSERT( filter.isAccepted(&m_bbar) );
      CPPUNIT_ASSERT( filter.isAccepted(&m_ele) );

      filter.setFilter( &orig );
      CPPUNIT_ASSERT(  filter.isAccepted(&m_b) );
      CPPUNIT_ASSERT( !filter.isAccepted(&m_bbar) );
    }
  }

  /// Test setters and getters
  void testSettersAndGetters()
  {
    ChargeFilter<IParticle> filter;
    CPPUNIT_ASSERT( filter.isAccepted(&m_b) );
    CPPUNIT_ASSERT( filter.isAccepted(&m_bbar) );
    CPPUNIT_ASSERT( filter.isAccepted(&m_ele) );
    
    try {
      CPPUNIT_ASSERT( filter.charge() );
      CPPUNIT_ASSERT( false );
    } catch( std::exception& e ) {
      const std::string error = "No charge selection defined";
      CPPUNIT_ASSERT( error == e.what() );
    }

    filter.setCharge( -1./3. );
    CPPUNIT_ASSERT( -1./3. == filter.charge() );
    CPPUNIT_ASSERT(  filter.isAccepted(&m_b) );
    CPPUNIT_ASSERT( !filter.isAccepted(&m_bbar) );
    CPPUNIT_ASSERT( !filter.isAccepted(&m_ele) );

    filter.setCharge( 1./3. );
    CPPUNIT_ASSERT( !filter.isAccepted(&m_b) );
    CPPUNIT_ASSERT(  filter.isAccepted(&m_bbar) );
    CPPUNIT_ASSERT( !filter.isAccepted(&m_ele) );
    
    filter.setCharge( -1. );
    CPPUNIT_ASSERT( !filter.isAccepted(&m_b) );
    CPPUNIT_ASSERT( !filter.isAccepted(&m_bbar) );
    CPPUNIT_ASSERT(  filter.isAccepted(&m_ele) );
    
    filter.setCharge( 0. );
    CPPUNIT_ASSERT( !filter.isAccepted(&m_b) );
    CPPUNIT_ASSERT( !filter.isAccepted(&m_bbar) );
    CPPUNIT_ASSERT( !filter.isAccepted(&m_ele) );
    
  }

};

/// Registration of the test suite "ChargeFilterTest" so it will be 
/// recognized by the main test program which drives the different tests
CPPUNIT_TEST_SUITE_REGISTRATION( ChargeFilterTest );

/// CppUnit test-driver common for all the cppunit test classes.
/// In ATLAS sw release it is located in TestPolicy package
#include <TestPolicy/CppUnit_SGtestdriver.cxx>
