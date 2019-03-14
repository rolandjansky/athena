/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GeoModel_SCT_DataBase_H
#define SCT_GeoModel_SCT_DataBase_H

#include "SCT_GeoModel/SCT_GeoModelAthenaComps.h"
#include <string>

#include "RDBAccessSvc/IRDBAccessSvc.h"
class IRDBRecord;


class SCT_DataBase
{

  
public:

  SCT_DataBase(const SCT_GeoModelAthenaComps* athenaComps);

  const SCT_GeoModelAthenaComps* athenaComps() const;

  IRDBRecordset_ptr weightTable() const;
  IRDBRecordset_ptr scalingTable() const;
  IRDBRecordset_ptr topLevelTable() const;

  IRDBRecordset_ptr conditionsTable() const;
  const IRDBRecord* conditions() const;

  const IRDBRecord* brlSensor() const;
  const IRDBRecord* brlModule() const;

  const IRDBRecord* brlSki() const;
  const IRDBRecord* brlSkiZ(int i) const;
  int brlSkiZSize() const;
  const IRDBRecord* brlLayer(int i) const;
  const IRDBRecord* brlServices() const;
  const IRDBRecord* brlServPerLayer(int i) const;
  const IRDBRecord* brlThermalShield() const;
  const IRDBRecord* brlGeneral() const;
  const IRDBRecord* brlFSI() const;
  int brlFSISize() const;
  const IRDBRecord* brlFSILocation(int i) const;

  const IRDBRecord* fwdSensor(int i) const;
  const IRDBRecord* fwdHybrid() const;
  const IRDBRecord* fwdSpine(int i) const;
  const IRDBRecord* fwdModule(int i) const;
  int fwdModuleSize() const;
  IRDBRecordset_ptr fwdModuleConnectorTable() const;
  const IRDBRecord* fwdModuleConnector() const;

  const IRDBRecord* fwdRing(int i) const;
  int fwdRingSize()  const;
  const IRDBRecord* fwdWheel(int i) const;
  const IRDBRecord* fwdWheelRingMap(int i) const;
  int fwdWheelRingMapSize() const;
  const IRDBRecord* fwdDiscSupport() const;
  const IRDBRecord* fwdPatchPanelLoc(int i) const;
  int fwdPatchPanelLocSize() const;
  const IRDBRecord* fwdPatchPanel(int i) const;
  int fwdPatchPanelSize() const;
  const IRDBRecord* fwdPPConnector() const;
  int fwdPPConnectorSize() const;
  const IRDBRecord* fwdPPCooling() const;
  int fwdPPCoolingSize() const;
  const IRDBRecord* fwdCoolingBlock(int i) const;
  const IRDBRecord* fwdRingServices(int i) const;
  const IRDBRecord* fwdServices() const;
  const IRDBRecord* fwdFSILocation(int i) const;
  int fwdFSILocationSize()  const;
  const IRDBRecord* fwdFSIType(int i) const;
  int fwdFSITypeSize() const;
  const IRDBRecord* fwdFSI(int i) const;
  int fwdFSISize() const;
  const IRDBRecord* fwdThermalShield(int i) const;
  int fwdThermalShieldSize()  const;
  const IRDBRecord* fwdGeneral() const;
  IRDBRecordset_ptr fwdOptoHarnessTable() const;
  const IRDBRecord* fwdOptoHarness(int i) const;
  IRDBRecordset_ptr fwdDiscFixationTable() const;
  const IRDBRecord* fwdDiscFixation() const;
  const IRDBRecord* fwdCylServ(int i) const;
  int fwdCylServSize() const;
  const IRDBRecord* fwdCylServLoc(int i) const;
  int fwdCylServLocSize() const;

  // Return the SCT version tag.
  const std::string & versionTag() const;

  MsgStream& msg (MSG::Level lvl) const;

private:
  
  SCT_DataBase(const SCT_DataBase &);
  SCT_DataBase& operator= (const SCT_DataBase &);

private:

  const SCT_GeoModelAthenaComps* m_athenaComps;

  std::string m_sctVersionTag;

  IRDBRecordset_ptr m_weightTable;
  IRDBRecordset_ptr m_scalingTable;
  IRDBRecordset_ptr m_topLevel;
  IRDBRecordset_ptr m_conditions;

  IRDBRecordset_ptr m_brlSensor;
  IRDBRecordset_ptr m_brlModule;
  IRDBRecordset_ptr m_brlSki;
  IRDBRecordset_ptr m_brlSkiZ;
  IRDBRecordset_ptr m_brlLayer;
  IRDBRecordset_ptr m_brlServices;
  IRDBRecordset_ptr m_brlServPerLayer;
  IRDBRecordset_ptr m_brlThermalShield;
  IRDBRecordset_ptr m_brlGeneral;
  IRDBRecordset_ptr m_brlFSI;
  IRDBRecordset_ptr m_brlFSILocation;
  IRDBRecordset_ptr m_fwdSensor;
  IRDBRecordset_ptr m_fwdHybrid;
  IRDBRecordset_ptr m_fwdSpine;
  IRDBRecordset_ptr m_fwdModule;
  IRDBRecordset_ptr m_fwdModuleConnector;
  IRDBRecordset_ptr m_fwdRing;
  IRDBRecordset_ptr m_fwdWheel;
  IRDBRecordset_ptr m_fwdWheelRingMap;
  IRDBRecordset_ptr m_fwdDiscSupport;
  IRDBRecordset_ptr m_fwdPatchPanelLoc;
  IRDBRecordset_ptr m_fwdPatchPanel;
  IRDBRecordset_ptr m_fwdPPConnector;
  IRDBRecordset_ptr m_fwdPPCooling;
  IRDBRecordset_ptr m_fwdCoolingBlock;
  IRDBRecordset_ptr m_fwdRingServices;
  IRDBRecordset_ptr m_fwdServices;
  IRDBRecordset_ptr m_fwdFSILocation;
  IRDBRecordset_ptr m_fwdFSIType;
  IRDBRecordset_ptr m_fwdFSI;
  IRDBRecordset_ptr m_fwdThermalShield;
  IRDBRecordset_ptr m_fwdGeneral;
  IRDBRecordset_ptr m_fwdOptoHarness;
  IRDBRecordset_ptr m_fwdDiscFixation;
  IRDBRecordset_ptr m_fwdCylServ;
  IRDBRecordset_ptr m_fwdCylServLoc;


};

#endif //SCT_GeoModel_SCT_DataBase_H
