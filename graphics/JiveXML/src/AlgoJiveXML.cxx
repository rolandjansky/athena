/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JiveXML/AlgoJiveXML.h"
#include "JiveXML/IDataRetriever.h"
#include "JiveXML/IGeometryWriter.h"
#include "JiveXML/XMLFormatTool.h"
#include "JiveXML/StreamToFileTool.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/PileUpEventInfo.h"
#include "xAODEventInfo/EventInfo.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"

#include <algorithm>

namespace JiveXML{

  AlgoJiveXML::AlgoJiveXML(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator) {

    //Configurable properties
    declareProperty("AtlasRelease"       ,m_AtlasRelease = "unknown", "The Athena release number");
    declareProperty("DataTypes"          ,m_dataTypes, "List of data retriever names to be run");
    declareProperty("WriteToFile"        ,m_writeToFile = true, "Whether XML files shall be produced");
    declareProperty("OnlineMode"         ,m_onlineMode  = false, "Whether an XMLRPC server shall be started");
    declareProperty("WriteGeometry"      ,m_writeGeometry = false, "Whether Geometry-XML files shall be produced");
    declareProperty("GeometryVersion"    ,m_geometryVersionIn = "default", "Geometry version as read from Athena");
    declareProperty("GeometryWriterTools",m_GeoWriterNames, "The names of the geometry-writer tools");

    /// Default list of geometries to write
    m_GeoWriterNames.push_back("JiveXML::GeometryWriter/GeometryWriter");
    m_GeoWriterNames.push_back("JiveXML::MuonGeometryWriter/MuonGeometryWriter");

    //Array of tools that retrieve the data, set public and createIf as default
    declareProperty("DataRetrievers", m_DataRetrievers = ToolHandleArray<IDataRetriever>( NULL ,true));

    //Generate XMLFormatTool as default privat format tool
    declareProperty("FormatTool", m_FormatTool = ToolHandle<IFormatTool>("JiveXML::XMLFormatTool/XMLFormatTool",this));

    //Array of tools for streaming events, set public and createIf as default
    declareProperty("StreamTools", m_StreamTools = ToolHandleArray<IStreamTool>(NULL,true));
  }

  /**
   * Initialize - called once in the beginning
   * - Get StoreGate service
   * - Get GeometryWriters and write geometry if asked for
   * - Get the formatting tool
   * - Get the data retrievers
   * - Get the streaming tools, including defaults if asked for
   */
  StatusCode AlgoJiveXML::initialize(){

    //be verbose
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) <<"Initialize()" << endmsg;

    /**
     * Get the geometry writer tools and write geometries
     */
    if(m_writeGeometry){

      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"Writing geometry to file" << endmsg;

      /// Loop over names
      std::vector<std::string>::iterator GeoWriterNameItr = m_GeoWriterNames.begin();
      for ( ; GeoWriterNameItr != m_GeoWriterNames.end(); ++GeoWriterNameItr){

        /// Get a tool handle and retrieve the tool

        ToolHandle<IGeometryWriter> GeometryWriter(*GeoWriterNameItr);
        if ( GeometryWriter.retrieve().isFailure() ){
          if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Unable to locate "
              << GeometryWriter.name() << " tool" << endmsg;
        } else {
          /// Write geometry
          if ( GeometryWriter->writeGeometry().isFailure() ){
            if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) <<"Unable to write geometry" << endmsg;
          }
          /// Release tool
          if ( GeometryWriter.release().isFailure() ){
            if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) <<"Unable to release tool GeometryWriter" << endmsg;
          }
        }
      }
    }

    /**
     * Get the formatting tools
     */
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"Retrieving XML format tool" << endmsg;

    //Retrieve the format tool
    if (m_FormatTool.retrieve().isFailure()){
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) <<"Unable to retrieve XML format tool" << endmsg;
      return StatusCode::FAILURE;
    }

    //Setting the tags
    if ( m_FormatTool->SetTag( TagType("Release",m_AtlasRelease)).isFailure() ){
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) <<"Couldn't set Release version" << endmsg;
    }

    /**
     * Add default file streaming tools if requested
     */
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"Retrieving streaming tools" << endmsg;
    if (m_writeToFile){
      if (msgLvl(MSG::INFO)) msg(MSG::INFO) <<"Adding default file streaming tool" << endmsg;
      m_StreamTools.push_back("JiveXML::StreamToFileTool/StreamToFileTool");
    }
    if (m_onlineMode){
      if (msgLvl(MSG::INFO)) msg(MSG::INFO) <<"Adding default XMLRPC streaming tool" << endmsg;
      m_StreamTools.push_back("JiveXML::XMLRPCStreamTool/XMLRPCStreamTool");
    }

    /// Get the streaming tools
    if (m_StreamTools.size() == 0) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) <<"No streaming tools defined, events will be created but not stored!" << endmsg;
    } else {
      if (m_StreamTools.retrieve().isFailure()){
        if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) <<"Unable to retrieve streaming tools !" << endmsg;
        return StatusCode::FAILURE;
      }
    }

    /**
     * Get the IDataRetrievers requested in the m_dataTypes list from the toolSvc 
     * and store them in the ToolHandleArray
     */ 
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"Obtaining list of data retrievers" << endmsg;

    /// Iteratate over the given data types
    std::vector<std::string>::iterator DataTypesEnd = m_dataTypes.end();
    std::vector<std::string>::iterator DataTypeItr = m_dataTypes.begin();

    /// Loop over the retriever names
    for( ; DataTypeItr != DataTypesEnd; ++DataTypeItr)
      {
        /// Create a tool handle for this tool
        ToolHandle<IDataRetriever> DataRetrieverTool(*DataTypeItr);
        /// See if we can retrieve the tool
        if( DataRetrieverTool.retrieve().isFailure() ) {
          if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) <<"Unable to locate tool "
                    << DataRetrieverTool.type() << " with name "
                    << DataRetrieverTool.name() << endmsg;
        } else {
          /// If so, store it in our list
          m_DataRetrievers.push_back(DataRetrieverTool);
        }
      }

    if (msgLvl(MSG::INFO)) msg(MSG::INFO) <<"Retrieving data from " << m_DataRetrievers.size() << " tools" << endmsg;

    return StatusCode::SUCCESS;
  }

  /**
   * Execute - called for every event
   * - retrive general event information
   * - call data retrievers
   * - pass formatted events to streamers
   */
  StatusCode AlgoJiveXML::execute() {

    /** 
     * Firstly retrieve all the event header information 
     */
    //The run and event number of the current event
    unsigned int runNo = 0, lumiBlock = 0;
    uint64_t eventNo=0;
    //Date and time string of the current event
    char dateTime[32];
    //general event property, unused yet
    std::string eventProperty = "default";

    // geometry-version/geometry-tag from Athena (sourced via jOs)
    std::string geometryVersion = "default";
    geometryVersion = DataType(m_geometryVersionIn).toString();

    //Retrieve eventInfo from StoreGate
    const EventInfo* eventInfo;
    if (evtStore()->retrieve(eventInfo).isFailure()){
      if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) <<"Could not find EventInfo" << endmsg;
      return StatusCode::FAILURE;
    }
    const xAOD::EventInfo* eventInfo2;
    if (evtStore()->retrieve(eventInfo2).isFailure()){
      if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) <<"Could not find xAODEventInfo" << endmsg;
      return StatusCode::FAILURE;
    }else{
    //Get run and event numbers - Test only ! Still using old EventInfo for xml output
    //    Event/xAOD/xAODEventInfo/trunk/xAODEventInfo/versions/EventInfo_v1.h
     if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " xAODEventInfo: " 
          << ", runNumber: "  << eventInfo2->runNumber()  // is '222222' for mc events ?
          << ", eventNumber: " << eventInfo2->eventNumber() 
          << ", mcChannelNumber: "  << eventInfo2->mcChannelNumber()
          << ", mcEventNumber: "  << eventInfo2->mcEventNumber() // MC: use this instead of runNumber
          << ", lumiBlock: "  << eventInfo2->lumiBlock()
          << ", timeStamp: "  << eventInfo2->timeStamp()
          << ", bcid: "  << eventInfo2->bcid()
          << ", eventTypeBitmask: "  << eventInfo2->eventTypeBitmask()
          << ", actualInteractionsPerCrossing: "  << eventInfo2->actualInteractionsPerCrossing()
          << ", averageInteractionsPerCrossing: "  << eventInfo2->averageInteractionsPerCrossing()
          << endmsg; 
    }

    // new treatment of mc_channel_number for mc12
    // from: https://twiki.cern.ch/twiki/bin/viewauth/Atlas/PileupDigitization#Contents_of_Pileup_RDO
    //Retrieve new PileupEventInfo from StoreGate
    unsigned int mcChannelNo = 0;
    bool firstEv = true;
    const PileUpEventInfo* pevt = 0;
    if (evtStore()->retrieve(pevt).isFailure()){
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) <<"Could not find PileUpEventInfo" << endmsg;
    }
    else {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Pileup Info Retrieved Successfully as 'PileUpEventInfo' Object " << endmsg;
      
      //+++ Get sub-event info object
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Sub Event Infos: " << endmsg;
      PileUpEventInfo::SubEvent::const_iterator it  = pevt->beginSubEvt();
      PileUpEventInfo::SubEvent::const_iterator end = pevt->endSubEvt();
      if (it == end) if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "None found" << endmsg;
      for (; it != end; ++it) {
	const EventInfo* sevt = (*it).pSubEvt;
	if (sevt) {
	  if (firstEv){ 
	    mcChannelNo =  sevt->event_type()->mc_channel_number(); // the 'real' mc-channel 
	    if (msgLvl(MSG::INFO)) msg(MSG::INFO)
	      << " mc_channel from PileUpEventInfo   : " << sevt->event_type()->mc_channel_number() << endmsg;
            firstEv = false;
          }       
	  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Sub Event Info:" << endmsg;
          if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)
	       << "  Time         : " << (*it).time()                             << endmsg
	       << "  Index        : " << (*it).index()                            << endmsg
	       << "  Provenance   : " << (*it).type()                        << endmsg // This is the provenance stuff: signal, minbias, cavern, etc
	       << "  Run Number   : " << sevt->event_ID()->run_number()           << endmsg
	       << "  Event Number : " << sevt->event_ID()->event_number()         << endmsg
	       << "  ns Offset    : " << sevt->event_ID()->time_stamp_ns_offset() << endmsg
	       << "  Lumi Block   : " << sevt->event_ID()->lumi_block()           << endmsg
	       << "  mc_channel   : " << sevt->event_type()->mc_channel_number() << endmsg
               << "  BCID         : " << sevt->event_ID()->bunch_crossing_id()    << endmsg
               << "  Geo version  : " << m_geometryVersionIn                      << endmsg
	       << "  User Type    : " << sevt->event_type()->user_type()          << endmsg;
	}
	else if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Subevent is null ptr " << endmsg;
      }
    }

    //Get run and event numbers
    runNo   = eventInfo->event_ID()->run_number();
    eventNo = eventInfo->event_ID()->event_number(); 

// Note: 4294967293 is the maximum value for a unsigned long

    if ( mcChannelNo != 0 ){ runNo = mcChannelNo + 140000000; } // indicating 'mc14'
    if (msgLvl(MSG::INFO)) msg(MSG::INFO) << " runNumber for filename: " << runNo 
        << ", eventNumber: " << eventNo << endmsg;

    if ( eventInfo->event_ID()->lumi_block() ){ 
      lumiBlock = eventInfo->event_ID()->lumi_block(); 
    }else{ 
      lumiBlock = -1; // placeholder 
    } 
    if ( mcChannelNo != 0 ) lumiBlock = -1; // mask for mc11a    

    // lumiBlock from mc can be just huge number, ignore then
    if ( lumiBlock > 1000000 ) { lumiBlock = 0; }

    //Get timestamp of the event
    //If Grid job not running in CEST or CET, change to UTC
    //Only option to avoid odd timezones. jpt 29Mar11
    size_t found1;
    size_t found2;
    if (eventInfo->event_ID()->time_stamp() > 0) {
      time_t unixtime = (time_t) eventInfo->event_ID()->time_stamp(); 
      struct tm *time = localtime(&unixtime);
      strftime(dateTime, 32, "%Y-%m-%d %H:%M:%S %Z", time);
      struct tm *utctime = gmtime(&unixtime);
      found1 = (DataType(dateTime).toString().find("CEST"));
      found2 = (DataType(dateTime).toString().find("CET")); 
      if ( int(found1)<0 && int(found2)<0 ){ // not found is -1
         strftime(dateTime, 32, "%Y-%m-%d %H:%M:%S UTC", utctime);
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " TIME NOT CET/CEST. Adjusted to:" << dateTime << endmsg;
      }
    } else {
      dateTime[0] = '\0'; // empty string
    }
    if ( mcChannelNo != 0 ){ dateTime[0] = '\0'; } // mask for mc11a

    /**
     * Then start a new event with this header information
     */
    if ( m_FormatTool->StartEvent(eventNo, runNo, dateTime, lumiBlock, eventProperty, geometryVersion).isFailure() ){
      if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) <<"Couldn't start event in FormatTool" << endmsg;
      return StatusCode::FAILURE;
    }

    /**
     * Now iterate over all the IDataRetrievers and 
     * write their data to the xml file by giving it the XMLWriter 
     */
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"Starting loop over data retrievers" << endmsg;
    //Loop over data retrievers
    ToolHandleArray<IDataRetriever>::iterator DataRetrieverItr = m_DataRetrievers.begin();
    for(; DataRetrieverItr != m_DataRetrievers.end(); ++DataRetrieverItr)  {
      //Add try-catch to avoid retrieval to fail on single retriever
      try {
        //Retrieve information and pass it to formatting tool object
        if ((*DataRetrieverItr)->retrieve(m_FormatTool).isFailure()) {
          if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) <<"Failed to fill " << (*DataRetrieverItr)->dataTypeName() << endmsg;
        } else {
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"Filled: " << (*DataRetrieverItr)->dataTypeName() << endmsg;
        }
      //Only catch std::exception
      } catch ( std::exception& ex ){
        //Now show some message
        if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) <<"Caught exception in " << (*DataRetrieverItr)->name() 
                          << " while retrieving data for " << (*DataRetrieverItr)->dataTypeName() 
                          << " : " << ex.what() << endmsg;
        //and return with an error
        return StatusCode::FAILURE;
      }
    }
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"Finished loop over data retrievers" << endmsg;

    /**
     * Finish the event with a proper footer
     */
    if ( m_FormatTool->EndEvent().isFailure() ){
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) <<"Couldn't end event in FormatTool" << endmsg;
      return StatusCode::FAILURE;
    }

    /**
     * Now stream the events to all registered streaming tools
     */
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"Starting loop over event streamers" << endmsg;
    //Loop over streaming tools
    ToolHandleArray<IStreamTool>::iterator StreamToolsItr = m_StreamTools.begin();
    for ( ; StreamToolsItr != m_StreamTools.end(); ++StreamToolsItr ){
      if (msgLvl(MSG::INFO)) msg(MSG::INFO) <<"Streaming event to " << (*StreamToolsItr)->name() << endmsg;
        if ( (*StreamToolsItr)->StreamEvent(eventNo, runNo, m_FormatTool->getFormattedEvent()).isFailure() ){
           if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not stream event to " << (*StreamToolsItr)->name() << endmsg;
        } 
    }
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"Finished loop over event streamers" << endmsg;

    return StatusCode::SUCCESS;
  }

  /**
   * Finalize called once in the end
   * - release all tools
   */
  StatusCode AlgoJiveXML::finalize() {

    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) <<"finalize()" << endmsg;  

    /// Release all the tools
    m_DataRetrievers.release().ignore();
    m_FormatTool.release().ignore();
    m_StreamTools.release().ignore();

    return StatusCode::SUCCESS;
  }
} //namespace
