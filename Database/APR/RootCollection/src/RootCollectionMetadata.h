/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RootCollectionMetadata_h
#define RootCollectionMetadata_h

#include "CollectionBase/ICollection.h"
#include "CollectionBase/ICollectionMetadata.h"

class TFile;
class TTree;
class TBranch;

namespace coral {
   class MessageStream;
}

#include <map>

namespace pool {
   namespace RootCollection { 

      class RootCollectionMetadata : public ICollectionMetadata
      {
     public:
        typedef        std::map< std::string, long long int>        MetadataKeyMap_t;

        RootCollectionMetadata();        
        virtual ~RootCollectionMetadata();

        void 	initialize( TFile* file, pool::ICollection::OpenMode mode );

        void 	commit();
        
	unsigned long long	entries() const;
	
        void 	readKeys() const;

        bool 	existsKey( const std::string& key ) const;

        const char* getValueForKey( const std::string& key ) const;
        
        void 	setValueForKey( const std::string& key,
				const std::string& val );

        
        ICollectionMetadata::const_iterator        begin() const;
        
        ICollectionMetadata::const_iterator        end() const;



        
        class RootCollectionMetadataIterator
           : public ICollectionMetadata::ICollectionMetadataIterator
        {
        public:
       friend class RootCollectionMetadata;
          
          RootCollectionMetadataIterator( const MetadataKeyMap_t::const_iterator& iter,
                                          const RootCollectionMetadata* mdata );

          virtual ~RootCollectionMetadataIterator() {}

          bool operator==( const ICollectionMetadataIterator& _rhs ) const;

          bool operator!=( const ICollectionMetadataIterator& rhs ) const {
             return !operator==( rhs );
          }
             
          void operator++();
             
          const std::string&        key() const;
          const char *              value() const;

       protected:
          MetadataKeyMap_t::const_iterator        m_iterator;
          const RootCollectionMetadata*           m_metadata;
        };

        
     private:
        ICollection::OpenMode      m_mode;
        TTree*                     m_tree;
        TBranch*                   m_keyBranch;
        TBranch*                   m_valBranch;
        mutable bool               m_hasKeys;
        
        mutable MetadataKeyMap_t   	m_keyMap;

	/// buffer for reading strings from ROOT
        mutable char               	*m_charBuffer;

	mutable coral::MessageStream	*m_poolOut;
      };  // end RootCollectionMetadata
   } // end namespace
}

#endif
