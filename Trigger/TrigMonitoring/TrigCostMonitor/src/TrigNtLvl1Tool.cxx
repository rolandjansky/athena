/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Framework
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/StoreGateSvc.h"

// Trigger
#include "TrigSteeringEvent/Lvl1Result.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"
#include "TrigT1Result/RoIBResult.h"

// Local
#include "TrigCostMonitor/TrigNtLvl1Tool.h"

//---------------------------------------------------------------------------------------
Trig::TrigNtLvl1Tool::TrigNtLvl1Tool(const std::string &name,
             const std::string &type,
             const IInterface  *parent)
  :AthAlgTool(name, type, parent)
{
  declareInterface<Trig::ITrigNtTool>(this);

  declareProperty("keyL1Result",     m_keyL1Result = "Lvl1Result");
  declareProperty("keyRBResult",     m_keyRBResult = "");
}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtLvl1Tool::initialize()
{    
  //
  // Get my tools and services
  //

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtLvl1Tool::finalize()
{
  //
  // Clean up
  //  
  ATH_MSG_DEBUG("finalize()" );

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtLvl1Tool::Fill(TrigMonConfig *)
{
  //
  // Nothing to do - this tool does not need TrigMonConfig
  //
  return true;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtLvl1Tool::Fill(TrigMonEvent &event)
{
  //
  // Collect RoIs and TriggerElements 
  //
  if(!FillFromL1Result(event)) { 
    FillFromRBResult(event);
  }

  return true;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtLvl1Tool::FillFromL1Result(TrigMonEvent &event)
{
  //
  // Second case: use Lvl1ResultAccessTool
  //
  if(!evtStore()->contains<LVL1CTP::Lvl1Result>(m_keyL1Result)) {
    ATH_MSG_DEBUG("Lvl1Result does not exist with key: " << m_keyL1Result );
    return false;
  }

  const LVL1CTP::Lvl1Result* l1Result = 0;

  if(evtStore()->retrieve<LVL1CTP::Lvl1Result>(l1Result, m_keyL1Result).isFailure()) {
    ATH_MSG_WARNING("Error retrieving Lvl1Result from StoreGate" );
    return false;
  } else {
    ATH_MSG_DEBUG("Got Lvl1Result with key: " << m_keyL1Result );
  }

  //
  // Check status
  //
  if(!l1Result || !l1Result->isConfigured()) {
    ATH_MSG_DEBUG("L1Result is not configured" );
    return false;
  }
    
  return Fill(*l1Result, event);
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtLvl1Tool::FillFromRBResult(TrigMonEvent &event)
{
  //
  // Second case: extract L1 decisons from ROIB fragment
  //
  if(!evtStore()->contains<ROIB::RoIBResult>(m_keyRBResult)) {
    ATH_MSG_DEBUG("RoIBResult does not exist with key: " << m_keyRBResult );
    return false;
  }

  const ROIB::RoIBResult* roIBResult = 0;

  if(evtStore()->retrieve(roIBResult, m_keyRBResult).isFailure() || !roIBResult) {
    ATH_MSG_ERROR("Error retrieving RoIBResult from StoreGate" );
    return false;
  }
  
  ATH_MSG_DEBUG("Retrieved ROIBResult:" << m_keyRBResult );

  //
  // Copied code from Lvl1ResultAccessTool
  //
  LVL1CTP::Lvl1Result lvl1Result(true);
  
  // 1.) TAV
  const std::vector<ROIB::CTPRoI> ctpRoIVecAV = roIBResult->cTPResult().TAV();
  for (unsigned int iWord = 0; iWord < ctpRoIVecAV.size(); ++iWord) {
    uint32_t roIWord = ctpRoIVecAV[iWord].roIWord();
    lvl1Result.itemsAfterVeto().push_back(roIWord);
  }

  // 1.) TBP
  const std::vector<ROIB::CTPRoI> ctpRoIVecBP = roIBResult->cTPResult().TBP();
  for (unsigned int iWord=0; iWord < ctpRoIVecBP.size(); ++iWord) {
    uint32_t roIWord = ctpRoIVecBP[iWord].roIWord();
    lvl1Result.itemsBeforePrescale().push_back(roIWord);
  }
  
  // 2.) TAP
  const std::vector<ROIB::CTPRoI> ctpRoIVecAP = roIBResult->cTPResult().TAP();
  for (unsigned int iWord=0; iWord < ctpRoIVecAP.size(); ++iWord) {
    uint32_t roIWord = ctpRoIVecAP[iWord].roIWord();
    lvl1Result.itemsAfterPrescale().push_back(roIWord);
  }

  // make sure TBP, TAP, TAV all have 8 entries !
  while (lvl1Result.itemsBeforePrescale().size() < 8) lvl1Result.itemsBeforePrescale().push_back(0);
  while (lvl1Result.itemsAfterPrescale().size()  < 8) lvl1Result.itemsAfterPrescale() .push_back(0);
  while (lvl1Result.itemsAfterVeto().size()      < 8) lvl1Result.itemsAfterVeto()     .push_back(0);
 
  return Fill(lvl1Result, event);
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtLvl1Tool::Fill(const LVL1CTP::Lvl1Result &l1Result, TrigMonEvent &event)
{
  //
  // Iterate over L1 items and fill L1 decisions in TrigMonL1Item objects
  //
  const unsigned int nitem = l1Result.nItems();

  ATH_MSG_DEBUG("Reading decisions for " << nitem << " L1 item(s)" );

  for(unsigned i = 0; i < nitem; ++i) {
    //
    // Fill trigger decision bits
    //
    std::vector<TrigMonL1Item::Decision> dvec;

    if(l1Result.isPassedAfterVeto(i)) { 
      dvec.push_back(TrigMonL1Item::kPassed);
      dvec.push_back(TrigMonL1Item::kL1AfterVeto);
    }
    
    if(l1Result.isPrescaled(i)) { 
      dvec.push_back(TrigMonL1Item::kPrescaled);
    }

    if(l1Result.isPassedBeforePrescale(i)) { 
      dvec.push_back(TrigMonL1Item::kL1BeforePrescale);
    }

    if(l1Result.isPassedAfterPrescale(i)) { 
      dvec.push_back(TrigMonL1Item::kL1AfterPrescale);
    }

    //
    // Save L1 item decisions if passed
    //
    if(dvec.empty()) continue; 

    TrigMonL1Item mon_item;
    mon_item.setCtpId(i);

    for(std::vector<TrigMonL1Item::Decision>::const_iterator dit = dvec.begin(); dit != dvec.end(); ++dit) {
      mon_item.addDecision(*dit);
    }

    ATH_MSG_DEBUG("Saving L1 item CTPID:" << i 
      << " TBP:" << l1Result.isPassedBeforePrescale(i)
      << " TAP:" << l1Result.isPassedAfterPrescale(i) 
      << " TAV:" << l1Result.isPassedAfterVeto(i) );

    event.add<TrigMonL1Item>(mon_item);
  }

  return true;
}
