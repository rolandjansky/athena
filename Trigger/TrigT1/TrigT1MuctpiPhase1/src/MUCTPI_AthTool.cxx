/*                                                                                                                      
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration                                               
*/

// First the corresponding header.
#include "TrigT1MuctpiPhase1/MUCTPI_AthTool.h"
#include "TrigT1MuctpiPhase1/SimController.h"
#include "TrigT1MuctpiPhase1/TriggerProcessor.h"
#include "TrigT1MuctpiPhase1/Configuration.h"

// The headers from other ATLAS packages,
// from most to least dependent.

// Headers from external packages.

// The new trigger configuration
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigConfL1Data/Muctpi.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/L1DataDef.h"

// Interfaces used by the simulation
#include "TrigT1Interfaces/MuCTPICTP.h"
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"

// The output object of the simulation
#include "TrigT1Interfaces/Lvl1MuCTPIInputPhase1.h"
#include "TrigT1Interfaces/MuCTPIL1Topo.h"

#include "TrigT1Result/MuCTPI_RDO.h"
#include "TrigT1Result/MuCTPIRoI.h"
#include "xAODTrigger/MuonRoI.h"
#include "xAODTrigger/MuonRoIAuxContainer.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"

#include "PathResolver/PathResolver.h"

// System headers.


namespace LVL1MUCTPIPHASE1 {
  const std::string MUCTPI_AthTool::m_DEFAULT_locationMuCTPItoCTP      = "/Run/L1MuCTPItoCTPLocation";
  const std::string MUCTPI_AthTool::m_DEFAULT_locationMuCTPItoRoIB     = "/Run/L1MuCTPItoRoIBLocation";
  const std::string MUCTPI_AthTool::m_DEFAULT_L1MuctpiStoreLocationRPC = "/Event/L1MuctpiStoreRPC";
  const std::string MUCTPI_AthTool::m_DEFAULT_L1MuctpiStoreLocationTGC = "/Event/L1MuctpiStoreTGC";
  const std::string MUCTPI_AthTool::m_DEFAULT_AODLocID                 = "LVL1_ROI";
  const std::string MUCTPI_AthTool::m_DEFAULT_RDOLocID                 = "MUCTPI_RDO";
  const std::string MUCTPI_AthTool::m_DEFAULT_geometryXMLFile          = "TrigConfMuctpi/L1MuonGeometry_20200629.xml";

  MUCTPI_AthTool::MUCTPI_AthTool(const std::string& type, const std::string& name, 
				 const IInterface* parent)
    :
    base_class(type, name, parent),
    m_MuCTPIL1TopoKey(LVL1MUCTPI::DEFAULT_MuonL1TopoLocation),
    m_MuCTPIL1TopoKey_p1(""),
    m_configSvc( "TrigConf::TrigConfigSvc/TrigConfigSvc", name ),
    m_theMuctpi(new SimController())
  {
    
    // Init message
    ATH_MSG_INFO( "=======================================" );
    ATH_MSG_INFO( "Constructor for Phase1 MUCTPI_AthTool."  );
    ATH_MSG_INFO( "=======================================" );

    // Declare the service handles as properties:
    declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service" );

    // Declare the properties of the overlap treatment:
    declareProperty( "OverlapStrategyName", m_overlapStrategyName = "NULL" );
    declareProperty( "LUTXMLFile", m_lutXMLFile = "" );
    declareProperty( "FlaggingMode", m_flagMode = false );
    
    // Declare the properties of the output generation for L1Topo:
    declareProperty( "GeometryXMLFile", m_geometryXMLFile = m_DEFAULT_geometryXMLFile );
       
    // Declare the properties for the multiplicity summation:
    declareProperty( "MultiplicityStrategyName", m_multiplicityStrategyName = "INCLUSIVE" );

    // Declare the properties of the input selection:
    declareProperty( "InputSource", m_inputSource = "DIGITIZATION" );
    declareProperty( "AODLocID", m_aodLocId = m_DEFAULT_AODLocID );
    declareProperty( "RDOLocID", m_rdoLocId = m_DEFAULT_RDOLocID );
    declareProperty( "RDOOutputLocID", m_rdoOutputLocId = m_DEFAULT_RDOLocID );
    declareProperty( "RoIOutputLocID", m_roiOutputLocId = m_DEFAULT_locationMuCTPItoRoIB );
    declareProperty( "CTPOutputLocID", m_ctpOutputLocId = m_DEFAULT_locationMuCTPItoCTP );
    declareProperty( "L1TopoOutputLocID", m_MuCTPIL1TopoKey, "Storegate key for MuCTPItoL1Topo ");

    // These are just here for flexibility, normally they should not be changed:
    declareProperty( "TGCLocID", m_tgcLocId = m_DEFAULT_L1MuctpiStoreLocationTGC );
    declareProperty( "RPCLocID", m_rpcLocId = m_DEFAULT_L1MuctpiStoreLocationRPC );
   
  }
  
  MUCTPI_AthTool::~MUCTPI_AthTool()
  {
    
  }

  void MUCTPI_AthTool::handle(const Incident& incident) {

    ATH_MSG_INFO( "=======================================" );
    ATH_MSG_INFO( "Handle in Phase1 MUCTPI_AthTool"  );
    ATH_MSG_INFO( "=======================================" );

    if (incident.type()!="BeginRun") return;
    ATH_MSG_DEBUG( "In MUCTPI_AthTool BeginRun incident");
  }

  StatusCode MUCTPI_AthTool::initialize()
  {
    ATH_MSG_INFO( "=======================================" );
    ATH_MSG_INFO( "Initialisation for Phase1 MUCTPI_AthTool"  );
    ATH_MSG_INFO( "=======================================" );

    //
    // Set up the input source, and running conditions:
    //
    if( m_inputSource == "DIGITIZATION" ) {

      ATH_MSG_INFO( "Setting input source to digitization" );
      m_executeFunction = &MUCTPI_AthTool::executeFromDigi;

    } else if( m_inputSource == "AOD" ) {

      ATH_MSG_INFO( "Setting input source to AOD" );
      m_executeFunction = &MUCTPI_AthTool::executeFromAOD;

    } else if( m_inputSource == "RDO" ) {

      ATH_MSG_INFO( "Setting input source to RDO" );
      m_executeFunction = &MUCTPI_AthTool::executeFromRDO;

    } else {

      REPORT_ERROR( StatusCode::FAILURE )
	<< "Wrong input source specified: \"" << m_inputSource
	<< "\"";
      return StatusCode::FAILURE;

    }
    ATH_MSG_INFO( "Retrieving trigger config service" << m_configSvc );
    CHECK(m_configSvc.retrieve());

    //initialize MSP ROI configuration
    const std::string fullFileName = PathResolverFindCalibFile( m_geometryXMLFile );
    m_theMuctpi->configureMSP(fullFileName);

    m_theMuctpi->getTriggerProcessor()->setThresholds(m_configSvc->thresholdConfig()->getThresholdVector(TrigConf::L1DataDef::MUON));
    //m_theMuctpi->getTriggerProcessor()->setThresholds(m_configSvc->muctpiConfig()->thresholds()); // maybe better to switch to this once new config is implemented

    //                                                                                                                                                                                        
    // Set up the overlap handling of the simulation:                                                                                                                                         
    //     
    
    if( m_overlapStrategyName == "NULL" ) {

      ATH_MSG_DEBUG( "Setting overlap strategy: \"NULL\"" );
      //m_theMuctpi->setOverlapStrategy( NO_OVERLAP ); // overlap removal to be implemented later

    } else if( m_overlapStrategyName == "LUT" ) {

      ATH_MSG_DEBUG( "Setting overlap strategy: \"LUT\"" );
      //m_theMuctpi->setOverlapStrategy( LUT_OVERLAP ); // overlap removal to be implemented later

      if( m_flagMode ) {
	ATH_MSG_INFO( "Using 'flagging mode' in the overlap handling" );
      }

      ATH_MSG_INFO( "XML LUT file defined in jobO: " << m_lutXMLFile << " with a RunPeriod=" << m_runPeriod );
      const std::string fullFileName = PathResolverFindCalibFile( m_lutXMLFile );
      ATH_MSG_DEBUG( "Full path to XML LUT file: " << fullFileName );
      //CHECK( m_theMuctpi->initializeLUTOverlapStrategy( fullFileName, m_flagMode,
      //                                                        m_dumpLut, m_runPeriod ) ); // overlap removal to be implemented later

    } else {

      REPORT_ERROR( StatusCode::FAILURE )
        << "Wrong overlap strategy name specified: \""
        << m_overlapStrategyName << "\"";
      return StatusCode::FAILURE;

    }
    


    //Initialize Read/WriteHandleKeys
    ATH_CHECK(m_muctpiPhase1KeyRPC.initialize());
    ATH_CHECK(m_muctpiPhase1KeyTGC.initialize());
    ATH_CHECK(m_MuCTPI_RDOReadKey.initialize(m_inputSource == "RDO"));
    ATH_CHECK(m_MuCTPI_RDOWriteKey.initialize(m_inputSource != "RDO"));
    ATH_CHECK(m_MuCTPI_xAODWriteKey.initialize());
    ATH_CHECK(m_MuCTPIL1TopoKey.initialize());

    m_MuCTPIL1TopoKey_m2 = m_MuCTPIL1TopoKey.key()+std::to_string(-2);
    ATH_CHECK(m_MuCTPIL1TopoKey_m2.initialize());
    m_MuCTPIL1TopoKey_m1 = m_MuCTPIL1TopoKey.key()+std::to_string(-1);
    ATH_CHECK(m_MuCTPIL1TopoKey_m1.initialize());
    m_MuCTPIL1TopoKey_p1 = m_MuCTPIL1TopoKey.key()+std::to_string(1);
    ATH_CHECK(m_MuCTPIL1TopoKey_p1.initialize());
    m_MuCTPIL1TopoKey_p2 = m_MuCTPIL1TopoKey.key()+std::to_string(2);
    ATH_CHECK(m_MuCTPIL1TopoKey_p2.initialize());


    return StatusCode::SUCCESS;
  }



  //----------------------------------------------
  // execute() method called once per event
  //----------------------------------------------
  StatusCode MUCTPI_AthTool::execute( ) {

    // Now this is a tricky part. We have to force the message logging of the
    // MuCTPI simulation to display messages of the same level as this MsgStream.
    ATH_MSG_DEBUG( "===============================" );
    ATH_MSG_DEBUG( "Executing MUCTPI_AthTool" );
    ATH_MSG_DEBUG( "===============================" );

    // Now this is a tricky one. I only had to make the decision of which function to call
    // at execute once in initialize(). Now I can call the right function like this:
    return ( this->*m_executeFunction )();
  }

  /**
   * This is the default execute() function. It reads inputs from the RPC and TGC sector logics,
   * and runs the MuCTPI simulation with their inputs.
   */
  StatusCode MUCTPI_AthTool::executeFromDigi() {

    ATH_MSG_DEBUG( "in executeFromDigi()" );

    //
    // Get the RPC MUCTPI-input object from StoreGate.
    //
    bool gotRPCInput( false );
    SG::ReadHandle<LVL1MUONIF::Lvl1MuCTPIInputPhase1> rh_muctpiRpc(m_muctpiPhase1KeyRPC);
    const LVL1MUONIF::Lvl1MuCTPIInputPhase1* dHandleRPC = rh_muctpiRpc.ptr();
    if (!dHandleRPC) {
      ATH_MSG_WARNING( "Cannot retrieve RPC L1Muctpi object from StoreGate!" );
    }
    else {
      ATH_MSG_DEBUG( "Retrieved RPC Lvl1MuCTPIInput object from StoreGate" );
      gotRPCInput = true;
    }

    //
    // Get the TGC MUCTPI-input object from StoreGate.
    //
    bool gotTGCInput( false );
    SG::ReadHandle<LVL1MUONIF::Lvl1MuCTPIInputPhase1> rh_muctpiTgc(m_muctpiPhase1KeyTGC);
    const LVL1MUONIF::Lvl1MuCTPIInputPhase1* dHandleTGC = rh_muctpiTgc.ptr();
    if (!dHandleTGC) {
      ATH_MSG_WARNING( "Cannot retrieve TGC L1Muctpi object from StoreGate!" );
    }
    else {
      ATH_MSG_DEBUG( "Retrieved TGC Lvl1MuCTPIInput object from StoreGate" );
      gotTGCInput = true;
    }

    // Merge the two inputs:
    LVL1MUONIF::Lvl1MuCTPIInputPhase1 mergedInput;

    if( ! ( gotRPCInput && gotTGCInput ) ) {
      ATH_MSG_WARNING( "No input received from the RPC or TGC sector logic simulation" );
      return StatusCode::SUCCESS;
    }

    mergedInput.merge( *dHandleRPC );
    mergedInput.merge( *dHandleTGC );

    /// Need to check here if there are several BC slices in the input object, and run
    /// the simulation for each one


    //always process the central slice, which defaults to bcidOffset = 0
    // process the input in the MUCTPI simulation
    m_theMuctpi->processData( &mergedInput );      
    // Save the output of the simulation
    CHECK( saveOutput() );
      
    // check the other 4 possible BC offset values in case the input objects tells us there are
    // out of time candidates


    if (mergedInput.hasOutOfTimeCandidates(LVL1MUONIF::Lvl1MuCTPIInputPhase1::idBarrelSystem()) ||
	mergedInput.hasOutOfTimeCandidates(LVL1MUONIF::Lvl1MuCTPIInputPhase1::idEndcapSystem()) || 
	mergedInput.hasOutOfTimeCandidates(LVL1MUONIF::Lvl1MuCTPIInputPhase1::idForwardSystem()) ){
      
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

  
  //This is a new execute() function. It reads the LVL1_ROI object from an AOD file, it converts
  //the muon RoIs back into the input format of the MuCTPI, then runs the MuCTPI information
  //with this transformed input.
  StatusCode MUCTPI_AthTool::executeFromAOD() {

    //this functionality still needs converted to the new data format.
    //in particular, Converter.cxx doesn't exist anymore.

    // Retrieve the LVL1_ROI object from the AOD:
    // const LVL1_ROI* lvl1RoI = 0;
    // CHECK( evtStore()->retrieve( lvl1RoI, m_aodLocId ) );
    // ATH_MSG_DEBUG( "Retrieved LVL1_ROI object from StoreGate" );

    // // Extract the RoIs into a vector:
    // std::vector< unsigned int > convertableRoIs;
    // LVL1_ROI::muons_type::const_iterator muonRoI_itr = lvl1RoI->getMuonROIs().begin();
    // LVL1_ROI::muons_type::const_iterator muonRoI_end = lvl1RoI->getMuonROIs().end();
    // for( ; muonRoI_itr != muonRoI_end; ++muonRoI_itr ) {
    //   convertableRoIs.push_back( muonRoI_itr->getROIWord() );
    // }

    // // Create the input to the MuCTPI:
    // LVL1MUONIF::Lvl1MuCTPIInputPhase1 convertedInput;
    // CHECK( Converter::convertRoIs( convertableRoIs, &convertedInput ) );

    // // process the input with the MuCTPI simulation
    // m_theMuctpi->processData( &convertedInput );

    // store CTP result in interface object and put to StoreGate
    //TEMPORARILY REMOVED UNTIL MUCTPICTP UPDATED TO VECTOR!
    //LVL1::MuCTPICTP* theCTPResult = new LVL1::MuCTPICTP( m_theMuctpi->getCTPData() );
    //CHECK( evtStore()->record( theCTPResult, m_ctpOutputLocId ) );
    // ATH_MSG_DEBUG( "CTP word recorded to StoreGate with key: "
    // 		   << m_ctpOutputLocId );

    // // Save the output of the simulation
    // CHECK( saveOutput() );

    return StatusCode::SUCCESS;
  }

  
  //This is a new execute() function. It reads the MuCTPI_RDO object from an RDO file, it
  //converts the muon data words back into the input format of the MuCTPI, then runs the
  //MuCTPI information with this transformed input.
   
  StatusCode MUCTPI_AthTool::executeFromRDO() {

    //this function still needs to be written.
    //in particular, converter doesn't exist anymore.

    // ATH_MSG_DEBUG( "in executeFromRDO()" );

    // // Retrieve the MuCTPI_RDO object from the digitized file:
    // SG::ReadHandle<MuCTPI_RDO> rh_muctpi_rdo(m_MuCTPI_RDOReadKey);
    // const MuCTPI_RDO* old_RDO = rh_muctpi_rdo.ptr();
    // ATH_MSG_DEBUG( "Retrieved MuCTPI_RDO object from StoreGate" );

    // // Get the BCID of the collision:
    // const unsigned int bcid = ( ( old_RDO->candidateMultiplicity() >> 18 ) & 0x7 );

    // // Create the input to the MuCTPI:
    // LVL1MUONIF::Lvl1MuCTPIInputPhase1 convertedInput;
    // CHECK( Converter::convertRDO( old_RDO->dataWord(), bcid, &convertedInput ) );

    // // process the input with the MuCTPI simulation
    // m_theMuctpi->processData( &convertedInput );

    // // Save the output of the simulation
    // CHECK( saveOutput() );

    return StatusCode::SUCCESS;
  }




  StatusCode MUCTPI_AthTool::fillMuCTPIL1Topo(LVL1::MuCTPIL1Topo& l1topoCandidates, int bcidOffset) const {

    // get outputs for L1Topo 
    ATH_MSG_DEBUG("Getting the output for L1Topo");
    l1topoCandidates = m_theMuctpi->getL1TopoData(bcidOffset);
    
    return StatusCode::SUCCESS;
  }

  StatusCode MUCTPI_AthTool::saveOutput(int bcidOffset) {

    /// the standart processing is done for the central slice, with no Bcid offset
    if (bcidOffset == 0 ) {
      // store CTP result in interface object and put to StoreGate
      std::vector<unsigned int> ctpData = m_theMuctpi->getCTPData();
    //TEMPORARILY REMOVED UNTIL MUCTPICTP UPDATED TO VECTOR!
      //LVL1::MuCTPICTP* theCTPResult = new LVL1::MuCTPICTP( ctpData );
      //CHECK( evtStore()->record( theCTPResult, m_ctpOutputLocId ) );
      //ATH_MSG_DEBUG( "CTP word recorded to StoreGate with key: "
      //               << m_ctpOutputLocId );

      // create MuCTPI RDO
      const std::vector<unsigned int>& daqData = m_theMuctpi->getDAQData();

      // size check
      // check that the multiplicity was properly filled
      int multSize = ctpData.size();
      if( multSize < 1 ) {
	REPORT_ERROR( StatusCode::FAILURE )
	  << "TriggerProcessor didn't provide correct CTP data";
	return StatusCode::FAILURE;
      }

      // data word
      std::vector<unsigned int> dataWords= daqData;

      // create MuCTPI RDO
      auto muCTPI_RDO = std::make_unique< MuCTPI_RDO >( std::move(ctpData), std::move(dataWords) );
      SG::WriteHandle<MuCTPI_RDO> wh_muctpi_rdo(m_MuCTPI_RDOWriteKey);
      ATH_CHECK(wh_muctpi_rdo.record(std::move(muCTPI_RDO)));
      ATH_MSG_DEBUG( "MuCTPI_RDO object recorded to StoreGate");

      // create MuCTPI xAOD
      auto xAODRoIs = SG::makeHandle(m_MuCTPI_xAODWriteKey);
      ATH_CHECK(xAODRoIs.record(std::make_unique<xAOD::MuonRoIContainer>(), std::make_unique<xAOD::MuonRoIAuxContainer>()));
      ATH_MSG_DEBUG("Recorded MuonRoIContainer with key " << m_MuCTPI_xAODWriteKey.key());
      for (const unsigned int word : daqData) {
        xAODRoIs->push_back(new xAOD::MuonRoI);
        // RB: dummy values just to have the objects for downstream code development
        xAODRoIs->back()->initialize(word, 99, 99, "DummyThreshold", 99);
      }

      // get outputs for L1Topo and store into Storegate
      ATH_MSG_DEBUG("Getting the output for L1Topo");
      LVL1::MuCTPIL1Topo* l1topo = new LVL1::MuCTPIL1Topo(m_theMuctpi->getL1TopoData(bcidOffset).getCandidates());
      SG::WriteHandle<LVL1::MuCTPIL1Topo> wh_l1topo(m_MuCTPIL1TopoKey);
      ATH_CHECK(wh_l1topo.record(std::unique_ptr<LVL1::MuCTPIL1Topo>(l1topo)));
    }

    /// if we have a bcid offset, then just get the topo output and put it on storegate
    if (bcidOffset  != 0) {
      ATH_MSG_DEBUG("Getting the output for L1Topo for BCID slice");
      LVL1::MuCTPIL1Topo* l1topoBC = new LVL1::MuCTPIL1Topo(m_theMuctpi->getL1TopoData(bcidOffset).getCandidates());
      l1topoBC->setBcidOffset(bcidOffset);

      if (bcidOffset == -2){
	SG::WriteHandle<LVL1::MuCTPIL1Topo> wh_l1topo(m_MuCTPIL1TopoKey_m2);
	ATH_CHECK(wh_l1topo.record(std::unique_ptr<LVL1::MuCTPIL1Topo>(l1topoBC)));
      } else if (bcidOffset == -1){
	SG::WriteHandle<LVL1::MuCTPIL1Topo> wh_l1topo(m_MuCTPIL1TopoKey_m1);
	ATH_CHECK(wh_l1topo.record(std::unique_ptr<LVL1::MuCTPIL1Topo>(l1topoBC)));
      } else if (bcidOffset == +1){
	SG::WriteHandle<LVL1::MuCTPIL1Topo> wh_l1topo(m_MuCTPIL1TopoKey_p1);
	ATH_CHECK(wh_l1topo.record(std::unique_ptr<LVL1::MuCTPIL1Topo>(l1topoBC)));
      } else if (bcidOffset == +2){
	SG::WriteHandle<LVL1::MuCTPIL1Topo> wh_l1topo(m_MuCTPIL1TopoKey_p2);
	ATH_CHECK(wh_l1topo.record(std::unique_ptr<LVL1::MuCTPIL1Topo>(l1topoBC)));
      }

    }


    return StatusCode::SUCCESS;
  }
}

