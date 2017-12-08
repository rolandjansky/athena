/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TEventProxyDict.cxx 796514 2017-02-10 04:33:07Z ssnyder $
//
// File holding the implementation of the xAOD::TEvent functions that implement
// the IProxyDict interface. Just to make TEvent.cxx a little smaller.
//

// System include(s):
#include <stdexcept>
#include <set>

// ROOT include(s):
#include <TClass.h>
#include <TString.h>
#include <TBranch.h>

// Offline include(s):
#ifndef XAOD_STANDALONE
#   include "SGTools/DataProxy.h"
#   include "SGTools/TransientAddress.h"
#   include "SGTools/DataBucketBase.h"
#   include "GaudiKernel/Converter.h"
#   include "GaudiKernel/GenericAddress.h"
#endif // NOT XAOD_STANDALONE

// Local include(s):
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/Message.h"
#include "xAODRootAccess/tools/THolder.h"
#include "xAODRootAccess/tools/TObjectManager.h"

#ifndef XAOD_STANDALONE
namespace xAODPrivate {

   /// Helper object for holding something through a THolder
   class THolderBucket : public DataBucketBase {

   public:
      /// Constructor with an existing holder
      THolderBucket( const std::string& key,
                     const std::type_info& ti,
                     xAOD::TEvent& event )
         : m_key( key ), m_ti( ti ), m_event( event ) {}

      /// Return the object held
      void* object() override {

         // Look among the output objects first:
         const void* result = m_event.getOutputObject( m_key, m_ti );
         // Check if it succeeded:
         if( ! result ) {
            // Try the input then:
            result = m_event.getInputObject( m_key, m_ti );
         } else {
            // Even if there is an output object with this key, it may
            // be an input object that was copied to the output. So let's
            // try to silently retrieve an input object as well. This makes
            // sure that the input/output object is updated for the current
            // event.
            m_event.getInputObject( m_key, m_ti, kTRUE );
         }

         // Return the pointer:
         return const_cast< void* >( result );
      }

      /// Return the type_info of the stored object
      const std::type_info& tinfo() const override {

         return m_ti;
      }

      /// Return the object, cast to a CLID's type
      void* cast( CLID, SG::IRegisterTransient*,
                  bool ) override {

         throw std::runtime_error( "THolderBucket::cast not implemented" );
         return 0;
      }

      /// Return the object, cast to a certain type
      void* cast( const std::type_info& tinfo,
                  SG::IRegisterTransient*,
                  bool isConst ) override {

         // Do the cast:
         static const bool QUIET = true;
         const void* result = 0;
         if( isConst ) {
            // Just look among the inputs:
            result = m_event.getInputObject( m_key, tinfo, QUIET );
         } else {
            // Look among the output objects first:
            result = m_event.getOutputObject( m_key, tinfo, QUIET );
            // Check if it succeeded:
            if( ! result ) {
               // Try the input then:
               result = m_event.getInputObject( m_key, tinfo, QUIET );
            } else {
               // Even if there is an output object with this key, it may
               // be an input object that was copied to the output. So let's
               // try to silently retrieve an input object as well. This makes
               // sure that the input/output object is updated for the current
               // event.
               m_event.getInputObject( m_key, tinfo, QUIET );
            }
         }

         // Return the pointer:
         return const_cast< void* >( result );
      }

      /// Clone the current bucket
      DataBucketBase* clone() const override {

         return new THolderBucket( m_key, m_ti, m_event );
      }

      /// Give up ownership of the bucket's contents. A no-op.
      void relinquish() override {}
      /// Lock the held object. A no-op.
      void lock() override {}

   private:
      /// The key of the object
      const std::string m_key;
      /// The type info of the object
      const std::type_info& m_ti;
      /// The original TEvent object
      xAOD::TEvent& m_event;

   }; // class THolderBucket

   class TLoader
     : public Converter
   {
   public:
     TLoader (xAOD::TEvent& event,
              const std::string& name,
              const std::type_info& ti)
       :  Converter (0, CLID_NULL),
          m_event (event),
          m_name (name),
          m_ti (ti),
          m_proxy (nullptr)
     {
     }


     void setProxy (SG::DataProxy& proxy)
     {
       m_proxy = &proxy;
     }


     virtual StatusCode createObj(IOpaqueAddress*, DataObject*&) override;
     virtual long repSvcType() const override { return 0; }


   private:
     xAOD::TEvent& m_event;
     std::string m_name;
     const std::type_info& m_ti;
     SG::DataProxy* m_proxy;
   };


StatusCode TLoader::createObj(IOpaqueAddress* /*addr*/, DataObject*& obj)
   {
     static const bool SILENT = true;
     static const bool METADATA = false;
     
     // Try to find the object amongst the output objects first:
     if( m_event.getOutputObject( m_name, m_ti, METADATA ) ) {
       obj = new xAODPrivate::THolderBucket( m_name, m_ti, m_event);
     }
     // If it's not on the output, try the input:
     else if( m_event.getInputObject( m_name, m_ti, SILENT, METADATA ) ) {
       obj = new xAODPrivate::THolderBucket( m_name, m_ti, m_event);
       m_proxy->setConst();
     }

     return StatusCode::SUCCESS;
   }


} // xAODPrivate namespace
#endif // not XAOD_STANDALONE

namespace xAOD {

#ifdef XAOD_STANDALONE
   /// Helper typedef when building the code standalone
   typedef TReturnCode StatusCode;
   /// Helper variable for returning a successful status code
   static const TReturnCode::EReturnCode STATUSCODE_SUCCESS =
      TReturnCode::kSuccess;
#else
   /// Helper variable for returning a successful status code
   static const unsigned long STATUSCODE_SUCCESS = StatusCode::SUCCESS;
#endif // XAOD_STANDALONE

   SG::DataProxy* TEvent::proxy( const void* const pTransient ) const {

      // Look up the name of this object
      std::string name = getName( pTransient );
      if( name.empty() ) {
         // Apparently the object is not known...
         return nullptr;
      }

      // Get the metadata object for it:
      const xAOD::EventFormatElement* efe = 0;
      static const bool QUIET = true;
      if( m_outputEventFormat ) {
         efe = m_outputEventFormat->get( name, QUIET );
      }
      if( ! efe ) {
         efe = m_inputEventFormat.get( name, QUIET );
      }
      if( ! efe ) {
         // No metadata found...
         return nullptr;
      }

      // Return the proxy:
      BranchInfo* bi = getBranchInfo( efe->hash() );
      return bi->m_proxy.get();
   }

   SG::DataProxy* TEvent::proxy( const CLID&,
                                 const std::string& key ) const {

      const SG::sgkey_t sgkey = getHash( key );
      if( ! sgkey ) {
         return 0;
      }
      return proxy_exact( sgkey );
   }

   SG::DataProxy* TEvent::proxy_exact( SG::sgkey_t sgkey ) const {

      // Get the object describing this branch/object:
      BranchInfo* bi = getBranchInfo( sgkey );
      if( ! bi ) {
         static std::set< SG::sgkey_t > missingSGKeys;
         if( missingSGKeys.insert( sgkey ).second ) {
            ::Warning( "xAOD::TEvent::proxy_exact",
                       "Can't find BranchInfo for %d.",
                       sgkey );
         }
         return 0;
      }

      // Access its data proxy:
      SG::DataProxy* proxy = bi->m_proxy.get();

      // Return the proxy:
      return proxy;
   }

   TEvent::BranchInfo* TEvent::getBranchInfo( SG::sgkey_t sgkey ) const {

      // If the object already exists, return it:
      auto it = m_branches.find( sgkey );
      if( it != m_branches.end() ) {
         return &( it->second );
      }

      // If not, construct it now:
      BranchInfo bi;
      const xAOD::EventFormatElement* efe = getEventFormatElement( sgkey );
      if( ! efe ) {
         // Apparently this key is not known:
         return 0;
      }

      // Get the dictionary for the type:
      const std::string& className = efe->className();
      bi.m_class = TClass::GetClass( className.c_str() );
      if( ! bi.m_class ) {
         ::Warning( "xAOD::TEvent::getBranchInfo",
                    "Can't find TClass for `%s'",
                    className.c_str() );
         return 0;
      }
      if( ! bi.m_class->GetTypeInfo() ) {
         ::Warning( "xAOD::TEvent::getBranchInfo",
                    "No type_info available for `%s'",
                    className.c_str() );
         return 0;
      }

#ifndef XAOD_STANDALONE
      // Create a proper proxy for the input branch:
      SG::TransientAddress* taddr =
         new SG::TransientAddress( CLID_NULL,
                                   efe->branchName(),
                                   new GenericAddress() );
      taddr->setSGKey( sgkey );
      TEvent* nc_this = const_cast<TEvent*>(this);
      xAODPrivate::TLoader* loader =
        new xAODPrivate::TLoader (*nc_this,
                                  getName( sgkey ),
                                  *bi.m_class->GetTypeInfo());
      bi.m_proxy.reset( new SG::DataProxy( taddr, loader ) );
      loader->setProxy (*bi.m_proxy.get());
#endif // not XAOD_STANDALONE

      // Add the branch info to our list:
      auto ret = m_branches.insert( std::make_pair( sgkey, std::move( bi ) ) );

      // Return a pointer to the branch info:
      return &( ret.first->second );
   }

   const xAOD::EventFormatElement*
   TEvent::getEventFormatElement( SG::sgkey_t sgkey ) const {

      const xAOD::EventFormatElement* efe = 0;
      static const bool QUIET = true;
      if( m_outputEventFormat ) {
         efe = m_outputEventFormat->get( sgkey, QUIET );
      }
      if( ! efe ) {
         efe = m_inputEventFormat.get( sgkey, QUIET );
      }
      static std::set< SG::sgkey_t > missingSGKeys;
      if( ( ! efe ) && missingSGKeys.insert( sgkey ).second ) {
         ::Warning( "xAOD::TEvent::getEventFormatElement",
                    "Can't find EventFormatElement for hashed "
                    "SG key %d", sgkey );
         return 0;
      }
      return efe;
   }

   /// Smart pointers to objects that don't exist in the input event, can end
   /// up calling this function. In this case warn the user that something
   /// fishy is happening, and take posession of the received proxy.
   ///
   /// @param clid The CLID of the type. Not taken into account.
   /// @param proxy The proxy to take posession of. Not used for anything
   ///              useful.
   ///
   StatusCode TEvent::addToStore( CLID /*clid*/, SG::DataProxy* proxy ) {

      // Warn the user that the function got called:
      static bool warningPrinted = false;
      if( ! warningPrinted ) {
         ::Warning( "xAOD::TEvent::addToStore",
                    "Function should only be called through an invalid "
                    "ElementLink" );
         warningPrinted = true;
      }

      BranchInfo bi;
      bi.m_proxy.reset( proxy );
      m_branches.insert( std::make_pair(
#ifdef XAOD_STANDALONE
                                         0,
#else
                                         proxy->sgkey(),
#endif // XAOD_STANDALONE
                                         std::move( bi ) ) );

      // Return gracefully:
      return STATUSCODE_SUCCESS;
   }

   std::vector< const SG::DataProxy* > TEvent::proxies() const {

      std::vector< const SG::DataProxy* > ret;
      for( const auto& p : m_branches ) {
         const SG::DataProxy* proxy = p.second.m_proxy.get();
         if( proxy ) {
            ret.push_back( proxy );
         }
      }
      return ret;
   }

   SG::sgkey_t TEvent::stringToKey( const std::string& str, CLID ) {

      return getHash( str );
   }

   const std::string* TEvent::keyToString( SG::sgkey_t key ) const {

      return &( getName( key ) );
   }

   const std::string* TEvent::keyToString( SG::sgkey_t key, CLID& ) const {

      return &( getName( key ) );
   }

   void TEvent::registerKey( SG::sgkey_t, const std::string&, CLID ) {

      return;
   }

   SG::DataProxy* TEvent::recordObject( SG::DataObjectSharedPtr<DataObject>,
                                        const std::string&, bool, bool ) {

      throw std::runtime_error( "xAOD::TEvent::recordObject is not "
                                "implemented" );
   }

   StatusCode TEvent::updatedObject( CLID, const std::string& ) {

      throw std::runtime_error( "xAOD::TEvent::updatedObject is not "
                                "implemented" );
   }

   unsigned long TEvent::addRef() {

      return 0;
   }

   long unsigned int TEvent::release() {

      return 0;
   }

   const std::string& TEvent::name() const {

      static const std::string NAME = "xAOD::TEvent";
      return NAME;
   }

   StatusCode TEvent::queryInterface( const InterfaceID&, void** ) {

      // Return without doing anything:
      return STATUSCODE_SUCCESS;
   }

} // namespace xAOD
