/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


// Trigger includes
#include "TrigByteStreamCnvSvc.h"


// =============================================================================
// Standard constructor
// =============================================================================
TrigByteStreamCnvSvc::TrigByteStreamCnvSvc(const std::string& name, ISvcLocator* svcLoc)
: ByteStreamCnvSvcBase(name, svcLoc) {}

// =============================================================================
// Standard destructor
// =============================================================================
TrigByteStreamCnvSvc::~TrigByteStreamCnvSvc() {}

// =============================================================================
// Implementation of Service::initialize
// =============================================================================
StatusCode TrigByteStreamCnvSvc::initialize() {
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  // ...
  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of Service::finalize
// =============================================================================
StatusCode TrigByteStreamCnvSvc::finalize() {
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  // ...
  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of IConversionSvc::connectOutput
// The argument outputFile is not used
// =============================================================================
StatusCode TrigByteStreamCnvSvc::connectOutput(const std::string& /*outputFile*/) {
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  // ...
  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of IConversionSvc::commitOutput
// The arguments outputFile and do_commit are not used
// =============================================================================
StatusCode TrigByteStreamCnvSvc::commitOutput(const std::string& /*outputFile*/, bool /*do_commit*/) {
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  // ...
  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}