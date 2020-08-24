/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_MDTCALIBRATIONREGIONSVC_H
#define MUONCALIB_MDTCALIBRATIONREGIONSVC_H

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h"

#include <vector>

// Type of RT/Correction function regions, set in remapRtRegions() 
// MdtCalibDbCoolStrTool uses these regions to load RTs from COOL into m_rtGate (pointer to DetStore)
// MdtCalibrationDBSvc uses these regions to access the constants in m_rtGate
enum MdtRegionType { ONERT, ONEPERCHAMBER, ONEPERMULTILAYER };
 
// For ONERT and ONEPERCHAMBER m_regionHash stores the index of every chamber's RT in m_rtData.
// In this case m_regionHash should be accessed with the chamber hash from m_mdtIdHelper->get_hash()
// For ONERT only 1 RT is put in m_rtData so the index in m_regionHash is 0 for all chambers
// For ONEPERCHAMBER each chamber RT will be put in m_rtData so there will be m_mdtIdHelper->module_hash_max() RTs.
// For ONEPERMULTILAYER option m_regionHash stores the index of every ML RT.
// In this case m_regionHash should be accessed with the ML hash from m_mdtIdHelper->get_detectorElement_hash()
// Initially m_rtData is filled with RTs for each chamber (treating chamber RTs as ML1 RTs).
// Initially the number of RTs in m_rtData is m_mdtIdHelper->module_hash_max(), the same as for ONEPERCHAMBER
// If ML2 RTs are read from COOL they are added at the end of m_rtData in MdtCalibrationDBStrTool and
// the index of the ML2 RT in m_rtData is written into m_regionHash.  Hence, addition memory for ML2 RTs 
// is only required if ML2 RTs are loaded into COOL.

class MdtIdHelper;

class MdtCalibrationRegionSvc : public AthService, virtual public IInterface  {
public:
  /** constructor */
  MdtCalibrationRegionSvc(const std::string &name,ISvcLocator *sl);

  /** destructor */
  virtual ~MdtCalibrationRegionSvc()=default;

  /** IInterface implementation  */
  static const InterfaceID &interfaceID() {
    static const InterfaceID s_iID("MdtCalibrationRegionSvc", 1, 0);
    return s_iID;
  }

  /** IInterface implementation  */
  virtual StatusCode queryInterface(const InterfaceID &riid,void **ppvIF);

  /** initialization */
  virtual StatusCode initialize(void);

  // Currently 3 types of mappings are defined: "OneRt", "OnePerChamber", and "OnePerMultilayer" 
  // and this method can be used to switch between one and the other.  
  virtual void remapRtRegions(std::string mapName);

  /**  get type of regions being used */
  virtual MdtRegionType RegionType() const;

  /**  number of regions */
  virtual unsigned int  numberOfRegions() const;

  /**  get hash to use for ML2 (works for ML1 too) */
  // mlHash is from m_mdtIdHelper->get_detectorElement_hash( athenaId, mlHash )
  virtual unsigned int  getRegionHash(const unsigned int mlHash) const;

  /**  set hash for ML2 */
  virtual unsigned int  setRegionHash(const unsigned int mlHash);

private: 
  const MdtIdHelper *m_mdtIdHelper;
  MdtRegionType      m_regionType;
  unsigned int       m_numberOfRegions;//number of RTs stored in m_rtData
  std::vector<int>   m_regionHash;     //stores hash to use for ML2 (which is ML1 hash by default)
};

inline MdtRegionType MdtCalibrationRegionSvc::RegionType() const {
  return m_regionType;
}

inline unsigned int MdtCalibrationRegionSvc::numberOfRegions() const {
  return m_numberOfRegions;
}

inline unsigned int MdtCalibrationRegionSvc::getRegionHash(const unsigned int mlHash) const {
  return m_regionHash[mlHash];
}

//  Add a new ML2 region hash.
//  ML2 RT will added to the end of m_rtData in MdtCalibDbCoolStrTool if an ML2 RT is read from COOL
//  Store the index of this RT in m_rtData in m_regionHash[mlHash]
//  Increase the count of RTs in m_rtData
inline unsigned int MdtCalibrationRegionSvc::setRegionHash(const unsigned int mlHash) {
  m_regionHash[mlHash] = m_numberOfRegions;
  m_numberOfRegions++;
  return m_numberOfRegions;
}

#endif
