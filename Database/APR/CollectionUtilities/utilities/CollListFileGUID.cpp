/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CollListFileGUID.cpp
 * @brief Utility to list the file GUID's used by a POOL collection
 * @author K. Karr <Kristo.Karr@cern.ch>
 * @author Marcin.Nowak@cern.ch
 * $Id: CollListFileGUID.cpp 726582 2016-02-27 11:23:37Z krasznaa $
 */

#include "PersistentDataModel/Token.h"

#include "CollectionBase/ICollection.h"
#include "CollectionBase/ICollectionRelationalExtensions.h"
#include "CollectionBase/TokenList.h"
#include "CollectionBase/ICollectionQuery.h"
#include "CollectionBase/ICollectionCursor.h"
#include "CollectionBase/CollectionRowBuffer.h"
#include "CollectionBase/CollectionService.h"
#include "CollectionBase/CollectionDescription.h"

#include "PersistencySvc/ISession.h"
#include "PersistencySvc/DatabaseConnectionPolicy.h"
#include "PersistencySvc/ITransaction.h"

#include "CoralBase/MessageStream.h"
#include "POOLCore/Exception.h"
#include "POOLCore/SystemTools.h"

#include "CollectionUtilities/Args2Container.h"
#include "CollectionUtilities/SrcInfo.h"
#include "CollectionUtilities/QueryInfo.h"
#include "CollectionUtilities/CatalogInfo.h"
#include "CxxUtils/checker_macros.h"

#include <iostream>
#include <iomanip>
#include <memory>

ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

using namespace std;
using namespace pool;


void printGroupedGuids( const ICollectionGUIDQuery::CountedGroupedGUIDs& guids )
{
   cout << "Event count per distinct GUIDs group:" << endl;
   cout << "    #Events";
   for( vector<string>::const_iterator tok = guids.tokenNames.begin(),
           end = guids.tokenNames.end();
        tok != end; ++tok ) {
      cout << std::setw(37) << *tok;
   }
   cout << endl;
 
   for( vector< ICollectionGUIDQuery::countedGUIDGroup_t >::const_iterator
           row = guids.groupedGUIDRows.begin(), end = guids.groupedGUIDRows.end();
        row != end; ++row ) {
      cout << std::setw(11) << row->first << " " ;
      for( vector<string>::const_iterator tok = row->second.begin(),
           end = row->second.end();
           tok != end; ++tok ) {
         cout << *tok << " ";
      }
      cout << endl;
   }
}


void printCountedGuids( const ICollectionGUIDQuery::CountedGroupedGUIDs& guids )
{
   // mapping GUID -> (token_name, count)
   map<string, pair<string,unsigned> > count;
   unsigned     tokenN = guids.tokenNames.size();
   for( vector< ICollectionGUIDQuery::countedGUIDGroup_t >::const_iterator
           row = guids.groupedGUIDRows.begin(), end = guids.groupedGUIDRows.end();
        row != end; ++row ) {
      unsigned n =  row->first;
      for( unsigned i = 0; i<tokenN; i++ ) {
         const string& guid = row->second[i];
         std::pair<std::string,unsigned>& c = count[guid];
         if (c.first.empty()) {
           c.first = guids.tokenNames[i];
           c.second = 0;
         }
         c.second += n;
      }
   }
   for( map<string, pair<string,unsigned> >::const_iterator
           row = count.begin(), end = count.end(); row != end; ++row ) {
      cout << row->first << " "
           << setw(20) << setiosflags(ios::left) << row->second.first
           << "  (" << row->second.second << ")" << endl;
   }
}

   

int main(int argc, const char *argv[])
{
   string       thisProgram("CollListFileGUID");
   SystemTools::initGaudi();
     
   try
   { 
      coral::MessageStream log( thisProgram );
      pool::CollectionService   collectionService;

      // vector of CmdLineArg objects
      Args2Container argsVec(thisProgram, true, &log);
      argsVec.desc << thisProgram
                   << " is a tool for listing POOL file GUIDs referenced from the source event collection(s). "
                   << endl;

      QualList markers;
      markers.insert( make_pair("-maintoken", ArgQual(0)) );
      markers["-maintoken"].desc << "Print only GUIDs from the primary collection Token";
      markers.insert( make_pair("-grouped",    ArgQual(0)) );
      markers["-grouped"].desc << "Relational ONLY: Group and count events that have the same GUIDs in selected tokens";
	  
      CmdLineArgs2 cmdLineArgs;
      cmdLineArgs.setArgQuals(markers);
      argsVec.push_back(&cmdLineArgs); 

      CatalogInfo catinfo; argsVec.push_back(&catinfo);
      QueryInfo queryinfo; argsVec.push_back(&queryinfo);
      SrcInfo srcinfo;     argsVec.push_back(&srcinfo);

     // Check that all cmd line args are valid
      if( !argsVec.evalArgs(argc, argv) ) return 1;
      if( !argsVec.checkValid() ) return 1;

      bool mainToken = cmdLineArgs.hasQual("-maintoken");
      bool countGroupedGuids = cmdLineArgs.hasQual("-grouped");

      if( mainToken && queryinfo.queryOptions().size() ) {
         log << coral::Error << "-maintoken and -queryopt flags are exclusive"
             << coral::MessageStream::endmsg;
         exit(-2); 
      }

      catinfo.setCatalogs( &collectionService );

      size_t	row_count(0);
      std::map< Guid, int > 	fileIdMap;
      std::map< std::string, int > 	emptyTokensCount;
      std::map< Guid, std::string> guidtoNameMap;
      
      for( unsigned int i=0; i<srcinfo.nSrc(); i++ ) {
         bool readOnly( true );
         pool::ICollection *collection = collectionService.handle( srcinfo.name(i), srcinfo.type(i), srcinfo.connect(), readOnly );
         if( mainToken ) {
            log << coral::Info << "Collection: " << collection->description().name()
                << ", Primary Token name is: " << collection->description().eventReferenceColumnName()
                << coral::MessageStream::endmsg; 
         }
         log << coral::Debug << "Creating query for the collection" << coral::MessageStream::endmsg;

         if( collection->description().type() == "RelationalCollection" ) {
            // the fast query version for relational collections
            // (gets only references to the Link table)
            ICollectionRelationalExtensions* relcoll = 
              dynamic_cast<ICollectionRelationalExtensions*>(collection);
            if (!relcoll)
              std::abort();
            unique_ptr<pool::ICollectionGUIDQuery>
               collQuery( relcoll->newGUIDQuery() );
            collQuery->setCondition( queryinfo.query(i) );
            if( mainToken ) {
               collQuery->addToOutputList( collection->description().eventReferenceColumnName() );
            } else if( queryinfo.queryOptions().size() ) {
               collQuery->addToOutputList( queryinfo.queryOptions() );
            } else {
               collQuery->selectAllTokens();
            }
            log << coral::Debug << "Executing fast GUID query for a relational collection"
                << coral::MessageStream::endmsg;
            if( countGroupedGuids ) {
               printGroupedGuids( collQuery->getGroupedGUIDs() );
            } else {
               printCountedGuids( collQuery->getGroupedGUIDs() );
            }
         } else {
            // standard query version - read all rows and collect GUIDs
            pool::ICollectionQuery *collQuery = collection->newQuery();
            collQuery->setRowCacheSize( 10000 );
            collQuery->setCondition( queryinfo.query(i) );
            if( mainToken ) {
               // get the main token only by default
            } else if( queryinfo.queryOptions().size() ) {
               collQuery->skipEventReference( true );
               collQuery->addToOutputList( queryinfo.queryOptions() );
            } else {
               collQuery->selectAllTokens();
            }
            log << coral::Debug
                << ">> Executing the query " << coral::MessageStream::endmsg;
            pool::ICollectionCursor& cursor = collQuery->execute();

            while( cursor.next() ) {
               if( ++row_count % 10000 == 0 ) {
                  log << coral::Debug
                      << "Read in " << row_count << " rows" << coral::MessageStream::endmsg;
               }
               const pool::TokenList &tokens = cursor.currentRow().tokenList();
               for( pool::TokenList::const_iterator iter = tokens.begin();
                    iter != tokens.end(); ++iter ) {
                  Guid fileId = iter->dbID();
                  if( fileId != Guid::null() ) {
                     map< Guid, int >::iterator mapIter = fileIdMap.find( fileId );
                     if( mapIter == fileIdMap.end() ) {
                        fileIdMap[ fileId ] = 1;
                        guidtoNameMap[ fileId ] = iter.tokenName();
                     } else {
                        ++(mapIter->second);
                     }
                  } else {
                     // empty token - count them
                     map< string, int >::iterator mapIter = emptyTokensCount.find( iter.tokenName() );
                     if( mapIter == emptyTokensCount.end() ) {
                        emptyTokensCount[iter.tokenName()] = 1;
                     } else {
                        ++(mapIter->second);
                     }
                  }
               }
            }

            std::map<Guid, int>::iterator it = fileIdMap.begin();
            while( it!=fileIdMap.end() ) {
               std::string tName = "UNKNOWN";
               std::map<Guid, std::string>::iterator name_it = guidtoNameMap.find(it->first);
               if( name_it != guidtoNameMap.end() )
                  tName = name_it->second;
               std::cout << it->first.toString() << " " << setw(20) << setiosflags(ios::left) << tName
                         << "  (" << it->second << ")" << std::endl;
               ++it;
            }
            map< string, int >::iterator it2 = emptyTokensCount.begin();
            while( it2 != emptyTokensCount.end() ) {
               std::cout << "00000000-0000-0000-0000-000000000000" << " " << setw(20) << setiosflags(ios::left)
                         << it2->first << "  (" << it2->second << ")" << std::endl;
               ++it2;
            }
         }
         collection->close();
         delete collection;
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
