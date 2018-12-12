/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoring/DQBadLBFilterTool.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/AttributeListException.h"

DQBadLBFilterTool::DQBadLBFilterTool(const std::string& type,const std::string& name,const IInterface* parent)
: AthAlgTool( type, name, parent )
, m_fallbackValue(true)
, m_alwaysReturnTrue(false)
, m_key("")
{
  declareInterface<IDQFilterTool>(this);
  declareProperty("fallbackValue", m_fallbackValue);
  declareProperty("alwaysReturnTrue", m_alwaysReturnTrue);
  declareProperty("ReadKey", m_key);
}
        
DQBadLBFilterTool::~DQBadLBFilterTool () {}
        
StatusCode DQBadLBFilterTool::initialize()
{
  ATH_MSG_DEBUG("DQ Bad LB Filter tool: fallback and force: " << m_fallbackValue << "; " << m_alwaysReturnTrue );

  ATH_MSG_VERBOSE("ATLAS Ready initialize");
  // don't register key if we always return true anyway
  if (m_alwaysReturnTrue) return StatusCode::SUCCESS;

  ATH_CHECK( m_key.initialize() );
  return StatusCode::SUCCESS;
}

bool DQBadLBFilterTool::accept() const {
  if (m_alwaysReturnTrue) {
    return true;
  } else {
    SG::ReadCondHandle<AthenaAttributeList> rch(m_key);
    const AthenaAttributeList* attrList{*rch};
    if (attrList == 0) {
      ATH_MSG_WARNING("Unable to retrieve DataTakingMode information; falling back to" << m_fallbackValue);
      return m_fallbackValue;
    }
    bool value = (*attrList)["Accept"].data<bool>();
    ATH_MSG_VERBOSE("Bad LB accept called, value " << value);
    return value;
  }
}
