// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODCORE_ADDDVPROXY_H
#define XAODCORE_ADDDVPROXY_H

// STL include(s):
#include <vector>
#include <set>

// ROOT include(s):
#include <TClass.h>
#include <TError.h>
#include <TInterpreter.h>

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODCore/tools/TDVCollectionProxy.h"

/// Macro for setting up proxying for a specific DataVector<T> type
///
/// In the xAOD packages that use DataVector classes, one must attach
/// "collection proxies" to the container dictionaries. This collection
/// proxy describes to ROOT how to navigate the contents of the
/// DataVector container.
///
/// This is set up by adding a file called
/// "src/dict/CollectionProxies.cxx" to the package (well, all files
/// under "src/dict/" are taken into account in the end...), and for each
/// DataVector type defined by a package, adding a line like:
///
/// <code>
///   ADD_DV_PROXY( MySuperContainer );
/// </code>
///
#define ADD_DV_PROXY( TYPE )                                            \
   namespace ROOT {                                                     \
      TGenericClassInfo* GenerateInitInstance( const TYPE* );           \
   }                                                                    \
   int register_##TYPE##_CollectionProxy() {                            \
      xAOD::AddDVProxy::add< TYPE >( ROOT::GenerateInitInstance( ( TYPE* ) 0x0 ) ); \
      return 1;                                                         \
   }                                                                    \
   static int _R__UNIQUE_( dummy_##TYPE##_Var ) =                       \
                        register_##TYPE##_CollectionProxy();            \
   R__UseDummy( _R__UNIQUE_( dummy_##TYPE##_Var ) )

/// Macro for setting up proxying for a specific namespaced DataVector<T> type
///
/// In the xAOD packages that use DataVector classes, one must attach
/// "collection proxies" to the container dictionaries. This collection
/// proxy describes to ROOT how to navigate the contents of the
/// DataVector container.
///
/// This is set up by adding a file called
/// "src/dict/CollectionProxies.cxx" to the package (well, all files
/// under "src/dict/" are taken into account in the end...), and for each
/// DataVector type defined by a package, adding a line like:
///
/// <code>
///   ADD_DV_PROXY( xAOD, MySuperContainer );
/// </code>
///
#define ADD_NS_DV_PROXY( NS, TYPE )                                     \
   namespace ROOT {                                                     \
      TGenericClassInfo* GenerateInitInstance( const NS::TYPE* );       \
   }                                                                    \
   int register_##NS##_##TYPE##_CollectionProxy() {                     \
      xAOD::AddDVProxy::add< NS::TYPE >( ROOT::GenerateInitInstance( ( NS::TYPE* ) 0x0 ) ); \
      return 1;                                                         \
   }                                                                    \
   static int _R__UNIQUE_( dummy_##NS##_##TYPE##_Var ) =                \
                        register_##NS##_##TYPE##_CollectionProxy();     \
   R__UseDummy( _R__UNIQUE_( dummy_##NS##_##TYPE##_Var ) )

namespace xAOD {

   /// Set up collection proxy for a @c DataVector class.
   ///
   /// If @a cl is a @c DataVector class, or derives from one, then we attach
   /// an appropriate Root collection proxy to it.  This is required for Root
   /// to recognize the class as a collection in, eg, @c TTree::Draw.
   ///
   class AddDVProxy {

   private:
      /// Helper structure implementing the resize(...) function
      ///
      /// When reading a branch of DataVector<T>, one needs to use a non-generic
      /// version of the resize(...) function. This implementation should work
      /// fine for all DataVector<T> types.
      ///
      /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
      ///
      template< class T >
      struct Helper {

         /// Type of the DataVector container
         typedef T Cont_t;
         /// Type of the elements in the DataVector
         typedef typename Cont_t::base_value_type Value_t;

         /// Function taking care of resizing DataVector<T> objects in memory
        /// when branches are read from a TTree.
         ///
         /// @param obj A pointer to the full-blown DataVector object
         /// @param size The size to re-size the vector to
         static void resize( void* obj, size_t size )  {

            // Cast the container to the right type:
            Cont_t* c = reinterpret_cast< Cont_t* >( obj );

            // Detach the DataVector from its auxiliary store:
            c->setStore( ( SG::IAuxStore* ) 0 );

            // Resize the container:
            c->resize( size );

            // Make sure that all elements exist and are usable:
            for( size_t i = 0; i < size; ++i ) {

               // If the element already exists, we're done:
               if( ( *c )[ i ] ) continue;

               // Create a new element:
               ( *c )[ i ] = new Value_t();
            }

            return;
         }

      }; // struct Helper

      /// Helper function force-loading all the needed dictionaries
      static void loadDictionaries();

   public:
      /// Set up collection proxy for a @c DataVector class.
      ///
      /// If @a cl is a @c DataVector class, or derives from one, then we attach
      /// an appropriate Root collection proxy to it.  This is required for Root
      /// to recognize the class as a collection in, eg, @c TTree::Draw.
      template < typename T >
      static void add( ROOT::TGenericClassInfo* clInfo ) {

         // Load the minimal amount of required dictionaries:
         loadDictionaries();

         // Create the collection proxy instance:
         TDVCollectionProxy* proxy =
            new TDVCollectionProxy( ClassName< T >::name().c_str() );
         proxy->SetResize( Helper< T >::resize );

         // Add it to the class info:
         clInfo->AdoptCollectionProxy( proxy );

         return;
      }

   }; // struct AddDVProxy

} // namespace xAOD

#endif // XAODCORE_ADDDVPROXY_H
