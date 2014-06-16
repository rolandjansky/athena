/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <string>
#include <iostream>

/* MN: putting common functionality of the test driver in one place
 */

#include "DataModelRoot/RootType.h"

#include "RVersion.h"
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,0)
#include "Reflex/SharedLibrary.h"
std::string libname (const std::string &name)
{ 
#ifdef _WIN32 
  return name + ".dll"; 
#elif defined __hpux 
  return "lib" + name + ".sl"; 
#else 
  return "lib" + name + ".so"; 
#endif 
} 
#else
#include "GaudiKernel/System.h"
#endif


namespace pool {}
using namespace pool;
void TestDriver::loadLibraries( const std::vector<std::string>& libraries )
{
  for ( std::vector<std::string>::const_iterator iLibrary = libraries.begin();
	iLibrary != libraries.end(); ++iLibrary ) {
    const std::string& library = *iLibrary;
    std::cout << "Loading library " << library << std::endl;

#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,0)
    Reflex::SharedLibrary libloader( libname(library) );    
    if(!libloader.Load()){
      std::cout << libloader.Error() << std::endl;
    }
#else
    System::ImageHandle         libhandle;
    if( System::loadDynamicLib(library, &libhandle) != 1 ) {
       std::cout << "Error! library loading failed"  << std::endl;
    }
#endif    
  }
}
