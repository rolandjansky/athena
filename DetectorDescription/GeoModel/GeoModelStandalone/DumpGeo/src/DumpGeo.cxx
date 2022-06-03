/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
  ATH_MSG_INFO("in initialize()");

  //Create GeoExporter object and see if it considers settings to be valid.
  m_geoExporter = new GeoExporter(&(*evtStore()), &(*detStore()), serviceLocator(), m_toolSvc);
  if (!m_geoExporter->argumentsAreValid()) {
    delete m_geoExporter;
    m_geoExporter = 0;
    return StatusCode::FAILURE;
  }

  try
  {
    m_geoExporter->init();//Launch!
  }
  catch ( char* c)
  {
    ATH_MSG_ERROR("Exception: " << c);
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

//____________________________________________________________________
StatusCode DumpGeo::execute()
{
  return StatusCode::SUCCESS;
}

//____________________________________________________________________
void DumpGeo::handle(const Incident& inc)
{
  ATH_MSG_INFO("Handling incident '" << inc.type() << "'");

  if (!m_geoExporter) {
    ATH_MSG_INFO("Aborting due to null GeoExporter pointer.");
    return;
  }
}

