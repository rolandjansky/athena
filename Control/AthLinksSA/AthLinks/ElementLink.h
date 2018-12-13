// Dear emacs, this is -*- c++ -*-
/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */
#ifndef ATHLINKS_ELEMENTLINK_H
#define ATHLINKS_ELEMENTLINK_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <string>

// Local include(s):
#include "AthLinks/ElementLinkBase.h"
#include "AthLinks/tools/selection_ns.h"
#include "AthLinks/tools/TypeTools.h"

// Forward declaration(s):
ENTER_ROOT_SELECTION_NS
template< typename STORABLE >
class ElementLink;
EXIT_ROOT_SELECTION_NS
namespace xAOD {
   class TVirtualEvent;
}

/// @short ElementLink implementation for ROOT usage
///
/// This class follows the interface of the offline implementation of
/// ElementLink as closely as reasonably possible. Its functions are
/// supposed to behave the same way as the functions of the offline
/// ElementLink class.
///
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
///
/// $Revision: 649074 $
/// $Date: 2015-02-23 17:51:40 +0100 (Mon, 23 Feb 2015) $
///
template< typename STORABLE >
class ElementLink : public ElementLinkBase {

public:
   /// @name Type definitions taken from the StoreGate-based implementation
   /// @{

   /// Type of the "regular" identifiers in the event
   typedef std::string ID_type;
   /// Integer identifier for the object in the event
   typedef uint32_t sgkey_t;

   /// Constant reference to the container
   typedef const STORABLE& BaseConstReference;
   /// Constant pointer to the container
   typedef const STORABLE* BaseConstPointer;

   /// Type of the index used on the container
   typedef size_t index_type;

   /// Type of the values inside the container
   typedef typename STORABLE::value_type value_type;
   /// Constant iterator type for the container
   typedef typename STORABLE::const_iterator const_iterator;

   /// Type of the elements in the container
   typedef typename TypeTools::Copy< value_type >::type ElementType;
   /// Constant reference to an element in the container
   typedef typename TypeTools::Copy< value_type >::const_reference
      ElementConstReference;
   /// Constant pointer to an element in the container
   typedef typename TypeTools::Copy< value_type >::const_pointer
      ElementConstPointer;

   /// @}

   /// @name Constructors
   /// @{

   /// Default constructor
   ElementLink();
   /// Constructor setting up the link with a specific event object
   ElementLink( xAOD::TVirtualEvent* event );

   /// Construct object from storable key and element index (fast)
   ElementLink( const ID_type& key, index_type index,
                xAOD::TVirtualEvent* event = 0 );
   /// Construct object from storable id and element index (fastest)
   ElementLink( sgkey_t id, index_type index,
                xAOD::TVirtualEvent* event = 0 );

   /// Construct from a storable key and element index, taking the element
   /// object from the user. Be careful, no checks are done on the received
   /// object!
   ElementLink( const ID_type& key, index_type index, ElementType element,
                xAOD::TVirtualEvent* event = 0 );
   /// Construct from a storable id and element index, taking the element
   /// object from the user. Be careful, no checks are done on the received
   /// object!
   ElementLink( sgkey_t id, index_type index, ElementType element,
                xAOD::TVirtualEvent* event = 0 );

   /// Construct from a container reference and element index (slower)
   ElementLink( BaseConstReference container, index_type index,
                xAOD::TVirtualEvent* event = 0 );
   /// Construct from a container reference and element pointer (slowest)
   ElementLink( ElementType element, BaseConstReference container,
                xAOD::TVirtualEvent* event = 0 );

   /// Copy-constructor
   ElementLink( const ElementLink& parent );

   /// @}

   /// Assignment operator
   ElementLink& operator=( const ElementLink& rhs );

   /// @name Setter functions
   /// @{

   /// Set link to point to given index (slower)
   bool toIndexedElement( BaseConstReference container, index_type index );
   /// Set link to point to given element (slowest)
   bool toContainedElement( BaseConstReference container, ElementType element );

   /// Set link to point to an Element (slowest)
   bool setElement( ElementType element );
   /// Set link storable to data object pointed by data (slower)
   bool setStorableObject( BaseConstReference data, bool replace = false );

   /// Reset with storable key and element index (fast)
   void resetWithKeyAndIndex( const ID_type& key, index_type index,
                              xAOD::TVirtualEvent* event = 0 );
   /// Reset with storable id and element index (fastest)
   void resetWithKeyAndIndex( sgkey_t id, index_type index,
                              xAOD::TVirtualEvent* event = 0 );
   /// Reset to default state 
   void reset();

   /// @}

   /// @name Accessor functions/operators
   /// @{

   /// Retrieve storable object pointer
   BaseConstPointer getStorableObjectPointer() const;
   /// Retrieve storable object reference
   BaseConstReference getStorableObjectRef() const;

   /// Get the index of the element inside of its container
   index_type index() const { return persIndex(); }

   /// Get the key that we reference, as a string
   const ID_type& dataID() const;
   /// Get the key that we reference, as a hash
   sgkey_t key() const { return persKey(); }

   /// Comparison operator
   bool operator==( const ElementLink& rhs ) const;
   /// Comparison operator
   bool operator!=( const ElementLink& rhs ) const;

   /// Get a reference to the object in question
   ElementConstReference operator*() const;

   /// Access the object as a smart pointer
   ElementConstPointer operator->() const { return cptr(); }
   /// Auto-convert to the element type
   operator ElementConstPointer () const { return cptr(); }
   /// Get a constant pointer to the referenced object
   ElementConstPointer cptr() const;

   /// Check if the element can be found
   bool isValid() const;
   /// Operator checking the validity of the link
   bool operator!() const { return ! isValid(); }

   /// Access the currently cached element. Doesn't trigger lookup.
   ElementType cachedElement() const { return m_element; }
   /// Check if the link has a cached element pointer
   bool hasCachedElement() const { return m_elementCached; }

   /// @}

   /// Get the data source for the reference
   xAOD::TVirtualEvent* source() const { return m_event; }

   /// Dummy function provinding the offline interface
   bool toPersistent() { return true; }

private:
   /// Reset the object's internal cache
   virtual bool toTransient();
   /// Find the identifier of the object being pointed to
   void findIdentifier();
   /// Find the index of an element inside the container
   void findIndex();

   /// Cached pointer to the container
   mutable BaseConstPointer m_container;
   /// Cached pointer to the object
   mutable ElementType m_element;
   /// Status of the cached pointer
   mutable bool m_elementCached;
   /// Pointer to the event object
   xAOD::TVirtualEvent* m_event;

   /// Needed to take the Reflex/Cling specifications into account
   typedef typename
   ROOT_SELECTION_NS::ElementLink< STORABLE >::self DictSel;

}; // class ElementLink

/*
 * The following piece of code declares all members of the ElementLink
 * class as transient.
 */
ENTER_ROOT_SELECTION_NS
template< typename STORABLE >
class ElementLink
#if ROOT_VERSION_CODE >= ROOT_VERSION( 6, 0, 2 )
   : public SelectNoInstance
#endif // ROOT_VERSION
{

public:
   /// Helper definition
   typedef ElementLink< STORABLE > self;

#if ROOT_VERSION_CODE < ROOT_VERSION( 5, 99, 0 )

   /// Don't generate such dictionaries automatically
   ROOT_SELECTION_NS::NO_SELF_AUTOSELECT dummy;

   /// Mark all transient members:
   ROOT_SELECTION_NS::TRANSIENT m_container;
   ROOT_SELECTION_NS::TRANSIENT m_element;
   ROOT_SELECTION_NS::TRANSIENT m_elementCached;
   ROOT_SELECTION_NS::TRANSIENT m_event;

#else

   /// Mark all transient members:
   ROOT_SELECTION_NS::MemberAttributes< kTransient > m_container;
   ROOT_SELECTION_NS::MemberAttributes< kTransient > m_element;
   ROOT_SELECTION_NS::MemberAttributes< kTransient > m_elementCached;
   ROOT_SELECTION_NS::MemberAttributes< kTransient > m_event;

#endif // ROOT_VERSION

};
EXIT_ROOT_SELECTION_NS

// Include the implementation:
#ifndef __GCCXML__
#   include "ElementLink.icc"
#endif // __GCCXML__

#endif // ATHLINKS_ELEMENTLINK_H
