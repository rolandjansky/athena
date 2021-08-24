/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/** @file TRT_DCS_ConditionsSvc.cxx
 *  @brief Service to provide a simple Athena interface to read
 *  DCS conditions data from COOL
 * author Denver Whittington <Denver.Whittington@cern.ch>
 **/
#include "TRT_DCS_ConditionsSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "xAODEventInfo/EventInfo.h"

#include "InDetIdentifier/TRT_ID.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadCondHandle.h"

#include "TH1D.h"
#include "TFile.h"

#include <sstream>

//////////
/// Constructor
/////
TRT_DCS_ConditionsSvc::TRT_DCS_ConditionsSvc( const std::string& name,
					      ISvcLocator* pSvcLocator ) :
  AthService( name, pSvcLocator ),
  m_evtStore("StoreGateSvc",name),
  m_detStore("DetectorStore",name),
  m_mapSvc("TRT_HWMappingSvc",name),
  m_condSvc("CondSvc",name),
  m_barrelReadKey("/TRT/DCS/HV/BARREL"),
  m_EAReadKey("/TRT/DCS/HV/ENDCAPA"),
  m_ECReadKey("/TRT/DCS/HV/ENDCAPC"),
  m_TRT_ID_Helper(nullptr),
  m_numFlagRED(0),
  m_numFlagNOINFO(0),
  m_currentTimestamp(0),
  m_doMonitoring(false),
  m_h_Barrel_nRED(nullptr),
  m_h_EndcapA_nRED(nullptr),
  m_h_EndcapC_nRED(nullptr),
  m_h_Barrel_nNOINFO(nullptr),
  m_h_EndcapA_nNOINFO(nullptr),
  m_h_EndcapC_nNOINFO(nullptr),
  m_h_Barrel_HVvalAvg(nullptr),
  m_h_EndcapA_HVvalAvg(nullptr),
  m_h_EndcapC_HVvalAvg(nullptr),
  m_nBAEvts(0),
  m_nEAEvts(0),
  m_nECEvts(0)
{
  // Get properties from job options
  declareProperty( "Barrel_HV_COOLFolderName", m_Barrel_HV_COOLFolderName = "/TRT/DCS/HV/BARREL" );
  declareProperty( "EndcapA_HV_COOLFolderName", m_EndcapA_HV_COOLFolderName = "/TRT/DCS/HV/ENDCAPA" );
  declareProperty( "EndcapC_HV_COOLFolderName", m_EndcapC_HV_COOLFolderName = "/TRT/DCS/HV/ENDCAPC" );
  declareProperty( "VeryVerbose", m_VeryVerbose = false );
  declareProperty( "HV_WarningValueLow",  m_HVWarnValHi = 2000. );
  declareProperty( "HV_WarningValueHigh", m_HVWarnValLo = 1000. );
  declareProperty( "HWMapSvc", m_mapSvc );
  declareProperty( "DoIOVChecking", m_doIOVchecking = false );
  declareProperty( "IOVmaxLength", m_IOVmaxLength = 86400 /* 24 hours */ );
  declareProperty( "FallBackOnCOOLChanNames", m_FallBackOnCOOLChanNames = false );
  declareProperty( "EventStore", m_evtStore );
  declareProperty( "DetectorStore", m_detStore );
  declareProperty( "DoMonitoring", m_doMonitoring );
  if ( m_doIOVchecking ) {
    ATH_MSG_WARNING( "DoIOVChecking is deprecated and does nothing. Please remove from your job options configuration." );
    m_doIOVchecking = false;
  }
  // initialize caches
  m_evtBA.push_back(-1);
  m_evtEA.push_back(-1);
  m_evtEC.push_back(-1);
  m_Barrel_HV_COOLCont.push_back(nullptr);
  m_EndcapA_HV_COOLCont.push_back(nullptr);
  m_EndcapC_HV_COOLCont.push_back(nullptr);
}

//////////
/// Destructor
/////
TRT_DCS_ConditionsSvc::~TRT_DCS_ConditionsSvc() {}

//////////
/// Initialize
/////
StatusCode TRT_DCS_ConditionsSvc::initialize() {
  if (msgLvl(MSG::DEBUG)) msg() << "Initialize." << endmsg;
  StatusCode sc(StatusCode::SUCCESS);

  // Retrieve the EventStore and DetectorStore
  sc = m_evtStore.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR( "Unable to retrieve " << m_evtStore );
  }
  sc = m_detStore.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to retrieve " << m_detStore << endmsg;
  }

  // initialize DataHandle keys
  ATH_CHECK( m_EventInfoKey.initialize());
  ATH_CHECK( m_barrelReadKey.initialize() );
  ATH_CHECK( m_EAReadKey.initialize() );
  ATH_CHECK( m_ECReadKey.initialize() );

  // Get the TRT Identifier Helper.
  sc = m_detStore->retrieve( m_TRT_ID_Helper, "TRT_ID" );
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR( "Unable to retrieve pointer to TRT ID Helper." );
    return sc;
  }

  // Get the TRT_HWMappingSvc
  sc = m_mapSvc.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR( "Couldn't get " << m_mapSvc );
    return sc;
  }

  return sc;
}

//////////
/// getFlag
/////
InDet::TRT_DCS_StatusFlag TRT_DCS_ConditionsSvc::getFlag( const Identifier ident ) {
  StatusCode sc(StatusCode::SUCCESS);

  // Temporary Solution: check voltage against upper and lower warning levels
  InDet::TRT_DCS_ValueType theVoltage = -10.;
  sc = getValue( ident, InDet::TRT_DCS_HV_VOLTAGE, theVoltage );
  if ( sc.isFailure() ) {
    if (m_VeryVerbose) ATH_MSG_VERBOSE( "Failed to get HV voltage value. Returning NOINFO" );
    m_numFlagNOINFO++;
    return InDet::TRT_DCS_NOINFO;
  }
  if ( theVoltage < m_HVWarnValLo || theVoltage > m_HVWarnValHi ) {
    if (m_VeryVerbose) ATH_MSG_VERBOSE( "Voltage out of valid range. (" << theVoltage << ") Flagging RED." );
    m_numFlagRED++;
    return InDet::TRT_DCS_RED;
  }

  if (m_VeryVerbose) ATH_MSG_VERBOSE( "Voltage = " << theVoltage << " Flagging GREEN" );
  return InDet::TRT_DCS_GREEN;
}

//////////
/// getValue
/////
StatusCode TRT_DCS_ConditionsSvc::getValue( const Identifier ident,
					    InDet::TRT_DCS_DataType dataType,
					    InDet::TRT_DCS_ValueType& theValue ) {
  StatusCode sc(StatusCode::SUCCESS);

  // Decode the identifier
  int barrel_ec       = m_TRT_ID_Helper->barrel_ec( ident );
  int phi_slice       = m_TRT_ID_Helper->phi_module( ident );
  int module_or_wheel = m_TRT_ID_Helper->layer_or_wheel( ident );
  int straw_layer     = m_TRT_ID_Helper->straw_layer( ident );
  int straw           = m_TRT_ID_Helper->straw( ident );

  // HV Voltage
  if ( dataType == InDet::TRT_DCS_HV_VOLTAGE ) {

    // Construct the DCS folder name
    std::string folderName = "";
    if ( abs(barrel_ec) == 1 ) folderName = m_Barrel_HV_COOLFolderName;
    if ( barrel_ec == 2 ) folderName = m_EndcapA_HV_COOLFolderName;
    if ( barrel_ec == -2 ) folderName = m_EndcapC_HV_COOLFolderName;
    if ( folderName.empty() ) {
      ATH_MSG_ERROR( "Invalid barrel/endcap identifier given." );
      return StatusCode::FAILURE;
    }

    // Get the Channel Number
    int chanNum = -1;
    chanNum = m_mapSvc->get_HV_CoolChanNum( ident );
    if ( chanNum < 0 ) {
      ATH_MSG_WARNING( "Failed to get COOL channel number from map tool: "
		       << barrel_ec << "," << phi_slice << "," << module_or_wheel << ","
		       << straw_layer << "," << straw );
    }

    // Get the value for the determined folder name and channel number
    sc = getValue( folderName, chanNum, theValue );
    if ( sc.isFailure() ) {
      // Try to get the value by channel name?
      if ( m_FallBackOnCOOLChanNames ) {
	// Get the Channel Name
	std::string chanName = "";
	chanName = m_mapSvc->get_HV_CoolChanName( ident );
	if ( chanName.empty() ) {
	  ATH_MSG_WARNING( "Failed to get COOL channel name from map tool: "
			   << barrel_ec << "," << phi_slice << "," << module_or_wheel << ","
			   << straw_layer << "," << straw );
	  return StatusCode::FAILURE;
	}
	// Get the value for the determined folder name and channel name
	sc = getValue( folderName, chanName, theValue );
	if ( sc.isFailure() ) {
	  ATH_MSG_WARNING( "Failed to fall back on COOL channel name." );
	  return sc;
	}
      } else return StatusCode::FAILURE;
    }

  } else {
    ATH_MSG_WARNING( "getValue called for unknown datatype" );
    return StatusCode::FAILURE;
  }

  return sc;
}

//////////
/// getValue by folder name and channel name
/////
StatusCode TRT_DCS_ConditionsSvc::getValue( const std::string & foldername,
					    const std::string & chanName,
					    InDet::TRT_DCS_ValueType& theValue ) {
  StatusCode sc(StatusCode::SUCCESS);

  /*
  //  Specify foldername and chanName in COOL form
  //  e.g. foldername = "/TRT/DCS/HV/BARREL"
  //       chanName   = "HVB_S24_M3_B3_OutputVoltage"
   */

  // Get the folder from the CondStore.
  const CondAttrListCollection* DCScondFolder = getCollection(foldername);
  if ( !DCScondFolder ) {
    sc = m_detStore->retrieve( DCScondFolder, foldername );
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING( "Couldn't retrieve folder " << foldername
		       << " from DetectorStore.  Has it been loaded into IOVDbSvc?" );
      return StatusCode::FAILURE;
    }
  }
   if ( !DCScondFolder ) return StatusCode::FAILURE;
  // Get the channel number for this channel name.
  int chanNum = -1;
  CondAttrListCollection::name_const_iterator chanNameMapItr;
  if ( DCScondFolder->name_size() == 0 ) {
    ATH_MSG_WARNING( "This CondAttrListCollection has no entries"
		     << " in its ChanNameMap.  Won't be able to get channel numbers." );
  }
  for ( chanNameMapItr = DCScondFolder->name_begin();
	chanNameMapItr != DCScondFolder->name_end(); ++chanNameMapItr ) {
    if ( (*chanNameMapItr).second == chanName ) {
      chanNum = (*chanNameMapItr).first;
      break;
    }
  }

  // Get the requested channel/value pair
  CondAttrListCollection::const_iterator chanAttrListPair;
  chanAttrListPair = DCScondFolder->chanAttrListPair( chanNum );
  if ( chanAttrListPair == DCScondFolder->end() ) {
    ATH_MSG_VERBOSE( "Channel " << chanName
		     << " not found in folder " << foldername
		     << " for this IOV." );
    return StatusCode::FAILURE;
  }
  const CondAttrListCollection::AttributeList& attrList = (*chanAttrListPair).second;

  // Get the value
  theValue = attrList["OUTPUTVOLTAGE_VALUE"].data<InDet::TRT_DCS_ValueType>();

  return sc;
}

//////////
/// getValue by folder name and COOL channel number
/////
StatusCode TRT_DCS_ConditionsSvc::getValue( const std::string & foldername,
					    const int chanNum,
					    InDet::TRT_DCS_ValueType& theValue ) {

  /*
  //  Directly access the value by folder channel number
  //  e.g. foldername = "/TRT/DCS/HV/BARREL"
  //       chanNum    = e.g. result from HV map query
   */

  // Get the folder from the CondStore.
  const CondAttrListCollection* DCScondFolder = getCollection(foldername);
  if ( !DCScondFolder ) {
    StatusCode sc = m_detStore->retrieve( DCScondFolder, foldername );
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING( "Couldn't retrieve folder " << foldername
		       << " from DetectorStore.  Has it been loaded into IOVDbSvc?" );
      return sc;
    }
  }
 if ( !DCScondFolder ) return StatusCode::FAILURE;
  // Get the requested channel/value pair
  CondAttrListCollection::const_iterator chanAttrListPair;
  chanAttrListPair = DCScondFolder->chanAttrListPair( chanNum );
  if ( chanAttrListPair == DCScondFolder->end() ) {
    ATH_MSG_VERBOSE( "Channel " << chanNum
		     << " not found in folder " << foldername
		     << " for this IOV." );
    return StatusCode::FAILURE;
  }
  const CondAttrListCollection::AttributeList& attrList = (*chanAttrListPair).second;

  // Get the value
  theValue = attrList["OUTPUTVOLTAGE_VALUE"].data<InDet::TRT_DCS_ValueType>();

  return StatusCode::SUCCESS;
}

//////////
/// StatusFlag for TRT_ConditionsSummarySvc (by Identifier)
/////
InDet::TRT_CondFlag TRT_DCS_ConditionsSvc::condSummaryStatus( const Identifier& ident ) {

  InDet::TRT_CondFlag condFlag(InDet::TRT_COND_NOINFO);
  InDet::TRT_DCS_StatusFlag theFlag = getFlag( ident );

  if ( theFlag == InDet::TRT_DCS_GREEN ) condFlag = InDet::TRT_COND_GOOD;
  if ( theFlag == InDet::TRT_DCS_RED   ) condFlag = InDet::TRT_COND_BAD;

  if ( m_VeryVerbose && theFlag != InDet::TRT_DCS_GREEN ) {
    int det = m_TRT_ID_Helper->barrel_ec( ident );
    int phi = m_TRT_ID_Helper->phi_module( ident );
    int lay = m_TRT_ID_Helper->layer_or_wheel( ident );
    int strawLay = m_TRT_ID_Helper->straw_layer( ident );
    int straw = m_TRT_ID_Helper->straw( ident );
    std::string chanName = m_mapSvc->get_HV_CoolChanName( ident );
    // InDet::TRT_DCS_StatusFlag enum defined in ITRT_DCS_ConditionsSvc.h
    ATH_MSG_VERBOSE("Channel " << det << " " << phi << " " << lay << " " << strawLay << " " << straw << " on line " << chanName << " has flag " << theFlag);
  }

  return condFlag;
}

//////////
/// Finalize
/////
StatusCode TRT_DCS_ConditionsSvc::finalize() {
  ATH_MSG_INFO( "Reported NOINFO " << m_numFlagNOINFO << " times." );
  ATH_MSG_INFO( "Reported RED " << m_numFlagRED << " times." );
  ATH_MSG_INFO( "If these are suspicious numbers, turn on VERBOSE output and set VeryVerbose=True to see more info." );
  if ( m_doMonitoring ) {
    m_h_Barrel_HVvalAvg->Scale(1./double(m_nBAEvts));
    m_h_EndcapA_HVvalAvg->Scale(1./double(m_nEAEvts));
    m_h_EndcapC_HVvalAvg->Scale(1./double(m_nECEvts));
    TFile* outFile = new TFile("TRT_DCS_Monitoring.root","RECREATE");
    bool file = outFile->cd();
    if (file){
    	m_h_Barrel_nRED->Write();
    	m_h_Barrel_nNOINFO->Write();
    	m_h_Barrel_HVvalAvg->Write();
    	m_h_EndcapA_nRED->Write();
    	m_h_EndcapA_nNOINFO->Write();
    	m_h_EndcapA_HVvalAvg->Write();
    	m_h_EndcapC_nRED->Write();
    	m_h_EndcapC_nNOINFO->Write();
    	m_h_EndcapC_HVvalAvg->Write();
    }
    outFile->Close();
  }
  return StatusCode::SUCCESS;
}

//////////
/// get pointer
//////////
const CondAttrListCollection* TRT_DCS_ConditionsSvc::getCollection( const std::string & foldername) {
  const EventContext& event_context=Gaudi::Hive::currentContext();
  EventContext::ContextID_t slot=event_context.slot();
  EventContext::ContextEvt_t event_id=event_context.evt();
  std::lock_guard<std::mutex> lock(m_cacheMutex);
  if(foldername == m_Barrel_HV_COOLFolderName  ) {
    if(slot>=m_evtBA.size()) {
      m_evtBA.resize(slot+1);
      m_Barrel_HV_COOLCont.resize(slot+1);
    }    
    if(m_evtBA[slot]!=event_id) {
      SG::ReadCondHandle<CondAttrListCollection> rst(m_barrelReadKey,event_context);
      m_evtBA[slot]=event_id;
      m_Barrel_HV_COOLCont[slot]=(*rst);
      monitorBarrel();
    }
    return  m_Barrel_HV_COOLCont[slot];

  } else if(foldername == m_EndcapA_HV_COOLFolderName  ) {
    if(slot>=m_evtEA.size()) {
      m_evtEA.resize(slot+1);
      m_EndcapA_HV_COOLCont.resize(slot+1);
    }    
    if(m_evtEA[slot]!=event_id) {
      SG::ReadCondHandle<CondAttrListCollection> rst(m_EAReadKey,event_context);
      m_evtEA[slot]=event_id;
      m_EndcapA_HV_COOLCont[slot]=(*rst);
      monitorEndcapA();
    }
    return  m_EndcapA_HV_COOLCont[slot];

  } else if (foldername == m_EndcapC_HV_COOLFolderName  ) {
    if(slot>=m_evtEC.size()) {
      m_evtEC.resize(slot+1);
      m_EndcapC_HV_COOLCont.resize(slot+1);
    }    
    if(m_evtEC[slot]!=event_id) {
      SG::ReadCondHandle<CondAttrListCollection> rst(m_ECReadKey,event_context);
      m_evtEC[slot]=event_id;
      m_EndcapC_HV_COOLCont[slot]=(*rst);
      monitorEndcapC();
    }
    return  m_EndcapC_HV_COOLCont[slot];

  } else {
    ATH_MSG_WARNING( " TRT DCS HV folder requested with bad folder name " );
  }
  return nullptr; 
}


/// Monitor barrel HV
/////
void TRT_DCS_ConditionsSvc::monitorBarrel(){

    StatusCode sc(StatusCode::SUCCESS);

    // Get the current event timestamp
    m_currentTimestamp = 0;
    SG::ReadHandle<xAOD::EventInfo> evtInfo(m_EventInfoKey);

    if (not evtInfo.isValid()) {
      ATH_MSG_WARNING( "Couldn't get " << m_EventInfoKey.key()
		       << " from StoreGate." );
      return;
    }

    m_currentTimestamp = evtInfo->timeStamp();
    if (m_VeryVerbose) ATH_MSG_VERBOSE( "Event timestamp: " << m_currentTimestamp );

    // Monitoring Histograms
    if ( m_doMonitoring ) {
      CondAttrListCollection::name_const_iterator chanNameMapItr;
      m_nBAEvts++;

      const CondAttrListCollection* clc = getCollection(m_Barrel_HV_COOLFolderName);      
      // Set up Monitoring Histograms if not done (first event)
      if (!m_h_Barrel_HVvalAvg) {
	m_h_Barrel_nRED = new TH1D("Barrel_nRED","Barrel - nRED",clc->name_size(),1,clc->name_size());
	m_h_Barrel_nNOINFO = new TH1D("Barrel_nNOINFO","Barrel - nNOINFO",clc->name_size(),1,clc->name_size());
	m_h_Barrel_HVvalAvg = new TH1D("Barrel_HVvalAvg","Barrel - HVvalAvg",clc->name_size(),1,clc->name_size());
	for ( chanNameMapItr = clc->name_begin();
	      chanNameMapItr != clc->name_end(); ++chanNameMapItr ) {
	  std::string chanName( (*chanNameMapItr).second );
	  int chanNum( (*chanNameMapItr).first );
	  m_h_Barrel_nRED->GetXaxis()->SetBinLabel(chanNum,chanName.c_str());
	  m_h_Barrel_nNOINFO->GetXaxis()->SetBinLabel(chanNum,chanName.c_str());
	  m_h_Barrel_HVvalAvg->GetXaxis()->SetBinLabel(chanNum,chanName.c_str());
	}
      }

      // Fill Monitoring Histograms

      for ( chanNameMapItr = clc->name_begin();
	    chanNameMapItr != clc->name_end(); ++chanNameMapItr ) {
	std::string chanName( (*chanNameMapItr).second );
	int chanNum( (*chanNameMapItr).first );
	InDet::TRT_DCS_StatusFlag theFlag;
	InDet::TRT_DCS_ValueType theVoltage = -10.;
	sc = getValue( m_Barrel_HV_COOLFolderName, chanNum, theVoltage );
	if ( sc.isFailure() ) theFlag = InDet::TRT_DCS_NOINFO;
	else if ( theVoltage < m_HVWarnValLo || theVoltage > m_HVWarnValHi ) theFlag = InDet::TRT_DCS_RED;
	else theFlag = InDet::TRT_DCS_GREEN;
	if ( theFlag == InDet::TRT_DCS_RED ) m_h_Barrel_nRED->Fill(chanNum);
	if ( theFlag == InDet::TRT_DCS_NOINFO ) m_h_Barrel_nNOINFO->Fill(chanNum);
	if (!sc.isFailure()) m_h_Barrel_HVvalAvg->Fill(chanNum,theVoltage);
      }

    } // end of doMonitoring

  return;
}
/// Monitor EndcapA HV
/////
void TRT_DCS_ConditionsSvc::monitorEndcapA(){

    StatusCode sc(StatusCode::SUCCESS);

    // Get the current event timestamp
    m_currentTimestamp = 0;
    SG::ReadHandle<xAOD::EventInfo> evtInfo(m_EventInfoKey);

    if (not evtInfo.isValid()) {
      ATH_MSG_WARNING( "Couldn't get " << m_EventInfoKey.key()
		       << " from StoreGate." );
      return;
    }

    m_currentTimestamp = evtInfo->timeStamp();
    if (m_VeryVerbose) ATH_MSG_VERBOSE( "Event timestamp: " << m_currentTimestamp );

    // Monitoring Histograms
    if ( m_doMonitoring ) {
      CondAttrListCollection::name_const_iterator chanNameMapItr;
      m_nEAEvts++;

      const CondAttrListCollection* clc = getCollection(m_EndcapA_HV_COOLFolderName);      
      // Set up Monitoring Histograms if not done (first event)
      if (!m_h_EndcapA_HVvalAvg) {
	m_h_EndcapA_nRED = new TH1D("EndcapA_nRED","EA - nRED",clc->name_size(),1,clc->name_size());
	m_h_EndcapA_nNOINFO = new TH1D("EndcapA_nNOINFO","EA - nNOINFO",clc->name_size(),1,clc->name_size());
	m_h_EndcapA_HVvalAvg = new TH1D("EndcapA_HVvalAvg","EA - HVvalAvg",clc->name_size(),1,clc->name_size());
	for ( chanNameMapItr = clc->name_begin();
	      chanNameMapItr != clc->name_end(); ++chanNameMapItr ) {
	  std::string chanName( (*chanNameMapItr).second );
	  int chanNum( (*chanNameMapItr).first );
	  m_h_EndcapA_nRED->GetXaxis()->SetBinLabel(chanNum,chanName.c_str());
	  m_h_EndcapA_nNOINFO->GetXaxis()->SetBinLabel(chanNum,chanName.c_str());
	  m_h_EndcapA_HVvalAvg->GetXaxis()->SetBinLabel(chanNum,chanName.c_str());
	}
      }

      for ( chanNameMapItr = clc->name_begin();
	    chanNameMapItr != clc->name_end(); ++chanNameMapItr ) {
	std::string chanName( (*chanNameMapItr).second );
	int chanNum( (*chanNameMapItr).first );
	InDet::TRT_DCS_StatusFlag theFlag;
	InDet::TRT_DCS_ValueType theVoltage = -10.;
	sc = getValue( m_EndcapA_HV_COOLFolderName, chanNum, theVoltage );
	if ( sc.isFailure() ) theFlag = InDet::TRT_DCS_NOINFO;
	else if ( theVoltage < m_HVWarnValLo || theVoltage > m_HVWarnValHi ) theFlag = InDet::TRT_DCS_RED;
	else theFlag = InDet::TRT_DCS_GREEN;
	if ( theFlag == InDet::TRT_DCS_RED ) m_h_EndcapA_nRED->Fill(chanNum);
	if ( theFlag == InDet::TRT_DCS_NOINFO ) m_h_EndcapA_nNOINFO->Fill(chanNum);
	if (!sc.isFailure()) m_h_EndcapA_HVvalAvg->Fill(chanNum,theVoltage);
      }
    }
   return;
}
/// Monitor EndcapC HV
/////
void TRT_DCS_ConditionsSvc::monitorEndcapC(){

    StatusCode sc(StatusCode::SUCCESS);

    // Get the current event timestamp
    m_currentTimestamp = 0;
    SG::ReadHandle<xAOD::EventInfo> evtInfo(m_EventInfoKey);

    if (not evtInfo.isValid()) {
      ATH_MSG_WARNING( "Couldn't get " << m_EventInfoKey.key()
		       << " from StoreGate." );
      return;
    }

    m_currentTimestamp = evtInfo->timeStamp();
    if (m_VeryVerbose) ATH_MSG_VERBOSE( "Event timestamp: " << m_currentTimestamp );

    // Monitoring Histograms
    if ( m_doMonitoring ) {
      CondAttrListCollection::name_const_iterator chanNameMapItr;
      m_nECEvts++;

      const CondAttrListCollection* clc = getCollection(m_EndcapC_HV_COOLFolderName);      
      // Set up Monitoring Histograms if not done (first event)
      if (!m_h_EndcapC_HVvalAvg) {
	m_h_EndcapC_nRED = new TH1D("EndcapC_nRED","EC - nRED",clc->name_size(),1,clc->name_size());
	m_h_EndcapC_nNOINFO = new TH1D("EndcapC_nNOINFO","EC - nNOINFO",clc->name_size(),1,clc->name_size());
	m_h_EndcapC_HVvalAvg = new TH1D("EndcapC_HVvalAvg","EC - HVvalAvg",clc->name_size(),1,clc->name_size());
	for ( chanNameMapItr = clc->name_begin();
	      chanNameMapItr != clc->name_end(); ++chanNameMapItr ) {
	  std::string chanName( (*chanNameMapItr).second );
	  int chanNum( (*chanNameMapItr).first );
	  m_h_EndcapC_nRED->GetXaxis()->SetBinLabel(chanNum,chanName.c_str());
	  m_h_EndcapC_nNOINFO->GetXaxis()->SetBinLabel(chanNum,chanName.c_str());
	  m_h_EndcapC_HVvalAvg->GetXaxis()->SetBinLabel(chanNum,chanName.c_str());
	}
      }

      for ( chanNameMapItr = clc->name_begin();
	    chanNameMapItr != clc->name_end(); ++chanNameMapItr ) {
	std::string chanName( (*chanNameMapItr).second );
	int chanNum( (*chanNameMapItr).first );
	InDet::TRT_DCS_StatusFlag theFlag;
	InDet::TRT_DCS_ValueType theVoltage = -10.;
	sc = getValue( m_EndcapC_HV_COOLFolderName, chanNum, theVoltage );
	if ( sc.isFailure() ) theFlag = InDet::TRT_DCS_NOINFO;
	else if ( theVoltage < m_HVWarnValLo || theVoltage > m_HVWarnValHi ) theFlag = InDet::TRT_DCS_RED;
	else theFlag = InDet::TRT_DCS_GREEN;
	if ( theFlag == InDet::TRT_DCS_RED ) m_h_EndcapC_nRED->Fill(chanNum);
	if ( theFlag == InDet::TRT_DCS_NOINFO ) m_h_EndcapC_nNOINFO->Fill(chanNum);
	if (!sc.isFailure()) m_h_EndcapC_HVvalAvg->Fill(chanNum,theVoltage);
      }
    }
   return;
}
