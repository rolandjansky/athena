/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "DumpGeo/DumpGeo.h"

#include "GeoExporter/GeoExporter.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/TriggerInfo.h"
#include "EventInfo/EventID.h"
#include "PathResolver/PathResolver.h"

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/FileIncident.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IIncidentSvc.h"

#include <vector>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <cstdlib> //For setenv


//____________________________________________________________________
DumpGeo::DumpGeo(const std::string& name, ISvcLocator* svcLocator):
  AthAlgorithm(name, svcLocator),
  m_toolSvc(0),m_geoExporter(0)
{
  declareProperty("NoGui",m_noGui=false);

  ::setenv("LCGPATCH_COINMULTISELECT","1",1);
}

//____________________________________________________________________
DumpGeo::~DumpGeo()
{
}

//____________________________________________________________________
StatusCode DumpGeo::initialize()
{
  msg(MSG::INFO) << " in initialize() " << endmsg;

  //ToolSvc
  StatusCode status = service("ToolSvc",m_toolSvc);
  if (status.isFailure()||!m_toolSvc) {
    msg(MSG::ERROR) << " Unable to get ToolSvc!" << endmsg;
    return status;
  }

  // use the incident service to register a handler
  IIncidentSvc* incsvc = 0;
  status = service("IncidentSvc", incsvc, true);

  if(status.isFailure() || incsvc==0) {
    msg(MSG::WARNING) << "Unable to get IncidentSvc! MF mechanism is disabled" << endmsg;
    return StatusCode::SUCCESS;
  }

  std::string endfilekey("EndTagFile");
  incsvc->addListener(this, endfilekey, 0);
  msg(MSG::DEBUG) << "Added listener on "<<endfilekey << endmsg;

  //Create GeoExporter object and see if it considers settings to be valid.
  m_geoExporter = new GeoExporter(&(*evtStore()), &(*detStore()), serviceLocator(), m_toolSvc);
  if (!m_geoExporter->argumentsAreValid()) {
    delete m_geoExporter;
    m_geoExporter = 0;
    exit(1); 
    return StatusCode::FAILURE;
  }

  try
  {
    m_geoExporter->init();//Launch!
  }
  catch ( char* c)
  {
    std::cout << "Exception: " << c << std::endl;
    return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS;
}

//____________________________________________________________________
StatusCode DumpGeo::execute()
{
  msg(MSG::DEBUG) <<" in execute() " << endmsg;
  return StatusCode::FAILURE;
}

//____________________________________________________________________
StatusCode DumpGeo::finalize()
{
  msg(MSG::INFO) <<" in finalize() " << endmsg;
  return StatusCode::SUCCESS;
}

//____________________________________________________________________
void DumpGeo::handle(const Incident& inc)
{
  msg(MSG::INFO) << "Handling incident '" << inc.type() << "'" << endmsg;

  if (!m_geoExporter) {
    msg(MSG::INFO) << "Aborting due to null GeoExporter pointer." << endmsg;
    return;
  }

  const FileIncident* fileInc  = dynamic_cast<const FileIncident*>(&inc);
  if(fileInc == 0) {
    msg(MSG::WARNING) << " Unable to cast to file incident" << endmsg;
    return;
  }
  else
    msg(MSG::DEBUG) << " Casting to file incident successful" << endmsg;

  // Locate the EventSelector
  ServiceHandle<IEvtSelector> pEvtSelector("EventSelector", this->name());
  StatusCode sc = pEvtSelector.retrieve();

  if(!sc.isSuccess() || 0 == pEvtSelector) {
    msg(MSG::WARNING) << "Could not find EventSelector" << endmsg;
    return;
  }
  else
    msg(MSG::DEBUG) << " Got EventSelector" << endmsg;

  IProperty* propertyServer = dynamic_cast<IProperty*>(pEvtSelector.operator->());
  if (!propertyServer) {
    msg(MSG::WARNING) << "Could not get propertyServer" << endmsg;
    return;
  }
}
