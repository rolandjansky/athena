/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file TRT_DCS_ConditionsSvc.cxx
 *  @brief Service to provide a simple Athena interface to read
 *  DCS conditions data from COOL
 * author Denver Whittington <Denver.Whittington@cern.ch>
 **/
#include "TRT_DCS_ConditionsSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "InDetIdentifier/TRT_ID.h"

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
  m_Barrel_HV_COOLCont(0),
  m_EndcapA_HV_COOLCont(0),
  m_EndcapC_HV_COOLCont(0),
  m_TRT_ID_Helper(0),
  m_numFlagRED(0),
  m_numFlagNOINFO(0),
  m_currentTimestamp(0),
  m_doMonitoring(false),
  m_h_Barrel_nRED(0),
  m_h_EndcapA_nRED(0),
  m_h_EndcapC_nRED(0),
  m_h_Barrel_nNOINFO(0),
  m_h_EndcapA_nNOINFO(0),
  m_h_EndcapC_nNOINFO(0),
  m_h_Barrel_HVvalAvg(0),
  m_h_EndcapA_HVvalAvg(0),
  m_h_EndcapC_HVvalAvg(0),
  m_nEvts(0)
{
  // Get properties from job options
  declareProperty( "VeryVerbose", m_VeryVerbose = false );
  declareProperty( "Barrel_HV_COOLFolderName", m_Barrel_HV_COOLFolderName = "/TRT/DCS/HV/BARREL" );
  declareProperty( "EndcapA_HV_COOLFolderName", m_EndcapA_HV_COOLFolderName = "/TRT/DCS/HV/ENDCAPA" );
  declareProperty( "EndcapC_HV_COOLFolderName", m_EndcapC_HV_COOLFolderName = "/TRT/DCS/HV/ENDCAPC" );
  declareProperty( "HV_WarningValueLow",  m_HVWarnValHi = 2000. );
  declareProperty( "HV_WarningValueHigh", m_HVWarnValLo = 1000. );
  declareProperty( "HWMapSvc", m_mapSvc );
  declareProperty( "EventInfoKey", m_EventInfoKey = "EventInfo" );
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

  // Register a callback for "BeginEvent"
  IIncidentSvc* incSvc;
  sc = service( "IncidentSvc", incSvc );
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR( "Couldn't get the IncidentSvc." );
    return sc;
  }
  incSvc->addListener( this, std::string("BeginEvent") );

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
    if ( folderName == "" ) {
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
	if ( chanName == "" ) {
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
StatusCode TRT_DCS_ConditionsSvc::getValue( const std::string foldername,
					    const std::string chanName,
					    InDet::TRT_DCS_ValueType& theValue ) {
  StatusCode sc(StatusCode::SUCCESS);

  /*
  //  Specify foldername and chanName in COOL form
  //  e.g. foldername = "/TRT/DCS/HV/BARREL"
  //       chanName   = "HVB_S24_M3_B3_OutputVoltage"
   */

  // Get the folder from the DetectorStore.
  const CondAttrListCollection* DCScondFolder = 0;
  if ( foldername == m_Barrel_HV_COOLFolderName  ) DCScondFolder = m_Barrel_HV_COOLCont;
  if ( foldername == m_EndcapA_HV_COOLFolderName ) DCScondFolder = m_EndcapA_HV_COOLCont;
  if ( foldername == m_EndcapC_HV_COOLFolderName ) DCScondFolder = m_EndcapC_HV_COOLCont;
  if ( !DCScondFolder ) {
    sc = m_detStore->retrieve( DCScondFolder, foldername );
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING( "Couldn't retrieve folder " << foldername
		       << " from DetectorStore.  Has it been loaded into IOVDbSvc?" );
      return StatusCode::FAILURE;
    }
  }

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
  CondAttrListCollection::AttributeList attrList = (*chanAttrListPair).second;

  // Get the value
  theValue = attrList["OUTPUTVOLTAGE_VALUE"].data<InDet::TRT_DCS_ValueType>();

  return sc;
}

//////////
/// getValue by folder name and COOL channel number
/////
StatusCode TRT_DCS_ConditionsSvc::getValue( const std::string foldername,
					    const int chanNum,
					    InDet::TRT_DCS_ValueType& theValue ) {

  /*
  //  Directly access the value by folder channel number
  //  e.g. foldername = "/TRT/DCS/HV/BARREL"
  //       chanNum    = e.g. result from HV map query
   */

  // Get the folder from the DetectorStore.
  const CondAttrListCollection* DCScondFolder = 0;
  if ( foldername == m_Barrel_HV_COOLFolderName  ) DCScondFolder = m_Barrel_HV_COOLCont;
  if ( foldername == m_EndcapA_HV_COOLFolderName ) DCScondFolder = m_EndcapA_HV_COOLCont;
  if ( foldername == m_EndcapC_HV_COOLFolderName ) DCScondFolder = m_EndcapC_HV_COOLCont;
  if ( !DCScondFolder ) {
    StatusCode sc = m_detStore->retrieve( DCScondFolder, foldername );
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING( "Couldn't retrieve folder " << foldername
		       << " from DetectorStore.  Has it been loaded into IOVDbSvc?" );
      return sc;
    }
  }

  // Get the requested channel/value pair
  CondAttrListCollection::const_iterator chanAttrListPair;
  chanAttrListPair = DCScondFolder->chanAttrListPair( chanNum );
  if ( chanAttrListPair == DCScondFolder->end() ) {
    ATH_MSG_VERBOSE( "Channel " << chanNum
		     << " not found in folder " << foldername
		     << " for this IOV." );
    return StatusCode::FAILURE;
  }
  CondAttrListCollection::AttributeList attrList = (*chanAttrListPair).second;

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
    m_h_Barrel_HVvalAvg->Scale(1./double(m_nEvts));
    m_h_EndcapA_HVvalAvg->Scale(1./double(m_nEvts));
    m_h_EndcapC_HVvalAvg->Scale(1./double(m_nEvts));
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
/// IncidentSvc incident handler
/////
void TRT_DCS_ConditionsSvc::handle( const Incident& inc ) {
  StatusCode sc(StatusCode::SUCCESS);

  // BeginEvent handler
  if ( inc.type() == "BeginEvent" ) {
    // Get the COOL DCS conditions attribute list pointers from the detector store
    m_Barrel_HV_COOLCont = 0;
    m_EndcapA_HV_COOLCont = 0;
    m_EndcapC_HV_COOLCont = 0;
    sc = m_detStore->retrieve( m_Barrel_HV_COOLCont, m_Barrel_HV_COOLFolderName );
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING( "Couldn't retrieve folder " << m_Barrel_HV_COOLFolderName
		       << " from DetectorStore.  Has it been loaded into IOVDbSvc?" );
      return;
    }
    sc = m_detStore->retrieve( m_EndcapA_HV_COOLCont, m_EndcapA_HV_COOLFolderName );
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING( "Couldn't retrieve folder " << m_EndcapA_HV_COOLFolderName
		       << " from DetectorStore.  Has it been loaded into IOVDbSvc?" );
      return;
    }
    sc = m_detStore->retrieve( m_EndcapC_HV_COOLCont, m_EndcapC_HV_COOLFolderName );
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING( "Couldn't retrieve folder " << m_EndcapC_HV_COOLFolderName
		       << " from DetectorStore.  Has it been loaded into IOVDbSvc?" );
      return;
    }

    // Get the current event timestamp
    m_currentTimestamp = 0;
    const EventInfo* evtInfo;
    sc = m_evtStore->retrieve(evtInfo,m_EventInfoKey);
    if ( sc.isFailure() || !evtInfo ) {
      ATH_MSG_WARNING( "Couldn't get " << m_EventInfoKey
		       << " from StoreGate." );
      return;
    }
    if ( !evtInfo->event_ID() ) {
      ATH_MSG_WARNING( m_EventInfoKey << " object has no EventID object." );
      return;
    }
    m_currentTimestamp = evtInfo->event_ID()->time_stamp();
    if (m_VeryVerbose) ATH_MSG_VERBOSE( "Event timestamp: " << m_currentTimestamp );

    // Monitoring Histograms
    if ( m_doMonitoring ) {
      CondAttrListCollection::name_const_iterator chanNameMapItr;
      m_nEvts++;
      
      // Set up Monitoring Histograms if not done (first event)
      if (!m_h_Barrel_HVvalAvg) {
	m_h_Barrel_nRED = new TH1D("Barrel_nRED","Barrel - nRED",m_Barrel_HV_COOLCont->name_size(),1,m_Barrel_HV_COOLCont->name_size());
	m_h_Barrel_nNOINFO = new TH1D("Barrel_nNOINFO","Barrel - nNOINFO",m_Barrel_HV_COOLCont->name_size(),1,m_Barrel_HV_COOLCont->name_size());
	m_h_Barrel_HVvalAvg = new TH1D("Barrel_HVvalAvg","Barrel - HVvalAvg",m_Barrel_HV_COOLCont->name_size(),1,m_Barrel_HV_COOLCont->name_size());
	for ( chanNameMapItr = m_Barrel_HV_COOLCont->name_begin();
	      chanNameMapItr != m_Barrel_HV_COOLCont->name_end(); ++chanNameMapItr ) {
	  std::string chanName( (*chanNameMapItr).second );
	  int chanNum( (*chanNameMapItr).first );
	  m_h_Barrel_nRED->GetXaxis()->SetBinLabel(chanNum,chanName.c_str());
	  m_h_Barrel_nNOINFO->GetXaxis()->SetBinLabel(chanNum,chanName.c_str());
	  m_h_Barrel_HVvalAvg->GetXaxis()->SetBinLabel(chanNum,chanName.c_str());
	}
      }

      if (!m_h_EndcapA_HVvalAvg) {
	m_h_EndcapA_nRED = new TH1D("EndcapA_nRED","EndcapA - nRED",m_EndcapA_HV_COOLCont->name_size(),1,m_EndcapA_HV_COOLCont->name_size());
	m_h_EndcapA_nNOINFO = new TH1D("EndcapA_nNOINFO","EndcapA - nNOINFO",m_EndcapA_HV_COOLCont->name_size(),1,m_EndcapA_HV_COOLCont->name_size());
	m_h_EndcapA_HVvalAvg = new TH1D("EndcapA_HVvalAvg","EndcapA - HVvalAvg",m_EndcapA_HV_COOLCont->name_size(),1,m_EndcapA_HV_COOLCont->name_size());
	for ( chanNameMapItr = m_EndcapA_HV_COOLCont->name_begin();
	      chanNameMapItr != m_EndcapA_HV_COOLCont->name_end(); ++chanNameMapItr ) {
	  std::string chanName( (*chanNameMapItr).second );
	  int chanNum( (*chanNameMapItr).first );
	  m_h_EndcapA_nRED->GetXaxis()->SetBinLabel(chanNum,chanName.c_str());
	  m_h_EndcapA_nNOINFO->GetXaxis()->SetBinLabel(chanNum,chanName.c_str());
	  m_h_EndcapA_HVvalAvg->GetXaxis()->SetBinLabel(chanNum,chanName.c_str());
	}
      }

      if (!m_h_EndcapC_HVvalAvg) {
	m_h_EndcapC_nRED = new TH1D("EndcapC_nRED","EndcapC - nRED",m_EndcapC_HV_COOLCont->name_size(),1,m_EndcapC_HV_COOLCont->name_size());
	m_h_EndcapC_nNOINFO = new TH1D("EndcapC_nNOINFO","EndcapC - nNOINFO",m_EndcapC_HV_COOLCont->name_size(),1,m_EndcapC_HV_COOLCont->name_size());
	m_h_EndcapC_HVvalAvg = new TH1D("EndcapC_HVvalAvg","EndcapC - HVvalAvg",m_EndcapC_HV_COOLCont->name_size(),1,m_EndcapC_HV_COOLCont->name_size());
	for ( chanNameMapItr = m_EndcapC_HV_COOLCont->name_begin();
	      chanNameMapItr != m_EndcapC_HV_COOLCont->name_end(); ++chanNameMapItr ) {
	  std::string chanName( (*chanNameMapItr).second );
	  int chanNum( (*chanNameMapItr).first );
	  m_h_EndcapC_nRED->GetXaxis()->SetBinLabel(chanNum,chanName.c_str());
	  m_h_EndcapC_nNOINFO->GetXaxis()->SetBinLabel(chanNum,chanName.c_str());
	  m_h_EndcapC_HVvalAvg->GetXaxis()->SetBinLabel(chanNum,chanName.c_str());
	}
      }

      // Fill Monitoring Histograms

      // Barrel
      for ( chanNameMapItr = m_Barrel_HV_COOLCont->name_begin();
	    chanNameMapItr != m_Barrel_HV_COOLCont->name_end(); ++chanNameMapItr ) {
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
      // EndcapA
      for ( chanNameMapItr = m_EndcapA_HV_COOLCont->name_begin();
	    chanNameMapItr != m_EndcapA_HV_COOLCont->name_end(); ++chanNameMapItr ) {
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
      // EndcapC
      for ( chanNameMapItr = m_EndcapC_HV_COOLCont->name_begin();
	    chanNameMapItr != m_EndcapC_HV_COOLCont->name_end(); ++chanNameMapItr ) {
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

    } // end of doMonitoring

  }

  return;
}
