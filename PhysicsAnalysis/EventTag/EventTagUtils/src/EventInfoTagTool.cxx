/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : EventInfoTagTool.cxx
Package : offline/PhysicsAnalysis/EventTag/EventTagUtils
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : create a EventInfoTag - The Tag information associated to the event
          is built here

*****************************************************************************/

#include "GaudiKernel/MsgStream.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"

//#include "StoreGate/StoreGateSvc.h"

#include "CLHEP/Random/RandFlat.h"

#include "LumiBlockComps/ILumiBlockMuTool.h"
#include "xAODEventInfo/EventInfo.h"

#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

#include <sstream>

#include "TagEvent/GlobalEventAttributeNames.h"
#include "EventTagUtils/EventInfoTagTool.h"

#include "DBDataModel/CollectionMetadata.h"


//using xAOD::EventInfo;

/** the constructor */
EventInfoTagTool::EventInfoTagTool (const std::string& type, const
					std::string& name, const IInterface* parent) : 
  AthAlgTool( type, name, parent ), 
  m_lumiBlockMuTool("LumiBlockMuTool/LumiBlockMuTool"),
  m_isMC(false),
  m_weightSum(0)
{
  declareProperty("LumiBlockMuTool",   m_lumiBlockMuTool);
  /** AOD Object Name */
  declareProperty("IncludeEventFlag",  m_includeEventFlag=true);
  declareProperty("IncludeExtras",     m_includeExtras=true);

  declareInterface<EventInfoTagTool>( this );

}

/** initialization - called once at the beginning */
StatusCode  EventInfoTagTool::initialize() {
  ATH_MSG_INFO("in initialize()");

  // Set to be listener for incidents
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
  StatusCode sc = incSvc.retrieve();
  if (!sc.isSuccess()) {
    ATH_MSG_ERROR("Unable to get the IncidentSvc");
    return(sc);
  }

  // retrieve LumiBlockMuTool
  CHECK(m_lumiBlockMuTool.retrieve());

  return AthAlgTool::initialize();
}

/** build the attribute list - global event */
StatusCode 
EventInfoTagTool::attributeSpecification(std::vector<std::pair<std::string,AthenaAttributeType> >& attrMap) 
{

  ATH_MSG_DEBUG("in attributeSpecification()");

  // Fill spec from attributes in EventInfoAttributeSpecs
  for (unsigned int i = 0; i <= Evt::ConditionsRun; ++i) {
     attrMap.push_back(std::make_pair(EventAttributeSpecs[i].name(),
                                      EventAttributeSpecs[i].attributeType() 
                      )              );
  }

  if ( m_includeEventFlag )  {
     for (unsigned int i = Evt::Pixel; i <= Evt::Core; ++i) {
        attrMap.push_back(std::make_pair(EventAttributeSpecs[i].name(),
                                         EventAttributeSpecs[i].attributeType() 
                         )              );
     }
  }

  if ( m_includeExtras )  {
     for (unsigned int i = Evt::Simu; i <= Evt::AvgInt; ++i) {
        attrMap.push_back(std::make_pair(EventAttributeSpecs[i].name(),
                                         EventAttributeSpecs[i].attributeType() 
                         )              );
     }
  }
  
  return StatusCode::SUCCESS;
}

/** build the attribute list - global event */
StatusCode 
EventInfoTagTool::attributeSpecification(std::vector<AthenaAttributeSpecification>& attrMap) 
{

  ATH_MSG_DEBUG("in attributeSpecification()");

  // Fill spec from attributes in EventInfoAttributeSpecs
  for (unsigned int i = 0; i <= Evt::ConditionsRun; ++i) {
     attrMap.push_back(EventAttributeSpecs[i]);
  }

  if ( m_includeEventFlag )  {
     for (unsigned int i = Evt::Pixel; i <= Evt::Core; ++i) {
        attrMap.push_back(EventAttributeSpecs[i]);
     }
  }

  if ( m_includeExtras )  {
     for (unsigned int i = Evt::Simu; i <= Evt::Random; ++i) {
        attrMap.push_back(EventAttributeSpecs[i]);
     }
  }
  
  return StatusCode::SUCCESS;
}

void EventInfoTagTool::handle(const Incident& ) 
{
}

/** execute - called on every event */
StatusCode  EventInfoTagTool::execute(TagFragmentCollection& eventTag) {

  ATH_MSG_DEBUG("in execute()");

  /** retrieve event info */
  const DataHandle<xAOD::EventInfo> eventInfo;
  StatusCode sc = evtStore()->retrieve(eventInfo);
  if (sc.isFailure()) 
  {
    ATH_MSG_ERROR("Could not retrieve event info from TDS.");
  } else {
    // set monte carlo flag
    m_isMC  = eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION);

    sc = this->eventTag (eventTag, eventInfo);
    if (sc.isFailure()) ATH_MSG_WARNING("Unable to build Tag Fragments for the Event");
    if ( m_includeEventFlag )  {
      StatusCode sc = eventTagFlags (eventTag, eventInfo);
      if (sc.isFailure()) ATH_MSG_WARNING("Unable to build Tag Fragments for the Flags");
    }
    if ( m_includeExtras )  {
      StatusCode sc = eventExtrasTag (eventTag, eventInfo);
      if (sc.isFailure()) ATH_MSG_WARNING("Unable to build Tag Fragments for the Extras");
    }
  }

  ATH_MSG_DEBUG("EventInfoTagTool - execute() return success");

  return StatusCode::SUCCESS;
}

/** build the tag associate to the event information */
StatusCode EventInfoTagTool::eventTag(TagFragmentCollection& eventTag, 
                                      const DataHandle<xAOD::EventInfo> eventInfo) 
{

  ATH_MSG_DEBUG("in execute() - eventTag");

  /** run number and Event number */
  m_runNumber   = eventInfo->runNumber();
  m_condRunNumber = m_runNumber;
  m_lumiBlock   = eventInfo->lumiBlock();
  m_eventNumber = eventInfo->eventNumber();
  eventTag.insert (EventAttributeSpecs[Evt::Run].name(), m_runNumber );
  eventTag.insert (EventAttributeSpecs[Evt::ConditionsRun].name(), m_condRunNumber );
  eventTag.insert (EventAttributeSpecs[Evt::NLumiBlock].name(), m_lumiBlock );
  eventTag.insert (EventAttributeSpecs[Evt::Event].name(), m_eventNumber );

  unsigned long timeStamp   = eventInfo->timeStamp();
  unsigned long timeStampNS = eventInfo->timeStampNSOffset();
  unsigned long bunchId     = eventInfo->bcid();
  eventTag.insert (EventAttributeSpecs[Evt::Time].name(), timeStamp );
  eventTag.insert (EventAttributeSpecs[Evt::TimeNS].name(), timeStampNS );
  eventTag.insert (EventAttributeSpecs[Evt::BunchId].name(), bunchId );

  
  // event weight 
  // used for event weighting in monte carlo or just an event count in data
  double evweight = 1;
  bool isSimulation  = eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION);
  if (isSimulation) evweight = eventInfo->mcEventWeight();
  if (isSimulation) m_runNumber = eventInfo->mcChannelNumber();
  m_weightSum += evweight;
  eventTag.insert (EventAttributeSpecs[Evt::Weight].name(), evweight );

  // interaction counts
  float actualInt = m_lumiBlockMuTool->actualInteractionsPerCrossing();
  float avgInt    = m_lumiBlockMuTool->averageInteractionsPerCrossing();
  eventTag.insert (EventAttributeSpecs[Evt::ActualInt].name(), actualInt );
  eventTag.insert (EventAttributeSpecs[Evt::AvgInt].name()   , avgInt );

  return StatusCode::SUCCESS;

}

StatusCode EventInfoTagTool::eventTagFlags(TagFragmentCollection& eventTag, 
                                           const DataHandle<xAOD::EventInfo> eventInfo) 
{

  ATH_MSG_DEBUG("in execute() - eventTag");

  //Fill Detector status: Pixel, SCT, TRT, LAr, Tile, Muon, ForwardDet, Core
  for (unsigned int i = 0; i <= xAOD::EventInfo::Core; ++i) {
    unsigned int  result = 0x0;
    result = eventInfo->eventFlags(xAOD::EventInfo::EventFlagSubDet(i));
    xAOD::EventInfo::EventFlagErrorState error = eventInfo->errorState(xAOD::EventInfo::EventFlagSubDet(i));
    result = result | (error << 28);
    eventTag.insert(EventAttributeSpecs[Evt::Pixel+i].name(), result);
  }
   
  return StatusCode::SUCCESS;

}

/** build the tag associate to the event information */
StatusCode EventInfoTagTool::eventExtrasTag(TagFragmentCollection& eventTag, 
                                            const DataHandle<xAOD::EventInfo> eventInfo) 
{

  ATH_MSG_DEBUG("in execute() - eventTag");

  /** Event Type */
  bool isSimulation  = eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION);
  m_isMC = isSimulation;
  bool isTestBeam    = eventInfo->eventType(xAOD::EventInfo::IS_TESTBEAM);
  bool isCalibration = eventInfo->eventType(xAOD::EventInfo::IS_CALIBRATION);
  eventTag.insert(EventAttributeSpecs[Evt::Simu].name(), isSimulation );
  eventTag.insert(EventAttributeSpecs[Evt::Test].name(), isTestBeam );
  eventTag.insert(EventAttributeSpecs[Evt::Calib].name(), isCalibration );

  double rand = CLHEP::RandFlat::shoot();
  eventTag.insert(EventAttributeSpecs[Evt::Random].name(), rand);

  return StatusCode::SUCCESS;

}

/** finialize - called once at the end */
StatusCode  EventInfoTagTool::finalize() {
  ATH_MSG_DEBUG("in finalize()");
  return AthAlgTool::finalize();
}

StatusCode  EventInfoTagTool::stop() {

  ATH_MSG_DEBUG("in stop()");

  CollectionMetadataContainer* cmet = 0;
  bool newlist(true);

  ServiceHandle<StoreGateSvc> pMetaDataStore("StoreGateSvc/MetaDataStore", this->name());
  StatusCode sc = pMetaDataStore.retrieve();
  if (sc.isFailure() || 0 == pMetaDataStore) {
     ATH_MSG_ERROR("Could not find MetaDataStore");
     return sc;
  }

  if (pMetaDataStore->contains<CollectionMetadataContainer>(this->name())) {
     newlist=false;
     ATH_MSG_DEBUG("Pre-existing CollectionMetadataContainer found");
     StatusCode status = pMetaDataStore->retrieve(cmet,this->name());
     if (!status.isSuccess()) {
        ATH_MSG_ERROR("Could not retrieve CollectionMetadataContainer for " << this->name());
     }
  }
  else {
     cmet = new CollectionMetadataContainer;
  }

  std::map<std::string,std::string> weightS;
  std::stringstream temp;  temp << m_weightSum;
  weightS.insert(std::make_pair("WeightedEventSum",temp.str()));
  CollectionMetadata* newmap = new CollectionMetadata(weightS);
  cmet->push_back(newmap);

  // if new list then record it in metadata store.
  if (newlist) {
     StatusCode sc = pMetaDataStore->record(cmet,this->name());
     if (!sc.isSuccess()) {
        ATH_MSG_ERROR("Could not store event sum in Metadata store for " << this->name());
     }
     else {
        ATH_MSG_INFO("Event sum copied to MetaDataStore for "  << this->name());
     }
  }
  //ATH_MSG_INFO(pMetaDataStore->dump());
  
  return AthAlgTool::stop();
}
