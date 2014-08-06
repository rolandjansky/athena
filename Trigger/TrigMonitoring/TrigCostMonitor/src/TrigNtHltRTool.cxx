/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Framework
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

// Trigger
#include "TrigSteeringEvent/Chain.h"
#include "TrigSteeringEvent/HLTResult.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"

// Local
#include "TrigCostMonitor/TrigNtHltRTool.h"

//---------------------------------------------------------------------------------------
Trig::TrigNtHltRTool::TrigNtHltRTool(const std::string &name,
				     const std::string &type,
				     const IInterface  *parent)
  :AlgTool(name, type, parent),
   m_log(0),
   m_storeGate("StoreGateSvc", name),
   m_hltTool("HLT::HLTResultAccessTool/HLTResultAccessTool")
{
  declareInterface<Trig::ITrigNtTool>(this);

  declareProperty("keyResult", m_keyResult = "");
  declareProperty("saveFailedChains", m_saveFailedChains = true);
}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtHltRTool::initialize()
{    
  //
  // Retrieve my tools and services
  //
  m_log = new MsgStream(msgSvc(), name());
  
  if(m_storeGate.retrieve().isFailure()) {
    log() << MSG::ERROR << "Could not retrieve StoreGateSvc!" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    log() << MSG::DEBUG << "Retrieved " << m_storeGate << endreq;
  }

  if(m_hltTool.retrieve().isFailure()) {
    log() << MSG::ERROR << "Failed to retreive: " << m_hltTool << endreq;
    return StatusCode::FAILURE;
  }
  else {
    log() << MSG::INFO << "Retrieved tool: " << m_hltTool << endreq;
  }

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtHltRTool::finalize()
{
  //
  // Clean up
  //  
  log() << MSG::DEBUG << "finalize()" << endreq;
  
  delete m_log; m_log = 0;

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtHltRTool::Fill(TrigMonConfig *)
{
  //
  // Nothing to do - this tool does not need TrigMonConfig
  //
  return true;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtHltRTool::Fill(TrigMonEvent &event)
{
  //
  // Retrieve and unpack HLT result
  //
  if(!m_hltTool) {
    log() << MSG::WARNING << "Missing HLT tool: " << m_hltTool << endreq;
    return false;
  }
 
  //
  // Read and unpack HLT result
  //
  if(!m_storeGate->contains<HLT::HLTResult>(m_keyResult)) {
    log() << MSG::WARNING << "HLTResult does not exist: " << m_keyResult << endreq;
    return false;
  }

  const HLT::HLTResult *hlt_result = 0;

  if(m_storeGate->retrieve<HLT::HLTResult>(hlt_result, m_keyResult).isFailure() || !hlt_result) {
    log() << MSG::WARNING << "Failed to retrieve HLTResult: " << m_keyResult << endreq;
    return false;
  }

  if(outputLevel() <= MSG::DEBUG) {
    log() << MSG::DEBUG << "Retrieved HLTResult: " << m_keyResult
	  << " containing " << hlt_result->getChainResult().size() << " chain(s)" << endreq;
  }

  //
  // Determine HLT level
  //
  unsigned level = 0;

  if     (hlt_result->getHLTLevel() == HLT::L2)  { level = 2; }
  else if(hlt_result->getHLTLevel() == HLT::EF)  { level = 3; }
  else if(hlt_result->getHLTLevel() == HLT::HLT) { level = 2; }
  else {
    log() << MSG::WARNING << "Unknown level for HLTResult: " << hlt_result->getHLTLevel() << endreq;
    return false;
  }

  //
  // Update hltAccessTool with new HLTResult:
  //
  if(m_hltTool->updateResult(*hlt_result, 0).isFailure()) {
    log() << MSG::WARNING << "HLTResultAccessTool failed to update HLTResult" << endreq;
    return false;
  }

  //
  // Iterate over HLT chains and extract decisions
  //
  const std::vector<HLT::Chain> &chains = m_hltTool->getChains();

  for(unsigned i = 0; i < chains.size(); ++i) {
    const HLT::Chain &chain = chains.at(i);

    //
    // Collect all decisions bits
    //
    std::vector<TrigMonChain::Decision> dvec;

    if(chain.chainPassed())     dvec.push_back(TrigMonChain::kPassed);
    if(chain.chainPassedRaw())  dvec.push_back(TrigMonChain::kPassedRaw);
    if(chain.isPassedThrough()) dvec.push_back(TrigMonChain::kPassedThrough);
    if(chain.isPrescaled())     dvec.push_back(TrigMonChain::kPrescaled);
    if(chain.isResurrected())   dvec.push_back(TrigMonChain::kResurrected);

    //
    // Save chain if any decision is positive or m_saveFailedChains is true
    //
    if(dvec.empty() && m_saveFailedChains == false) continue;

    TrigMonChain decis(level, chain.getChainCounter());

    if(outputLevel() <= MSG::DEBUG) {
      log() << MSG::DEBUG << "Saving TrigNtHltRTool HLTResult: " << chain << endreq;
    }
    
    for(std::vector<TrigMonChain::Decision>::const_iterator dit = dvec.begin(); dit != dvec.end(); ++dit) {
      decis.addDecision(*dit);
    }

    event.add<TrigMonChain>(decis);
  }

  return true;
}
