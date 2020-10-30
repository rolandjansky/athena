/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RelationalCollectionMetadata_h
#define RelationalCollectionMetadata_h

#include "CollectionBase/ICollection.h"
#include "CollectionBase/ICollectionMetadata.h"

#include "CoralBase/AttributeList.h"

#include <string>
#include <map>


namespace coral {
   class ISessionProxy;
   class IQuery;
   class ICursor;
   class ITable;
}


namespace pool {
   namespace RelationalCollection { 

      class RelationalCollectionMetadata : public ICollectionMetadata
      {
     public:

        RelationalCollectionMetadata();
        virtual ~RelationalCollectionMetadata();

        void    initialize( const std::string& collectionName,
                            coral::ISessionProxy& session,
                            pool::ICollection::OpenMode mode );

	/// refresh table pointers after commit
        void    		reinitialize( coral::ISessionProxy* session );
        
        // void readKeys() const;

	/// count metadata entries for this collection
	unsigned long long 	entries() const;
	   
        bool    		existsKey( const std::string& key ) const;

	/// returns a pointer to an internal cache - next call will erase previous value
        const char* 		getValueForKey( const std::string& key ) const;
        
        void		    	setValueForKey( const std::string& key,
						const std::string& val );

        int     		deleteCollectionMetadata();


        class RelationalCollectionMetadataIterator
           : public ICollectionMetadata::ICollectionMetadataIterator
        {
        public:
       friend class RelationalCollectionMetadata;
          
          RelationalCollectionMetadataIterator( const RelationalCollectionMetadata* meta,
						coral::IQuery *query,
						std::map<std::string,bool> *keymap );
          virtual ~RelationalCollectionMetadataIterator( );

          bool operator==( const ICollectionMetadataIterator& _rhs ) const;
          bool operator!=( const ICollectionMetadataIterator& rhs ) const {
             return !operator==( rhs );
          } 
          void operator++();
             
          const std::string&	key() const;
          const char *		value() const;

       protected:
	  const RelationalCollectionMetadata* m_metadata;
          coral::IQuery*	m_query;
          coral::ICursor*	m_cursor;
	  std::map<std::string,bool> *m_keyInfo;
          bool			m_valid;
        };


        
        ICollectionMetadata::const_iterator        begin() const;
        ICollectionMetadata::const_iterator        end() const;
           
     private:
        ICollection::OpenMode		m_mode;
	
        std::string			m_collectionName;
	/// true if the collection is in Oracle
	bool				m_isOracle;

	mutable std::map<std::string, bool>	m_keyInfo;
        
        /// Reference to the metadata table (do not delete)
        coral::ITable*			m_metadataTable;
        
        mutable coral::AttributeList	m_whereDataForMetadata;
        mutable coral::AttributeList	m_whereDataForMetadataKey;

	/// cache for the last value retrieved by getValue()
	mutable std::string		m_lastValue;
      };  // end RelationalCollectionMetadata

   } // end namespace
}

#endif






