/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TestDriver.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <memory>

#include "PersistentDataModel/Guid.h"
#include "PersistentDataModel/Token.h"

#include "CollectionBase/ICollectionService.h"
#include "CollectionBase/CollectionService.h"
#include "CollectionBase/CollectionDescription.h"
#include "CollectionBase/ICollection.h"
#include "CollectionBase/CollectionRowBuffer.h"
#include "CollectionBase/TokenList.h"
#include "CollectionBase/ICollectionQuery.h"
#include "CollectionBase/ICollectionCursor.h"
#include "CollectionBase/ICollectionColumn.h"
#include "CollectionBase/ICollectionDataEditor.h"
#include "CollectionBase/ICollectionMetadata.h"

#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"

int numTokenInstances();

using namespace std;
using namespace pool;

TestDriver::TestDriver( const std::string& name,
                        const std::string& connection )
      : m_name( name ),  m_connection( connection )
{
}


TestDriver::~TestDriver()
{
  std::cout << "Number of floating tokens : " << numTokenInstances() << std::endl;
}


void
TestDriver::write()
{
   cout << "Creationg CollectionService" << endl;
   auto serviceHandle = std::unique_ptr<pool::CollectionService>(new pool::CollectionService);
   
   cout << "Creating a collection description object" << endl;
   pool::CollectionDescription description( m_name, "RelationalCollection", m_connection, "MainEventRef" );

   cout << "Creating new collection" << endl;
   pool::ICollection* collection = serviceHandle->create( description, true );
  if ( ! collection )   {
    throw std::runtime_error( "Could not create a relational collection object" );
  }

  cout << "Adding columns to the collection schema" << endl;
  // (Event reference Token column was added by default upon collection creation).
  collection->schemaEditor().insertColumn( "RowId", "int" );
  collection->schemaEditor().insertColumn( "Xx", "double", "This is user annotation for XX" );
  collection->schemaEditor().insertColumn( "Yy", "float" );
  collection->schemaEditor().insertColumn( "Empty", "string" );

  collection->schemaEditor().annotateColumn( "Yy", "annotation for YY" );
  collection->schemaEditor().annotateColumn( "MainEventRef", "This is the main Event reference Token" );
  
  cout << "Adding token columns..." << endl;
  collection->schemaEditor().insertTokenColumn( "Ref", "This is a secondary Token attribute" );

  // Get row buffer for adding data to collection.
  pool::CollectionRowBuffer rowBuffer = collection->dataEditor().rowBuffer();

  cout << "Adding rows to collection " << endl;
  for ( unsigned i = 0; i < 10; ++i ) 
  {
    // Add event reference Token to data buffer (this is always the first column in the row).
    Token token;
    token.setTechnology( 1 );
    token.setDb( "myDb" );
    std::ostringstream os;
    os << "Container_" << i / 4;
    token.setCont( os.str() );
    pool::TokenList::iterator iToken = rowBuffer.tokenList().begin();
    int        tok_n = 1;
    while( iToken != rowBuffer.tokenList().end() ) {
       token.oid().first = i%4;
       token.oid().second = 100000*tok_n + i;
       if( iToken.tokenName() == "Ref" )
          token.oid().second += 1u<<31U;
       token.setData( &*iToken );
       cout << "Storing token:" << iToken->toString() << endl;
       tok_n++;  ++iToken;
    }

    // Add Attribute meta data to data buffer.
    coral::AttributeList::iterator iAttribute = rowBuffer.attributeList().begin();
    iAttribute->data<int>() = i;
    ++iAttribute;
    iAttribute->data< double >() = i + 0.11;
    ++iAttribute;
    iAttribute->data< float >() = float(i + 0.22);

    // Add new row to collection.
    collection->dataEditor().insertRow( rowBuffer );
  }

  
  // Metadata test 
  ICollectionMetadata& metadata( collection->metadata() );
  metadata.setValueForKey("Author", "This collection has been created by WriteRead test from RelationalCollection package");
  
  const char *author = metadata.getValueForKey("Author");
  if( !author ) {
     throw std::runtime_error("The collection has no Author key in the metadata");
  } else {
     cout << "Collection author metadata inserted: " << author << endl;
  }

  metadata.setValueForKey("random1", "random metadata 1");
  metadata.setValueForKey("random5", "random metadata 5");
  metadata.setValueForKey("random1", "overwritten random metadata 1");
  
   
  cout << "Committing..." << endl;
  collection->commit();

  // Close collection.
  collection->close();
  delete collection;
}


void
TestDriver::read()
{
   cout << " Retrieve handle to loaded collection service" << endl;
   auto_ptr<pool::CollectionService> serviceHandle( new CollectionService );

   cout << "Getting handle to existing collection ( opened for read-only transactions by default )" << endl;
   pool::ICollection* collection = serviceHandle->handle( m_name, "RelationalCollection" , m_connection );

  if( ! collection )   {
    throw std::runtime_error( "Could not create a relational collection object" );
  }

  cout << "Executing query that only selects event reference column (the default)" << endl;
  pool::ICollectionQuery* query1 = collection->newQuery();
  query1->setCondition( "Xx * Yy < 20 AND Xx > 2" );
  pool::ICollectionCursor& cursor1 = query1->execute();
  int counter = 0;
  while ( cursor1.next() )
  {
//MN:    std::cout << "Token : " << cursor1.currentRow().eventRef().toString() << std::endl;
     std::cout << "Token : " << cursor1.eventRef().toString() << std::endl;
     coral::AttributeList attributeList = cursor1.currentRow().attributeList();
     std::cout << "Meta data : ";
     for( coral::AttributeList::const_iterator iAttribute = attributeList.begin();
           iAttribute != attributeList.end(); ++iAttribute )
     {
        if ( iAttribute != attributeList.begin() ) std::cout << ", ";
        std::cout << "[";
        iAttribute->toOutputStream( std::cout );
        std::cout << "]";
     }
     cout << endl;
     counter++;
  }
  std::cout << counter << " records read back" << std::endl;
  delete query1;

  // Execute query that selects 2 meta data columns.
  pool::ICollectionQuery* query2 = collection->newQuery();
  query2->setCondition( "Xx * Yy < 20 AND Xx > 2" );
  query2->addToOutputList( "Xx" );
  query2->addToOutputList( "Yy" );
  pool::ICollectionCursor& cursor2 = query2->execute();
  counter = 0;
  while ( cursor2.next() )
  {
     //MN: std::cout << "Token : " << cursor2.currentRow().eventRef().toString() << std::endl;
    std::cout << "Token : " << cursor2.eventRef().toString() << std::endl;
    coral::AttributeList attributeList = cursor2.currentRow().attributeList();
    std::cout << "Meta data : ";
    for ( coral::AttributeList::const_iterator iAttribute = attributeList.begin();
          iAttribute != attributeList.end(); ++iAttribute )
    {
      if ( iAttribute != attributeList.begin() ) std::cout << ", ";
      std::cout << "[";
      iAttribute->toOutputStream( std::cout );
      std::cout << "]";
    }
    cout << endl;
    counter++;
  }
  std::cout << counter << " records read back" << std::endl;
  delete query2;

  // Execute query that selects all meta data.
  pool::ICollectionQuery* query3 = collection->newQuery();
  query3->setCondition( "Xx * Yy < 20 AND Xx > 2" );
  query3->selectAll();
  pool::ICollectionCursor& cursor3 = query3->execute();
  counter = 0;
  while( cursor3.next() )  {
     //MN:  std::cout << "Token : " << cursor3.currentRow().eventRef().toString() << std::endl;
    std::cout << "Token : " << cursor3.eventRef().toString() << std::endl;
    coral::AttributeList attributeList = cursor3.currentRow().attributeList();
    std::cout << "Meta data : ";
    for ( coral::AttributeList::const_iterator iAttribute = attributeList.begin();
          iAttribute != attributeList.end(); ++iAttribute )
    {
      if ( iAttribute != attributeList.begin() ) std::cout << ", ";
      std::cout << "[";
      iAttribute->toOutputStream( std::cout );
      std::cout << "]";
    }
    const pool::TokenList tokens = cursor3.currentRow().tokenList();
    std::cout << endl;
    cout << "Token 2: " << tokens["Ref"].toString() << std::endl;
   
    cout << "-------- " << endl;
    
    counter++;
  }
  std::cout << counter << " records read back" << std::endl;
  delete query3;


  // Metadata test
  const char *author = collection->metadata().getValueForKey("Author");
  if( !author ) {
     cout << "The collection has no Author key in the metadata" << endl;
  } else {
     cout << "Collection author metadata: " << author << endl;
  }

  std::cout << endl << "Dumping all metadata" << std::endl;
  ICollectionMetadata::const_iterator  mdIter
     = collection->metadata().begin();
  while( mdIter !=  collection->metadata().end() ) {
     cout << "  Metadata Key=" << mdIter.key();
     cout << ",  Value=" << mdIter.value() << endl;
     ++mdIter;
  }
  
  // Close collection.
  collection->close();
  delete collection;
}
