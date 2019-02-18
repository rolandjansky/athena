/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : EventInfoAttListTool.cxx
Author  : Jack Cranshaw
Created : January 2017
Purpose : create a EventInfoAttList - The Tag information associated to the event
          is built here

*****************************************************************************/

#include "EventInfoAttListTool.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"

#include "xAODEventInfo/EventInfo.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "CoralBase/AttributeListSpecification.h"

#include <sstream>


//using xAOD::EventInfo;

/** the constructor */
EventInfoAttListTool::EventInfoAttListTool (const std::string& type, const
					std::string& name, const IInterface* parent) : 
  AthAlgTool( type, name, parent ) 
{
  declareInterface<EventInfoAttListTool>( this );
}

/** initialization - called once at the beginning */
StatusCode  EventInfoAttListTool::initialize() {
  ATH_MSG_DEBUG("in initialize()");

  m_attribListSpec  = new coral::AttributeListSpecification();
  m_attribListSpec->extend("IsSimulation",    "bool");
  m_attribListSpec->extend("IsCalibration",   "bool");
  m_attribListSpec->extend("IsTestBeam",      "bool");
  m_attribListSpec->extend("McChannel",       "unsigned int");
  m_attribListSpec->extend("RunNumber",       "unsigned int");
  m_attribListSpec->extend("EventNumber",     "unsigned long long");
  m_attribListSpec->extend("LumiBlockN",      "unsigned int");
  m_attribListSpec->extend("ConditionsRun",   "unsigned int");
  m_attribListSpec->extend("EventTime",       "unsigned int");
  m_attribListSpec->extend("EventTimeNanoSec","unsigned int");
  m_attribListSpec->extend("BunchId",         "unsigned int");
  m_attribListSpec->extend("EventWeight",     "float");
  
  return AthAlgTool::initialize();
}


/* Build attribute list from EventInfo object */
const AthenaAttributeList EventInfoAttListTool::getAttributeList(const xAOD::EventInfo& eventInfo) {

  // Create attributeList with appropriate attributes
  AthenaAttributeList eventTag( *m_attribListSpec );

  StatusCode sc = this->eventTag (eventTag, eventInfo);
  if (sc.isFailure()) {
    ATH_MSG_WARNING("Unable to build Tag Fragments for the Event");
  }

  ATH_MSG_DEBUG("EventInfoAttListTool - getAttributeList() return success");

  return eventTag;
}


/* Build attribute list from EventInfo object */
const AthenaAttributeList EventInfoAttListTool::getAttributeList(const EventInfo& eventInfo) {

  // Create attributeList with appropriate attributes
  AthenaAttributeList eventTag( *m_attribListSpec );

  StatusCode sc = this->eventTag (eventTag, eventInfo);
  if (sc.isFailure()) {
    ATH_MSG_WARNING("Unable to build Tag Fragments for the Event");
  }

  ATH_MSG_DEBUG("EventInfoAttListTool - getAttributeList() return success");

  return eventTag;
}

/** build the tag associate to the event information */
StatusCode EventInfoAttListTool::eventTag(AthenaAttributeList& eventTag, 
                                          const xAOD::EventInfo& eventInfo) 
{

  /** Event Type */
  bool isSimulation  = eventInfo.eventType(xAOD::EventInfo::IS_SIMULATION);
  bool isTestBeam    = eventInfo.eventType(xAOD::EventInfo::IS_TESTBEAM);
  bool isCalibration = eventInfo.eventType(xAOD::EventInfo::IS_CALIBRATION);
  eventTag["IsSimulation"] .data<bool>() = isSimulation;
  eventTag["IsCalibration"].data<bool>() = isCalibration;
  eventTag["IsTestBeam"]   .data<bool>() = isTestBeam;

  // run number and Event number 
  unsigned int       runNumber     = eventInfo.runNumber();
  unsigned int       condRunNumber = runNumber;
  unsigned long long eventNumber   = eventInfo.eventNumber();
  unsigned int       lumiBlock     = eventInfo.lumiBlock();
  unsigned int       mcChannel = 0;
  if (isSimulation)  mcChannel = eventInfo.mcChannelNumber();
  eventTag["McChannel"]    .data<unsigned int>()       = mcChannel;
  eventTag["RunNumber"]    .data<unsigned int>()       = runNumber;
  eventTag["EventNumber"]  .data<unsigned long long>() = eventNumber;
  eventTag["LumiBlockN"]   .data<unsigned int>()       = lumiBlock;
  eventTag["ConditionsRun"].data<unsigned int>()       = condRunNumber;

  unsigned long timeStamp   = eventInfo.timeStamp();
  unsigned long timeStampNS = eventInfo.timeStampNSOffset();
  unsigned long bunchId     = eventInfo.bcid();
  eventTag["EventTime"]       .data<unsigned int>() = timeStamp;
  eventTag["EventTimeNanoSec"].data<unsigned int>() = timeStampNS;
  eventTag["BunchId"]         .data<unsigned int>() = bunchId;

  // event weight 
  // used for event weighting in monte carlo or just an event count in data
  float evweight = 1;
  if (isSimulation) evweight = eventInfo.mcEventWeight();
  eventTag["EventWeight"].data<float>() = evweight;

  return StatusCode::SUCCESS;

}

/** build the tag associate to the event information */
StatusCode EventInfoAttListTool::eventTag(AthenaAttributeList& eventTag, 
                                          const EventInfo& eventInfo) 
{

  /** Event Type */
  bool isSimulation  = eventInfo.event_type()->test(EventType::IS_SIMULATION);
  bool isTestBeam    = eventInfo.event_type()->test(EventType::IS_TESTBEAM);
  bool isCalibration = eventInfo.event_type()->test(EventType::IS_CALIBRATION);
  eventTag["IsSimulation"] .data<bool>() = isSimulation;
  eventTag["IsCalibration"].data<bool>() = isCalibration;
  eventTag["IsTestBeam"]   .data<bool>() = isTestBeam;

  // run number and Event number 
  unsigned int       runNumber     = eventInfo.event_ID()->run_number();
  unsigned int       condRunNumber = runNumber;
  unsigned long long eventNumber   = eventInfo.event_ID()->event_number();
  unsigned int       lumiBlock     = eventInfo.event_ID()->lumi_block();
  unsigned int       mcChannel = 0;
  if (isSimulation)  mcChannel = eventInfo.event_type()->mc_channel_number();;
  eventTag["McChannel"]    .data<unsigned int>()       = mcChannel;
  eventTag["RunNumber"]    .data<unsigned int>()       = runNumber;
  eventTag["EventNumber"]  .data<unsigned long long>() = eventNumber;
  eventTag["LumiBlockN"]   .data<unsigned int>()       = lumiBlock;
  eventTag["ConditionsRun"].data<unsigned int>()       = condRunNumber;

  unsigned long timeStamp   = eventInfo.event_ID()->time_stamp();
  unsigned long timeStampNS = eventInfo.event_ID()->time_stamp_ns_offset();
  unsigned long bunchId     = eventInfo.event_ID()->bunch_crossing_id();
  eventTag["EventTime"]       .data<unsigned int>() = timeStamp;
  eventTag["EventTimeNanoSec"].data<unsigned int>() = timeStampNS;
  eventTag["BunchId"]         .data<unsigned int>() = bunchId;

  // event weight 
  // used for event weighting in monte carlo or just an event count in data
  float evweight = 1;
  if (isSimulation) evweight = eventInfo.event_type()->mc_event_weight();
  eventTag["EventWeight"].data<float>() = evweight;

  return StatusCode::SUCCESS;

}

/** finialize - called once at the end */
StatusCode  EventInfoAttListTool::finalize() {
  ATH_MSG_DEBUG("in finalize()");
  return AthAlgTool::finalize();
}
