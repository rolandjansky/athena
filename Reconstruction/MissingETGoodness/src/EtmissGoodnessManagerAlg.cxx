/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "MissingETGoodness/EtmissGoodnessManagerAlg.h"

#include "xAODEventInfo/EventInfo.h"

#include "MissingETGoodness/EtmissGoodnessManager.h"
#include "MissingETGoodness/TMsgLogger.h"
#include "TROOT.h"


// MB: reference to MET Goodies map for storing derived quantities
MET::Goodies& EtmissGoodnessManagerAlg::goodies(MET::Goodies::instance());


EtmissGoodnessManagerAlg::EtmissGoodnessManagerAlg(const std::string& name, ISvcLocator* pSvcLocator) 
 : AthAlgorithm(name, pSvcLocator)
 , m_version(-1) // latest version
 , m_quality(-1) // unset
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
  MET::TMsgLogger::SetMinLevel(static_cast<MET::TMsgLevel>(msgLevel()));

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
  const xAOD::EventInfo*  evt = 0;
  StatusCode status = StatusCode::SUCCESS;
  // retrieve event info
  status = evtStore()->retrieve(evt);
  if ( !status.isSuccess() || evt==0 ) {
    ATH_MSG_WARNING ("Unable to retrieve EventInfo from StoreGate");
    return status;
  } 

  int eventNumber = evt->eventNumber();
  int runNumber = evt->runNumber();
  //int m_lumiBlock = evt->event_ID()->lumi_block();
  //int m_timeStamp = evt->event_ID()->time_stamp();
  ATH_MSG_DEBUG ("Got run number = " << runNumber << ", event number = " << eventNumber);

  goodies.setEntry(eventNumber);
  goodies.setRunNumber(runNumber);

  ATH_MSG_DEBUG ("execute() successful");

  return StatusCode::SUCCESS;
}


StatusCode EtmissGoodnessManagerAlg::finalize() 
{
  ATH_MSG_DEBUG ("finalize()");

  ATH_MSG_DEBUG ("finalize() successful");

  return StatusCode::SUCCESS;
}


