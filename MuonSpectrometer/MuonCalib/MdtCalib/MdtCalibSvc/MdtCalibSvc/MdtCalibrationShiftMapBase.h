/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTCALIBSVC_MDTCALIBRATIONSHIFTMAPBASE_H
#define MDTCALIBSVC_MDTCALIBRATIONSHIFTMAPBASE_H

#include <map>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MdtCalibInterfaces/IShiftMapTools.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonCablingData/MuonMDT_CablingMap.h"
#include "StoreGate/ReadCondHandleKey.h"

/**
   @class MdtCalibrationShiftMapBase
   Provides the base class for the per-tube shifting tools, like
   MdtCalibT0ShiftTool & MdtCalibTMaxShiftTool.
   @author Andreas Hoenle
*/
class MdtCalibrationShiftMapBase
    : public extends<AthAlgTool, MuonCalib::IShiftMapTools> {
 public:
  /* constructor */
  MdtCalibrationShiftMapBase(const std::string& type, const std::string& name, const IInterface* parent);

  /* destructor */
  ~MdtCalibrationShiftMapBase()=default;

  /* get shift value, override from IShiftMapTools */
  float getValue(const Identifier& id) override;

  virtual StatusCode initialize() override;

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
  Gaudi::Property<std::string> m_mapFileName{this,"MapFile",""};
  Gaudi::Property<float> m_centralValue{this,"CentralValue",0};
  Gaudi::Property<float> m_sigma{this,"Sigma",10};
  Gaudi::Property<bool> m_forceMapRecreate{this,"ForceMapRecreate",false};

  std::map<Identifier, float> m_shiftValues;
  bool m_mapIsInitialized;

  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  SG::ReadCondHandleKey<MuonMDT_CablingMap> m_mdtCab {this, "MdtCabling", "MuonMDT_CablingMap"};
};

#endif
