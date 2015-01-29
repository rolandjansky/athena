/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Framework
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/StoreGateSvc.h"

// Trigger
#include "TrigSteeringEvent/Chain.h"
#include "TrigSteeringEvent/HLTResult.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"
#include "TrigSteering/SteeringChain.h"
#include "TrigSteering/TrigSteer.h"

// Local
#include "TrigCostMonitor/TrigNtHltRTool.h"

//---------------------------------------------------------------------------------------
Trig::TrigNtHltRTool::TrigNtHltRTool(const std::string &name,
             const std::string &type,
             const IInterface  *parent)
  :AthAlgTool(name, type, parent),
   m_parentAlg(0),
   m_hltTool("HLT::HLTResultAccessTool/HLTResultAccessTool")
{
  declareInterface<Trig::ITrigNtTool>(this);

  declareProperty("keyResult", m_keyResult = "");
  declareProperty("saveFailedChains", m_saveFailedChains = true);
  declareProperty("useSteering", m_useSteering = true);
}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtHltRTool::initialize()
{    
  //
  // Retrieve my tools and services
  //
  CHECK(m_hltTool.retrieve());
  ATH_MSG_DEBUG("Retrieved tool: " << m_hltTool );

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
void Trig::TrigNtHltRTool::SetSteer(const HLT::TrigSteer *ptr)
{
  if(!ptr) {
    ATH_MSG_WARNING("Null HLT::TrigSteer pointer" );
    return;
  }

  m_parentAlg = ptr;
}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtHltRTool::finalize()
{
  //
  // Clean up
  //  
  ATH_MSG_DEBUG("finalize()" );
  

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
  if (m_useSteering == true) return FillFromSteering(event);
  else return FillFromHLTResult(event);
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtHltRTool::FillFromHLTResult(TrigMonEvent &event)
{
  //
  // Retrieve and unpack HLT result - note in Run-2, this may be being made _after_ the OPI mon tool has run.
  // To use this again, might need to ask to re-order the steering, or just use FillFromSteering. It's just as good.
  //
  if(!m_hltTool) {
    ATH_MSG_WARNING("Missing HLT tool: " << m_hltTool );
    return false;
  }
 
  //
  // Read and unpack HLT result
  //
  if(!evtStore()->contains<HLT::HLTResult>(m_keyResult)) {
    ATH_MSG_WARNING("HLTResult does not exist: " << m_keyResult );
    return false;
  }

  const HLT::HLTResult *hlt_result = 0;

  if(evtStore()->retrieve<HLT::HLTResult>(hlt_result, m_keyResult).isFailure() || !hlt_result) {
    ATH_MSG_WARNING("Failed to retrieve HLTResult: " << m_keyResult );
    return false;
  }

  ATH_MSG_DEBUG("Retrieved HLTResult: " << m_keyResult << " containing " << hlt_result->getChainResult().size() << " chain(s)" );
  
  //
  // Determine HLT level
  //
  unsigned level = 0;

  if     (hlt_result->getHLTLevel() == HLT::L2)  { level = 2; }
  else if(hlt_result->getHLTLevel() == HLT::EF)  { level = 3; }
  else if(hlt_result->getHLTLevel() == HLT::HLT) { level = 2; }
  else {
    ATH_MSG_WARNING("Unknown level for HLTResult: " << hlt_result->getHLTLevel() );
    return false;
  }

  //
  // Update hltAccessTool with new HLTResult:
  //
  if(m_hltTool->updateResult(*hlt_result, 0).isFailure()) {
    ATH_MSG_WARNING("HLTResultAccessTool failed to update HLTResult" );
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

    ATH_MSG_DEBUG("Saving TrigNtHltRTool HLTResult [HLTResult] : " << chain );
    
    for(std::vector<TrigMonChain::Decision>::const_iterator dit = dvec.begin(); dit != dvec.end(); ++dit) {
      decis.addDecision(*dit);
    }

    event.add<TrigMonChain>(decis);
  }

  return true;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtHltRTool::FillFromSteering(TrigMonEvent &event)
{
  //
  // Read chain decision bits
  //
  const std::vector<const HLT::SteeringChain *> chains = m_parentAlg->getActiveChains();

  for(std::vector<const HLT::SteeringChain *>::const_iterator it = chains.begin(); it != chains.end(); ++it) {
    const HLT::SteeringChain *chain_steer = *it;
    if(!chain_steer) {
      ATH_MSG_WARNING("Null HLT::SteeringChain pointer!" );
      continue;
    }

    const TrigConf::HLTChain *chain_confg = chain_steer -> getConfigChain();
    if(!chain_confg) {
      ATH_MSG_WARNING("Null TrigConf::HLTChain pointer!" );
      continue;
    }

    //
    // Read decision bits and save chain if passed
    //
    std::vector<TrigMonChain::Decision> dvec;

    if(chain_steer->chainPassed())     dvec.push_back(TrigMonChain::kPassed);
    if(chain_steer->chainPassedRaw())  dvec.push_back(TrigMonChain::kPassedRaw);
    if(chain_steer->isPassedThrough()) dvec.push_back(TrigMonChain::kPassedThrough);
    if(chain_steer->isPrescaled())     dvec.push_back(TrigMonChain::kPrescaled);
    if(chain_steer->isResurrected())   dvec.push_back(TrigMonChain::kResurrected);
    
    if(m_saveFailedChains == false && dvec.empty()) continue;
    
    TrigMonChain decis(chain_confg->level(), chain_confg->chain_counter());

    for(std::vector<TrigMonChain::Decision>::const_iterator dit = dvec.begin(); dit != dvec.end(); ++dit) {
      decis.addDecision(*dit);
    }
    
    ATH_MSG_DEBUG(" TrigNtHltRTool Saving HLTResult [Steering] : " << chain_steer );

    event.add<TrigMonChain>(decis);
  }
  return true;
}