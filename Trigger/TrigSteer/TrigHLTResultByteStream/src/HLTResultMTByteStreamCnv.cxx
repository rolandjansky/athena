/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTResultByteStream/HLTResultMTByteStreamCnv.h"
#include "TrigSteeringEvent/HLTResultMT.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthenaKernel/ClassID_traits.h"

// =============================================================================
// Standard constructor
// =============================================================================
HLT::HLTResultMTByteStreamCnv::HLTResultMTByteStreamCnv(ISvcLocator* svcLoc) :
  Converter(ByteStream_StorageType, classID(), svcLoc),
  AthMessaging(msgSvc(), "HLTResultMTByteStreamCnv") {}

// =============================================================================
// Standard destructor
// =============================================================================
HLT::HLTResultMTByteStreamCnv::~HLTResultMTByteStreamCnv() {}

// =============================================================================
// Implementation of Converter::initialize
// =============================================================================
StatusCode HLT::HLTResultMTByteStreamCnv::initialize() {
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of Converter::finalize
// =============================================================================
StatusCode HLT::HLTResultMTByteStreamCnv::finalize() {
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of Converter::createObj
// =============================================================================
StatusCode HLT::HLTResultMTByteStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) {
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of Converter::createRep
// =============================================================================
StatusCode HLT::HLTResultMTByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr) {
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
// CLID
// =============================================================================
const CLID& HLT::HLTResultMTByteStreamCnv::classID() {
  return ClassID_traits<HLT::HLTResultMT>::ID();
}

