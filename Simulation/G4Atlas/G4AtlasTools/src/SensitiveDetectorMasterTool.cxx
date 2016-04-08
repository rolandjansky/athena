/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SensitiveDetectorMasterTool.h"

SensitiveDetectorMasterTool::SensitiveDetectorMasterTool(const std::string& type,
                                                         const std::string& name,
                                                         const IInterface* parent)
  : AthAlgTool(type,name,parent)
  , m_SenDetList(this)
{
  declareProperty( "SensitiveDetectors", m_SenDetList,
                   "Tool handle array of all sensitive detector tools" );
}

StatusCode SensitiveDetectorMasterTool::initialize()
{
  ATH_MSG_DEBUG("initialize");

  // Retrieve all of the sensitive detector tools.
  // SD creation is deferred until later for multi-threading.
  CHECK(m_SenDetList.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode SensitiveDetectorMasterTool::initializeSDs()
{
  ATH_MSG_VERBOSE( name() << "::initializeSDs()" );

  // Loop through all the sensitive detector tools and
  // initialize them for this thread.
  ATH_MSG_INFO( "Initializing list of " << m_SenDetList.size() <<
                " sensitive detectors." );
  for (auto& isd : m_SenDetList) {
    CHECK(isd->initializeSD());
  }
  return StatusCode::SUCCESS;
}

StatusCode SensitiveDetectorMasterTool::BeginOfAthenaEvent()
{
  // Call setup for all sensitive detectors
  for (auto& isd : m_SenDetList) {
    CHECK(isd->SetupEvent());
  }
  return StatusCode::SUCCESS;
}

StatusCode SensitiveDetectorMasterTool::EndOfAthenaEvent()
{
  // Call gather for all sensitive detectors
  for (auto& isd : m_SenDetList) {
    CHECK(isd->Gather());
  }
  return StatusCode::SUCCESS;
}

StatusCode SensitiveDetectorMasterTool::queryInterface(const InterfaceID& riid,
                                                       void** ppvInterface)
{
  if ( ISensitiveDetectorMasterTool::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<ISensitiveDetectorMasterTool*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthAlgTool::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

