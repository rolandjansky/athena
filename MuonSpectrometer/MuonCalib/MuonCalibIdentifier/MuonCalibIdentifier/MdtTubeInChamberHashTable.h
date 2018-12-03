/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 * Mdt Calibration Data Model
 * -----------------------------------------
 *
 * Author       : Martin Woudstra
 * Creation Date: 5 April 2004
 * Last Update  : 9 May 2005
 ***************************************************************************/

/** class for making an MDT tube hash within one chamber. */

#ifndef MUONCALIBIDENTIFIERS_MDTTUBEINCHAMBERHASHTABLE_H
# define MUONCALIBIDENTIFIERS_MDTTUBEINCHAMBERHASHTABLE_H
// std

// other packages

// this package
#include "MuonCalibIdentifier/MdtFixedIds.h"
#include "MuonCalibIdentifier/MdtFixedHashes.h"
#include "MuonCalibIdentifier/MdtTubeInChamberHashTableType.h"

class MdtTubeInChamberHashTable {
public:
   /** 'less' helper class for binary search on global offset */
   struct lessGlobalOffset :
      public std::binary_function<MdtTubeInChamberHashTable,unsigned int, bool> {
      bool operator()( const MdtTubeInChamberHashTable& table, unsigned int offset ) {
	 return table.m_globalHashOffset < offset;
      }
   };
   /** constructor */
   MdtTubeInChamberHashTable( const MdtTubeInChamberHashTableType* type,
			      const MdtChamberId& chamberId );
   /** constructor */
   MdtTubeInChamberHashTable( const MdtTubeInChamberHashTableType* type,
			      const MdtChamberId& chamberId, unsigned int globalHashOffset );
   /** Clear the hashtable. */
   void clear();
   /** Set global hash offset to offset */
   void setGlobalHashOffset( unsigned int offset );
   /** Return the chamber identifier */
   MdtChamberId chamberId() const;
   /** Return the number of multilayers in the chamber */
   unsigned int nMultilayers() const;
   /** Return the number of layers per multilayer */
   unsigned int nLayers() const;
   /** Return the number of tubes per layer */
   unsigned int nTubes() const;
   /** Return the total number of tubes in the chamber */
   unsigned int nTotalTubes() const;
   /** Return the hash-within-chamber from 0 to numberOfTubes()-1. */
   MdtTubeHash localHash( const MdtTubeId& id ) const;
   /** Return the global tube hash. Meaning depends on context. */
   MdtTubeHash globalHash( const MdtTubeId& id ) const;
   /** Returns the maximum local hash value plus 1. The minimum local hash value is zero. */
   unsigned int localHashMax() const;
   /** Return minimum global hash for this chamber. It is equal to the global hash offset
       of this chamber. */
   unsigned int globalHashMin() const;
   /** Return maximum global hash plus 1 for this chamber. It is equal to the global hash offset
       plus localHashMax() */
   unsigned int globalHashMax() const;
   /** Get (full) tube identifier from local hash */
   MdtTubeId tubeIdFromLocalHash( const MdtTubeHash& localHash ) const;
   /** Get (full) tube identifier from global hash */
   MdtTubeId tubeIdFromGlobalHash( const MdtTubeHash& globalHash ) const;
private:
   const MdtTubeInChamberHashTableType* m_localHashTable;
   MdtChamberId m_chamberId;
   unsigned int m_globalHashOffset;
};

inline MdtTubeInChamberHashTable::MdtTubeInChamberHashTable(
   const MdtTubeInChamberHashTableType* type, const MdtChamberId& id )
   : m_localHashTable(type), m_chamberId(id), m_globalHashOffset(0)
{}

inline MdtTubeInChamberHashTable::MdtTubeInChamberHashTable(
   const MdtTubeInChamberHashTableType* type, const MdtChamberId& id, unsigned int offset )
   : m_localHashTable(type), m_chamberId(id), m_globalHashOffset(offset)
{}

inline MdtChamberId MdtTubeInChamberHashTable::chamberId() const {
   return m_chamberId; 
}

inline unsigned int MdtTubeInChamberHashTable::localHashMax() const {
   return m_localHashTable->size();
}

inline MdtTubeHash MdtTubeInChamberHashTable::localHash( const MdtTubeId& id ) const {
   return m_localHashTable->getHash( id );
}

inline unsigned int MdtTubeInChamberHashTable::globalHashMin() const {
   return m_globalHashOffset;
}

inline unsigned int MdtTubeInChamberHashTable::globalHashMax() const {
   return m_globalHashOffset + localHashMax();
}

inline unsigned int MdtTubeInChamberHashTable::nMultilayers() const {
   return m_localHashTable->nMultilayers();
}

inline unsigned int MdtTubeInChamberHashTable::nLayers() const {
   return m_localHashTable->nLayers();
}

inline unsigned int MdtTubeInChamberHashTable::nTubes() const {
   return m_localHashTable->nTubes();
}

inline unsigned int MdtTubeInChamberHashTable::nTotalTubes() const {
   return m_localHashTable->nTotalTubes();
}

inline MdtTubeHash
MdtTubeInChamberHashTable::globalHash( const MdtTubeId& id ) const {
   return MdtTubeHash( m_globalHashOffset + localHash( id ) );
}

inline MdtTubeId
MdtTubeInChamberHashTable::tubeIdFromLocalHash( const MdtTubeHash& locHash ) const {
   return MdtTubeId( m_chamberId, m_localHashTable->getIdentifier( locHash ) );
}

inline MdtTubeId
MdtTubeInChamberHashTable::tubeIdFromGlobalHash( const MdtTubeHash& globHash ) const {
   return tubeIdFromLocalHash( MdtTubeHash( globHash - globalHashMin() ) );
}


#endif // MUONCALIBIDENTIFIERS_MDTTUBEINCHAMBERHASHTABLE_H
