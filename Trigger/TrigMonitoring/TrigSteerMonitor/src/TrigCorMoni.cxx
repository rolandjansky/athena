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

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"

#include "TrigConfInterfaces/ITrigConfigSvc.h"


#include "StoreGate/StoreGateSvc.h"
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

#include "TrigSteerMonitor/TrigCorMoni.h"

#include "boost/foreach.hpp"

//------------------------------------------------------------------------------------------
TrigCorMoni::TrigCorMoni(const std::string & type,
					 const std::string & name,
					 const IInterface* parent)
  :TrigMonitorToolBase(type, name, parent),   
   m_log(0),
   m_parentAlg(0),
   m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
   m_storeGate("StoreGateSvc", name),
   m_rejectL1(0),
   m_streamL1(0)
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
  m_log = new MsgStream(msgSvc(), name());
  
  //if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << "intitialize()" << endreq;

  if(TrigMonitorToolBase::initialize().isFailure()) {
    log() << MSG::ERROR << " Unable to initialize base class!" << endreq;
    return StatusCode::FAILURE;
  }

  m_parentAlg = dynamic_cast<const HLT::TrigSteer*>(parent());
  if(!m_parentAlg) {
    log() << MSG::ERROR << "Unable to cast the parent algorithm to HLT::TrigSteer!" << endreq;
    return StatusCode::FAILURE;
  }

  m_level = m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::L2 ? "L2" : m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::EF ? "EF" : "HLT" ;
  
  if(m_storeGate.retrieve().isFailure()) {
    log() << MSG::ERROR << "Failed to retrieve StoreGateSvc: " << m_storeGate << endreq;
    return StatusCode::FAILURE;
  }
  
  if(m_configSvc.retrieve().isFailure()) {
    log() << MSG::ERROR << "Failed to retreive  TrigConfigSvc: " << m_configSvc << endreq;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------------------
StatusCode TrigCorMoni::finalize()
{  
  delete m_log; m_log = 0;

  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------------------
StatusCode TrigCorMoni::bookHists()
{ 
  //if(outputLevel() <= MSG::DEBUG) log()<<MSG::DEBUG<<"MMMM bookHistograms "<< endreq;

  // Clear maps
  m_stream2bin.clear();
 
  const TrigConf::HLTChainList *chn_confg = m_configSvc->chainList();
  if(!chn_confg) {
    log() << MSG::WARNING << "Failed to get HLTChainList" << endreq;
    return StatusCode::SUCCESS;
  }

  const TrigConf::CTPConfig *ctp_confg = m_configSvc->ctpConfig();
  if(!ctp_confg) {
    log() << MSG::WARNING << "Failed to get CTPConfig" << endreq;
    return StatusCode::SUCCESS;
  }

  // Loop over the chain list to get
  // only those that match the trigger level (L2 or EF or HLT)
  std::set<std::string> streams;
  std::map<std::string, TrigConf::HLTChain *> chains;

  BOOST_FOREACH(TrigConf::HLTChain *hchn, *chn_confg) {
    if(!hchn) {
      log() << MSG::WARNING << "Null HLTChain pointer" << endreq;
      continue;
    }
    
    if(hchn->level() != m_level) {
      continue;
    }
    
    // Save chains from this level
    //chains.push_back(hchn);
    chains[ hchn->chain_name() ] = hchn;

    // Save all stream tags
    BOOST_FOREACH(TrigConf::HLTStreamTag* htag, hchn->streams()) {
      if(htag)
         streams.insert(htag->stream());
    }
  }

  // Create and register histograms
  TrigMonGroup expertHistograms( this, m_parentAlg->name(), expert );

  m_rejectL1 = new TH1I("Corr_NumberOfAcceptL1_and_rejectHLT", 
			"Number of HLT rejected events for L1 accepts by CTP id",
			256, 0, 256);

  m_streamL1 = new TH2I("Corr_NumberOfacceptL1_and_streamHLT",
			"Number of events per stream for L1 accepts by CTP id",
			256, 0, 256, streams.size(), 0, streams.size());

  m_acceptL1 = new TH2I("Corr_NumberOfacceptL1_and_acceptHLT",
			"Number of events per HLT chain for L1 accepts by CTP id",
			256, 0, 256, chains.size(), 0, chains.size());

  if(expertHistograms.regHist(m_rejectL1).isFailure()){
    log() << MSG::WARNING << "Can't book " << m_rejectL1->GetName() << endreq;
    delete m_rejectL1; m_rejectL1 = 0;
    return StatusCode::SUCCESS;
  }

  if(expertHistograms.regHist(m_streamL1).isFailure()){
    log() << MSG::WARNING << "Can't book " << m_streamL1->GetName() << endreq;
    delete m_streamL1; m_streamL1 = 0;
    return StatusCode::SUCCESS;
  }

  if(expertHistograms.regHist(m_acceptL1).isFailure()){
    log() << MSG::WARNING << "Can't book " << m_acceptL1->GetName() << endreq;
    delete m_acceptL1; m_acceptL1 = 0;
    return StatusCode::SUCCESS;
  }


  unsigned int idxstream = 0;
  for(std::set<std::string>::iterator sit = streams.begin(); sit != streams.end(); ++sit) {
    std::string stream = *sit;

    const int ibin = m_streamL1->GetYaxis()->FindBin(idxstream+0.5);
    m_streamL1->GetYaxis()->SetBinLabel(ibin, stream.c_str());
    m_stream2bin.insert(std::make_pair(stream, idxstream));

    /*
    if(outputLevel() <= MSG::DEBUG) {
      log() << MSG::DEBUG 
	    << "Check bins: " << m_streamL1->GetYaxis()->FindBin(stream.c_str())
	    << " =? " << ibin << endreq;
    }
    */

    ++idxstream;
  }

  //
  // Get L1 items (CTP ids and names) and set bin labels
  //
  BOOST_FOREACH(TrigConf::TriggerItem *item, ctp_confg->menu().items()) {
    if(!item) {
      log() << MSG::WARNING << "Null TriggerItem pointer" << endreq;
      continue;
    }

    const int ibin = m_streamL1->GetXaxis()->FindBin(item->ctpId()+0.5);

    m_rejectL1->GetXaxis()->SetBinLabel(ibin, item->name().c_str());
    m_streamL1->GetXaxis()->SetBinLabel(ibin, item->name().c_str());
    m_acceptL1->GetXaxis()->SetBinLabel(ibin, item->name().c_str());

    /*
    if(outputLevel() <= MSG::DEBUG) {
      log() << MSG::DEBUG 
	    << "Check bins: " << m_streamL1->GetXaxis()->FindBin(item->name().c_str())
	    << " =? " << ibin << endreq;
    }
    */
  }

  //
  // Make map from HLT chains to CTP ids
  // 
  unsigned chain_bin = 0;

  
  for( std::map<std::string, TrigConf::HLTChain *>::const_iterator ich =  chains.begin();
       ich != chains.end(); ich++) {
    //for(unsigned i = 0; i < chains.size(); ++i) {
    //const TrigConf::HLTChain *chain = chains.at(i);
    const TrigConf::HLTChain *chain = ich->second;
    if(!chain) {
      log() << MSG::WARNING << "Null HLTChain pointer" << endreq;
      continue;
    }
    
    const std::vector<const TrigConf::TriggerItem *> items = FindL1Items(*chain);
    Data &data = m_hash2chain[chain->chain_hash_id()];

    for(unsigned t = 0; t < items.size(); ++t) {
      data.ctpids.insert(items.at(t)->ctpId());
    }

    data.bin = ++chain_bin;
    m_acceptL1->GetYaxis()->SetBinLabel(data.bin, ich->first.c_str());
  }

  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------------------
StatusCode TrigCorMoni::fillHists()
{
  //if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << "fillHists()" << endreq;

  if(!m_rejectL1 || !m_streamL1) {
    if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << "Invalid TH1 pointer(s)" << endreq;
    return StatusCode::SUCCESS;
  }

  //
  // Get EventInfo and TriggerInfo for stream tags
  //
  const DataHandle<EventInfo> event_handle;
  if(m_storeGate -> retrieve(event_handle).isFailure()) {
    log() << MSG::WARNING << "Failed to read EventInfo" << endreq;
    return StatusCode::SUCCESS;
  }

  TriggerInfo *trig = event_handle->trigger_info();
  if(!trig) {    
    log() << MSG::WARNING << "Null TriggerInfo pointer" << endreq;
    return StatusCode::SUCCESS;
  }

  LVL1CTP::Lvl1Result resultL1(true);
  if(!getLvl1Result(resultL1)) {
    log() << MSG::DEBUG << "Failed to fill Lvl1Result" << endreq;
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
      else if(outputLevel() <= MSG::DEBUG) {
	log() << MSG::DEBUG << "Failed to find bin for stream: " << stag.name() << endreq;
      }
    }
  }
  
  const std::vector<const HLT::SteeringChain *> chains = m_parentAlg->getActiveChains();

  for(std::vector<const HLT::SteeringChain *>::const_iterator it = chains.begin(); it != chains.end(); ++it) {
    const HLT::SteeringChain *chain_steer = *it;
    if(!chain_steer) {
      log() << MSG::WARNING << "Null HLT::SteeringChain pointer!" << endreq;
      continue;
    }

    const TrigConf::HLTChain *chain_confg = chain_steer -> getConfigChain();
    if(!chain_confg) {
      log() << MSG::WARNING << "Null TrigConf::HLTChain pointer!" << endreq;
      continue;
    }

    //
    // Read decision bits and save chain if passed
    //
    if(!chain_steer->chainPassed()) continue;

    std::map<unsigned,  Data>::const_iterator idata = m_hash2chain.find(chain_confg->chain_hash_id());
    if(idata == m_hash2chain.end()) {
      log() << MSG::DEBUG << "No bin and ctpd ids for chain: " << chain_confg->chain_name() << endreq;
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
  if(m_storeGate->contains<LVL1CTP::Lvl1Result>(m_keyL1Result)) {

    const LVL1CTP::Lvl1Result* l1ptr = 0;    
    if(m_storeGate->retrieve<LVL1CTP::Lvl1Result>(l1ptr, m_keyL1Result).isSuccess() && l1ptr) {
      resultL1 = *l1ptr;
      return true;
    }
    else {
      log() << MSG::WARNING << "Error retrieving Lvl1Result from StoreGate" << endreq;
      return false;
    }
  }
  else {
    if(outputLevel() <= MSG::DEBUG) {
      log() << MSG::DEBUG << "Lvl1Result does not exist with key: " << m_keyL1Result << endreq;
    }
  }

  //
  // Now try to extract L1 decisons from ROIB fragment
  //
  if(!m_storeGate->contains<ROIB::RoIBResult>(m_keyRBResult)) {
    if(outputLevel() <= MSG::DEBUG)
      log() << MSG::DEBUG << "RoIBResult does not exist with key: " << m_keyRBResult << endreq;
    return false;
  }

  const ROIB::RoIBResult* roIBResult = 0;

  if(m_storeGate->retrieve(roIBResult, m_keyRBResult).isFailure() || !roIBResult) {
    log() << MSG::ERROR << "Error retrieving RoIBResult from StoreGate" << endreq;
    return false;
  }
  else {
    if(outputLevel() <= MSG::DEBUG)
      log() << MSG::DEBUG << "Got ROIBResult with key " << m_keyRBResult << endreq;
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
         
         BOOST_FOREACH(const TrigConf::TriggerItem* item, m_configSvc->ctpConfig()->menu().items()) {
            if(!item) {
               log() << MSG::WARNING << "Null TriggerItem pointer" << endreq;
               continue;
            }
	
            const unsigned hashid = TrigConf::HLTUtils::string2hash(item->name());
            if(std::find(hashes.begin(), hashes.end(), hashid) != hashes.end()) {
               items.push_back(item);
            }
         }
      }
      else if(chain.lower_chain_name() != "") {

         BOOST_FOREACH(const TrigConf::TriggerItem* item, m_configSvc->ctpConfig()->menu().items()) {
            if(!item) {
               log() << MSG::WARNING << "Null TriggerItem pointer" << endreq;
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

         BOOST_FOREACH(TrigConf::HLTChain *config, *m_configSvc->chainList()) {
            if(!config) {
               log() << MSG::WARNING << "Null HLTChain pointer" << endreq;
               continue;
            }
	
            if(std::find(hashes.begin(), hashes.end(), config->chain_hash_id()) != hashes.end()) {
               const std::vector<const TrigConf::TriggerItem *> new_items = FindL1Items(*config);
               items.insert(items.end(), new_items.begin(), new_items.end());
            }
         }
      }
      else if(chain.lower_chain_name() != "") {
         BOOST_FOREACH(TrigConf::HLTChain *config, *m_configSvc->chainList()) {
            if(!config) {
               log() << MSG::WARNING << "Null HLTChain pointer" << endreq;
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
