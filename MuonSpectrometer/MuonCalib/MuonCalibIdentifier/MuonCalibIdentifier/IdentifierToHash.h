/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBIDENTIFIER_IDENTIFIERTOHASH_H
#define MUONCALIBIDENTIFIER_IDENTIFIERTOHASH_H

/***************************************************************************
 * Identifier utility
 * -----------------------------------------
 *
 * Author       : Martin Woudstra
 * Creation Date: 28 April 2004
 * Last Update  : 27 April 2005
 ***************************************************************************/

#include "MuonCalibIdentifier/MultiDimArray.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <iostream>
#include <sstream>
#include <string>
#include <limits.h>

/**
 * The IdentifierToHash table.
 * It uses a multi-dimensional array and identifier fields for fast access of
 * Identifier-to-hash conversions.
 * For hash-to-Identifier conversion it uses a one-dimensional array of Identifiers.
 *
 * The template parameter is a helper class which is used to extract the fields
 * from the identifier. This helper class must derive from class IdHashTableHelper,
 * which is again a templated class, and also serves as a traits class
 * (i.e. supplies some type definitions + number of fields).
 * 
 */
template <class T>
class IdentifierToHash {
public:
   //
   // nested types
   //
   /** N is the number of fields to use from the identifier
    *  NMAX is just to make sure we get unsigned int type of enum.
    */
   enum { NFIELDS = T::NFIELDS, NMAX=UINT_MAX };
   /** define type IdType */
   typedef typename T::IdentifierType IdType;
   /** define type HashType */
   typedef typename T::HashType HashType;
   /** define type ArrayType */
   typedef MultiDimArray<T,NFIELDS> ArrayType;
   /** Helper function to get a properly initialised hash */
   static HashType defaultHashValue() {
      return T::defaultHash();
   }
   /**
    * Helper class for IdentifierToHash to automate recursive index call
    */
   template < unsigned int N, class K >
   class RecursiveIndexCall {
   public:
      static const typename K::ValueType& getConstRef( const K& obj, const T& indices ) {
	 int idx = indices[N-1];
#ifdef IDENTIFIERTOHASH_DEBUG
    MsgStream log(Athena::getMessageSvc(),"RecursiveIndexCall");
    log<<MSG::DEBUG<<"[" << idx << "]"<<endmsg;
#endif
	 return RecursiveIndexCall<N-1,typename K::SubType>::getConstRef( obj[ idx ], indices );
      }
      static typename K::ValueType& getRef( K& obj, const T& indices ) {
	 int idx = indices[N-1];
#ifdef IDENTIFIERTOHASH_DEBUG
    log<<MSG::DEBUG<<"{" << idx << "}"<<endmsg;
#endif
	 return RecursiveIndexCall<N-1,typename K::SubType>::getRef( obj[ idx ], indices );
      }
   };
   
   // specialise for N=1 to end recursion
   template < class K >
   class RecursiveIndexCall<1,K> {
   public:
      static const typename K::ValueType& getConstRef( const K& obj, const T& indices ) {
	 int idx = indices[0];
#ifdef IDENTIFIERTOHASH_DEBUG
    MsgStream log(Athena::getMessageSvc(),"RecursiveIndexCall");
    log<<MSG::DEBUG<<"[" << idx << "]"<<endmsg;
#endif
	 return obj[ idx ];
      }
      static typename K::ValueType& getRef( K& obj, const T& indices ) {
	 int idx = indices[0];
#ifdef IDENTIFIERTOHASH_DEBUG
    log<<MSG::DEBUG<<"{" << idx << "}"<<endmsg;
#endif
	 return obj[ idx ];
      }
   };
   
   // zero is invalid. Make empty class.
   template < class K >
   class RecursiveIndexCall<0,K> {
   };
   /**
    *  Helper class for one-go recursive range check on all indices
    */
   template < unsigned int N, class K>
   class RecursiveRangeCheck {
   public:
      static bool isInRange( const K& obj, const T& indices ) {
	 int idx = indices[N-1];
	 bool bOK = obj.isInRange( idx );
#ifdef IDENTIFIERTOHASH_DEBUG
    MsgStream log(Athena::getMessageSvc(),"RecursiveRangeCheck");
    log<<MSG::DEBUG<<"(" << idx << ")"<<endmsg;
	 if ( !bOK ) {
      log<<MSG::DEBUG<<" CHECK: " << idx << " Out of range (" << obj.minIndex() << "," << obj.maxIndex() << ")"<<endmsg;
	 } 
#endif
	 return bOK && RecursiveRangeCheck<N-1,typename K::SubType>::isInRange( obj[ idx ], indices );
      }
	 
   };
   /** Specialise for N=1 to end recursion */
   template <class K>
   class RecursiveRangeCheck<1,K> {
   public:
      static bool isInRange( const K& obj, const T& indices ) {
	 int idx = indices[0];
	 bool bOK = obj.isInRange( idx );
#ifdef IDENTIFIERTOHASH_DEBUG
    MsgStream log(Athena::getMessageSvc(),"RecursiveRangeCheck");
    log<<MSG::DEBUG<<"(" << idx << ")"<<endmsg;
	 if ( !bOK ) {
      log<<MSG::DEBUG<<" CHECK: Out of range (" << obj.minIndex() << "," << obj.maxIndex() << ")"<<endmsg;
	 }else {
      log<<MSG::DEBUG<<" CHECK: Range OK"<<endmsg;
	 }
#endif
	 return bOK;
      }
   };
   /** N=0 is invalid */
   template <class K>
   class RecursiveRangeCheck<0,K> {
      // empty class
   };
   //
   // Member functions
   //
   /** Default constructor makes empty hash table and creates default idFields object. */
   IdentifierToHash();
   /** Make empty table and initialise with idFields object. */
   explicit IdentifierToHash( const T& idFields );
   /** Clear the hashtable */
   void clear();
   /** Number of valid hashes in the table */
   unsigned int size() const;
   /** Total number of hashes in the table */
   unsigned int totalSize() const;
   /** Add an identifier to the table, and return the hash value belonging to it.
       Hash values start at zero, and increase by 1 at every call to addEntry().
       Returns defaultHashValue() if id is invalid or was already present. */
   HashType addEntry( const IdType& id );
   /** Add an identifier+hash pair to the table. Returns true if the pair was added.
       Multiple times the same id is not allowed, but multiple times the same
       hash is allowed. It is therefore the responsibility of the caller to make
       sure that the hashes are unique (if so desired). */
   bool addEntry( const IdType& id, const HashType& aHash );
   /** Get hash from 0 to size()-1. Returns invalid hash if id is not in table. */
   HashType getHash( const IdType& id ) const;
   /** Dump one entry for given id to os */
   void dumpOneEntry( const IdType& id, std::ostream& os ) const;
   /** Dump one entry for given id to a string */
   std::string dumpOneEntryToString( const IdType& id ) const;
   /** Dump complete table to output stream */
   void dump( std::ostream& os = std::cout ) const;
   /** Dump complete table into a string */
   std::string dumpToString() const;
private:
   mutable T m_idFields;
   unsigned int m_entries;
   ArrayType m_data;
};

template <class T>
inline IdentifierToHash<T>::IdentifierToHash()
   : m_entries(0)
{}

template <class T>
inline IdentifierToHash<T>::IdentifierToHash( const T& idFields )
   : m_idFields( idFields ), m_entries(0)
{}

template <class T>
bool
IdentifierToHash<T>::addEntry( const typename IdentifierToHash<T>::IdType& id,
			       const typename IdentifierToHash<T>::HashType& aHash ) {
#ifdef IDENTIFIERTOHASH_DEBUG
   MsgStream log(Athena::getMessageSvc(),"IdentifierToHash<T>");
   log<<MSG::DEBUG<<"IdentifierToHash<T>::addEntry(0x" << std::hex << id << std::dec << "," << aHash << ")"<<endmsg;
#endif
   if ( !T::isValid( id ) ) {
#ifdef IDENTIFIERTOHASH_DEBUG
   log<<MSG::DEBUG<<": id is invalid. Nothing added."<<endmsg;
#endif
      return false;
   }
   m_idFields.setAll( id );
#ifdef 
   log<<MSG::DEBUG<<"::IdToHashTable"<<endmsg;
#endif
   
   HashType& hashRef =
      RecursiveIndexCall< NFIELDS, ArrayType >::getRef( m_data, m_idFields );
   // only add hash if entry is not yet taken
   if ( hashRef != defaultHashValue() ) {
#ifdef IDENTIFIERTOHASH_DEBUG
      log<<MSG::WARNING<<"id already taken by hash=" << hashRef << ". Nothing added."<<endmsg;
#endif
      return false;
   } else {
      // set the hash to the indicated value
      hashRef = aHash;
#ifdef IDENTIFIERTOHASH_DEBUG
      log<<MSG::DEBUG<<" ADDED."<<endmsg;
#endif
   }

   ++m_entries;
   return true;
}

template <class T>
typename IdentifierToHash<T>::HashType
IdentifierToHash<T>::addEntry( const typename IdentifierToHash<T>::IdType& id ) {
   HashType aHash = m_entries;
   if ( !addEntry( id, aHash ) ) return defaultHashValue();
   return aHash;
}

template <class T>
inline typename IdentifierToHash<T>::HashType
IdentifierToHash<T>::getHash( const typename IdentifierToHash<T>::IdType& id ) const {
   if ( !T::isValid( id ) ) return defaultHashValue();
   m_idFields.setAll( id );
   // check that fields are in range
   if ( !RecursiveRangeCheck< NFIELDS, ArrayType >::isInRange( m_data, m_idFields ) ) {
      return defaultHashValue();
   }
   return RecursiveIndexCall< NFIELDS, ArrayType >::getConstRef( m_data, m_idFields );
}

template <class T>
inline void IdentifierToHash<T>::clear() {
   m_entries = 0;
   m_data.clear();
}

template <class T>
inline unsigned int IdentifierToHash<T>::size() const {
   return m_entries;
}


template <class T>
inline unsigned int IdentifierToHash<T>::totalSize() const {
   return m_data.totalSize();
}

template <class T>
inline void IdentifierToHash<T>::dumpOneEntry( const typename IdentifierToHash<T>::IdType& id,
					       std::ostream& os ) const {
   m_idFields.setAll( id );
   m_data.dumpOneEntry( m_idFields, os );
}

template <class T>
inline std::string IdentifierToHash<T>::dumpOneEntryToString( const typename IdentifierToHash<T>::IdType& id ) const {
   m_idFields.setAll( id );
   return m_data.dumpOneEntryToString( m_idFields );
}

template <class T>
inline void IdentifierToHash<T>::dump( std::ostream& os ) const {
   m_data.dump( os );
}

template <class T>
inline std::string IdentifierToHash<T>::dumpToString() const {
   return m_data.dumpToString();
}


#endif // MUONCALIBIDENTIFIER_IDENTIFIERTOHASH_H
