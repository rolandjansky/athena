///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DFlowAlg1.cxx 
// Implementation file for class DFlowAlg1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#undef NDEBUG

// AthExStoreGateExample includes
#include "DFlowAlg1.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "CxxUtils/make_unique.h"

namespace AthEx {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
DFlowAlg1::DFlowAlg1( const std::string& name, 
                      ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_r_evtInfo("EventInfo"),
  m_w_int("dflow_int")
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty("IntFlow", 
                  m_w_int,
                  "Data flow of int");

  declareProperty("EvtInfo",
                  m_r_evtInfo,
                  "event info handle");
}

// Destructor
///////////////
DFlowAlg1::~DFlowAlg1()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode DFlowAlg1::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode DFlowAlg1::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode DFlowAlg1::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  if (!m_r_evtInfo.isValid()) {
    ATH_MSG_ERROR("Could not get the EventInfo object. Going to next event");
    return StatusCode::RECOVERABLE;
  }
  ATH_MSG_INFO("evtinfo handle...");
  ATH_MSG_INFO("name: [" << m_r_evtInfo.name() << "]");
  ATH_MSG_INFO("store [" << m_r_evtInfo.store() << "]");
  ATH_MSG_INFO("clid: [" << m_r_evtInfo.clid() << "]");
  const EventInfo* ei = &*m_r_evtInfo;
  ATH_MSG_INFO("ei: " << ei);
  const EventID* eid = ei->event_ID();    
  ATH_MSG_INFO("retrieving event-info...");
  unsigned int runnbr = eid->run_number();
  ATH_MSG_INFO("evt-info.runnbr: " << runnbr);
  ATH_MSG_INFO("evt-info.evtnbr: " << eid->event_number());
  
  ATH_MSG_INFO("myint handle...");
  ATH_MSG_INFO("name: [" << m_w_int.name() << "]");
  ATH_MSG_INFO("store [" << m_w_int.store() << "]");
  ATH_MSG_INFO("clid: [" << m_w_int.clid() << "]");
  
  m_w_int = CxxUtils::make_unique<int>(m_r_evtInfo->event_ID()->event_number());

  //redundant check as op = would throw if m_w_int was not valid (e.g. because if clid/key combo was duplicated)
  if (m_w_int.isValid()) {
    ATH_MSG_INFO("ptr: " << m_w_int.cptr());
    ATH_MSG_INFO("val: " << *m_w_int);
    
    ATH_MSG_INFO("modify myint by value...");
    *m_w_int = m_r_evtInfo->event_ID()->event_number() + 20;

    ATH_MSG_INFO("ptr: " << m_w_int.cptr());
    ATH_MSG_INFO("val: " << *m_w_int);
  }
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace AthEx
