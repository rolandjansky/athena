/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
    ATH_MSG_VERBOSE("Initialize()");

    /**
     * Get the geometry writer tools and write geometries
     */
    if(m_writeGeometry){

      ATH_MSG_DEBUG("Writing geometry to file");

      /// Loop over names
      std::vector<std::string>::iterator GeoWriterNameItr = m_GeoWriterNames.begin();
      for ( ; GeoWriterNameItr != m_GeoWriterNames.end(); ++GeoWriterNameItr){

        /// Get a tool handle and retrieve the tool

        ToolHandle<IGeometryWriter> GeometryWriter(*GeoWriterNameItr);
        if ( GeometryWriter.retrieve().isFailure() ){
          ATH_MSG_WARNING("Unable to locate "<<GeometryWriter.name()<<" tool");
        } else {
          /// Write geometry
          if ( GeometryWriter->writeGeometry().isFailure() ){
            ATH_MSG_WARNING("Unable to write geometry");
          }
          /// Release tool
          if ( GeometryWriter.release().isFailure() ){
            ATH_MSG_WARNING("Unable to release tool GeometryWriter");
          }
        }
      }
    }

    /**
     * Get the formatting tools
     */
    ATH_MSG_DEBUG("Retrieving XML format tool");

    //Retrieve the format tool
    ATH_CHECK(m_FormatTool.retrieve());

    //Setting the tags
    if ( m_FormatTool->SetTag( TagType("Release",m_AtlasRelease)).isFailure() ){
      ATH_MSG_WARNING( "Couldn't set Release version" );
    }

    /**
     * Add default file streaming tools if requested
     */
    ATH_MSG_DEBUG("Retrieving streaming tools");
    if (m_writeToFile){
      ATH_MSG_INFO("Adding default file streaming tool");
      m_StreamTools.push_back("JiveXML::StreamToFileTool/StreamToFileTool");
    }
    if (m_onlineMode){
      ATH_MSG_INFO("Adding default XMLRPC streaming tool");
      m_StreamTools.push_back("JiveXML::XMLRPCStreamTool/XMLRPCStreamTool");
    }

    /// Get the streaming tools
    if (m_StreamTools.size() == 0) {
      ATH_MSG_WARNING("No streaming tools defined, events will be created but not stored!");
    } else {
      if (m_StreamTools.retrieve().isFailure()){
        ATH_MSG_ERROR("Unable to retrieve streaming tools !");
        return StatusCode::FAILURE;
      }
    }

    /**
     * Get the IDataRetrievers requested in the m_dataTypes list from the toolSvc 
     * and store them in the ToolHandleArray
     */ 
    ATH_MSG_DEBUG("Obtaining list of data retrievers");

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
          ATH_MSG_WARNING( "Unable to locate tool "
                    << DataRetrieverTool.type() << " with name "
                    << DataRetrieverTool.name());
        } else {
          /// If so, store it in our list
          m_DataRetrievers.push_back(DataRetrieverTool);
        }
      }

    ATH_MSG_INFO("Retrieving data from " << m_DataRetrievers.size() << " tools" );

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
    const xAOD::EventInfo* eventInfo = nullptr;
    if (evtStore()->retrieve(eventInfo).isFailure()){
      ATH_MSG_FATAL("Could not find xAODEventInfo" );
      return StatusCode::FAILURE;
    }else{
    // Event/xAOD/xAODEventInfo/trunk/xAODEventInfo/versions/EventInfo_v1.h
     ATH_MSG_VERBOSE(" xAODEventInfo: runNumber: "  << eventInfo->runNumber()  // is '222222' for mc events ?
          << ", eventNumber: " << eventInfo->eventNumber()
          << ", mcChannelNumber: " << eventInfo->mcChannelNumber()
          << ", mcEventNumber: "  << eventInfo->mcEventNumber() // MC: use this instead of runNumber
          << ", lumiBlock: "  << eventInfo->lumiBlock()
          << ", timeStamp: "  << eventInfo->timeStamp()
          << ", bcid: "  << eventInfo->bcid()
          << ", eventTypeBitmask: "  << eventInfo->eventTypeBitmask()
          << ", actualInteractionsPerCrossing: "  << eventInfo->actualInteractionsPerCrossing()
          << ", averageInteractionsPerCrossing: "  << eventInfo->averageInteractionsPerCrossing()
          ); 
    }

    // new treatment of mc_channel_number for mc12
    // from: https://twiki.cern.ch/twiki/bin/viewauth/Atlas/PileupDigitization#Contents_of_Pileup_RDO
    //Retrieve new PileupEventInfo from StoreGate
    unsigned int mcChannelNo = 0;
    bool firstEv = true;
    const PileUpEventInfo* pevt = 0;
    if (evtStore()->retrieve(pevt).isFailure()){
      ATH_MSG_WARNING( "Could not find PileUpEventInfo" );
    }
    else {
      ATH_MSG_DEBUG( "Pileup Info Retrieved Successfully as 'PileUpEventInfo' Object " );
      
      //+++ Get sub-event info object
      ATH_MSG_DEBUG( "Sub Event Infos: " );
      PileUpEventInfo::SubEvent::const_iterator it  = pevt->beginSubEvt();
      PileUpEventInfo::SubEvent::const_iterator end = pevt->endSubEvt();
      if (it == end) ATH_MSG_DEBUG( "None found" );
      for (; it != end; ++it) {
	const EventInfo* sevt = (*it).pSubEvt;
	if (sevt) {
	  if (firstEv){ 
	    mcChannelNo =  sevt->event_type()->mc_channel_number(); // the 'real' mc-channel 
	    ATH_MSG_INFO( " mc_channel from PileUpEventInfo   : " << sevt->event_type()->mc_channel_number() );
            firstEv = false;
          }       
	  ATH_MSG_VERBOSE("Sub Event Info:\n  Time         : " << (*it).time()                             
	       << "  Index        : " << (*it).index()                            
	       << "  Provenance   : " << (*it).type()                         // This is the provenance stuff: signal, minbias, cavern, etc
	       << "  Run Number   : " << sevt->event_ID()->run_number()           
	       << "  Event Number : " << sevt->event_ID()->event_number()         
	       << "  ns Offset    : " << sevt->event_ID()->time_stamp_ns_offset() 
	       << "  Lumi Block   : " << sevt->event_ID()->lumi_block()           
	       << "  mc_channel   : " << sevt->event_type()->mc_channel_number() 
               << "  BCID         : " << sevt->event_ID()->bunch_crossing_id()    
               << "  Geo version  : " << m_geometryVersionIn                      
	       << "  User Type    : " << sevt->event_type()->user_type()          );
	}
	else ATH_MSG_VERBOSE("Subevent is null ptr ");
      }
    }

    //Get run and event numbers
    runNo   = eventInfo->runNumber();
    eventNo = eventInfo->eventNumber();

// Note: 4294967293 is the maximum value for a unsigned long

    if ( mcChannelNo != 0 ){ runNo = mcChannelNo + 140000000; } // indicating 'mc14'
    ATH_MSG_INFO( " runNumber for filename: " << runNo << ", eventNumber: " << eventNo);

    if ( eventInfo->lumiBlock() ){
      lumiBlock = eventInfo->lumiBlock();
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
    if (eventInfo->timeStamp() > 0) {
      time_t unixtime = (time_t) eventInfo->timeStamp();
      struct tm *time = localtime(&unixtime);
      strftime(dateTime, 32, "%Y-%m-%d %H:%M:%S %Z", time);
      struct tm *utctime = gmtime(&unixtime);
      found1 = (DataType(dateTime).toString().find("CEST"));
      found2 = (DataType(dateTime).toString().find("CET")); 
      if ( int(found1)<0 && int(found2)<0 ){ // not found is -1
         strftime(dateTime, 32, "%Y-%m-%d %H:%M:%S UTC", utctime);
         ATH_MSG_DEBUG( " TIME NOT CET/CEST. Adjusted to:" << dateTime );
      }
    } else {
      dateTime[0] = '\0'; // empty string
    }
    if ( mcChannelNo != 0 ){ dateTime[0] = '\0'; } // mask for mc11a

    /**
     * Then start a new event with this header information
     */
    if ( m_FormatTool->StartEvent(eventNo, runNo, dateTime, lumiBlock, eventProperty, geometryVersion).isFailure() ){
      ATH_MSG_FATAL("Couldn't start event in FormatTool");
      return StatusCode::FAILURE;
    }

    /**
     * Now iterate over all the IDataRetrievers and 
     * write their data to the xml file by giving it the XMLWriter 
     */
    ATH_MSG_DEBUG("Starting loop over data retrievers" );
    //Loop over data retrievers
    ToolHandleArray<IDataRetriever>::iterator DataRetrieverItr = m_DataRetrievers.begin();
    for(; DataRetrieverItr != m_DataRetrievers.end(); ++DataRetrieverItr)  {
      //Add try-catch to avoid retrieval to fail on single retriever
      try {
        //Retrieve information and pass it to formatting tool object
        if ((*DataRetrieverItr)->retrieve(m_FormatTool).isFailure()) {
          ATH_MSG_WARNING( "Failed to fill " << (*DataRetrieverItr)->dataTypeName() );
        } else {
          ATH_MSG_DEBUG("Filled: " << (*DataRetrieverItr)->dataTypeName() );
        }
      //Only catch std::exception
      } catch ( std::exception& ex ){
        //Now show some message
        ATH_MSG_FATAL("Caught exception in " << (*DataRetrieverItr)->name() 
                          << " while retrieving data for " << (*DataRetrieverItr)->dataTypeName() 
                          << " : " << ex.what() );
        //and return with an error
        return StatusCode::FAILURE;
      }
    }
    ATH_MSG_DEBUG( "Finished loop over data retrievers" );

    /**
     * Finish the event with a proper footer
     */
    if ( m_FormatTool->EndEvent().isFailure() ){
      ATH_MSG_WARNING( "Couldn't end event in FormatTool" );
      return StatusCode::FAILURE;
    }

    /**
     * Now stream the events to all registered streaming tools
     */
    ATH_MSG_DEBUG( "Starting loop over event streamers" );
    //Loop over streaming tools
    ToolHandleArray<IStreamTool>::iterator StreamToolsItr = m_StreamTools.begin();
    for ( ; StreamToolsItr != m_StreamTools.end(); ++StreamToolsItr ){
      ATH_MSG_INFO("Streaming event to " << (*StreamToolsItr)->name() );
        if ( (*StreamToolsItr)->StreamEvent(eventNo, runNo, m_FormatTool->getFormattedEvent()).isFailure() ){
           ATH_MSG_WARNING( "Could not stream event to " << (*StreamToolsItr)->name() );
        } 
    }
    ATH_MSG_DEBUG( "Finished loop over event streamers" );

    return StatusCode::SUCCESS;
  }

  /**
   * Finalize called once in the end
   * - release all tools
   */
  StatusCode AlgoJiveXML::finalize() {

    ATH_MSG_VERBOSE( "finalize()" );  

    /// Release all the tools
    m_DataRetrievers.release().ignore();
    m_FormatTool.release().ignore();
    m_StreamTools.release().ignore();

    return StatusCode::SUCCESS;
  }
} //namespace
