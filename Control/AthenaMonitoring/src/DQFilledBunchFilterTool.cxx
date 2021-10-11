/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "DQFilledBunchFilterTool.h"
#include "AthenaKernel/errorcheck.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

DQFilledBunchFilterTool::DQFilledBunchFilterTool(const std::string& type,const std::string& name,const IInterface* parent)
: AthAlgTool( type, name, parent )
, m_alwaysReturnTrue(false)
, m_invert(false)
{
  declareInterface<IDQFilterTool>(this);
  declareProperty("alwaysReturnTrue", m_alwaysReturnTrue);
  declareProperty("invert", m_invert);
}

DQFilledBunchFilterTool::~DQFilledBunchFilterTool () {}

StatusCode DQFilledBunchFilterTool::initialize() {
  ATH_MSG_VERBOSE("ATLAS Ready initialize");
  if (!m_alwaysReturnTrue) {
    ATH_CHECK(m_bcDataKey.initialize());
  }
  return StatusCode::SUCCESS;
}

bool DQFilledBunchFilterTool::accept() const {
  if (m_alwaysReturnTrue) {
    return true;
  } else {
    const EventContext ctx = Gaudi::Hive::currentContext();
    EventID::number_type bcid = ctx.eventID().bunch_crossing_id();
    SG::ReadCondHandle<BunchCrossingCondData> bcData(m_bcDataKey, ctx);
    bool value = bcData->isFilled(bcid) ^ m_invert;
    ATH_MSG_VERBOSE("Filled bunch DQ tool accept called, value " << value);
    return value;
  }
}
