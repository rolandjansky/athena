/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// ROOT include(s):
#include <TClass.h>
#include <TError.h>
#include <TString.h>
#include <TVirtualCollectionProxy.h>
#include <TInterpreter.h>

// Local include(s):
#include "xAODRootAccess/tools/TAuxVectorFactory.h"
#include "xAODRootAccess/tools/TAuxVector.h"
#include "xAODRootAccess/tools/Message.h"

namespace xAOD {

   TAuxVectorFactory::TAuxVectorFactory( ::TClass* cl )
      : m_class( cl ), m_proxy( cl->GetCollectionProxy() ),
        m_defElt( 0 ) {

      // A little sanity check:
      if( ! m_proxy ) {
         ::Fatal( "xAOD::TAuxVectorFactory::TAuxVectorFactory",
                  XAOD_MESSAGE( "No collection proxy found for type %s" ),
                  cl->GetName() );
      }

      // Check if the elements of the vector are objects:
      ::TClass* eltClass = m_proxy->GetValueClass();
      if( eltClass ) {
         // Initialise the assignment operator's method call:
         ::TString proto = "const ";
         proto += eltClass->GetName();
         proto += "&";
         m_assign = new TMethodCall;
         m_assign->InitWithPrototype( eltClass, "operator=", proto );
         if( ! m_assign->IsValid() ) {
            ::Warning( "xAOD::TAuxVectorFactory::TAuxVectorFactory",
                       XAOD_MESSAGE( "Can't get assignment operator for "
                                     "class %s" ),
                       eltClass->GetName() );
         }
         m_defElt = eltClass->New();
      }
   }

   TAuxVectorFactory::~TAuxVectorFactory() {

      // Remove the default element from memory if it exists:
      if( m_defElt ) {
         m_proxy->GetValueClass()->Destructor( m_defElt );
      }

      // The TMethodCall destructor will fail if TCling has already
      // been destroyed...
      if( gCling ) {
        delete m_assign;
      }
   }

   SG::IAuxTypeVector*
   TAuxVectorFactory::create( size_t size, size_t capacity ) const {

      return new TAuxVector( this, m_class, size, capacity );
   }

   SG::IAuxTypeVector*
   TAuxVectorFactory::createFromData( void* /*data*/, bool /*isPacked*/, bool ) const {

      std::abort();
   }

   void TAuxVectorFactory::copy( void* dst,        size_t dst_index,
                                 const void* src,  size_t src_index ) const {

      // The size of one element in memory:
      const size_t eltsz = m_proxy->GetIncrement();

      // Get the location of the source and target element in memory:
      dst = reinterpret_cast< void* >( reinterpret_cast< unsigned long >( dst ) +
                                       eltsz * dst_index );
      src =
         reinterpret_cast< const void* >( reinterpret_cast< unsigned long >( src ) +
                                          eltsz * src_index );

      // Do the copy either using the assignment operator of the type, or using
      // simple memory copying:
      if( m_assign && m_assign->IsValid() ) {
         m_assign->ResetParam();
         m_assign->SetParam( ( Long_t ) src );
         m_assign->Execute( dst );
      } else {
         memcpy( dst, src, eltsz );
      }

      return;
   }

   void TAuxVectorFactory::swap( void* a, size_t aindex,
                                 void* b, size_t bindex ) const {

      // The size of one element in memory:
      const size_t eltsz = m_proxy->GetIncrement();

      // Get the location of the two elements in memory:
      a = reinterpret_cast< void* >( reinterpret_cast< unsigned long >( a ) +
                                     eltsz * aindex );
      b = reinterpret_cast< void* >( reinterpret_cast< unsigned long >( b ) +
                                     eltsz * bindex );

      if( m_assign && m_assign->IsValid() ) {

         // Create a temporary object in memory:
         TClass* eltClass = m_proxy->GetValueClass();
         void* tmp = eltClass->New();

         // tmp = a
         m_assign->ResetParam();
         m_assign->SetParam( ( Long_t ) a );
         m_assign->Execute( tmp );
         // a = b
         m_assign->ResetParam();
         m_assign->SetParam( ( Long_t ) b );
         m_assign->Execute( a );
         // b = tmp
         m_assign->ResetParam();
         m_assign->SetParam( ( Long_t ) tmp );
         m_assign->Execute( b );

         // Delete the temporary object:
         eltClass->Destructor( tmp );

      } else {

         // Allocate some temporary memory for the swap:
         std::vector< char > tmp( eltsz );
         // tmp = a
         memcpy( tmp.data(), a, eltsz );
         // a = b
         memcpy( a, b, eltsz );
         // b = tmp
         memcpy( b, tmp.data(), eltsz );
      }

      return;
   }

   void TAuxVectorFactory::clear( void* dst, size_t dst_index ) const {

      // The size of one element in memory:
      const size_t eltsz = m_proxy->GetIncrement();

      // Get the memory address of the element:
      dst = reinterpret_cast< void* >( reinterpret_cast< unsigned long >( dst ) +
                                       eltsz * dst_index );

      if( m_assign && m_assign->IsValid() ) {
         // Assign the default element's contents to this object:
         m_assign->ResetParam();
         m_assign->SetParam( ( Long_t ) m_defElt );
         m_assign->Execute( dst );
      } else {
         // Set the memory to zero:
         memset( dst, 0, eltsz );
      }

      return;
   }

   size_t TAuxVectorFactory::getEltSize() const {

      return m_proxy->GetIncrement();
   }

   const std::type_info* TAuxVectorFactory::tiVec() const {

      return m_class->GetTypeInfo();
   }

} // namespace xAOD
