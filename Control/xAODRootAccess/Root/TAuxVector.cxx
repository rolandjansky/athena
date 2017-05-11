/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TAuxVector.cxx 797214 2017-02-14 19:51:39Z ssnyder $

// ROOT include(s):
#include <TClass.h>
#include <TVirtualCollectionProxy.h>
#include <TError.h>

// EDM include(s):
#include "AthContainersInterfaces/IAuxTypeVectorFactory.h"

// Local include(s):
#include "xAODRootAccess/tools/TAuxVector.h"
#include "xAODRootAccess/tools/Message.h"

namespace xAOD {

   TAuxVector::TAuxVector( const SG::IAuxTypeVectorFactory* factory,
                           ::TClass* cl, size_t size, size_t )
      : m_factory( factory ),
        m_proxy( cl->GetCollectionProxy() ), m_vec( cl->New() ) {

      // A little sanity check:
      if( ! m_proxy ) {
         ::Fatal( "xAOD::TAuxVector::TAuxVector",
                  XAOD_MESSAGE( "No collection proxy found for type %s" ),
                  cl->GetName() );
      }

      // Make sure the object is of the right size:
      this->resize( size );
   }

   TAuxVector::TAuxVector( const TAuxVector& parent )
      : m_factory( parent.m_factory ), m_proxy( parent.m_proxy ),
        m_vec( parent.m_proxy->GetCollectionClass()->New() ) {

      // Check the size of the parent object:
      const size_t size = parent.size();
      // Check if we need to do anything:
      if( ! size ) {
         return;
      }

      // First off, resize this vector:
      this->resize( size );

      // Get a pointer to the start of the parent's payload:
      const void* parentPtr = 0;
      {
         TVirtualCollectionProxy::TPushPop bind( m_proxy, parent.m_vec );
         parentPtr = m_proxy->At( 0 );
      }

      // Do the copy:
      this->copyRange( parentPtr, this->toPtr(), size );
   }

   TAuxVector::~TAuxVector() {

      m_proxy->Destructor( m_vec );
   }

   TAuxVector& TAuxVector::operator= ( const TAuxVector& other ) {

      // Check if anything needs to be done:
      if( this == &other ) {
         return *this;
      }

      // Clean out the previous payload:
      m_proxy->Destructor( m_vec );

      // Get the information from the other object:
      m_factory = other.m_factory;
      m_proxy   = other.m_proxy;

      // Create a new vector:
      m_vec = m_proxy->GetCollectionClass()->New();

      // Check the size of the other object:
      const size_t size = other.size();
      // Check if we need to do anything:
      if( ! size ) {
         return *this;
      }

      // Get a pointer to the start of the other object's payload:
      const void* otherPtr = 0;
      {
         TVirtualCollectionProxy::TPushPop bind( m_proxy, other.m_vec );
         otherPtr = m_proxy->At( 0 );
      }

      // Do the copy:
      this->copyRange( otherPtr, this->toPtr(), size );

      // Return a reference to this object:
      return *this;
   }

   SG::IAuxTypeVector* TAuxVector::clone() const {

      return new TAuxVector( *this );
   }

   void* TAuxVector::toPtr() {

      TVirtualCollectionProxy::TPushPop bind( m_proxy, m_vec );
      return m_proxy->At( 0 );
   }

   void* TAuxVector::toVector() {

      return m_vec;
   }

   size_t TAuxVector::size() const {

      TVirtualCollectionProxy::TPushPop bind( m_proxy, m_vec );
      return m_proxy->Size();
   }

   bool TAuxVector::resize( size_t sz ) {

      TVirtualCollectionProxy::TPushPop bind( m_proxy, m_vec );
      const void* orig = toPtr();
      m_proxy->Allocate( sz, false );
      return toPtr() == orig;
   }

   void TAuxVector::reserve( size_t ) {

      return;
   }

   /**
    * @brief Shift the elements of the vector.
    * @param pos The starting index for the shift.
    * @param offs The (signed) amount of the shift.
    *
    * This operation shifts the elements in the vectors for all
    * aux data items, to implement an insertion or deletion.
    * @c offs may be either positive or negative.
    *
    * If @c offs is positive, then the container is growing.
    * The container size should be increased by @c offs,
    * the element at @c pos moved to @c pos + @c offs,
    * and similarly for following elements.
    * The elements between @c pos and @c pos + @c offs should
    * be default-initialized.
    *
    * If @c offs is negative, then the container is shrinking.
    * The element at @c pos should be moved to @c pos + @c offs,
    * and similarly for following elements.
    * The container should then be shrunk by @c -offs elements
    * (running destructors as appropriate).
    */
   void TAuxVector::shift( size_t pos, ptrdiff_t offs ) {

      TVirtualCollectionProxy::TPushPop bind( m_proxy, m_vec );
      size_t eltsz = m_proxy->GetIncrement();
      if( offs < 0 ) {

         if( -offs > static_cast< ptrdiff_t >( pos ) ) {
            offs = -pos;
         }
         char* beg = reinterpret_cast< char* >( m_proxy->At( 0 ) );
         copyRange( beg + eltsz * pos,
                    beg + eltsz * ( pos + offs ),
                    m_proxy->Size() - pos );
         m_proxy->Allocate( m_proxy->Size() + offs, false );

      } else if( offs > 0 ) {

         size_t oldsz = m_proxy->Size();
         m_proxy->Allocate( oldsz + offs, false );
         char* beg = reinterpret_cast< char* >( m_proxy->At( 0 ) );
         copyRange( beg + eltsz * pos,
                    beg + eltsz * ( pos + offs ),
                    m_proxy->Size() - pos - offs);
         clearRange( beg + eltsz * pos, offs );
      }

      return;
   }

   bool TAuxVector::insertMove (size_t pos, void* beg, void* end)
   {
     TVirtualCollectionProxy::TPushPop bind (m_proxy, m_vec);
     size_t eltsz = m_proxy->GetIncrement();
     const void* orig = this->toPtr();

     char* begp = reinterpret_cast<char*> (beg);
     char* endp = reinterpret_cast<char*> (end);
     size_t nelt = (endp-begp) / eltsz;

     shift (pos, nelt);
     // FIXME: want move, not copy.
     // But i don't seem to be able to call move operations through cling,
     // so just use copy for now.
     copyRange (beg,
                reinterpret_cast<char*>(this->toPtr()) + pos*eltsz,
                nelt);
     return this->toPtr() == orig;
   }


   void TAuxVector::copyRange( const void* src, void* dst, size_t n ) {

      // Size of an element in the vector:
      const size_t eltsz = m_proxy->GetIncrement();

      // If the payload is a class:
      if( m_proxy->GetValueClass() ) {
         // If the source range doesn't overlap with the destination:
         if( ( dst > src ) &&
             ( ( reinterpret_cast< unsigned long >( src ) + n * eltsz ) >
               reinterpret_cast< unsigned long >( dst ) ) ) {
            for( size_t i = n - 1; i < n; --i ) {
               m_factory->copy( dst, i, src, i );
            }
         }
         // If it does:
         else {
            for( size_t i = 0; i < n; ++i ) {
               m_factory->copy( dst, i, src, i );
            }
         }
      }
      // If the payload is an array of primitives, the copying is much simpler:
      else {
         memmove( dst, src, n * eltsz );
      }

      return;
   }

   void TAuxVector::clearRange( void* dst, size_t n ) {

      // Size of an element in the vector:
      size_t eltsz = m_proxy->GetIncrement();

      // If the payload is a class:
      if( m_proxy->GetValueClass() ) {
         for( size_t i = 0; i < n; ++i ) {
            m_factory->clear (dst, i);
         }
      }
      // If the payload is an array of primitives, the cleaning is much simpler:
      else {
         memset( dst, 0, n * eltsz );
      }

      return;
   }

} // namespace xAOD
