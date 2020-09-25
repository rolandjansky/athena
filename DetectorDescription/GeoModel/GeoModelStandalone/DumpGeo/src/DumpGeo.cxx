/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "DumpGeo/DumpGeo.h"

#include "GeoExporter/GeoExporter.h"

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
StatusCode DumpGeo::initialize()
{
  msg(MSG::INFO) << " in initialize() " << endmsg;

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
}

