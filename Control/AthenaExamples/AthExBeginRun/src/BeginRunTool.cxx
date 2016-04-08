/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Package includes
#include "BeginRunTool.h"

// Gaudi includes
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ServiceHandle.h"

// Athena includes
#include "StoreGate/StoreGate.h"

// Test conditions
#include "IOVDbTestConditions/IOVDbTestAmdbCorrection.h"


////////////////////////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////////////////////////
BeginRunTool::BeginRunTool( const std::string& type, const std::string& name,
                            const IInterface* parent ) 
  : AthAlgTool( type, name, parent )
{
}

////////////////////////////////////////////////////////////////////////////////
// Initialize tool
////////////////////////////////////////////////////////////////////////////////
StatusCode BeginRunTool::initialize()
{
  ATH_MSG_INFO ("initialize()");

  // Get handle on IncidentSvc
  ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", name());
  StatusCode sc = incidentSvc.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_ERROR ("IncidentSvc not found");
    return sc;
  }
  
  // Register listener for BeginRun
  incidentSvc->addListener(this, "BeginRun");

  return StatusCode::SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////
// Handle incidents
////////////////////////////////////////////////////////////////////////////////
void BeginRunTool::handle(const Incident& inc)
{
  if (inc.type()=="BeginRun") {
    ATH_MSG_INFO ("BeginRun Incident received");
    // ...
    // Here you would do your begin-of-run initializations
    // ...
  }
}

