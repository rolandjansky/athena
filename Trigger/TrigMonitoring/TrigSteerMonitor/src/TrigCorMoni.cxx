/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <iostream>
#include <sstream>
#include <limits>
#include <vector> 
#include <algorithm>
#include <map>

#include "TH2I.h"

#include "Gaudi/Property.h"

#include "TrigConfInterfaces/ITrigConfigSvc.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TriggerInfo.h"

#include "TrigSteering/TrigSteer.h"
#include "TrigSteering/SteeringChain.h"
#include "TrigSteeringEvent/Lvl1Result.h"
#include "TrigInterfaces/AlgoConfig.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/TriggerItem.h"
#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTStreamTag.h"
#include "TrigConfHLTData/HLTUtils.h"

#include "TrigCorMoni.h"

//------------------------------------------------------------------------------------------
TrigCorMoni::TrigCorMoni(const std::string & type,
					 const std::string & name,
					 const IInterface* parent)
  :TrigMonitorToolBase(type, name, parent),   
   m_parentAlg(0),
   m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
   m_rejectL1(0),
   m_streamL1(0),
   m_acceptL1(0)
{
  declareProperty("keyL1Result",  m_keyL1Result = "Lvl1Result");
  declareProperty("keyRBResult",  m_keyRBResult = "");
  declareProperty("histPath",     m_histPath    = "/EXPERT/TrigSteering");
}

//------------------------------------------------------------------------------------------
TrigCorMoni::~TrigCorMoni()
{
}

//------------------------------------------------------------------------------------------
StatusCode TrigCorMoni::initialize()
{
  ATH_CHECK(TrigMonitorToolBase::initialize());

  m_parentAlg = dynamic_cast<const HLT::TrigSteer*>(parent());
  if(!m_parentAlg) {
    ATH_MSG_ERROR("Unable to cast the parent algorithm to HLT::TrigSteer!");
    return StatusCode::FAILURE;
  }

  m_level = m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::L2 ? "L2" : m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::EF ? "EF" : "HLT" ;

  ATH_CHECK(m_configSvc.retrieve());
  
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------------------
StatusCode TrigCorMoni::bookHists()
{ 
  // Clear maps
  m_stream2bin.clear();
 
  const TrigConf::HLTChainList *chn_confg = m_configSvc->chainList();
  if(!chn_confg) {
    ATH_MSG_WARNING("Failed to get HLTChainList");
    return StatusCode::SUCCESS;
  }

  const TrigConf::CTPConfig *ctp_confg = m_configSvc->ctpConfig();
  if(!ctp_confg) {
    ATH_MSG_WARNING("Failed to get CTPConfig");
    return StatusCode::SUCCESS;
  }

  // Loop over the chain list to get
  // only those that match the trigger level (L2 or EF or HLT)
  std::set<std::string> streams;
  std::map<std::string, TrigConf::HLTChain *> chains;

  for(TrigConf::HLTChain *hchn : *chn_confg) {
    if(!hchn) {
      ATH_MSG_WARNING("Null HLTChain pointer");
      continue;
    }
    
    if(hchn->level() != m_level) {
      continue;
    }
    
    // Save chains from this level
    //chains.push_back(hchn);
    chains[ hchn->chain_name() ] = hchn;

    // Save all stream tags
    for(TrigConf::HLTStreamTag* htag : hchn->streams()) {
      if(htag)
         streams.insert(htag->stream());
    }
  }

  // Create and register histograms
  TrigMonGroup expertHistograms( this, m_parentAlg->name(), expert );

  m_rejectL1 = new TH1I("Corr_NumberOfAcceptL1_and_rejectHLT", 
			"Number of HLT rejected events for L1 accepts by CTP id",
			512, 0, 512);

  m_streamL1 = new TH2I("Corr_NumberOfacceptL1_and_streamHLT",
			"Number of events per stream for L1 accepts by CTP id",
			512, 0, 512, streams.size(), 0, streams.size());

  m_acceptL1 = new TH2I("Corr_NumberOfacceptL1_and_acceptHLT",
			"Number of events per HLT chain for L1 accepts by CTP id",
			512, 0, 512, chains.size(), 0, chains.size());

  if(expertHistograms.regHist(m_rejectL1).isFailure()){
    ATH_MSG_WARNING("Can't book " << m_rejectL1->GetName());
    delete m_rejectL1; m_rejectL1 = 0;
    return StatusCode::SUCCESS;
  }

  if(expertHistograms.regHist(m_streamL1).isFailure()){
    ATH_MSG_WARNING("Can't book " << m_streamL1->GetName());
    delete m_streamL1; m_streamL1 = 0;
    return StatusCode::SUCCESS;
  }

  if(expertHistograms.regHist(m_acceptL1).isFailure()){
    ATH_MSG_WARNING("Can't book " << m_acceptL1->GetName());
    delete m_acceptL1; m_acceptL1 = 0;
    return StatusCode::SUCCESS;
  }

  //
  //default binning
  char ctpid[20];
  for (int ibin=1;ibin<=m_rejectL1->GetXaxis()->GetNbins(); ibin++) {
    snprintf(ctpid,sizeof(ctpid),"CTPID-%d",ibin);
    m_rejectL1->GetXaxis()->SetBinLabel(ibin, ctpid);
    m_streamL1->GetXaxis()->SetBinLabel(ibin, ctpid);
    m_acceptL1->GetXaxis()->SetBinLabel(ibin, ctpid);
  }

  unsigned int idxstream = 0;
  for(std::set<std::string>::iterator sit = streams.begin(); sit != streams.end(); ++sit) {
    std::string stream = *sit;

    const int ibin = m_streamL1->GetYaxis()->FindBin(idxstream+0.5);
    m_streamL1->GetYaxis()->SetBinLabel(ibin, stream.c_str());
    m_stream2bin.insert(std::make_pair(stream, idxstream));

    ++idxstream;
  }

  //
  // Get L1 items (CTP ids and names) and set bin labels
  //
  for(TrigConf::TriggerItem *item : ctp_confg->menu().items()) {
    if(!item) {
      ATH_MSG_WARNING("Null TriggerItem pointer");
      continue;
    }

    const int ibin = m_streamL1->GetXaxis()->FindBin(item->ctpId()+0.5);

    m_rejectL1->GetXaxis()->SetBinLabel(ibin, item->name().c_str());
    m_streamL1->GetXaxis()->SetBinLabel(ibin, item->name().c_str());
    m_acceptL1->GetXaxis()->SetBinLabel(ibin, item->name().c_str());
  }

  //
  // Make map from HLT chains to CTP ids
  // 
  unsigned chain_bin = 0;
  
  for (const auto& ch : chains) { // std::map<std::string, TrigConf::HLTChain *>
    if(!ch.second) {
      ATH_MSG_WARNING("Null HLTChain pointer");
      continue;
    }
    
    const std::vector<const TrigConf::TriggerItem *> items = FindL1Items(*ch.second);
    Data &data = m_hash2chain[ch.second->chain_hash_id()];

    for(unsigned t = 0; t < items.size(); ++t) {
      data.ctpids.insert(items.at(t)->ctpId());
    }

    data.bin = ++chain_bin;
    m_acceptL1->GetYaxis()->SetBinLabel(data.bin, ch.first.c_str());
  }

  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------------------
StatusCode TrigCorMoni::fillHists()
{
  if(!m_rejectL1 || !m_streamL1) {
    ATH_MSG_DEBUG("Invalid TH1 pointer(s)");
    return StatusCode::SUCCESS;
  }

  //
  // Get EventInfo and TriggerInfo for stream tags
  //
  const DataHandle<EventInfo> event_handle;
  if(evtStore()->retrieve(event_handle).isFailure()) {
    ATH_MSG_WARNING("Failed to read EventInfo");
    return StatusCode::SUCCESS;
  }

  const TriggerInfo *trig = event_handle->trigger_info();
  if(!trig) {    
    ATH_MSG_WARNING("Null TriggerInfo pointer");
    return StatusCode::SUCCESS;
  }

  LVL1CTP::Lvl1Result resultL1(true);
  if(!getLvl1Result(resultL1)) {
    ATH_MSG_DEBUG("Failed to fill Lvl1Result");
    return StatusCode::SUCCESS;
  }

  const std::vector<TriggerInfo::StreamTag> &streams = trig->streamTags();

  if(streams.empty()) {
    for(unsigned i = 0; i < resultL1.nItems(); ++i) {
      if(resultL1.isPassedAfterVeto(i)) {
	m_rejectL1->Fill(i+0.5);
      }
    }
  }
  else {
    for(unsigned j = 0; j < streams.size(); ++j) {
      const TriggerInfo::StreamTag &stag = streams[j];
      
      const std::map<std::string, unsigned>::const_iterator sit = m_stream2bin.find(stag.name());
      if(sit != m_stream2bin.end()) {	
        
        for(unsigned i = 0; i < resultL1.nItems(); ++i) {
          if(resultL1.isPassedAfterVeto(i)) {
            m_streamL1->Fill(i+0.5, sit->second+0.5);
          }
        }
      }
      else {
        ATH_MSG_DEBUG("Failed to find bin for stream: " << stag.name());
      }
    }
  }
  
  const std::vector<const HLT::SteeringChain *> chains = m_parentAlg->getActiveChains();

  for(std::vector<const HLT::SteeringChain *>::const_iterator it = chains.begin(); it != chains.end(); ++it) {
    const HLT::SteeringChain *chain_steer = *it;
    if(!chain_steer) {
      ATH_MSG_WARNING("Null HLT::SteeringChain pointer!");
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
    if(!chain_steer->chainPassed()) continue;

    std::map<unsigned,  Data>::const_iterator idata = m_hash2chain.find(chain_confg->chain_hash_id());
    if(idata == m_hash2chain.end()) {
      ATH_MSG_DEBUG("No bin and ctpd ids for chain: " << chain_confg->chain_name() );
      continue;
    }

    const Data &data = idata->second;

    for(unsigned i = 0; i < resultL1.nItems(); ++i) {
      if(resultL1.isPassedAfterVeto(i) && (data.ctpids.empty() || data.ctpids.count(i) > 0)) {
	m_acceptL1->Fill(i+0.5, data.bin-0.5);
      }
    }
  }

  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------------------
StatusCode TrigCorMoni::finalHists()
{
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------------------
bool TrigCorMoni::getLvl1Result(LVL1CTP::Lvl1Result &resultL1)
{  
  //
  // First, read Lvl1Result from StoreGate
  //
  if(evtStore()->contains<LVL1CTP::Lvl1Result>(m_keyL1Result)) {

    const LVL1CTP::Lvl1Result* l1ptr = 0;    
    if(evtStore()->retrieve<LVL1CTP::Lvl1Result>(l1ptr, m_keyL1Result).isSuccess() && l1ptr) {
      resultL1 = *l1ptr;
      return true;
    }
    else {
      ATH_MSG_WARNING("Error retrieving Lvl1Result from StoreGate" );
      return false;
    }
  }
  else {
      ATH_MSG_DEBUG("Lvl1Result does not exist with key: " << m_keyL1Result );
  }

  //
  // Now try to extract L1 decisons from ROIB fragment
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
  else {
    ATH_MSG_DEBUG("Got ROIBResult with key " << m_keyRBResult );
  }

  // 1.) TAV
  const std::vector<ROIB::CTPRoI> ctpRoIVecAV = roIBResult->cTPResult().TAV();
  for (unsigned int iWord = 0; iWord < ctpRoIVecAV.size(); ++iWord) {
    uint32_t roIWord = ctpRoIVecAV[iWord].roIWord();
    resultL1.itemsAfterVeto().push_back(roIWord);
  }
  
  // 1.) TBP
  const std::vector<ROIB::CTPRoI> ctpRoIVecBP = roIBResult->cTPResult().TBP();
  for (unsigned int iWord=0; iWord < ctpRoIVecBP.size(); ++iWord) {
    uint32_t roIWord = ctpRoIVecBP[iWord].roIWord();
    resultL1.itemsBeforePrescale().push_back(roIWord);
  }
  
  // 2.) TAP
  const std::vector<ROIB::CTPRoI> ctpRoIVecAP = roIBResult->cTPResult().TAP();
  for (unsigned int iWord=0; iWord < ctpRoIVecAP.size(); ++iWord) {
    uint32_t roIWord = ctpRoIVecAP[iWord].roIWord();
    resultL1.itemsAfterPrescale().push_back(roIWord);
  }
  
  // make sure TBP, TAP, TAV all have 8 entries !
  while (resultL1.itemsBeforePrescale().size() < 8) resultL1.itemsBeforePrescale().push_back(0);
  while (resultL1.itemsAfterPrescale().size()  < 8) resultL1.itemsAfterPrescale().push_back(0);
  while (resultL1.itemsAfterVeto().size()      < 8) resultL1.itemsAfterVeto().push_back(0);
  
  return true;
}

//------------------------------------------------------------------------------------------
std::vector<const TrigConf::TriggerItem *> TrigCorMoni::FindL1Items(const TrigConf::HLTChain &chain)
{
   //
   // Find input L1 items for HLT chain: for unseeded chain return empty vector
   //
   std::vector<const TrigConf::TriggerItem *> items;
    
   if(chain.level() == "L2" || chain.level() == "HLT") {
      std::vector<const TrigConf::TriggerItem *> items;  
  
      if(chain.hasMultipleLowerChains()) {
         const std::vector<unsigned int> hashes = chain.lower_chain_hash_ids();
         
         for(const TrigConf::TriggerItem* item : m_configSvc->ctpConfig()->menu().items()) {
            if(!item) {
              ATH_MSG_WARNING("Null TriggerItem pointer" );
              continue;
            }
	
            const unsigned hashid = TrigConf::HLTUtils::string2hash(item->name());
            if(std::find(hashes.begin(), hashes.end(), hashid) != hashes.end()) {
               items.push_back(item);
            }
         }
      }
      else if(chain.lower_chain_name() != "") {

         for(const TrigConf::TriggerItem* item : m_configSvc->ctpConfig()->menu().items()) {
            if(!item) {
              ATH_MSG_WARNING("Null TriggerItem pointer" );
              continue;
            }
	
            if(item->name() == chain.lower_chain_name()) {
               items.push_back(item);
            }
         }
      }
   }
   else if(chain.level() == "EF") {

      if(chain.hasMultipleLowerChains()) {
         const std::vector<unsigned int> hashes = chain.lower_chain_hash_ids();

         for(TrigConf::HLTChain *config : *m_configSvc->chainList()) {
            if(!config) {
              ATH_MSG_WARNING("Null HLTChain pointer" );
              continue;
            }
	
            if(std::find(hashes.begin(), hashes.end(), config->chain_hash_id()) != hashes.end()) {
               const std::vector<const TrigConf::TriggerItem *> new_items = FindL1Items(*config);
               items.insert(items.end(), new_items.begin(), new_items.end());
            }
         }
      }
      else if(chain.lower_chain_name() != "") {
         for(TrigConf::HLTChain *config : *m_configSvc->chainList()) {
            if(!config) {
              ATH_MSG_WARNING("Null HLTChain pointer" );
              continue;
            }
	
            if(config->chain_name() == chain.lower_chain_name()) {
               const std::vector<const TrigConf::TriggerItem *> new_items = FindL1Items(*config);
               items.insert(items.end(), new_items.begin(), new_items.end());
            }
         }
      }
   }
  
   return items;
}
