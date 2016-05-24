/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCostD3PDMakerTool.cxx 2014-09-23 12:48:32Z tamartin $

// Athena/Gaudi include(s):
#include "AthenaKernel/errorcheck.h"
#include "SGTools/crc64.h"

// STL include(s):
#include "limits.h"

// Trigger [TrigMonConfig] data include(s):
#include "TrigMonitoringEvent/TrigMonEventCollection.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"

// D3PDMaker include(s):
#include "D3PDMakerInterfaces/ID3PD.h"

// Local include(s):
#include "TrigCostD3PDMakerTool.h"

namespace D3PD {

  TrigCostD3PDMakerTool::TrigCostD3PDMakerTool( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
    : AthAlgTool( type, name, parent ),
      m_dir( "" ),
      m_incidentSvc( "IncidentSvc", name ),
      m_d3pdSvc( "D3PD::RootD3PDSvc/TrigConfD3PDSvc", name ),
      m_tree( nullptr ),
      m_onlySaveCostEvents( true ),
      m_doBasic (false),
      m_doExtra (false),
      m_doChain (false),
      m_doL1 (false),
      m_doROS (false),
      m_doROSSum (false),
      m_doRoI (false),
      m_doSeq (false),
      m_doTE (false),
      m_doDBKey (false),
      m_doEB (false),

      m_event (nullptr),
      m_lumi (nullptr),
      m_lumiLength (nullptr),
      m_bunch_id (nullptr),
      m_run (nullptr),
      m_sec (nullptr),
      m_nsec (nullptr),
      m_timer (nullptr),
      m_ebWeight (nullptr),
      m_ebWeightBG (nullptr),
      m_ebUnbiased (nullptr),
      m_ranScaleTools(nullptr),
      m_appId (nullptr),
      m_chainN (nullptr),
      m_chainCounter (nullptr),
      m_chainLevel (nullptr),
      m_chainTimer (nullptr),
      m_chainIsPassed (nullptr),
      m_chainIsPassedRaw (nullptr),
      m_chainIsPassthrough (nullptr),
      m_chainIsResurrected (nullptr),
      m_chainIsPrescaled (nullptr),
      m_chainWasL1AfterVeto (nullptr),
      m_chainWasL1BeforePrescale (nullptr),
      m_chainWasL1AfterPrescale (nullptr),
      m_chainIsExpressStream (nullptr),
      m_timerTrigCost (nullptr),
      m_timerEndSteer (nullptr),
      m_timerChainProcess (nullptr),
      m_timerResultBuilder (nullptr),
      m_timerMon (nullptr),
      m_costEvent (nullptr),
      m_costRunSec (nullptr),
      m_costRunNsec (nullptr),
      m_l1N (nullptr),
      m_l1CtpId (nullptr),
      m_l1PassedBeforePrescale (nullptr),
      m_l1PassedAfterPrescale (nullptr),
      m_l1PassedAfterVeto (nullptr),
      m_l1Passed (nullptr),
      m_l1Prescaled (nullptr),
      m_l1Vetoed (nullptr),
      m_robN (nullptr),
      m_robRequestorId (nullptr),
      m_robTimer (nullptr),
      m_robTimeStartSec (nullptr),
      m_robTimeStartMicroSec (nullptr),
      m_robTimeStopSec (nullptr),
      m_robTimeStopMicroSec (nullptr),
      m_robDataN (nullptr),
      m_robDataRobId (nullptr),
      m_robDataRobSize (nullptr),
      m_robDataIsUnclassified (nullptr),
      m_robDataIsCached (nullptr),
      m_robDataIsRetrieved (nullptr),
      m_robDataIsIgnored (nullptr),
      m_robDataIsDisabled (nullptr),
      m_robDataIsStatusOk (nullptr),
      m_robDataIsPrefetched (nullptr),
      m_robSumDataN (nullptr),
      m_robSumDataSize (nullptr),
      m_robSumDataNRob (nullptr),
      m_robSumDataSubDet (nullptr),
      m_robSumDataIsUnclassified (nullptr),
      m_robSumDataIsCached (nullptr),
      m_robSumDataIsRetrieved (nullptr),
      m_robSumDataIsIgnored (nullptr),
      m_robSumDataIsDisabled (nullptr),
      m_roiN (nullptr),
      m_roiIsNoneType (nullptr),
      m_roiIsMuonType (nullptr),
      m_roiIsEmTauType (nullptr),
      m_roiIsTau (nullptr),
      m_roiIsJetType (nullptr),
      m_roiIsJetEtType (nullptr),
      m_roiIsEnergyType (nullptr),
      m_roiId (nullptr),
      m_roiNL1thresholds (nullptr),
      m_roiEta (nullptr),
      m_roiPhi (nullptr),
      m_roiArea (nullptr),
      m_roiEt (nullptr),
      m_roiEtLarge (nullptr),
      m_roiMuCharge (nullptr),
      m_roiIsolationBits (nullptr),
      m_roiVectorEX (nullptr),
      m_roiVectorEY (nullptr),
      m_roiOverflowEX (nullptr),
      m_roiOverflowEY (nullptr),
      m_roiOverflowET (nullptr),
      m_seqN (nullptr),
      m_seqIsInitial (nullptr),
      m_seqIsExecuted (nullptr),
      m_seqIsAlreadyExecuted (nullptr),
      m_seqIsPrevious (nullptr),
      m_seqLevel (nullptr),
      m_seqChannelCounter (nullptr),
      m_seqIndex (nullptr),
      m_seqSequenceTimer (nullptr),
      m_seqAlgTimerTot (nullptr),
      m_seqAlgN (nullptr),
      m_seqAlgPosition (nullptr),
      m_seqAlgNRoIs (nullptr),
      m_seqAlgRoIIndex (nullptr),
      m_seqRoI (nullptr),
      m_seqAlgIsCached (nullptr),
      m_seqAlgIsCalled (nullptr),
      m_seqAlgTimer (nullptr),
      m_seqAlgTimeStartSec (nullptr),
      m_seqAlgTimeStartMicroSec (nullptr),
      m_seqAlgTimeStopSec (nullptr),
      m_seqAlgTimeStopMicroSec (nullptr),
      m_teN (nullptr),
      m_teId (nullptr),
      m_teIndex (nullptr),
      m_teIsRegularTE (nullptr),
      m_teIsInitialTE (nullptr),
      m_teIsRoITE (nullptr),
      m_teIsL1ThreshTE (nullptr),
      m_teActiveState (nullptr),
      m_teErrorState (nullptr),
      m_teIsTerminalNode (nullptr),
      m_teIsOutputL2Node (nullptr),
      m_teIsOutputEFNode (nullptr),
      m_teIsTopologicalTE (nullptr),
      m_teChildIndex (nullptr),
      m_teParentIndex (nullptr),
      m_teRoiId (nullptr),
      m_teClid (nullptr),
      m_DB_SMK (nullptr),
      m_DB_L1PSK (nullptr),
      m_DB_HLTPSK (nullptr)
  {

    declareProperty( "dir", m_dir = "/StreamNTUP_TRIGCOST/trig_cost" );
    declareProperty( "IncidentSvc", m_incidentSvc );
    declareProperty( "D3PDSvc", m_d3pdSvc );
    declareProperty( "prefix", m_prefix = "TrigCostHLT_" );
    declareProperty( "keyEvent", m_keyEvent = "HLT_TrigMonEventCollection_OPI_HLT_monitoring_event");
    declareProperty( "mode", m_writeMode = "COST");
    declareProperty( "onlySaveCostEvents", m_onlySaveCostEvents = true, "Export only events which ran the full suite of Cost tools online"); 
  }

  StatusCode TrigCostD3PDMakerTool::initialize() {

    ATH_MSG_INFO( "Initializing. Package version: " << PACKAGE_VERSION );

    // Retrieve the needed services:
    CHECK( m_incidentSvc.retrieve() );
    CHECK( m_d3pdSvc.retrieve() );

    // Set up the tool to listen to a few incidents:
    m_incidentSvc->addListener( this, IncidentType::EndEvent );

    // Create the configuration objects through the D3PD service:
    CHECK( m_d3pdSvc->make( m_dir, m_tree ) );

    if (m_writeMode == "COST") {
      m_doBasic = true;
      m_doExtra = true;
      m_doChain = true;
      m_doL1    = true;
      m_doROS   = true;
      m_doROSSum= false;
      m_doRoI   = true;
      m_doSeq   = true;
      m_doTE    = true;
      m_doDBKey = true;
      m_doEB    = false;
    } else if (m_writeMode == "RATE") {
      m_doBasic = true;
      m_doExtra = false;
      m_doChain = true;
      m_doL1    = true;
      m_doROS   = false;
      m_doROSSum= false;
      m_doRoI   = true;
      m_doSeq   = false;
      m_doTE    = false;
      m_doDBKey = true;
      m_doEB    = false;
    } else if (m_writeMode == "EBWEIGHT") {
      m_doBasic = false;
      m_doExtra = false;
      m_doChain = false;
      m_doL1    = false;
      m_doROS   = false;
      m_doROSSum= false;
      m_doRoI   = false;
      m_doSeq   = false;
      m_doTE    = false;
      m_doDBKey = false;
      m_doEB    = true;
    } else {
      ATH_MSG_ERROR("Unrecognised write mode " << m_writeMode << " given. Options are COST, RATE or EBWEIGHT.");
    }

    //////////////
    //BASIC DATA//
    //////////////
    if (m_doBasic) {
      CHECK( m_tree->addVariable( m_prefix + "eventNumber", m_event, "Event number" ) );
      CHECK( m_tree->addVariable( m_prefix + "lumi", m_lumi, "Lumi block number for this event" ) );
      CHECK( m_tree->addVariable( m_prefix + "lumiLength", m_lumiLength, "Length in s of this lumi block" ) );
      CHECK( m_tree->addVariable( m_prefix + "bunchCrossingId", m_bunch_id, "BCID for this event" ) );
      CHECK( m_tree->addVariable( m_prefix + "runNumber", m_run, "Run number for this event" ) );
      CHECK( m_tree->addVariable( m_prefix + "seconds", m_sec, "Start of the event in seconds" ) );
      CHECK( m_tree->addVariable( m_prefix + "nanoSeconds", m_nsec, "Start of the event nanoseconds" ) );
      CHECK( m_tree->addVariable( m_prefix + "timer", m_timer, "Nominal time for this event" ) );
      CHECK( m_tree->addVariable( m_prefix + "appId", m_appId, "Hash of AppId of the XPU node processing this event." ) );
      CHECK( m_tree->addVariable( m_prefix + "ranScaleTools", m_ranScaleTools, "If this was a monitored event (did we run the scale tools online)" ) );
      CHECK( m_tree->addVariable( m_prefix + "costRunSec", m_costRunSec, "Second the data were saved by CostMon" ) );
      CHECK( m_tree->addVariable( m_prefix + "costRunNsec", m_costRunNsec, "Nanosecond the data were saved by CostMon" ) );
      CHECK( m_tree->addVariable( m_prefix + "costEvent", m_costEvent, "Sequential number of cost events processed" ) );
    }

    if (m_doEB && !m_doBasic) {
      CHECK( m_tree->addVariable( m_prefix + "eventNumber", m_event, "Event number" ) );
      CHECK( m_tree->addVariable( m_prefix + "ebWeight", m_ebWeight, "Enhanced bias weighting factor." ) );
      CHECK( m_tree->addVariable( m_prefix + "ebWeightBG", m_ebWeightBG, "Enhanced bias bunch group identifier." ) );
      CHECK( m_tree->addVariable( m_prefix + "ebUnbiased", m_ebUnbiased, "If EB event was unbiased online." ) );
    }

    //////////////
    //EXTRA DATA//
    //////////////
    if (m_doExtra) {
      CHECK( m_tree->addVariable( m_prefix + "timerTrigCost", m_timerTrigCost, "Detailed: Time to run the OPI trig cost tools" ) );
      CHECK( m_tree->addVariable( m_prefix + "timerEndSteer", m_timerEndSteer, "Detailed: Time to end of steering" ) );
      CHECK( m_tree->addVariable( m_prefix + "timerChainProcess", m_timerChainProcess, "Detailed: Time of chain execution" ) );
      CHECK( m_tree->addVariable( m_prefix + "timerResultBuilder", m_timerResultBuilder, "Detailed: Time of ResultBuilder tool execution" ) );
      CHECK( m_tree->addVariable( m_prefix + "timerMon", m_timerMon, "Detailed: Total time of monitoring tool(s) execution" ) );
    }

    //////////////
    //CHAIN DATA//
    //////////////
    if (m_doChain) {
      CHECK( m_tree->addVariable( m_prefix + "chain_n", m_chainN, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "chain_counter", m_chainCounter, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "chain_level", m_chainLevel, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "chain_timer", m_chainTimer, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "chain_isPassed", m_chainIsPassed, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "chain_isPassedRaw", m_chainIsPassedRaw, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "chain_isPassthrough", m_chainIsPassthrough, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "chain_isResurrected", m_chainIsResurrected, "" ) ); 
      CHECK( m_tree->addVariable( m_prefix + "chain_isPrescaled", m_chainIsPrescaled, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "chain_wasL1AfterVeto", m_chainWasL1AfterVeto, "" ) );     
      CHECK( m_tree->addVariable( m_prefix + "chain_wasL1BeforePrescale", m_chainWasL1BeforePrescale, "" ) );  
      CHECK( m_tree->addVariable( m_prefix + "chain_wasL1AfterPrescale", m_chainWasL1AfterPrescale, "" ) );  
      CHECK( m_tree->addVariable( m_prefix + "chain_isExpressStream", m_chainIsExpressStream, "" ) );
    }

    ///////////
    //L1 DATA//
    ///////////
    if (m_doL1) {
      CHECK( m_tree->addVariable( m_prefix + "l1_n", m_l1N, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "l1_ctpId", m_l1CtpId, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "l1_passedBeforePrescale", m_l1PassedBeforePrescale, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "l1_passedAfterPrescale", m_l1PassedAfterPrescale, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "l1_passedAfterVeto", m_l1PassedAfterVeto, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "l1_passed", m_l1Passed, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "l1_isPrescaled", m_l1Prescaled, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "l1_isVetoed", m_l1Vetoed, "" ) ); 
    }

    ////////////
    //ROS DATA//
    ////////////
    if (m_doROS) {
      CHECK( m_tree->addVariable( m_prefix + "rob_n", m_robN, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "rob_requestorId", m_robRequestorId, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "rob_timer", m_robTimer, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "rob_timeStartSec", m_robTimeStartSec, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "rob_timeStartMicroSec", m_robTimeStartMicroSec, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "rob_timeStopSec", m_robTimeStopSec, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "rob_timeStopMicroSec", m_robTimeStopMicroSec, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "rob_data_n", m_robDataN, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "rob_data_id", m_robDataRobId, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "rob_data_size", m_robDataRobSize, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "rob_data_isUnclassified", m_robDataIsUnclassified, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "rob_data_isCached", m_robDataIsCached, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "rob_data_isRetrieved", m_robDataIsRetrieved, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "rob_data_isIgnored", m_robDataIsIgnored, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "rob_data_isDisabled", m_robDataIsDisabled, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "rob_data_isStatusOk", m_robDataIsStatusOk, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "rob_data_isStatusPrefetched", m_robDataIsPrefetched, "" ) );
    }

    ////////////////
    //ROS SUM DATA//
    ////////////////
    if (m_doROSSum) {
      CHECK( m_tree->addVariable( m_prefix + "rob_sum_n", m_robSumDataN, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "rob_sum_size", m_robSumDataSize, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "rob_sum_nRob", m_robSumDataNRob, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "rob_sum_subDet", m_robSumDataSubDet, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "rob_sum_isUnclassified", m_robSumDataIsUnclassified, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "rob_sum_isCached", m_robSumDataIsCached, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "rob_sum_isRetrieved", m_robSumDataIsRetrieved, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "rob_sum_isIgnored", m_robSumDataIsIgnored, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "rob_sum_isDisabled", m_robSumDataIsDisabled, "" ) );
    }


    ////////////
    //RoI DATA//
    ////////////
    if (m_doRoI) {
      CHECK( m_tree->addVariable( m_prefix + "roi_n", m_roiN, "Number of L1 RoI" ) );
      CHECK( m_tree->addVariable( m_prefix + "roi_isTypeNone", m_roiIsNoneType, "Flag RoI has no type" ) );
      CHECK( m_tree->addVariable( m_prefix + "roi_isTypeMuon", m_roiIsMuonType, "Flag RoI was MUON" ) );
      CHECK( m_tree->addVariable( m_prefix + "roi_isTypeEmTau", m_roiIsEmTauType, "Flag RoI was EMTAU (see isTau)" ) );
      CHECK( m_tree->addVariable( m_prefix + "roi_isTau", m_roiIsTau, "Flag, disambiguity between EM and TAU" ) );
      CHECK( m_tree->addVariable( m_prefix + "roi_isTypeJet", m_roiIsJetType, "Flag RoI was Jet" ) );
      CHECK( m_tree->addVariable( m_prefix + "roi_isTypeJetEt", m_roiIsJetEtType, "Floag RoI was JetET" ) );
      CHECK( m_tree->addVariable( m_prefix + "roi_isTypeEnergy", m_roiIsEnergyType, "Flag RoI was Energy" ) );
      CHECK( m_tree->addVariable( m_prefix + "roi_id", m_roiId, "RoI ID for matching to algorithms" ) );
      CHECK( m_tree->addVariable( m_prefix + "roi_nL1Thresholds", m_roiNL1thresholds, "Number of thresholds passed, depends on config" ) );
      CHECK( m_tree->addVariable( m_prefix + "roi_eta", m_roiEta, "RoI eta coordinate" ) );
      CHECK( m_tree->addVariable( m_prefix + "roi_phi", m_roiPhi, "RoI phi coordinate" ) );
      CHECK( m_tree->addVariable( m_prefix + "roi_area", m_roiArea, "RoI area in eta x phi" ) ); 
      // Extra RoI
      CHECK( m_tree->addVariable( m_prefix + "roi_et", m_roiEt, "RoI ET for EMTAU, ETSmall for Jet, pT for muon, scalar sum ET for ENERGY" ) ); 
      CHECK( m_tree->addVariable( m_prefix + "roi_etLarge", m_roiEtLarge, "RoI ET Large for jets only" ) ); 
      CHECK( m_tree->addVariable( m_prefix + "roi_muCharge", m_roiMuCharge, "RoI muon charge, muons only" ) ); 
      CHECK( m_tree->addVariable( m_prefix + "roi_isoBits", m_roiIsolationBits, "RoI isolation bits, EMTAU only" ) ); 
      CHECK( m_tree->addVariable( m_prefix + "roi_vectorEX", m_roiVectorEX, "RoI signed vector sum ET in X, ENERGY only" ) ); 
      CHECK( m_tree->addVariable( m_prefix + "roi_vectorEY", m_roiVectorEY, "RoI signed vector sum ET in Y, ENEGY only" ) ); 
      CHECK( m_tree->addVariable( m_prefix + "roi_overflowEX", m_roiOverflowEX, "RoI vector sum ET X overflow bit" ) ); 
      CHECK( m_tree->addVariable( m_prefix + "roi_overflowEY", m_roiOverflowEY, "RoI vector sum ET Y overflow bit" ) ); 
      CHECK( m_tree->addVariable( m_prefix + "roi_overflowET", m_roiOverflowET, "RoI scalar sum ET overflow bit" ) ); 
    }

    ////////////////
    //SEQ/ALG DATA//
    ////////////////
    if (m_doSeq) {
      CHECK( m_tree->addVariable( m_prefix + "seq_n", m_seqN, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "seq_level", m_seqLevel, "" ) ); 
      CHECK( m_tree->addVariable( m_prefix + "seq_isInitial", m_seqIsInitial, "" ) ); 
      CHECK( m_tree->addVariable( m_prefix + "seq_isExecuted", m_seqIsExecuted, "" ) ); 
      CHECK( m_tree->addVariable( m_prefix + "seq_isAlreadyExecuted", m_seqIsAlreadyExecuted, "" ) ); 
      CHECK( m_tree->addVariable( m_prefix + "seq_isPrevious", m_seqIsPrevious, "" ) ); 
      CHECK( m_tree->addVariable( m_prefix + "seq_channelCounter", m_seqChannelCounter, "" ) );         
      CHECK( m_tree->addVariable( m_prefix + "seq_index", m_seqIndex, "" ) );         
      CHECK( m_tree->addVariable( m_prefix + "seq_timer", m_seqSequenceTimer, "" ) );         
      CHECK( m_tree->addVariable( m_prefix + "seq_algTotalTime", m_seqAlgTimerTot, "" ) );         
      CHECK( m_tree->addVariable( m_prefix + "seq_alg_n", m_seqAlgN, "" ) );         
      CHECK( m_tree->addVariable( m_prefix + "seq_alg_position", m_seqAlgPosition, "" ) );  
      CHECK( m_tree->addVariable( m_prefix + "seq_alg_roi_n", m_seqAlgNRoIs, "How many RoIs this alg has." ) );   
      CHECK( m_tree->addVariable( m_prefix + "seq_alg_roi_index", m_seqAlgRoIIndex, "Index where to find these RoI IDs" ) );
      CHECK( m_tree->addVariable( m_prefix + "seq_roi", m_seqRoI, "Look at this index to get the vector<RoI ID>" ) ); 
      CHECK( m_tree->addVariable( m_prefix + "seq_alg_isCached", m_seqAlgIsCached, "" ) );       
      CHECK( m_tree->addVariable( m_prefix + "seq_alg_isCalled", m_seqAlgIsCalled, "" ) );       
      CHECK( m_tree->addVariable( m_prefix + "seq_alg_timer", m_seqAlgTimer, "" ) );    
      CHECK( m_tree->addVariable( m_prefix + "seq_alg_timeStartSec", m_seqAlgTimeStartSec, "" ) );  
      CHECK( m_tree->addVariable( m_prefix + "seq_alg_timeStartMicroSec", m_seqAlgTimeStartMicroSec, "" ) );  
      CHECK( m_tree->addVariable( m_prefix + "seq_alg_timeStopSec", m_seqAlgTimeStopSec, "" ) );  
      CHECK( m_tree->addVariable( m_prefix + "seq_alg_timeStopMicroSec", m_seqAlgTimeStopMicroSec, "" ) ); 
    }

    ///////////
    //TE DATA//
    ///////////
    if (m_doTE) {
      CHECK( m_tree->addVariable( m_prefix + "te_n", m_teN, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "te_id", m_teId, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "te_index", m_teIndex, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "te_isRegularTe", m_teIsRegularTE, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "te_isInitialTe", m_teIsInitialTE, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "te_isRoITe", m_teIsRoITE, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "te_isL1ThresholdTe", m_teIsL1ThreshTE, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "te_isActiveState", m_teActiveState, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "te_isErrorState", m_teErrorState, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "te_isTerminalNode", m_teIsTerminalNode, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "te_isOutputL2Node", m_teIsOutputL2Node, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "te_isOutputEFNode", m_teIsOutputEFNode, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "te_isTopologicalTe", m_teIsTopologicalTE, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "te_childIndex", m_teChildIndex, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "te_parentIndex", m_teParentIndex, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "te_roiId", m_teRoiId, "" ) );
      CHECK( m_tree->addVariable( m_prefix + "te_clid", m_teClid, "" ) );  
    }

    ///////////
    //DB DATA//
    ///////////
    if (m_doDBKey) {
      CHECK( m_tree->addVariable( "trig_DB_SMK", m_DB_SMK, "" ) );
      CHECK( m_tree->addVariable( "trig_DB_L1PSK", m_DB_L1PSK, "" ) );
      CHECK( m_tree->addVariable( "trig_DB_HLTPSK", m_DB_HLTPSK, "" ) );
    }

    return StatusCode::SUCCESS;
  }

  StatusCode TrigCostD3PDMakerTool::queryInterface( const InterfaceID& riid,
                                                    void** ppvIf ) {

    if( riid == IMetadataTool::interfaceID() ) {
       *ppvIf = static_cast< IMetadataTool* >( this );
       addRef();
       return StatusCode::SUCCESS;
    } else if( riid == IIncidentListener::interfaceID() ) {
       *ppvIf = static_cast< IIncidentListener* >( this );
       addRef();
       return StatusCode::SUCCESS;
    }

    return AthAlgTool::queryInterface( riid, ppvIf );
  }

   /**
    * The function is actually empty. The metadata should already be in the output
    * file at this point...
    */
  StatusCode TrigCostD3PDMakerTool::writeMetadata( ID3PD* ) {

    ATH_MSG_INFO( "Trig cost information available in the D3PD in directory: \""
                  << m_dir << "\"" );

    return StatusCode::SUCCESS;
  }

   /**
    * This function is called whenever an incident happens that the tool
    * was set up to listen to. The function has to decide what kind of
    * incident it was, and the call the appropriate internal function to
    * handle that incident.
    *
    * @param inc The incident that happened
    */
  void TrigCostD3PDMakerTool::handle( const Incident& inc ) {
    //
    // Handle trigger configuration change incidents:
    //
    if( inc.type() == IncidentType::EndEvent ) { 
       handleNewEvent( inc );
       return;
    }

    ATH_MSG_WARNING( "Received incident not recognised" );
    return;
  }
   
   
  /**
  * Save all events in the TrigMonEventCollection to the D3PD
  * Note magic numbers come from TrigMonVar.h
  */
  void TrigCostD3PDMakerTool::handleNewEvent( const Incident& ) {
      
    const TrigMonEventCollection *eventCol = 0;
 
    if( !evtStore()->contains<TrigMonEventCollection>(m_keyEvent) ) {
      ATH_MSG_DEBUG( "No TrigMonEventCollection in this event Key:" << m_keyEvent );
      return;
    } else {
      ATH_MSG_DEBUG( "TrigMonEventCollection in this event Key:" << m_keyEvent << ". Exporting to D3PD." );
    }
   
    if( evtStore()->retrieve(eventCol, m_keyEvent).isFailure() || !eventCol) {
      ATH_MSG_WARNING( "TrigMonEventCollection present, however failed to retrieve from SG. Key:" << m_keyEvent );
      return;
    }
      
    ATH_MSG_DEBUG( "Got TrigMonEventCollection: " << m_keyEvent << " Size:" << eventCol->size() );

    for( TrigMonEventCollection::const_iterator it = eventCol->begin(); it != eventCol->end(); ++it) {
      const TrigMonEvent* event = *it;
      if(!event) {
        ATH_MSG_WARNING( "Could not resolve TrigMonEvent pointer." );
        continue;
      }

      // Do we want to save this event?
      if (m_onlySaveCostEvents == true) {
        float _result = 0.;
        event->getVar(Trig::kIsCostEvent, _result); // 47, is the magic number in this case. 
        // Bool stored as float. Test for 0
        if (_result < 0.5) {
          ATH_MSG_DEBUG( "Not a Cost Event (scale tools were not run, but L1 result info still there). Skipping event due to m_onlySaveCostEvents == true" );
          continue;
        }
      }

      // Zero before new event
      clear();
	
      //////////////
      //BASIC DATA//
      //////////////
      if (m_doBasic) {
        *m_event    = event->getEvent();
        *m_lumi     = event->getLumi();
        *m_bunch_id = event->getBunchId();
        *m_run      = event->getRun();
        *m_sec      = event->getSec();
        *m_nsec     = event->getNanoSec();
        *m_timer    = event->getTimer();

        for (unsigned i=0; i < event->getWord().size(); ++i) {
          switch (i) {
            case 0:
              *m_costRunSec = event->getWord().at(i);
              break;
            case 1:
              *m_costRunNsec = event->getWord().at(i);
              break;
            case 2:
              *m_appId = event->getWord().at(i);
              break;
            default:
              break;
          }
        }  
        for (unsigned i=0; i < event->getVarKey().size(); ++i) {
          switch (event->getVarKey().at(i)) {
            case Trig::kIsCostEvent:
              *m_ranScaleTools = (uint8_t) event->getVarVal().at(i);
              break;
            case Trig::kEventLumiBlockLength:
              *m_lumiLength = event->getVarVal().at(i);
              break;
            case Trig::kEventNumber:
              *m_costEvent = event->getVarVal().at(i);
              break;
            default:
              break;
          }
        }
      }

      //////////////
      //EXTRA DATA//
      //////////////
      if (m_doExtra) {
        // Check and save additonal payload relating to the event
        if ( event->getVarKey().size() != event->getVarVal().size() ) {
          REPORT_MESSAGE( MSG::WARNING ) << "Trigger cost monitor additional integer payload size different to key size, skipping.";
        } else {
          for (unsigned i=0; i < event->getVarKey().size(); ++i) {
            switch (event->getVarKey().at(i)) {
              case Trig::kTimeCostMonitoring:
                *m_timerTrigCost = event->getVarVal().at(i);
                break;
              case Trig::kTimeExec:
                *m_timerEndSteer = event->getVarVal().at(i);
                break;
              case Trig::kTimeProc:
                *m_timerChainProcess = event->getVarVal().at(i);
                break;
              case Trig::kTimeRes:
                *m_timerResultBuilder = event->getVarVal().at(i);
                break;
              case Trig::kTimeMon:
                *m_timerMon = event->getVarVal().at(i);
                break;
              default:
                break;
            } // Switch
          } // Key loop
        } // Size check
      } // m_doExtra

      ///////////
      //EB DATA//
      ///////////
      if (m_doEB && !m_doBasic) {
        *m_event = event->getEvent();
        for (unsigned i=0; i < event->getVarKey().size(); ++i) {
          switch (event->getVarKey().at(i)) {
            case Trig::kEBWeight:
              *m_ebWeight = event->getVarVal().at(i);
              break;
            case Trig::kEBBunchGroup:
              *m_ebWeightBG = (uint32_t) event->getVarVal().at(i);
              break;
            case Trig::kEBIsUnbiasedFlag:
              *m_ebUnbiased = (uint8_t) event->getVarVal().at(i);
              break;
            default:
              break;
          }
        }
      }

      //////////////
      //CHAIN DATA//
      //////////////
      const std::vector<TrigMonChain> eventChains = event->getChains();
      if (m_doChain && eventChains.size() > 0) {
        *m_chainN = eventChains.size();
        //Save details about the chain
        m_chainCounter            ->resize( eventChains.size() );
        m_chainLevel              ->resize( eventChains.size() );         
        m_chainTimer              ->resize( eventChains.size() );         
        m_chainIsPassed           ->resize( eventChains.size() );
        m_chainIsPassedRaw        ->resize( eventChains.size() );
        m_chainIsPassthrough      ->resize( eventChains.size() );    
        m_chainIsResurrected      ->resize( eventChains.size() );
        m_chainIsPrescaled        ->resize( eventChains.size() );
        m_chainWasL1AfterVeto     ->resize( eventChains.size() );
        m_chainWasL1BeforePrescale->resize( eventChains.size() );
        m_chainWasL1AfterPrescale ->resize( eventChains.size() );
        m_chainIsExpressStream    ->resize( eventChains.size() );
        for(unsigned int i = 0; i < eventChains.size(); ++i) {
          m_chainCounter            ->at(i) = eventChains.at(i).getCounter();     
          m_chainLevel              ->at(i) = eventChains.at(i).getLevel();
          m_chainTimer              ->at(i) = eventChains.at(i).getTimer();
          m_chainIsPassed           ->at(i) = (uint8_t) eventChains.at(i).isPassed( TrigMonChain::kPassed );
          m_chainIsPassedRaw        ->at(i) = (uint8_t) eventChains.at(i).isPassed( TrigMonChain::kPassedRaw );
          m_chainIsPassthrough      ->at(i) = (uint8_t) eventChains.at(i).isPassed( TrigMonChain::kPassedThrough );
          m_chainIsResurrected      ->at(i) = (uint8_t) eventChains.at(i).isPassed( TrigMonChain::kResurrected );
          m_chainIsPrescaled        ->at(i) = (uint8_t) eventChains.at(i).isPassed( TrigMonChain::kPrescaled );
          m_chainWasL1AfterVeto     ->at(i) = (uint8_t) eventChains.at(i).isPassed( TrigMonChain::kL1AfterVeto );
          m_chainWasL1BeforePrescale->at(i) = (uint8_t) eventChains.at(i).isPassed( TrigMonChain::kL1BeforePrescale );
          m_chainWasL1AfterPrescale ->at(i) = (uint8_t) eventChains.at(i).isPassed( TrigMonChain::kL1AfterPrescale );
          m_chainIsExpressStream    ->at(i) = (uint8_t) eventChains.at(i).isPassed( TrigMonChain::kExpressStream );

          // Check and save additonal payload 
          if ( eventChains.at(i).getVarKey().size() != eventChains.at(i).getVarVal().size() ) {
            REPORT_MESSAGE( MSG::WARNING ) << "Trigger chain float payload size different to key size, skipping.";
          } else {
            for (unsigned j=0; j < eventChains.at(i).getVarKey().size(); ++j) {
              REPORT_MESSAGE( MSG::DEBUG ) << "Got a CHAIN["<<i<<"] int=>float pair. Currently not saved. " << eventChains.at(i).getVarKey().at(j) << " => " << eventChains.at(i).getVarVal().at(j);
            }
          }  
        }
      }

      ///////////
      //L1 DATA//
      ///////////
      const std::vector<TrigMonL1Item> eventL1Items = event->getL1Items();
      if (m_doL1 && eventL1Items.size() > 0) {
        *m_l1N = eventL1Items.size();
        m_l1CtpId               ->resize( eventL1Items.size() );
        m_l1PassedBeforePrescale->resize( eventL1Items.size() );    
        m_l1PassedAfterPrescale ->resize( eventL1Items.size() );    
        m_l1PassedAfterVeto     ->resize( eventL1Items.size() );
        m_l1Passed              ->resize( eventL1Items.size() );
        m_l1Prescaled           ->resize( eventL1Items.size() );
        m_l1Vetoed              ->resize( eventL1Items.size() );  
        for (unsigned int i = 0; i < eventL1Items.size(); ++i) {
          m_l1CtpId               ->at(i) = eventL1Items.at(i).getCtpId();
          m_l1PassedBeforePrescale->at(i) = (uint8_t) eventL1Items.at(i).isPassedBeforePrescale();
          m_l1PassedAfterPrescale ->at(i) = (uint8_t) eventL1Items.at(i).isPassedAfterPrescale();
          m_l1PassedAfterVeto     ->at(i) = (uint8_t) eventL1Items.at(i).isPassedAfterVeto();
          m_l1Passed              ->at(i) = (uint8_t) eventL1Items.at(i).isPassed();
          m_l1Prescaled           ->at(i) = (uint8_t) eventL1Items.at(i).isPrescaled();
          m_l1Vetoed              ->at(i) = (uint8_t) eventL1Items.at(i).isVeto();
        }
      }

      ////////////
      //ROS DATA//
      ////////////
      const std::vector<TrigMonROB> eventROBs = event->getVec<TrigMonROB>();
      if (m_doROS && eventROBs.size() > 0) {
        *m_robN = eventROBs.size();
        m_robRequestorId          ->resize( eventROBs.size() );
        m_robTimer                ->resize( eventROBs.size() ); 
        m_robTimeStartSec         ->resize( eventROBs.size() );
        m_robTimeStartMicroSec    ->resize( eventROBs.size() );
        m_robTimeStopSec           ->resize( eventROBs.size() );
        m_robTimeStopMicroSec      ->resize( eventROBs.size() );
        m_robDataN                ->resize( eventROBs.size() );
        m_robDataRobId            ->resize( eventROBs.size() );
        m_robDataRobSize          ->resize( eventROBs.size() );
        m_robDataIsUnclassified   ->resize( eventROBs.size() );
        m_robDataIsCached         ->resize( eventROBs.size() );
        m_robDataIsRetrieved      ->resize( eventROBs.size() );
        m_robDataIsIgnored        ->resize( eventROBs.size() );
        m_robDataIsDisabled       ->resize( eventROBs.size() );
        m_robDataIsStatusOk       ->resize( eventROBs.size() );
        m_robDataIsPrefetched     ->resize( eventROBs.size() );   
        for(unsigned int i = 0; i < eventROBs.size(); ++i) { 
          m_robRequestorId      ->at(i) = eventROBs.at(i).getRequestorId();
          m_robTimer            ->at(i) = eventROBs.at(i).getTimer();
          m_robTimeStartSec     ->at(i) = eventROBs.at(i).start().getSec();
          m_robTimeStartMicroSec->at(i) = eventROBs.at(i).start().getMicroSec();
          m_robTimeStopSec      ->at(i) = eventROBs.at(i).stop().getSec();
          m_robTimeStopMicroSec ->at(i) = eventROBs.at(i).stop().getMicroSec();
          //Loop over ROB data items
          const std::vector<TrigMonROBData> robData = eventROBs.at(i).getData(); 
          m_robDataN->at(i) = robData.size();
          (m_robDataRobId         ->at(i)).resize( robData.size() );
          (m_robDataRobSize       ->at(i)).resize( robData.size() );
          (m_robDataIsUnclassified->at(i)).resize( robData.size() );
          (m_robDataIsCached      ->at(i)).resize( robData.size() );
          (m_robDataIsRetrieved   ->at(i)).resize( robData.size() );
          (m_robDataIsIgnored     ->at(i)).resize( robData.size() );
          (m_robDataIsDisabled    ->at(i)).resize( robData.size() );
          (m_robDataIsStatusOk    ->at(i)).resize( robData.size() );
          (m_robDataIsPrefetched  ->at(i)).resize( robData.size() ); 
          for (unsigned int j = 0; j < robData.size(); ++j) {
            (m_robDataRobId         ->at(i)).at(j) = robData.at(j).getROBId();
            (m_robDataRobSize       ->at(i)).at(j) = robData.at(j).getROBSize();
            (m_robDataIsUnclassified->at(i)).at(j) = (uint8_t) robData.at(j).isUnclassified();
            (m_robDataIsCached      ->at(i)).at(j) = (uint8_t) robData.at(j).isCached();
            (m_robDataIsRetrieved   ->at(i)).at(j) = (uint8_t) robData.at(j).isRetrieved();
            (m_robDataIsIgnored     ->at(i)).at(j) = (uint8_t) robData.at(j).isIgnored();
            (m_robDataIsDisabled    ->at(i)).at(j) = (uint8_t) robData.at(j).isDisabled();
            (m_robDataIsStatusOk    ->at(i)).at(j) = (uint8_t) robData.at(j).isStatusOk();
            (m_robDataIsPrefetched  ->at(i)).at(j) = (uint8_t) robData.at(j).isStatusPrefetched(); 
          }
        }
      }

      ////////////////
      //ROS SUM DATA//
      ////////////////
      if (m_doROSSum && eventROBs.size() > 0) {
        m_robSumDataN             ->resize( eventROBs.size() );
        m_robSumDataSize          ->resize( eventROBs.size() );
        m_robSumDataNRob          ->resize( eventROBs.size() );
        m_robSumDataSubDet        ->resize( eventROBs.size() );   
        m_robSumDataIsUnclassified->resize( eventROBs.size() );
        m_robSumDataIsCached      ->resize( eventROBs.size() );
        m_robSumDataIsRetrieved   ->resize( eventROBs.size() );
        m_robSumDataIsIgnored     ->resize( eventROBs.size() );
        m_robSumDataIsDisabled    ->resize( eventROBs.size() );
        for(unsigned int i = 0; i < eventROBs.size(); ++i) { 
          //Loop over ROB SUM data items
          const std::vector<TrigMonROBSum> robSum = eventROBs.at(i).getSum();
          m_robSumDataN->at(i) = robSum.size();
          (m_robSumDataSize          ->at(i)).resize( robSum.size() );
          (m_robSumDataNRob          ->at(i)).resize( robSum.size() );
          (m_robSumDataSubDet        ->at(i)).resize( robSum.size() );   
          (m_robSumDataIsUnclassified->at(i)).resize( robSum.size() );
          (m_robSumDataIsCached      ->at(i)).resize( robSum.size() );
          (m_robSumDataIsRetrieved   ->at(i)).resize( robSum.size() );
          (m_robSumDataIsIgnored     ->at(i)).resize( robSum.size() );
          (m_robSumDataIsDisabled    ->at(i)).resize( robSum.size() );
          for (unsigned int j = 0; j < robSum.size(); ++j) {
            (m_robSumDataSize          ->at(i)).at(j) = robSum.at(j).getSize();
            (m_robSumDataNRob          ->at(i)).at(j) = robSum.at(j).getNROB();
            (m_robSumDataSubDet        ->at(i)).at(j) = robSum.at(j).getSubDet();
            (m_robSumDataIsUnclassified->at(i)).at(j) = (uint8_t) (robSum.at(j).getHistory() == TrigMonROBData::kUNCLASSIFIED);
            (m_robSumDataIsCached      ->at(i)).at(j) = (uint8_t) (robSum.at(j).getHistory() == TrigMonROBData::kCACHED);
            (m_robSumDataIsRetrieved   ->at(i)).at(j) = (uint8_t) (robSum.at(j).getHistory() == TrigMonROBData::kRETRIEVED);
            (m_robSumDataIsIgnored     ->at(i)).at(j) = (uint8_t) (robSum.at(j).getHistory() == TrigMonROBData::kIGNORED);
            (m_robSumDataIsDisabled    ->at(i)).at(j) = (uint8_t) (robSum.at(j).getHistory() == TrigMonROBData::kDISABLED);
          }
        }
      }

      ////////////
      //RoI DATA//
      ////////////
      const std::vector<TrigMonRoi> eventRoIs = event->getVec<TrigMonRoi>();
      if (m_doRoI && eventRoIs.size() > 0) {
        *m_roiN = eventRoIs.size();
        m_roiIsNoneType     ->resize( eventRoIs.size() );
        m_roiIsMuonType     ->resize( eventRoIs.size() );
        m_roiIsEmTauType    ->resize( eventRoIs.size() );
        m_roiIsTau          ->resize( eventRoIs.size() );
        m_roiIsJetType      ->resize( eventRoIs.size() );
        m_roiIsJetEtType    ->resize( eventRoIs.size() );
        m_roiIsEnergyType   ->resize( eventRoIs.size() );
        m_roiId             ->resize( eventRoIs.size() );
        m_roiNL1thresholds  ->resize( eventRoIs.size() );
        m_roiEta            ->resize( eventRoIs.size() );
        m_roiPhi            ->resize( eventRoIs.size() );
        m_roiArea           ->resize( eventRoIs.size() );
        m_roiEt             ->resize( eventRoIs.size() );
        m_roiEtLarge        ->resize( eventRoIs.size() );
        m_roiMuCharge       ->resize( eventRoIs.size() );
        m_roiIsolationBits  ->resize( eventRoIs.size() );
        m_roiVectorEX       ->resize( eventRoIs.size() );
        m_roiVectorEY       ->resize( eventRoIs.size() );
        m_roiOverflowEX     ->resize( eventRoIs.size() );
        m_roiOverflowEY     ->resize( eventRoIs.size() );
        m_roiOverflowET     ->resize( eventRoIs.size() );
        for(unsigned int i = 0; i < eventRoIs.size(); ++i) {
          m_roiIsNoneType   ->at(i) = (uint8_t) (eventRoIs.at(i).getRoiType() == TrigMonRoi::kNone);
          m_roiIsMuonType   ->at(i) = (uint8_t) (eventRoIs.at(i).getRoiType() == TrigMonRoi::kMuon);
          m_roiIsEmTauType  ->at(i) = (uint8_t) (eventRoIs.at(i).getRoiType() == TrigMonRoi::kEmTau);
          m_roiIsTau        ->at(i) = (uint8_t) (eventRoIs.at(i).getVarVal(Trig::kRoIIsTau)); // Resolve ambiguity
          m_roiIsJetType    ->at(i) = (uint8_t) (eventRoIs.at(i).getRoiType() == TrigMonRoi::kJet);
          m_roiIsJetEtType  ->at(i) = (uint8_t) (eventRoIs.at(i).getRoiType() == TrigMonRoi::kJetEt);
          m_roiIsEnergyType ->at(i) = (uint8_t) (eventRoIs.at(i).getRoiType() == TrigMonRoi::kEnergy);
          m_roiId           ->at(i) = eventRoIs.at(i).getRoiId();
          m_roiNL1thresholds->at(i) = eventRoIs.at(i).getNL1th();
          m_roiEta          ->at(i) = eventRoIs.at(i).getEta();
          m_roiPhi          ->at(i) = eventRoIs.at(i).getPhi();
          m_roiArea         ->at(i) = eventRoIs.at(i).getRoIArea();
          m_roiEt           ->at(i) = eventRoIs.at(i).getVarVal(Trig::kRoIET); //EMTAU=ET, muon=pT, jet=ETSmall, energy=scaler total energy
          m_roiEtLarge      ->at(i) = eventRoIs.at(i).getVarVal(Trig::kRoIETLarge); //Only for jets
          m_roiMuCharge     ->at(i) = (uint8_t) eventRoIs.at(i).getVarVal(Trig::kRoIMuonCharge); //Only for muons
          m_roiIsolationBits->at(i) = (uint32_t) eventRoIs.at(i).getVarVal(Trig::kRoIIsolationBits); //Only for EMTAU
          m_roiVectorEX     ->at(i) = eventRoIs.at(i).getVarVal(Trig::kRoIEnergyVectorX); //Only for ENERGY
          m_roiVectorEY     ->at(i) = eventRoIs.at(i).getVarVal(Trig::kRoIEnergyVectorY); //Only for ENERGY
          m_roiOverflowEX   ->at(i) = (uint8_t) eventRoIs.at(i).getVarVal(Trig::kRoIEnergyOverflowX); //Only for ENERGY
          m_roiOverflowEY   ->at(i) = (uint8_t) eventRoIs.at(i).getVarVal(Trig::kRoIEnergyOverflowY); //Only for ENERGY
          m_roiOverflowET   ->at(i) = (uint8_t) eventRoIs.at(i).getVarVal(Trig::kRoIEnergyOverflowT); //Only for ENERGY
        }
      }

      ////////////////
      //SEQ/ALG DATA//
      ////////////////
      const std::vector<TrigMonSeq> eventSeqs = event->getVec<TrigMonSeq>();
      if (m_doSeq && eventSeqs.size() > 0) {
        int _RoIIndex = 0;
        *m_seqN = eventSeqs.size();
        
        m_seqLevel               ->resize( eventSeqs.size() );
        m_seqIsInitial           ->resize( eventSeqs.size() );
        m_seqIsExecuted          ->resize( eventSeqs.size() );
        m_seqIsAlreadyExecuted   ->resize( eventSeqs.size() );
        m_seqIsPrevious          ->resize( eventSeqs.size() );
        m_seqChannelCounter      ->resize( eventSeqs.size() ); 
        m_seqIndex               ->resize( eventSeqs.size() );
        m_seqSequenceTimer       ->resize( eventSeqs.size() ); 
        m_seqAlgTimerTot         ->resize( eventSeqs.size() );
        m_seqAlgN                ->resize( eventSeqs.size() ); 
        m_seqAlgPosition         ->resize( eventSeqs.size() );
        m_seqAlgNRoIs            ->resize( eventSeqs.size() );
        m_seqAlgRoIIndex         ->resize( eventSeqs.size() );
        // We do not resize m_seqRoI as we don't yet know how big it will be
        m_seqAlgIsCached         ->resize( eventSeqs.size() );
        m_seqAlgIsCalled         ->resize( eventSeqs.size() );
        m_seqAlgTimer            ->resize( eventSeqs.size() );
        m_seqAlgTimeStartSec     ->resize( eventSeqs.size() );
        m_seqAlgTimeStartMicroSec->resize( eventSeqs.size() );
        m_seqAlgTimeStopSec      ->resize( eventSeqs.size() );
        m_seqAlgTimeStopMicroSec ->resize( eventSeqs.size() ); 
        
        for(unsigned int i = 0; i < eventSeqs.size(); ++i) {
          m_seqLevel            ->at(i) = eventSeqs.at(i).getLevel();
          m_seqIsInitial        ->at(i) = (uint8_t) eventSeqs.at(i).isInitial();
          m_seqIsExecuted       ->at(i) = (uint8_t) eventSeqs.at(i).isExecuted();
          m_seqIsAlreadyExecuted->at(i) = (uint8_t) eventSeqs.at(i).isAlreadyExecuted();
          m_seqIsPrevious       ->at(i) = (uint8_t) eventSeqs.at(i).isPrevious();
          m_seqChannelCounter   ->at(i) = eventSeqs.at(i).getChnCounter();
          m_seqIndex            ->at(i) = eventSeqs.at(i).getSeqIndex();
          m_seqSequenceTimer    ->at(i) = eventSeqs.at(i).getSeqTimer();
          m_seqAlgTimerTot      ->at(i) = eventSeqs.at(i).getAlgTimer();
          //Loop over sequence algorithms
          const std::vector<TrigMonAlg> seqAlgs = eventSeqs.at(i).getAlg();
          m_seqAlgN->at(i) = seqAlgs.size();
          (m_seqAlgPosition         ->at(i)).resize( seqAlgs.size() );
          (m_seqAlgNRoIs            ->at(i)).resize( seqAlgs.size() );
          (m_seqAlgRoIIndex         ->at(i)).resize( seqAlgs.size() );
          (m_seqAlgIsCached         ->at(i)).resize( seqAlgs.size() );
          (m_seqAlgIsCalled         ->at(i)).resize( seqAlgs.size() );
          (m_seqAlgTimer            ->at(i)).resize( seqAlgs.size() );
          (m_seqAlgTimeStartSec     ->at(i)).resize( seqAlgs.size() );
          (m_seqAlgTimeStartMicroSec->at(i)).resize( seqAlgs.size() );
          (m_seqAlgTimeStopSec      ->at(i)).resize( seqAlgs.size() );
          (m_seqAlgTimeStopMicroSec ->at(i)).resize( seqAlgs.size() );
          for (unsigned int j = 0; j < seqAlgs.size(); ++j) {
            (m_seqAlgPosition   ->at(i)).at(j) = seqAlgs.at(j).getPosition();
            // We shall not go deeper than vector<vector< bla > >, so store this vector<uint8_t> "up one level"
            (m_seqAlgNRoIs            ->at(i)).at(j) = seqAlgs.at(j).getNRoi();
            (m_seqAlgRoIIndex         ->at(i)).at(j) = _RoIIndex++;
            // We need to go via a vector, but this class gives us a set. Need to convert
            std::vector< uint8_t > _RoIIdSet;
            _RoIIdSet.clear();
            _RoIIdSet.resize( seqAlgs.at(j).getNRoi() );
            for (unsigned int _roi = 0; _roi < seqAlgs.at(j).getNRoi(); ++_roi) {
              _RoIIdSet.at(_roi) = seqAlgs.at(j).getRoiId( _roi );
            }
            m_seqRoI                  ->push_back( _RoIIdSet );
            (m_seqAlgIsCached         ->at(i)).at(j) = (uint8_t) seqAlgs.at(j).isCached();
            (m_seqAlgIsCalled         ->at(i)).at(j) = (uint8_t) seqAlgs.at(j).isCalled();
            (m_seqAlgTimer            ->at(i)).at(j) = seqAlgs.at(j).getTimer();
            (m_seqAlgTimeStartSec     ->at(i)).at(j) = seqAlgs.at(j).start().getSec();
            (m_seqAlgTimeStartMicroSec->at(i)).at(j) = seqAlgs.at(j).start().getMicroSec();
            (m_seqAlgTimeStopSec      ->at(i)).at(j) = seqAlgs.at(j).stop().getSec();
            (m_seqAlgTimeStopMicroSec ->at(i)).at(j) = seqAlgs.at(j).stop().getMicroSec();
          }

          // Check for additonal payload 
          if ( eventSeqs.at(i).getVarKey().size() != eventSeqs.at(i).getVarVal().size() ) {
            REPORT_MESSAGE( MSG::WARNING ) << "Trigger sequences additional float payload size different to key size, skipping.";
          } else {
            for (unsigned j=0; j < eventSeqs.at(i).getVarKey().size(); ++j) {
              REPORT_MESSAGE( MSG::DEBUG ) << "Got a SEQ["<<i<<"] int=>float pair. " << eventSeqs.at(i).getVarKey().at(j) << " => " << eventSeqs.at(i).getVarVal().at(j);
            }
          }  
        }
      }

      ///////////
      //TE DATA//
      ///////////
      const std::vector<TrigMonTE> eventTEs = event->getVec<TrigMonTE>();
      if (m_doTE && eventTEs.size() > 0) {  
        *m_teN = eventTEs.size();
        m_teId             ->resize( eventTEs.size() );
        m_teIndex          ->resize( eventTEs.size() );
        m_teIsRegularTE    ->resize( eventTEs.size() );
        m_teIsInitialTE    ->resize( eventTEs.size() );
        m_teIsRoITE        ->resize( eventTEs.size() );
        m_teIsL1ThreshTE   ->resize( eventTEs.size() );
        m_teActiveState    ->resize( eventTEs.size() );
        m_teErrorState     ->resize( eventTEs.size() );
        m_teIsTerminalNode ->resize( eventTEs.size() );
        m_teIsOutputL2Node ->resize( eventTEs.size() );
        m_teIsOutputEFNode ->resize( eventTEs.size() );
        m_teIsTopologicalTE->resize( eventTEs.size() );
        m_teChildIndex     ->resize( eventTEs.size() ); 
        m_teParentIndex    ->resize( eventTEs.size() );
        m_teRoiId          ->resize( eventTEs.size() );
        m_teClid           ->resize( eventTEs.size() ); 
        for(unsigned int i = 0; i < eventTEs.size(); ++i) {
          m_teId             ->at(i) = eventTEs.at(i).getId();
          m_teIndex          ->at(i) = eventTEs.at(i).getIndex();
          m_teIsRegularTE    ->at(i) = (uint8_t) (eventTEs.at(i).getType() == TrigMonTE::kELEM);
          m_teIsInitialTE    ->at(i) = (uint8_t) (eventTEs.at(i).getType() == TrigMonTE::kINIT);
          m_teIsRoITE        ->at(i) = (uint8_t) (eventTEs.at(i).getType() == TrigMonTE::kROI);
          m_teIsL1ThreshTE   ->at(i) = (uint8_t) (eventTEs.at(i).getType() == TrigMonTE::kL1TH);
          m_teActiveState    ->at(i) = (uint8_t) eventTEs.at(i).getActiveState();
          m_teErrorState     ->at(i) = (uint8_t) eventTEs.at(i).getErrorState();
          m_teIsTerminalNode ->at(i) = (uint8_t) eventTEs.at(i).isTerminalNode();
          m_teIsOutputL2Node ->at(i) = (uint8_t) eventTEs.at(i).isOutputL2Node();
          m_teIsOutputEFNode ->at(i) = (uint8_t) eventTEs.at(i).isOutputEFNode();
          m_teIsTopologicalTE->at(i) = (uint8_t) eventTEs.at(i).isTopologicalTE();
          //Get inner vectors direct from the TE
          m_teChildIndex ->at(i) = eventTEs.at(i).getChildIndex();
          m_teParentIndex->at(i) = eventTEs.at(i).getParentIndex();
          m_teRoiId      ->at(i) = eventTEs.at(i).getRoiId();
          m_teClid       ->at(i) = eventTEs.at(i).getClid();

          // Check additonal payload 
          if ( eventTEs.at(i).getVarKey().size() != eventTEs.at(i).getVarVal().size() ) {
            REPORT_MESSAGE( MSG::WARNING ) << "Trigger TE additional float payload size different to key size, skipping.";
          } else {
            for (unsigned j=0; j < eventTEs.at(i).getVarKey().size(); ++j) {
              REPORT_MESSAGE( MSG::DEBUG ) << "Got a TE["<<i<<"] int=>float pair. " << eventTEs.at(i).getVarKey().at(j) << " => " << eventTEs.at(i).getVarVal().at(j);
            }
          }  
        }
      }

      ///////////
      //DB DATA//
      ///////////
      if (m_doDBKey) {
        for (unsigned i=0; i < event->getVarKey().size(); ++i) {
          switch (event->getVarKey().at(i)) {
            case Trig::kSMK:
              *m_DB_SMK = event->getVarVal().at(i);
              break;
            case Trig::kL1PSK:
              *m_DB_L1PSK = event->getVarVal().at(i);
              break;
            case Trig::kHLTPSK:
              *m_DB_HLTPSK = event->getVarVal().at(i);
              break;
            default:
              break;
          } // Switch
        }
      } 

      if( m_tree->capture().isFailure() ) {
        ATH_MSG_ERROR( "Couldn't save the TrigMonEvent." );
        return;
      } else {
        ATH_MSG_DEBUG( "Trigger monitoring event from TrigMonEvent source successfully saved." );
      }

    } // Loop over TrigMonEventCollection DataVector
  } // TrigCostD3PDMakerTool::handleNewEvent

  void TrigCostD3PDMakerTool::clear() {
    if (m_doBasic) {
      *m_event = 0;
      *m_lumi = 0;
      *m_lumiLength = 0;
      *m_bunch_id = 0;
      *m_run = 0;
      *m_sec = 0;
      *m_nsec = 0;
      *m_timer = 0;
      *m_ranScaleTools= 0;
      *m_appId = 0;
      *m_costEvent = 0;
      *m_costRunSec = 0;
      *m_costRunNsec = 0;
    }

    if (m_doEB && !m_doBasic) {
      *m_event = 0;
      *m_ebWeight = 0;
      *m_ebWeightBG = 0;
      *m_ebUnbiased = 0;
    }

    //////////////
    //EXTRA DATA//
    //////////////
    if (m_doExtra) {
      *m_timerTrigCost = 0;
      *m_timerEndSteer = 0;
      *m_timerChainProcess = 0;
      *m_timerResultBuilder = 0;
      *m_timerMon = 0;
    }

    //////////////
    //CHAIN DATA//
    //////////////
    if (m_doChain) {
      *m_chainN = 0;
      m_chainCounter->clear();
      m_chainLevel->clear();
      m_chainTimer->clear();
      m_chainIsPassed->clear();
      m_chainIsPassedRaw->clear();
      m_chainIsPassthrough->clear();
      m_chainIsResurrected->clear();
      m_chainIsPrescaled->clear();
      m_chainWasL1AfterVeto->clear();
      m_chainWasL1BeforePrescale->clear();
      m_chainWasL1AfterPrescale->clear();
      m_chainIsExpressStream->clear();
    }

    ///////////
    //L1 DATA//
    ///////////
    if (m_doL1) {
      *m_l1N = 0;
      m_l1CtpId ->clear();
      m_l1PassedBeforePrescale ->clear();
      m_l1PassedAfterPrescale->clear();
      m_l1PassedAfterVeto->clear();
      m_l1Passed->clear();
      m_l1Prescaled->clear();
      m_l1Vetoed->clear();
    }

    ////////////
    //ROS DATA//
    ////////////
    if (m_doROS) {
      *m_robN = 0;
      m_robRequestorId->clear();
      m_robTimer->clear();
      m_robTimeStartSec->clear();
      m_robTimeStartMicroSec->clear();
      m_robTimeStopSec->clear();
      m_robTimeStopMicroSec->clear();
      m_robDataN->clear();
      m_robDataRobId->clear();
      m_robDataRobSize->clear();
      m_robDataIsUnclassified->clear();
      m_robDataIsCached->clear();
      m_robDataIsRetrieved->clear();
      m_robDataIsIgnored->clear();
      m_robDataIsDisabled->clear();
      m_robDataIsStatusOk->clear();
      m_robDataIsPrefetched->clear();
    }

    ////////////////
    //ROS SUM DATA//
    ////////////////
    if (m_doROSSum) {
      m_robSumDataN->clear();
      m_robSumDataSize->clear();
      m_robSumDataNRob->clear();
      m_robSumDataSubDet->clear();
      m_robSumDataIsUnclassified->clear();
      m_robSumDataIsCached->clear();
      m_robSumDataIsRetrieved->clear();
      m_robSumDataIsIgnored->clear();
      m_robSumDataIsDisabled->clear();
    }


    ////////////
    //RoI DATA//
    ////////////
    if (m_doRoI) {
      *m_roiN = 0;
      m_roiIsNoneType->clear();
      m_roiIsMuonType->clear();
      m_roiIsEmTauType->clear();
      m_roiIsTau->clear();
      m_roiIsJetType->clear();
      m_roiIsJetEtType->clear();
      m_roiIsEnergyType->clear();
      m_roiId->clear();
      m_roiNL1thresholds->clear();
      m_roiEta->clear();
      m_roiPhi->clear();
      m_roiArea->clear();
      //
      m_roiEt->clear();
      m_roiEtLarge->clear();
      m_roiMuCharge->clear();
      m_roiIsolationBits->clear();
      m_roiVectorEX->clear();
      m_roiVectorEY->clear();
      m_roiOverflowEX->clear();
      m_roiOverflowEY->clear();
      m_roiOverflowET->clear();
    }

    ////////////////
    //SEQ/ALG DATA//
    ////////////////
    if (m_doSeq) {
      *m_seqN = 0;
      m_seqIsInitial->clear();
      m_seqIsExecuted->clear();
      m_seqIsAlreadyExecuted->clear();
      m_seqIsPrevious->clear();
      m_seqLevel->clear();
      m_seqChannelCounter->clear();
      m_seqIndex->clear();
      m_seqSequenceTimer->clear();
      m_seqAlgTimerTot->clear();
      m_seqAlgN->clear();
      m_seqAlgPosition->clear();
      m_seqAlgNRoIs->clear();
      m_seqAlgRoIIndex->clear();
      m_seqRoI->clear();
      m_seqAlgIsCached->clear();
      m_seqAlgIsCalled->clear();
      m_seqAlgTimer->clear();
      m_seqAlgTimeStartSec->clear();
      m_seqAlgTimeStartMicroSec->clear();
      m_seqAlgTimeStopSec->clear();
      m_seqAlgTimeStopMicroSec->clear();
    }

    ///////////
    //TE DATA//
    ///////////
    if (m_doTE) {
      *m_teN = 0;
      m_teId->clear();
      m_teIndex->clear();
      m_teIsRegularTE->clear();
      m_teIsInitialTE->clear();
      m_teIsRoITE->clear();
      m_teIsL1ThreshTE->clear();
      m_teActiveState->clear();
      m_teErrorState->clear();
      m_teIsTerminalNode->clear();
      m_teIsOutputL2Node->clear();
      m_teIsOutputEFNode->clear();
      m_teIsTopologicalTE->clear();
      m_teChildIndex->clear();
      m_teParentIndex->clear();
      m_teRoiId->clear();
      m_teClid->clear(); 
    }

    ///////////
    //DB DATA//
    ///////////
    if (m_doDBKey) {
      *m_DB_SMK = 0;
      *m_DB_L1PSK = 0;
      *m_DB_HLTPSK = 0;
    }
  }

} // namespace D3PD
