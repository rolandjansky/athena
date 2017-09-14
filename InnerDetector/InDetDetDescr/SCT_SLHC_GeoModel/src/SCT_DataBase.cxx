/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_DataBase.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

#include "SCT_SLHC_GeoModel/SCT_GeoModelAthenaComps.h"

#include <iostream>

namespace InDetDDSLHC {


SCT_DataBase::SCT_DataBase(const SCT_GeoModelAthenaComps * athenaComps)
  : m_athenaComps(athenaComps)
{
  IGeoDbTagSvc * geoDbTag = m_athenaComps->geoDbTagSvc();

  DecodeVersionKey versionKey(geoDbTag, "SCT");
    std::string versionTag  = versionKey.tag();
  std::string versionNode = versionKey.node();

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

  // Envelope table. If not present will use a boolean of barrel + endcaps
  m_envelope = rdbSvc->getRecordsetPtr("SctEnvelope", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctEnvelope Fetched" << endmsg;

  //
  // SCT Barrel 
  //
  m_brlSensor = rdbSvc->getRecordsetPtr("SSctBrlSensor", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctBrlSensor Fetched" << endmsg;

  m_brlModule = rdbSvc->getRecordsetPtr("SSctBrlModule", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctBrlModule Fetched" << endmsg;

  m_brlLadder = rdbSvc->getRecordsetPtr("SSctBrlLadder", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctBrlLadder Fetched" << endmsg;

  m_brlServPerLayer = rdbSvc->getRecordsetPtr("SSctBrlServPerLayer", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctBrlServPerLayer Fetched" << endmsg;

  m_brlSkiZ = rdbSvc->getRecordsetPtr("SSctBrlSkiZ", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctBrlSkiZ Fetched" << endmsg;

  m_brlLayer = rdbSvc->getRecordsetPtr("SSctBrlLayer", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctBrlLayer Fetched" << endmsg;

  m_brlGeneral = rdbSvc->getRecordsetPtr("SSctBrlGeneral", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctBrlGeneral Fetched" << endmsg;

  m_fwdSensor = rdbSvc->getRecordsetPtr("SSctFwdSensor", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdSensor Fetched" << endmsg;

  m_fwdSpine = rdbSvc->getRecordsetPtr("SSctFwdSpine", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdSpine Fetched" << endmsg;

  m_fwdModule = rdbSvc->getRecordsetPtr("SSctFwdModule", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdModule Fetched" << endmsg;

  //
  // SCT Forward
  //
  m_fwdRing = rdbSvc->getRecordsetPtr("SSctFwdRing", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdRing Fetched" << endmsg;

  m_fwdWheel = rdbSvc->getRecordsetPtr("SSctFwdWheel", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdWheel Fetched" << endmsg;

  m_fwdWheelRingMap = rdbSvc->getRecordsetPtr("SSctFwdWheelRingMap", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdWheelRingMap Fetched" << endmsg;

  m_fwdDiscSupport = rdbSvc->getRecordsetPtr("SSctFwdDiscSupport", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdDiscSupport Fetched" << endmsg;

  m_fwdGeneral = rdbSvc->getRecordsetPtr("SSctFwdGeneral", versionTag, versionNode);
  msg(MSG::DEBUG) << "Table SctFwdGeneral Fetched" << endmsg;

}


IRDBRecordset_ptr SCT_DataBase::weightTable() const {return m_weightTable;}

IRDBRecordset_ptr SCT_DataBase::topLevelTable() const {return m_topLevel;}

IRDBRecordset_ptr SCT_DataBase::sctEnvelope() const {return m_envelope;}

IRDBRecordset_ptr SCT_DataBase::brlSensor() const {return m_brlSensor;}
IRDBRecordset_ptr SCT_DataBase::brlModule() const {return m_brlModule;}

IRDBRecordset_ptr SCT_DataBase::brlLadder() const {return m_brlLadder;}
IRDBRecordset_ptr SCT_DataBase::brlServPerLayer() const {return m_brlServPerLayer;}
IRDBRecordset_ptr SCT_DataBase::brlSkiZ() const {return m_brlSkiZ;}
IRDBRecordset_ptr SCT_DataBase::brlLayer() const {return m_brlLayer;}
IRDBRecordset_ptr SCT_DataBase::brlGeneral() const {return m_brlGeneral;}

IRDBRecordset_ptr SCT_DataBase::fwdSensor() const {return m_fwdSensor;}
IRDBRecordset_ptr SCT_DataBase::fwdSpine() const {return m_fwdSpine;}
IRDBRecordset_ptr SCT_DataBase::fwdModule() const {return m_fwdModule;}
IRDBRecordset_ptr SCT_DataBase::fwdRing() const {return m_fwdRing;}
IRDBRecordset_ptr SCT_DataBase::fwdWheel() const {return m_fwdWheel;}
IRDBRecordset_ptr SCT_DataBase::fwdWheelRingMap() const {return m_fwdWheelRingMap;}
IRDBRecordset_ptr SCT_DataBase::fwdDiscSupport() const {return m_fwdDiscSupport;}
IRDBRecordset_ptr SCT_DataBase::fwdGeneral() const {return m_fwdGeneral;}

const std::string & SCT_DataBase::versionTag() const {
  return m_sctVersionTag;
}

MsgStream&  SCT_DataBase::msg (MSG::Level lvl) const 
{ 
  return m_athenaComps->msg(lvl); 
}
}
