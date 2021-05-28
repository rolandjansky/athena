/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
        
	virtual unsigned long long	entries() const override;
	
        void 	readKeys();

        virtual bool 	existsKey( const std::string& key ) override;

        virtual const char* getValueForKey( const std::string& key ) override;
        
        virtual void 	setValueForKey( const std::string& key,
                                        const std::string& val ) override;

        
        virtual ICollectionMetadata::const_iterator        begin() override;
        
        virtual ICollectionMetadata::const_iterator        end() override;



        
        class RootCollectionMetadataIterator
           : public ICollectionMetadata::ICollectionMetadataIterator
        {
        public:
       friend class RootCollectionMetadata;
          
          RootCollectionMetadataIterator( const MetadataKeyMap_t::const_iterator& iter,
                                          RootCollectionMetadata* mdata );

          virtual ~RootCollectionMetadataIterator() {}

          virtual bool operator==( const ICollectionMetadataIterator& _rhs ) const override;

          virtual bool operator!=( const ICollectionMetadataIterator& rhs ) const override {
             return !operator==( rhs );
          }
             
          virtual void operator++() override;
             
          virtual const std::string&        key() const override;
          virtual const char *              value() override;

       protected:
          MetadataKeyMap_t::const_iterator        m_iterator;
          RootCollectionMetadata*                 m_metadata;
        };

        
     private:
        ICollection::OpenMode      m_mode;
        TTree*                     m_tree;
        TBranch*                   m_keyBranch;
        TBranch*                   m_valBranch;
        bool                       m_hasKeys;
        
        MetadataKeyMap_t           m_keyMap;

	/// buffer for reading strings from ROOT
        char                      *m_charBuffer;

	coral::MessageStream      *m_poolOut;
      };  // end RootCollectionMetadata
   } // end namespace
}

#endif
