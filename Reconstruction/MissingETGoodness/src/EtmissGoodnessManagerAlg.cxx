/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MissingETGoodness/EtmissGoodnessManagerAlg.h"

#include "StoreGate/StoreGate.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "MissingETGoodness/EtmissGoodnessManager.h"
#include "MissingETGoodness/TMsgLogger.h"
#include "TROOT.h"


// MB: reference to MET Goodies map for storing derived quantities
MET::Goodies& EtmissGoodnessManagerAlg::goodies(MET::Goodies::instance());


EtmissGoodnessManagerAlg::EtmissGoodnessManagerAlg(const std::string& name, ISvcLocator* pSvcLocator) 
 : AthAlgorithm(name, pSvcLocator)
 , m_version(-1) // latest version
 , m_quality(-1) // unset
 , m_storeGate(0)
{
  declareProperty( "SetAndLockVersion", m_version );
  declareProperty( "SetAndLockQuality", m_quality );
}


EtmissGoodnessManagerAlg::~EtmissGoodnessManagerAlg()
{ 
} 


StatusCode EtmissGoodnessManagerAlg::initialize() 
{
  ATH_MSG_DEBUG ("initialize()");

  // set output level
  MET::TMsgLogger::SetMinLevel(static_cast<MET::TMsgLevel>(outputLevel()));

  // Get StoreGate service:
  StatusCode status = service("StoreGateSvc", m_storeGate);
  if( !status.isSuccess() ) {
    ATH_MSG_WARNING ("Unable to get StoreGateSvc!");
    return status;
  }

  // now set version and quality
  MET::EtmissGoodnessManager& manager = MET::EtmissGoodnessManager::instance();

  if (m_version>=0) manager.SetAndLockVersion(m_version);
  if (m_quality>=0) {
    if (m_quality>=MET::MAX_QUALITY) { return StatusCode::FAILURE; }
    manager.SetAndLockQuality( static_cast<MET::Quality>(m_quality) );
  }

  // dump summary
  manager.Summary(true);

  ATH_MSG_DEBUG ("initialize() successful");

  return StatusCode::SUCCESS;
}


StatusCode 
EtmissGoodnessManagerAlg::execute() 
{
  ATH_MSG_DEBUG ("execute()");

  MET::EtmissGoodnessManager::instance().ClearEvent();

  // get event number here ...
  const EventInfo*  _evt = 0;
  StatusCode status = StatusCode::SUCCESS;
  // retrieve event info
  status = m_storeGate->retrieve(_evt);
  if ( !status.isSuccess() || _evt==0 ) {
    ATH_MSG_WARNING ("Unable to retrieve EventInfo from StoreGate");
    return status;
  } 

  int m_eventNumber = _evt->event_ID()->event_number();
  int m_runNumber = _evt->event_ID()->run_number();
  //int m_lumiBlock = _evt->event_ID()->lumi_block();
  //int m_timeStamp = _evt->event_ID()->time_stamp();
  ATH_MSG_DEBUG ("Got run number = " << m_runNumber << ", event number = " << m_eventNumber);

  goodies.setEntry(m_eventNumber);
  goodies.setRunNumber(m_runNumber);

  ATH_MSG_DEBUG ("execute() successful");

  return StatusCode::SUCCESS;
}


StatusCode EtmissGoodnessManagerAlg::finalize() 
{
  ATH_MSG_DEBUG ("finalize()");

  ATH_MSG_DEBUG ("finalize() successful");

  return StatusCode::SUCCESS;
}


