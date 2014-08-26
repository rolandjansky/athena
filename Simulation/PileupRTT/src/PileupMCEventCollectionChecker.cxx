/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PileupRTT/PileupMCEventCollectionChecker.h"

#include <iostream>	
#include <sstream>                                      // C++ utilities
#include <string>
#include <algorithm>
#include <fstream>

#include "TFile.h"                                    
#include "TNtuple.h"
#include "TROOT.h"
#include "TH1D.h"
#include "TH1I.h"

#include "GaudiKernel/ISvcLocator.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/PileUpEventInfo.h"
/*
#include "DataModel/DataVector.h"
#include "DataModel/ElementLink.h"
#include "DataModel/DataVector.h"
*/
#include "StoreGate/StoreGateSvc.h"             // Storegate stuff
#include "StoreGate/DataHandle.h"
//#include "AthenaKernel/DefaultKey.h"

#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include "GeneratorObjects/McEventCollection.h"

#include "GaudiKernel/ITHistSvc.h"

///////////////////////////////////////////

using namespace PileupRTT;


PileupMCEventCollectionChecker::PileupMCEventCollectionChecker(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),

  m_nExecute(0),
  m_nCollections(0),
  m_nCollectionsWrong(0),
  m_nSubEvents(0),
  m_nNonSeparatorGenEvents(0),
  m_nSubEventsTotal(0),
  m_nNonSeparatorGenEventsTotal(0),

  m_MCEventCollectionNo(0),
  m_GenEventNo(0),
  m_BugTest(0),

  m_NNonSeparatorGenEvents(0),
  m_NSubEvents(0),
  m_SubEventTest(0),

  m_BCID(0),

  m_theTree(0),
  //m_MCEventCollectionNumber(0),
  //m_GenEventNumber(0),
  m_thistSvc(0)

 {
  
  declareProperty("McEventCollection", m_mcTruthInfo = "TruthEvent");
  declareProperty("PileUpEventInfo", m_pileUpEventInfo = "McEventInfo");

  declareProperty("McOutFile",         m_outfile     = "MCEventInfo.txt");
  declareProperty("dumpMcInfo",        m_dumpMCInfo  = false);
}

StatusCode PileupMCEventCollectionChecker::initialize(){

  ATH_MSG_INFO("initialize");
  
  // get the tool service
  IToolSvc* toolSvc;
  StatusCode sc = service("ToolSvc",toolSvc);
  if (StatusCode::SUCCESS != sc) {
    ATH_MSG_ERROR("Unable to retrieve ToolSvc");
    return StatusCode::FAILURE;
  }

  m_MCEventCollectionNo = new std::vector<int>;
  m_GenEventNo = new std::vector<int>;
  m_BugTest = new std::vector<int>;
  m_NNonSeparatorGenEvents = new std::vector<int>;
  m_NSubEvents = new std::vector<int>;
  m_SubEventTest = new std::vector<int>;
  m_BCID = new std::vector<int>;

  if ((service("THistSvc", m_thistSvc)).isFailure()){
    ATH_MSG_FATAL("THistSvc service not found");
    return StatusCode::FAILURE;
  }
  m_theTree = new TTree("MCTestTree","MCTestTree");
  m_theTree->Branch("MCEventCollectionNo",&m_MCEventCollectionNo);
  m_theTree->Branch("GenEventNo",&m_GenEventNo);
  m_theTree->Branch("SignalEventTest",&m_BugTest);
  m_theTree->Branch("NNonSeparatorGenEvents",&m_NNonSeparatorGenEvents);
  m_theTree->Branch("NSubEvents",&m_NSubEvents);
  m_theTree->Branch("SubEventTest",&m_SubEventTest);
  m_theTree->Branch("BCID",&m_BCID);
  if (StatusCode::SUCCESS == m_thistSvc->regTree("/AANT/MCTestTree",m_theTree)) {
    ATH_MSG_INFO("Booked module ntuple");
  } else {
    ATH_MSG_ERROR("Unable to register module ntuple");
  }

  /*m_MCEventCollectionNumber = new TH1I("h_MCEventCollectionNumber","MCEventCollectionNumber",5000,-0.5,4999.5);
  if (StatusCode::SUCCESS != m_thistSvc->regHist("/AANT/h_MCEventCollectionNumber",m_MCEventCollectionNumber)) {
    ATH_MSG_ERROR("Unable to register module histogram m_MCEventCollectionNumber");
  }
  m_GenEventNumber = new TH1I("h_GenEventNumber","GenEventNumber",5000,-0.5,4999.5);
  if (StatusCode::SUCCESS != m_thistSvc->regHist("/AANT/h_GenEventNumber",m_GenEventNumber)) {
    ATH_MSG_ERROR("Unable to register module histogram m_GenEventNumber");
  }*/

  return StatusCode::SUCCESS;
}

StatusCode PileupMCEventCollectionChecker::execute(){

  m_MCEventCollectionNo->clear();
  m_GenEventNo->clear();
  m_BugTest->clear();
  m_NNonSeparatorGenEvents->clear();
  m_NSubEvents->clear();
  m_SubEventTest->clear();

  m_nExecute++;
  ATH_MSG_INFO("execute=========================" << m_nExecute);
    
  const McEventCollection* mcTruth(0);
  StatusCode sc = StatusCode::SUCCESS;
  sc = evtStore()->retrieve(mcTruth, m_mcTruthInfo);
  if( sc.isFailure() ) {
    ATH_MSG_WARNING("MC event not found:" << m_mcTruthInfo);
    return StatusCode::SUCCESS;
  } else {
    m_nCollections++;
  }
  //m_MCEventCollectionNumber->Fill(m_nCollections);
  m_MCEventCollectionNo->push_back(m_nCollections);

  ATH_MSG_INFO("collections=====================" << m_nCollections);
  ATH_MSG_INFO("MCEventCollection successfully retrieved");
  ATH_MSG_DEBUG("Number of GenEvents is " << mcTruth->size());

  // ATH_MSG_XXX, XXX =  VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL

  const char* out = m_outfile.c_str();
  std::ofstream outfile(out);
  //  outfile << "Start of loop XX " << endl;
  if (mcTruth) {
    McEventCollection::const_iterator MCeventItr    = mcTruth->begin();
    McEventCollection::const_iterator MCeventItrEnd = mcTruth->end();

    const HepMC::GenEvent* genEventFirst = ( mcTruth->size() ? (*MCeventItr) : 0) ;
    if(!genEventFirst || genEventFirst->vertices_size() < 2){ 
      m_nCollectionsWrong++;
      m_BugTest->push_back(0);
    } else {
      m_BugTest->push_back(1);
    }
 
    m_nNonSeparatorGenEvents = 0;
    int MCeventNumber(0);
    for(; MCeventItr != MCeventItrEnd; ++MCeventItr, ++MCeventNumber){
      const HepMC::GenEvent* genEvent = (*MCeventItr);
      if(genEvent) {
	//ATH_MSG_DEBUG("genEvent======================="); 
	genEvent->print(outfile);
	
	if(genEvent->vertices_size() >= 2){ 
	  ATH_MSG_DEBUG("GenEvent number " << MCeventNumber << " has >= 2 vertices in event loop " << m_nExecute );
	  //m_GenEventNumber->Fill(MCeventNumber);
	  m_GenEventNo->push_back(MCeventNumber);
	}

	if(genEvent->signal_process_id() < 1 && genEvent->event_number() < 0){
	  //ATH_MSG_DEBUG("GenEvent number " << MCeventNumber << " is a separator : ID = " << genEvent->signal_process_id() << " , event number = " << genEvent->event_number());
	} else {
	  //ATH_MSG_DEBUG("GenEvent number " << MCeventNumber << " is not a separator : ID = " << genEvent->signal_process_id() << " , event number = " << genEvent->event_number());
	  m_nNonSeparatorGenEvents++;
	  m_nNonSeparatorGenEventsTotal++;
	}

	// print info about all particles for this GenEvent
	//ATH_MSG_DEBUG("m_dumpMCInfo=====================");
	HepMC::GenEvent::particle_const_iterator mcItr    = genEvent->particles_begin();
	HepMC::GenEvent::particle_const_iterator mcItrEnd = genEvent->particles_end();
	for (; mcItr != mcItrEnd; ++mcItr) {
	  (*mcItr)->print(outfile);
	}  

      }
    }// loop over McEventCollections
    m_NNonSeparatorGenEvents->push_back(m_nNonSeparatorGenEvents);

    const PileUpEventInfo *PUEI(0);
    sc = evtStore()->retrieve(PUEI, m_pileUpEventInfo);
    if( sc.isFailure() ) {
      ATH_MSG_WARNING("PileUpEventInfo not found: " << m_mcTruthInfo);
      return StatusCode::SUCCESS;
    }

    //const PileUpEventInfo::SubEvent* SE = PUEI->SubEvent();
    PileUpEventInfo::SubEvent::const_iterator SEItr = PUEI->beginSubEvt();
    PileUpEventInfo::SubEvent::const_iterator SEItrEnd = PUEI->endSubEvt();
    
    m_nSubEvents = 0;
    int SENumber(0);
    for(; SEItr != SEItrEnd; ++SEItr, ++SENumber){
      //ATH_MSG_DEBUG("SubEvent number " << SENumber);
      m_nSubEvents++;
      m_nSubEventsTotal++;
      /*if (m_GenEventNo->at(SENumber))*/ m_BCID->push_back(SEItr->BCID());
    }// loop over SubEvents
    m_NSubEvents->push_back(m_nSubEvents);

    if (m_nSubEvents == m_nNonSeparatorGenEvents){
      ATH_MSG_DEBUG("nSubEvents == m_nNonSeparatorGenEvents == " << m_nSubEvents);
    } else {
      ATH_MSG_DEBUG("nSubEvents != m_nNonSeparatorGenEvents");
      ATH_MSG_DEBUG("nSubEvents == " << m_nSubEvents);
      ATH_MSG_DEBUG("nNonSeparatorGenEvents == " << m_nNonSeparatorGenEvents);
    }
    
    if (m_nSubEvents == m_nNonSeparatorGenEvents){
      m_SubEventTest->push_back(1);
    } else {
      m_SubEventTest->push_back(0);
    }

  } // if (mcTruth)
  
  m_theTree->Fill();
  
  return StatusCode::SUCCESS;
}


StatusCode PileupMCEventCollectionChecker::finalize(){

  ATH_MSG_INFO("finalize=================================");

  double fracCollectionsWrong = m_nCollectionsWrong/m_nCollections;
  double fracCollectionsRight = 1 - fracCollectionsWrong;

  ATH_MSG_INFO("fracCollectionsWrong = " << fracCollectionsWrong);
  ATH_MSG_INFO("fracCollectionsRight = " << fracCollectionsRight); 
  
  if (m_nSubEventsTotal == m_nNonSeparatorGenEventsTotal){
    ATH_MSG_INFO("nSubEventsTotal == nNonSeparatorGenEventsTotal == " << m_nSubEventsTotal);
  } else {
    ATH_MSG_INFO("nSubEventsTotal != nNonSeparatorGenEventsTotal");
    ATH_MSG_INFO("nSubEventsTotal == " << m_nSubEventsTotal);
    ATH_MSG_INFO("nNonSeparatorGenEventsTotal == " << m_nNonSeparatorGenEventsTotal);
  }

  m_theTree->Print(); 
  
  return StatusCode::SUCCESS;
}

