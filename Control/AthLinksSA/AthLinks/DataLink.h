// Dear emacs, this is -*- c++ -*-
/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */
#ifndef ATHLINKS_DATALINK_H
#define ATHLINKS_DATALINK_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <string>

// Local include(s):
#include "AthLinks/DataLinkBase.h"
#include "AthLinks/tools/selection_ns.h"

// Forward declaration(s):
namespace xAOD {
   class TVirtualEvent;
}
ENTER_ROOT_SELECTION_NS
template< class STORABLE >
class DataLink;
EXIT_ROOT_SELECTION_NS

/// @short DataLink implementation for ROOT usage
/// @file AthLinksSA/AthLinks/DataLink.h
/// This is an implementation of DataLink<T> for vanilla ROOT.
/// The code is quite simple, leaving most of the work for the
/// xAOD::TEvent class.
///
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
///
/// $Revision: 649074 $
/// $Date: 2015-02-23 17:51:40 +0100 (Mon, 23 Feb 2015) $
///
template< typename STORABLE >
class DataLink : public DataLinkBase {

public:
   /// @name Type definitions taken from the StoreGate-based implementation
   /// @{

   /// Type of the "regular" identifiers in the event
   typedef std::string ID_type;
   /// Integer identifier for the object in the event
   typedef uint32_t sgkey_t;

   /// Type of the object being handled
   typedef STORABLE value_type;
   /// Pointer type for the object being handled
   typedef STORABLE* __attribute__((deprecated)) pointer;
   /// Constant pointer type for the object being handled
   typedef const STORABLE* const_pointer;
   /// Reference type for the object being handled
   typedef STORABLE& __attribute__((deprecated)) reference;
   /// Constant reference type for the object being handled
   typedef const STORABLE& const_reference;

   /// @}

   /// @name Constructors
   /// @{

   /// Default constructor, creating a null pointer basically
   DataLink();

   /// Constructor taking an alternative TVirtualEvent object to use
   DataLink( xAOD::TVirtualEvent* event );

   /// Constructor receiving a reference to the target object
   DataLink( const_reference data, xAOD::TVirtualEvent* event = 0 );

   /// Constructor receiving a pointer to the target object
   DataLink( const_pointer data, xAOD::TVirtualEvent* event = 0 );

   /// Constructor receiving a user-readable ID for the target object
   DataLink( const ID_type& id, xAOD::TVirtualEvent* event = 0 );

   /// Constructor receiving a hashed ID for the target object
   DataLink( sgkey_t key, xAOD::TVirtualEvent* event = 0 );

   /// Copy constructor
   template< typename T >
   DataLink( const DataLink< T >& parent );

   /// @}

   /// @name Setter functions
   /// @{

   /// Set the link to a data object that's already in the event
   void toStorableObject( const_reference data );

   /// Set the link to the default object of this type in the event
   void toDefaultObject();

   /// Set the link using a user readable ID for the target object
   void toIdentifiedObject( const ID_type& id );
   /// Set the link using a hashed ID for the target object
   void toIdentifiedObject( sgkey_t key );

   /// @}

   /// @name Accessor functions
   /// @{

   /// Get a reference to the target object. May throw an exception.
   const_reference operator*() const;
   /// Get a pointer to the target object
   const_pointer operator->() const { return cptr(); }

   /// Automatic conversion operator
   operator const_pointer() const { return cptr(); }

   /// Get a constant pointer to the referenced object
   const_pointer cptr() const;

   /// Check if the object can be found
   bool isValid() const;

   /// Convenience validity check operator
   operator bool() const { return isValid(); }
   /// Another convenience validity checker
   bool operator!() const { return !isValid(); }

   /// Get the key that we reference, as a string
   const ID_type& dataID() const;
   /// Get the key that we reference, as a hash
   sgkey_t key() const { return persKey(); }

   /// @}

   /// Get the data source for the reference
   xAOD::TVirtualEvent* source() const { return m_event; }

   /// Prepare the object to be written out
   bool toPersistent();

private:
   /// Reset the object's internal cache
   virtual bool toTransient();
   /// Find the identifier of the object being pointed to
   void findIdentifier();

   /// Cached pointer to the object
   mutable const_pointer m_object;
   /// Pointer to the event object
   xAOD::TVirtualEvent* m_event;

   /// Needed to take the Reflex/Cling specifications into account
   typedef typename ROOT_SELECTION_NS::DataLink< STORABLE >::self DictSel;

}; // class DataLink

/*
 * The following piece of code declares the member variables of all
 * DataLink<T> types as transient.
 */
ENTER_ROOT_SELECTION_NS
template< class STORABLE >
class DataLink
#if ROOT_VERSION_CODE >= ROOT_VERSION( 6, 0, 2 )
   : public SelectNoInstance
#endif // ROOT_VERSION
{

public:
   /// Convenience declaration of this structure's type
   typedef DataLink< STORABLE > self;

#if ROOT_VERSION_CODE < ROOT_VERSION( 5, 99, 0 )

   /// Do not generate such dictionaries automatically
   ROOT_SELECTION_NS::NO_SELF_AUTOSELECT dummy;

   /// Declare m_object as transient
   ROOT_SELECTION_NS::TRANSIENT m_object;
   /// Declare m_event as transient
   ROOT_SELECTION_NS::TRANSIENT m_event;

#else

   /// Declare m_object as transient
   ROOT_SELECTION_NS::MemberAttributes< kTransient > m_object;
   /// Declare m_event as transient
   ROOT_SELECTION_NS::MemberAttributes< kTransient > m_event;

#endif // ROOT_VERSION

};
EXIT_ROOT_SELECTION_NS

/// Equality checker operator
template< typename STORABLE >
bool operator==( const DataLink< STORABLE >& a,
                 const DataLink< STORABLE >& b );

/// Non-equality checker operator
template< typename STORABLE >
bool operator!=( const DataLink< STORABLE >& a,
                 const DataLink< STORABLE >& b );

// Include the implementation:
#ifndef __GCCXML__
#   include "DataLink.icc"
#endif // __GCCXML__

#endif // ATHLINKS_DATALINK_H
