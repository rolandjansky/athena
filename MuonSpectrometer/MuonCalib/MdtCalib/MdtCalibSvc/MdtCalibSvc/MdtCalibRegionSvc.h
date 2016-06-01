/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_MDTCALIBREGIONSVC_H
#define MUONCALIB_MDTCALIBREGIONSVC_H

#include <vector>

#include "Identifier/Identifier.h"
#include "MuonCalibIdentifier/MdtRegionTool.h"
#include "MuonCalibIdentifier/MdtRegion.h"

/** 
   OBSOLETE, TO BE USED ONLY FOR TestBeam!!!.
   Use MdtCalibrationRegionSvc for collision data!!!

   Service providing mapping between identifiers and calibration regions 
   The mapping is done using hashes to ensure fast access. 
   USED ONLY BY MdtCalibSvc (IF EXPLICITELY REQUIRED BY is_testbeam PROPERTY
   OF MdtCalibrationSvc). 
*/

class MdtCalibRegionSvc  {
 public:

  /** routines for rt-calibration regions */

  /** returns region hash associated with identifier */
  MdtRegionHash     getRtRegionHash(const MdtBasicRegionId &hash) const;

  /** returns region hash associated with identifier hash */
  MdtRegionHash     getRtRegionHash(const MdtBasicRegionHash &hash) const;

  /** return RT region associated with region hash */
  const MdtRegion*  getRtRegion(const MdtRegionHash &key) const;

  /**  number of rt regions */
  unsigned int      numberOfRtRegions() const;
  
  /** routines for correction function regions */

  /** returns region hash associated with identifier */
  MdtRegionHash     getCorRegionHash(const MdtBasicRegionId &hash) const;

  /** returns region hash associated with identifier hash */
  MdtRegionHash     getCorRegionHash(const MdtBasicRegionHash &hash) const;

  /** return correction function region associated with region hash */
  const MdtRegion*  getCorRegion(const MdtRegionHash &key) const;

  /**  number of correction function regions */
  unsigned int      numberOfCorRegions() const;
  
 private:  
  MdtRegionTool m_rtRegionTool;
  MdtRegionSet  m_rtRegions;
  
  MdtRegionTool m_corRegionTool;
  MdtRegionSet  m_corRegions;
};

inline MdtRegionHash MdtCalibRegionSvc::getRtRegionHash(const MdtBasicRegionId &hash) const {
  return m_rtRegionTool.getRegionHash(hash);
}

inline MdtRegionHash MdtCalibRegionSvc::getRtRegionHash(const MdtBasicRegionHash &hash) const {
  if( hash < 0 || hash >= m_rtRegions.size() ) return MdtRegionHash();
  return m_rtRegions[hash];
}

inline const MdtRegion* MdtCalibRegionSvc::getRtRegion(const MdtRegionHash &hash) const {
  if( hash < 0 || hash >= m_rtRegions.size() ) return 0;
  return m_rtRegions[hash];
}

inline unsigned int MdtCalibRegionSvc::numberOfRtRegions() const {
  return m_rtRegions.size();
}

inline MdtRegionHash MdtCalibRegionSvc::getCorRegionHash(const MdtBasicRegionId &hash) const {
  return m_corRegionTool.getRegionHash(hash);
}

inline MdtRegionHash MdtCalibRegionSvc::getCorRegionHash(const MdtBasicRegionHash& hash) const {
  if( hash < 0 || hash >= m_corRegions.size() ) return MdtRegionHash();
  return m_corRegions[hash];
}

inline unsigned int MdtCalibRegionSvc::numberOfCorRegions() const {
  return m_corRegions.size();
}

inline const MdtRegion* MdtCalibRegionSvc::getCorRegion(const MdtRegionHash& hash) const {
  if( hash < 0 || hash >= m_corRegions.size() ) return 0;
  return m_corRegions[hash];
}

#endif
