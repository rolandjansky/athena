/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <string>
#include <iostream>

/* MN: putting common functionality of the test driver in one place
 */

#include "DataModelRoot/RootType.h"
#include "GaudiKernel/System.h"


namespace pool {}
using namespace pool;
void TestDriver::loadLibraries( const std::vector<std::string>& libraries )
{
  for ( std::vector<std::string>::const_iterator iLibrary = libraries.begin();
	iLibrary != libraries.end(); ++iLibrary ) {
    const std::string& library = *iLibrary;
    std::cout << "Loading library " << library << std::endl;

    System::ImageHandle         libhandle;
    if( System::loadDynamicLib(library, &libhandle) != 1 ) {
       std::cout << "Error! library loading failed"  << std::endl;
    }
  }
}
