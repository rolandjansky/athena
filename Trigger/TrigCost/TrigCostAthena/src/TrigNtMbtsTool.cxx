/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <string>

// Framework
#include "GaudiKernel/MsgStream.h"

// Reconstruction and MC
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"

#include "TileEvent/TileContainer.h"
//#include "LArRecEvent/LArCollisionTime.h"

// Local
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"
#include "TrigCostAthena/TrigNtMbtsTool.h"

//---------------------------------------------------------------------------------------
Trig::TrigNtMbtsTool::TrigNtMbtsTool(const std::string &name,
				     const std::string &type,
				     const IInterface  *parent)
  :AlgTool(name, type, parent),
   m_log(0),
   m_storeGate("StoreGateSvc", name),
   m_detStore("DetectorStore", name),
   m_histSvc("THistSvc/THistSvc", name),
   m_tileTBID(0)
{
  declareInterface<Trig::ITrigNtTool>(this);

  declareProperty("mbtsContainerName", m_mbtsContainerName = "MBTSContainer");
  declareProperty("debug",             m_debug             = false);
  declareProperty("mbtsThreshold",     m_mbtsThreshold     = 0.18);
}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtMbtsTool::initialize()
{    
  //
  // Get services
  //
  m_log = new MsgStream(msgSvc(), name());
  
  if(m_storeGate.retrieve().isFailure()) {
    log() << MSG::ERROR << "Could not retrieve StoreGateSvc!" << endreq;
    return StatusCode::FAILURE;
  }

  if(m_detStore.retrieve().isSuccess()) {
    if(m_detStore->retrieve(m_tileTBID).isFailure()) {
      log() << MSG::INFO << "Unable to retrieve TileTBID" << endreq;
    }
    else {
      log() << MSG::INFO << "Retrieved TileTBID" << endreq;
    }
  }
  else {
    log() << MSG::INFO << "Could not retrieve DetectorStore" << endreq;
    return StatusCode::FAILURE;
  }  
  
  if(m_debug && outputLevel() <= MSG::DEBUG) {
    log() << MSG::DEBUG << "dumpDetec--------------------------------------------" << endreq;
    m_detStore->dump();
  }
  
  if(m_histSvc.retrieve().isFailure()) {
    log() << MSG::ERROR << "Failed to retrieve THistSvc" << endreq;
    return StatusCode::FAILURE;
  }

  m_mbtsEvent = Anp::NtupleSvc::Instance("event").Branch<Anp::EventMBTS>("mbts");
  m_mbtsEvent.SetType("Anp::EventMBTS");

  return StatusCode::SUCCESS;
}
//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtMbtsTool::finalize()
{
  //
  // Clean up internal state
  //
  log() << MSG::DEBUG << "finalize()" << endreq;

  delete m_log; m_log = 0;

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtMbtsTool::Fill(TrigMonConfig *confg)
{
  //
  // Copy configuration
  //
  if(!confg) {
    log() << MSG::WARNING << "Null TrigMonConfig pointer" << endreq;
    return false;
  }

  return true;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtMbtsTool::Fill(TrigMonEvent &event)
{
  //
  // Process current event
  //
  
  if(m_debug && outputLevel() <= MSG::DEBUG) {
    log() << MSG::INFO << "dumpStore--------------------------------------------" << endreq;
    m_storeGate->dump();
  }

  // step 1
  /*
  const LArCollisionTime* larCollisionTime = 0;
  
  float lar_A_time = -1.0e6;
  float lar_C_time = +1.0e6;

  int lar_A_ncells = 0;
  int lar_C_ncells = 0;

  if(m_storeGate->contains<LArCollisionTime>("LArCollisionTime") &&
     m_storeGate->retrieve(larCollisionTime,"LArCollisionTime").isSuccess()) {
    lar_A_ncells = larCollisionTime->ncellA();
    lar_A_time   = larCollisionTime->timeA();
    
    lar_C_ncells = larCollisionTime->ncellC();
    lar_C_time   = larCollisionTime->timeC();
  }
  else {
    lar_A_time = -100000;
    lar_C_time = +100000;
  }
  */

  // step 2: MBTS A/C time difference

  if(!m_tileTBID) { 
    log() << MSG::DEBUG << "Invalid TileTBID handle... stop." << endreq;
    return true;
  }
  
  if(!m_storeGate->contains<TileCellContainer>(m_mbtsContainerName)) {
    log() << MSG::DEBUG << "NoTileCellContainer with key " << m_mbtsContainerName  << endreq;
    return true;
  }

  const TileCellContainer *tileCellCnt = 0;
  if(m_storeGate->retrieve(tileCellCnt, m_mbtsContainerName).isFailure()) {
    log() << MSG::DEBUG << "No TileCellContainer with key " << m_mbtsContainerName  << endreq;
    return false;
  }
	
  double   timeA = 0.0, chargeA = 0.0, timeA_w = 0.0;
  double   timeC = 0.0, chargeC = 0.0, timeC_w = 0.0;
  unsigned countA = 0;
  unsigned countC = 0;
  
  Anp::EventMBTS MbtsInfo;

  TileCellContainer::const_iterator itr     = tileCellCnt->begin();
  TileCellContainer::const_iterator itr_end = tileCellCnt->end();
  
  for(; itr != itr_end; ++itr, ++itr) {
    const TileCell *cell = *itr;
    
    // charge cut
    if(!(cell->energy() > m_mbtsThreshold)) { 
      log() << MSG::DEBUG << "Ignoring cell with not positive energy" << endreq;
      continue;
    }
    
    // only endcap
    //if(std::fabs(cell->eta()) < 1.5 ) continue;
    
    // check type, module and channel
    // MBTS Id type    is "side"  +/- 1
    // MBTS Id module  is "phi"   0-7
    // MBTS Id channel is "eta"   0-1   zero is closer to beam pipe

    const int type_id    = m_tileTBID->type   (cell->ID());
    const int module_id  = m_tileTBID->module (cell->ID());
    const int channel_id = m_tileTBID->channel(cell->ID());
    
    // Catch errors
    if(std::abs(type_id) != 1) { 
      log() << MSG::WARNING << "Failed id check" << endreq;
      log() << MSG::WARNING << type_id << endreq;
      continue;
    }
    if(channel_id < 0 || channel_id > 1) { 
      log() << MSG::WARNING << "Failed channel check" << endreq;
      continue;
    }
    if(module_id < 0  || module_id > 7) {
      log() << MSG::WARNING << "Failed module check" << endreq;
      continue;
    }
  
    MbtsInfo.SetCharge(type_id, channel_id, module_id, cell->energy());
    MbtsInfo.SetTime(type_id, channel_id, module_id, cell->time());  
    
    if(type_id > 0)  {
      timeA   += cell->time();
      timeA_w += cell->time()*cell->energy();
      chargeA += cell->energy();
      countA++;
    }
    else {
      timeC   += cell->time();
      timeC_w += cell->time()*cell->energy();
      chargeC += cell->energy();
      countC++;
    }
  }
  
  if(chargeA > 0.0) timeA_w /= chargeA;
  if(chargeC > 0.0) timeC_w /= chargeC;

  if(countA > 0) timeA /= double(countA);
  if(countC > 0) timeC /= double(countC);

  event.addVar(2101, countA);
  event.addVar(2102, chargeA);
  event.addVar(2103, timeA);
  event.addVar(2104, timeA_w);

  event.addVar(2111, countC);
  event.addVar(2112, chargeC);
  event.addVar(2113, timeC);
  event.addVar(2114, timeC_w);

  if(m_mbtsEvent.valid()) {
    m_mbtsEvent.GetRef() = MbtsInfo;
  }

  log() << MSG::DEBUG
	<< "MBTS A count  = " << countA  << endreq
	<< "MBTS A charge = " << chargeA << endreq
	<< "MBTS A time   = " << timeA   << endreq
	<< "MBTS C count  = " << countC  << endreq
	<< "MBTS C charge = " << chargeC << endreq
	<< "MBTS C time   = " << timeC   << endreq
	<< "MBTS A Info charge = " << MbtsInfo.GetCharge(1)  << endreq
	<< "MBTS A Info time   = " << MbtsInfo.GetTime(1)    << endreq
	<< "MBTS C Info charge = " << MbtsInfo.GetCharge(-1) << endreq
	<< "MBTS C Info time   = " << MbtsInfo.GetTime(-1)   << endreq;
  
  return true;
}

