/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CollListPFN.cpp
 * @brief Utility to list the physical file names used by a POOL collection
 * @author K. Karr <Kristo.Karr@cern.ch>
 * $Id: CollListPFN.cpp 726582 2016-02-27 11:23:37Z krasznaa $
 */

#include "PersistentDataModel/Token.h"

#include "CollectionBase/ICollectionDescription.h"
#include "CollectionBase/ICollectionColumn.h"
#include "CollectionBase/ICollectionQuery.h"
#include "CollectionBase/ICollectionCursor.h"
#include "CollectionBase/CollectionRowBuffer.h"
#include "CollectionBase/CollectionService.h"
#include "CollectionBase/ICollection.h"

#include "FileCatalog/IFileCatalog.h"
#include "POOLCore/Exception.h"
#include "POOLCore/SystemTools.h"

#include "CoralBase/MessageStream.h"

#include "CollectionUtilities/Args2Container.h"
#include "CollectionUtilities/SrcInfo.h"
#include "CollectionUtilities/QueryInfo.h"
#include "CollectionUtilities/CatalogInfo.h"
#include "CxxUtils/checker_macros.h"

#include <iostream>

ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

using namespace std;
using namespace pool;

int main(int argc, const char *argv[])
{
   string thisProgram("CollListPFN");
   SystemTools::initGaudi();
   
   try
   { 
      pool::CollectionService   collectionService;
      coral::MessageStream log( thisProgram );
 
      // vector of CmdLineArg objects
      Args2Container argsVec(thisProgram, true, &log);
      argsVec.desc << thisProgram
                   << " lists Physical File Names of POOL files referenced from the source event collection(s). "
                   << endl;

      CatalogInfo catinfo; argsVec.push_back(&catinfo);
      QueryInfo queryinfo; argsVec.push_back(&queryinfo);
      SrcInfo srcinfo;     argsVec.push_back(&srcinfo);

      // Check that all cmd line args are valid
      if( !argsVec.evalArgs(argc, argv) ) return 1;
      if( !argsVec.checkValid() ) return 1;

      catinfo.setCatalogs( &collectionService );

      pool::IFileCatalog	FC;
      catinfo.setFileCatalogs( &FC );
      FC.start();

      std::map<int, pool::ICollection*> collMap;
      std::map< string, string > guidToPfn;
      std::map< std::string, int > pfnMap;
      std::map< std::string, int > notFoundGuids;
      for( unsigned int i=0; i<srcinfo.nSrc(); i++ ) {
         bool readOnly( true );
         pool::ICollection *collection = collectionService.handle( srcinfo.name(i), srcinfo.type(i), srcinfo.connect(), readOnly, 0 );
         collMap[i] = collection;

         log << coral::Debug
             << " Creating query for the collection" << coral::MessageStream::endmsg;
         pool::ICollectionQuery *collQuery = collection->newQuery();
         collQuery->setCondition( queryinfo.query() );
         if( queryinfo.queryOptions().size() ) {
            collQuery->addToOutputList( queryinfo.queryOptions() );
         } else {
            collQuery->selectAllTokens();
         }
         log << coral::Debug << " Executing the query " << coral::MessageStream::endmsg;
         pool::ICollectionCursor& cursor = collQuery->execute();

         while( cursor.next() ) {
            std::string dbFileId = cursor.eventRef().dbID().toString();
            if( notFoundGuids.find( dbFileId ) != notFoundGuids.end() ) {
               // "known" unknown guid
               notFoundGuids[dbFileId]++;
            } else {
	       if( guidToPfn.find( dbFileId ) != guidToPfn.end() ) {
		  pfnMap[ guidToPfn[dbFileId] ] ++;
	       } else {
		  // new GUID
		  std::string bestPFN;
		  std::string fileType;
		  FC.getFirstPFN( dbFileId, bestPFN, fileType );
		  if( bestPFN.size() ) {
		     // found a name
		     guidToPfn[ dbFileId ] = bestPFN;
		     pfnMap[ bestPFN ] = 1;
		  } else {
		     notFoundGuids[dbFileId] = 1;
		  }
	       }
	    }
	 }
      }
      if( notFoundGuids.size() ) {
         log << coral::Warning << "Found " << notFoundGuids.size()
                   << " unresolved POOL file GUID(s)" << coral::MessageStream::endmsg;
	 std::map< std::string, int >::iterator mapIter;
 	 for( mapIter = notFoundGuids.begin(); mapIter != notFoundGuids.end(); ++mapIter ) {
	    log<< coral::Debug << "   unresolved GUID: " << mapIter->first << coral::MessageStream::endmsg;
	 }
      }
      if( !pfnMap.size() ) {
         log << coral::Warning << "No PFNs found - is your POOL file catalog correctly specified?" << coral::MessageStream::endmsg;
      } else {
         log << coral::Info << "Found PFNs referenced from the collection(s): " << coral::MessageStream::endmsg;
         std::map< std::string, int >::iterator mapIter;
         for( mapIter = pfnMap.begin(); mapIter != pfnMap.end(); ++mapIter ) {
            std::cout << mapIter->first << std::endl;
         }
      }
      for( unsigned int i=0; i<srcinfo.nSrc(); i++ ) {      
         collMap[i]->close();
      }
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
