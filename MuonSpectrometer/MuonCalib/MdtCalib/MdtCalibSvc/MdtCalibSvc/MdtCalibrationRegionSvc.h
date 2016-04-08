/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_MDTCALIBRATIONREGIONSVC_H
#define MUONCALIB_MDTCALIBRATIONREGIONSVC_H

#include <vector>

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h"

#include "Identifier/Identifier.h"
#include "MuonCalibIdentifier/MdtRegionTool.h"
#include "MuonCalibIdentifier/MdtRegion.h"

/** Service providing mapping between identifiers and calibration regions 
    The mapping is done using hashes to ensure fast access. 
*/

class MdtIdHelper;

class MdtCalibrationRegionSvc  : public AthService, virtual public IInterface  {
public:
  /** constructor */
  MdtCalibrationRegionSvc(const std::string& name,ISvcLocator* sl);

  /** destructor */
  virtual ~MdtCalibrationRegionSvc();

  /** IInterface implementation  */
  static const InterfaceID& interfaceID() {
    static InterfaceID s_iID("MdtCalibrationRegionSvc", 1, 0);
    return s_iID;
  }

  /** IInterface implementation  */
  virtual StatusCode queryInterface(const InterfaceID& riid,void** ppvIF);

  /** initialization */
  virtual StatusCode initialize(void);

  /** finalization */
  virtual StatusCode finalize(void);  

  /** currently 2 types of mappings are hardcoded: "OnePerChamber" and "OneRt"
   *  and this method can be used to switch between one and the other. It is
   *  foreseen to store maps in DB to gain more flexibility.*/
  virtual void remapRtRegions(std::string mapName);

  /** routines for rt-calibration regions */

  /** returns region hash associated with identifier */
  MdtRegionHash     getRtRegionHash(const MdtBasicRegionId& hash) const;

  /** returns region hash associated with identifier hash */
  MdtRegionHash     getRtRegionHash(const MdtBasicRegionHash& hash) const;

  /** return MdtRegion associated with region hash */
  const MdtRegion*  getRtRegion(const MdtRegionHash& key) const;

  /**  number of rt regions */
  unsigned int      numberOfRtRegions() const;
  
  
  /** routines for correction function regions */


  /** returns region hash associated with identifier */
  MdtRegionHash     getCorRegionHash(const MdtBasicRegionId& hash) const;

  /** returns region hash associated with identifier hash */
  MdtRegionHash     getCorRegionHash(const MdtBasicRegionHash& hash) const;

  /** return MdtRegion associated with region hash */
  const MdtRegion*  getCorRegion(const MdtRegionHash& key) const;

  /**  number of rt regions */
  unsigned int      numberOfCorRegions() const;
  
private:  
  void initializeRegions();

  const MdtIdHelper* m_mdtIdHelper;
  

  MdtRegionTool m_rtRegionTool;
  
  MdtRegionTool m_corRegionTool;
  
};

inline
MdtRegionHash MdtCalibrationRegionSvc::getRtRegionHash(const MdtBasicRegionId& /*id*/) const
{
  /** convert id into hash 
      return getRtRegionHash(hash);
  */

  return MdtRegionHash();
}

inline								   
MdtRegionHash MdtCalibrationRegionSvc::getRtRegionHash(const MdtBasicRegionHash& hash) const
{
  return m_rtRegionTool.getRegionHash(hash);
}

inline
const MdtRegion*    MdtCalibrationRegionSvc::getRtRegion(const MdtRegionHash& hash) const
{
  return m_rtRegionTool.getRegion(hash);
}

inline
unsigned int MdtCalibrationRegionSvc::numberOfRtRegions() const
{
  return m_rtRegionTool.numberOfRegions();
}


inline
MdtRegionHash MdtCalibrationRegionSvc::getCorRegionHash(const MdtBasicRegionId& /*id*/) const
{
  /** convert id into hash 
      return getCorRegionHash(hash);
  */

  return MdtRegionHash();
}

inline								   
MdtRegionHash MdtCalibrationRegionSvc::getCorRegionHash(const MdtBasicRegionHash& hash) const
{
  return m_corRegionTool.getRegionHash(hash);
}

inline
const MdtRegion*    MdtCalibrationRegionSvc::getCorRegion(const MdtRegionHash& hash) const
{
  return m_rtRegionTool.getRegion(hash);
}

inline
unsigned int MdtCalibrationRegionSvc::numberOfCorRegions() const
{
  return m_corRegionTool.numberOfRegions();
}


#endif
