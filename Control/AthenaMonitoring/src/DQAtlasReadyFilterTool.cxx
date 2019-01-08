/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoring/DQAtlasReadyFilterTool.h"

DQAtlasReadyFilterTool::DQAtlasReadyFilterTool(const std::string& type,const std::string& name,const IInterface* parent)
: AthAlgTool( type, name, parent )
, m_fallbackValue(true)
, m_alwaysReturnTrue(false)
, m_key("/TDAQ/RunCtrl/DataTakingMode")
{
  declareInterface<IDQFilterTool>(this);
 declareProperty("fallbackValue", m_fallbackValue);
 declareProperty("alwaysReturnTrue", m_alwaysReturnTrue);
}
        
DQAtlasReadyFilterTool::~DQAtlasReadyFilterTool () {}
        
StatusCode DQAtlasReadyFilterTool::initialize()
{
  ATH_MSG_VERBOSE("ATLAS Ready initialize");
  // don't register if we always return true anyway
  if (m_alwaysReturnTrue) return StatusCode::SUCCESS;

  ATH_CHECK( m_key.initialize() );
  return StatusCode::SUCCESS;
}

bool DQAtlasReadyFilterTool::accept() const {
  if (m_alwaysReturnTrue) {
    return true;
  } else {
    SG::ReadCondHandle<AthenaAttributeList> rch(m_key);
    const AthenaAttributeList* attrList{*rch};
    if (attrList == 0) {
      ATH_MSG_WARNING("Unable to retrieve DataTakingMode information; falling back to" << m_fallbackValue);
      return m_fallbackValue;
    }
    ATH_MSG_DEBUG("ReadyForPhysics is: " << (*attrList)["ReadyForPhysics"].data<uint32_t>());
    return ((*attrList)["ReadyForPhysics"].data<uint32_t>() != 0);
  }
}
