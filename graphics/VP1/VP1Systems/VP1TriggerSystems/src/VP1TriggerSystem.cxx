/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***********************************************************************************
 * @Package : VP1TriggerSystems
 * @Class   : VP1TriggerSystem
 *
 * @brief   : Trigger Data System - access and store trigger data
 *
 * @author  : Manuel Proissl  <mproissl@cern.ch> - University of Edinburgh
 ***********************************************************************************/

//Local includes
#include "VP1TriggerSystems/VP1TriggerSystem.h"

//L1 includes

// check the include of the Chain.h file here below, maybe it's obsolete because it seems it's not used
#include "TrigSteeringEvent/Chain.h" // NEW include - Requested by Francesca Pastore, 13 Sep 2013

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/Feature.h"
#include "TrigDecisionTool/TDTUtilities.h"

//L2 and EF includes
#include "TrigMuonEvent/TrigMuonEFContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"
#include "TrigMuonEvent/TrigMuonEFIsolationContainer.h"
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"
#include "TrigMuonEvent/TileMuFeatureContainer.h"
#include "TrigMuonEvent/TileTrackMuFeatureContainer.h"
#include "TrigMuonEvent/MuonFeatureContainer.h"
#include "TrigMuonEvent/MuonFeatureDetailsContainer.h"

//Other includes
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdio.h>

//DEFs
const float ZERO_LIMIT = 0.00001;


//Trigger System Constructor
//_____________________________________________________________________________________________
VP1Trig::VP1TriggerSystem::VP1TriggerSystem()
  : Logger("VP1TriggerSystem"),
    m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
    m_trigMatch("TrigMatchTool/TrigMatchTool") {log_verbose("constructor");}


//Trigger System Destructor
//_____________________________________________________________________________________________
VP1Trig::VP1TriggerSystem::~VP1TriggerSystem() {log_verbose("destructor");}


//Load Trigger Data from StoreGate (once new event loaded)
//_____________________________________________________________________________________________
bool VP1Trig::VP1TriggerSystem::loadTriggerHandles(StoreGateSvc* m_storeGate, QString triglvl)
{
  log_verbose("loadTriggerHandles()");
  
  //Safety handle check
  if(m_handleL1.size()!=0 || m_handleL2.size()!=0 || m_handleEF.size()!=0)
    clearTriggerHandles();
  
  
  if(triglvl.compare("ALL", Qt::CaseInsensitive)==0 || triglvl.compare("L1", Qt::CaseInsensitive)==0) {
    //######################################## L1 ############################################### >
    log_info("Retrieving L1 data via TrigDecisionTool: start");
    
    //VARs
    std::string trigID;
    int trigcount=0;
    
    //Try to retrieve the TrigDecisionTool
    if(m_trigDec.retrieve().isFailure()) {
      log_fatal("Could not retrieve TrigDecisionTool!");
      return false; //ref: daqstatus
    }
    //Try to retrieve the TrigMatchTool
    if(m_trigMatch.retrieve().isFailure()) {
      log_fatal("Could not retrieve TrigMatchTool!");
      return false; //ref: daqstatus
    }
    
    //Loading entire ChainGroup information via TrigDecisionTool
    const Trig::ChainGroup* m_all = m_trigDec->getChainGroup(".*"); //all triggers
    if(!m_all) {
      log_fatal("ChainGroup <m_all> could not be loaded!");
      return false; //ref: daqstatus
    }
    
    //Retrieve list of valid triggers associated with ChainGroup
    std::vector<std::string> trigList = m_all->getListOfTriggers();
    std::vector<std::string>::iterator trigItr; //Iterator over trigList
    
    //Loop over list of triggers
    for(trigItr=trigList.begin(); trigItr!=trigList.end(); ++trigItr) {
      
      //Safety Check: item not empty!
      if((*trigItr).empty()) continue;
      
      //Process passed triggers
      if(m_trigDec->isPassed((*trigItr))) {
	++trigcount;
	trigID = std::string((*trigItr).c_str());
	//std::cout << "INFO :: Processing trigger chain: " << trigger << "[" << trigcount << "] ";
	
	using namespace Trig;
	Trig::FeatureContainer f = m_trigDec->features(trigID); //get trigger and its objects
	//std::cout << "fsize: " << f.getCombinations().size() << std::endl;
	
	//Export to L1 handle
	VP1TriggerHandleL1 handleL1(f, QString::fromStdString(trigID));
	m_handleL1.push_back(handleL1);
      }//END: passed triggers
    }//END: loop over chain group items
    log_verbose("Retrieving L1 data: complete");
  }
  
  
  if(triglvl.compare("ALL", Qt::CaseInsensitive)==0 || triglvl.compare("L2", Qt::CaseInsensitive)==0) {
    //######################################## L2 ############################################### >
    // Retrieve MuonFeatureDetailsContainer based on
    /* MuFastMon.cxx
       Authors: Akimasa Ishikawa (akimasa.ishikawa@cern.ch)
       Kunihiro Nagano (nagano@mail.cern.ch) */
    log_verbose("Retrieving L2 data: start");
    
    const DataHandle<MuonFeatureDetailsContainer> mfdContainer;
    const DataHandle<MuonFeatureDetailsContainer> lastmfdContainer;
    bool runL2=false;

    while(runL2==false) {
      if(m_storeGate->retrieve(mfdContainer,lastmfdContainer).isSuccess())
	log_verbose("MuonFeatureDetailsContainer retrieved");
      
      if(!mfdContainer) {
	log_fatal("MuonFeatureDetailsContainer not retrieved!");
        return false; //ref: daqstatus
      }
      
      // -----------------------------
      // Dump muonFeatureDetails info
      // -----------------------------
      std::vector<const MuonFeatureDetails*> vec_muonFeatureDetails;
      
      for(; mfdContainer!=lastmfdContainer; mfdContainer++) {
	MuonFeatureDetailsContainer::const_iterator mfd     = mfdContainer->begin();
	MuonFeatureDetailsContainer::const_iterator lastmfd = mfdContainer->end();
        for(; mfd != lastmfd; mfd++) {
          if((*mfd)==0) continue;
          vec_muonFeatureDetails.push_back(*mfd);
        }
      }
      
      VP1TriggerHandleL2 handleL2(vec_muonFeatureDetails);
      m_handleL2.push_back(handleL2);
      
      runL2=true;
    } //END: looping MuonFetureDetails
    log_verbose("Retrieving L2 data: complete");
  }
  
  
  if(triglvl.compare("ALL", Qt::CaseInsensitive)==0 || triglvl.compare("EF", Qt::CaseInsensitive)==0) {
    //######################################## EF ############################################### >
    //--- Note: Adapted from the code TrigEDMchecker.cxx, which was                  ---
    //--- "Adapted from code by A.Hamilton to check trigger EDM; R.Goncalo 21/11/07" ---
    log_verbose("Retrieving EF data: start");
    
    const DataHandle<TrigMuonEFInfoContainer> trigMuon;
    const DataHandle<TrigMuonEFInfoContainer> lastTrigMuon;
    unsigned int muonCounter=0;
    
    if(m_storeGate->retrieve(trigMuon,lastTrigMuon).isSuccess()) {            
      for(int i=0; trigMuon!=lastTrigMuon; ++trigMuon, ++i) {
	TrigMuonEFInfoContainer::const_iterator MuonItr  = trigMuon->begin(); //Iterators over top-level EDM
	TrigMuonEFInfoContainer::const_iterator MuonItrE = trigMuon->end();   //objects (TrigMuonEFInfoContainer)
	
	for(int j=0; MuonItr!=MuonItrE; ++MuonItr, ++j ) { //looping over TrigMuonEFInfo objects
	  TrigMuonEFInfo* muonInfo = (*MuonItr);
	  std::vector<std::string> chains = m_trigMatch->chainsPassedByObject<TrigMuonEFInfo>(muonInfo);
	  std::vector<std::string>::iterator itChain; //iterator over chains
	  QList<QString> chainIDs;
	  
	  for(itChain=chains.begin(); itChain!=chains.end(); ++itChain)
	    chainIDs << QString((*itChain).c_str());
	  
	  VP1TriggerHandleEF handleEF(muonInfo, QString("Muon No. ")+QString::number(++muonCounter), chainIDs);
	  m_handleEF.push_back(handleEF);
	}
      }//loop
    } else {
      log_fatal("Could not retrieve TrigMuonEF from StoreGate!");
      return false; //ref: daqstatus
    }
    log_verbose("Retrieving EF data: complete");
  }//END: EF
  
  return true; //ref: daqstatus  
}


//Clear Trigger Handles (at governing system erase)
//_____________________________________________________________________________________________
void VP1Trig::VP1TriggerSystem::clearTriggerHandles()
{
  log_verbose("Clearing trigger handles");
  m_handleL1.clear();
  m_handleL2.clear();
  m_handleEF.clear();
}
