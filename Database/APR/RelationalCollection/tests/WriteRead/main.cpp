/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <cstdlib>
#include <exception>

#ifdef WIN32
#   include <windows.h>
#   define sleep(x) Sleep((x)*1000)   /* Windows version */
#else
#   include <unistd.h>
#endif

#include "TestDriver.h"


int main( int, char** )
{
   try {
      // const std::string userNameEnv = "CORAL_AUTH_USER=ral_writer";
//       ::putenv( const_cast<char*>( userNameEnv.c_str() ) );
//       const std::string passwdEnv = "CORAL_AUTH_PASSWORD=ral";
//       ::putenv( const_cast<char*>( passwdEnv.c_str() ) );

//      const std::string connection = "write_test";
      const std::string connection = "POOL-Collection-Oracle-lcgnight";
      //const std::string connection = "sqlite://pool.db";
      //const std::string connection = "mysql://pcitdb59/IOANNISDB";

      std::string name = "/My/First/Collection";
      std::cout << "ReadWrite test starting..." << std::endl;

      for ( int i = 0; i < 2; ++i ) {
         TestDriver driver( name, connection );

         std::cout << std::endl << "************************************" << std::endl;
         std::cout << "Writing the collection" << std::endl;
         driver.write();

         ::sleep( 2 );

         std::cout << std::endl << "************************************" << std::endl;
         std::cout << "Reading the collection" << std::endl;
         driver.read();

         name = "/My/Second/Collection";
      }

      std::cout << "Exiting..." << std::endl;
   }
   catch ( std::exception& e ) {
    std::cerr << "---> Exception:" << std::endl;
    std::cerr << e.what() << std::endl;
    return 1;
  }
  catch (...) {
    std::cerr << "Unhandled exception..." << std::endl;
    return 1;
  }
  return 0;
}
