/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 * Muon Calibration Identifiers
 * -----------------------------------------
 *
 * Author       : Martin Woudstra
 * Creation Date: 02 May 2005
 * Last Update  : 03 May 2005
 ***************************************************************************/

/** @class MdtRegion
* defines a calibration region through the list of the identifiers associated to it 
*/

#ifndef MUONCALIBIDENTIFIER_MDTREGION_H
# define MUONCALIBIDENTIFIER_MDTREGION_H
// std
#include <vector>

// other packages

// this package
//#include "MuonCalibIdentifier/MdtFixedIds.h"
//#include "MuonCalibIdentifier/MdtFixedHashes.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"


/** MdtRegionHash is generated from MdtRegionId with class IdentifierHashTable.
    Nothing but an unsigned int. Wrapper for initialisation, isValid() and
    function overloading. */
//CREATE_HASH_CLASS(MdtRegionHash)

/** define type MdtBasicRegionId for the smallest possible MDT calibration region */
typedef Identifier     MdtBasicRegionId;
/** define type MdtBasicRegionHash for the smallest possible MDT calibration region */
typedef IdentifierHash MdtBasicRegionHash;
/** define type MdtRegionHash */
typedef IdentifierHash MdtRegionHash;

class MdtRegion {
 public:
  /** define type IdVec, a vector of MdtBasicRegionId */
  typedef std::vector<MdtBasicRegionId> IdVec;
  /** define type const_iterator to loop over IdVec */
  typedef IdVec::const_iterator         const_iterator;
 public:
  /** Create a region, the region SHOULD be assigned a unique MdtRegionHash */
  explicit MdtRegion( const MdtRegionHash& id ) : m_hash(id) {};

  /** Generate a region identifier based on the multilayers in the region */
  MdtRegionHash getHash() const { return m_hash; }

  /** add identifier to region, returns false if identifier is already present */
  bool addIdentifier( const MdtBasicRegionId& id ) { m_idsInRegion.push_back( id ); return true; }

  /** iterator to access identifiers */
  const_iterator begin() const { return m_idsInRegion.begin(); }
  /** iterator to access identifiers */
  const_iterator end() const   { return m_idsInRegion.end(); }

 private:
  MdtRegionHash  m_hash;
  IdVec          m_idsInRegion;
};



/** @class MdtRegionSet
* A full set of regions, should normally cover the whole detector */

class MdtRegionSet : public std::vector<MdtRegion> {
};




#endif // MUONCALIBIDENTIFIER_MDTREGION_H
