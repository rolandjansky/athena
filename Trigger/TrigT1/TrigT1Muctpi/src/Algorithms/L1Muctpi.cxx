/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L1Muctpi.cxx 794528 2017-01-30 12:36:33Z fwinkl $

// STL include(s):
#include <iostream>
#include <sstream>

// Gaudi/Athena include(s):
#include "PathResolver/PathResolver.h"
#include "AthenaKernel/errorcheck.h"

// The AOD object that we may want to read as input
#include "AnalysisTriggerEvent/LVL1_ROI.h"

// The new trigger configuration
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigConfL1Data/Muctpi.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/TriggerThreshold.h"

// Interfaces used by the simulation
#include "TrigT1Interfaces/Lvl1MuCTPIInput.h"
#include "TrigT1Interfaces/MuCTPICTP.h"
#include "TrigT1Interfaces/MuCTPIToRoIBSLink.h"
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"
#include "TrigT1Interfaces/NimCTP.h"

// The output object of the simulation
#include "TrigT1Result/MuCTPI_RDO.h"

// Inlcudes for the MuCTPI simulation
#include "L1Muctpi.h"
#include "../Common/MuctpiSim.h"
#include "../Common/Configuration.h"
#include "../Logging/MsgWriter.h"
#include "../Mioct/StrategyName.h"
#include "../Common/Converter.h"

namespace LVL1MUCTPI {

  // Set the default StoreGate locations of input and output objects:
  const std::string L1Muctpi::m_DEFAULT_locationMuCTPItoCTP      = "/Run/L1MuCTPItoCTPLocation";
  const std::string L1Muctpi::m_DEFAULT_locationMuCTPItoL1Topo   = "/Run/L1MuCTPItoL1TopoLocation";
  const std::string L1Muctpi::m_DEFAULT_locationMuCTPItoRoIB     = "/Run/L1MuCTPItoRoIBLocation";
  const std::string L1Muctpi::m_DEFAULT_L1MuctpiStoreLocationRPC = "/Event/L1MuctpiStoreRPC";
  const std::string L1Muctpi::m_DEFAULT_L1MuctpiStoreLocationTGC = "/Event/L1MuctpiStoreTGC";
  const std::string L1Muctpi::m_DEFAULT_AODLocID                 = "LVL1_ROI";
  const std::string L1Muctpi::m_DEFAULT_RDOLocID                 = "MUCTPI_RDO";

  //--------------
  // Constructor
  //--------------
  L1Muctpi::L1Muctpi( const std::string& name, ISvcLocator* pSvcLocator )
    : AthAlgorithm( name, pSvcLocator ),
      m_configSvc( "TrigConf::TrigConfigSvc/TrigConfigSvc", name ),
      m_theMuctpi( 0 ), m_executeFunction(nullptr)  {

    // Declare the service handles as properties:
    declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service" );

    // Declare the properties of the overlap treatment:
    declareProperty( "OverlapStrategyName", m_overlapStrategyName = "NULL" );
    declareProperty( "LUTXMLFile", m_lutXMLFile = "" );
    declareProperty( "RunPeriod", m_runPeriod = "RUN1" );
    declareProperty( "FlaggingMode", m_flagMode = false );

    // Declare the properties of the output generation for L1Topo:
    declareProperty( "GeometryXMLFile", m_geometryXMLFile = "" );
       
    // Declare the properties for the multiplicity summation:
    declareProperty( "MultiplicityStrategyName", m_multiplicityStrategyName = "INCLUSIVE" );
    declareProperty( "MultiplicityXMLFile", m_multiplicityXMLFile = "" );

    // Declare the properties of the input selection:
    declareProperty( "InputSource", m_inputSource = "DIGITIZATION" );
    declareProperty( "AODLocID", m_aodLocId = m_DEFAULT_AODLocID );
    declareProperty( "RDOLocID", m_rdoLocId = m_DEFAULT_RDOLocID );
    declareProperty( "RDOOutputLocID", m_rdoOutputLocId = m_DEFAULT_RDOLocID );
    declareProperty( "RoIOutputLocID", m_roiOutputLocId = m_DEFAULT_locationMuCTPItoRoIB );
    declareProperty( "CTPOutputLocID", m_ctpOutputLocId = m_DEFAULT_locationMuCTPItoCTP );
    declareProperty( "L1TopoOutputLocID", m_l1topoOutputLocId = m_DEFAULT_locationMuCTPItoL1Topo );
    // These are just here for flexibility, normally they should not be changed:
    declareProperty( "TGCLocID", m_tgcLocId = m_DEFAULT_L1MuctpiStoreLocationTGC );
    declareProperty( "RPCLocID", m_rpcLocId = m_DEFAULT_L1MuctpiStoreLocationRPC );
    // The LUTs can be huge, so the default is to turn their printing off:
    declareProperty( "DumpLUT", m_dumpLut = false );
   
    // Allow rerunning of simulation on data
    declareProperty( "IsData", m_IsData = false, "Rerun simulation on data" );  
 
    // Declare the properties of the NIM output creation:
    declareProperty( "DoNIMOutput", m_doNimOutput = false );
    declareProperty( "NIMOutputLocID", m_nimOutputLocId = LVL1::DEFAULT_NimCTPLocation );
    declareProperty( "NIMBarrelBit", m_nimBarrelBit = 0,
		     "Bit on the NIM input of the CTP, showing that there was at least "
		     "one barrel candidate in the event" );
    declareProperty( "NIMEndcapBit", m_nimEndcapBit = 1,
		     "Bit on the NIM input of the CTP, showing that there was at least "
		     "one endcap candidate in the event" );

  }

  //--------------
  // Destructor
  //--------------
  L1Muctpi::~L1Muctpi() {

    ATH_MSG_DEBUG( "L1Muctpi destructor called" );
    if( m_theMuctpi ) delete m_theMuctpi;
  }

  /**
   * Initialisation of the algorithm. This is where the MuCTPI simulation object gets created,
   * the look up table is loaded for overlap handling between the muon sectors and the MuCTPI
   * configuration is read from LVL1ConfigSvc. Also m_executeFunction gets set up to point to
   * the execute function we have selected in the jobOptions.
   */
  StatusCode L1Muctpi::initialize() {

    // Init message
    ATH_MSG_INFO( "=======================================" );
    ATH_MSG_INFO( "Initialisation for L1Muctpi algorithm."  );
    ATH_MSG_INFO( "Package version: " << PACKAGE_VERSION    );
    ATH_MSG_INFO( "=======================================" );

    // Now this is a tricky part. We have to force the message logging of the
    // MuCTPI simulation to display messages of the same level as this MsgStream.
    MsgWriter::instance()->setMinType( msg().level() );
    MsgWriter::instance()->setSource( name() );

    // Create the MuCTPI simulation:
    m_theMuctpi = new MuctpiSim();

    //
    // Set up the overlap handling of the simulation:
    //
    if( m_overlapStrategyName == "NULL" ) {

      ATH_MSG_DEBUG( "Setting overlap strategy: \"NULL\"" );
      m_theMuctpi->setOverlapStrategy( NO_OVERLAP );

    } else if( m_overlapStrategyName == "LUT" ) {

      ATH_MSG_DEBUG( "Setting overlap strategy: \"LUT\"" );
      m_theMuctpi->setOverlapStrategy( LUT_OVERLAP );

      if( m_flagMode ) {
	ATH_MSG_INFO( "Using 'flagging mode' in the overlap handling" );
      }

      ATH_MSG_INFO( "XML LUT file defined in jobO: " << m_lutXMLFile << " with a RunPeriod=" << m_runPeriod );
      const std::string fullFileName = PathResolverFindCalibFile( m_lutXMLFile );
      ATH_MSG_DEBUG( "Full path to XML LUT file: " << fullFileName );
      CHECK( m_theMuctpi->initializeLUTOverlapStrategy( fullFileName, m_flagMode,
							m_dumpLut, m_runPeriod ) );

    } else {

      REPORT_ERROR( StatusCode::FAILURE )
	<< "Wrong overlap strategy name specified: \""
	<< m_overlapStrategyName << "\"";
      return StatusCode::FAILURE;

    }

    //
    // Set up the handling of the outputs for L1Topo
    //
      
    ATH_MSG_INFO( "Geometry XML file defined in jobO: " << m_geometryXMLFile );
    const std::string fullGeometryFileName = PathResolverFindCalibFile( m_geometryXMLFile );
    ATH_MSG_INFO( "Full path to Geometry XML file: " << fullGeometryFileName );

    m_theMuctpi->setupL1TopoConverter(fullGeometryFileName);

    //
    // Set the multiplicity summation style of the simulation:
    //
    if( m_multiplicityStrategyName == "INCLUSIVE" ) {

      ATH_MSG_DEBUG( "Setting INCLUSIVE multiplicity summation" );
      m_theMuctpi->setMultiplicityStrategy( INCLUSIVE );

    } else if( m_multiplicityStrategyName == "EXCLUSIVE" ) {

      ATH_MSG_DEBUG( "Setting EXCLUSIVE multiplicity summation" );
      m_theMuctpi->setMultiplicityStrategy( EXCLUSIVE );

    } else if( m_multiplicityStrategyName == "XML" ) {

      ATH_MSG_DEBUG( "Setting XML multiplicity summation" );
      m_theMuctpi->setMultiplicityStrategy( XML );

      ATH_MSG_INFO( "XML multiplicity file defined in jobO: "
		    << m_multiplicityXMLFile );
      const std::string packagePrefix = "TrigT1Muctpi/";
      const std::string fullFileName = PathResolverFindCalibFile( packagePrefix +
                                                                  m_multiplicityXMLFile );
      ATH_MSG_DEBUG( " Full path to XML file: " << fullFileName );
      m_theMuctpi->initializeXMLMultStrategy( fullFileName );

    } else {

      REPORT_ERROR( StatusCode::FAILURE )
	<< "Wring multiplicity summation type specified: \""
	<< m_multiplicityStrategyName << "\"";
      return StatusCode::FAILURE;

    }

    //
    // Set up the input source, and running conditions:
    //
    if( m_inputSource == "DIGITIZATION" ) {

      ATH_MSG_INFO( "Setting input source to digitization" );
      m_executeFunction = &L1Muctpi::executeFromDigi;

    } else if( m_inputSource == "AOD" ) {

      ATH_MSG_INFO( "Setting input source to AOD" );
      m_executeFunction = &L1Muctpi::executeFromAOD;

    } else if( m_inputSource == "RDO" ) {

      ATH_MSG_INFO( "Setting input source to RDO" );
      m_executeFunction = &L1Muctpi::executeFromRDO;

    } else {

      REPORT_ERROR( StatusCode::FAILURE )
	<< "Wrong input source specified: \"" << m_inputSource
	<< "\"";
      return StatusCode::FAILURE;

    }

    //
    // Give feedback about the NIM output creation:
    //
    if( m_doNimOutput ) {
      ATH_MSG_INFO( "NIM output for CTP will be created" );
      ATH_MSG_DEBUG( "SG key for the NIM object will be: "
		     << m_nimOutputLocId );
      // A small sanity check:
      if( m_nimBarrelBit == m_nimEndcapBit ) {
	REPORT_ERROR( StatusCode::FAILURE )
	  << "The barrel and endcap NIM output bits are set to end up on the "
	  << "same bit (" << m_nimBarrelBit << ")";
	return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG( "Position for the barrel NIM bit: " << m_nimBarrelBit );
      ATH_MSG_DEBUG( "Position for the endcap NIM bit: " << m_nimEndcapBit );
    } else {
      ATH_MSG_INFO( "NIM output for CTP will *not* be created" );
    }

    ATH_MSG_INFO( "Retrieving trigger config service" << m_configSvc );
    CHECK(m_configSvc.retrieve());

    return StatusCode::SUCCESS;
  }

  StatusCode L1Muctpi::beginRun( ) {

    Configuration muctpiConfiguration;
    // Connect to the LVL1 configuration service (create it if it doesn't exist):
    if(!m_IsData){
      // convert MuCTPI config to Configuration
      muctpiConfiguration.setThreshold1Candidate( m_configSvc->muctpiConfig()->lowptThreshold() );
      muctpiConfiguration.setThreshold2Candidate( m_configSvc->muctpiConfig()->highptThreshold() );
      muctpiConfiguration.setMaxCandSendToRoib( m_configSvc->muctpiConfig()->maxCand() );
    }
      
    //if running on data, hard code muctpi configuration
    else{
      muctpiConfiguration.setThreshold1Candidate( 1 );
      muctpiConfiguration.setThreshold2Candidate( 1 );
      muctpiConfiguration.setMaxCandSendToRoib( 64 );
    }

    // set Configuration
    m_theMuctpi->setConfiguration( muctpiConfiguration );

    ATH_MSG_INFO( "The Muctpi has been configured ->"
		  << " 1stThr:" << muctpiConfiguration.getThreshold1Candidate()
		  << " 2ndThr:" << muctpiConfiguration.getThreshold2Candidate()
		  << " MaximalCandCount:" << muctpiConfiguration.getMaxCandSendToRoib() );

    StatusCode sc = validate( m_configSvc->thresholdConfig()->getThresholdVector(TrigConf::L1DataDef::MUON) );

    if( sc.isFailure() ) {
      REPORT_ERROR( StatusCode::FAILURE )
	<< "LVL1 muon configuration failed sanity check";
      REPORT_ERROR( StatusCode::FAILURE )
	<< "See previous messages for more information";
      REPORT_ERROR( StatusCode::FAILURE )
	<< "The muon trigger results will not be reliable";
      return sc;
    } else {
      ATH_MSG_DEBUG( "LVL1 muon configuration passed sanity check" );
    }

    return StatusCode::SUCCESS;
  }


  //---------------------------------
  // finalize()
  //---------------------------------
  StatusCode L1Muctpi::finalize() {

    // Now this is a tricky part. We have to force the message logging of the
    // MuCTPI simulation to display messages of the same level as this MsgStream.
    MsgWriter::instance()->setMinType( msg().level() );
    MsgWriter::instance()->setSource( name() );

    ATH_MSG_INFO( "===============================" );
    ATH_MSG_INFO( "Finalizing L1Muctpi algorithm." );
    ATH_MSG_INFO( "===============================" );

    delete m_theMuctpi;
    m_theMuctpi = 0;

    return StatusCode::SUCCESS;
  }



  //----------------------------------------------
  // execute() method called once per event
  //----------------------------------------------
  StatusCode L1Muctpi::execute( ) {

    // Now this is a tricky part. We have to force the message logging of the
    // MuCTPI simulation to display messages of the same level as this MsgStream.
    MsgWriter::instance()->setMinType( msg().level() );
    MsgWriter::instance()->setSource( name() );

    ATH_MSG_DEBUG( "===============================" );
    ATH_MSG_DEBUG( "Executing L1Muctpi algorithm." );
    ATH_MSG_DEBUG( "===============================" );

    // Now this is a trictky one. I only had to make the decision of which function to call
    // at execute once in initialize(). Now I can call the right function like this:
    return ( this->*m_executeFunction )();
  }

  /**
   * This is the default execute() function. It reads inputs from the RPC and TGC sector logics,
   * and runs the MuCTPI simulation with their inputs.
   */
  StatusCode L1Muctpi::executeFromDigi() {

    ATH_MSG_DEBUG( "in executeFromDigi()" );

    //
    // Get the RPC MUCTPI-input object from StoreGate.
    //
    bool gotRPCInput( false );
    const LVL1MUONIF::Lvl1MuCTPIInput* dHandleRPC = 0;
    if( evtStore()->retrieve( dHandleRPC, m_rpcLocId ).isFailure() ) {
      ATH_MSG_WARNING( "Cannot retrieve RPC L1Muctpi object from StoreGate!" );
    } else {
      ATH_MSG_DEBUG( "Retrieved RPC Lvl1MuCTPIInput object from StoreGate" );
      gotRPCInput = true;
    }

    //
    // Get the TGC MUCTPI-input object from StoreGate.
    //
    bool gotTGCInput( false );
    const LVL1MUONIF::Lvl1MuCTPIInput* dHandleTGC = 0;
    if( evtStore()->retrieve( dHandleTGC, m_tgcLocId ).isFailure() ) {
      ATH_MSG_WARNING( "Cannot retrieve TGC L1Muctpi object from StoreGate!" );
    } else {
      ATH_MSG_DEBUG( "Retrieved TGC Lvl1MuCTPIInput object from StoreGate" );
      gotTGCInput = true;
    }

    // Merge the two inputs:
    LVL1MUONIF::Lvl1MuCTPIInput mergedInput;
    if( gotRPCInput ) mergedInput.merge( *dHandleRPC );
    if( gotTGCInput ) mergedInput.merge( *dHandleTGC );

    if( ! ( gotRPCInput || gotTGCInput ) ) {    // this should be an "&&" in final ver.
      ATH_MSG_WARNING( "No input received from the RPC or TGC sector logic simulation" );
      return StatusCode::SUCCESS;
    }

    /// Need to check here if there are several BC slices in the input object, and run
    /// the simulation for each one


    //// TESTING ONLY  /////
    /// mergedInput.duplicateToOtherBC(-1);

    //always process the central slice, which defaults to bcidOffset = 0
    // process the input in the MUCTPI simulation
    m_theMuctpi->processData( &mergedInput );      
    // Save the output of the simulation
    CHECK( saveOutput() );
      
    // check the other 4 possible BC offset values in case the input objects tells us there are
    // out of time candidates


    if (mergedInput.hasOutOfTimeCandidates(LVL1MUONIF::Lvl1MuCTPIInput::idBarrelSystem()) ||
	mergedInput.hasOutOfTimeCandidates(LVL1MUONIF::Lvl1MuCTPIInput::idEndcapSystem()) || 
	mergedInput.hasOutOfTimeCandidates(LVL1MUONIF::Lvl1MuCTPIInput::idForwardSystem()) ){
      
      for (std::vector<int>::iterator it = m_bcidOffsetList.begin(); it != m_bcidOffsetList.end(); ++it){
	if (! mergedInput.isEmptyAll( (*it) ) ){
	  // process the input in the MUCTPI simulation
	  m_theMuctpi->processData( &mergedInput, (*it));      
	  // Save the output of the simulation
	  CHECK( saveOutput( (*it) ) );	    
	}
      }
    }    

    return StatusCode::SUCCESS;
  }

  /**
   * This is a new execute() function. It reads the LVL1_ROI object from an AOD file, it converts
   * the muon RoIs back into the input format of the MuCTPI, then runs the MuCTPI information
   * with this transformed input.
   */
  StatusCode L1Muctpi::executeFromAOD() {

    ATH_MSG_DEBUG( "in executeFromAOD()" );

    // Retrieve the LVL1_ROI object from the AOD:
    const LVL1_ROI* lvl1RoI = 0;
    CHECK( evtStore()->retrieve( lvl1RoI, m_aodLocId ) );
    ATH_MSG_DEBUG( "Retrieved LVL1_ROI object from StoreGate" );

    // Extract the RoIs into a vector:
    std::vector< unsigned int > convertableRoIs;
    LVL1_ROI::muons_type::const_iterator muonRoI_itr = lvl1RoI->getMuonROIs().begin();
    LVL1_ROI::muons_type::const_iterator muonRoI_end = lvl1RoI->getMuonROIs().end();
    for( ; muonRoI_itr != muonRoI_end; ++muonRoI_itr ) {
      convertableRoIs.push_back( muonRoI_itr->getROIWord() );
    }

    // Create the input to the MuCTPI:
    LVL1MUONIF::Lvl1MuCTPIInput convertedInput;
    CHECK( Converter::convertRoIs( convertableRoIs, &convertedInput ) );

    // process the input with the MuCTPI simulation
    m_theMuctpi->processData( &convertedInput );

    // store CTP result in interface object and put to StoreGate
    LVL1::MuCTPICTP* theCTPResult = new LVL1::MuCTPICTP( m_theMuctpi->getCTPData() );
    CHECK( evtStore()->record( theCTPResult, m_ctpOutputLocId ) );
    ATH_MSG_DEBUG( "CTP word recorded to StoreGate with key: "
		   << m_ctpOutputLocId );

    // Save the output of the simulation
    CHECK( saveOutput() );

    return StatusCode::SUCCESS;
  }

  /**
   * This is a new execute() function. It reads the MuCTPI_RDO object from an RDO file, it
   * converts the muon data words back into the input format of the MuCTPI, then runs the
   * MuCTPI information with this transformed input.
   */
  StatusCode L1Muctpi::executeFromRDO() {

    ATH_MSG_DEBUG( "in executeFromRDO()" );

    // Retrieve the MuCTPI_RDO object from the digitized file:
    const MuCTPI_RDO* old_RDO = 0;
    CHECK( evtStore()->retrieve( old_RDO, m_rdoLocId ) );
    ATH_MSG_DEBUG( "Retrieved MuCTPI_RDO object from StoreGate" );

    // Get the BCID of the collision:
    const unsigned int bcid = ( ( old_RDO->candidateMultiplicity() >> 18 ) & 0x7 );

    // Create the input to the MuCTPI:
    LVL1MUONIF::Lvl1MuCTPIInput convertedInput;
    CHECK( Converter::convertRDO( old_RDO->dataWord(), bcid, &convertedInput ) );

    // process the input with the MuCTPI simulation
    m_theMuctpi->processData( &convertedInput );

    // Save the output of the simulation
    CHECK( saveOutput() );

    return StatusCode::SUCCESS;
  }

  StatusCode L1Muctpi::validate( const std::vector< TrigConf::TriggerThreshold* > &
				 thresholds ) const {

    //
    // Check that it's not a null-pointer:
    //
    //       if( ! thresholds ) {
    //          REPORT_ERROR( StatusCode::FAILURE )
    //             << "No muon threshold vector given for validation!";
    //          return StatusCode::FAILURE;
    //       }

    //
    // Check that there are 6 thresholds:
    //
    if( thresholds.size() != 6 ) {
      REPORT_ERROR( StatusCode::FAILURE )
	<< thresholds.size() << " muon thresholds defined";
      REPORT_ERROR( StatusCode::FAILURE )
	<< "There have to be exactly 6 threshold defined!";
      return StatusCode::FAILURE;
    }

    //
    // Check that they are all muon thresholds:
    //
    for( TrigConf::TriggerThreshold* thr : thresholds ) {
      if( ( thr->cableName() != "MU" && thr->cableName() != "MUCTPI" ) || ( thr->type() != "MUON" ) ) {
	REPORT_ERROR( StatusCode::FAILURE )
	  << "There is a non-muon threshold in the LVL1 muon configuration!";
	return StatusCode::FAILURE;
      }
    }

    //
    // Check that they're all on the same cable:
    //
    const std::string ctpin     = thresholds.at( 0 )->cableCtpin();
    const std::string connector = thresholds.at( 0 )->cableConnector();
    for( const TrigConf::TriggerThreshold * thr : thresholds ) {
      if( ( thr->cableCtpin() != ctpin ) || ( thr->cableConnector() != connector ) ) {
	REPORT_ERROR( StatusCode::FAILURE )
	  << "The muon thresholds are not all configured on the same cable!";
	return StatusCode::FAILURE;
      }
    }

    //
    // Check that they are configured on the correct wires:
    //
    unsigned int mask = 0;
    for( TrigConf::TriggerThreshold* thr : thresholds ) {
      if( ( thr->cableEnd() - thr->cableStart() ) != 2 ) {
	REPORT_ERROR( StatusCode::FAILURE )
	  << "A muon threshold is not 3 bit wide in the configuration!";
	return StatusCode::FAILURE;
      }
      mask |= ( static_cast< unsigned int >( pow( 2, thr->cableEnd() - thr->cableStart() + 1 ) - 1 ) << thr->cableStart() );
    }
    if( mask != 0x7fffe ) {
      REPORT_ERROR( StatusCode::FAILURE )
	<< "The muon thresholds don't cover the correct bits on the cable";
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

  /**
   * This function is used by all the different execute functions to save the output
   * of the MuCTPI simulation into various objects in StoreGate.
   */
  StatusCode L1Muctpi::saveOutput(int bcidOffset) {
     
    /// the standart processing is done for the central slice, with no Bcid offset
    if (bcidOffset == 0 ) {
      // store CTP result in interface object and put to StoreGate
      LVL1::MuCTPICTP* theCTPResult = new LVL1::MuCTPICTP( m_theMuctpi->getCTPData() );
      CHECK( evtStore()->record( theCTPResult, m_ctpOutputLocId ) );
      ATH_MSG_DEBUG( "CTP word recorded to StoreGate with key: "
                     << m_ctpOutputLocId );

      // create MuCTPI RDO
      const std::list< unsigned int >& daqData = m_theMuctpi->getDAQData();

      const int HEADER_SIZE = 9;
      const int STATUSandTAILER_SIZE = 2 + 3;

      // size check
      // payload should contain at least 1 data (Candidate Multiplicity)
      int payloadSize = daqData.size() - HEADER_SIZE - STATUSandTAILER_SIZE;
      if( payloadSize < 1 ) {
	REPORT_ERROR( StatusCode::FAILURE )
	  << "MIROD didn't provide correct DAQ data";
	return StatusCode::FAILURE;
      }

      // skip header part
      std::list< unsigned int >::const_iterator itDAQ = daqData.begin();
      for( int iHead = 0; iHead < HEADER_SIZE; ++iHead ) {
	++itDAQ;
      }

      // candidate multiplicity
      const uint32_t can = *itDAQ;
      ++itDAQ;

      // data word
      std::vector< uint32_t > dataWord;
      for( int iData = 1; iData < payloadSize; ++iData, ++itDAQ ) {
	dataWord.push_back( *itDAQ );
      }

      // create MuCTPI RDO
      MuCTPI_RDO * muCTPI_RDO = new MuCTPI_RDO( can, dataWord );
      CHECK( evtStore()->record( muCTPI_RDO, m_rdoOutputLocId ) );
      ATH_MSG_DEBUG( "MuCTPI_RDO object recorded to StoreGate with key: "
                     << m_rdoOutputLocId );

      // store RoIB result in interface object and put to StoreGate
      std::list< unsigned int > resultForRoIB = m_theMuctpi->getRoIBData();
      std::vector< unsigned int > roibResultVector;

      std::list< unsigned int >::iterator roib_itr = resultForRoIB.begin();
      std::list< unsigned int >::iterator roib_end = resultForRoIB.end();
      for( ; roib_itr != roib_end; ++roib_itr ) {
	roibResultVector.push_back( *roib_itr );
      }

      L1MUINT::MuCTPIToRoIBSLink* theRoIBResult =
	new L1MUINT::MuCTPIToRoIBSLink( roibResultVector );

      CHECK( evtStore()->record( theRoIBResult, m_roiOutputLocId ) );
      ATH_MSG_DEBUG( "RoIB result recorded to StoreGate with key: "
                     << m_roiOutputLocId );

      //construct muctpi nim words (for MUE and MUB items)
      unsigned int cw1=0;
      if( m_doNimOutput ) {
	if(m_theMuctpi->hasBarrelCandidate()){
	  unsigned int nimBarrelBitMask = 1<<m_nimBarrelBit;
	  cw1|=nimBarrelBitMask;
	}
	if(m_theMuctpi->hasEndcapCandidate()){
	  unsigned int nimEndcapBitMask = 1<<m_nimEndcapBit;
	  cw1|=nimEndcapBitMask;
	}

	//constructing NimCTP object 
	LVL1::NimCTP* nim = new LVL1::NimCTP( 0, cw1, 0 );

	// Save it into StoreGate:
	if( evtStore()->contains< LVL1::NimCTP >( m_nimOutputLocId ) ) {
	  ATH_MSG_ERROR("NimCTP object already in store gate. This should not happen!");
	  return StatusCode::FAILURE;
	}
	CHECK( evtStore()->record( nim, m_nimOutputLocId ) );
      }

      // get outputs for L1Topo and store into Storegate
      ATH_MSG_DEBUG("Getting the output for L1Topo");
      LVL1::MuCTPIL1Topo l1topoCandidates = m_theMuctpi->getL1TopoData();
      LVL1::MuCTPIL1Topo* l1topo = new LVL1::MuCTPIL1Topo(l1topoCandidates.getCandidates());
      CHECK( evtStore()->record( l1topo, m_l1topoOutputLocId ) );
      //      std::cout << "TW: ALG central slice: offset: " <<  bcidOffset << "  location: " << m_l1topoOutputLocId << std::endl;
      //      l1topo->print();
   }

    /// if we have a bcid offset, then just get the topo output and put it on storegate
    if (bcidOffset  != 0) {
      ATH_MSG_DEBUG("Getting the output for L1Topo for BCID slice");
      LVL1::MuCTPIL1Topo l1topoCandidatesBC = m_theMuctpi->getL1TopoData();
      LVL1::MuCTPIL1Topo* l1topoBC = new LVL1::MuCTPIL1Topo(l1topoCandidatesBC.getCandidates());
      l1topoBC->setBcidOffset(bcidOffset);
      CHECK( evtStore()->record( l1topoBC, m_l1topoOutputLocId+std::to_string(bcidOffset) ) );
      // std::cout << "TW: slice: offset: " <<  bcidOffset << "  location: " 
      //		<< m_l1topoOutputLocId+std::to_string(bcidOffset) << std::endl;
      // l1topoBC->print();
    }


    return StatusCode::SUCCESS;
  }

} // namespace LVL1MUCTPI
