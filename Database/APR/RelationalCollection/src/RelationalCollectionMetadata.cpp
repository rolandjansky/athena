/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "RelationalCollectionMetadata.h"
#include "RelationalCollectionNames.h"
#include "RelationalCollectionBindVariables.h"

#include "POOLCore/Exception.h"

#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/ISessionProperties.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ITableDataEditor.h"
#include "RelationalAccess/ITableSchemaEditor.h"
#include "RelationalAccess/ITablePrivilegeManager.h"
#include "RelationalAccess/TableDescription.h"
#include "CoralBase/AttributeSpecification.h"
#include "CoralBase/Attribute.h"

#include "CoralBase/MessageStream.h"

#include <memory>
#include <iostream>
using namespace std;


namespace pool {
   namespace RelationalCollection { 

      RelationalCollectionMetadata::RelationalCollectionMetadata()
        : m_mode(ICollection::READ), m_isOracle(true), m_metadataTable(0)
      {
      }

      
      RelationalCollectionMetadata::~RelationalCollectionMetadata()
      { }
        
      void
      RelationalCollectionMetadata::initialize( const std::string& collectionName,
                                                coral::ISessionProxy& session,
                                                pool::ICollection::OpenMode mode )
      {
         m_mode = mode;
         m_collectionName = collectionName;
	 m_isOracle = ( session.properties().flavorName() == "Oracle" );
         coral::ISchema& nominalSchema = session.nominalSchema();
         bool metadataTableExists = nominalSchema.existsTable( RelationalCollectionNames::metadataTable() );
         
         if( !metadataTableExists ) {
            coral::MessageStream log( "pool::RelationalCollectionMetadata::init()" );
            log << coral::Warning
                << " ****** no metadata table found" << coral::MessageStream::endmsg;
            
            if( mode == ICollection::READ ) {
               m_metadataTable = 0;
               return;
            } else {
               // create the metadata table 
               coral::TableDescription description( RelationalCollectionNames::collectionTypeName() );
               description.setName( RelationalCollectionNames::metadataTable() );
               description.insertColumn( RelationalCollectionNames::collectionNameColumn(),
                                         coral::AttributeSpecification::typeNameForType<std::string>(),
					 2000, false);
               description.insertColumn( RelationalCollectionNames::metadataKeyColumn(),
                                         coral::AttributeSpecification::typeNameForType<std::string>() );
               description.insertColumn( RelationalCollectionNames::metadataValueColumn(),
                                         coral::AttributeSpecification::typeNameForType<std::string>() );
               
               std::vector<std::string> primaryKeyColumns;
               primaryKeyColumns.push_back( RelationalCollectionNames::collectionNameColumn() );
               primaryKeyColumns.push_back( RelationalCollectionNames::metadataKeyColumn() );
               description.setPrimaryKey( primaryKeyColumns );               
               
               nominalSchema.createTable( description ); //.privilegeManager().grantToPublic( coral::ITablePrivilegeManager::Select );
            
               log << coral::Info
                   << " ****** created the metadata table" << coral::MessageStream::endmsg;
            }
         }
         
         m_metadataTable = &nominalSchema.tableHandle( RelationalCollectionNames::metadataTable() );
         
         // Define bind variables value holder for querying metadata table 
         m_whereDataForMetadata.extend<std::string>( RelationalCollectionNames::collectionNameColumn() );
         m_whereDataForMetadata[0].data<std::string>() = m_collectionName;

         m_whereDataForMetadataKey.extend<std::string>( RelationalCollectionNames::collectionNameColumn() );
         m_whereDataForMetadataKey[0].data<std::string>() = m_collectionName;
         m_whereDataForMetadataKey.extend<std::string>( RelationalCollectionNames::metadataKeyColumn() );
      }

        
      void
      RelationalCollectionMetadata::reinitialize( coral::ISessionProxy* session )
      {
	 if( m_metadataTable )
	    m_metadataTable = & session->nominalSchema().tableHandle( RelationalCollectionNames::metadataTable() );
      }
      
	 
      unsigned long long
      RelationalCollectionMetadata::entries() const
      {
         if( !m_metadataTable )
            return 0; 
	 std::unique_ptr<coral::IQuery> query( m_metadataTable->newQuery() );
         query->setCondition( RelationalCollectionBindVariables::whereClauseForMetadata(), m_whereDataForMetadata );
	 query->limitReturnedRows( 1, 0 );
	 query->addToOutputList( "count(*)");
	 query->defineOutputType( "count(*)", "unsigned long long");
         coral::ICursor& cursor = query->execute();
         if( !cursor.next() )
            return 0;
         return cursor.currentRow()[0].data<unsigned long long>();
      }
	 


      bool
      RelationalCollectionMetadata::existsKey( const std::string& key ) const
      {
         if( !m_metadataTable )
            return false;
	 map<string,bool>::iterator  ki= m_keyInfo.find( key );
	 if( ki != m_keyInfo.end() )
	    return ki->second;
         std::unique_ptr<coral::IQuery> query( m_metadataTable->newQuery() );
         m_whereDataForMetadataKey[1].data<std::string>() = key;
	 query->addToOutputList( RelationalCollectionNames::collectionNameColumn() );	 
         query->setCondition( RelationalCollectionBindVariables::whereClauseForMetadataKey(), m_whereDataForMetadataKey );
         query->limitReturnedRows( 1, 0 );
         coral::ICursor& cursor = query->execute();
         bool exists = cursor.next();
	 m_keyInfo[ key ] = exists;
	 return exists;
      }

      
      const char*
      RelationalCollectionMetadata::getValueForKey( const std::string& key ) const
      {
         if( !m_metadataTable )
            return 0; 
         std::unique_ptr<coral::IQuery> query( m_metadataTable->newQuery() );
         m_whereDataForMetadataKey[1].data<std::string>() = key;
         query->setCondition( RelationalCollectionBindVariables::whereClauseForMetadataKey(), m_whereDataForMetadataKey );
         query->limitReturnedRows( 1, 0 );
         coral::ICursor& cursor = query->execute();
         bool exists = cursor.next();
	 m_keyInfo[ key ] = exists;
	 if( !exists ) return 0;
	 // cache the query result, as I am afraid it can go away when the query is deleted
	 m_lastValue = cursor.currentRow()[2].data<std::string>();
         return m_lastValue.c_str();
      }

        
      void
      RelationalCollectionMetadata::setValueForKey( const std::string& key,
                                                    const std::string& value )
      {
         if( m_mode == ICollection::READ ) {
            throw pool::Exception( "Metadata update not allowed in Read-only mode",
                                   "RelationalCollectionMetadata::setValueForKey", 
                                   "RelationalCollection");
         }
         if( key.empty() ) {
            throw pool::Exception( "Attempt to set metadata with an empty Key",
                                   "RelationalCollectionMetadata::setValueForKey", 
                                   "RelationalCollection");
         } 
         coral::MessageStream log( "pool::RelationalCollectionMetadata::setValueForKey()" );

         if( existsKey(key) ) {
            //update
            std::string setClause = RelationalCollectionNames::metadataValueColumn() + " = '" + value + "'";
            m_whereDataForMetadataKey[1].data<std::string>() = key;
            m_metadataTable->dataEditor().updateRows(
               setClause,
               RelationalCollectionBindVariables::whereClauseForMetadataKey(),
               m_whereDataForMetadataKey );
            log << coral::Debug
                << " Updated metadata entry: (" << key <<", " << value << ") " << coral::MessageStream::endmsg;
         } else {
            // instert
            coral::AttributeList rowBuffer;
            rowBuffer.extend<std::string>( RelationalCollectionNames::collectionNameColumn() );
            rowBuffer.extend<std::string>( RelationalCollectionNames::metadataKeyColumn() );
            rowBuffer.extend<std::string>( RelationalCollectionNames::metadataValueColumn() );
            coral::AttributeList::iterator iAttribute = rowBuffer.begin();
            iAttribute->data< std::string >() = m_collectionName;
            ++iAttribute;
            iAttribute->data< std::string >() = key;
            ++iAttribute;
            iAttribute->data< std::string >() = value;
            m_metadataTable->dataEditor().insertRow( rowBuffer );       
            log << coral::Debug
                << " Inserted new metadata entry: (" << key <<", " << value << ") " << coral::MessageStream::endmsg;
         }
	 m_keyInfo[ key ] = true; 
      }

     
      int
      RelationalCollectionMetadata::deleteCollectionMetadata()
      {
         if( m_mode == ICollection::READ ) {
            throw pool::Exception( "Metadata deletion not possible in Read-only mode",
                                   "RelationalCollectionMetadata::deleteCollectionMetadata", 
                                   "RelationalCollection");
         }
         if( !m_metadataTable )
            return 0;
         return m_metadataTable->dataEditor().deleteRows( RelationalCollectionBindVariables::whereClauseForMetadata(), m_whereDataForMetadata );
      }





      // -------  RelationalCollectionMetadataIterator

      RelationalCollectionMetadata::RelationalCollectionMetadataIterator::
      RelationalCollectionMetadataIterator( const RelationalCollectionMetadata* meta,
					    coral::IQuery *query,
					    std::map<std::string,bool> *keymap )
            : m_metadata( meta ),
	      m_query( query ),
              m_cursor( 0 ),
	      m_keyInfo( keymap ),
              m_valid( false )
      {
         if( query ) {
            m_cursor = &query->execute();
            ++*this;
         }
      }

 
      RelationalCollectionMetadata::RelationalCollectionMetadataIterator::
      ~RelationalCollectionMetadataIterator( )
      {
         delete m_query;
      }
      
     
      bool
      RelationalCollectionMetadata::RelationalCollectionMetadataIterator::
       operator==( const ICollectionMetadataIterator& _rhs ) const
      {
         const RelationalCollectionMetadataIterator* rhs
            = dynamic_cast<const RelationalCollectionMetadataIterator*>( &_rhs );
         if( !rhs )
            throw pool::Exception( "Incompatible iterator comparison",           
                                   "RelationalCollectionMetadataItorator::operator==", 
                                   "RelationalCollection");
         return key() == rhs->key();
      }


      void
      RelationalCollectionMetadata::RelationalCollectionMetadataIterator::
      operator++()
      {
         if( m_cursor && m_cursor->next() ) {
            m_valid = true;
	    if( m_keyInfo )  (*m_keyInfo)[ key() ] = true;
         } else {
            m_valid = false;
         }
      }


      const std::string&
      RelationalCollectionMetadata::RelationalCollectionMetadataIterator::
      key() const
      {
         static std::string        emptyKey;
         return ( m_valid? m_cursor->currentRow()["key"].data<std::string>() : emptyKey );
      }

      
      const char *
      RelationalCollectionMetadata::RelationalCollectionMetadataIterator::
      value() const
      {
	 if( !m_valid ) return 0;
	 const coral::Attribute &attr = m_cursor->currentRow()["value"];
	 const string& value = attr.data<std::string>();
	 if( !m_metadata->m_isOracle || value.length() != 4000 ) {
            // cppcheck-suppress stlcstr
	    return value.c_str();
         }

	 // most likely a truncated CLOB entry - read the full value
	 const char * val = m_metadata->getValueForKey( key() );
	 if( !val ) {
	    throw pool::Exception( string("Failed to retrieve the full value for large metadata entry: ") + key(),
				   "RelationalCollectionMetadataIterator::value()", 
				   "RelationalCollection");
	 }
	 const_cast<coral::Attribute&>(attr).setValue( string(val) );  //cache it
	 return val;
      }


      
      
      ICollectionMetadata::const_iterator
      RelationalCollectionMetadata::begin() const
      {
         if( !m_metadataTable )
            return end();
	 std::unique_ptr<coral::IQuery> query( m_metadataTable->newQuery() );

	 coral::AttributeList        output;
	 output.extend( "key", "string");
	 query->addToOutputList( RelationalCollectionNames::metadataKeyColumn() );
	 output.extend( "value", "string");
	 if( m_isOracle ) {
	    // CLOB -> string conversion to enable bulk reads
	    query->addToOutputList(" TO_CHAR( SUBSTR( " + RelationalCollectionNames::metadataValueColumn() + ", 0, 4000 ) )"  );
	 } else {
	    query->addToOutputList( RelationalCollectionNames::metadataValueColumn() );
	 }
	 query->defineOutput( output );

	 query->setRowCacheSize( 5000 );
         query->setCondition( RelationalCollectionBindVariables::whereClauseForMetadata(),
                              m_whereDataForMetadata );
         return ICollectionMetadata::const_iterator(
           std::make_unique<RelationalCollectionMetadataIterator>( this, query.release(),
						      entries() < 10000? &m_keyInfo : 0 ) );
      }

        
      ICollectionMetadata::const_iterator
      RelationalCollectionMetadata::end() const
      {
         return ICollectionMetadata::const_iterator(
            std::make_unique<RelationalCollectionMetadataIterator>( nullptr, nullptr, nullptr ) );
      }
        

   } // end namespace
}

