/*                                                                                                                      
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration                                               
*/

// First the corresponding header.
#include "TrigT1MuctpiPhase1/MUCTPI_AthTool.h"
#include "TrigT1MuctpiPhase1/SimController.h"
#include "TrigT1MuctpiPhase1/TriggerProcessor.h"
#include "TrigT1MuctpiPhase1/Configuration.h"
#include "TrigT1MuctpiPhase1/TrigThresholdDecisionTool.h"

// The headers from other ATLAS packages,
// from most to least dependent.

// Headers from external packages.

// The new trigger configuration
#include "TrigConfData/L1Menu.h"

// Interfaces used by the simulation
#include "TrigT1Interfaces/MuCTPICTP.h"
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"

// The output object of the simulation
#include "TrigT1Interfaces/Lvl1MuCTPIInputPhase1.h"
#include "TrigT1Interfaces/MuCTPIL1Topo.h"

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
  const std::string MUCTPI_AthTool::m_DEFAULT_barrelRoIFile          = "TrigConfMuctpi/Data_ROI_Mapping_Barrel.txt";
  const std::string MUCTPI_AthTool::m_DEFAULT_ecfRoIFile          = "TrigConfMuctpi/Data_RoI_Mapping_EF.txt";
  const std::string MUCTPI_AthTool::m_DEFAULT_side0LUTFile          = "TrigConfMuctpi/lookup_0.json";
  const std::string MUCTPI_AthTool::m_DEFAULT_side1LUTFile          = "TrigConfMuctpi/lookup_1.json";

  MUCTPI_AthTool::MUCTPI_AthTool(const std::string& type, const std::string& name, 
				 const IInterface* parent)
    :
    base_class(type, name, parent),
    m_MuCTPIL1TopoKey(LVL1MUCTPI::DEFAULT_MuonL1TopoLocation),
    m_theMuctpi(new SimController())
  {
    
    // Init message
    ATH_MSG_INFO( "=======================================" );
    ATH_MSG_INFO( "Constructor for Phase1 MUCTPI_AthTool."  );
    ATH_MSG_INFO( "=======================================" );

    // Declare the properties of the overlap treatment:
    declareProperty( "OverlapStrategyName", m_overlapStrategyName = "NULL" );
    declareProperty( "LUTXMLFile", m_lutXMLFile = "" );
    
    // Declare the properties of the output generation for L1Topo:
    declareProperty( "BarrelRoIFile", m_barrelRoIFile = m_DEFAULT_barrelRoIFile );
    declareProperty( "EndcapForwardRoIFile", m_ecfRoIFile = m_DEFAULT_ecfRoIFile );
    declareProperty( "Side0LUTFile", m_side0LUTFile = m_DEFAULT_side0LUTFile );
    declareProperty( "Side1LUTFile", m_side1LUTFile = m_DEFAULT_side1LUTFile );
       
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

    //initialize MSP ROI configuration
    const std::string barrelFileName = PathResolverFindCalibFile( m_barrelRoIFile );
    const std::string ecfFileName = PathResolverFindCalibFile( m_ecfRoIFile );
    const std::string side0LUTFileName = PathResolverFindCalibFile( m_side0LUTFile );
    const std::string side1LUTFileName = PathResolverFindCalibFile( m_side1LUTFile );
    m_theMuctpi->configureTopo(barrelFileName,
			       ecfFileName,
			       side0LUTFileName,
			       side1LUTFileName);

    //                                                                                                                                                                                        
    // Set up the overlap handling of the simulation:                                                                                                                                         
    //     
    
    if( m_overlapStrategyName == "NULL" ) {

      ATH_MSG_DEBUG( "Setting overlap strategy: \"NULL\"" );

    } else if( m_overlapStrategyName == "LUT" ) {

      ATH_MSG_DEBUG( "Setting overlap strategy: \"LUT\"" );

      ATH_MSG_INFO( "XML LUT file defined in jobO: " << m_lutXMLFile );
      const std::string fullFileName = PathResolverFindCalibFile( m_lutXMLFile );
      ATH_MSG_DEBUG( "Full path to XML LUT file: " << fullFileName );

      m_theMuctpi->configureOverlapRemoval(fullFileName);

    } else {

      REPORT_ERROR( StatusCode::FAILURE )
        << "Wrong overlap strategy name specified: \""
        << m_overlapStrategyName << "\"";
      return StatusCode::FAILURE;

    }
    


    //Initialize Read/WriteHandleKeys
    ATH_CHECK(m_muctpiPhase1KeyRPC.initialize());
    ATH_CHECK(m_muctpiPhase1KeyTGC.initialize());
    ATH_CHECK(m_MuCTPICTPWriteKey.initialize());
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

    CHECK( m_rpcTool.retrieve() );
    CHECK( m_tgcTool.retrieve() );
    CHECK( m_trigThresholdDecisionTool.retrieve() );
    m_theMuctpi->getTriggerProcessor()->setTrigTool(*m_trigThresholdDecisionTool);

    return StatusCode::SUCCESS;
  }

  StatusCode MUCTPI_AthTool::start()
  {
    ATH_MSG_INFO( "=======================================" );
    ATH_MSG_INFO( "Start for Phase1 MUCTPI_AthTool"  );
    ATH_MSG_INFO( "=======================================" );


    ATH_MSG_INFO( "initialize(): use L1 trigger menu from detector store" );
    const TrigConf::L1Menu * l1menu = nullptr;
    ATH_CHECK( m_detStore->retrieve(l1menu) ); 
    m_theMuctpi->getTriggerProcessor()->setMenu(l1menu);

    return StatusCode::SUCCESS;
  }

  //----------------------------------------------
  // execute() method called once per event
  //----------------------------------------------
  StatusCode MUCTPI_AthTool::execute( ) const {

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
  StatusCode MUCTPI_AthTool::executeFromDigi() const {

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
    bool success = m_theMuctpi->processData( &mergedInput );      
    if (!success)
    {
      REPORT_ERROR( StatusCode::FAILURE )
	<< "Error while processing MUCTPI data";
      return StatusCode::FAILURE;
    }
    // Save the output of the simulation
    CHECK( saveOutput() );
      
    // check the other 4 possible BC offset values in case the input objects tells us there are
    // out of time candidates


    if (mergedInput.hasOutOfTimeCandidates(LVL1MUONIF::Lvl1MuCTPIInputPhase1::idBarrelSystem()) ||
	mergedInput.hasOutOfTimeCandidates(LVL1MUONIF::Lvl1MuCTPIInputPhase1::idEndcapSystem()) || 
	mergedInput.hasOutOfTimeCandidates(LVL1MUONIF::Lvl1MuCTPIInputPhase1::idForwardSystem()) ){
      
      for (std::vector<int>::const_iterator it = m_bcidOffsetList.begin(); it != m_bcidOffsetList.end(); ++it){
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
  StatusCode MUCTPI_AthTool::executeFromAOD() const {

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
   
  StatusCode MUCTPI_AthTool::executeFromRDO() const {

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

  StatusCode MUCTPI_AthTool::saveOutput(int bcidOffset) const {

    /// the standart processing is done for the central slice, with no Bcid offset
    if (bcidOffset == 0 ) {
      // store CTP result in interface object and put to StoreGate
      const std::vector<unsigned int>& ctpData = m_theMuctpi->getTriggerProcessor()->getCTPData();

      LVL1::MuCTPICTP* theCTPResult = new LVL1::MuCTPICTP( ctpData );
      SG::WriteHandle<LVL1::MuCTPICTP> wh_muctpi_ctp(m_MuCTPICTPWriteKey);
      ATH_CHECK(wh_muctpi_ctp.record(std::make_unique<LVL1::MuCTPICTP>(*theCTPResult)));
      ATH_MSG_DEBUG( "CTP word recorded to StoreGate" );

      // size check
      // check that the multiplicity was properly filled
      int multSize = ctpData.size();
      if( multSize < 1 ) {
	REPORT_ERROR( StatusCode::FAILURE )
	  << "TriggerProcessor didn't provide correct CTP data";
	return StatusCode::FAILURE;
      }

      // create MuCTPI RDO
      const std::vector<DAQData>& daqData = m_theMuctpi->getTriggerProcessor()->getDAQData();

      // create MuCTPI xAOD
      auto xAODRoIs = SG::makeHandle(m_MuCTPI_xAODWriteKey);
      ATH_CHECK(xAODRoIs.record(std::make_unique<xAOD::MuonRoIContainer>(), std::make_unique<xAOD::MuonRoIAuxContainer>()));
      ATH_MSG_DEBUG("Recorded MuonRoIContainer with key " << m_MuCTPI_xAODWriteKey.key());
      for (DAQData data : daqData) {
        xAODRoIs->push_back(new xAOD::MuonRoI);

	LVL1::TrigT1MuonRecRoiData roiData;
	if (m_rpcTool->getSystem(data.dataWord) == LVL1::ITrigT1MuonRecRoiTool::Barrel) roiData = m_rpcTool->roiData(data.dataWord);
	else roiData = m_tgcTool->roiData(data.dataWord); // Endcap/Forward

	std::pair<std::string, double> minThrInfo = m_trigThresholdDecisionTool->getMinThresholdNameAndValue(data.thresholdDecisions, roiData.eta());

        xAODRoIs->back()->initialize(data.dataWord, roiData.eta(), roiData.phi(), minThrInfo.first, minThrInfo.second);
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

