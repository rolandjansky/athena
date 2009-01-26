/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include"CppUnitSGServiceExample/SimpleDataVector.h" 
#include<cppunit/extensions/HelperMacros.h>
#include<cppunit/Exception.h>
#include<iostream>
/** Class to test the "SimpleDataVector.h" class 
*   StoreGate service required
*
*  Use the HelperMacros.h header file to establish in a easy 
*  way the "SimpleDataVectorTest" test suite in which you can 
*  place all the tests concerning the tested class
*/
class SimpleDataVectorTest : public CppUnit::TestFixture
{
/// Definition of the unit test suite "SimpleDataVectorTest"
 CPPUNIT_TEST_SUITE( SimpleDataVectorTest );
 CPPUNIT_TEST( testSDV );
 CPPUNIT_TEST( testSDV_false_negative );
 CPPUNIT_TEST_SUITE_END();
 private:

 public:
   void setUp()
   {
   }
   void tearDown()
   {
   }
/// the test
   void testSDV()
   {
     std::cout << " " << std::endl;
     std::cout << "================================================================" << std::endl;
     std::cout << "This is a simple CppUnit test of class SimpleDataVector." << std::endl;
     std::cout << "The CppUnit test driver with StoreGate service is used." << std::endl; 
     std::cout << "CppUnit tests can be included in ATLAS nightlies." << std::endl;   
     std::cout << "Details in http://atlas.web.cern.ch/Atlas/GROUPS/SOFTWARE/" << std::endl;
     std::cout << "OO/dist/nightlies/nicoswww/cppunit_testing.html" << std::endl;
     std::cout << "================================================================" << std::endl;
                                                                             
     typedef DataVector<int> IntVector;
     IntVector intV;
     CPPUNIT_ASSERT(intV.empty());
     intV.reserve(10);
     CPPUNIT_ASSERT(intV.capacity() == 10);
     unsigned int i;
     const unsigned int IFILL(3);
     for (i=0; i<IFILL; ++i)  intV.push_back(new int(i));
     CPPUNIT_ASSERT (IFILL == intV.size());
     CPPUNIT_ASSERT(distance(intV.begin(), intV.end()) == (int)IFILL);
     CPPUNIT_ASSERT(*(intV.front()) == 0);
     CPPUNIT_ASSERT(2 == *(intV.back()));
     intV.front() = new int(7);
     CPPUNIT_ASSERT(7 == *(intV.front()));
     *(intV.back()) = 3;

     DataVector<SimpleTestStruct>* aa = new DataVector<SimpleTestStruct>();
     aa->push_back(new SimpleTestStruct());
     aa->push_back(new SimpleTestStruct(9));

     CPPUNIT_ASSERT(2 == aa->size());

     DataVector<SimpleTestStruct>::iterator iter = aa->begin();
     int n = 0;
     for (; iter != aa->end(); iter++) {
       (*iter)->foo();
       (*iter)->fooMessageSvc(); 
       n++;
     }
     CPPUNIT_ASSERT(2 == n);

     delete aa;
   }
void testSDV_false_negative()
  {
    DataVector<SimpleTestStruct>* aa = new DataVector<SimpleTestStruct>();
    aa->push_back(new SimpleTestStruct(9));
    CPPUNIT_ASSERT_MESSAGE( "This test fails intentionally", 2 == aa->size());
    delete aa;
  }
};
/// Registration of the test suite "SimpleDataVectorTest", so it 
/// will be recognized by the main test program which drives the 
/// different tests. 
CPPUNIT_TEST_SUITE_REGISTRATION(SimpleDataVectorTest);
/// CppUnit test-driver common for all the cppunit test classes.
/// In ATLAS sw release it is located in TestPolicy package.
#include <TestPolicy/CppUnit_SGtestdriver.cxx>
