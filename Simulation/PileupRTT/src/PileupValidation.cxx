/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///==================================================
///
/// PileupValidation.h, (c) ATLAS Detector software
///
/// Validation for Pileup Production
///
/// Authors : David W. Miller
/// In Collaboration with  : Sven Vahsen
///
/// Date : 16 April 2007
///
///==================================================

#include "PileupRTT/PileupValidation.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TH1.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/PileUpEventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TriggerInfo.h"

///=================================
#include <iostream>
#include <sstream>
#include <fstream>
#include "TTree.h"
///=================================

namespace PileupRTT
{

  ///============
  /// CONSTRUCTOR
  ///=============
  PileupRTT::PileupValidation::PileupValidation(const std::string& name,ISvcLocator* pSvcLocator) : 
    AthAlgorithm(name, pSvcLocator),

    m_NPileupEvents(0),
    m_NPileupInTimeEvents(0),
    m_pileupEventTime(0),

    m_theTree(0),
    m_thistSvc(0)

  {
    declareProperty("PileUpEventInfo",   m_pileupInfo    = "OverlayEvent");
    declareProperty("EventInfo",         m_eventInfo     = "MyEvent");
  }

  ///============
  /// DESTRUCTOR
  ///============
  PileupRTT::PileupValidation::~PileupValidation() { }


  ///=============================================
  /// Initialize
  /// --> initialize StoreGate
  /// --> get a handle on the analysis tools
  /// --> book histograms
  ///=============================================
  StatusCode PileupRTT::PileupValidation::initialize()
  {

    ATH_MSG_INFO("Initializing PileupValidation Algorithm");

    ///+++ Get handle to StoreGate (Persistency) Service
    StatusCode sc = service("StoreGateSvc", m_storeGate);
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Unable to retrieve pointer to StoreGateSvc");
        return StatusCode::FAILURE;
    }

    ///+++ Get handle to Histogram Service
    sc = service("THistSvc", m_thistSvc);
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Unable to retrieve pointer to THistSvc");
        return StatusCode::FAILURE;
    }

    ///+++ Get handle on Tools Service
    IToolSvc* toolSvc;
    sc = service("ToolSvc",toolSvc);
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Can't get ToolSvc");
      return StatusCode::FAILURE;
    }
    
    ///+++ Grab the PileupRTT Helper Tools
    if ( toolSvc->retrieveTool("PileupRTT::PileupValidationTools", m_pileupRTTTool).isFailure() ) {
      ATH_MSG_ERROR("Creation of algTool PileupValidationTools FAILED !!");
      return StatusCode::RECOVERABLE;
    }

    m_pileupEventTime = new std::vector<double>;

    ///+++ Book ROOT histograms
    sc = this->bookHistograms();
    if (sc.isFailure()) {
      ATH_MSG_ERROR("bookHistograms failed!");
      return StatusCode::FAILURE;
    }

    //+++ Add branches and leaves to the tree
    sc = this->addBranches();
    if (sc.isFailure()) {
      ATH_MSG_ERROR("addBranches failed!");
      return StatusCode::FAILURE;
    }

    //+++ Normalization counter for number of pileup events
    m_NTotPileup = 0;

    ATH_MSG_INFO("initialize() successful in " << name());
    return StatusCode::SUCCESS;
  }

  ///=============================================
  /// Finalize:
  /// delete any memory allocation from the heap
  ///=============================================
  StatusCode PileupRTT::PileupValidation::finalize() {

    ATH_MSG_INFO("Total # Pileup events = " << m_NTotPileup);
    if (m_NTotPileup > 0) {
      
      ATH_MSG_INFO("Calculating the probability of each min-bias or cavern event being recycled");
      
      /// plot the frequency of re-using each min bias and cavern background event
      std::vector< std::pair<int,int> >::iterator findEvtInner,findEvtOuter;
      
      // Loop over all events used
      for (findEvtOuter = m_OverlayEventsUsed.begin(); findEvtOuter != m_OverlayEventsUsed.end(); findEvtOuter++) {
        long Nrepeat = 0;
        // Count up repetitions by looking at the rest of the events 
        for (findEvtInner = m_OverlayEventsUsed.begin(); findEvtInner != m_OverlayEventsUsed.end(); findEvtInner++) {
          if (findEvtOuter != findEvtInner) {                                     
            if ( ((*findEvtInner).first == (*findEvtOuter).first) && 
                 ((*findEvtInner).second == (*findEvtOuter).second) ) {
              Nrepeat++; 
              m_OverlayEventsUsed.erase(findEvtInner);
              findEvtInner--;                                                           
            }
          }
        } // End inner loop
        
        // Now erase this entry from the vector so we don't double count
        m_OverlayEventsUsed.erase(findEvtOuter);
        findEvtOuter--;
        
        if (Nrepeat > 0) h_eventPercent->Fill((100.*(double)Nrepeat/(double)m_NTotPileup));  // We use 50 instead of 100/2 
        else h_eventPercent->Fill(-0.05);                                                     // Keep track of N events not recycled
      }// End outer loop
    }
    


  ///=============================================
  /// Clear:
  /// clear CBNT members
  ///=============================================


 
    ATH_MSG_DEBUG("in PileupRTT::PileupValidation::CBNT_clear()");

    ///+++ For Athena-Aware NTuple
    m_NPileupEvents = 0;
    m_NPileupInTimeEvents = 0;
    m_pileupEventTime->clear();

    return StatusCode::SUCCESS;
  }

  ///=============================================
  /// Execute:
  /// on event by event
  ///=============================================
  StatusCode PileupRTT::PileupValidation::execute()
  {

    ATH_MSG_DEBUG("in execute()");

    ///+++ Read the Pileup RDO
    StatusCode sc = readEventInfo();
    if (sc.isFailure()) {
      ATH_MSG_ERROR("The method readEventInfo() failed");
      return StatusCode::RECOVERABLE;
    }
    else ATH_MSG_INFO("readEventInfo() successful!");

    return StatusCode::SUCCESS;
  }

  ///=============================================
  /// Pileup RDO Validation
  ///     "readPileupRDO"
  /// --> Read EventInfo and PileUpEventInfo
  /// --> Read MC Truth Info
  ///=============================================

  StatusCode PileupRTT::PileupValidation::readEventInfo() {

    ATH_MSG_INFO("in PileupRTT::PileupValidation::readEventInfo()");
    StatusCode sc = StatusCode::SUCCESS;

    //+++++++++++++++++++++++++++
    // Top-level Event Info
    //+++++++++++++++++++++++++++

    //+++ Histogram to count BC-by-BC (Bunch Crossing) overlay events to check for repeats
    std::vector< std::pair<int,int> > v_perBCMinBiasRecycling;
    std::vector< std::pair<int,int> > v_perBCCavernBckgRecycling;
    std::vector< std::pair<int,int> > v_perBCBeamGasHaloRecycling;
    std::vector< std::pair<int,int> > v_perBCZeroBiasRecycling;
    std::vector< std::pair<int,int> > v_perBCUnknownRecycling;
    
    //+++ Signed integer to keep track of bunch crossing ID
    int bcTime = -123456;
    
    //+++ PileupEventInfo
    const PileUpEventInfo* pevt = 0;
    sc = m_storeGate->retrieve(pevt, m_pileupInfo);
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING("** Could not get pileup event info..skipping this data object");
    }
    else {
      ATH_MSG_INFO("Pileup Info Retrieved Successfully as 'PileUpEventInfo' Object ");
        m_pileupRTTTool->printEventInfo(pevt);
        m_pileupRTTTool->printPileupTriggerInfo(pevt);

        //+++ Get sub-event info object
        PileUpEventInfo::SubEvent::const_iterator it  = pevt->beginSubEvt();
        PileUpEventInfo::SubEvent::const_iterator end = pevt->endSubEvt();
 
        // Loop over all sub events for this event (i.e. all pileup overlay)
        for (; it != end; ++it) {

            // Only consider the overlaid events from here on out
            if ((*it).index() > 0) {
              m_NPileupEvents++;
              m_NTotPileup++;
              m_pileupEventTime->push_back((*it).time());
              int runNum  = (*it).pSubEvt->event_ID()->run_number();
              int evtNum  = (*it).pSubEvt->event_ID()->event_number();
              int Nrepeat = 0;
              // Keep track of ALL overlay events used in this dataset
              m_OverlayEventsUsed.push_back(std::pair<int,int>(runNum,evtNum));
              
              // Check to see if this event is in a new bunch crossing
              if ( (*it).time() != bcTime) {
                // If we found more than 1 of the follwing types of overlay events
                // in the last bunch crossing, then we have a problem.
                // Log in "N Repeat" and "Time Repeat" histos, and issue a message.
                if (m_pileupRTTTool->hasRecycledEvents(v_perBCMinBiasRecycling,bcTime,Nrepeat)) { 
                  h_NMinBiasRepeat->Fill(Nrepeat);
                  h_TimeMinBiasRepeat->Fill(bcTime);
                }
                if (m_pileupRTTTool->hasRecycledEvents(v_perBCCavernBckgRecycling,bcTime,Nrepeat)) {
                  h_NCavernBckgRepeat->Fill(Nrepeat);
                  h_TimeCavernBckgRepeat->Fill(bcTime);
                }
                if (m_pileupRTTTool->hasRecycledEvents(v_perBCBeamGasHaloRecycling,bcTime,Nrepeat)) {
                  h_NBeamGasHaloRepeat->Fill(Nrepeat);
                  h_TimeBeamGasHaloRepeat->Fill(bcTime);
                }
                if (m_pileupRTTTool->hasRecycledEvents(v_perBCZeroBiasRecycling,bcTime,Nrepeat)) {
                  h_NZeroBiasRepeat->Fill(Nrepeat);
                  h_TimeZeroBiasRepeat->Fill(bcTime);
                }
                if (m_pileupRTTTool->hasRecycledEvents(v_perBCUnknownRecycling,bcTime,Nrepeat)) {
                  h_NUnknownRepeat->Fill(Nrepeat);
                  h_TimeUnknownRepeat->Fill(bcTime);
                }
                // Reset the per BC histos and the current BC time
                v_perBCMinBiasRecycling.clear();
                v_perBCCavernBckgRecycling.clear();
                v_perBCBeamGasHaloRecycling.clear();
                v_perBCZeroBiasRecycling.clear();
                v_perBCUnknownRecycling.clear();
                bcTime = (int)(*it).time();
              }  

              // Fill the per BC histos with the runnum, event num for every overlay event in this BC
                   if ( (*it).type()==1 ) v_perBCMinBiasRecycling.push_back(std::pair<int,int>(runNum,evtNum));
              else if ( (*it).type()==2 ) v_perBCCavernBckgRecycling.push_back(std::pair<int,int>(runNum,evtNum));
              else if ( (*it).type()==3 ) v_perBCBeamGasHaloRecycling.push_back(std::pair<int,int>(runNum,evtNum));
              else if ( (*it).type()==4 ) v_perBCZeroBiasRecycling.push_back(std::pair<int,int>(runNum,evtNum));
              else                        v_perBCUnknownRecycling.push_back(std::pair<int,int>(runNum,evtNum));
              
              // Count overlay events in the signal event bunch crossing
              if ((*it).time() == 0) {
                m_NPileupInTimeEvents++;
              }
           }
	  
           m_pileupRTTTool->printSubEventInfo((*it)); 

        }
    }

    return StatusCode::SUCCESS;
  }

  ///=================
  /// Book Histograms
  ///=================

  StatusCode PileupRTT::PileupValidation::bookHistograms() {

    StatusCode sc;
    ATH_MSG_DEBUG("Booking histograms");

    h_eventPercent  = new TH1F("eventPercent","Event recycling percentage",110,-0.05,0.5);
    
    //sc = m_thistSvc->regHist("/AANT/Pileup/eventUsage",  h_eventUsage);
    sc = m_thistSvc->regHist("/AANT/Pileup/eventPercent",h_eventPercent);

    h_NMinBiasRepeat        = new TH1F("NMinBiasRepeat",""       ,10,0,10);
    h_TimeMinBiasRepeat     = new TH1F("TimeMinBiasRepeat",""    ,80,-1000,1000);
    h_NCavernBckgRepeat     = new TH1F("NCavernBckgRepeat",""    ,10,0,10);
    h_TimeCavernBckgRepeat  = new TH1F("TimeCavernBckgRepeat","" ,80,-1000,1000);
    h_NBeamGasHaloRepeat    = new TH1F("NBeamGasHaloRepeat",""   ,10,0,10); 
    h_TimeBeamGasHaloRepeat = new TH1F("TimeBeamGasHaloRepeat","",80,-1000,1000);
    h_NZeroBiasRepeat       = new TH1F("NZeroBiasRepeat",""      ,10,0,10);
    h_TimeZeroBiasRepeat    = new TH1F("TimeZeroBiasRepeat",""   ,80,-1000,1000);
    h_NUnknownRepeat        = new TH1F("NUnknownRepeat",""       ,10,0,10);
    h_TimeUnknownRepeat     = new TH1F("TimeUnknownRepeat",""    ,80,-1000,1000);

    sc = m_thistSvc->regHist("/AANT/Pileup/NMinBiasRepeat",       h_NMinBiasRepeat);
    sc = m_thistSvc->regHist("/AANT/Pileup/TimeMinBiasRepeat",    h_TimeMinBiasRepeat);
    sc = m_thistSvc->regHist("/AANT/Pileup/NCavernBckgRepeat",    h_NCavernBckgRepeat);
    sc = m_thistSvc->regHist("/AANT/Pileup/TimeCavernBckgRepeat", h_TimeCavernBckgRepeat);
    sc = m_thistSvc->regHist("/AANT/Pileup/NBeamGasHaloRepeat",   h_NBeamGasHaloRepeat);
    sc = m_thistSvc->regHist("/AANT/Pileup/TimeBeamGasHaloRepeat",h_TimeBeamGasHaloRepeat);
    sc = m_thistSvc->regHist("/AANT/Pileup/NZeroBiasRepeat",      h_NZeroBiasRepeat);
    sc = m_thistSvc->regHist("/AANT/Pileup/TimeZeroBiasRepeat",   h_TimeZeroBiasRepeat);
    sc = m_thistSvc->regHist("/AANT/Pileup/NUnknownRepeat",       h_NUnknownRepeat);
    sc = m_thistSvc->regHist("/AANT/Pileup/TimeUnknownRepeat",    h_TimeUnknownRepeat);
        
    if (sc.isFailure()) {
      ATH_MSG_INFO("ROOT Hist registration failed!");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;

  }

  ///======================
  /// Add Branches to AANT
  ///======================

  StatusCode PileupRTT::PileupValidation::addBranches() {

    ///=======================================
    ///+++ Add branches and leaves to the tree
    ATH_MSG_DEBUG("Adding Branches to custom AANT");

    m_theTree = new TTree("ValidationTree","ValidationTree");

    m_theTree->Branch("NPileupEvents",m_NPileupEvents,"NPileupEvents/I");
    m_theTree->Branch("NPileupInTimeEvents",m_NPileupInTimeEvents,"NPileupInTimeEvents/I");
    m_theTree->Branch("pileupEventTime",m_pileupEventTime);

    if (StatusCode::SUCCESS == m_thistSvc->regTree("/AANT/ValidationTree",m_theTree)) {
    ATH_MSG_INFO("Booked module ntuple");
  } else {
    ATH_MSG_ERROR("Unable to register module ntuple");
  }

    return StatusCode::SUCCESS;
  }

} //end of namespace declaration
