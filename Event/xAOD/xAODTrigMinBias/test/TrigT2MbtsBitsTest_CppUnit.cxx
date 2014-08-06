/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Code to test some of the methods in the MissingET object. Especially
// where the code in the TrigMissingET object is anything more than very
// simple.
//

#ifdef ROOTCORE

int main() { return 0; }

#else

#include "xAODTrigMinBias/TrigT2MbtsBits.h"
#include "xAODTrigMinBias/TrigT2MbtsBitsAuxContainer.h"

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Exception.h>

using namespace std;
using namespace xAOD;

///
/// The object that contains all the tests for the TrigT2MbtsTest functionality.
///
class TrigT2MbtsBitsTest : public CppUnit::TestFixture
{
  /////////////
  // Define the tests that are going to be run.
  //

  CPPUNIT_TEST_SUITE (TrigT2MbtsBitsTest);

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
    std::cout<<"creating new TrigT2MbtsBitsAuxContainer() object"<<std::endl;
    TrigT2MbtsBitsAuxContainer *tc = new TrigT2MbtsBitsAuxContainer();

    std::cout<<"creating new TrigT2MbtsBits() object"<<std::endl;
    TrigT2MbtsBits *t = new TrigT2MbtsBits();
    t->setStore(tc);

    t->setTriggerEnergies(std::vector<float>(xAOD::TrigT2MbtsBits::NUM_MBTS,0.));
    t->setTriggerTimes(std::vector<float>(xAOD::TrigT2MbtsBits::NUM_MBTS,2.));
    t->print();
    
    
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

CPPUNIT_TEST_SUITE_REGISTRATION (TrigT2MbtsBitsTest);

#include <TestPolicy/CppUnit_testdriver.cxx>

#endif // ROOTCORE
