/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 * Identifier Hashes
 * -----------------------------------------
 *
 * Author       : Martin Woudstra
 * Creation Date: 03 May 2004
 * Last Update  : 27 April 2005
 ***************************************************************************/

/** Helper base class for instantiation of templated class IdentifierHashTable */

#ifndef MUONCALIBIDENTIFIER_IDHASHTABLEHELPER_H
# define MUONCALIBIDENTIFIER_IDHASHTABLEHELPER_H
// std
#include <limits.h>
// other packages
// this package

template < unsigned int N, class ID, class HASH >
class IdHashTableHelper {
public:
   //
   // nested types 
   //
   /** N is the number of fields to use from the identifier.
    *  NMAX is just to make sure we get unsigned int type of enum. */
   enum { NFIELDS = N, NMAX=UINT_MAX };
   /** define type IdentifierType */
   typedef ID IdentifierType;
   /** define type HashType */
   typedef HASH HashType;
   /** needed for MultiDimArray<> instantiation */
   typedef HASH ValueType; 
   //
   // Member functions
   //
   /** Default constructor. Identifier and fields are set to invalid. */
   IdHashTableHelper();
   /** Constructor to initialise identifier. Fields are set to invalid. */
   explicit IdHashTableHelper( const IdentifierType& id );
   /** Constructor to initialise fields. Identifier is invalid. */
   explicit IdHashTableHelper( int fields[N] );
   /** virtual destructor needed because of derivation */
   virtual ~IdHashTableHelper();
   /** Get the identifier */
   const IdentifierType& getId() const;
   /** Set (only) the identifier */
   void setId( const IdentifierType& id );
   /** Return the value encoded in field. No range check! */
   int operator[]( unsigned int fieldIndex ) const;
   /** Set field at index to value. No range check! */
   void setField( int index, int value );
   /** Set fields from array. */
   void setFields( int fields[N] );
   /** Set the default (invalid) values for identifier and hash.
       Must be called (once per type T) if the default constructors
       of HashType or IdentifierType do not give a well defined (invalid!)
       value. This is typically the case for built-in types. */
   static void setDefaults( const IdentifierType& defId, const HashType& defHash );
   /* Return whether identifier is valid by comparing it with the default.
      Can be overloaded in derived class (probably not needed). */
   static bool isValid( const IdentifierType& );
   /** Returns default (=invalid) hash. */
   static HashType defaultHash();
   /** Return default (=invalid) identifier */
   static IdentifierType defaultIdentifier();
   /** Return default hash. Needed for class MultiDimArray. */
   static ValueType defaultValue();
private:
   //
   // Member data
   //
   IdentifierType m_id;
   int m_fields[N];     // the expanded identifier fields to use as indices
   //
   // Static members
   //
   static IdentifierType s_defaultId;
   static HashType       s_defaultHash;
};

template < unsigned int N, class ID, class HASH >
inline IdHashTableHelper<N,ID,HASH>::IdHashTableHelper() {
   for ( unsigned int i = 0; i < N; ++i ) m_fields[i] = INT_MAX;
}

template < unsigned int N, class ID, class HASH >
inline
IdHashTableHelper<N,ID,HASH>::IdHashTableHelper(
   const typename IdHashTableHelper<N,ID,HASH>::IdentifierType& id )
   : m_id(id)
{
   for ( unsigned int i = 0; i < N; ++i ) m_fields[i] = INT_MAX;
}

template < unsigned int N, class ID, class HASH >
inline IdHashTableHelper<N,ID,HASH>::IdHashTableHelper( int fields[N] ) {
   for ( unsigned int i = 0; i < N; ++i ) m_fields[i] = fields[i];
}

template < unsigned int N, class ID, class HASH >
inline IdHashTableHelper<N,ID,HASH>::~IdHashTableHelper() {
   // nothing to do
}

template < unsigned int N, class ID, class HASH >
inline int IdHashTableHelper<N,ID,HASH>::operator[]( unsigned int index ) const {
   return m_fields[index];
}

template < unsigned int N, class ID, class HASH >
inline const typename IdHashTableHelper<N,ID,HASH>::IdentifierType&
IdHashTableHelper<N,ID,HASH>::getId() const {
   return m_id;
}

template < unsigned int N, class ID, class HASH >
inline void
IdHashTableHelper<N,ID,HASH>::setId( const typename IdHashTableHelper<N,ID,HASH>::IdentifierType& id ) {
   m_id = id;
}

template < unsigned int N, class ID, class HASH >
inline void IdHashTableHelper<N,ID,HASH>::setField( int index, int value ) {
   m_fields[index] = value;
}

template < unsigned int N, class ID, class HASH >
inline void IdHashTableHelper<N,ID,HASH>::setFields( int fields[N] ) {
   for ( unsigned int i = 0; i < N; ++i ) m_fields[i] = fields[i];
}

template < unsigned int N, class ID, class HASH >
inline void IdHashTableHelper<N,ID,HASH>::setDefaults(
   const typename IdHashTableHelper<N,ID,HASH>::IdentifierType& defId,
   const typename IdHashTableHelper<N,ID,HASH>::HashType& defHash ) {
   s_defaultId   = defId;
   s_defaultHash = defHash;
}

template < unsigned int N, class ID, class HASH >
inline typename IdHashTableHelper<N,ID,HASH>::HashType
IdHashTableHelper<N,ID,HASH>::defaultHash() {
   return s_defaultHash;
}

template < unsigned int N, class ID, class HASH >
inline typename IdHashTableHelper<N,ID,HASH>::IdentifierType
IdHashTableHelper<N,ID,HASH>::defaultIdentifier() {
   return s_defaultId;
}

template < unsigned int N, class ID, class HASH >
inline typename IdHashTableHelper<N,ID,HASH>::ValueType
IdHashTableHelper<N,ID,HASH>::defaultValue() {
   return defaultHash();
}

template < unsigned int N, class ID, class HASH >
inline bool IdHashTableHelper<N,ID,HASH>::isValid( const IdentifierType& id ) {
  return id != s_defaultId;
}

//
// static members
//
template < unsigned int N, class ID, class HASH > 
typename IdHashTableHelper<N,ID,HASH>::IdentifierType 
IdHashTableHelper<N,ID,HASH>::s_defaultId =
typename IdHashTableHelper<N,ID,HASH>::IdentifierType();

template < unsigned int N, class ID, class HASH >
typename IdHashTableHelper<N,ID,HASH>::HashType 
IdHashTableHelper<N,ID,HASH>::s_defaultHash =
typename IdHashTableHelper<N,ID,HASH>::HashType();

#endif // MUONCALIBIDENTIFIER_IDHASHTABLEHELPER_H
