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
#include "CollectionBase/ICollectionColumn.h"
#include "CollectionBase/ICollectionQuery.h"
#include "CollectionBase/ICollectionCursor.h"
#include "CollectionBase/ICollectionDataEditor.h"
#include "CollectionBase/ICollectionMetadata.h"

#include "PersistentDataModel/Token.h"
#include <stdexcept>
#include <cstdlib>
#include <memory>
#include "CoralBase/Attribute.h"

#include "TInterpreter.h"
#include "TClass.h"

using namespace std;
using namespace pool;


/*
void write() {
   
   string connection( "" );
   try{
      cout << "Openning collection 'test_collection' in CREATE_AND_OVERWRITE mode." << endl;
      RootCollection collection( 0, connection, "test_collection", ICollection::CREATE_AND_OVERWRITE );
      cout << "Openning the collection." << endl;
    
      cout << "Specifying attribute list: 'attr1' of type 'int', 'attr2' of type 'std::string' "
           << "and attr3 of type 'double'." << endl;
      AttributeListSpecification& spec = collection.attributeListSpecification();
      spec.push_back( "attr1", "int" );
      spec.push_back( "attr2", "string" );
      spec.push_back( "attr3", "double" );
      spec.push_back( "attr4", "bool" );
      spec.push_back( "attr 64bit", "unsigned long long" );
    
      cout << "Adding 30 elements to the collection." << endl;
      for( int i=100; i<130; i++ )
      {
         AttributeList attrList( spec );
         attrList[ "attr1" ].setValue( i );
         char s[20];
         ::sprintf( s, "%s_%d", "testString", i );
         attrList[ "attr2" ].setValue( string( s ) );
         attrList[ "attr3" ].setValue( double( i ) / 3 );
         attrList[ "attr4" ].setValue( bool(i%3) );
         ::sprintf( s, "%s_%d","testToken", i );
         Token* t = new Token();
         t->fromString( string( "" ) );
         t->setDb( s );
         collection.add( *t, attrList );

         cout << "Token: " << t->toString();
         cout << endl;
         cout << "Attributes: ";
         printAttributes( attrList );
         cout << endl;
         t->release();
      }
      cout << "Done." << endl;
      cout << "Commiting the collection." << endl;
      collection.commit();
      cout << "Closing the collection." << endl;
      collection.close();
    
      cout << "[OVAL] finished hopefully" << endl;
    
   } catch ( seal::Exception sealException )
   {
      cout << "SEAL EXCEPTION: " << sealException.message() << endl;
   } catch ( ... )
   {
      cout << "Unknown exception" << endl;
   }
}



void read()
{
   string connection( "" );
   try
   {
      cout << "Openning collection 'test_collection' in READ mode." << endl;
      RootCollection collection( 0, connection, "test_collection", ICollection::READ );

      cout << "Creating Iterator with empty query." << endl;
      ICollectionIterator* iterator = collection.select( "", "", "" );
      cout << "Contents of the collection: " << endl;
      while( iterator->next() )
      {
         cout << "Token: " << iterator->token()->toString();
         cout << endl;
         cout << "Attributes: ";
         printAttributes( iterator->attributeList() );
         cout << endl;
      }
      cout << "Done." << endl;
      cout << "Closing the collection." << endl;
      collection.close();

      cout << endl << endl;
      cout << "Openning the collection." << endl;
      collection.open();
    
      std::string query = "attr1 > 117 && attr1 <123 && strstr(attr2,\"String_11\")";
      cout << "Creating Iterator with query: '" << query << "'." << endl;
      iterator = collection.select(query, "", "" );
      cout << "Contents of the collection: " << endl;
      while( iterator->next() ) {
         cout << "[OVAL] ";
         cout << "Token: " << iterator->token()->toString();
         cout << endl;
         cout << "[OVAL] ";
         cout << "Attributes: ";
         printAttributes( iterator->attributeList() );
         cout << endl;
      }
      cout << "Done." << endl;
      cout << "Closing the collection." << endl;
      collection.close();
    
      cout << "[OVAL] finished successfully" << endl;
  
   } catch ( seal::Exception sealException )
   {
      cout << "SEAL EXCEPTION: " << sealException.message() << endl;
   } catch ( ... )
   {
      cout << "Unknown exception" << endl;
   }

}

*/


class TestDriver {
public:
  TestDriver( const std::string& name,
              const std::string& connection );

  ~TestDriver() {}

  void write();

  void read();


private:
  std::string m_name;
  std::string m_connection;
};



TestDriver::TestDriver( const std::string& name,
                        const std::string& connection )
      : m_name( name ),  m_connection( connection )
{
   // Load message service.
   //   pool::POOLContext::loadComponent( "SEAL/Services/MessageService" );
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
   description.insertColumn( "attr1", "int", "integer test attribute" );
   description.insertColumn( "attr2", "string", "some string" );
   description.insertColumn( "attr3", "double", "floating point attribute" );
   description.insertColumn( "attr4", "bool" );
   description.insertColumn( "attr64bit", "unsigned long long" );

   cout << "Creating Collection" << endl;
   pool::ICollection* collection = serviceHandle->create( description, true );
   if ( ! collection )   {
      throw std::runtime_error( "Could not create a relational collection object" );
   }

   cout << "Renaming primary Token column" << endl;
   collection->schemaEditor().renameColumn( "Token", "RenamedToken" );
   collection->schemaEditor().setEventReferenceColumnName( "RenamedEvRef" );
   cout << "Primary Token column is now " << collection->description().eventReferenceColumnName() << endl;
   
   cout << "Adding 30 elements to the collection." << endl;
   pool::CollectionRowBuffer rowBuffer = collection->dataEditor().rowBuffer();

   unsigned long long ntab[] = {
      0x0000000000000000ULL,
      0x0000000000000001ULL,
      0x0000000000010000ULL,
      0x0000000000010001ULL,
      0x7000000000010001ULL,

      0x8000000000000000ULL,
      0x8000000000000001ULL,
      0x8000000000010000ULL,
      0x8000000000010001ULL,
      0xF000000000010001ULL
   };

      
   for( int i=0; i<10; i++ )   {
      rowBuffer.attributeList()[ "attr1" ].data<int>() = i ;
      char s[20];
      ::sprintf( s, "%s_%d", "testString", i );
      rowBuffer.attributeList()[ "attr2" ].data<std::string>() = string( s );
      rowBuffer.attributeList()[ "attr3" ].data<double>() = double( i ) * (1./3);
      rowBuffer.attributeList()[ "attr4" ].data<bool>() = ((i%3)!=0);

      rowBuffer.attributeList()[ "attr64bit" ].data<unsigned long long>() = ntab[i];
      cout << ">>  writing uint64: " << ntab[i] << "  " << (double)ntab[i] << "   " << (unsigned long long)(double)ntab[i] << endl;
      collection->dataEditor().insertRow( rowBuffer );
   }

   ICollectionMetadata& metadata( collection->metadata() );
   metadata.setValueForKey("Author", "This collection has been created by readTest from RootCollection package");

   const char *author = metadata.getValueForKey("Author");
   if( !author ) {
      throw std::runtime_error("The collection has no Author key in the metadata");
   } else {
      cout << "Collection author metadata inserted: " << author << endl;
   }

   metadata.setValueForKey("random1", "random metadata 1");
   metadata.setValueForKey("random5", "random metadata 5");
   metadata.setValueForKey("random1", "overwritten random metadata 1");
   metadata.setValueForKey("empty", "");
   metadata.setValueForKey("last", "xxx");
      
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
   cout << "Creating CollectionService" << endl;
   unique_ptr<CollectionService> serviceHandle(new CollectionService());

  cout << "Getting handle to existing collection ( opened for read-only transactions by default )" << endl;
  pool::ICollection* collection = serviceHandle->handle( m_name, "RootCollection" , m_connection );

  if( ! collection )   {
     throw std::runtime_error( "Could not create a rootCollection object" );
  }

  cout << "Executing query that only selects event reference column (the default)" << endl;
  pool::ICollectionQuery* query1 = collection->newQuery();
//  query1->setCondition( "Xx * Yy < 20 AND Xx > 2" );
  pool::ICollectionCursor& cursor1 = query1->execute();
  cout << "Iterating over query results..." << endl;
  int counter = 0;
  while( cursor1.next() && counter < 10 )  {
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

  
  cout << "Executing query that selects 2 meta data columns." << endl;
  pool::ICollectionQuery* query2 = collection->newQuery();
  query2->setCondition( "attr1 > 115" );
  query2->addToOutputList( "attr1,attr2" );
  pool::ICollectionCursor& cursor2 = query2->execute();
  counter = 0;
  while ( cursor2.next() && counter < 100 )
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

  cout << endl << "Executing query on 64bit unsigned int attribute:" << endl;
  pool::ICollectionQuery* query3 = collection->newQuery();
  query3->setCondition( "" );
  query3->addToOutputList( "attr64bit" );
  pool::ICollectionCursor& cursor3 = query3->execute();
  counter = 0;
  while ( cursor3.next() && counter < 100 )
  {
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
    cout << endl;
    counter++;
  }
  std::cout << counter << " records read back" << std::endl << endl;
  delete query3;


  /*
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
  */

  const char *author = collection->metadata().getValueForKey("Author");
  //std::string crashtest = collection->metadata().getValueForKey("afwefagfadrg");
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
     if (mdIter.key() == "POOLCollectionID") {
       // The contents of this line will vary from run to run.
       // Add this to prevent post.sh from comparing it.
       cout << " [0x00000000]";
     }
     cout << ",  Value=" << mdIter.value() << endl;
     ++mdIter;
  }
  
  // Close collection.
  collection->close();
  delete collection;
}





int main()
{
  gInterpreter->EnableAutoLoading();
  TClass::GetClass("map<string,string>");

   (void)remove("test_collection.root");
   try {
      std::cout << "Read test starting..." << std::endl;
      TestDriver driver( "test_collection", "" );
      driver.write();
      driver.read();

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

