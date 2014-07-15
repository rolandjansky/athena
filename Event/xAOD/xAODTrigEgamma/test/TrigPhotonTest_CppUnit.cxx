/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigPhotonTest_CppUnit.cxx 602825 2014-06-20 07:29:53Z krasznaa $
//
//  Some simple tests for TrigPhoton
//

#ifndef ROOTCORE

#include "xAODTrigEgamma/TrigPhoton.h"

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Exception.h>
#include <iostream>

using namespace xAOD;

class TrigPhotonTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE (TrigPhotonTest);

  CPPUNIT_TEST ( simpleCtor );

  CPPUNIT_TEST_SUITE_END ();

  void simpleCtor() {
    TrigPhoton *e = new TrigPhoton();
  }

};
CPPUNIT_TEST_SUITE_REGISTRATION (TrigPhotonTest);

//#include <TestPolicy/CppUnit_testdriver.cxx>

#else

int main() { return 0; }

#endif // not ROOTCORE
