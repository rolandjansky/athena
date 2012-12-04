/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// Class to test the DecayParser class
/// Author : S.Binet<binet@cern.ch>

// CppUnit includes
#include<cppunit/extensions/HelperMacros.h>
#include<cppunit/Exception.h>

// STL includes
#include <iostream>
#include <stdexcept>
#include <cmath>

// McParticleUtils includes
#include "McParticleUtils/DecayParser.h"

class DecayParserTest : public CppUnit::TestFixture
{
  /// Definition of the unit test suite "DecayParserTest"
  CPPUNIT_TEST_SUITE( DecayParserTest );

  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testSettersAndGetters );

  /// end the definition test suite
  CPPUNIT_TEST_SUITE_END();

private:
  
public:

  /// Set up the data members
  void setUp()
  {
  }

  /// destroy any on-the-heap-created data member
  void tearDown()
  {
  }

  /// Test that momentum filter cuts are well copied
  void testConstructor()
  {
    {
      DecayParser parser( "23 -> -11 + 11" );
      CPPUNIT_ASSERT( parser.getParents().size()  == 1 );
      CPPUNIT_ASSERT( parser.getChildren().size() == 2 );
    }

    {
      DecayParser parser( "->" );
      CPPUNIT_ASSERT( parser.getParents().size()  == 0 );
      CPPUNIT_ASSERT( parser.getChildren().size() == 0 );
    }    
    {
      try {
        DecayParser parser( " " );
        // should not be there because DecayParser should throw an exception
        CPPUNIT_ASSERT(false);
      } catch (std::invalid_argument& e) {
        std::string error = "problem while parsing command [ ]";
        CPPUNIT_ASSERT( error == e.what() );
      } catch (std::exception& e) {
        std::string error = "problem while parsing command [ ]";
        CPPUNIT_ASSERT( error == e.what() );
      }
    }    
  }

  /// Test the setters and getters
  void testSettersAndGetters()
  {
    DecayParser parser( "->" );
    CPPUNIT_ASSERT( parser.getParents().size()  == 0 );
    CPPUNIT_ASSERT( parser.getChildren().size() == 0 );

    parser.parse( "23 -> -11 + 11" );
    CPPUNIT_ASSERT( parser.getParents().size()  == 1 );
    CPPUNIT_ASSERT( parser.getChildren().size() == 2 );
    
    parser.parse( "6 -> 24 + -5" );
    CPPUNIT_ASSERT( parser.getParents().size()  == 1 );
    CPPUNIT_ASSERT( parser.getChildren().size() == 2 );
    
    parser.parse( "23 -> -11 + 11 + 13" );
    CPPUNIT_ASSERT( parser.getParents().size()  == 1 );
    CPPUNIT_ASSERT( parser.getChildren().size() == 3 );
    
    parser.parse( "6 -> 24 + -5 + 21 + 21" );
    CPPUNIT_ASSERT( parser.getParents().size()  == 1 );
    CPPUNIT_ASSERT( parser.getChildren().size() == 4 );
    
    parser.parse( "21+21 -> -6 + 6" );
    CPPUNIT_ASSERT( parser.getParents().size()  == 2 );
    CPPUNIT_ASSERT( parser.getChildren().size() == 2 );

    parser.parse( "23 -> -11|-13 + 11|13" );
    CPPUNIT_ASSERT( parser.getParents().size()  == 1 );
    CPPUNIT_ASSERT( parser.getChildren().size() == 2 );
    
    parser.parse( "23 -> " );
    CPPUNIT_ASSERT( parser.getParents().size()  == 1 );
    CPPUNIT_ASSERT( parser.getChildren().size() == 0 );
    
    parser.parse( " -> -11 + 11" );
    CPPUNIT_ASSERT( parser.getParents().size()  == 0 );
    CPPUNIT_ASSERT( parser.getChildren().size() == 2 );
    
  }

};

/// Registration of the test suite "DecayParserTest" so it will be 
/// recognized by the main test program which drives the different tests
CPPUNIT_TEST_SUITE_REGISTRATION( DecayParserTest );

/// CppUnit test-driver common for all the cppunit test classes.
/// In ATLAS sw release it is located in TestPolicy package
#include <TestPolicy/CppUnit_testdriver.cxx>
