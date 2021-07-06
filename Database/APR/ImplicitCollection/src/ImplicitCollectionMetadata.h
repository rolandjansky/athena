/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ImplicitCollectionMetadata_h
#define ImplicitCollectionMetadata_h

/**
 * @file ImplicitCollectionMetadata.h
 * @brief Fake metadata class implementation for ImplicitCollection
 * @author Marcin.Nowak@cern.ch
 */


#include "CollectionBase/ICollection.h"
#include "CollectionBase/ICollectionMetadata.h"

#include "POOLCore/Exception.h"


namespace pool {

  /// Fake metadata class implementation for ImplicitCollection
  class ImplicitCollectionMetadata : public ICollectionMetadata
  {
  public:
    ImplicitCollectionMetadata() { }

    virtual unsigned long long		entries() const override {
       return 0;
    }
    
    virtual bool existsKey( const std::string& /* key */) override {
        return false;
    }

    virtual const char* getValueForKey( const std::string& /*key*/ ) override {
        return 0;
    }

    
    virtual void setValueForKey( const std::string& /*key*/,
                                 const std::string& /*val*/ ) override
    {
        throw pool::Exception( "Metadata functionality not implemented",
                              "ImplicitCollectionMetadata::setValueForKey", 
                              "ImplicitCollection");
    }


    class ImplicitCollectionMetadataIterator
        : public ICollectionMetadata::ICollectionMetadataIterator
    {
    public:
    friend class ImplicitCollectionMetadata;
      
      ImplicitCollectionMetadataIterator() {}

      virtual
      bool operator==( const ICollectionMetadataIterator& /*_rhs*/ ) const override {
          return true;
      }


      virtual
      bool operator!=( const ICollectionMetadataIterator& rhs ) const override {
          return !operator==( rhs );
      }
          
      virtual void operator++() override { }

      virtual const std::string&        key() const override { static std::string key; return key; }
      virtual const char *                value() override { return 0; }

    };

    
    virtual
    ICollectionMetadata::const_iterator        begin() override {
        return ICollectionMetadata::const_iterator( std::make_unique<ImplicitCollectionMetadataIterator>() );
    }


    virtual
    ICollectionMetadata::const_iterator        end() override {
        return ICollectionMetadata::const_iterator( std::make_unique<ImplicitCollectionMetadataIterator>() );
    }
            
  };  // end ImplicitCollectionMetadata

} // end namespace

#endif
