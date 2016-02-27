/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CollListToken.cpp
 * @brief Utility to list the event references of a POOL collection
 * @author K. Karr <Kristo.Karr@cern.ch>
 * @author Marcin.Nowak@cern.ch
 * $Id: CollListToken.cpp 726582 2016-02-27 11:23:37Z krasznaa $ 
 */

#include "PersistentDataModel/Token.h"

#include "CollectionBase/ICollection.h"
#include "CollectionBase/TokenList.h"
#include "CollectionBase/ICollectionDescription.h"
#include "CollectionBase/ICollectionColumn.h"
#include "CollectionBase/ICollectionQuery.h"
#include "CollectionBase/ICollectionCursor.h"
#include "CollectionBase/CollectionRowBuffer.h"
#include "CollectionBase/CollectionService.h"

#include "CoralBase/MessageStream.h"
#include "POOLCore/Exception.h"

#include "CollectionUtilities/Args2Container.h"
#include "CollectionUtilities/SrcInfo.h"
#include "CollectionUtilities/QueryInfo.h"
#include "CollectionUtilities/CatalogInfo.h"
#include "CollectionUtilities/MaxEventsInfo.h"

#include <sstream>
#include <iostream>
using namespace std;
using namespace pool;
#define corENDL coral::MessageStream::endmsg

int main(int argc, const char *argv[])
{
   string thisProgram("CollListToken");
   try
   { 
      coral::MessageStream log( thisProgram );
      pool::CollectionService   collectionService;

      // vector of CmdLineArg objects
      Args2Container argsVec(thisProgram, true, &log);
      argsVec.desc << thisProgram
                   << " is a tool for querying an event collection (or collections), "
                   << "and displaying POOL Tokens contained in the query result set. " << endl
                   << "By default all rows are displayed" << endl;

      CatalogInfo catinfo;  argsVec.push_back(&catinfo);
      QueryInfo queryinfo;  argsVec.push_back(&queryinfo);
      SrcInfo srcinfo;      argsVec.push_back(&srcinfo);
      MaxEventsInfo maxEvents; argsVec.push_back(&maxEvents);

      // Check that all cmd line args are valid
      if( !argsVec.evalArgs(argc, argv) ) return 1;
      if( !argsVec.checkValid() ) return 1;

      catinfo.setCatalogs( &collectionService );
      
      std::map<int, pool::ICollection*> collMap;
      unsigned int collCounter = 0;
      for ( unsigned int i=0; i<srcinfo.nSrc(); i++ )
      {
	 collCounter++;
	 
	 bool readOnly( true );
         pool::ICollection *collection = collectionService.handle( srcinfo.name(i), srcinfo.type(i), srcinfo.connect(), readOnly );
	 collMap[i] = collection;

	 const pool::ICollectionDescription &description = collection->description();

	 std::cout << std::endl;
	 std::cout << "Collection name: " << description.name() << std::endl;
	 std::cout << "Collection type: " << description.type() << std::endl;
	 std::cout << "Number of collection fragments: " << description.numberOfCollectionFragments() << std::endl;
         std::cout << "Primary Reference Token name is: " << description.eventReferenceColumnName() << std::endl;
	 std::cout << "Number of tokens: " << description.numberOfTokenColumns() << std::endl;
	 std::cout << "Tokens are: " << std::endl;

	 unsigned  maxNameSize = 0;
	 for( int f=0; f < description.numberOfCollectionFragments(); f++ ) {
	    for( int c=0; c < description.numberOfTokenColumns( f ); c++ ) {
	       const pool::ICollectionColumn&	column = description.tokenColumn(c, f);
	       if( maxNameSize < column.name().size() ) 
		  maxNameSize = column.name().size();
	    }
	 }
	 for( int f=0; f < description.numberOfCollectionFragments(); f++ ) {
	    for( int c=0; c < description.numberOfTokenColumns( f ); c++ ) {
	       const pool::ICollectionColumn&	column = description.tokenColumn(c, f);
	       std::cout.setf(std::ios::left);
	       std::cout << "NAME: ";
	       std::cout.width(maxNameSize+5);
	       std::cout << column.name();
	       std::cout << "INFO: ";
	       std::cout << column.annotation();
	       std::cout << std::endl;
	    }
	 }

	 log << coral::Debug << "Creating query for the collection" << corENDL;
	 pool::ICollectionQuery *collQuery = collection->newQuery();
         collQuery->setCondition( queryinfo.query() );
         if( queryinfo.queryOptions().size() ) {
            collQuery->skipEventReference();
            collQuery->addToOutputList( queryinfo.queryOptions() );
         } else {
            collQuery->selectAllTokens();
         }
	 log << coral::Debug << "Executing query: " << corENDL;
	 pool::ICollectionCursor& cursor = collQuery->execute();	       

	 int evtCounter = 0;
	 while( cursor.next() ) {
	    evtCounter++;
            if( maxEvents.specified() && evtCounter > maxEvents.get()  )
               break;

            std::cout << "Number of events counted: " << evtCounter << std::endl;
            std::cout << "Tokens: " << std::endl;

            const pool::TokenList &tokens = cursor.currentRow().tokenList();
            for( pool::TokenList::const_iterator iter = tokens.begin();
                 iter != tokens.end(); ++iter ) {
               std::cout.setf(std::ios::left);
               std::cout << "NAME: ";
               std::cout.width(maxNameSize+5);
               std::cout << iter.tokenName();
               std::cout << "VALUE: " << iter->toString();
               std::cout << std::endl;
            }
	 }
      }

      for( unsigned int i=0; i<<srcinfo.nSrc(); i++ ) {      
	 collMap[i]->close();
      }

      return 0;
   }
   catch( pool::Exception& poolException )
   {
      std::cerr << "pool::Exception: " << poolException.what() << std::endl;;
      return 1;
   }
   catch( std::exception& exception )
   {
      std::cerr << "std::exception: " << exception.what() << std::endl;;
      return 1;
   }
   catch( ... )
   {
      std::cerr << "Unknown exception caught in main()." << std::endl;;
      return 1;
   }
}
