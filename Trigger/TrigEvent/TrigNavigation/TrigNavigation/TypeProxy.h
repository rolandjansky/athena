// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGNAVIGATION_TYPEPROXY_H
#define TRIGNAVIGATION_TYPEPROXY_H

#include <type_traits>

// Gaudi/Athena include(s):
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/StatusCode.h"
#include "CxxUtils/checker_macros.h"
#include "SGTools/DataProxy.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/ClassID_traits.h"
#include "AthenaKernel/errorcheck.h"
#include "AthContainers/AuxVectorBase.h"

// Forward declaration(s):
namespace SG {
   class IAuxStore;
}

namespace HLTNavDetails {

   /**
    *  @short Interface to the TypeProxy<...> objects created at runtime
    *
    *         It's not really a pure interface, but to simplify the class
    *         structure, let's keep this naming...
    *
    * @author Tomasz Bold <Tomasz.Bold@cern.ch>
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    */
   class ITypeProxy {

   public:
      /// Default constructor
      ITypeProxy() = default;
      /// Virtual destructor, to make vtable happy...
      virtual ~ITypeProxy() = default;

      /// Make a new object
      virtual StatusCode create() = 0;
      /// Save proxied object in DG (record)
      virtual StatusCode reg( StoreGateSvc* sg, const std::string& key ) = 0;
      /// SG retrieve, and fill the proxy
      virtual StatusCode sync( StoreGateSvc* sg, const std::string& key );
      /// Remove the object from SG and make the pointer null
      virtual StatusCode clear( StoreGateSvc* sg ) = 0;

      /// @name Cast operations for xAOD decorations
      /// Actual implementation rely in std::is_base_of to avoid dynamic_cast
      /// @{

      /// Return a pointer to the SG::AuxVectorBase base class of the object if
      /// possible
      SG::AuxVectorBase* castAuxVectorBase();
      /// Return a pointer to the SG::IAuxStore base class of the object if
      /// possible
      SG::IAuxStore* castIAuxStore();

      virtual void syncTypeless() = 0;

      /// @}

      /// Check whether StoreGate contains an object of this type and with
      /// the specified key
      bool contains( StoreGateSvc* sg, const std::string& key ) const;

      /// Check whether StoreGate contains a modifyable object of this type
      /// and with the specified key
      bool transientContains( StoreGateSvc* sg, const std::string& key ) const;

      /// True if proxy has not yet made object or not synced to any yet
      /// (0 pointer)
      bool empty() const;

      /// This is how typed proxy is obtained
      virtual ITypeProxy* clone() const = 0;

      /// The CLID of the object being proxied
      CLID clid() const;

      /// The type name of the object being proxied
      const std::string& typeName() const;

      const void* cptr() const { return m_pointer; }

   protected:
      CLID m_clid{0}; ///< The CLID of the type being proxied
      std::string m_typeName; ///< The type name of the object being proxied
      std::string m_key; ///< StoreGate key of the proxied object
      SG::DataProxy* m_proxy{nullptr}; ///< StoreGate proxy for the object
      void* m_ncPointer{nullptr}; ///< Non-const pointer to the proxied object
      const void* m_pointer{nullptr}; ///< Const pointer to the proxied object
      /// Does the proxied type inherit from SG::AuxVectorBase?
      bool m_isAuxVectorBase{false};
   }; // class ITypeProxy
  
  /*
   *  @short Typeless read-only AuxStore implementation of ITypeProxy
   *
   * @author Lukas Heinrich <lukas.heinrich@cern.ch>
   *
   */
  class TypelessAuxProxy : public ITypeProxy {
  public:
    
    virtual StatusCode create() override {
      REPORT_MESSAGE( MSG::ERROR ) << "read-only aux proxy. it can't create new objects" << endmsg;
      return StatusCode::FAILURE;
    }
    virtual StatusCode reg( StoreGateSvc* , const std::string&  ) override {
      REPORT_MESSAGE( MSG::ERROR ) << "read-only aux proxy. it can't register new objects to SG" << endmsg;
      return StatusCode::FAILURE;
    };
    virtual StatusCode clear( StoreGateSvc*  ) override {
      REPORT_MESSAGE( MSG::ERROR ) << "read-only aux proxy. it can't delete objects (because it didn't create any)" << endmsg;
      return StatusCode::FAILURE;
    };
    
    virtual StatusCode sync( StoreGateSvc* sg, const std::string& key ) override {
      REPORT_MESSAGE( MSG::INFO ) << "syncing a read-only Aux proxy with key " << key << endmsg;
      const SG::IAuxStore* aux = nullptr;
      StatusCode sc = sg->retrieve(aux,key);
      if(!aux or sc.isFailure() ){
        REPORT_MESSAGE( MSG::ERROR ) << "syncing of read-only Aux proxy failed" << key << endmsg;
        return StatusCode::FAILURE;
      }
      // We do need the non-const Aux store pointer as well to be able to fill
      // it during deserialization. So the comment above that this is read-only
      // is not quite correct.
      auto nonconstaux ATLAS_THREAD_SAFE = const_cast<SG::IAuxStore*>(aux);
      m_ncPointer = nonconstaux;
      m_pointer = aux;

      REPORT_MESSAGE( MSG::INFO ) << "syncing of read-only Aux proxy succeeded" << key << endmsg;

      return StatusCode::SUCCESS;
    }

    virtual void syncTypeless() override {
      REPORT_MESSAGE( MSG::ERROR ) << "read-only Aux Proxy, syncing typeless" << endmsg;
    };
    
    virtual ITypeProxy* clone() const override {
      REPORT_MESSAGE( MSG::ERROR ) << "read-only Aux Proxy should be created by direct object creation, not via clone of static instance" << endmsg;
      return nullptr;
    };



  };

   /**
    *  @short Type specific implementation of ITypeProxy
    *
    * @author Tomasz Bold <Tomasz.Bold@cern.ch>
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 783046 $
    * $Date: 2016-11-08 18:17:33 +0100 (Tue, 08 Nov 2016) $
    */
   template< class T >
   class TypeProxy : public ITypeProxy {

   public:
      /// Convenience type declaration
      typedef T value_type;
      /// Convenience type declaration
      typedef T* pointer_type;

      /// Default constructor
      TypeProxy();
      /// Constructor pointing to an existing object
      TypeProxy( pointer_type t );

      /// Make a new object
      virtual StatusCode create() override;
      /// Save proxied object in DG (record)
      virtual StatusCode reg( StoreGateSvc* sg, const std::string& key ) override;
      /// SG retrieve, and fill the proxy
      virtual StatusCode sync( StoreGateSvc* sg, const std::string& key ) override;
      /// Remove the object from SG and make the pointer null
      virtual StatusCode clear( StoreGateSvc* sg ) override;

      virtual void syncTypeless() override {
         m_ncPointer = m_data;
         m_pointer = m_data;
      }

      /// This is how typed proxy is obtained
      virtual ITypeProxy*  clone() const override { return new TypeProxy<T>(); }

      /// Pointer to the exact object type
      pointer_type data() { return m_data; }
      /// Constant pointer to the exact object type
      const pointer_type& data() const { return m_data; }
      /// Reference to the exact object type pointer
      pointer_type& data_ref() { return m_data; }

   private:
      pointer_type m_data{nullptr}; ///< Non-const pointer to the proxied object

   }; // class TypeProxy

   /// Default constructor
   template< class T >
   TypeProxy< T >::TypeProxy()
      : ITypeProxy() {

      // Set the properties of the base class:
      m_clid = ClassID_traits< T >::ID();
      m_typeName = ClassID_traits< T >::typeName();
      m_isAuxVectorBase = std::is_base_of< SG::AuxVectorBase, T >::value;
   }

   /// Constructor pointing to an existing object
   ///
   /// Notice that this constructor delegates much of the work to the default
   /// constructor. (C++11 magic...)
   ///
   template< class T >
   TypeProxy< T >::TypeProxy( pointer_type t )
      : TypeProxy() {

      // Set the properties of this class:
      m_data = t;

      // Set the properties of the base class:
      m_ncPointer = m_data;
      m_pointer = m_data;
   }

   template< class T >
   StatusCode TypeProxy< T >::create() {

      // Check if we already proxy some object. Then this should not be called
      // again...
      if( m_data ) {
         REPORT_MESSAGE( MSG::ERROR )
            << "Object is already being proxied";
         return StatusCode::FAILURE;
      }

      // Create the new object:
      m_data = new T();

      // Fill the base class's variables:
      m_ncPointer = m_data;
      m_pointer = m_data;

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   template< class T >
   StatusCode TypeProxy< T >::reg( StoreGateSvc* sg, const std::string& key ) {

      // Record the object:
      CHECK( sg->record( m_data, key ) );

      // Fill some variables in the base class:
      m_key = key;
      m_proxy = sg->proxy( m_clid, m_key );

      return StatusCode::SUCCESS;
   }

   /**
    * Unfortunately the typeless removeProxy functions of StoreGateSvc are all
    * private. Although I would've loved to implement this function in a
    * non-template way as well... :-/
    */
   template< class T >
   StatusCode TypeProxy< T >::clear( StoreGateSvc* sg ) {

      // Do the deed:
      CHECK( sg->remove( m_data ) );

      // Clear the object:
      m_data = nullptr;
      m_proxy = nullptr;
      m_ncPointer = nullptr;
      m_pointer = nullptr;

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   template< class T >
   StatusCode TypeProxy< T >::sync( StoreGateSvc* sg, const std::string& key ) {

      // First let the base class do its own thing:
      CHECK( ITypeProxy::sync( sg, key ) );

      // Get the object out of the proxy:
      m_data = SG::DataProxy_cast< T >( m_proxy );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace HLTNavDetails

#endif // not TRIGNAVIGATION_TYPEPROXY_H
