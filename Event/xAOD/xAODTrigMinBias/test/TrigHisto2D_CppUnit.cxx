/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//
// Code to test some of the methods in the MissingET object. Especially
// where the code in the TrigMissingET object is anything more than very
// simple.
//

#ifdef ROOTCORE

int main() { return 0; }

#else

#include "TRandom.h"

#include "xAODTrigMinBias/TrigHisto2D.h"
#include "xAODTrigMinBias/TrigHisto2DAuxContainer.h"

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Exception.h>

using namespace std;
using namespace xAOD;

///
/// The object that contains all the tests for the TrigT2MbtsTest functionality.
///
class TrigHisto2DTest : public CppUnit::TestFixture
{
  /////////////
  // Define the tests that are going to be run.
  //

  CPPUNIT_TEST_SUITE (TrigHisto2DTest);

  CPPUNIT_TEST (testSimpleCreation);
  //  CPPUNIT_TEST_EXCEPTION (testSetComponentNotAllocated, std::out_of_range);
  //CPPUNIT_TEST (setPreallocatedComponent);
  //CPPUNIT_TEST (preallocatedComponentHasNameSet);
  //CPPUNIT_TEST (setPreallocatedComponentSetZero);
  //CPPUNIT_TEST_EXCEPTION (cantPreallocateTwice, std::runtime_error);

  CPPUNIT_TEST_SUITE_END();
  // End of the test suite definition.
  ////////////

  // Make sure we can create and save a simple variable (non-component).
  void testSimpleCreation()
  {
    std::cout<<"creating new TrigHisto2DAuxContainer() object"<<std::endl;
//     TrigHisto2DAuxContainer *tc = new TrigHisto2DAuxContainer();

    std::cout<<"creating new TrigHisto2D() object"<<std::endl;
    TrigHisto2D *t = new TrigHisto2D();
//     t->setStore(tc);

    t->initialize(4, -2., 2, 6, -3, 3); 
    
    TrigHisto2D *hist1 = new TrigHisto2D(3, 0., 3., 5, 0., 5);
    
    for(unsigned int i = 0 ; i < 100; i++){
      hist1->fill(gRandom->Rndm()*5-1, gRandom->Rndm()*5-1,1);
      hist1->fill(gRandom->Rndm()*5-1, gRandom->Rndm()*7-1,1);
    }
    
    hist1->dump();
      
    std::cout<< "Now testing profile functions()"<< std::endl;
    
    std::vector<float> profX = hist1->profileX();
    std::vector<float> profY = hist1->profileY();
    
    
    std::cout<<"Profile X"<< std::endl;

    for (float f : profX)
      std::cout<<(f)<<"\t";
    std::cout<<std::endl;
    
    
    std::cout<<"Profile Y"<< std::endl;

    for (float f : profY)
      std::cout<<(f)<<"\t";
    std::cout<<std::endl;
    
    std::cout<<">sumEntries( 2.4, 2.4, TrigHistoCutTypes::BELOW_X_BELOW_Y) = "<<hist1->sumEntries( 2.4, 2.4, TrigHistoCutType::BELOW_X_BELOW_Y)<<std::endl;
    std::cout<<">sumEntries( 2.4, 2.4, TrigHistoCutTypes::ABOVE_X_BELOW_Y) = "<<hist1->sumEntries( 2.4, 2.4, TrigHistoCutType::ABOVE_X_BELOW_Y)<<std::endl;
    std::cout<<">sumEntries( 2.4, 2.4, TrigHistoCutTypes::BELOW_X_ABOVE_Y) = "<<hist1->sumEntries( 2.4, 2.4, TrigHistoCutType::BELOW_X_ABOVE_Y)<<std::endl;
    std::cout<<">sumEntries( 2.4, 2.4, TrigHistoCutTypes::ABOVE_X_ABOVE_Y) = "<<hist1->sumEntries( 2.4, 2.4, TrigHistoCutType::ABOVE_X_ABOVE_Y)<<std::endl;
    
    /*
    CPPUNIT_ASSERT_EQUAL (5, t->flag());
    CPPUNIT_ASSERT_DOUBLES_EQUAL (10.0, t->ex(), 0.001);
    */
  }

  /*
  // Make sure we fail if we try to set the value of a component that we've
  // not yet defined.
  void testSetComponentNotAllocated()
  {
    TrigMissingETAuxContainer *tc = new TrigMissingETAuxContainer();
    TrigMissingET *t = new TrigMissingET();
    t->setStore(tc);

    // Should cause failure since we've not pre-allocated the component
    t->setNameOfComponent(4, "hi");
  }

  // Make sure we can set a preallocated component value, as expected.
  void setPreallocatedComponent()
  {
    TrigMissingETAuxContainer *tc = new TrigMissingETAuxContainer();
    TrigMissingET *t = new TrigMissingET();
    t->setStore(tc);

    // Allocate the components
    vector<string> names;
    names.push_back("hi");
    names.push_back("there");
    t->defineComponents(names);

    // Should cause failure since we've not pre-allocated the component
    t->setExComponent(1, 5.4);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (5.4, t->exComponent(1), 0.0001);
  }

  // Make sure names are being saved in the pre allocated components
  void preallocatedComponentHasNameSet()
  {
    TrigMissingETAuxContainer *tc = new TrigMissingETAuxContainer();
    TrigMissingET *t = new TrigMissingET();
    t->setStore(tc);

    // Allocate the components
    vector<string> names;
    names.push_back("hi");
    names.push_back("there");
    t->defineComponents(names);

    // Should cause failure since we've not pre-allocated the component
    CPPUNIT_ASSERT_EQUAL (string("there"), t->nameOfComponent(1));
  }

  // Make sure that component variables are initialized to zero when allocated.
  void setPreallocatedComponentSetZero()
  {
    TrigMissingETAuxContainer *tc = new TrigMissingETAuxContainer();
    TrigMissingET *t = new TrigMissingET();
    t->setStore(tc);

    // Allocate the components
    vector<string> names;
    names.push_back("hi");
    names.push_back("there");
    t->defineComponents(names);

    // Should cause failure since we've not pre-allocated the component
    CPPUNIT_ASSERT_DOUBLES_EQUAL (0.0, t->exComponent(1), 0.0001);
  }

  // It is not legal to allocate a set of components twice. This should throw
  // an exception.
  void cantPreallocateTwice()
  {
    TrigMissingETAuxContainer *tc = new TrigMissingETAuxContainer();
    TrigMissingET *t = new TrigMissingET();
    t->setStore(tc);

    // Allocate the components
    vector<string> names;
    names.push_back("hi");
    names.push_back("there");
    t->defineComponents(names);
    t->defineComponents(names);
  }
  */
};

CPPUNIT_TEST_SUITE_REGISTRATION (TrigHisto2DTest);

#include <TestTools/CppUnit_testdriver.cxx>

#endif // ROOTCORE
