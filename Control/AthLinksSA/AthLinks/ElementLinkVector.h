// Dear emacs, this is -*- c++ -*-
/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */
#ifndef ATHLINKS_ELEMENTLINKVECTOR_H
#define ATHLINKS_ELEMENTLINKVECTOR_H

// System include(s):
#include <vector>

// Local include(s):
#include "AthLinks/ElementLink.h"
#include "AthLinks/ElementLinkVectorBase.h"
#include "AthLinks/tools/selection_ns.h"

// Forward declaration(s):
ENTER_ROOT_SELECTION_NS
template< class CONTAINER >
class ElementLinkVector;
EXIT_ROOT_SELECTION_NS

/**
 *  @short ElementLinkVector implementation for standalone ROOT
 *
 *         This class provides part of the interface provided by the offline
 *         implementation of ElementLinkVector. Just enough to make the code
 *         meaningfully usable in ROOT. But the plan is not to provide the
 *         full flexibility of the offline class.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 649074 $
 * $Date: 2015-02-23 17:51:40 +0100 (Mon, 23 Feb 2015) $
 */
template< class CONTAINER >
class ElementLinkVector : public ElementLinkVectorBase {

public:
   /// Definition of the current type
   typedef ElementLinkVector< CONTAINER > ElemLinkVec;
   /// Type of the EL that we have a vector of
   typedef ElementLink< CONTAINER > ElemLink;

   /// @name STL-like type definitions
   /// @{

   /// Standard typedef for the type of the held objects
   typedef ElemLink value_type;
   /// Type of the reference pointing to one element link
   typedef ElemLink& reference;
   /// Type of the constant reference pointing to one element link
   typedef const ElemLink& const_reference;
   /// Type of the iterator for traversing the vector
   typedef typename std::vector< ElemLink >::iterator iterator;
   /// Type of the constant iterator for traversing the vector
   typedef typename std::vector< ElemLink >::const_iterator const_iterator;
   /// Type of the reverse iterator for traversing the vector
   typedef typename std::vector< ElemLink >::reverse_iterator reverse_iterator;
   /// Type of the constant reverse iterator for traversing the vector
   typedef typename std::vector< ElemLink >::const_reverse_iterator
      const_reverse_iterator;
   /// Type of the size of the vector
   typedef typename std::vector< ElemLink >::size_type size_type;

   /// @}

   /// @name Constructors
   /// @{

   /// Default constructor
   ElementLinkVector();
   /// Constructor creating a vector of a given size
   ElementLinkVector( size_type n, const ElemLink& link = ElemLink() );
   /// Copy constructor
   ElementLinkVector( const ElemLinkVec& parent );

   /// @}

   /// Assignment operator
   ElemLinkVec& operator=( const ElemLinkVec& rhs );

   /// Comparison operator
   bool operator==( const ElementLinkVector& rhs ) const;
   /// Comparison operator
   bool operator!=( const ElementLinkVector& rhs ) const;   

   /// @name Vector iterator functions
   /// @{

   /// Get an iterator pointing at the first element
   iterator begin() { return m_elVec.begin(); }
   /// Get a constant iterator pointing at the first element
   const_iterator begin() const { return m_elVec.begin(); }

   /// Get an iterator pointing at the end of the container
   iterator end() { return m_elVec.end(); }
   /// Get a constant iterator pointing at the end of the container
   const_iterator end() const { return m_elVec.end(); }

   /// Get a reverse iterator pointing at the last element
   reverse_iterator rbegin() { return m_elVec.rbegin(); }
   /// Get a constant reverse iterator pointing at the last element
   const_reverse_iterator rbegin() const { return m_elVec.rbegin(); }

   /// Get a reverse iterator pointing at the beginning of the container
   reverse_iterator rend() { return m_elVec.rend(); }
   /// Get a constant reverse iterator pointing at the beginning of the container
   const_reverse_iterator rend() const { return m_elVec.rend(); }

   /// @}

   /// @name Vector capacity functions
   /// @{

   /// Get the size of the vector
   size_type size() const { return m_elVec.size(); }
   /// Get the maximal size of the object
   size_type max_size() const { return m_elVec.max_size(); }
   /// Get the capacity of the currently allocated object
   size_type capacity() const { return m_elVec.capacity(); }
   /// Resize the vector to the specified size
   void resize( size_type sz, const ElemLink& link = ElemLink() );
   /// Reserve some slots in the vector
   void reserve( size_type sz ) { m_elVec.reserve( sz ); }
   /// Function checking if the vector is empty
   bool empty() const { return ( size() == 0 ); }

   /// @}

   /// @name Vector element accessors
   /// @{

   /// Get the element at a given index
   const_reference operator[]( size_type n ) const { return m_elVec[ n ]; }
   /// Get the element at a given index (with size check)
   const_reference at( size_type n ) const { return m_elVec.at( n ); }
   /// Get the first element
   const_reference front() const { return m_elVec.front(); }
   /// Get the last element
   const_reference back() const { return m_elVec.back(); }

   /// @}

   /// @name Vector modifiers
   /// @{

   /// Add a new element link at the end of the vector
   void push_back( const ElemLink& el );
   /// Remove the last element from the vector
   void pop_back();

   /// Erase one element from the vector
   iterator erase( iterator position );
   /// Erase a set of elements from the vector
   iterator erase( iterator first, iterator last );

   /// Swap the container's contents with another object's
   void swap( ElemLinkVec& vec );

   /// Fill the vector with the contents of another container
   template< class InputIterator >
   void assign( InputIterator first, InputIterator last );
   /// Fill the vector with N identical links
   void assign( size_type n, const ElemLink& link );

   /// Insert a link in a certain position
   iterator insert( iterator position, const ElemLink& link );
   /// Insert a number of identical links in a certain position
   void insert( iterator position, size_type n, const ElemLink& link );

   /// Clear the object
   void clear();

   /// @}

   /// Reset the object's internal cache
   virtual bool toTransient();
   /// Function copying the transient data into the persistent variables
   bool toPersistent();

private:
   /// Transient vector of element links
   std::vector< ElemLink > m_elVec;

   /// Needed to take the Reflex/Cling specifications into account
   typedef typename
   ROOT_SELECTION_NS::ElementLinkVector< CONTAINER >::self DictSel;

}; // class ElementLinkVector

/*
 * This code declares that the variables of ElementLinkVector
 * are supposed to be transient in all instantiations.
 */
ENTER_ROOT_SELECTION_NS
template< class CONTAINER >
class ElementLinkVector
#if ROOT_VERSION_CODE >= ROOT_VERSION( 6, 0, 2 )
   : public SelectNoInstance
#endif // ROOT_VERSION
{

public:
   /// Helper definition
   typedef ElementLinkVector< CONTAINER > self;

#if ROOT_VERSION_CODE < ROOT_VERSION( 5, 99, 0 )

   /// Don't generate such dictionaries automatically
   ROOT_SELECTION_NS::NO_SELF_AUTOSELECT dummy;

   /// Mark all transient members:
   ROOT_SELECTION_NS::TRANSIENT m_elVec;

#else

   /// Mark all transient members:
   ROOT_SELECTION_NS::MemberAttributes< kTransient > m_elVec;

#endif // ROOT_VERSION

};
EXIT_ROOT_SELECTION_NS

// Include the implementation:
#ifndef __GCCXML__
#   include "AthLinks/ElementLinkVector.icc"
#endif // not __GCCXML__

#endif // ATHLINKS_ELEMENTLINKVECTOR_H
