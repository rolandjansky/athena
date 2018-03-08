/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "AthenaMonitoring/MonitoredScope.h"
#include "TrigCaloDataAccessSvc.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

#include <sstream>
#include <type_traits>

// Register our StatusCode category

TrigCaloDataAccessSvc::TrigCaloDataAccessSvc( const std::string& name, ISvcLocator* pSvcLocator )
  : AthService( name, pSvcLocator ) {
}

TrigCaloDataAccessSvc::~TrigCaloDataAccessSvc() {}

StatusCode TrigCaloDataAccessSvc::initialize() {

  return StatusCode::SUCCESS;
}

StatusCode TrigCaloDataAccessSvc::finalize() {

  return StatusCode::SUCCESS;
}

