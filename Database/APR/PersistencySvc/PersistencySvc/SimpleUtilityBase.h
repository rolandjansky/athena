/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef POOL_SIMPLEUTILITYBASE
#define POOL_SIMPLEUTILITYBASE

#include <iostream>
#include <stdexcept>

#include "StorageSvc/IStorageSvc.h"
#include "StorageSvc/IStorageExplorer.h"
#include "StorageSvc/DbType.h"
#include "StorageSvc/DbOption.h"
#include "StorageSvc/FileDescriptor.h"


namespace pool {
   
/// Common base class for POOL utilities
   class SimpleUtilityBase
   {
    public:
     /// constructor taking commandline parmeters
     SimpleUtilityBase( int argc = 0, char* argv[] = 0 );
     virtual ~SimpleUtilityBase();

     virtual int	run();

     virtual bool 	parseArguments(); 
     virtual void 	startSession();
     virtual void 	readFileGUIDs();
     virtual std::string readFileGUID( const std::string& pfn );

     virtual void 	execute() {}
     virtual void 	printSyntax() {}

    protected:
     std::string	        executableName;
     std::string                technologyName;

     pool::Session*             session = nullptr;
     pool::IStorageSvc*         storageSvc = nullptr;
     pool::IStorageExplorer*    storageExplorer = nullptr;

     std::vector< std::string > args;
     std::vector< std::string > fileNames;
     std::vector< std::pair< std::string, std::string > > fidAndPfn;
   };


   /// shortcut for reading file GUID from python
   /* this is a class and not a namespace so it can be autoloaded by ROOT */
   class Utils {
    public:
     static std::string        readFileGUID( const std::string& pfn );
   };

   
} // pool namespace
#endif
