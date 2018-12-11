/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTCALIBSVC_MDTCALIBRATIONSHIFTMAPBASE_H
#define MDTCALIBSVC_MDTCALIBRATIONSHIFTMAPBASE_H

/*
 *
 * Author Andreas Hoenle (der.andi@cern.ch)
 *
 */

#include <map>

// Framework includes
#include "AthenaBaseComps/AthService.h"

// MDT interfaces
#include "MdtCalibInterfaces/IShiftMapTools.h"

// MDT includes
#include "MuonMDT_Cabling/MuonMDT_CablingSvc.h"

class Identifier;
class TTree;

/**
   @class MdtCalibrationShiftMapBase
   Provides the base class for the per-tube shifting tools, like
   MdtCalibT0ShiftTool & MdtCalibTMaxShiftTool.
   @author Andreas Hoenle
*/
class MdtCalibrationShiftMapBase
    : public extends<AthService, MuonCalib::IShiftMapTools> {
 public:
  /* constructor */
  MdtCalibrationShiftMapBase(const std::string& name, ISvcLocator* pSvcLocator);

  /* destructor */
  ~MdtCalibrationShiftMapBase();

  /* get shift value, override from IShiftMapTools */
  float getValue(const Identifier& id) override;

  /*
   * initialization of map cannot happen before first event
   * special function required
   * we need the cabling service to be ready first
   */
  virtual StatusCode initializeMap();

  /* dump the map in binary file, given a path */
  StatusCode dumpMapAsFile();

  /* load the map from a binary file, given a path */
  StatusCode loadMapFromFile();

 protected:
  ServiceHandle<MuonMDT_CablingSvc> m_cablingSvc;
  std::map<Identifier, float> m_shiftValues;
  bool m_mapIsInitialized;

  /* Make map configurable in job options */
  std::string m_mapFileName;
  float m_centralValue;
  float m_sigma;
  bool m_forceMapRecreate;

 private:
};

#endif
