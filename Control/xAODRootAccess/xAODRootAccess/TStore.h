// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef XAODROOTACCESS_TSTORE_H
#define XAODROOTACCESS_TSTORE_H

// System include(s):
#include <string>
#include <map>
#include <memory>

// ROOT include(s):
#include <Rtypes.h>

// EDM include(s):
#include "AthContainers/ConstDataVector.h"

// Local include(s):
#include "xAODRootAccess/tools/TReturnCode.h"

namespace xAOD {

   // Forward declaration(s):
   class THolder;
   class TEvent;

   /// A relatively simple transient store for objects created in analysis
   ///
   /// This is a very simple transient store for objects that are created
   /// during analysis, but don't have to be written to the output file(s).
   /// To make it easier for the analysis tools to communicate with each other
   /// similar to how they would do it in Athena (with StoreGateSvc), they can
   /// use this class.
   ///
   /// The usage of this class is highly optional in analysis, it should only
   /// be used if really necessary. (Passing around objects in analysis code
   /// directly is usually a better approach than using a store in my mind...)
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class TStore {

      /// Make TEvent a friend of this class
      friend class TEvent;

   public:
      /// Default constructor
      TStore();
      /// Disallow copying the object
      TStore( const TStore& ) = delete;
      /// Destructor
      virtual ~TStore();

      /// Disallow copying the object
      TStore& operator=( const TStore& ) = delete;

      /// Set this as the active transient store in the application
      void setActive();

      /// @name Transient data accessor/modifier functions
      /// @{

      /// Function checking if an object is available from the store
      template< typename T >
      ::Bool_t contains( const std::string& key ) const;

      /// Function checking if an object with a given type is constant
      template< typename T >
      ::Bool_t isConst( const std::string& key ) const;

      /// Retrieve either a constant or non-constant object from the store
      template< typename T >
      TReturnCode retrieve( const T*& obj, const std::string& key ) const;
      /// Retrieve a non-constant object from the store
      template< typename T >
      TReturnCode retrieve( T*& obj, const std::string& key ) const;

      /// Add an object to the store
      template< typename T >
      TReturnCode record( T* obj, const std::string& key );
      /// Add an object othe store, explicitly taking ownership of it
      template< typename T >
      TReturnCode record( std::unique_ptr< T > obj, const std::string& key );

      /// Remove an object from the store by name
      TReturnCode remove( const std::string& key );
      /// Remove an object from the store by pointer
      TReturnCode remove( void* ptr );

      /// Clear the store of all of its contents
      void clear();

      /// @}

      /// Print the current contents of the transient store
      void print() const;

   protected:
      /// Non-templated function implementing the containment check
      ::Bool_t contains( const std::string& key,
                         const std::type_info& ti ) const;
      /// Non-templated function implementing the const-ness check
      ::Bool_t isConst( const std::string& key,
                        const std::type_info& ti ) const;
      /// Function retrieving a non-const object in a non-template way
      void* getObject( const std::string& key, const std::type_info& ti ) const;
      /// Function retrieving a const object in a non-template way
      const void* getConstObject( const std::string& key,
                                  const std::type_info& ti ) const;
      /// Function recording an object that has a dictionary available
      TReturnCode record( void* obj, const std::string& key,
                          const std::string& classname,
                          ::Bool_t isOwner = kTRUE );
      /// Function recording an object that has no dictionary
      TReturnCode record( void* obj, const std::string& key,
                          const std::type_info& ti );
      /// Function doing the first step of recording a ConstDataVector object
      template< class T >
      TReturnCode record( ConstDataVector< T >* obj, const std::string& key,
                          const std::type_info& ti );
      /// Function doing the second step of recording a ConstDataVector object
      TReturnCode record( THolder* hldr, const std::string& key );

      /// @name Functions mostly used by TEvent in the TVirtualEvent functions
      /// @{

      /// Check if an object with a given hash is managed by the store
      ::Bool_t contains( uint32_t hash ) const;
      /// Check if an object with a given pointer is managed by the store
      ::Bool_t contains( const void* ptr ) const;
      /// Get the name corresponding to a hashed key
      const std::string& getName( uint32_t hash ) const;
      /// Get the name of a managed object
      const std::string& getName( const void* ptr ) const;

      /// @}

      /// Type of the internal container storing all the objects
      typedef std::map< std::string, THolder* > Objects_t;
      /// Type of the internal storage for the hashed keys of the object names
      typedef std::map< uint32_t, std::string > HashedKeys_t;

      /// The object storage
      Objects_t m_objects;
      /// The key map
      HashedKeys_t m_keys;

   }; // class TStore

} // namespace xAOD

// Include the template implementation.
#include "TStore.icc"

#endif // XAODROOTACCESS_TSTORE_H
