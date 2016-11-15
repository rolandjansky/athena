/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoring/DQAtlasReadyFilterTool.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

DQAtlasReadyFilterTool::DQAtlasReadyFilterTool(const std::string& type,const std::string& name,const IInterface* parent)
: AthAlgTool( type, name, parent )
, m_fallbackValue(true)
, m_alwaysReturnTrue(false)
, m_valueCache(true)
{
  declareInterface<IDQFilterTool>(this);
 declareProperty("fallbackValue", m_fallbackValue);
 declareProperty("alwaysReturnTrue", m_alwaysReturnTrue);
}
        
DQAtlasReadyFilterTool::~DQAtlasReadyFilterTool () {}
        
StatusCode DQAtlasReadyFilterTool::initialize()
{
  ATH_MSG_VERBOSE("ATLAS Ready initialize");
  // don't register callback if we always return true anyway
  if (m_alwaysReturnTrue) return StatusCode::SUCCESS;

  const DataHandle<AthenaAttributeList> attrList;
  StatusCode sc = detStore()->regFcn(&DQAtlasReadyFilterTool::callback, 
  				     this, attrList,
  				     "/TDAQ/RunCtrl/DataTakingMode");
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Unable to register ATLAS Ready filter tool callback");
    return StatusCode::FAILURE;
  }
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Unable to set up ready value cache");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode DQAtlasReadyFilterTool::updateCache() {
  const AthenaAttributeList* attrList(0);
  StatusCode sc = detStore()->retrieve(attrList, "/TDAQ/RunCtrl/DataTakingMode");
  if (sc.isFailure()) {
    ATH_MSG_WARNING("Unable to retrieve DataTakingMode information; falling back to" << m_fallbackValue);
    m_valueCache = m_fallbackValue;
    return StatusCode::SUCCESS;
  }
  if (attrList != 0) {
    ATH_MSG_VERBOSE("ReadyForPhysics is: " << (*attrList)["ReadyForPhysics"].data<uint32_t>());
    m_valueCache = ((*attrList)["ReadyForPhysics"].data<uint32_t>() != 0);
  }
  return StatusCode::SUCCESS;
}
        
bool DQAtlasReadyFilterTool::accept() const {
  if (m_alwaysReturnTrue) {
    return true;
  } else {
    ATH_MSG_DEBUG("ATLAS Ready accept called, value " << m_valueCache);
    return m_valueCache;
  }
}

StatusCode DQAtlasReadyFilterTool::callback( IOVSVC_CALLBACK_ARGS) {
  ATH_MSG_VERBOSE("ATLAS Ready callback called");
  return updateCache();
}
