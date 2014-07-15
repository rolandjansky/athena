/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigElectronTest_CppUnit.cxx 602825 2014-06-20 07:29:53Z krasznaa $
//
//  Some simple tests for TrigElectron
//

#ifndef ROOTCORE

#include "xAODTrigEgamma/TrigElectron.h"

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Exception.h>
#include <iostream>

using namespace xAOD;

class TrigElectronTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE (TrigElectronTest);

  CPPUNIT_TEST ( simpleCtor );

  CPPUNIT_TEST_SUITE_END ();

  void simpleCtor() {
    TrigElectron *e = new TrigElectron();
  }

};
CPPUNIT_TEST_SUITE_REGISTRATION (TrigElectronTest);

#include <TestPolicy/CppUnit_testdriver.cxx>

#else

int main() { return 0; }

#endif // not ROOTCORE
