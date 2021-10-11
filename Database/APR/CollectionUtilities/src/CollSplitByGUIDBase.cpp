/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CollSplitByGUIDBase.cpp
 * @brief Base class for the utility to list the file GUIDs used by a POOL collection and split the
 * collection into sub-collections by GUID
 * @author K. Karr <Kristo.Karr@cern.ch>, C. Nicholson <Caitriana.Nicholson@cern.ch>, Marcin.Nowak@cern.ch
 * $Id: CollSplitByGUIDBase.cpp 726582 2016-02-27 11:23:37Z krasznaa $
 */

#include "PersistentDataModel/Token.h"

#include "CollectionBase/CollectionService.h"
#include "CollectionBase/CollectionDescription.h"
#include "CollectionBase/CollectionRowBuffer.h"
#include "CollectionBase/TokenList.h"
#include "CollectionBase/ICollectionQuery.h"
#include "CollectionBase/ICollectionCursor.h"
#include "CollectionBase/ICollectionDataEditor.h"
#include "CollectionBase/ICollectionMetadata.h"

#include "CoralBase/Attribute.h"
#include "CoralBase/MessageStream.h"

#include "POOLCore/Exception.h"

#include "CollectionUtilities/CollSplitByGUIDBase.h"


#include <iostream>
#include <sstream>
#include <fstream>
#include <memory>
#include <stdexcept>

using namespace std;
using namespace pool;

#define corENDL coral::MessageStream::endmsg


bool guidPairSort( const pair<Guid, int> &p1, const pair<Guid, int> &p2){
  return p1.second > p2.second;
}



CollSplitByGUIDBase::CollSplitByGUIDBase(const std::string& name) :
      m_thisProgram( name ),
      m_maxSplit( 500 ),
      m_minEvents( -1 ),
      m_rowsCached( 1000 ),
      m_numEventsPerCommit( static_cast<unsigned int>(-1) ),
      m_numRowsCached( 0 ),
      m_collectionService( new pool::CollectionService() ),
      m_log( name ),
      m_argsVec( name, true, &m_log ),
      m_collectionPool( 0 ),
      m_outputCollSeqN( 1 )

{
}


CollSplitByGUIDBase::~CollSplitByGUIDBase()
{
   delete m_collectionPool;
   delete m_collectionService;
}




int
CollSplitByGUIDBase::execute( std::vector<std::string> argv_v )
{
   // Primary try block
   try {
      if( init( argv_v ) ) {
	 //time(&m_starttime);
	 openSourceCollections();
	 openDestCollections();
	 copyRows();
	 //time(&m_endtime);
	 copyMetadata();
	 finalize();
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
      std::cerr << "std::exception: " << exception.what() << std::endl;
      return 1;
   }
   catch( ... )
   {
      std::cerr << "Unknown exception caught in main()." << std::endl;
      return 1;
   }
}




bool
CollSplitByGUIDBase::init( std::vector<std::string> argv_v )
{
   m_argsVec.desc << m_thisProgram << " is a tool for querying an event collection, or "
                 << "collections, and storing the results in a number of output collections, "
                 << "one for each different event file GUID. " << endl
                 << "Currently, these sub-collections are output as LOCAL, "
                 << "ROOT-based collections with a fixed name and will appear in the directory from "
                 << "which the program was executed." << endl;

    // list of CollAppend *specific* cli keys and their argument properties
    QualList markers;
    markers["-guidfile"].desc << "List of GUIDs for output collections. One GUID per line. Lines starting with ':' assign collection name for GUIDS that follow";
    markers["-maxsplit"].desc << "Limit number of produced subcollections. Spillover will be stored in the last collection";
    markers["-rowscached"].desc << "Number of rows cached in memory for each output collection that is not in open collections pool. Bigger cache may speed up writing, but uses more memory. DEFAULT=" << m_rowsCached;
    markers["-splitref"].desc << "Name of ref to use for boundaries of split (DEFAULT=primary ref)";
    markers["-minevents"].desc << "minimum number of events required to create a separate output collection for a particular GUID";
    markers["-nevtpercommit"].desc << "Max. number of events to process between "
                                   << "output transaction commits (default is infinity)";
    markers["-nevtcached"].desc << "size of the insert buffer for bulk operations "
                                << "DEFAULT = 0 (no bulk operations)";

    CmdLineArgs2 cmdLineArgs;
    cmdLineArgs.setArgQuals(markers);
    m_argsVec.push_back(&cmdLineArgs);  // Add it to the list

    // Classes with shared cli keys and their argument properties
    // Add them to the list
   m_argsVec.push_back(&m_catinfo);
   m_argsVec.push_back(&m_queryinfo);
   m_argsVec.push_back(&m_srcinfo);
   m_argsVec.push_back(&m_metainfo);

    // Check that all cmd line args are valid
   if( !m_argsVec.evalArgs(argv_v) ) return false;
   if( !m_argsVec.checkValid() )     return false;

   (void)cmdLineArgs.getOpt("-splitref", m_splitRef);
   (void)cmdLineArgs.getOpt("-minevents", m_minEvents);
   (void)cmdLineArgs.getOpt("-rowscached", m_rowsCached);
   (void)cmdLineArgs.getOpt("-maxsplit", m_maxSplit);
   if( m_maxSplit < 1 )  m_maxSplit = 1;
   string guidfile;
   if( cmdLineArgs.getOpt("-guidfile", guidfile) ) {
      if( !readGuidList(guidfile) ) {
         exit( -5 );
      }
   }
   // ---
   // EXPERT OPTIONS
   // For tuning purposes the number of events between commits can be specified
   // ---
   (void)cmdLineArgs.getOpt("-nevtpercommit", m_numEventsPerCommit);
   (void)cmdLineArgs.getOpt("-nevtcached", m_numRowsCached);

   m_catinfo.setCatalogs( m_collectionService );
   if( !m_collectionPool ) m_collectionPool = new CollectionPool(50, m_rowsCached);

   return true;
}



void
CollSplitByGUIDBase::openSourceCollections()
{
   for( unsigned int i=0; i<m_srcinfo.nSrc(); i++ )   {
      m_log << coral::Info
	    << "Opening source collection " << m_srcinfo.name(i)
	    << " of type " << m_srcinfo.type(i) <<  coral::MessageStream::endmsg;
      bool readOnly( true );
      pool::ICollection *collection = m_collectionService->handle( m_srcinfo.name(i), m_srcinfo.type(i), m_srcinfo.connect(), readOnly );
      m_srcCollections.push_back( collection );

      if( (m_queryinfo.query() != "" || m_queryinfo.queryOptions() != "")
	  && !collection->description().equals( m_srcCollections[0]->description() ) ) {
	 m_log << coral::Warning << " The schemas of one or more "
	       << "input collections are different and a query has been "
	       << "requested. This may lead to unexpected behavior."
	       << coral::MessageStream::endmsg;
      }
   }
}



void
CollSplitByGUIDBase::openDestCollections()
{
   std::map< Guid, int >  eventsPerGuid;
   std::multimap< std::string, std::string> invCollMap;  // map to keep sub-collection name --> guid

   for( unsigned int i=0; i<m_srcCollections.size(); i++ )
   {
      ICollection* collection = m_srcCollections[i];
      pool::ICollectionQuery *collQuery = collection->newQuery();
      collQuery->setCondition( m_queryinfo.query() );
      if( m_queryinfo.queryOptions().size() ) {
	 collQuery->addToOutputList( m_queryinfo.queryOptions() );
      } else {
	 collQuery->selectAll();
      }

      m_log << coral::Info << "Executing query for the source collection" << coral::MessageStream::endmsg;
      pool::ICollectionCursor& cursor = collQuery->execute();

      // set parameters for the sub-collections.
      // currently caters only for writing local, Root-based collections.
      std::string subCollType = "RootCollection";
      std::string subCollConnect = "";

      // first loop: for each event, find the GUID / fileId and
      // count how many events match that GUID
      int totalEvents = 0;
      int uniqueGuids = 0;

      // Token name to split on (if not specified, use default for each source coll)
      string refname = ( m_splitRef.size()? m_splitRef : collection->description().eventReferenceColumnName() );
      while( cursor.next() ) {
	 const pool::TokenList &tokens = cursor.currentRow().tokenList();
	 for( pool::TokenList::const_iterator iter = tokens.begin(); iter != tokens.end(); ++iter ) {
	    if( iter.tokenName() == refname ) {
	       Guid guid = iter->dbID();
	       if( eventsPerGuid.find( guid ) == eventsPerGuid.end() ) {
		  // new unique GUID found
		  eventsPerGuid[ guid ] = 1;
		  uniqueGuids++;
	       }
	       else {
		  eventsPerGuid[ guid ]++;
	       }
	    }
	 }
	 totalEvents++;
      }
      m_log << coral::Info << "Collection " << collection->description().name() << " has " << totalEvents
	    << " entries with " << uniqueGuids << " unique file GUIDs in Token " << refname
	    << coral::MessageStream::endmsg;

      for( map<Guid, int>::iterator guidIter1 = eventsPerGuid.begin(),
	      end = eventsPerGuid.end(); guidIter1 != end; ++guidIter1 )
      {
	 m_log << coral::Debug << "GUID: " << guidIter1->first << ", rows: "
	       << guidIter1->second << corENDL;
      }
      //--------  make suitable output collections ( each with nEvents >= m_minEvents )
      int createdCollections = 0;

      vector<pair<Guid, int> > sortedGuids;
      map<string, ICollection*> collMap;
      for( map<Guid, int>::iterator guidIter1 = eventsPerGuid.begin(),
	      end = eventsPerGuid.end(); guidIter1 != end; ++guidIter1 )
      {
	 string guid = guidIter1->first.toString();
	 string subCollName = collectionNameForGuid( guid );
	 if( !subCollName.empty() )
	 {
	    // process guid from a list
	    pool::ICollection* subCollection = 0;
	    if( collMap.find( subCollName ) == collMap.end() )
	    {
	       // create a new collection
	       pool::CollectionDescription newDestDesc( collection->description() );
	       newDestDesc.setName( subCollName );
	       newDestDesc.setType( subCollType );
	       newDestDesc.setConnection( subCollConnect );
	       subCollection = m_collectionService->create( newDestDesc, true );
	       createdCollections++;
	       subCollection->dataEditor().setRowCacheSize( 0 );
	       collMap[ subCollName ] = subCollection;
	    }
	    else
	    {
	       // find an already created collection
	       subCollection = collMap[ subCollName ];
	    }
	    // map to appropriate GUID
	    m_collectionPool->addCollection( guid, subCollection );
	    invCollMap.insert( std::pair<std::string, std::string>( subCollName, guid ) );
	 }
	 else
	 {
	    // guid not from the list, keep it for default processing
	    sortedGuids.push_back( *guidIter1 );
	 }
      }
      // sort the remaining GUIDs by cardinality
      sort( sortedGuids.begin(), sortedGuids.end(), guidPairSort );

      int rowCounter = 0;
      unsigned guidCounter = 0;
      pool::ICollection* subCollection = 0;
      string subCollName;
      vector<pair<Guid, int> >::iterator guidIter2 = sortedGuids.begin();
      while( guidIter2 != sortedGuids.end() )
      {
	 guidCounter++;
	 std::string guid = guidIter2->first.toString();
	 int thisNumEvents = guidIter2->second;
	 bool collLimit = ( createdCollections >= m_maxSplit );

	 // create a new output collection if
	 if( !subCollection    // there is no collection yet or
	     || ( rowCounter >= m_minEvents    // enough events were written to the previous one
		  && !collLimit ) )   // but we are not over the collection limit
	 {
	    // create a new sub-collection
	    subCollName = generateNextCollName();
	    pool::CollectionDescription newDestDesc( collection->description() );
	    newDestDesc.setName( subCollName );
	    newDestDesc.setType( subCollType );
	    newDestDesc.setConnection( subCollConnect );

	    subCollection = m_collectionService->create( newDestDesc, true );
	    subCollection->dataEditor().setRowCacheSize( 0 );
	    createdCollections++;
	    rowCounter = 0;
	 }
	 // map to appropriate GUID
	 m_collectionPool->addCollection( guid, subCollection );
	 invCollMap.insert( std::pair<std::string, std::string>(subCollName, guid));
	 rowCounter += thisNumEvents;
	 ++guidIter2;
      }

      std::string lastCollName = "";
      for( std::multimap<std::string, std::string>::const_iterator invCollIter = invCollMap.begin();
	   invCollIter != invCollMap.end();
	   ++invCollIter)
      {
	 std::string thisCollName = invCollIter->first;
	 if (thisCollName == lastCollName)
	    m_log << coral::Info << " " << invCollIter->second;
	 else
	 {
	    if (invCollIter != invCollMap.begin())
	       m_log << coral::Info << coral::MessageStream::endmsg;
	    m_log << coral::Info << "Created new sub-collection " << thisCollName << " with files:\t" << invCollIter->second;
	 }
	 lastCollName = thisCollName;
      }
      m_log << coral::Info << coral::MessageStream::endmsg;
   }
}



void
CollSplitByGUIDBase::copyRows()
{
   for( unsigned int i=0; i<m_srcCollections.size(); i++ )
   {
      ICollection* collection = m_srcCollections[i];
      unique_ptr<ICollectionQuery>  collQuery( collection->newQuery() );
      collQuery->setCondition( m_queryinfo.query() );
      if( m_queryinfo.queryOptions().size() ) {
	 collQuery->addToOutputList( m_queryinfo.queryOptions() );
      } else {
	 collQuery->selectAll();
      }
      pool::ICollectionCursor& cursor = collQuery->execute();

      const std::string srcRefName = collection->description().eventReferenceColumnName();
      const std::string dstRefName = m_collectionPool->getDstRefName();
      // Token name to split on (if not specified, use default for each source coll)
      const std::string splitRefName = ( m_splitRef.size()? m_splitRef : srcRefName );
      size_t row = 0;
      while( cursor.next() ) {
	 row ++;
	 // get file GUID from event
	 const TokenList &tokens = cursor.currentRow().tokenList();
	 string guid = tokens[ splitRefName ].dbID().toString();
	 CollectionRowBuffer &rowBuffer = m_collectionPool->getRowBuffer( guid );
	 // copy all attributes
	 rowBuffer.attributeList() = cursor.currentRow().attributeList();
	 // copy the tokens
	 for( TokenList::const_iterator ti = tokens.begin(); ti != tokens.end(); ++ti ) {
	    if( ti.tokenName() == srcRefName ) {
	       ti->setData( &rowBuffer.tokenList()[ dstRefName ] );
	    } else {
	       ti->setData( &rowBuffer.tokenList()[ ti.tokenName() ] );
	    }
	 }
	 m_collectionPool->insertRow( guid, rowBuffer );
      }
      m_log << coral::Info << "Finished writing all events from input collection " << collection->description().name() << coral::MessageStream::endmsg;
   }
}


// Example of a copy metadata methos
// this one does nothing very useful
void
CollSplitByGUIDBase::copyMetadata()
{
   CollectionPool::flushing_iterator i( m_collectionPool );
   i.forceOpen();  // ensures all accessed collections are open
   while( i.isValid() ) {
      i->metadata().setValueForKey( "CreatedBy", m_thisProgram );
      ++i;
   }
}



void
CollSplitByGUIDBase::finalize()
{
   for( unsigned int i=0; i<m_srcCollections.size(); i++ ) {
      m_srcCollections[i]->close();
   }
   delete m_collectionPool;   // this will close output collections
   m_collectionPool = 0;
}


/// use a different collection pool handler than the default one
void
CollSplitByGUIDBase::setCollectionPool( CollectionPool* cp )
{
   if( !cp ) return;
   if( m_collectionPool )  delete  m_collectionPool;
   m_collectionPool = cp;
}



bool
CollSplitByGUIDBase::readGuidList( const std::string& filename )
{
  ifstream     file( filename.c_str() );
  const int    linelen = 1000;
  char         line[linelen];

  string collectionName;
  while( file.good() ) {
    file.getline(line, linelen);
    char *p = line;
    while( *p == ' ' || *p == '\t' ) p++;
    if( *p == 0 || *p == '#' )
      continue;
    if( *p == ':' ) {
      char *q = ++p;
      while( *q != 0 ) q++;
      while( *(q-1) == ' ' || *(q-1) == '\t' )  q--;
      collectionName = string(p,q);
      if( collectionName.empty() ) {
        // need to generate a default output collection name here, so the GUIDs are properly grouped
        collectionName = generateNextCollName();
      }
      continue;
    }
    char *q = p;
    while( *q != 0 ) q++;
    while( *(q-1) == ' ' || *(q-1) == '\t' )  q--;
    string guid = string(p,q);
    CollNameforGuidMap[ guid ] = collectionName;
  }
  if( file.bad() ) {
    cerr << "Warning: problem reading input file <" << filename
         << "> " << endl;
    file.close();
    return false;
  }

  file.close();
  return true;
}


/*
  string guid:  GUID to look up in the user-specified GUID list
*/
std::string
CollSplitByGUIDBase::collectionNameForGuid( const std::string& guid )
{
  std::map<std::string,std::string>::const_iterator i = CollNameforGuidMap.find( guid );
  if( i != CollNameforGuidMap.end() ) {
    return i->second;
  }
  return "";
}


// generate next output collection name
std::string
CollSplitByGUIDBase::generateNextCollName( )
{
  std::stringstream collectionNameStream;
  collectionNameStream << "sub_collection_" << m_outputCollSeqN++;
  return collectionNameStream.str();
}

