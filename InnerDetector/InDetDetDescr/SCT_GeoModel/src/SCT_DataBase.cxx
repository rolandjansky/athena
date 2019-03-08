/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_DataBase.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

#include "SCT_GeoModel/SCT_GeoModelAthenaComps.h"

#include <iostream>

SCT_DataBase::SCT_DataBase(const SCT_GeoModelAthenaComps * athenaComps)
{
  m_athenaComps = athenaComps;

  IGeoDbTagSvc * geoDbTag = m_athenaComps->geoDbTagSvc();

  // Get version tag and node for SCT
  DecodeVersionKey versionKey(geoDbTag,"SCT");
  std::string versionTag  = versionKey.tag();
  std::string versionNode = versionKey.node();

  // Get version tag and node for InnerDetector.
  DecodeVersionKey indetVersionKey(geoDbTag,"InnerDetector");

  // Access the RDB
  IRDBAccessSvc* rdbSvc = m_athenaComps->rdbAccessSvc();

  // SCT version tag
  m_sctVersionTag = rdbSvc->getChildTag("SCT", versionKey.tag(), versionKey.node());


/////////////////////////////////////////////////////////
//
// Gets the structures
//
/////////////////////////////////////////////////////////

    msg(MSG::INFO) << "Retrieving Record Sets from database ..." << endmsg;
    msg(MSG::DEBUG) << " Using version tag: " << versionTag << endmsg;
    msg(MSG::DEBUG) << "           at node: " << versionNode << endmsg;
    msg(MSG::DEBUG) << " SCT Version:       " << m_sctVersionTag << endmsg;

  // ATLS - not sure I use it.
  // General atlas parameters

  //
  // SCT General
  //

  // SCT TopLevel
  m_topLevel = rdbSvc->getRecordsetPtr("SctTopLevel", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctTopLevel Fetched" << endmsg;

  // Weight Table
  m_weightTable = rdbSvc->getRecordsetPtr("SctWeights", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctWeights Fetched" << endmsg;

  // Extra Scaling Table. This is used for extra material studies. For nominal material the table should be empty.
  // NB this is at InnerDetector level node.
  m_scalingTable = rdbSvc->getRecordsetPtr("SctMatScaling", indetVersionKey.tag(), indetVersionKey.node());
  msg(MSG::DEBUG) << "Table SctMatScaling Fetched" << endmsg;

  // Default conditions
  m_conditions = rdbSvc->getRecordsetPtr("SctConditions", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctConditions Fetched" << endmsg;

  //
  // SCT Barrel 
  //
  m_brlSensor = rdbSvc->getRecordsetPtr("SctBrlSensor", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctBrlSensor Fetched" << endmsg;

  m_brlModule = rdbSvc->getRecordsetPtr("SctBrlModule", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctBrlModule Fetched" << endmsg;

  m_brlSki = rdbSvc->getRecordsetPtr("SctBrlSki", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctBrlSki Fetched" << endmsg;

  m_brlSkiZ = rdbSvc->getRecordsetPtr("SctBrlSkiZ", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctBrlSkiZ Fetched" << endmsg;

  m_brlLayer = rdbSvc->getRecordsetPtr("SctBrlLayer", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctBrlLayer Fetched" << endmsg;

  m_brlServices = rdbSvc->getRecordsetPtr("SctBrlServices", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctBrlServices Fetched" << endmsg;

  m_brlServPerLayer = rdbSvc->getRecordsetPtr("SctBrlServPerLayer", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctBrlServPerLayer Fetched" << endmsg;

  m_brlThermalShield = rdbSvc->getRecordsetPtr("SctBrlThermalShield", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctBrlThermalShield Fetched" << endmsg;

  m_brlGeneral = rdbSvc->getRecordsetPtr("SctBrlGeneral", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctBrlGeneral Fetched" << endmsg;

  m_brlFSI = rdbSvc->getRecordsetPtr("SctBrlFSI", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctBrlFSI Fetched" << endmsg;

  m_brlFSILocation = rdbSvc->getRecordsetPtr("SctBrlFSILocation", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctBrlFSILocation Fetched" << endmsg;

  m_fwdSensor = rdbSvc->getRecordsetPtr("SctFwdSensor", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdSensor Fetched" << endmsg;

  m_fwdHybrid = rdbSvc->getRecordsetPtr("SctFwdHybrid", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdHybrid Fetched" << endmsg;

  m_fwdSpine = rdbSvc->getRecordsetPtr("SctFwdSpine", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdSpine Fetched" << endmsg;

  m_fwdModule = rdbSvc->getRecordsetPtr("SctFwdModule", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdModule Fetched" << endmsg;

  m_fwdModuleConnector = rdbSvc->getRecordsetPtr("SctFwdModuleConnector", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdModuleConnector Fetched" << endmsg;

  //
  // SCT Forward
  //
  m_fwdRing = rdbSvc->getRecordsetPtr("SctFwdRing", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdRing Fetched" << endmsg;

  m_fwdWheel = rdbSvc->getRecordsetPtr("SctFwdWheel", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdWheel Fetched" << endmsg;

  m_fwdWheelRingMap = rdbSvc->getRecordsetPtr("SctFwdWheelRingMap", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdWheelRingMap Fetched" << endmsg;

  m_fwdDiscSupport = rdbSvc->getRecordsetPtr("SctFwdDiscSupport", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdDiscSupport Fetched" << endmsg;

  m_fwdPatchPanelLoc = rdbSvc->getRecordsetPtr("SctFwdPatchPanelLoc", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdPatchPanelLoc Fetched" << endmsg;

  m_fwdPatchPanel = rdbSvc->getRecordsetPtr("SctFwdPatchPanel", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdPatchPanel Fetched" << endmsg;

  m_fwdPPConnector = rdbSvc->getRecordsetPtr("SctFwdPPConnector", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdPPConnector Fetched" << endmsg;

  m_fwdPPCooling = rdbSvc->getRecordsetPtr("SctFwdPPCooling", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdPPCooling Fetched" << endmsg;

  m_fwdCoolingBlock = rdbSvc->getRecordsetPtr("SctFwdCoolingBlock", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdCoolingBlock Fetched" << endmsg;

  m_fwdRingServices = rdbSvc->getRecordsetPtr("SctFwdRingServices", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdRingServices Fetched" << endmsg;

  m_fwdServices = rdbSvc->getRecordsetPtr("SctFwdServices", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdServices Fetched" << endmsg;

  m_fwdFSILocation = rdbSvc->getRecordsetPtr("SctFwdFSILocation", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdFSILocation Fetched" << endmsg;

  m_fwdFSIType = rdbSvc->getRecordsetPtr("SctFwdFSIType", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdFSIType Fetched" << endmsg;

  m_fwdFSI = rdbSvc->getRecordsetPtr("SctFwdFSI", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdFSI Fetched" << endmsg;

  m_fwdThermalShield = rdbSvc->getRecordsetPtr("SctFwdThermalShield", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdThermalShield Fetched" << endmsg;

  m_fwdGeneral = rdbSvc->getRecordsetPtr("SctFwdGeneral", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdGeneral Fetched" << endmsg;

  m_fwdOptoHarness = rdbSvc->getRecordsetPtr("SctFwdOptoHarness", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdOptoHarness Fetched" << endmsg;

  m_fwdDiscFixation = rdbSvc->getRecordsetPtr("SctFwdDiscFixation", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdDiscFixation Fetched" << endmsg;

  m_fwdCylServ = rdbSvc->getRecordsetPtr("SctFwdCylServ", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdCyServ Fetched" << endmsg;

  m_fwdCylServLoc = rdbSvc->getRecordsetPtr("SctFwdCylServLoc", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdCylServLoc Fetched" << endmsg;


}

const SCT_GeoModelAthenaComps* SCT_DataBase::athenaComps() const { return m_athenaComps; }

IRDBRecordset_ptr SCT_DataBase::weightTable() const {return m_weightTable;}

IRDBRecordset_ptr SCT_DataBase::scalingTable() const {return m_scalingTable;}

//const IRDBRecord* SCT_DataBase::atls() const {return *m_atls)[0];}  
IRDBRecordset_ptr SCT_DataBase::topLevelTable() const {return m_topLevel;}

IRDBRecordset_ptr SCT_DataBase::conditionsTable() const {return m_conditions;}
const IRDBRecord* SCT_DataBase::conditions() const {return (*m_conditions)[0];}

const IRDBRecord* SCT_DataBase::brlSensor() const {return (*m_brlSensor)[0];}
const IRDBRecord* SCT_DataBase::brlModule() const {return (*m_brlModule)[0];}

const IRDBRecord* SCT_DataBase::brlSki() const {return (*m_brlSki)[0];}
const IRDBRecord* SCT_DataBase::brlSkiZ(int i) const {return (*m_brlSkiZ)[i];}
int SCT_DataBase::brlSkiZSize() const {return m_brlSkiZ->size();}
const IRDBRecord* SCT_DataBase::brlLayer(int i) const {return (*m_brlLayer)[i];}
const IRDBRecord* SCT_DataBase::brlServices() const {return (*m_brlServices)[0];}
const IRDBRecord* SCT_DataBase::brlServPerLayer(int i) const {return (*m_brlServPerLayer)[i];}
const IRDBRecord* SCT_DataBase::brlThermalShield() const {return (*m_brlThermalShield)[0];}
const IRDBRecord* SCT_DataBase::brlGeneral() const {return (*m_brlGeneral)[0];}
const IRDBRecord* SCT_DataBase::brlFSI() const {return (*m_brlFSI)[0];}
int SCT_DataBase::brlFSISize()  const {return m_brlFSI->size();}
const IRDBRecord* SCT_DataBase::brlFSILocation(int i) const {return (*m_brlFSILocation)[i];}

const IRDBRecord* SCT_DataBase::fwdSensor(int i) const {return (*m_fwdSensor)[i];}
const IRDBRecord* SCT_DataBase::fwdHybrid() const {return (*m_fwdHybrid)[0];}
const IRDBRecord* SCT_DataBase::fwdSpine(int i) const {return (*m_fwdSpine)[i];}
const IRDBRecord* SCT_DataBase::fwdModule(int i) const {return (*m_fwdModule)[i];}
int SCT_DataBase::fwdModuleSize() const {return m_fwdModule->size();}
IRDBRecordset_ptr SCT_DataBase::fwdModuleConnectorTable() const {return m_fwdModuleConnector;}
const IRDBRecord* SCT_DataBase::fwdModuleConnector() const {return (*m_fwdModuleConnector)[0];}

const IRDBRecord* SCT_DataBase::fwdRing(int i) const {return (*m_fwdRing)[i];}
int SCT_DataBase::fwdRingSize()  const {return m_fwdRing->size();}
const IRDBRecord* SCT_DataBase::fwdWheel(int i) const {return (*m_fwdWheel)[i];}
const IRDBRecord* SCT_DataBase::fwdWheelRingMap(int i) const {return (*m_fwdWheelRingMap)[i];}
int SCT_DataBase::fwdWheelRingMapSize()  const {return m_fwdWheelRingMap->size();}
const IRDBRecord* SCT_DataBase::fwdDiscSupport() const {return (*m_fwdDiscSupport)[0];}
const IRDBRecord* SCT_DataBase::fwdPatchPanelLoc(int i) const {return (*m_fwdPatchPanelLoc)[i];}
int SCT_DataBase::fwdPatchPanelLocSize()  const {return m_fwdPatchPanelLoc->size();}
const IRDBRecord* SCT_DataBase::fwdPatchPanel(int i) const {return (*m_fwdPatchPanel)[i];}
int SCT_DataBase::fwdPatchPanelSize()  const {return m_fwdPatchPanel->size();}
const IRDBRecord* SCT_DataBase::fwdPPConnector() const {return (*m_fwdPPConnector)[0];}
int SCT_DataBase::fwdPPConnectorSize()  const {return m_fwdPPConnector->size();}
const IRDBRecord* SCT_DataBase::fwdPPCooling() const {return (*m_fwdPPCooling)[0];}
int SCT_DataBase::fwdPPCoolingSize()  const {return m_fwdPPCooling->size();}
const IRDBRecord* SCT_DataBase::fwdCoolingBlock(int i) const {return (*m_fwdCoolingBlock)[i];}
const IRDBRecord* SCT_DataBase::fwdRingServices(int i) const {return (*m_fwdRingServices)[i];}
const IRDBRecord* SCT_DataBase::fwdServices() const {return (*m_fwdServices)[0];}
const IRDBRecord* SCT_DataBase::fwdFSILocation(int i) const {return (*m_fwdFSILocation)[i];}
int SCT_DataBase::fwdFSILocationSize()  const {return m_fwdFSILocation->size();}
const IRDBRecord* SCT_DataBase::fwdFSIType(int i) const {return (*m_fwdFSIType)[i];}
int SCT_DataBase::fwdFSITypeSize() const {return m_fwdFSIType->size();}
const IRDBRecord* SCT_DataBase::fwdFSI(int i) const {return (*m_fwdFSI)[i];}
int SCT_DataBase::fwdFSISize() const {return m_fwdFSI->size();}
const IRDBRecord* SCT_DataBase::fwdThermalShield(int i) const {return (*m_fwdThermalShield)[i];}
int SCT_DataBase::fwdThermalShieldSize()  const {return m_fwdThermalShield->size();}
const IRDBRecord* SCT_DataBase::fwdGeneral() const {return (*m_fwdGeneral)[0];}
IRDBRecordset_ptr SCT_DataBase::fwdOptoHarnessTable() const {return m_fwdOptoHarness;}
const IRDBRecord* SCT_DataBase::fwdOptoHarness(int i) const {return (*m_fwdOptoHarness)[i];}
const IRDBRecord* SCT_DataBase::fwdDiscFixation() const {return (*m_fwdDiscFixation)[0];}
IRDBRecordset_ptr SCT_DataBase::fwdDiscFixationTable() const {return m_fwdDiscFixation;}
const IRDBRecord* SCT_DataBase::fwdCylServ(int i) const {return (*m_fwdCylServ)[i];}
int SCT_DataBase::fwdCylServSize()  const {return m_fwdCylServ->size();}
const IRDBRecord* SCT_DataBase::fwdCylServLoc(int i) const {return (*m_fwdCylServLoc)[i];}
int SCT_DataBase::fwdCylServLocSize()  const {return m_fwdCylServLoc->size();}

const std::string & SCT_DataBase::versionTag() const {
  return m_sctVersionTag;
}

MsgStream&  SCT_DataBase::msg (MSG::Level lvl) const 
{ 
  return m_athenaComps->msg(lvl); 
}
