/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include"CppUnitExample/ComplexNumber.h" 
#include<cppunit/extensions/HelperMacros.h>
#include<cppunit/Exception.h>
#include<iostream>
/** Class to test the "ComplexNumber.h" class 
*
*  Use the HelperMacros.h header file to establish in a easy 
*  way the "ComplexNumberTest" test suite in which you can 
*  place all the tests ("testEquality", "testAddition"... ) 
*  concerning the tested class "ComplexNumber.h"
*/
class ComplexNumberTest : public CppUnit::TestFixture
{
/// Definition of the unit test suite "ComplexNumberTest"
 CPPUNIT_TEST_SUITE( ComplexNumberTest );
 CPPUNIT_TEST( testEquality );
 CPPUNIT_TEST( testAddition );
 CPPUNIT_TEST_SUITE_END();
 private:
   ComplexNumber *m_10_1, *m_1_1, *m_11_2;
 public:
   void setUp()
   {
     m_10_1 = new ComplexNumber(10,1);
     m_1_1  = new ComplexNumber(1,1);
     m_11_2 = new ComplexNumber(11,2);  
   }
   void tearDown()
   {
     delete m_10_1;
     delete m_1_1;
     delete m_11_2;
   }
/// the first test
   void testEquality()
   {
     std::cout << " " << std::endl;
     std::cout << "================================================================" << std::endl;
     std::cout << "This is a simple CppUnit test of class ComplexNumber." << std::endl;
     std::cout << "Other CppUnit tests can be included in ATLAS nightlies." << std::endl;   
     std::cout << "Details in http://atlas.web.cern.ch/Atlas/GROUPS/SOFTWARE/" << std::endl;
     std::cout << "OO/dist/nightlies/nicoswww/cppunit_testing.html" << std::endl;
     std::cout << "================================================================" << std::endl;
     CPPUNIT_ASSERT( (*m_10_1 == *m_10_1) );
     CPPUNIT_ASSERT_MESSAGE( "This test fails intentionally", *m_10_1 == *m_1_1 );
   }
/// the second test
   void testAddition()
   {
     CPPUNIT_ASSERT( *m_10_1 + *m_1_1 == *m_11_2 );
   }
};
/// Registration of the test suite "ComplexNumberTest", so it 
/// will be recognized by the main test program which drives the 
/// different tests. 
CPPUNIT_TEST_SUITE_REGISTRATION(ComplexNumberTest);
/// CppUnit test-driver common for all the cppunit test classes.
/// In ATLAS sw release it is located in TestPolicy package.
#include <TestPolicy/CppUnit_testdriver.cxx>
 
