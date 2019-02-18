///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

namespace AthEx {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
DFlowAlg1::DFlowAlg1( const std::string& name, 
                      ISvcLocator* pSvcLocator ) : 
  ::AthReentrantAlgorithm( name, pSvcLocator )
{
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
  ATH_CHECK( m_w_int.initialize() );
  ATH_CHECK( m_r_evtInfo.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode DFlowAlg1::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode DFlowAlg1::execute (const EventContext& ctx) const
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  SG::ReadHandle<xAOD::EventInfo> r_evtInfo (m_r_evtInfo, ctx);
  if (!r_evtInfo.isValid()) {
    ATH_MSG_ERROR("Could not get the EventInfo object. Going to next event");
    return StatusCode::RECOVERABLE;
  }
  ATH_MSG_INFO("evtinfo handle...");
  ATH_MSG_INFO("name: [" << r_evtInfo.name() << "]");
  ATH_MSG_INFO("store [" << r_evtInfo.store() << "]");
  ATH_MSG_INFO("clid: [" << r_evtInfo.clid() << "]");
  ATH_MSG_INFO("ei: " << *r_evtInfo);
  ATH_MSG_INFO("retrieving event-info...");
  unsigned int runnbr = r_evtInfo->runNumber();
  ATH_MSG_INFO("evt-info.runnbr: " << runnbr);
  ATH_MSG_INFO("evt-info.evtnbr: " << r_evtInfo->eventNumber());
  
  ATH_MSG_INFO("myint handle...");
  SG::WriteHandle<int> w_int (m_w_int, ctx);
  ATH_MSG_INFO("name: [" << w_int.name() << "]");
  ATH_MSG_INFO("store [" << w_int.store() << "]");
  ATH_MSG_INFO("clid: [" << w_int.clid() << "]");

  ATH_CHECK( w_int.record (std::make_unique<int>(r_evtInfo->eventNumber())) );

  //redundant check as op = would throw if w_int was not valid (e.g. because if clid/key combo was duplicated)
  if (w_int.isValid()) {
    ATH_MSG_INFO("ptr: " << w_int.cptr());
    ATH_MSG_INFO("val: " << *w_int);
    
    ATH_MSG_INFO("modify myint by value...");
    *w_int = r_evtInfo->eventNumber() + 20;

    ATH_MSG_INFO("ptr: " << w_int.cptr());
    ATH_MSG_INFO("val: " << *w_int);
  }
  return StatusCode::SUCCESS;
}

} //> end namespace AthEx
