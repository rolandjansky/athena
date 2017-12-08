/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <mutex>
#include "SensitiveDetectorMasterTool.h"

SensitiveDetectorMasterTool::SensitiveDetectorMasterTool(const std::string& type,
                                                         const std::string& name,
                                                         const IInterface* parent)
  : AthAlgTool(type, name, parent)
  , m_senDetTools(this)
{
  declareProperty( "SensitiveDetectors", m_senDetTools,
                   "Tool handle array of all sensitive detector tools" );
}

StatusCode SensitiveDetectorMasterTool::initialize()
{
  ATH_MSG_DEBUG("initialize");

  // Retrieve all of the sensitive detector tools.
  // SD creation is deferred until later for multi-threading.
  CHECK( m_senDetTools.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode SensitiveDetectorMasterTool::initializeSDs()
{
  // This method will be called concurrently in AthenaMT during initialization.
  // We thus conservatively lock the entire method to protect things like the
  // MsgStream usage and downstream clients.
  static std::mutex sdMutex;
  std::lock_guard<std::mutex> sdLock(sdMutex);

  ATH_MSG_VERBOSE( name() << "::initializeSDs()" );

  // Loop through all the sensitive detector tools and
  // initialize them for this thread.
  ATH_MSG_INFO( "Initializing list of " << m_senDetTools.size() <<
                " sensitive detectors." );
  for (auto& isd : m_senDetTools) {
    CHECK( isd->initializeSD() );
  }
  return StatusCode::SUCCESS;
}

StatusCode SensitiveDetectorMasterTool::BeginOfAthenaEvent()
{
  // Call setup for all sensitive detectors
  for (auto& isd : m_senDetTools) {
    CHECK( isd->SetupEvent() );
  }
  return StatusCode::SUCCESS;
}

StatusCode SensitiveDetectorMasterTool::EndOfAthenaEvent()
{
  // Call gather for all sensitive detectors
  for (auto& isd : m_senDetTools) {
    CHECK( isd->Gather() );
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

