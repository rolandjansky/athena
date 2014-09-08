/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorModules/GeneratorName.h" 
#include <cppunit/extensions/HelperMacros.h>

/**
@class GeneratorModules/GeneratorModulesTest_CppUnit.cxx

@brief Class to test the "GeneratorName.h" global functions and enums.
@author Jan Schumacher August 2009
*/
class GeneratorNameTest : public CppUnit::TestFixture
{
  // Definition of the unit test suite "ComplexNumberTest"
  CPPUNIT_TEST_SUITE( GeneratorNameTest );
  CPPUNIT_TEST( testTrivial );
  CPPUNIT_TEST( testIsNewGenCoding );
  CPPUNIT_TEST( testFirstGenerator );
  CPPUNIT_TEST_SUITE_END();

  private:
    static const unsigned int old_PYTHIA = 1000000;
    static const unsigned int old_HERWIG = 2000000;
    static const unsigned int old_HYDJET = 10000000;
    static const unsigned int old_TAUOLA_PHOTOS = 30000;
    static const unsigned int old_ACERMC = 300000;
  public:
    void setUp()
    {
    }
    void tearDown()
    {
    }
    // Trivial test to see if the values of the enums change -- just a reminder
    // to look at the tests.
    void testTrivial()
    {
      CPPUNIT_ASSERT( 110000000 == PYTHIA );
    }
    // Test that the differentiation between old and new coding works. This
    // should be relatively trivial, since they are separated by a single cut.
    // Note, that all of this depends on there being a first generator.
    // I am using the not very elegant method of adding the enums -- for the
    // test ok, but it assumes knowledge of the coding.
    void testIsNewGenCoding()
    {
      // New coding
      CPPUNIT_ASSERT( isNewGenCoding( PYTHIA ) );
      CPPUNIT_ASSERT( isNewGenCoding( HERWIG ) );
      CPPUNIT_ASSERT( isNewGenCoding( HERWIG+TAUOLA_PHOTOS ) );
      CPPUNIT_ASSERT( isNewGenCoding( PYTHIA+LHEF+TAUOLA_PHOTOS ) );

      // Old coding
      CPPUNIT_ASSERT( ! isNewGenCoding( old_PYTHIA ) );
      CPPUNIT_ASSERT( ! isNewGenCoding( old_HERWIG ) );
      CPPUNIT_ASSERT( ! isNewGenCoding( old_HYDJET ) );
      CPPUNIT_ASSERT( ! isNewGenCoding( old_HERWIG+old_ACERMC+old_TAUOLA_PHOTOS ) );
    }
    // Test decoding of first generator
    void testFirstGenerator()
    {
      // New coding
      CPPUNIT_ASSERT_EQUAL( PYTHIA, first_generator( PYTHIA ) );
      CPPUNIT_ASSERT_EQUAL( HERWIG, first_generator( HERWIG ) );
      CPPUNIT_ASSERT_EQUAL( PYTHIA, first_generator( PYTHIA+ACERMC ) );
      CPPUNIT_ASSERT_EQUAL( PYTHIA, first_generator( PYTHIA+ACERMC+TAUOLA_PHOTOS ) );
      CPPUNIT_ASSERT_EQUAL( PYTHIA, first_generator( PYTHIA+CHARYBDIS+TAUOLA_PHOTOS ) );
      CPPUNIT_ASSERT_EQUAL( PYTHIA, first_generator( PYTHIA+LHEF+TAUOLA_PHOTOS ) );

      // Old coding
      CPPUNIT_ASSERT_EQUAL( PYTHIA, first_generator( old_PYTHIA ) );
      CPPUNIT_ASSERT_EQUAL( PYTHIA, first_generator( old_PYTHIA+old_ACERMC+old_TAUOLA_PHOTOS ) );
      CPPUNIT_ASSERT_EQUAL( HERWIG, first_generator( old_HERWIG+old_ACERMC+old_TAUOLA_PHOTOS ) );
      CPPUNIT_ASSERT_EQUAL( HYDJET, first_generator( old_HYDJET+old_ACERMC+old_TAUOLA_PHOTOS ) );
    }

};

/// Registration of the test suite(s)
CPPUNIT_TEST_SUITE_REGISTRATION(GeneratorNameTest);

/// CppUnit test-driver common for all the cppunit test classes.
/// In ATLAS sw release it is located in TestPolicy package.
#include <TestPolicy/CppUnit_testdriver.cxx>

