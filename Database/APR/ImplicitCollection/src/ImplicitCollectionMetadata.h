/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

    unsigned long long		entries() const 	{
       return 0;
    }
    
    bool existsKey( const std::string& /* key */) const        {
        return false;
    }

    const char* getValueForKey( const std::string& /*key*/ ) const {
        return 0;
    }

    
    void setValueForKey( const std::string& /*key*/,
                          const std::string& /*val*/ )
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

      bool operator==( const ICollectionMetadataIterator& /*_rhs*/ ) const {
          return true;
      }

          
      bool operator!=( const ICollectionMetadataIterator& rhs ) const {
          return !operator==( rhs );
      }
          
      void operator++() { }

      const std::string&        key() const { static std::string key; return key; }
      const char *                value() const { return 0; }

    };

    
    
    ICollectionMetadata::const_iterator        begin() const        {
        return ICollectionMetadata::const_iterator( std::make_unique<ImplicitCollectionMetadataIterator>() );
    }

    
    ICollectionMetadata::const_iterator        end() const        {
        return ICollectionMetadata::const_iterator( std::make_unique<ImplicitCollectionMetadataIterator>() );
    }
            
  };  // end ImplicitCollectionMetadata

} // end namespace

#endif
