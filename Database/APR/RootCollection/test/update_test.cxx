/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cstdio> // For sprintf on gcc45
#include <string>
#include <iostream>

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

#include "PersistentDataModel/Token.h"

#include <stdexcept>
#include <cstdlib>
#include <memory>
#include "CoralBase/Attribute.h"

#include "TInterpreter.h"
#include "TClass.h"

using namespace std;
using namespace pool;



class TestDriver {
public:
  TestDriver( const std::string& name,
              const std::string& connection );

  ~TestDriver() {}

  void write();
  void read();
  void update();
  void query();


private:
  std::string m_name;
  std::string m_connection;
};




TestDriver::TestDriver( const std::string& name,
                        const std::string& connection )
      : m_name( name ),  m_connection( connection )
{
   // Load message service.
   if( !getenv("POOL_OUTMSG_LEVEL") ) {
     //      // pool::POOLContext::setMessageVerbosityLevel( coral::Info );
   }
}



void
TestDriver::write()
{
   cout << "Creating CollectionService" << endl;
   unique_ptr<CollectionService> serviceHandle(new CollectionService());
   cout << "Creating Collection Description" << endl;
   pool::CollectionDescription description( m_name, "RootCollection", m_connection );
   description.insertColumn( "attr1", "int" );
   description.insertColumn( "attr2", "string" );
   description.insertColumn( "attr3", "double" );
   description.insertColumn( "attr4", "bool" );
   description.insertColumn( "64bitint", "unsigned long long" );
   description.insertColumn( "L2PassedTrigMask0", "unsigned long long" );

   cout << "Creating Collection" << endl;
   pool::ICollection* collection = serviceHandle->create( description, true );
   if ( ! collection )   {
      throw std::runtime_error( "Could not create a relational collection object" );
   }

   cout << "Adding 20 elements to the collection." << endl;
   pool::CollectionRowBuffer rowBuffer = collection->dataEditor().rowBuffer();

   for( int i=100; i<105; i++ )   {
      rowBuffer.attributeList()[ "attr1" ].data<int>() = i ;
      char s[20];
      ::sprintf( s, "%s_%d", "testString", i );
      rowBuffer.attributeList()[ "attr2" ].data<std::string>() = string( s );
      rowBuffer.attributeList()[ "attr3" ].data<double>() = double( i ) * (1./3);
      rowBuffer.attributeList()[ "64bitint" ].data<unsigned long long>() = 0x200000000ULL * (i==102 || i==104);
      rowBuffer.attributeList()[ "attr4" ].data<bool>() = ((i%3)!=0);

      ::sprintf( s, "%s_%d","testToken", i );
      Token* t = new Token();
      t->fromString( string( "" ) );
      t->setDb( s );

      t->setData( &*rowBuffer.tokenList().begin() );

      collection->dataEditor().insertRow( rowBuffer );
      t->release();
   }

   cout << "Done." << endl;
   cout << "Commiting the collection." << endl;
   collection->commit();
   cout << "Closing the collection." << endl;
   collection->close();
   delete collection;
   
   cout << "[OVAL] finished hopefully" << endl;
}

   

void
TestDriver::read()
{
   cout << "Creationg CollectionService" << endl;
   unique_ptr<CollectionService> serviceHandle(new CollectionService());
   
   cout << "Getting handle to existing collection for reading" << endl;
   bool        readOnly(true);
   pool::ICollection* collection = serviceHandle->handle( m_name, "RootCollection" , m_connection, readOnly );

   if( ! collection )   {
      throw std::runtime_error( "Could not create the RootCollection object" );
   }

   cout << "Executing query that only selects event reference column (the default)" << endl;
   pool::ICollectionQuery* query1 = collection->newQuery();
   pool::ICollectionCursor& cursor1 = query1->execute();
   cout << "Iterating over query results..." << endl;
   int counter = 0;
   while( cursor1.next() )  {
      std::cout << "Token : " << cursor1.eventRef().toString() << std::endl;
      counter++;
   }
   std::cout << counter << " records read" << std::endl;
   delete query1;

   collection->close();
   delete collection;
}


void
TestDriver::update()
{
   cout << "Creationg CollectionService" << endl;
   unique_ptr<CollectionService> serviceHandle(new CollectionService());
   
   cout << "Getting handle to existing collection for update" << endl;
   bool        readOnly(false);
   pool::ICollection *collection = serviceHandle->handle( m_name, "RootCollection" , m_connection, readOnly);
   if( ! collection )   {
      throw std::runtime_error( "Could not open RootCollection for update" );
   }   

   cout << "Adding new 10 elements to the collection." << endl;
   pool::CollectionRowBuffer rowBuffer = collection->dataEditor().rowBuffer();
   
   for( int i=120; i<130; i++ )   {
     rowBuffer.attributeList()[ "attr1" ].data<int>() = i ;
      char s[20];
      ::sprintf( s, "%s_%d", "testString", i );
      rowBuffer.attributeList()[ "attr2" ].data<std::string>() = string( s );
      rowBuffer.attributeList()[ "attr3" ].data<double>() = double( i ) * (1./3);
      rowBuffer.attributeList()[ "64bitint" ].data<unsigned long long>() = 0x200000000ULL * (i==122 || i==124);
      rowBuffer.attributeList()[ "attr4" ].data<bool>() = ((i%3)!=0);

      ::sprintf( s, "%s_%d","testToken", i );
      Token* t = new Token();
      t->fromString( string( "" ) );
      t->setDb( s );

      t->setData( &*rowBuffer.tokenList().begin() );

      collection->dataEditor().insertRow( rowBuffer );
      t->release();
   }

    cout << "Done." << endl;
    cout << "Commiting the collection." << endl;
    collection->commit();
    cout << "Closing the collection." << endl;
    collection->close();
    delete collection;
}
    

void
TestDriver::query() {
   
    cout << endl << endl;
   cout << "Creationg CollectionService" << endl;
   unique_ptr<CollectionService> serviceHandle(new CollectionService());
   
   cout << "Getting handle to existing collection for reading" << endl;
   bool        readOnly(true);
   pool::ICollection* collection = serviceHandle->handle( m_name, "RootCollection" , m_connection, readOnly );
   if( ! collection )   {
      throw std::runtime_error( "Could not create the RootCollection object" );
   }

    std::string querystr = "attr1 > 111 && attr1 <127 && strstr(attr2,\"5\")";
    cout << "Creating query: '" << querystr << "'." << endl;
    pool::ICollectionQuery* query2 = collection->newQuery();
    query2->setCondition( querystr );
    query2->addToOutputList( "attr1,attr2,attr3" );

    pool::ICollectionCursor& cursor2 = query2->execute();
    int counter = 0;
    while ( cursor2.next() && counter < 100 )
    {
       cout << "[OVAL] ";
       std::cout << "Token : " << cursor2.eventRef().toString() << std::endl;
       coral::AttributeList attributeList = cursor2.currentRow().attributeList();
       cout << "[OVAL] ";
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

   cout << "Done." << endl;
   cout << "Closing the collection." << endl;
   collection->close();

   cout << "[OVAL] finished successfully" << endl; 
   delete collection;
}





int main( int argc, char ** )
{
  gInterpreter->EnableAutoLoading();
  TClass::GetClass("map<string,string>");

   try {
      std::cout << "Update test starting..." << std::endl;
      TestDriver driver( "test_collection", "" );

      if( argc < 2 ) {
         driver.write();
         driver.read();
      } else {
         std::cout << " argument present, skipping creation of the collection" << std::endl;
      }
      driver.update();
      driver.update();
      driver.update();
      driver.query();      
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

