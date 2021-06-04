/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CollRemove.cpp
 * @brief Utility to delete a POOL collection
 * @author Marcin.Nowak@cern.ch
 */
#include "CollectionBase/ICollectionDescription.h"
#include "CollectionBase/CollectionService.h"
#include "CollectionBase/ICollection.h"

#include "CoralBase/MessageStream.h"
#include "POOLCore/Exception.h"

#include "CollectionUtilities/Args2Container.h"
#include "CollectionUtilities/SrcInfo.h"
#include "CollectionUtilities/CatalogInfo.h"
#include "CxxUtils/checker_macros.h"

#include <iostream>

ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

using namespace std;
using namespace pool;


int main(int argc, const char *argv[])
{
   string thisProgram("CollRemove");
   try
   { 
      coral::MessageStream log( thisProgram );
      pool::CollectionService   collectionService;

      // vector of CmdLineArg objects
      Args2Container argsVec(thisProgram, true, &log);
      argsVec.desc << thisProgram
                   << " is a tool for deleting Relational POOL Collections. "
                   << endl;

      CatalogInfo catinfo; argsVec.push_back(&catinfo);
      SrcInfo srcinfo;     argsVec.push_back(&srcinfo);

      // Check that all cmd line args are valid
      if( !argsVec.evalArgs(argc, argv) ) return 1;
      if( !argsVec.checkValid() ) return 1;

      catinfo.setCatalogs( &collectionService );

      std::map<int, pool::ICollection*> collMap;
      std::map< std::string, int > pfnMap;
      for( unsigned int i=0; i<srcinfo.nSrc(); i++ ) {
         if( srcinfo.type(i)=="RootCollection" ) {
            log << coral::Error
                << "This tool only supports deleting Relational Collections"
                << coral::MessageStream::endmsg;
            return 2;
         }
         log << coral::Info
             << "Removing collection '" << srcinfo.name(i)
             << "', type: " <<  srcinfo.type(i)
             << coral::MessageStream::endmsg;

         bool deleteChildFragments = true;
         bool ignoreExternalDependencies = true;   // ?? MN
         bool result = collectionService.drop( srcinfo.name(i), srcinfo.type(i), srcinfo.connect(), deleteChildFragments, ignoreExternalDependencies);
         if( !result ) {
            log << coral::Error
                << "Problems encountered when removing collection '" << srcinfo.name(i)
                << "', type: " <<  srcinfo.type(i)
                << coral::MessageStream::endmsg;
         }
      }

      log << coral::Info << "Collection(s) deleted" <<  coral::MessageStream::endmsg;
      return 0;

   }
   catch( pool::Exception& poolException )
   {
      std::cerr << "pool::Exception: " << poolException.what() << std::endl;
      return 1;
   }
   catch( std::exception& exception )
   {
      std::cerr << "std::Exception: " << exception.what() << std::endl;
      return 1;
   }
   catch( ... )
   {
      std::cerr << "Unknown exception. " << std::endl;
      return 1;
   }
}





