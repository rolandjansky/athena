/*                                                                                                                      
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// First the corresponding header.
#include "MUCTPI_AthTool.h"
#include "TriggerProcessor.h"
#include "MuonSectorProcessor.h"
#include "Configuration.h"
#include "TrigThresholdDecisionTool.h"

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
#include "TrigT1MuctpiBits/HelpersPhase1.h"
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
  const std::string MUCTPI_AthTool::m_DEFAULT_barrelRoIFile            = "TrigConfMuctpi/Data_ROI_Mapping_Barrel_20201214.txt";
  const std::string MUCTPI_AthTool::m_DEFAULT_ecfRoIFile               = "TrigConfMuctpi/Data_RoI_Mapping_EF_20201214.txt";
  const std::string MUCTPI_AthTool::m_DEFAULT_side0LUTFile             = "TrigConfMuctpi/lookup_0_20201214.json";
  const std::string MUCTPI_AthTool::m_DEFAULT_side1LUTFile             = "TrigConfMuctpi/lookup_1_20201214.json";

  MUCTPI_AthTool::MUCTPI_AthTool(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type, name, parent)
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

    // These are just here for flexibility, normally they should not be changed:
    declareProperty( "TGCLocID", m_tgcLocId = m_DEFAULT_L1MuctpiStoreLocationTGC );
    declareProperty( "RPCLocID", m_rpcLocId = m_DEFAULT_L1MuctpiStoreLocationRPC );
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
    ATH_MSG_INFO("Initializing L1Topo decoding with the following inputs");
    ATH_MSG_INFO("  Barrel file: " << barrelFileName);
    ATH_MSG_INFO("  EC/Fwd file: " << ecfFileName);
    ATH_MSG_INFO("  Side 0 LUT:  " << side0LUTFileName);
    ATH_MSG_INFO("  Side 1 LUT:  " << side1LUTFileName);
    std::vector<std::string> topo_errors = m_theMuctpi.configureTopo(barrelFileName,
								      ecfFileName,
								      side0LUTFileName,
								      side1LUTFileName);
    if (topo_errors.size())
    {
      std::stringstream err;
      err << "Couldn't initialize L1Topo eta/phi encoding/decoding:\n";
      for (unsigned i=0;i<topo_errors.size();i++) err << topo_errors[i] << "\n";
      REPORT_ERROR( StatusCode::FAILURE ) << err.str();
      return StatusCode::FAILURE;
    }

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

      for (MuonSectorProcessor& msp : m_theMuctpi.getMuonSectorProcessors())
      {
        msp.configureOverlapRemoval(fullFileName);
      }

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
    ATH_CHECK(m_MuCTPI_xAODWriteKeys.initialize());
    ATH_CHECK(m_MuCTPIL1TopoKeys.initialize());

    ATH_CHECK( m_rpcTool.retrieve() );
    ATH_CHECK( m_tgcTool.retrieve() );
    ATH_CHECK( m_trigThresholdDecisionTool.retrieve() );

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
    m_theMuctpi.getTriggerProcessor().setTrigTool(*m_trigThresholdDecisionTool);
    m_theMuctpi.getTriggerProcessor().setMenu(l1menu);
    for (MuonSectorProcessor& msp : m_theMuctpi.getMuonSectorProcessors())
    {
      msp.setMenu(l1menu);
      if (!msp.configurePtEncoding())
      {
        REPORT_ERROR( StatusCode::FAILURE )
          << "Couldn't configure pt encoding in MuonSectorProcessor " << msp.getSide();
        return StatusCode::FAILURE;
      }
    }

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
    MUCTPIResults resultsBC0;
    std::string ret = m_theMuctpi.processData(&mergedInput, resultsBC0, 0);
    if (ret != "")
    {
      REPORT_ERROR( StatusCode::FAILURE ) << "Error while processing MUCTPI data: " << ret;
      return StatusCode::FAILURE;
    }
    // Save the output of the simulation
    CHECK( saveOutput(resultsBC0) );
      
    // check the other 4 possible BC offset values in case the input objects tells us there are
    // out of time candidates
    if (mergedInput.hasOutOfTimeCandidates(LVL1MUONIF::Lvl1MuCTPIInputPhase1::idBarrelSystem()) ||
        mergedInput.hasOutOfTimeCandidates(LVL1MUONIF::Lvl1MuCTPIInputPhase1::idEndcapSystem()) || 
        mergedInput.hasOutOfTimeCandidates(LVL1MUONIF::Lvl1MuCTPIInputPhase1::idForwardSystem()) ){
      
      for (const int bcidOffset : m_bcidOffsetList) {
        if ( !mergedInput.isEmptyAll(bcidOffset) ) {
          MUCTPIResults results;
          // process the input in the MUCTPI simulation
          m_theMuctpi.processData(&mergedInput, results, bcidOffset);
          // Save the output of the simulation
          CHECK( saveOutput(results, bcidOffset) );
        } else {
          // Record empty outputs
          CHECK( saveOutput(std::nullopt, bcidOffset) );
        }
      }
    } else {
      // Record emtpy outputs
      for (const int bcidOffset : m_bcidOffsetList) {
        CHECK( saveOutput(std::nullopt, bcidOffset) );
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
    // m_theMuctpi.processData( &convertedInput );

    // store CTP result in interface object and put to StoreGate
    //TEMPORARILY REMOVED UNTIL MUCTPICTP UPDATED TO VECTOR!
    //LVL1::MuCTPICTP* theCTPResult = new LVL1::MuCTPICTP( m_theMuctpi.getCTPData() );
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
    // m_theMuctpi.processData( &convertedInput );

    // // Save the output of the simulation
    // CHECK( saveOutput() );

    return StatusCode::SUCCESS;
  }




  StatusCode MUCTPI_AthTool::saveOutput(std::optional<std::reference_wrapper<MUCTPIResults>> results, int bcidOffset) const {

    // Record an empty MuCTPI xAOD container (filled later below)
    auto xAODRoIsKeyIt = m_MuCTPI_xAODWriteKeys.begin();
    std::advance(xAODRoIsKeyIt, bcidOffset+2);
    auto xAODRoIs = SG::makeHandle(*xAODRoIsKeyIt);
    ATH_CHECK(xAODRoIs.record(std::make_unique<xAOD::MuonRoIContainer>(),
                              std::make_unique<xAOD::MuonRoIAuxContainer>()));
    ATH_MSG_DEBUG("Recorded MuonRoIContainer with key " << xAODRoIs.key());

    // Create a WriteHandle for L1Topo output
    auto l1topoKeyIt = m_MuCTPIL1TopoKeys.begin();
    std::advance(l1topoKeyIt, bcidOffset+2);
    SG::WriteHandle<LVL1::MuCTPIL1Topo> l1topo(*l1topoKeyIt);

    // If no results for this BCID, record empty outputs and return
    if (!results.has_value()) {
      ATH_CHECK(l1topo.record(std::make_unique<LVL1::MuCTPIL1Topo>()));
      ATH_MSG_DEBUG("L1Topo output recorded to StoreGate with key " << l1topo.key());
      ATH_MSG_DEBUG("No results for BCID offset " << bcidOffset << " - recorded empty outputs");
      return StatusCode::SUCCESS;
    }

    // Store CTP result in interface object and put to StoreGate (only for BC=0)
    if (bcidOffset==0) {
      // Check that the multiplicity was filled
      const std::vector<unsigned int>& ctpData = results->get().ctp_words;
      if (ctpData.empty()) {
        ATH_MSG_ERROR("TriggerProcessor didn't provide correct CTP data");
        return StatusCode::FAILURE;
      }
      // Record
      SG::WriteHandle<LVL1::MuCTPICTP> muctpi_ctp(m_MuCTPICTPWriteKey);
      ATH_CHECK(muctpi_ctp.record(std::make_unique<LVL1::MuCTPICTP>(ctpData)));
      ATH_MSG_DEBUG("CTP word recorded to StoreGate with key " << muctpi_ctp.key());
    }

    // Create MuCTPI xAOD and fill the container
    for (const DAQData& data : results->get().daq_data) {
      xAODRoIs->push_back(std::make_unique<xAOD::MuonRoI>());

      LVL1::TrigT1MuonRecRoiData roiData;
      switch (LVL1::MuCTPIBits::getSubsysID(data.dataWord, /*onlineFormat=*/ false)) {
        case LVL1::MuCTPIBits::SubsysID::Endcap: // same for Endcap and Forward
        case LVL1::MuCTPIBits::SubsysID::Forward: {
          roiData = m_tgcTool->roiData(data.dataWord);
          break;
        }
        case LVL1::MuCTPIBits::SubsysID::Barrel: {
          roiData = m_rpcTool->roiData(data.dataWord);
          break;
        }
        default: {
          ATH_MSG_ERROR("Failed to determine Sector ID from RoI word 0x" << std::hex << data.dataWord << std::dec);
          return StatusCode::FAILURE;
        }
      }

      std::pair<std::string, double> minThrInfo = m_trigThresholdDecisionTool->getMinThresholdNameAndValue(data.thresholdDecisions, roiData.eta());
      xAODRoIs->back()->initialize(data.dataWord, roiData.eta(), roiData.phi(), minThrInfo.first, minThrInfo.second);
    }

    // Get outputs for L1Topo and store into Storegate
    ATH_CHECK(l1topo.record(std::make_unique<LVL1::MuCTPIL1Topo>(results->get().l1topoData.getCandidates())));
    if (bcidOffset!=0) {l1topo->setBcidOffset(bcidOffset);}
    ATH_MSG_DEBUG("L1Topo output recorded to StoreGate with key " << l1topo.key());

    return StatusCode::SUCCESS;
  }
}

