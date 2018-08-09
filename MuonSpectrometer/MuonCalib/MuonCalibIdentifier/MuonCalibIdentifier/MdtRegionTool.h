/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 * Mdt Calibration
 * -----------------------------------------
 *
 * Author       : Martin Woudstra
 * Creation Date: 02 May 2005
 * Last Update  : 03 May 2005
 ***************************************************************************/

/** @class MdtRegionTool
* manages the access to the MdtRegion in the MdtRegionSet through hashes
*/ 

#ifndef MDTCALIBSVC_MDTREGIONTOOL_H
# define MDTCALIBSVC_MDTREGIONTOOL_H
// std

// other packages
#include "MuonCalibIdentifier/MdtRegion.h"
//#include "MuonCalibIdentifier/MdtHashTable.h"
// this package

class MdtIdHelper;

class MdtRegionTool {
public:
   /** default constructor */
   MdtRegionTool();
   /** intialize the vector of MdtRegionHash for a given MdtRegionSet */
   bool initialize( const MdtRegionSet& set, const MdtIdHelper* idHelper );
   /** clear both the MdtRegionSet and the vector of MdtRegionHash */ 
   void clear();

   /** Interface for superfast conversion of an MdtBasicRegionHash to an MdtRegionHash */
   MdtRegionHash getRegionHash( const MdtBasicRegionHash& ) const;

   /** Interface for superfast conversion of an MdtRegionHash to an MdtRegion, return zero if out off range */
   const MdtRegion* getRegion( const MdtRegionHash& hash ) const;

   /** returns number of regions */
   unsigned int numberOfRegions() const { return m_regions.size(); }

private:
   /** indexed with MdtBasicRegionHash */
   std::vector<MdtRegionHash> m_basicHashToRegionHash;
   MdtRegionSet               m_regions;
};

inline MdtRegionHash MdtRegionTool::getRegionHash( const MdtBasicRegionHash& aHash ) const {
   if ( aHash >= m_basicHashToRegionHash.size() ) return MdtRegionHash();
   return m_basicHashToRegionHash[ aHash ];
}

inline
const MdtRegion* MdtRegionTool::getRegion( const MdtRegionHash& hash ) const
{
  if ( hash >= m_regions.size() ) return 0;
  return &(m_regions[ hash ]);
}

#endif // MDTCALIBSVC_MDTREGIONTOOL_H
