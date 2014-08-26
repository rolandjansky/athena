/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///==================================================
///
/// PileupValidationTools.h, (c) ATLAS Detector software
///
/// Validation for Pileup Production
///
/// Authors : David W. Miller
/// In Collaboration with  : Sven Vahsen
///
/// Date : 16 April 2007
///
///==================================================

#include "PileupRTT/PileupValidationTools.h"
//#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/PileUpEventInfo.h"
#include "TH2.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TriggerInfo.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/StoreGateSvc.h"

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
  PileupRTT::PileupValidationTools::PileupValidationTools(const std::string& t,
                                                          const std::string& n,
                                                          const IInterface* p)
     : AthAlgTool(t,n,p) 
  {
   
    declareInterface<IPileupValidationTools>(this);  
    
  }

  ///============
  /// DESTRUCTOR
  ///============
  PileupRTT::PileupValidationTools::~PileupValidationTools() { }

  ///=============================================
  /// Initialize
  /// --> initialize StoreGate
  /// --> get a handle on the analysis tools
  /// --> book histograms
  ///=============================================
  StatusCode PileupRTT::PileupValidationTools::initialize()
  {
    ATH_MSG_INFO("In PileupRTT::PileupValidationTools::initialize()");
    return StatusCode::SUCCESS;
  }

  ///=============================================
  /// Finalize:
  /// delete any memory allocation from the heap
  ///=============================================
  StatusCode PileupRTT::PileupValidationTools::finalize() {
    ATH_MSG_INFO("In PileupRTT::PileupValidationTools::finalize()");
    return StatusCode::SUCCESS;
  }

  ///==================
  /// Print Event Info
  ///==================
  void PileupRTT::PileupValidationTools::printEventInfo(const EventInfo *evt) {

    ///+++ Print Event Info from EventInfo Object
    ATH_MSG_DEBUG("EventInfo: ");
    ATH_MSG_DEBUG(
		  "  Run Number   : " << evt->event_ID()->run_number()           << endreq
		  << "  Event Number : " << evt->event_ID()->event_number()         << endreq
		  << "  Time Stamp   : " << evt->event_ID()->time_stamp()           << endreq
		  << "  ns Offset    : " << evt->event_ID()->time_stamp_ns_offset() << endreq
		  << "  Lumi Block   : " << evt->event_ID()->lumi_block()           << endreq
		  << "  BCID         : " << evt->event_ID()->bunch_crossing_id()    << endreq
		  << "  User Type    : " << evt->event_type()->user_type()          << endreq);
  }

  ///=====================
  /// Print SubEvent Info
  ///=====================
  void PileupRTT::PileupValidationTools::printSubEventInfo(const PileUpEventInfo::SubEvent subEv) {

    ///+++ Print SubEvent Info
    const EventInfo* sevt = subEv.pSubEvt;
    if (sevt !=0) {
      ATH_MSG_DEBUG("Sub Event Info:");
      ATH_MSG_DEBUG(
		    "  Time (ns)    : " << subEv.time()                             << endreq
		    << "  Index        : " << subEv.index()                            << endreq
		    << "  Provenance   : " << subEv.type()                             << endreq // This is the provenance stuff: signal, minbias, cavern, etc
		    << "  Run Number   : " << sevt->event_ID()->run_number()           << endreq
		    << "  Event Number : " << sevt->event_ID()->event_number()         << endreq
		    << "  ns Offset    : " << sevt->event_ID()->time_stamp_ns_offset() << endreq
		    << "  Lumi Block   : " << sevt->event_ID()->lumi_block()           << endreq
		    << "  BCID         : " << sevt->event_ID()->bunch_crossing_id()    << endreq
		    << "  User Type    : " << sevt->event_type()->user_type()          << endreq);
    }
  }

  ///===============================
  /// Print PileupEvent trigger Info
  ///===============================
  void PileupRTT::PileupValidationTools::printPileupTriggerInfo(const PileUpEventInfo *pevt) {

    ///+++ Print trigger Info from PileUpEventInfo Object
    ATH_MSG_DEBUG("Trigger info:");
    ATH_MSG_DEBUG(
		  //<< "  Status    : " << pevt->trigger_info()->statusElement()     << endreq
		  "  extLvl1ID : " << pevt->trigger_info()->extendedLevel1ID()  << endreq
		  << "  lvl1Type  : " << pevt->trigger_info()->level1TriggerType() << endreq
		  << "  lvl2Info  : " << pevt->trigger_info()->level2TriggerInfo() << endreq);
      ATH_MSG_DEBUG("EventFilter Info ");
    for (unsigned int i = 0; i < pevt->trigger_info()->eventFilterInfo().size(); ++i) {
      ATH_MSG_DEBUG(pevt->trigger_info()->eventFilterInfo() << " ");
    }
  }  
  
  ///===============================
  /// Check for recycled events
  ///===============================
  
  // Input is a 2D histogram with the axes: X = RunNum, Y = EventNum
  //----------------------------------------------------------------
  bool PileupRTT::PileupValidationTools::hasRecycledEvents(TH2* hist, int bcTime, int &Nrepeat) {
  
    ///+++ Print trigger Info from PileUpEventInfo Object
    Nrepeat = (int)hist->GetMaximum();
    if (Nrepeat <= 1) return false;
    int xBin=0,yBin=0,zBin=0;
    hist->GetBinXYZ(hist->GetMaximumBin(),xBin,yBin,zBin);
    ATH_MSG_INFO("Bunch crossing with t=" << bcTime << " has: " << endreq
                      << "N   = " << Nrepeat  << " identical " << hist->GetName() << " events" << endreq 
                      << "Bin = " << hist->GetMaximumBin() << endreq
                      << "Run# = " << xBin << endreq
                      << "Evt# = " << yBin
		 );
    return true;
  }
  
  // Input is a vector list of the overlay events (RunNum, EventNum) in each bunch crossing
  //---------------------------------------------------------------------------------------
  bool PileupRTT::PileupValidationTools::hasRecycledEvents(std::vector< std::pair<int,int> > evtList, int bcTime, int &Nrepeat) {
  
    ///+++ Print trigger Info from PileUpEventInfo Object
    
    /// try to duplicated overlay events in the event list 
    Nrepeat = 0;
    int xBin=0,yBin=0;
    std::vector< std::pair<int,int> >::iterator firstEvt(evtList.begin());
    std::vector< std::pair<int,int> >::iterator lastEvt(evtList.end());
    std::vector< std::pair<int,int> >::iterator findEvtInner,findEvtOuter;
    for (findEvtOuter = firstEvt; findEvtOuter != lastEvt; findEvtOuter++) {  
      for (findEvtInner = firstEvt; findEvtInner != lastEvt; findEvtInner++) {
        if (findEvtOuter != findEvtInner) {
          if ( ((*findEvtInner).first == (*findEvtOuter).first) && 
               ((*findEvtInner).second == (*findEvtOuter).second) ) {
            Nrepeat++;  
            xBin = (*findEvtInner).first;
            yBin = (*findEvtInner).second;     
          }
        }
      }  
    }
    
    if (Nrepeat == 0) return false;
    ATH_MSG_INFO("Bunch crossing with t=" << bcTime << " has: " << endreq
                      << "N    = " << Nrepeat  << " identical events" << endreq 
                      << "Run# = " << xBin << endreq
                      << "Evt# = " << yBin
		 );
    return true;
  }
  
} //end of namespace declaration
