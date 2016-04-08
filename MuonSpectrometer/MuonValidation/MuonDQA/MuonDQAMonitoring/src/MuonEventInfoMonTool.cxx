/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// NAME:     MuonEventInfoMonTool.cxx 
// Author:   N. Benekos(Illinois) 
// March 2008
///////////////////////////////////////////////////////////////////////////////////
   

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/AlgFactory.h"

#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/PropertyMgr.h"

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaMonitoring/LogFileMsgStream.h"
 
#include "TTree.h"
#include "TString.h"

#include "MuonDQAMonitoring/MuonEventInfoMonTool.h"
#include "AthenaMonitoring/AthenaMonManager.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TriggerInfo.h"

// trigger includes:
#include "TrigT1Result/CTP_RDO.h"
#include "TrigT1Result/CTP_RIO.h"
#include "TrigT1Result/CTP_Decoder.h"

#include <sstream>
#include <TH1D.h>
#include <TH2D.h>
#include <TMath.h>
#include <TF1.h>
#include <sys/types.h>

namespace MuonDQA { 

  /////////////////////////////////////////////////////////////////////////////
  // *********************************************************************
  // Public Methods
  // ********************************************************************* 

  MuonEventInfoMonTool::MuonEventInfoMonTool( const std::string & type, const std::string & name, const IInterface* parent )
    : ManagedMonitorToolBase( type, name, parent ), m_eventStore(NULL), m_activeStore(NULL), m_log( msgSvc(), name ),
      m_debuglevel(false), mdt_eventstotal(-1), mdt_event_inarea(-1), in_area(false), testcounter(-1), m_eventNumber(0),
      m_hTriggerType(NULL)
  {
    /*---------------------------------------------------------*/ 
    declareProperty("LastEvent",     m_lastEvent=0);
    declareProperty("TriggerTagAdd", m_TriggerTagAdd = true);
  }
  /*---------------------------------------------------------*/
  
 
  /*---------------------------------------------------------*/
  MuonEventInfoMonTool::~MuonEventInfoMonTool()
    /*---------------------------------------------------------*/
  {
    m_log << MSG::INFO << " deleting MuonEventInfoMonTool " << endreq;
  }
 
  /*---------------------------------------------------------*/
  StatusCode MuonEventInfoMonTool:: initialize()
    /*---------------------------------------------------------*/
  {
    StatusCode sc;
    sc = ManagedMonitorToolBase::initialize();
    if(!sc.isSuccess()) return sc;

    // init message stream
    m_log.setLevel(outputLevel());                // individual outputlevel not known before initialise
    m_debuglevel = (m_log.level() <= MSG::DEBUG); // save if threshold for debug printout reached
      
    m_log << MSG::INFO << "initialize MuonEventInfoMonTool" << endreq;
 
    // The StoreGateSvc is where event-by-event information is stored.
    sc = service( "StoreGateSvc", m_eventStore);
    if( sc.isFailure() ) {
      m_log << MSG::FATAL << name() << ": Unable to locate Service StoreGateSvc" << endreq;
      return sc;
    }
    //    ManagedMonitorToolBase::initialize().ignore();  //  Ignore the checking code;
    return StatusCode::SUCCESS;
  }
 
  /*----------------------------------------------------------------------------------*/
  StatusCode MuonEventInfoMonTool::bookHistograms()
    /*----------------------------------------------------------------------------------*/
  {
 
    // init message stream
    m_log.setLevel(outputLevel());                // individual outputlevel not known before initialise
    m_debuglevel = (m_log.level() <= MSG::DEBUG); // save if threshold for debug printout reached
 
    if (m_debuglevel) m_log << MSG::DEBUG << "MuonEventInfoMonTool Histograms being filled" << endreq;
    StatusCode sc = StatusCode::SUCCESS;

    std::string m_generic_path_muonmonitoring = "Muon/MuonEventInfo";
   
    //declare a group of histograms
    MonGroup muonevt_shift( this, m_generic_path_muonmonitoring, run, ATTRIB_MANAGED );
      
    if(newEventsBlock){}
    if(newLumiBlock){}
    if(newRun)
      {      
	if (m_debuglevel) m_log << MSG::DEBUG << "MuonEventInfoMonTool : isNewRun" << endreq;
        
	// Trigger types
	
	// 	m_hTriggerType = new TH1F("TriggerType","NumberOfEventsPerTriggerType", 4, 0., 4.);
	// 	m_hTriggerType->SetFillColor(42);
	// 	m_hTriggerType->GetXaxis()->SetBinLabel(1,"Tile");
	// 	m_hTriggerType->GetXaxis()->SetBinLabel(2,"RPC");
	// 	m_hTriggerType->GetXaxis()->SetBinLabel(3,"TGC");
	// 	m_hTriggerType->GetXaxis()->SetBinLabel(4,"CTP");
	// 	m_hTriggerType->GetYaxis()->SetTitle("Number of Events");        


 	m_hTriggerType = new TH1F("TriggerType","Number_of_Events_per_L1_TriggerType(8 bits)",256, -0.5, 255.5);
        m_hTriggerType->SetFillColor(42);
	
	//    m_hTriggerType->GetXaxis()->SetBinLabel(1,"00000001");
	//    m_hTriggerType->GetXaxis()->SetBinLabel(2,"00000010");
	//    m_hTriggerType->GetXaxis()->SetBinLabel(3,"00000100");
	//    m_hTriggerType->GetXaxis()->SetBinLabel(4,"00001000");
	//    m_hTriggerType->GetXaxis()->SetBinLabel(5,"00010000");
	//    m_hTriggerType->GetXaxis()->SetBinLabel(6,"00100000");
	//    m_hTriggerType->GetXaxis()->SetBinLabel(7,"01000000");
	//    m_hTriggerType->GetXaxis()->SetBinLabel(8,"10000000");
     
	m_hTriggerType->GetXaxis()->SetTitle("L1 trigger word");
	m_hTriggerType->GetYaxis()->SetTitle("Number of Events");        
	

	sc=muonevt_shift.regHist(m_hTriggerType);
	if(sc.isFailure()){
	  m_log << MSG::FATAL << "m_hTriggerType Failed to register histogram " << endreq;       
	  return StatusCode::FAILURE;	
	}

     
	if (m_debuglevel){
	  m_log << MSG::DEBUG << "INSIDE bookHistograms : " << m_hTriggerType << endreq;
	 // m_log << MSG::DEBUG << "SHIFT : " << shift << endreq;
	  m_log << MSG::DEBUG << "RUN : " << run << endreq; 
	  m_log << MSG::DEBUG << "Booked TriggerType successfully" << endreq;}	 
     
      }// isEndOfRun
 
    return sc;

  
  }

  /*----------------------------------------------------------------------------------*/
  StatusCode MuonEventInfoMonTool::fillHistograms()
    /*----------------------------------------------------------------------------------*/ 
  {
 
    StatusCode sc = StatusCode::SUCCESS;
   
    if (m_debuglevel) m_log << MSG::DEBUG << "MuonEventInfoMonTool::EventInfo Monitoring Histograms being filled" << endreq;

    //Retrieve all ingredients needed to build an MuonDQAMonitoringEvent
    MuonDQA::MuonDQAEventInfo eventInfo = retrieveEventInfo();
    return sc;
  }
  /*---------------------------------------------------------*/
  MuonDQA::MuonDQAEventInfo MuonEventInfoMonTool::retrieveEventInfo() const  
  { 
  
    StatusCode sc = StatusCode::SUCCESS;  
    const EventInfo* eventInfo;
    m_log<<MSG::VERBOSE<<"MuonEventInfoMonTool::retrieveEventInfo() called"<<endreq;

   
    MuonDQAEventInfo MuonDQAeventInfo;
       
    sc = m_eventStore->retrieve(eventInfo);
    if ( sc.isFailure() ) {
      m_log << MSG::ERROR << "Could not find eventInfo " << endreq;
      return MuonDQAeventInfo;
    }else{
      if (m_debuglevel) m_log << MSG::DEBUG << "MuonEventInfoMonTool::retrieved eventInfo" << endreq;
    }
 
    //Cast eventID into MuonDQAEventInfo class:
    
    MuonDQAeventInfo.setRunNumber( eventInfo->event_ID()->run_number() ) ;
    MuonDQAeventInfo.setEventNumber( eventInfo->event_ID()->event_number() );
    MuonDQAeventInfo.setEventType( eventInfo->event_type()->user_type() );
    MuonDQAeventInfo.setTimeStamp( eventInfo->event_ID()->time_stamp() );
    // Number of days since 1/1/1970 
    MuonDQAeventInfo.setOffset( eventInfo->event_ID()->time_stamp()/(24*3600));

    // protection against simulated cosmics when the trigger_info() of the event_info is not filled and returns a null pointer. 
    if(eventInfo->trigger_info() != NULL) {
      MuonDQAeventInfo.setTrigType(eventInfo->trigger_info()->level1TriggerType());
    }
    else {
      MuonDQAeventInfo.setTrigType(0);
    }
    
    // Get time of the day for the event and convert from seconds to hours
    MuonDQAeventInfo.setRunTime( float( eventInfo->event_ID()->time_stamp() - ( ((eventInfo->event_ID()->time_stamp()/(24*3600))*24*3600)/3600. ) ) );         
    MuonDQAeventInfo.setLumiBlock(eventInfo->event_ID()->lumi_block() );  
     
    std::string eventTag=m_eventTag;
    MuonDQAeventInfo.setTag( eventTag );
    
    if (m_debuglevel) m_log << MSG::DEBUG << "MuonDQAeventInfo" << MuonDQAeventInfo << endreq;
 
    // Retrieve trigger Info
    TriggerInfo* trig = eventInfo->trigger_info(); 
    //bitset<8> m_l1Trig = 0; 
    //uint m_l1Trig = 0;
   
    // protection against simulated cosmics
    if(trig != NULL) {
      //bitset<8> m_l1Trig = trig->level1TriggerType();
      uint m_l1Trig = (uint) ( trig->level1TriggerType() );
      m_hTriggerType->Fill(m_l1Trig);
    }

    if (m_debuglevel) {
      // protection against simulated cosmics
      if(trig != NULL) {
	m_log << MSG::DEBUG << "MuonEventInfoMonTool : lvl2Info " << endreq;

	for (unsigned int i = 0; i < trig->level2TriggerInfo().size(); ++i)
	  {
	    m_log << MSG::DEBUG << trig->level2TriggerInfo()[i] << " " << endreq;
	  }

	m_log << MSG::DEBUG << "MuonEventInfoMonTool : EventFilterInfo " << endreq;

	for (unsigned int i = 0; i < trig->eventFilterInfo().size(); ++i) {
	  m_log << MSG::DEBUG << trig->eventFilterInfo()[i] << " " << endreq;
	}

	m_log << MSG::DEBUG << "EventFilterInfo " << endreq;
	for (unsigned int i = 0; i < trig->streamTags().size(); ++i) {
	  m_log << MSG::DEBUG << " i " << i << " name " << trig->streamTags()[i].name() << endreq;;
	  m_log << MSG::DEBUG << " type " << trig->streamTags()[i].type() << endreq;;
	  m_log << MSG::DEBUG << " ObeyLumi " << trig->streamTags()[i].obeysLumiblock() << endreq;;
	}
      }
      else{ m_log << MSG::DEBUG <<" Trigger Information Object not Filled! "<< endreq;}
    }

    // Get number of events per Trigger type : 0001 Tile | 0010 RPC | 0100 TGC | 1000 CTP
    //for(int idx = 0; idx < 0; idx++) if(m_l1Trig.test(idx)) m_hTriggerType->Fill(idx);

    // Fill CTP bin for exclusive CTP events only
    //if( m_l1Trig == 8) m_hTriggerType->Fill(3);

    if ( m_TriggerTagAdd ) {

      const DataHandle< CTP_RDO > ctpRDO;
      const DataHandle< CTP_RIO > ctpRIO;

      if(!m_eventStore->contains(ctpRDO, "CTP_RDO") || ! m_eventStore->contains(ctpRIO, "CTP_RIO"))
	{
	  return MuonDQAeventInfo;
	}

      sc = m_eventStore->retrieve( ctpRDO, "CTP_RDO" );
      if ( sc.isFailure() ) {
	m_log << MSG::WARNING << "CTP_RDO trigger info missing, not added to EventTag" << endreq;
	return MuonDQAeventInfo;
      }

      sc = m_eventStore->retrieve( ctpRIO, "CTP_RIO" );
      if ( sc.isFailure() ) {
	m_log << MSG::WARNING << "CTP_RIO trigger info missing, not added to EventTag" << endreq;
	return MuonDQAeventInfo;
      }

      CTP_Decoder ctp;
      ctp.setRDO(ctpRDO);
      const std::vector<CTP_BC>& BCs = ctp.getBunchCrossings();
      
      // now get the data
      //uint16_t l1aPos = ctpRIO->getDetectorEventType() >> 16;
      unsigned int storeBunch = ctpRDO->getL1AcceptBunchPosition();
      
      //const CTP_BC& bunch = ctp.getBunchCrossings().at(l1aPos);
      const CTP_BC& bunch = BCs.at(storeBunch);
      
      MuonDQAeventInfo.setNumberOfTriggerBits(bunch.getTAV().size() + bunch.getTAP().size());
      int nth_bit(0);
      for(unsigned int i=0; i<bunch.getTAV().size(); i++)
	{
	  MuonDQAeventInfo.setTriggerBit(nth_bit, bunch.getTAV().test(i));
	  nth_bit++;
	}
      for(unsigned int i=0; i<bunch.getTAP().size(); i++)
	{
	  MuonDQAeventInfo.setTriggerBit(nth_bit, bunch.getTAP().test(i));
	  nth_bit++;
	}

    }

    return MuonDQAeventInfo;  
    
  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

  StatusCode MuonEventInfoMonTool::procHistograms()
  {
    StatusCode sc = StatusCode::SUCCESS;
 
    if (m_debuglevel) m_log << MSG::DEBUG << "********MuonEventInfoMonTool : " << mdt_eventstotal << endreq;
    if (m_debuglevel) m_log << MSG::DEBUG << "********Last event : " << m_lastEvent << endreq;  


    if (m_debuglevel) {
      m_log << MSG::DEBUG << "********Reached Last Event in MuonEventInfoMonTool !!!" << endreq;	  
      m_log << MSG::DEBUG << "MuonEventInfoMonTool finalize()" << endreq;}
   
    if(endOfEventsBlock){}
    if(endOfLumiBlock){}
    if(endOfRun){
 
    } // isEndOfRun
 
    return sc;
  } 
  const MuonDQAEvent* MuonEventInfoMonTool::retrieveEvent() const {
     
    //Retrieve all ingredients needed to build an MuonDQAEvent
    MuonDQAEventInfo eventInfo = retrieveEventInfo();
    MuonDQAEvent* event = new MuonDQAEvent();
    event->setMuonDQAEventInfo( eventInfo );
    return event;
  }
}//end namespace MuonDQA
