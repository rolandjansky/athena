/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBIDENTIFIER_IDENTIFIERHASHCALC_H
#define MUONCALIBIDENTIFIER_IDENTIFIERHASHCALC_H

/***************************************************************************
 * Identifier hashes
 * -----------------------------------------
 *
 * Author       : Martin Woudstra
 * Creation Date: 7 May 2004
 * Last Update  : 9 May 2005
 ***************************************************************************/

/** @class IdentifierHashCalc<T> 
* makes identifier hashes based on identifier fields using calculations. 
*/

#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <limits.h>
#include <iostream>
#include <iomanip>

template <class T>
class IdentifierHashCalc {
public:
   //
   // Nested types
   //
   /** N is the number of fields to use from the identifier
    *  NMAX is just to make sure we get unsigned int type of enum.
    */
   enum { NFIELDS = T::NFIELDS, NMAX=UINT_MAX };
   /** define the type IdFieldsType */
   typedef T IdFieldsType;
   /** define the type IdType */
   typedef typename T::IdentifierType IdType;
   /** define the type HashType */
   typedef typename T::HashType HashType;
   //
   // Member functions
   //
   /** Default constructor makes empty hash table */
   IdentifierHashCalc();
   /** Make empty table with pointer set to external idHelper, which will be
       transmitted to the constructor of class T. */
   explicit IdentifierHashCalc( const T& idFields );
   /** Clear the hashtable */
   void clear();
   /** Number of hashes in the table */
   unsigned int size() const;
   /** Check that the table is internally consistent. An empty table will return false. */
   bool checkValidity() const;
   /** Add an identifier to the table */
   bool addEntry( const IdType& id );
   /** Set the limits of a specific field */
   void setFieldLimits( unsigned int fieldIndex, int fieldMin, int fieldMax );
   /** Get hash from 0 to size()-1. Returns invalid hash if id is not in table. */
   HashType getHash( const IdType& id ) const;   
   /** Get identifier from hash. Returns invalid identifier if hash is out of range. */
   IdType getIdentifier( const HashType& idHash ) const;
   /** Dump complete table to output stream */
   void dump( std::ostream& os = std::cout ) const;
   /** Dump complete table into a string */
   std::string dumpToString() const;
private:
   bool isInRange( const T& idFields ) const;
   void update();
   int m_fieldsMin[NFIELDS];
   int m_fieldsMax[NFIELDS];
   unsigned int m_fieldsSize[NFIELDS];
   unsigned int m_fieldsFactor[NFIELDS];
   mutable T m_idFields;
};

template <class T>
inline IdentifierHashCalc<T>::IdentifierHashCalc() {
   clear();
}

template <class T>
inline IdentifierHashCalc<T>::IdentifierHashCalc( const T& idFields )
   : m_idFields(idFields)
{
   clear();
}

template <class T>
inline bool IdentifierHashCalc<T>::isInRange( const T& idFields ) const {
   for ( unsigned int i = 0; i < NFIELDS; ++i ) {
      if ( idFields[i] < m_fieldsMin[i] || idFields[i] > m_fieldsMax[i] ) return false;
   }
   return true;
}

template <class T>
typename IdentifierHashCalc<T>::HashType
IdentifierHashCalc<T>::getHash( const IdType& id ) const {
   m_idFields.setAll( id );
   if ( !isInRange( m_idFields ) ) return T::defaultHash();
   unsigned int hash = m_idFields[0] - m_fieldsMin[0];
   for ( unsigned int i = 1; i < NFIELDS; ++i ) {
      hash += m_fieldsFactor[i-1] * ( m_idFields[i] - m_fieldsMin[i] );
   }
   return HashType(hash);
}

template <class T>
typename IdentifierHashCalc<T>::IdType
IdentifierHashCalc<T>::getIdentifier( const HashType& idHash ) const {
   int fields[NFIELDS];
   fields[0] = m_fieldsMin[0] + ( idHash % m_fieldsSize[0] );
   for ( unsigned int i = 1; i < NFIELDS; ++i ) { 
      fields[i] = m_fieldsMin[i] + ( ( idHash / m_fieldsFactor[i-1] ) % m_fieldsSize[i] );
   }
   m_idFields.setAll( fields );
   if ( !isInRange( m_idFields ) ) return IdType();
   return m_idFields.getId();
}

template <class T>
void IdentifierHashCalc<T>::update() {
   // update sizes and field factors
   m_fieldsSize[0] = m_fieldsMax[0] - m_fieldsMin[0] + 1;
   m_fieldsFactor[0] = m_fieldsSize[0];   
   for ( unsigned int i = 1; i < NFIELDS; ++i ) {
      m_fieldsSize[i] = m_fieldsMax[i] - m_fieldsMin[i] + 1;
      m_fieldsFactor[i] = m_fieldsFactor[i-1] * m_fieldsSize[i];
   }
}

template <class T>
inline bool IdentifierHashCalc<T>::addEntry( const IdType& id ) {
   if ( !T::isValid( id ) ) return false;
   // set ranges
   m_idFields.setAll( id );
   bool bChanged = false;
   for ( unsigned int i = 0; i < NFIELDS; ++i ) {
      if ( m_idFields[i] < m_fieldsMin[i] ) {
	 m_fieldsMin[i] = m_idFields[i];
	 bChanged = true;
      }
      if ( m_idFields[i] > m_fieldsMax[i] ) {
	 m_fieldsMax[i] = m_idFields[i];
	 bChanged = true;
      }
   }
   if ( bChanged ) update();
   return true;
}

template <class T>
void IdentifierHashCalc<T>::setFieldLimits( unsigned int fieldIndex, int fieldMin, int fieldMax ) {
   if ( fieldIndex >= NFIELDS ) return;
   if ( fieldMin > fieldMax ) return;
   bool bChanged = false;
   if ( fieldMin != m_fieldsMin[fieldIndex] ) {
      m_fieldsMin[fieldIndex] = fieldMin;
      bChanged = true;
   }
   if ( fieldMax != m_fieldsMax[fieldIndex] ) {
      m_fieldsMax[fieldIndex] = fieldMax;
      bChanged = true;
   }
   if ( bChanged ) update();
}

template <class T>
void IdentifierHashCalc<T>::clear() {
   for ( unsigned int i = 0; i < NFIELDS; ++i ) {
      m_fieldsMin[i]    = INT_MAX;
      m_fieldsMax[i]    = INT_MIN;
      m_fieldsSize[i]   = 0;
      m_fieldsFactor[i] = 0;
   }
}

template <class T>
inline unsigned int IdentifierHashCalc<T>::size() const {
   return m_fieldsFactor[NFIELDS - 1];
}

template <class T>
bool IdentifierHashCalc<T>::checkValidity() const {
   // check that table is non-empty
   if ( !size() ) {
      MsgStream log(Athena::getMessageSvc(),"IdentifierHashCalc");
      log<<MSG::WARNING<<"IdentifierHashCalc<T>::checkValidity() Table is empty."<<endmsg;
      return false;
   }
   // check that tables are each other's inverse
   unsigned int nErrors = 0;
#ifdef IDENTIFIERHASHCALC_DEBUG
   MsgStream log(Athena::getMessageSvc(),"IdentifierHashCalc");
#endif
   for ( unsigned int i = 0; i < size(); ++i ) {
      HashType iHash(i);
      IdType id = getIdentifier( iHash );
      unsigned int hash = getHash( id );
      if ( hash	!= i ) {
#ifndef IDENTIFIERHASHCALC_DEBUG
         MsgStream log(Athena::getMessageSvc(),"IdentifierHashCalc");
#endif
         log<<MSG::WARNING<<"IdentifierHashCalc<T>::checkValidity() getIdentifier(" << i << ")=0x" << std::hex << id << std::dec << " whereas getHash(" << std::hex << id << std::dec << ")=" << hash<<endmsg;
         ++nErrors;
      } else {
#ifdef IDENTIFIERHASHCALC_DEBUG
         log<<MSG::DEBUG<<"hash=" << i << " <--> id=0x" << std::hex << id << std::dec << ": OK"<<endmsg;
#endif
      }
   }
#ifdef IDENTIFIERHASHCALC_DEBUG
   if ( nErrors ) {
      log<<MSG::WARNING<<"IdentifierHashCalc<T>::checkValidity() table contains " << nErrors << " errors." << endmsg;
   } else {
      log<<MSG::DEBUG<<"IdentifierHashCalc<T>::checkValidity(): table OK"<<endmsg;
   }
#endif
   
   if ( nErrors ) return false;
   
   // if we get here, everything is OK!
   return true;
}

template <class T>
void IdentifierHashCalc<T>::dump( std::ostream& os ) const {
   unsigned int n = size();
   for ( unsigned int i = 0; i < n; ++i ) {
      HashType iHash(i);
      IdType id = getIdentifier( iHash );
      m_idFields.setAll( id );
      os << "ID=0x" << std::hex << id << std::dec << " fields=(";
      for ( int j = NFIELDS-1; j >= 0; --j ) {
	 os << m_idFields[j];
	 if ( j != 0 ) os << ",";
      }
      os << ") hash=" << i << "\n";
   }
   os << "IdentifierHashTable has " << n << " entries.\n";
   os << "\nSummary:\n";
   os << "field min max size factor\n";
   for ( unsigned int i = 0; i < NFIELDS; ++i ) {
      os << std::setw(5) << i << " "
	 << std::setw(3) << m_fieldsMin[i] << " "
	 << std::setw(3) << m_fieldsMax[i] << " "
	 << std::setw(4) << m_fieldsSize[i] << " "
	 << std::setw(6) << m_fieldsFactor[i] << "\n";
   }
   os.flush();
}

template <class T>
inline std::string IdentifierHashCalc<T>::dumpToString() const {
   std::ostringstream oss;
   dump( oss );
   return oss.str();
}




#endif // MUONCALIBIDENTIFIER_IDENTIFIERHASHCALC_H
