/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TestDriver.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <memory>

#include "PersistentDataModel/Guid.h"
#include "PersistentDataModel/Token.h"

#include "CollectionBase/CollectionService.h"
#include "CollectionBase/CollectionDescription.h"
#include "CollectionBase/ICollection.h"
#include "CollectionBase/CollectionRowBuffer.h"
#include "CollectionBase/TokenList.h"
#include "CollectionBase/ICollectionQuery.h"
#include "CollectionBase/ICollectionCursor.h"
#include "CollectionBase/ICollectionColumn.h"
#include "CollectionBase/ICollectionDataEditor.h"

#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"

#include <vector>
using namespace std;

int numTokenInstances();

TestDriver::TestDriver( const std::string& name,
                        const std::string& connection )
      :m_name( name ),  m_connection( connection )
{
   // pool::POOLContext::loadComponent( "POOL/Services/EnvironmentAuthenticationService" );
}


TestDriver::~TestDriver()
{
  std::cout << "Number of floating tokens : " << numTokenInstances() << std::endl;
}



void
TestDriver::write()
{
   cout << "Creationg CollectionService" << endl;
   unique_ptr<pool::CollectionService> serviceHandle( new pool::CollectionService() );
   
   cout << "Creating a collection description object" << endl;
   
  // Create a collection description object.
  pool::CollectionDescription description( m_name, "RelationalCollection", m_connection );

  // Create new collection.
  cout << "Creating new collection" << endl;
  pool::ICollection* collection = serviceHandle->create( description, true );

  if ( ! collection ) 
  {
    throw std::runtime_error( "Could not create a relational collection object" );
  }

  // Add columns to the collection schema ( Event reference Token column was added by default upon collection creation ).
  cout << "Adding columns to the collection schema" << endl;
  collection->schemaEditor().insertColumn( "RowId", "int" );
  collection->schemaEditor().insertColumn( "Xx", "double" );
  collection->schemaEditor().insertColumn( "Yy", "float" );

  collection->schemaEditor().insertTokenColumn( "Ref" );

  // Get row buffer for adding data to collection.
  pool::CollectionRowBuffer rowBuffer = collection->dataEditor().rowBuffer();

  // Add rows to collection.
  for ( unsigned long i = 0; i < 10; ++i ) 
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
       token.oid().second = 1000*tok_n + i;
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

  // Commit the new rows.
  collection->commit();

  // Close collection.
  collection->close();
}


void
TestDriver::update()
{
   unique_ptr<pool::CollectionService> serviceHandle( new pool::CollectionService );

  // Create a collection description object.
  cout << "Getting handle to existing collection" << endl;
  pool::ICollection* collection = serviceHandle->handle( m_name, "RelationalCollection" , m_connection, false );

  if( !collection )   {
    throw std::runtime_error( "Could not create a relational collection object" );
  }

  // Get row buffer for adding data to collection.
  pool::CollectionRowBuffer rowBuffer = collection->dataEditor().rowBuffer();

  // Add rows to collection.
  for ( unsigned long i = 0; i < 10; ++i ) 
  {
    // Add event reference Token to data buffer (this is always the first column in the row).
    Token token;
    token.setTechnology( 1 );
    token.setDb( "myDb" );
    std::ostringstream os;
    os << "Container_" << i / 4;
    token.setCont( os.str() );
    token.oid().first = i%4;
    token.oid().second = 1000 + i;
    pool::TokenList::iterator iToken = rowBuffer.tokenList().begin();
    token.setData( &*iToken );
    cout << "Storing token:" << iToken->toString() << endl;

    // Add Attribute meta data to data buffer.
    coral::AttributeList::iterator iAttribute = rowBuffer.attributeList().begin();
    iAttribute->data<int>() = i;
    ++iAttribute;
    iAttribute->data< double >() = i + 0.11;
    ++iAttribute;
    iAttribute->data< float >() = float(i + 0.22);

    cout << " > Add new row to collection" << endl;
    for( iAttribute = rowBuffer.attributeList().begin(); iAttribute != rowBuffer.attributeList().end(); ++iAttribute ) {
       if( iAttribute != rowBuffer.attributeList().begin() ) std::cout << ", ";        
       std::cout << "[";
       iAttribute->toOutputStream( std::cout );
       std::cout << "]";
    }
    cout << endl;
    
    collection->dataEditor().insertRow( rowBuffer ); 
  }

  // Commit the new rows.
  collection->commit();

  // Close collection.
  collection->close();
}
