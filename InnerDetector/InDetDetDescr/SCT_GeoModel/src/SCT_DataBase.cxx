/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_DataBase.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

#include "SCT_GeoModel/SCT_GeoModelAthenaComps.h"

#include <iostream>

SCT_DataBase * SCT_DataBase::s_instance = 0;
const SCT_GeoModelAthenaComps * SCT_DataBase::s_athenaComps = 0;

SCT_DataBase * SCT_DataBase::instance() 
{
  if (s_athenaComps) {
    if (!s_instance) s_instance = new SCT_DataBase;
  } else {
    std::cout << "SCT_Data ERROR. Cannot create instance without SCT_GeoModelAthenaComps object!" << std::endl;
  }
  return s_instance;
}

void SCT_DataBase::reinit()
{
  delete s_instance;
  s_instance = 0;
}

const SCT_GeoModelAthenaComps *
SCT_DataBase::athenaComps() const 
{
  return  s_athenaComps;
}
  
void 
SCT_DataBase::setAthenaComps(const SCT_GeoModelAthenaComps * athenaComps)
{
  s_athenaComps = athenaComps;
}

SCT_DataBase::SCT_DataBase()

{

  IGeoModelSvc * geoModel = s_athenaComps->geoModelSvc();

  // Get version tag and node for SCT
  DecodeVersionKey versionKey(geoModel, "SCT");
  std::string versionTag  = versionKey.tag();
  std::string versionNode = versionKey.node();

  // Get version tag and node for InnerDetector.
  DecodeVersionKey indetVersionKey(geoModel, "InnerDetector");

  // Access the RDB
  IRDBAccessSvc* rdbSvc = s_athenaComps->rdbAccessSvc();

  // SCT version tag
  m_sctVersionTag = rdbSvc->getChildTag("SCT", versionKey.tag(), versionKey.node(), false);


/////////////////////////////////////////////////////////
//
// Gets the structures
//
/////////////////////////////////////////////////////////

    msg(MSG::INFO) << "Retrieving Record Sets from database ..." << endreq;
    msg(MSG::DEBUG) << " Using version tag: " << versionTag << endreq;
    msg(MSG::DEBUG) << "           at node: " << versionNode << endreq;
    msg(MSG::DEBUG) << " SCT Version:       " << m_sctVersionTag << endreq;

  // ATLS - not sure I use it.
  // General atlas parameters

  //
  // SCT General
  //

  // SCT TopLevel
  m_topLevel = rdbSvc->getRecordsetPtr("SctTopLevel", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctTopLevel Fetched" << endreq;

  // Weight Table
  m_weightTable = rdbSvc->getRecordsetPtr("SctWeights", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctWeights Fetched" << endreq;

  // Extra Scaling Table. This is used for extra material studies. For nominal material the table should be empty.
  // NB this is at InnerDetector level node.
  m_scalingTable = rdbSvc->getRecordsetPtr("SctMatScaling", indetVersionKey.tag(), indetVersionKey.node());
  msg(MSG::DEBUG) << "Table SctMatScaling Fetched" << endreq;

  // Default conditions
  m_conditions = rdbSvc->getRecordsetPtr("SctConditions", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctConditions Fetched" << endreq;

  //
  // SCT Barrel 
  //
  m_brlSensor = rdbSvc->getRecordsetPtr("SctBrlSensor", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctBrlSensor Fetched" << endreq;

  m_brlModule = rdbSvc->getRecordsetPtr("SctBrlModule", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctBrlModule Fetched" << endreq;

  m_brlSki = rdbSvc->getRecordsetPtr("SctBrlSki", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctBrlSki Fetched" << endreq;

  m_brlSkiZ = rdbSvc->getRecordsetPtr("SctBrlSkiZ", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctBrlSkiZ Fetched" << endreq;

  m_brlLayer = rdbSvc->getRecordsetPtr("SctBrlLayer", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctBrlLayer Fetched" << endreq;

  m_brlServices = rdbSvc->getRecordsetPtr("SctBrlServices", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctBrlServices Fetched" << endreq;

  m_brlServPerLayer = rdbSvc->getRecordsetPtr("SctBrlServPerLayer", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctBrlServPerLayer Fetched" << endreq;

  m_brlThermalShield = rdbSvc->getRecordsetPtr("SctBrlThermalShield", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctBrlThermalShield Fetched" << endreq;

  m_brlGeneral = rdbSvc->getRecordsetPtr("SctBrlGeneral", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctBrlGeneral Fetched" << endreq;

  m_brlFSI = rdbSvc->getRecordsetPtr("SctBrlFSI", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctBrlFSI Fetched" << endreq;

  m_brlFSILocation = rdbSvc->getRecordsetPtr("SctBrlFSILocation", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctBrlFSILocation Fetched" << endreq;

  m_fwdSensor = rdbSvc->getRecordsetPtr("SctFwdSensor", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdSensor Fetched" << endreq;

  m_fwdHybrid = rdbSvc->getRecordsetPtr("SctFwdHybrid", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdHybrid Fetched" << endreq;

  m_fwdSpine = rdbSvc->getRecordsetPtr("SctFwdSpine", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdSpine Fetched" << endreq;

  m_fwdModule = rdbSvc->getRecordsetPtr("SctFwdModule", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdModule Fetched" << endreq;

  m_fwdModuleConnector = rdbSvc->getRecordsetPtr("SctFwdModuleConnector", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdModuleConnector Fetched" << endreq;

  //
  // SCT Forward
  //
  m_fwdRing = rdbSvc->getRecordsetPtr("SctFwdRing", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdRing Fetched" << endreq;

  m_fwdWheel = rdbSvc->getRecordsetPtr("SctFwdWheel", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdWheel Fetched" << endreq;

  m_fwdWheelRingMap = rdbSvc->getRecordsetPtr("SctFwdWheelRingMap", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdWheelRingMap Fetched" << endreq;

  m_fwdDiscSupport = rdbSvc->getRecordsetPtr("SctFwdDiscSupport", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdDiscSupport Fetched" << endreq;

  m_fwdPatchPanelLoc = rdbSvc->getRecordsetPtr("SctFwdPatchPanelLoc", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdPatchPanelLoc Fetched" << endreq;

  m_fwdPatchPanel = rdbSvc->getRecordsetPtr("SctFwdPatchPanel", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdPatchPanel Fetched" << endreq;

  m_fwdPPConnector = rdbSvc->getRecordsetPtr("SctFwdPPConnector", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdPPConnector Fetched" << endreq;

  m_fwdPPCooling = rdbSvc->getRecordsetPtr("SctFwdPPCooling", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdPPCooling Fetched" << endreq;

  m_fwdCoolingBlock = rdbSvc->getRecordsetPtr("SctFwdCoolingBlock", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdCoolingBlock Fetched" << endreq;

  m_fwdRingServices = rdbSvc->getRecordsetPtr("SctFwdRingServices", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdRingServices Fetched" << endreq;

  m_fwdServices = rdbSvc->getRecordsetPtr("SctFwdServices", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdServices Fetched" << endreq;

  m_fwdFSILocation = rdbSvc->getRecordsetPtr("SctFwdFSILocation", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdFSILocation Fetched" << endreq;

  m_fwdFSIType = rdbSvc->getRecordsetPtr("SctFwdFSIType", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdFSIType Fetched" << endreq;

  m_fwdFSI = rdbSvc->getRecordsetPtr("SctFwdFSI", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdFSI Fetched" << endreq;

  m_fwdThermalShield = rdbSvc->getRecordsetPtr("SctFwdThermalShield", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdThermalShield Fetched" << endreq;

  m_fwdGeneral = rdbSvc->getRecordsetPtr("SctFwdGeneral", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdGeneral Fetched" << endreq;

  m_fwdOptoHarness = rdbSvc->getRecordsetPtr("SctFwdOptoHarness", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdOptoHarness Fetched" << endreq;

  m_fwdDiscFixation = rdbSvc->getRecordsetPtr("SctFwdDiscFixation", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdDiscFixation Fetched" << endreq;

  m_fwdCylServ = rdbSvc->getRecordsetPtr("SctFwdCylServ", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdCyServ Fetched" << endreq;

  m_fwdCylServLoc = rdbSvc->getRecordsetPtr("SctFwdCylServLoc", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdCylServLoc Fetched" << endreq;


}


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
  return s_athenaComps->msg(lvl); 
}
