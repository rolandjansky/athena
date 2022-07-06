/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "BSMonitoringAlg.h"

TrigT1CTMonitoring::BSMonitoringAlgorithm::BSMonitoringAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator){}

StatusCode TrigT1CTMonitoring::BSMonitoringAlgorithm::initialize() {

  ATH_MSG_INFO("Initializing " << name());
  ATH_MSG_DEBUG("CTPmonitoring BSMonitorAlgorith::initialize");
  ATH_MSG_DEBUG("Package Name "<< m_packageName);
  // connect to RPC and TGC RoI tools
  //is this NOT used at all??? check the old code about these tools! - then remove or reactivate
  /*if ( m_processMuctpi ) {
    ATH_CHECK( m_rpcRoiTool.retrieve() );
    ATH_CHECK( m_tgcRoiTool.retrieve() );
    }*/
  
  ATH_CHECK( m_MuCTPI_RDOKey.initialize(m_processMuctpi) );
  //ATH_CHECK( m_MuCTPI_RIOKey.initialize(m_processMuctpi && m_processMuctpiRIO && ! m_runOnESD) );
  ATH_CHECK( m_CTP_RDOKey.initialize(m_processCTP) );
  ATH_CHECK( m_CTP_RIOKey.initialize(m_processCTP && ! m_runOnESD) );
  ATH_CHECK( m_CTP_RDO_RerunKey.initialize(m_processCTP && m_compareRerun) );
  ATH_CHECK( m_RoIBResultKey.initialize(m_processRoIB && m_processMuctpiRIO) );
  ATH_CHECK( m_RPCContainerKey.initialize(m_processMuctpi) );
  ATH_CHECK( m_TGCContainerKey.initialize(m_processMuctpi) );
  ATH_CHECK( m_EventInfoKey.initialize() );

  //COOL access
  ATH_CHECK( m_LBLBFolderInputKey.initialize(!m_isSim) );
  ATH_CHECK( m_FILLSTATEFolderInputKey.initialize(!m_isSim) );
  ATH_CHECK( m_DataTakingModeFolderInputKey.initialize(!m_isSim) );

  ATH_MSG_INFO("Printing the BSMonitoringAlgorithm Configuration: ");
  ATH_MSG_INFO("InclusiveTriggerThresholds: " << m_inclusiveTriggerThresholds);
  ATH_MSG_INFO("ProcessMuctpiData: " << m_processMuctpi);
  ATH_MSG_INFO("ProcessMuctpiDataRIO: " << m_processMuctpiRIO);
  ATH_MSG_INFO("RunOnESD: " << m_runOnESD);
  ATH_MSG_INFO("CompareRerun: " << m_compareRerun);

  ATH_MSG_INFO("ProcessCTPData: " << m_processCTP);
  ATH_MSG_INFO("ProcessRoIBResult: " << m_processRoIB);
  if (!m_isSim) ATH_MSG_INFO("Simulation or Data?: DATA");
  else ATH_MSG_INFO("Simulation or Data?: SIMULATION");

  return AthMonitorAlgorithm::initialize();
}

StatusCode TrigT1CTMonitoring::BSMonitoringAlgorithm::fillHistograms( const EventContext& ctx ) const {
  using namespace Monitored;
  ATH_MSG_DEBUG("CTPmonitoring BSMonitorAlgorithm::fillHistograms");
  try {
    ATH_MSG_DEBUG( "begin fillHistograms()");

    // Now see what exists in  StoreGate...
    const MuCTPI_RDO* theMuCTPI_RDO = 0;
    //const MuCTPI_RIO* theMuCTPI_RIO = 0;
    const CTP_RDO* theCTP_RDO = 0;
    const CTP_RIO* theCTP_RIO = 0;
    const ROIB::RoIBResult* roIBResult = 0;
    const Muon::TgcCoinDataContainer* theTGCContainer = 0;
    const RpcSectorLogicContainer* theRPCContainer = 0;

    //bool validMuCTPI_RIO = true;
    bool validMuCTPI_RDO = true;
    bool validCTP_RIO = true;
    bool validCTP_RDO = true;
    bool validRoIBResult = true;
    bool validTGCContainer = true;
    bool validRPCContainer = true;
    int numberOfInvalidFragments = 0;

    //ERROR histos
    auto errorSummaryX = Monitored::Scalar<int>("errorSummaryX",0);
    auto errorSummaryY = Monitored::Scalar<int>("errorSummaryY",0);
    auto errorSummaryPerLumiBlockX = Monitored::Scalar<int>("errorSummaryPerLumiBlockX",0);
    auto errorSummaryPerLumiBlockY = Monitored::Scalar<int>("errorSummaryPerLumiBlockY",0);
    auto errorPerLumiBlockX = Monitored::Scalar<int>("errorPerLumiBlockX",0);
    auto incompleteFragmentTypeX = Monitored::Scalar<int>("incompleteFragmentTypeX",0);
    auto incompleteFragmentTypeY = Monitored::Scalar<int>("incompleteFragmentTypeY",0);

    auto eventInfo = GetEventInfo(ctx);

    if (m_processMuctpi) {
      ATH_MSG_DEBUG( "CTPMON fillHistograms() m_processMuctpi");
      theMuCTPI_RDO = SG::get(m_MuCTPI_RDOKey, ctx);
      if (!theMuCTPI_RDO) {
	ATH_MSG_WARNING( "Could not find \"" << m_MuCTPI_RDOKey.key() << "\" in StoreGate");
	validMuCTPI_RDO = false;
	++numberOfInvalidFragments;
      }
      // now try to get RPC and TGC SL output for comparisons
      theRPCContainer = SG::get(m_RPCContainerKey, ctx);
      if (!theRPCContainer) {
	ATH_MSG_WARNING( "Could not find RPC container in StoreGate");
	validRPCContainer = false;
      }
      theTGCContainer = SG::get(m_TGCContainerKey, ctx);
      if (!theTGCContainer) {
	ATH_MSG_WARNING( "Could not find TGC container in StoreGate");
	validTGCContainer = false;
      }
    }
    
    if (m_processCTP) {
      ATH_MSG_DEBUG( "CTPMON fillHistograms() m_processCTP");
      theCTP_RDO = SG::get(m_CTP_RDOKey, ctx);
      //ATH_MSG_INFO( "CTPMON theCTP_RDO->isValid()" << theCTP_RDO->isValid());
      if (!theCTP_RDO) {
	ATH_MSG_WARNING( "Could not find \"" << m_CTP_RDOKey.key() << "\" in StoreGate");
	validCTP_RDO = false;
	++numberOfInvalidFragments;
      }
      if (!m_runOnESD) {
	theCTP_RIO = SG::get(m_CTP_RIOKey, ctx);
	if (!theCTP_RIO) {
	  ATH_MSG_WARNING( "Could not find \"" << m_CTP_RIOKey.key() << "\" in StoreGate");
	  validCTP_RIO = false;
	  ++numberOfInvalidFragments;
	}
        ATH_MSG_DEBUG( "validCTP_RIO: " << validCTP_RIO );
      }
    }
    if (m_processRoIB && m_processMuctpiRIO) {
      ATH_MSG_DEBUG( "CTPMON fillHistograms() m_processRoIB && m_processMuctpiRIO");
      roIBResult = SG::get(m_RoIBResultKey, ctx);
      if (!roIBResult) {
	ATH_MSG_WARNING( "Could not find \"" << m_RoIBResultKey.key() << "\" in StoreGate");
	validRoIBResult = false;
	++numberOfInvalidFragments;
      }
    }

    bool incompleteEvent = false;
    int runNumber = 0;
    unsigned int eventNumber = 0;
    uint32_t currentLumiBlock = 0;

    if (eventInfo->runNumber()) {
      currentLumiBlock = eventInfo->lumiBlock();
      runNumber = eventInfo->runNumber();
      eventNumber = eventInfo->eventNumber();
      //lumiBlockOfPreviousEvent = currentLumiBlock;
      //currentLumiBlock =  eventInfo->lumiBlock();
      incompleteEvent = eventInfo->eventFlags(xAOD::EventInfo::Core) & 0x40000;
      ATH_MSG_DEBUG( "Successfully retrieved EventInfo (run: " << runNumber << ", event: " << eventNumber << ")");
    }
    /*
    else {
      ATH_MSG_WARNING( "Could not retrieve EventInfo from StoreGate => run# = event# = 0, LB# = 99");
      lumiBlockOfPreviousEvent = currentLumiBlock;
      currentLumiBlock = 99; // dummy LB in case EventInfo is not available - prevents DQ defect flagging with LB# 0...
    }
    */
    if ( incompleteEvent ) {
      ATH_MSG_WARNING( "Incomplete event according to EventInfo flag");
      incompleteFragmentTypeX = 4;
      fill(m_packageName, incompleteFragmentTypeX);
    }

    //bool l1ctObjectMissingInStoreGate = ( !validCTP_RDO || !validCTP_RIO || !validMuCTPI_RDO || !validMuCTPI_RIO || !validRoIBResult );
    bool l1ctObjectMissingInStoreGate = ( !validCTP_RDO || !validCTP_RIO || !validMuCTPI_RDO || !validRoIBResult );
    if ( l1ctObjectMissingInStoreGate ) {
      ATH_MSG_WARNING( "At least one L1CT object is missing in SG");
    }

    //dumpData(theCTP_RDO, theCTP_RIO, theMuCTPI_RDO, theMuCTPI_RIO, roIBResult, ctx);
    dumpData(theCTP_RDO, /*theCTP_RIO,*/ theMuCTPI_RDO, roIBResult, ctx);
    if ( m_processCTP ) {
      if ( validCTP_RDO ) {
	const std::vector<uint32_t> &cDataWords = theCTP_RDO->getDataWords();
	if ( cDataWords.size() == 0 ) {
	  ATH_MSG_WARNING( "CTP_RDO is empty, ignoring CTP");
	  validCTP_RDO = false;
	}
      }

      if ( validCTP_RIO ) {
	if ( !m_runOnESD && (theCTP_RIO->getDetectorEventType() & 0xffff) == 0 ) {//LB == 0 only if RIO is empty
	  ATH_MSG_WARNING( "CTP_RIO is not valid, ignoring CTP");
	  validCTP_RIO = false;
	}
      }
    }

    if ( m_processMuctpi ) {
      if ( validMuCTPI_RDO ) {
	MuCTPI_MultiplicityWord_Decoder multWord(theMuCTPI_RDO->candidateMultiplicity(), m_inclusiveTriggerThresholds);
	// consider the fragment incomplete if the number of data words is less than
	// the reported number of candidates (zero words valid for muon-less events!)
	if (theMuCTPI_RDO->dataWord().size() < multWord.getNCandidates()) {
	  //patrick ATH_MSG_INFO
	  ATH_MSG_DEBUG("MuCTPI_RDO reports " << multWord.getNCandidates()
		       //the following gives 0 for now - but why???
		       << "  candidates, but there are only " << theMuCTPI_RDO->dataWord().size()
		       << " data words, ignoring MuCTPI");
	  validMuCTPI_RDO = false;
	}
      }
      // Note: there's no simple way of checking the validity of the MUCTPI_RIO, so we don't for now.
    }

    // if at least one fragment is missing/incomplete, print out a summary
    // if (!validCTP_RDO || !validCTP_RIO || !validMuCTPI_RDO || !validMuCTPI_RIO || !validRoIBResult) {
    if (!validCTP_RDO || !validCTP_RIO || !validMuCTPI_RDO || !validRoIBResult) {
      ATH_MSG_WARNING( "At least one missing/invalid L1CT fragment detected");
      ATH_MSG_WARNING( "CTP_RDO: " << validCTP_RDO << ", CTP_RIO: " << validCTP_RIO
		       //<< ", MuCTPI_RIO: " << validMuCTPI_RIO << ", MuCTPI_RDO: " << validMuCTPI_RDO
		       << ", RoIBResult: " << validRoIBResult);
      //ATH_MSG_INFO( "Run number: " << eventInfo->runNumber() << ", Event: " << eventInfo->eventNumber() << ", LB: " << eventInfo->lumiBlock() );
      if (validCTP_RIO) {
	ATH_MSG_WARNING( "CTP_RIO says LB: " << (theCTP_RIO->getDetectorEventType() & 0xffff)
                         << ", L1ID: " << std::dec << theCTP_RIO->getLvl1Id()
                         << " (HEX: " << std::hex << theCTP_RIO->getLvl1Id() << ")" << std::dec
                         << ", BCID: " << theCTP_RIO->getBCID());
	ATH_MSG_WARNING( "CTP_RIO says LB: " << (theCTP_RIO->getDetectorEventType() & 0xffff));
	ATH_MSG_WARNING( "CTP_RIO says L1ID: " << std::dec << theCTP_RIO->getLvl1Id());
	ATH_MSG_WARNING( "CTP_RIO says HEX: " << std::hex << theCTP_RIO->getLvl1Id() << ")" << std::dec);
	ATH_MSG_WARNING( "CTP_RIO says BCID: " << theCTP_RIO->getBCID());
	ATH_MSG_WARNING("in: validCTP_RIO - survived this? crashing now?");
      }
      else if (eventInfo->runNumber()) {
	ATH_MSG_WARNING( "CTP_RIO missing, EventInfo says LB: " << eventInfo->lumiBlock() 
			 << ", BCID: " << eventInfo->bcid()); 
      }
      else {
	ATH_MSG_WARNING( "Not printing event details since both CTP_RIO and EventInfo objects are missing");
      }

      // only fill error-per-LB histograms if L1CT fragments are missing and global incomplete-event flag
      // from EventInfo does not say that the event is incomplete
      if ( !incompleteEvent ) {
	errorSummaryPerLumiBlockX = currentLumiBlock;
	errorSummaryPerLumiBlockY = 15;
	fill(m_packageName, errorSummaryPerLumiBlockX, errorSummaryPerLumiBlockY);
	errorPerLumiBlockX = currentLumiBlock;
	fill(m_packageName, errorPerLumiBlockX);
      }
      errorSummaryX = 15;
      errorSummaryY = 1;
      fill(m_packageName, errorSummaryX, errorSummaryY);

      if (!validCTP_RIO) {
	incompleteFragmentTypeX = 0;
	fill(m_packageName, incompleteFragmentTypeX);
      }
      if (!validCTP_RDO) {
	incompleteFragmentTypeX = 1;
	fill(m_packageName, incompleteFragmentTypeX);
      }
      /*
      if (!validMuCTPI_RIO) {
        incompleteFragmentTypeX = 2;
	fill(m_packageName, incompleteFragmentTypeX);
	}*/
      if (!validMuCTPI_RDO) {
        incompleteFragmentTypeX = 2;
	fill(m_packageName, incompleteFragmentTypeX);
      }
      if (!validRoIBResult) {
        incompleteFragmentTypeX = 3;
	fill(m_packageName, incompleteFragmentTypeX);
      }
      if (!validTGCContainer) {
        incompleteFragmentTypeX = 4;
	fill(m_packageName, incompleteFragmentTypeX);
      }
      if (!validRPCContainer) {
        incompleteFragmentTypeX = 5;
	fill(m_packageName, incompleteFragmentTypeX);
      }
    }
    else { // since errorSummary holds error _rate_, also fill when there are no errors
      errorSummaryX = 15;
      fill(m_packageName, errorSummaryX);
    }

    // if the event is incomplete (missing L1CT objects or according to EventInfo), skip filling the rest of the histograms
    //if ( !validCTP_RDO || !validCTP_RIO || !validMuCTPI_RDO || !validMuCTPI_RIO || !validRoIBResult || incompleteEvent ) {
    if ( !validCTP_RDO || !validCTP_RIO || !validMuCTPI_RDO || !validRoIBResult || incompleteEvent ) {
      ATH_MSG_WARNING( "Event incomplete, will skip filling of all non-error histograms");
      //comment patrick: why was this here if later the single validities are checked?
      //bc of validCTP_RIO
      //return StatusCode::SUCCESS;
    }

    /*
     * Process and fill data
     */
    if (m_processCTP && validCTP_RDO && validCTP_RIO) {
      ATH_MSG_DEBUG( "CTPMON before begin doCtp()");
      doCtp(theCTP_RDO, theCTP_RIO, ctx);
    }

    //if (m_processMuctpi && m_processCTP && validCTP_RDO && validCTP_RIO && validMuCTPI_RDO && validMuCTPI_RIO) {
    if (m_processMuctpi && m_processCTP && validCTP_RDO && validMuCTPI_RDO ) {
      ATH_MSG_DEBUG( "CTPMON before begin doCtpMuctpi()");
      //doCtpMuctpi(theCTP_RDO, theCTP_RIO, theMuCTPI_RDO, theMuCTPI_RIO, ctx);
      doCtpMuctpi(theCTP_RDO, theMuCTPI_RDO, ctx);
    }

    //if (m_processMuctpi && validMuCTPI_RDO && validMuCTPI_RIO && validTGCContainer && validRPCContainer) {
    if (m_processMuctpi && validMuCTPI_RDO && validTGCContainer && validRPCContainer) {
      ATH_MSG_DEBUG( "CTPMON before begin doMuctpi()");
      //doMuctpi(theMuCTPI_RDO, theMuCTPI_RIO, theRPCContainer, theTGCContainer, ctx);
      doMuctpi(theMuCTPI_RDO, theRPCContainer, theTGCContainer, ctx);
      if (m_processRoIB && m_processMuctpiRIO) {
	ATH_MSG_DEBUG( "CTPMON before begin doMuonRoI()");
	//doMuonRoI(theMuCTPI_RDO, theMuCTPI_RIO, roIBResult, ctx);
	doMuonRoI(theMuCTPI_RDO, roIBResult, ctx);
      }
    }
    //++m_eventCount;
    ATH_MSG_DEBUG( "end fillHistograms()");
    return StatusCode::SUCCESS;
  }
  catch(const std::exception & e) {
    std::cerr << "Caught standard C++ exception: " << e.what() << " from fillHistograms()" << std::endl;
    return StatusCode::FAILURE;
  }
  }


void
TrigT1CTMonitoring::BSMonitoringAlgorithm::doMuctpi(const MuCTPI_RDO* theMuCTPI_RDO, //const MuCTPI_RIO* theMuCTPI_RIO,
						    const RpcSectorLogicContainer* theRPCContainer,
						    const Muon::TgcCoinDataContainer* theTGCContainer,
						    const EventContext& ctx) const
{
  ATH_MSG_DEBUG( "CTPMON begin doMuctpi()");
  //ERROR histos
  auto errorSummaryX = Monitored::Scalar<int>("errorSummaryX",0);
  auto errorSummaryY = Monitored::Scalar<int>("errorSummaryY",0);
  auto errorSummaryPerLumiBlockX = Monitored::Scalar<int>("errorSummaryPerLumiBlockX",0);
  auto errorSummaryPerLumiBlockY = Monitored::Scalar<int>("errorSummaryPerLumiBlockY",0);
  auto errorPerLumiBlockX = Monitored::Scalar<int>("errorPerLumiBlockX",0);
  //MUCTPI
  auto nCandidatesX = Monitored::Scalar<int>("nCandidatesX",0);
  auto nCandidatesDataWordX = Monitored::Scalar<int>("nCandidatesDataWordX",0);
  auto ptX = Monitored::Scalar<int>("ptX",0);
  auto nCandidatesPtX = Monitored::Scalar<int>("nCandidatesPtX",0);
  auto nCandidatesPtY = Monitored::Scalar<int>("nCandidatesPtY",0);
  auto nCandidatesDataWordPtX = Monitored::Scalar<int>("nCandidatesDataWordPtX",0);
  auto nCandidatesDataWordPtY = Monitored::Scalar<int>("nCandidatesDataWordPtY",0);
  auto nCandidatesMictpMioctX = Monitored::Scalar<int>("nCandidatesMictpMioctX",0);
  auto bcidMictpMioctX = Monitored::Scalar<int>("bcidMictpMioctX",0);
  //auto bcidMictpHeaderX = Monitored::Scalar<int>("bcidMictpHeaderX",0);
  //auto muctpiStatus1X = Monitored::Scalar<int>("muctpiStatus1X",0);
  //auto muctpiStatus2X = Monitored::Scalar<int>("muctpiStatus2X",0);
  auto barrelRoiSectorIDX = Monitored::Scalar<int>("barrelRoiSectorIDX",0);
  auto barrelRoiSectorIDY = Monitored::Scalar<int>("barrelRoiSectorIDY",0);
  auto endcapRoiSectorIDX = Monitored::Scalar<int>("endcapRoiSectorIDX",0);
  auto endcapRoiSectorIDY = Monitored::Scalar<int>("endcapRoiSectorIDY",0);
  auto forwardRoiSectorIDX = Monitored::Scalar<int>("forwardRoiSectorIDX",0);
  auto forwardRoiSectorIDY = Monitored::Scalar<int>("forwardRoiSectorIDY",0);
  auto barrelRoiSectorIDAllX = Monitored::Scalar<int>("barrelRoiSectorIDAllX",0);
  auto barrelRoiSectorIDAllY = Monitored::Scalar<int>("barrelRoiSectorIDAllY",0);
  auto endcapRoiSectorIDAllX = Monitored::Scalar<int>("endcapRoiSectorIDAllX",0);
  auto endcapRoiSectorIDAllY = Monitored::Scalar<int>("endcapRoiSectorIDAllY",0);
  auto forwardRoiSectorIDAllX = Monitored::Scalar<int>("forwardRoiSectorIDAllX",0);
  auto forwardRoiSectorIDAllY = Monitored::Scalar<int>("forwardRoiSectorIDAllY",0);
  auto barrelNCandSectorIDX = Monitored::Scalar<int>("barrelNCandSectorIDX",0);
  auto barrelNCandSectorIDY = Monitored::Scalar<int>("barrelNCandSectorIDY",0);
  auto endcapNCandSectorIDX = Monitored::Scalar<int>("endcapNCandSectorIDX",0);
  auto endcapNCandSectorIDY = Monitored::Scalar<int>("endcapNCandSectorIDY",0);
  auto forwardNCandSectorIDX = Monitored::Scalar<int>("forwardNCandSectorIDX",0);
  auto forwardNCandSectorIDY = Monitored::Scalar<int>("forwardNCandSectorIDY",0);
  auto barrelSectorIDOverlapBitsX = Monitored::Scalar<int>("barrelSectorIDOverlapBitsX",0);
  auto barrelSectorIDOverlapBitsY = Monitored::Scalar<int>("barrelSectorIDOverlapBitsY",0);
  auto endcapSectorIDOverlapBitX = Monitored::Scalar<int>("endcapSectorIDOverlapBitX",0);
  auto endcapSectorIDOverlapBitY = Monitored::Scalar<int>("endcapSectorIDOverlapBitY",0);
  auto barrelSectorIDX = Monitored::Scalar<int>("barrelSectorIDX",0);
  auto barrelSectorIDY = Monitored::Scalar<int>("barrelSectorIDY",0);
  auto endcapSectorIDX = Monitored::Scalar<int>("endcapSectorIDX",0);
  auto endcapSectorIDY = Monitored::Scalar<int>("endcapSectorIDY",0);
  auto forwardSectorIDX = Monitored::Scalar<int>("forwardSectorIDX",0);
  auto forwardSectorIDY = Monitored::Scalar<int>("forwardSectorIDY",0);
  auto barrelSectorIDAllX = Monitored::Scalar<int>("barrelSectorIDAllX",0);
  auto endcapSectorIDAllX = Monitored::Scalar<int>("endcapSectorIDAllX",0);
  auto forwardSectorIDAllX = Monitored::Scalar<int>("forwardSectorIDAllX",0);
  auto barrelPtX = Monitored::Scalar<int>("barrelPtX",0);
  auto endcapPtX = Monitored::Scalar<int>("endcapPtX",0);
  auto forwardPtX = Monitored::Scalar<int>("forwardPtX",0);

  auto muctpiNoRPCCandfoundX = Monitored::Scalar<int>("muctpiNoRPCCandfoundX",0);
  auto muctpiNoRPCCandfoundY = Monitored::Scalar<int>("muctpiNoRPCCandfoundY",0);
  auto rpcNoMuCTPICandfoundX = Monitored::Scalar<int>("rpcNoMuCTPICandfoundX",0);
  auto rpcNoMuCTPICandfoundY = Monitored::Scalar<int>("rpcNoMuCTPICandfoundY",0);
  auto muctpiNoTGCecCandfoundX = Monitored::Scalar<int>("muctpiNoTGCecCandfoundX",0);
  auto muctpiNoTGCecCandfoundY = Monitored::Scalar<int>("muctpiNoTGCecCandfoundY",0);
  auto tgcecNoMuCTPICandfoundX = Monitored::Scalar<int>("tgcecNoMuCTPICandfoundX",0);
  auto tgcecNoMuCTPICandfoundY = Monitored::Scalar<int>("tgcecNoMuCTPICandfoundY",0);
  auto muctpiNoTGCfwCandfoundX = Monitored::Scalar<int>("muctpiNoTGCfwCandfoundX",0);
  auto muctpiNoTGCfwCandfoundY = Monitored::Scalar<int>("muctpiNoTGCfwCandfoundY",0);
  auto tgcfwNoMuCTPICandfoundX = Monitored::Scalar<int>("tgcfwNoMuCTPICandfoundX",0);
  auto tgcfwNoMuCTPICandfoundY = Monitored::Scalar<int>("tgcfwNoMuCTPICandfoundY",0);

  // maps for comapring MuCTPI and RPC/TGC candidates
  std::map <std::string,MuCTPI_DataWord_Decoder> muctpiCandidates;
  std::map <std::string, const RpcSLTriggerHit* > rpcCandidates;
  std::map <std::string, const Muon::TgcCoinData* > tgcCandidates;


  // Get the data
  MuCTPI_MultiplicityWord_Decoder multWord(theMuCTPI_RDO->candidateMultiplicity(), m_inclusiveTriggerThresholds);
  MuCTPI_DataWord_Decoder dataWord(0);
  const std::vector<uint32_t> &vDataWords = theMuCTPI_RDO->dataWord();
  auto eventInfo = GetEventInfo(ctx);

  /*
   * Number of muon candidates
   */
  uint16_t numberCandidates = multWord.getNCandidates();
  //if (numberCandidates > 0) ++m_filledEventCount;
  nCandidatesX = numberCandidates;
  fill(m_packageName, nCandidatesX);

  for ( unsigned int i(0); i < LVL1::MuCTPIBits::MULT_THRESH_NUM; ++i ) {
    nCandidatesPtX = i+1u;
    nCandidatesPtY = multWord.getMultiplicity(i);
    fill(m_packageName, nCandidatesPtX, nCandidatesPtY);
  }
  /*
   * Consistency check: number of MICTP candidates vs. no of MIOCT
   * candidate words
   */
  int numberDataWords = 0;
  int ncand[3][96] = {}; //count number of candidates per event (0 barrel, 1 endcap, 2 forward)
  /*  for ( int x = 0; x < 3; x++ ) {
    for ( int y = 0; y < 96; y++ ) {
      ncand[x][y]=0;
    }
    }*/
 
 // because of overlap removal (and saturation) the number of data words may be different
  // from the reported candidate multiplicity => also check data words!
  // bookkeeping array to count per pT threshold
  int nCandsPerPtThreshold[6] = {0, 0, 0, 0, 0, 0};

  for ( auto it = vDataWords.begin(); it != vDataWords.end(); ++it ) {
    dataWord.setWord(*it);
    std::ostringstream keystring;

    //count all candidates for sector multiplicities
    if (dataWord.getSectorLocation() == MuCTPI_RDO::BARREL) {
      ++ncand[0][dataWord.getSectorID(1)+32*dataWord.getHemisphere()];
      keystring << "BA" << dataWord.getSectorID(1)+32*dataWord.getHemisphere() <<"-RoI" << dataWord.getRoiNumber()
                << "-Pt" << dataWord.getPt();
    }
    else if (dataWord.getSectorLocation() == MuCTPI_RDO::ENDCAP) {
      ++ncand[1][dataWord.getSectorID()+48*dataWord.getHemisphere()];
      keystring << "EC" << dataWord.getSectorID()+48*dataWord.getHemisphere() <<"-RoI" << dataWord.getRoiNumber()
                << "-Pt" << dataWord.getPt();
    }
    else if (dataWord.getSectorLocation() == MuCTPI_RDO::FORWARD) {
      ++ncand[2][dataWord.getSectorID()+24*dataWord.getHemisphere()];
      keystring << "FW" << dataWord.getSectorID()+24*dataWord.getHemisphere() <<"-RoI" << dataWord.getRoiNumber()
                << "-Pt" << dataWord.getPt();
    }  
  }

  for ( int y = 0; y < 96; y++ ) {
    if ( y < 64 ) {
      barrelNCandSectorIDX = y;
      barrelNCandSectorIDY = ncand[0][y];
      fill(m_packageName, barrelNCandSectorIDX, barrelNCandSectorIDY);
    }
    endcapNCandSectorIDX = y;
    endcapNCandSectorIDY = ncand[1][y];
    fill(m_packageName, endcapNCandSectorIDX, endcapNCandSectorIDY);
    if ( y < 48 ){
      forwardNCandSectorIDX = y;
      forwardNCandSectorIDY = ncand[2][y];
      fill(m_packageName, forwardNCandSectorIDX, forwardNCandSectorIDY);
    }
  }

  // multiplicities based on the data words
  nCandidatesDataWordX = numberDataWords;
  fill(m_packageName, nCandidatesDataWordX);

  for (int i = 0; i < 6; ++i) {
    nCandidatesDataWordPtX = i+1;
    nCandidatesDataWordPtY = nCandsPerPtThreshold[i];
    fill(m_packageName, nCandidatesDataWordPtX, nCandidatesDataWordPtY);
  }

  if (m_inclusiveTriggerThresholds && (numberDataWords > 7)) numberDataWords = 7;

  int diffNCandidates = (static_cast<int>(numberCandidates) - numberDataWords);
  uint32_t currentLumiBlock = eventInfo->lumiBlock();

  if (diffNCandidates != 0) {
    //patrick ATH_MSG_WARNING
    ATH_MSG_DEBUG( "Number of candidates in multiplicity word " << numberCandidates
                     << " != number of candidates " << numberDataWords << " from " << vDataWords.size() << " data words");
    // dump multiplicity and data words (DEBUG level)
    multWord.dumpData();
    for ( std::vector<uint32_t>::const_iterator it = vDataWords.begin(); it != vDataWords.end(); ++it ) {
      dataWord.setWord(*it);
      dataWord.dumpData();
    }
    errorSummaryX = 4;
    errorSummaryY = 1;
    fill(m_packageName, errorSummaryX, errorSummaryY);
    errorSummaryPerLumiBlockX = currentLumiBlock;
    errorSummaryPerLumiBlockY = 4;
    fill(m_packageName, errorSummaryPerLumiBlockX, errorSummaryPerLumiBlockY);
    errorPerLumiBlockX = currentLumiBlock;
    fill(m_packageName, errorPerLumiBlockX);
  }
  else {
    errorSummaryX = 4;
    errorSummaryY = 0;
    fill(m_packageName, errorSummaryX, errorSummaryY);
  }
  nCandidatesMictpMioctX = diffNCandidates;
  fill(m_packageName, nCandidatesMictpMioctX);

  /*
   * BCIDs
   */
  //uint16_t mictpBcid = multWord.getBCID();
  //uint16_t candidateBcid = 0;
  //uint16_t headerBcid =
  //  (theMuCTPI_RIO) ? theMuCTPI_RIO->getHeaderBCID() : 0;
  //bcidMictpHeaderX = mictpBcid - (headerBcid & 7);
  //fill(m_packageName, bcidMictpHeaderX);

  /*
   * pT thresholds of the MIOCT candidate words
   */
  std::vector<Int_t> MioctPtCount(LVL1::MuCTPIBits::MULT_THRESH_NUM, 0);
  std::vector<uint32_t>::const_iterator it = vDataWords.begin();
  std::vector<uint32_t>::const_iterator end = vDataWords.end();

  for ( ; it != end; ++it ) {
    dataWord.setWord(*it);
    //dataWord.dumpData();
    //Fill sector occupancy for all candidates
    if (dataWord.getSectorLocation() == MuCTPI_RDO::BARREL) {
      barrelSectorIDAllX = dataWord.getSectorID(1)+32*dataWord.getHemisphere();
      fill(m_packageName, barrelSectorIDAllX);
      barrelRoiSectorIDAllX = dataWord.getSectorID(1)+32*dataWord.getHemisphere();
      barrelRoiSectorIDAllY = dataWord.getRoiNumber();
      fill(m_packageName, barrelRoiSectorIDAllX, barrelRoiSectorIDAllY);
    }
    else if (dataWord.getSectorLocation() == MuCTPI_RDO::ENDCAP) {
      endcapSectorIDAllX = dataWord.getSectorID()+48*dataWord.getHemisphere();
      fill(m_packageName, barrelSectorIDAllX);
      endcapRoiSectorIDAllX = dataWord.getSectorID()+48*dataWord.getHemisphere();
      endcapRoiSectorIDAllY = dataWord.getRoiNumber();
      fill(m_packageName, endcapRoiSectorIDAllX, endcapRoiSectorIDAllY);
    }
    else if (dataWord.getSectorLocation() == MuCTPI_RDO::FORWARD) {
      forwardSectorIDAllX = dataWord.getSectorID()+24*dataWord.getHemisphere();
      fill(m_packageName, forwardSectorIDAllX);
      forwardRoiSectorIDAllX = dataWord.getSectorID()+24*dataWord.getHemisphere();
      forwardRoiSectorIDAllY = dataWord.getRoiNumber();
      fill(m_packageName, forwardRoiSectorIDAllX, forwardRoiSectorIDAllY);
    }

    // Use only non-vetoed candidates from the same BCID for multiplicity calculation
    if ( ! dataWord.getVetoed() ) {
      uint16_t candPt = dataWord.getPt();
      ptX = candPt;
      fill(m_packageName, ptX);
      if (0 < candPt && candPt <= LVL1::MuCTPIBits::MULT_THRESH_NUM) {
        if (m_inclusiveTriggerThresholds) {
          for ( uint16_t i(0); i < candPt; ++i )
            if (MioctPtCount[i] < 7) {
              MioctPtCount[i] += 1;
            }
        } else {
          if (MioctPtCount[candPt - 1] < 7) {
            MioctPtCount[candPt - 1] += 1;
          }
        }
      }
      if (dataWord.getSectorLocation() == MuCTPI_RDO::BARREL) {
	barrelSectorIDX = dataWord.getSectorID(1)+32*dataWord.getHemisphere();
	fill(m_packageName, barrelSectorIDX);
	barrelRoiSectorIDX = dataWord.getSectorID(1)+32*dataWord.getHemisphere();
	barrelRoiSectorIDY = dataWord.getRoiNumber();
	fill(m_packageName, barrelRoiSectorIDX, barrelRoiSectorIDY);
      }
      else if (dataWord.getSectorLocation() == MuCTPI_RDO::ENDCAP) {
	endcapSectorIDX = dataWord.getSectorID()+48*dataWord.getHemisphere();
	fill(m_packageName, endcapSectorIDX);
	endcapRoiSectorIDX = dataWord.getSectorID()+48*dataWord.getHemisphere();
	endcapRoiSectorIDY = dataWord.getRoiNumber();
	fill(m_packageName, endcapRoiSectorIDX, endcapRoiSectorIDY);
      }
      else if (dataWord.getSectorLocation() == MuCTPI_RDO::FORWARD) {
	forwardSectorIDX = dataWord.getSectorID()+24*dataWord.getHemisphere();
	fill(m_packageName, forwardSectorIDX);
	forwardRoiSectorIDX = dataWord.getSectorID()+24*dataWord.getHemisphere();
	forwardRoiSectorIDY = dataWord.getRoiNumber();
	fill(m_packageName, forwardRoiSectorIDX, forwardRoiSectorIDY);
      }
    }
    if (dataWord.getSectorLocation() == MuCTPI_RDO::BARREL) {
      barrelPtX = dataWord.getPt();
      fill(m_packageName, barrelPtX);
    }
    else if (dataWord.getSectorLocation() == MuCTPI_RDO::ENDCAP) {
      endcapPtX = dataWord.getPt();
      fill(m_packageName, endcapPtX);
    }
    else if (dataWord.getSectorLocation() == MuCTPI_RDO::FORWARD) {
      forwardPtX = dataWord.getPt();
      fill(m_packageName, forwardPtX);
    }
  }

  /*
   * Consistency check: MICTP vs MIOCT multiplicity
   */
  bool anyMismatch = false;
  for ( uint16_t i = 0; i < LVL1::MuCTPIBits::MULT_THRESH_NUM; ++i ) {
    if ( multWord.getMultiplicity(i) != MioctPtCount[i] ) {
      anyMismatch = true;
      //patrick ATH_MSG_WARNING
      ATH_MSG_DEBUG( "pT threshold " << i+1 << ": MICTP multiplicity (" << multWord.getMultiplicity(i)
		       << ") not equal MIOCT multiplicity (" << MioctPtCount[i] << ")");
      errorSummaryX = 5;
      errorSummaryY = 1;
      fill(m_packageName, errorSummaryX, errorSummaryY);
      errorSummaryPerLumiBlockX = currentLumiBlock;
      errorSummaryPerLumiBlockY = 5;
      fill(m_packageName, errorSummaryPerLumiBlockX, errorSummaryPerLumiBlockY);
      errorPerLumiBlockX = currentLumiBlock;
      fill(m_packageName, errorPerLumiBlockX);
    }
  }
  if ( !anyMismatch ) {
    errorSummaryX = 5;
    errorSummaryY = 0;
    fill(m_packageName, errorSummaryX, errorSummaryY);
  }
  // dump multiplicity and data words (DEBUG level)
  multWord.dumpData();
  for ( std::vector<uint32_t>::const_iterator it = vDataWords.begin(); it != vDataWords.end(); ++it ) {
    dataWord.setWord(*it);
    dataWord.dumpData();
  }

  /*
   * Check the error status words of the ROD Header
   */
  /*
  if (theMuCTPI_RIO) {
    uint32_t num = theMuCTPI_RIO->getHeaderNumberStatusWords();
    std::vector<uint32_t> vStatus = theMuCTPI_RIO->getHeaderStatusWords();
    for ( uint32_t i = 0; i < num; ++i ) {
      if (vStatus[i] == 0) continue;
      int Status = -1;
      if (i == 0) {
        ATH_MSG_DEBUG( "MuCTPI error status word #" << i << ": 0x" << MSG::hex << vStatus[i] << MSG::dec);
        Status = 1;
      } else if (i == 1) {
        Status = 2;
      } else {
        continue;
      }
      for ( int bit = 0; bit < 24; ++bit ) {
        if (vStatus[i] & (1 << bit)) {
	  if (Status == 1) {
            muctpiStatus1X = bit;
            fill(m_packageName, muctpiStatus1X);
	  }
	  else if (Status == 2) {
	    muctpiStatus2X = bit;
	    fill(m_packageName, muctpiStatus2X);
	  }
	}
      }
    }
  }
  */
  // Get candidates from TGC and RPC SLs for comparisons
  RpcSectorLogicContainer::const_iterator it_rpc = theRPCContainer->begin();
  for ( ; it_rpc !=theRPCContainer->end() ; ++it_rpc )
    {
      // Loop over the trigger hits of each sector
      RpcSectorLogic::const_iterator it_rpc_hit = (*it_rpc) -> begin();
      for ( ; it_rpc_hit != (*it_rpc) -> end() ; ++it_rpc_hit )
        {
          if (!(*it_rpc_hit) -> isInput() && (*it_rpc_hit) -> rowinBcid() == 1) {
	    std::ostringstream rpckey;
            rpckey << "BA" << (*it_rpc)->sectorId() <<"-RoI" << (*it_rpc_hit) -> roi()
                   << "-Pt" << (*it_rpc_hit) -> ptId();
            rpcCandidates.insert (std::pair<std::string, const RpcSLTriggerHit* >(rpckey.str(),(*it_rpc_hit)));
          }
        }
    }

  Muon::TgcCoinDataContainer::const_iterator it_tgc;
  Muon::TgcCoinDataCollection::const_iterator it_tgc_col;
  for (it_tgc = theTGCContainer->begin(); it_tgc != theTGCContainer->end(); it_tgc++) {
    for (it_tgc_col = (*it_tgc)->begin(); it_tgc_col != (*it_tgc)->end(); it_tgc_col++) {
      if ((*it_tgc_col)->pt() != 0 ) {
	std::ostringstream tgckey;
	std::string sys = "FW";
        if ((*it_tgc_col)->isForward()) {  // Forward sector, account for different numbering scheme in SL readout
          int secID = (*it_tgc_col)->phi();
          if (secID == 24) secID = 0;
          tgckey << sys << secID+(24*(*it_tgc_col)->isAside()) <<"-RoI" << (*it_tgc_col)->roi()
                 << "-Pt" << (*it_tgc_col)->pt();
        } else { // Endcap sector, account for different numbering scheme in SL readout
	  std::string sys = "EC";
          int secID =  (*it_tgc_col)->phi()+1;
          if (secID == 48 ) secID = 0;
          else if (secID == 49 ) secID = 1;
          tgckey << sys << secID + (48*(*it_tgc_col)->isAside()) <<"-RoI" << (*it_tgc_col)->roi()
                 << "-Pt" << (*it_tgc_col)->pt();
        }
        tgcCandidates.insert(std::pair<std::string, const Muon::TgcCoinData* >(tgckey.str(),(*it_tgc_col)));
      }
    }
  }
  // Now loop over MuCTPI/RPC/TGC maps and try to find matching keys, and plot candidates where no match is found
  // loop over MUCTPI candidates and try to find match in RPC/TGC maps

  bool miRPCmismatch = false;
  bool miTGCmismatch = false;
  for (std::map<std::string,MuCTPI_DataWord_Decoder>::const_iterator it_mui = muctpiCandidates.begin();
       it_mui != muctpiCandidates.end(); ++it_mui) {
    int tgcnum = tgcCandidates.count(it_mui->first);
    int rpcnum = rpcCandidates.count(it_mui->first);
    if (tgcnum > 0 || rpcnum > 0 ) {
      ATH_MSG_DEBUG( "MuCTPI to RPC/TGC match found: MuCTPI key/ MuCTPI BCID / #TGC matches / #RPC matches: "
                     << it_mui->first << " / " << /*it_mui->second.getBCID() <<*/ " / "
                     << tgcnum << " / " << rpcnum);
    } else {
      std::string det = (it_mui->first).substr(0,2);
      if ( det == "BA" )  {
        int baSecID = (it_mui->second).getSectorID(1)+32*(it_mui->second).getHemisphere();
        int baRoIID = (it_mui->second).getRoiNumber();
	muctpiNoRPCCandfoundX = baRoIID;
	muctpiNoRPCCandfoundY = baSecID;
	fill(m_packageName, muctpiNoRPCCandfoundX, muctpiNoRPCCandfoundY);
        miRPCmismatch = true;
      } else if ( det == "EC" ) {
        int ecSecID = (it_mui->second).getSectorID()+48*(it_mui->second).getHemisphere();
        int ecRoIID = (it_mui->second).getRoiNumber();
	muctpiNoTGCecCandfoundX = ecRoIID;
	muctpiNoTGCecCandfoundY = ecSecID;
	fill(m_packageName, muctpiNoTGCecCandfoundX, muctpiNoTGCecCandfoundY);
        miTGCmismatch= true;
      }else if ( det == "FW" ) {
        int fwSecID = (it_mui->second).getSectorID()+24*(it_mui->second).getHemisphere();
        int fwRoIID = (it_mui->second).getRoiNumber();
	muctpiNoTGCfwCandfoundX = fwRoIID;
	muctpiNoTGCfwCandfoundY = fwSecID;
	fill(m_packageName, muctpiNoTGCfwCandfoundX, muctpiNoTGCfwCandfoundY);
        miTGCmismatch= true;
      } else {
        ATH_MSG_WARNING( "Invalid string label in MuCTPI to RPC/TGC map: " << det);
      }
      ATH_MSG_WARNING( "No Muctpi to RPC/TGC match found: MuCTPI key / MuCTPI BCID: " << it_mui->first  << " / ");
    }
  }
  if (miRPCmismatch) {
    errorSummaryX = 17;
    errorSummaryY = 1;
    fill(m_packageName, errorSummaryX, errorSummaryY);
    errorSummaryPerLumiBlockX = currentLumiBlock;
    errorSummaryPerLumiBlockY = 17;
    fill(m_packageName, errorSummaryPerLumiBlockX, errorSummaryPerLumiBlockY);
    errorPerLumiBlockX = currentLumiBlock;
    fill(m_packageName, errorPerLumiBlockX);
  } else {
    errorSummaryX = 17;
    errorSummaryY = 0;
    fill(m_packageName, errorSummaryX, errorSummaryY);
  }
  if (miTGCmismatch) {
    errorSummaryX = 18;
    errorSummaryY = 1;
    fill(m_packageName, errorSummaryX, errorSummaryY);
    errorSummaryPerLumiBlockX = currentLumiBlock;
    errorSummaryPerLumiBlockY = 18;
    fill(m_packageName, errorSummaryPerLumiBlockX, errorSummaryPerLumiBlockY);
    errorPerLumiBlockX = currentLumiBlock;
    fill(m_packageName, errorPerLumiBlockX);
  } else {
    errorSummaryX = 18;
    errorSummaryY = 0;
    fill(m_packageName, errorSummaryX, errorSummaryY);
  }


  bool rpcMImismatch = false;
  // loop over RPC candidates and try to find match in Muctpi map
  for (std::map<std::string, const RpcSLTriggerHit*>::const_iterator it_rpc = rpcCandidates.begin();
       it_rpc != rpcCandidates.end(); ++it_rpc) {
    int muinum = muctpiCandidates.count(it_rpc->first);
    if (muinum > 0) {
      ATH_MSG_DEBUG( " RPC to Muctpi match found: RPC key / RPC BCID / # matches: "
                     << it_rpc->first << " / " <<  it_rpc->second->rowinBcid()  << " / "
                     << muinum);
    } else {
      int idEnd = (it_rpc->first).find("-RoI");
      int secID = std::stoi((it_rpc->first).substr(2,idEnd-2));
      int roiID = (it_rpc->second)->roi();
      rpcNoMuCTPICandfoundX = roiID;
      rpcNoMuCTPICandfoundY = secID;
      fill(m_packageName, rpcNoMuCTPICandfoundX, rpcNoMuCTPICandfoundY);
      //patrick ATH_MSG_WARNING
      ATH_MSG_DEBUG( "No RPC to Muctpi match found: RPC key / RPC BCID: "
                       << it_rpc->first << " / " <<  it_rpc->second->rowinBcid());
      rpcMImismatch =true;
    }
  }
  if (rpcMImismatch) {
    errorSummaryX = 19;
    errorSummaryY = 1;
    fill(m_packageName, errorSummaryX, errorSummaryY);
    errorSummaryPerLumiBlockX = currentLumiBlock;
    errorSummaryPerLumiBlockY = 19;
    fill(m_packageName, errorSummaryPerLumiBlockX, errorSummaryPerLumiBlockY);
    errorPerLumiBlockX = currentLumiBlock;
    fill(m_packageName, errorPerLumiBlockX);
  } else {
    errorSummaryX = 19;
    errorSummaryY = 0;
    fill(m_packageName, errorSummaryX, errorSummaryY);
  }
  bool tgcMImismatch = false;
  // loop over TGC candidates and try to find match in Muctpi map
  for (std::map<std::string, const Muon::TgcCoinData* >::const_iterator it_tgc = tgcCandidates.begin();
       it_tgc != tgcCandidates.end(); ++it_tgc) {
    int muinum = muctpiCandidates.count(it_tgc->first);
    if (muinum > 0) {
      ATH_MSG_DEBUG( "TGC to Muctpi match found: TGC key / TGC BCID / # matches: "
                     << it_tgc->first  << " / " << (int)TgcDigit::BC_CURRENT << muinum);
    } else {
      int idEnd = (it_tgc->first).find("-RoI");
      int secID = std::stoi((it_tgc->first).substr(2,idEnd-2));
      std::string det = (it_tgc->first).substr(0,2);
      if ( det == "EC" ) {
        int ecRoIID = (it_tgc->second)->roi();
	tgcecNoMuCTPICandfoundX = ecRoIID;
	tgcecNoMuCTPICandfoundY = secID;
	fill(m_packageName, tgcecNoMuCTPICandfoundX, tgcecNoMuCTPICandfoundY);
      }else if ( det == "FW" ) {
        int fwRoIID = (it_tgc->second)->roi();
	tgcfwNoMuCTPICandfoundX = fwRoIID;
	tgcfwNoMuCTPICandfoundY = secID;
	fill(m_packageName, tgcfwNoMuCTPICandfoundX, tgcfwNoMuCTPICandfoundY);
      } else {
        ATH_MSG_WARNING( "Invalid string label in TGC to MuCTPI map: "
                         << det );
      }
      ATH_MSG_WARNING( "No TGC to Muctpi match found: TGC key: " << it_tgc->first);
      tgcMImismatch =true;
    }
  }
  if (tgcMImismatch) {
    errorSummaryX = 20;
    errorSummaryY = 1;
    fill(m_packageName, errorSummaryX, errorSummaryY);
    errorSummaryPerLumiBlockX = currentLumiBlock;
    errorSummaryPerLumiBlockY = 20;
    fill(m_packageName, errorSummaryPerLumiBlockX, errorSummaryPerLumiBlockY);
    errorPerLumiBlockX = currentLumiBlock;
    fill(m_packageName, errorPerLumiBlockX);
  } else {
    errorSummaryX = 20;
    errorSummaryY = 0;
    fill(m_packageName, errorSummaryX, errorSummaryY);
  }
  }

void
TrigT1CTMonitoring::BSMonitoringAlgorithm::doCtp(const CTP_RDO* theCTP_RDO,
						 const CTP_RIO* theCTP_RIO,
						 const EventContext& ctx) const
{
  ATH_MSG_DEBUG( "CTPMON begin doCtp()");
  //ERROR histos
  auto errorSummaryX = Monitored::Scalar<int>("errorSummaryX",0);
  auto errorSummaryY = Monitored::Scalar<int>("errorSummaryY",0);
  auto errorSummaryPerLumiBlockX = Monitored::Scalar<int>("errorSummaryPerLumiBlockX",0);
  auto errorSummaryPerLumiBlockY = Monitored::Scalar<int>("errorSummaryPerLumiBlockY",0);
  auto errorPerLumiBlockX = Monitored::Scalar<int>("errorPerLumiBlockX",0);
  //CTP
  auto deltaBcidX = Monitored::Scalar<int>("deltaBcidX",0);
  auto triggerTypeX = Monitored::Scalar<int>("triggerTypeX",0);
  auto timeSinceLBStartX = Monitored::Scalar<int>("timeSinceLBStartX",0);
  auto timeUntilLBEndX = Monitored::Scalar<int>("timeUntilLBEndX",0);
  auto timeSinceL1AX = Monitored::Scalar<int>("timeSinceL1AX",0);
  auto turnCounterTimeErrorX = Monitored::Scalar<int>("turnCounterTimeErrorX",0);
  auto turnCounterTimeErrorVsLbX = Monitored::Scalar<int>("turnCounterTimeErrorVsLbX",0);
  auto turnCounterTimeErrorVsLbY = Monitored::Scalar<int>("turnCounterTimeErrorVsLbY",0);
  auto pitBCX = Monitored::Scalar<int>("pitBCX",0);
  auto pitBCY = Monitored::Scalar<int>("pitBCY",0);
  auto pitFirstBCX = Monitored::Scalar<int>("pitFirstBCX",0);
  auto pitFirstBCY = Monitored::Scalar<int>("pitFirstBCY",0);
  auto tavX = Monitored::Scalar<int>("tavX",0);
  auto tavY = Monitored::Scalar<int>("tavY",0);
  auto ctpStatus1X = Monitored::Scalar<int>("ctpStatus1X",0);
  auto ctpStatus2X = Monitored::Scalar<int>("ctpStatus2X",0);

  auto eventInfo = GetEventInfo(ctx);

  //get the cool data here:
  uint64_t lb_stime = 0;
  uint64_t lb_etime = 0;
  bool retrievedLumiBlockTimes = false;
  if (!m_isSim) {
    SG::ReadCondHandle<AthenaAttributeList> lblb(m_LBLBFolderInputKey, ctx);
    const AthenaAttributeList* lblbattrList{*lblb};
    if (lblbattrList==nullptr) {
      ATH_MSG_WARNING("Failed to retrieve /TRIGGER/LUMI/LBLB " << m_LBLBFolderInputKey.key() << " not found");
    }
    else {
      retrievedLumiBlockTimes = true;
      auto lb_stime_loc = (*lblbattrList)["StartTime"].data<cool::UInt63>();
      auto lb_etime_loc = (*lblbattrList)["EndTime"].data<cool::UInt63>();
      lb_stime = lb_stime_loc;
      lb_etime = lb_etime_loc;
      ATH_MSG_DEBUG("lb_stime: " << lb_stime << " lb_etime: " << lb_etime );
    }
  }

  //currentBeamMode
  std::string currentBeamMode = "sorry: not available!";
  if (!m_isSim){
    SG::ReadCondHandle<CondAttrListCollection> fillstate(m_FILLSTATEFolderInputKey,ctx);
    const CondAttrListCollection* fillstateattrListColl { *fillstate };
    if ( fillstateattrListColl == nullptr )    {
      ATH_MSG_WARNING("/LHC/DCS/FILLSTATE " << m_FILLSTATEFolderInputKey.key() << " not found");
    }
    else{
      CondAttrListCollection::const_iterator itrcoll;
      for (itrcoll = fillstateattrListColl->begin(); itrcoll != fillstateattrListColl->end(); ++itrcoll) {
	const coral::AttributeList &atr = itrcoll->second;
	currentBeamMode = *(static_cast<const std::string *>((atr["BeamMode"]).addressOfData()));
      }
    }
  }
  ATH_MSG_DEBUG("BeamMode: " << currentBeamMode  );
  
  //DataTakingMode
  int readyForPhysics = -1;
  if (!m_isSim){
    SG::ReadCondHandle<AthenaAttributeList> datatakingmode(m_DataTakingModeFolderInputKey, ctx);
    const AthenaAttributeList* datatakingmodeattrList{*datatakingmode};
    if (datatakingmodeattrList==nullptr) {
      ATH_MSG_WARNING( "Failed to retrieve /TDAQ/RunCtrl/DataTakingMode with key " << m_DataTakingModeFolderInputKey.key());
    }
    else{
      auto readyForPhysics_loc =  (*datatakingmodeattrList)["ReadyForPhysics"].data<uint32_t>();
      readyForPhysics = readyForPhysics_loc;
      ATH_MSG_DEBUG( "readyForPhysics: " << readyForPhysics);
    }
  }
  uint32_t evId = 0;
  uint32_t headerBcid = 0;
  int ttype=0;

  CTP_Decoder ctp;
  ctp.setRDO(theCTP_RDO);

  if (theCTP_RIO) {
    evId = theCTP_RIO->getLvl1Id();
    ttype = theCTP_RIO->getLvl1TriggerType();
    triggerTypeX = ttype;
    fill(m_packageName, triggerTypeX);

    headerBcid = (theCTP_RIO->getBCID() & 0xf);

    uint32_t currentLumiBlock = eventInfo->lumiBlock();
    // check that the LB number is the same in the EventInfo and the CTP_RIO. TODO: add error for this?
    if (currentLumiBlock != (theCTP_RIO->getDetectorEventType() & 0xffff)) {
      ATH_MSG_WARNING( "LB number in EventInfo (" << currentLumiBlock 
		       << ") does not match the one in the CTP_RIO object (" 
		       << (theCTP_RIO->getDetectorEventType() & 0xffff) << ")");
    }
    ATH_MSG_DEBUG( "Run number: " << eventInfo->runNumber() << ", Event: " << eventInfo->eventNumber() << ", LB: " << eventInfo->lumiBlock() ); 

    //if (currentLumiBlock > m_maxLumiBlock) m_maxLumiBlock = currentLumiBlock;
    if (currentLumiBlock < 1 ) //|| 
      //	(retrievedLumiBlockTimes && (find(m_lumiBlocks.begin(), m_lumiBlocks.end(), currentLumiBlock) == m_lumiBlocks.end()))) {
      //patrick check this
      {
      ATH_MSG_WARNING( "Invalid lumi block: " << currentLumiBlock);
      errorSummaryX = 9;
      errorSummaryY = 1;
      fill(m_packageName, errorSummaryX, errorSummaryY);
      errorSummaryPerLumiBlockX = currentLumiBlock;
      errorSummaryPerLumiBlockY = 9;
      fill(m_packageName, errorSummaryPerLumiBlockX, errorSummaryPerLumiBlockY);
      errorPerLumiBlockX = currentLumiBlock;
      fill(m_packageName, errorPerLumiBlockX);
    }
    else {
      errorSummaryX = 9;
      errorSummaryY = 0;
      fill(m_packageName, errorSummaryX, errorSummaryY);
      if (retrievedLumiBlockTimes) {
	uint64_t eventTime = static_cast<uint64_t>(theCTP_RDO->getTimeSec()*1e09 + theCTP_RDO->getTimeNanoSec());
	if (eventTime < lb_stime || eventTime > lb_etime) {
	  ATH_MSG_WARNING( "Event time (" << eventTime 
			   << ") not within time interval for current lumi block (LB: " << currentLumiBlock 
			   << ", start: " <<  lb_stime 
			   << ", stop: " << lb_etime << ")");
	  errorSummaryX = 10;
	  errorSummaryY = 1;
	  fill(m_packageName, errorSummaryX, errorSummaryY);
	  errorSummaryPerLumiBlockX = currentLumiBlock;
	  errorSummaryPerLumiBlockY = 10;
	  fill(m_packageName, errorSummaryPerLumiBlockX, errorSummaryPerLumiBlockY);
	  errorPerLumiBlockX = currentLumiBlock;
	  fill(m_packageName, errorPerLumiBlockX);
	}
	else {
          errorSummaryX = 10;
          errorSummaryY = 0;
          fill(m_packageName, errorSummaryX, errorSummaryY);
	  timeSinceLBStartX = (eventTime-lb_stime)/1e06;
          fill(m_packageName, timeSinceLBStartX);
	  timeUntilLBEndX = (lb_etime-eventTime)/1e06;
	  fill(m_packageName, timeUntilLBEndX);
	}

	// turn counter monitoring - first store turn counter, bcid and times for the first processed event

	// use the best available bunch-crossing interval
	double bcDurationInNs = m_defaultBcIntervalInNs;
	double freqFromCool = -1.0; //getFrequencyMeasurement(eventTime);
	if (freqFromCool > 40.078e6 && freqFromCool < 40.079e6) { // f prop. to beta, ok from HI injection to pp @ sqrt(s) = 14 TeV
	  // use average frequency since start of LB
	  // patrick
	  double lbStartFreqFromCool = -1.0; //getFrequencyMeasurement(lb_stime);
	  //double lbStartFreqFromCool = getFrequencyMeasurement(m_lbStartTimes[currentLumiBlock]);
	  if (lbStartFreqFromCool > 40.078e6 && lbStartFreqFromCool < 40.079e6) {
	    bcDurationInNs = 2./(freqFromCool+lbStartFreqFromCool)*1e9;
	  }
	  // or simply use the measurement closest to the event time
	  else {
	    bcDurationInNs = 1./freqFromCool*1e9;
	  }
	  ATH_MSG_DEBUG( "Will use BC interval calculated from frequency measurement(s) in COOL: f = " 
			 << freqFromCool << " Hz => t_BC = " << bcDurationInNs << " ns"); 
	}
	else {
	  //ATH_MSG_WARNING( "No valid frequency measurements found in COOL, will use hardcoded BC interval: t_BC = "
 	  ATH_MSG_INFO( "No valid frequency measurements found in COOL, will use hardcoded BC interval: t_BC = " 
			 << bcDurationInNs << " ns");
	}

	uint32_t lumiBlockOfPreviousEvent = eventInfo->lumiBlock() -1 ; 
	//patrick fix this!!! - first event! 
	uint64_t firstEventTime = 0;
	int64_t firstEventBcid = 0;
	int64_t firstEventTC = 0;
	// set the reference variables for the turn counter monitoring if this is the first processed event of the run/LB
	if ( (lumiBlockOfPreviousEvent != 0 && lumiBlockOfPreviousEvent != currentLumiBlock) ) {
	  //if ( !m_eventCount || (lumiBlockOfPreviousEvent != 0 && lumiBlockOfPreviousEvent != currentLumiBlock) ) {
	  firstEventTime = eventTime;
	  firstEventBcid = static_cast<int64_t>(theCTP_RIO->getBCID());
	  firstEventTC = static_cast<int64_t>(theCTP_RDO->getTurnCounter());
	}

	// calculate the time passed since the first processed event, based on GPS clock and turn counter+bcid
	int64_t timeDiff_GPS = eventTime - firstEventTime; 
	int64_t firstEventTimeInBc_TC = firstEventTC*m_bcsPerTurn+firstEventBcid;
	int64_t eventTimeInBc_TC = static_cast<int64_t>(theCTP_RDO->getTurnCounter())*m_bcsPerTurn+theCTP_RIO->getBCID();
	int64_t timeDiffInBc_TC = eventTimeInBc_TC-firstEventTimeInBc_TC;

	// fill turn counter monitoring plots if at least one of first and current turn-counter values are non-zero
	if ( !(firstEventTC == 0 && theCTP_RDO->getTurnCounter() == 0) ) {
	  std::string bm = currentBeamMode; 
	  double tDiffInNs = timeDiffInBc_TC*bcDurationInNs-timeDiff_GPS;
	  // flag an error if the offset for the timestamp calculated from TC+BCID is off by > half an LHC turn
	  // (if we're in STABLE BEAMS and did not just transition to ATLAS_READY in this LB)
	  if ( (bm == "STABLEBEAMS" || bm  == "STABLE BEAMS") && 
	       //patrick: do we need this condition?
	       //!((m_DataTakingModeFolderInputKey.find(currentLumiBlock) != m_DataTakingModeFolderInputKey.end()) &&
		 (readyForPhysics == 1) && 
	       (std::abs(tDiffInNs) > 45000) ) { 
	    ATH_MSG_WARNING( "Turn-counter based time off by " << tDiffInNs 
			     << " ns (> 0.5 LHC turn) during stable beams - missing orbit pulse?");
	    errorSummaryX = 16;
	    errorSummaryY = 1;
	    fill(m_packageName, errorSummaryX, errorSummaryY);
	    errorSummaryPerLumiBlockX = currentLumiBlock;
	    errorSummaryPerLumiBlockY = 16;
	    fill(m_packageName, errorSummaryPerLumiBlockX, errorSummaryPerLumiBlockY);
	    errorPerLumiBlockX = currentLumiBlock;
	    fill(m_packageName, errorPerLumiBlockX);
	  }
	  else {
	    errorSummaryX = 16;
            errorSummaryY = 0;
            fill(m_packageName, errorSummaryX, errorSummaryY);
	  }
	  turnCounterTimeErrorX = tDiffInNs/1e03;
	  fill(m_packageName, turnCounterTimeErrorX);
	  turnCounterTimeErrorVsLbX = currentLumiBlock;
	  turnCounterTimeErrorVsLbY = tDiffInNs/1e03;
	  fill(m_packageName, turnCounterTimeErrorVsLbX, turnCounterTimeErrorVsLbY);
	}
	else {
	  ATH_MSG_DEBUG( "Turn counter = 0 for both first processed and current event, not filling TC histograms");
	}
      }
    }
  }
  uint32_t currentLumiBlock = eventInfo->lumiBlock();
  if (theCTP_RDO->getTimeNanoSec() > 1e09) {
    ATH_MSG_WARNING( "Nanosecond timestamp too large: " << theCTP_RDO->getTimeNanoSec());
    errorSummaryX = 11;
    errorSummaryY = 1;
    fill(m_packageName, errorSummaryX, errorSummaryY);
    errorSummaryPerLumiBlockX = currentLumiBlock;
    errorSummaryPerLumiBlockY = 11;
    fill(m_packageName, errorSummaryPerLumiBlockX, errorSummaryPerLumiBlockY);
    errorPerLumiBlockX = currentLumiBlock;
    fill(m_packageName, errorPerLumiBlockX);
  } else {
    errorSummaryX = 11;
    errorSummaryY = 0;
    fill(m_packageName, errorSummaryX, errorSummaryY);
  }

  std::vector<uint32_t> vec=theCTP_RDO->getEXTRAWords();
  timeSinceL1AX = theCTP_RDO->getTimeSinceLastL1A()*25*10E-6;
  fill(m_packageName, timeSinceL1AX);

  uint32_t numberBC = theCTP_RDO->getNumberOfBunches();
  if (numberBC > 0) {
    unsigned int storeBunch = theCTP_RDO->getL1AcceptBunchPosition();
    const std::vector<CTP_BC> &BCs = ctp.getBunchCrossings();
    const CTP_BC & bunch = BCs[storeBunch];
    unsigned int bcid = bunch.getBCID();

    double bcid_offset = (static_cast < int >((bcid)&0xf) - static_cast < int >(headerBcid));
    deltaBcidX = bcid_offset;
    fill(m_packageName, deltaBcidX);
    if (bcid_offset != 0) {
      if (!m_runOnESD) {
	ATH_MSG_WARNING( "Found BCID offset of "<< bcid_offset << " between ROD Header (" 
			 << headerBcid << ") and data (" << (bcid&0xf) << ")");
	errorSummaryX = 1;
	errorSummaryY = 1;
	fill(m_packageName, errorSummaryX, errorSummaryY);
	errorSummaryPerLumiBlockX = currentLumiBlock;
	errorSummaryPerLumiBlockY = 1;
	fill(m_packageName, errorSummaryPerLumiBlockX, errorSummaryPerLumiBlockY);
	errorPerLumiBlockX = currentLumiBlock;
	fill(m_packageName, errorPerLumiBlockX);
      }
    }
    else {
      errorSummaryX = 1;
      errorSummaryY = 0;
      fill(m_packageName, errorSummaryX, errorSummaryY);
    }
    /*
     * TIP,TBP,TAP,TAV 
     */
    short bunchIndex = -storeBunch;
    std::bitset<512> TIPfirst;
    std::bitset<512> TBPfirst;
    std::bitset<512> TAPfirst;
    TIPfirst.set();
    TBPfirst.set();
    TAPfirst.set();

    //Vectors of TBP/TAP/TAB
    std::vector<int> tbpItems;
    std::vector<int> tapItems;
    std::vector<int> tavItems;
    std::vector<int> tbpBC;
    std::vector<int> tapBC;
    std::vector<int> tavBC;
    //int minbc=bunchIndex;
    for ( std::vector<CTP_BC>::const_iterator it = BCs.begin();
	  it != BCs.end(); ++it, ++bunchIndex ) {
      bcid = it->getBCID();

      if ( (!bunchIndex) && (m_compareRerun) ) {//gives position of L1A
        StatusCode sc = compareRerun(*it, ctx);
        if ( sc.isFailure() ) {
          ATH_MSG_WARNING( "compareRerun() returned failure");
        }
      }

      const std::bitset<512> currentTIP(it->getTIP());
      if (currentTIP.any()) {
	for ( size_t tipNum = 0; tipNum < currentTIP.size(); ++tipNum ) {
	  if (currentTIP.test(tipNum)) {
	    pitBCX = tipNum;
	    pitBCY = bunchIndex;
	    fill(m_packageName, pitBCX, pitBCY);
	    if (TIPfirst.test(tipNum)) {
	      TIPfirst.set(tipNum,0);
	      pitFirstBCX = tipNum;
	      pitFirstBCY = bunchIndex;
	      fill(m_packageName, pitFirstBCX, pitFirstBCY);
	    }
	  }
	}
      }

      const std::bitset<512> currentTBP(it->getTBP());
      if (currentTBP.any()) {
	for ( size_t item = 0; item < currentTBP.size(); ++item ) {
	  if (currentTBP.test(item)) {
	    tbpItems.push_back(item);
	    tbpBC.push_back(bunchIndex);
	    if (TBPfirst.test(item)) {
	      TBPfirst.set(item,0);
	    }
	  }
	}
      }
      
      const std::bitset<512> currentTAP(it->getTAP());
      if (currentTAP.any()) {
	for ( size_t item = 0; item < currentTAP.size(); ++item ) {
	  if (currentTAP.test(item)) {
	    tapItems.push_back(item);
	    tapBC.push_back(bunchIndex);
	    if (TAPfirst.test(item)) {
	      TAPfirst.set(item,0);
	    }
	  }
	}
      }

      const std::bitset<512> currentTAV(it->getTAV());

      if (currentTAV.any()) {
	for ( size_t item = 0; item < currentTAV.size(); ++item ) {
	  if (currentTAV.test(item)) {
	    tavX = item;
	    fill(m_packageName, tavX);
	    tavItems.push_back(item);
	    tavBC.push_back(bunchIndex);
	  }
	}
      }
    }
    //int maxbc=bunchIndex-1;

    bool allTAPFine=true;
    bool allTAVFine=true;
    for ( unsigned int i=0; i<tapItems.size(); i++ ) {
      ATH_MSG_DEBUG( tapItems.at(i) << " TAP fired at BC " << tapBC.at(i));
      bool isTBP=false;
      for ( unsigned int j=0; j<tbpItems.size() && isTBP==false; j++ ) {
	if ( tbpItems.at(j)==tapItems.at(i) && tbpBC.at(j)==tapBC.at(i) ) isTBP=true;
      }
      if ( isTBP==false ) {
	allTAPFine=false;
	ATH_MSG_WARNING( "TAP item " << tapItems.at(i) << " at BC " << tapBC.at(i) << " not found in TBP");
      }
    }
    for ( unsigned int i=0; i<tavItems.size(); i++ ) {
      ATH_MSG_DEBUG( tavItems.at(i) << " TAV fired at BC " << tavBC.at(i));
      bool isTAP=false;
      for ( unsigned int j=0; j<tapItems.size() && isTAP==false; j++ ) {
	if ( tapItems.at(j)==tavItems.at(i) && tapBC.at(j)==tavBC.at(i) ) isTAP=true;
      }
      if ( isTAP==false ) {
	allTAVFine=false;
	ATH_MSG_WARNING( "TAV item " << tavItems.at(i) << " at BC " << tavBC.at(i) << " not found in TAP");
      }
    }

    //Fill Error Hist
    if (allTAPFine==false) {
      errorSummaryX = 12;
      errorSummaryY = 1;
      fill(m_packageName, errorSummaryX, errorSummaryY);
      errorSummaryPerLumiBlockX = currentLumiBlock;
      errorSummaryPerLumiBlockY = 12;
      fill(m_packageName, errorSummaryPerLumiBlockX, errorSummaryPerLumiBlockY);
      errorPerLumiBlockX = currentLumiBlock;
      fill(m_packageName, errorPerLumiBlockX);
    }
    else {
      errorSummaryX = 12;
      errorSummaryY = 0;
      fill(m_packageName, errorSummaryX, errorSummaryY);
    }
    if (allTAVFine==false) {
      errorSummaryX = 13;
      errorSummaryY = 1;
      fill(m_packageName, errorSummaryX, errorSummaryY);
      errorSummaryPerLumiBlockX = currentLumiBlock;
      errorSummaryPerLumiBlockY = 13;
      fill(m_packageName, errorSummaryPerLumiBlockX, errorSummaryPerLumiBlockY);
      errorPerLumiBlockX = currentLumiBlock;
      fill(m_packageName, errorPerLumiBlockX);
    }
    else {
      errorSummaryX = 13;
      errorSummaryY = 0;
      fill(m_packageName, errorSummaryX, errorSummaryY);
    }
    //m_lastminbc=minbc;
    //m_lastmaxbc=maxbc;

    // TODO: is this really the check we want to do? Doesn't offline in general have more resources..? /CO
    //if (m_environment==AthenaMonManager::online)
    //  updateRangeUser();//Triggers LW->ROOT conversion so should certainly not be done offline

    errorSummaryX = 2;
    errorSummaryY = 0;
    fill(m_packageName, errorSummaryX, errorSummaryY);

    if (msgLvl(MSG::DEBUG)) {
    std::vector<unsigned int> triggersFired = ctp.getAllTriggers(storeBunch);
    std::stringstream str;
    //for ( unsigned int i = 0; i < triggersFired.size(); ++i ) {
    for (auto i : triggersFired ) { 
      str << i << " "; 
    }
    ATH_MSG_DEBUG( triggersFired.size() << " trigger items fired: " << str.str());
    }
  } 
  else {
    if (!m_runOnESD) {
      ATH_MSG_WARNING( "Zero bunches in CTP data for ext. LVL1 ID 0x" << MSG::hex << evId << MSG::dec);
      errorSummaryX = 2;
      errorSummaryY = 1;
      fill(m_packageName, errorSummaryX, errorSummaryY);
      errorSummaryPerLumiBlockX = currentLumiBlock;
      errorSummaryPerLumiBlockY = 2;
      fill(m_packageName, errorSummaryPerLumiBlockX, errorSummaryPerLumiBlockY);
      errorPerLumiBlockX = currentLumiBlock;
      fill(m_packageName, errorPerLumiBlockX);
    }
  }
  
  /*
   * Check the error status words of the ROD Header 
   */
  if (theCTP_RIO) {
    uint32_t num = theCTP_RIO->getNumberStatusWords();
    std::vector<uint32_t> vStatus = theCTP_RIO->getStatusWords();
    for ( uint32_t i = 0; i < num; ++i ) {
      if (vStatus[i] != 0) {
	int Status = -1;
	if (i == 0) {
	  ATH_MSG_DEBUG( "CTP error status word #" << i << ": 0x" << MSG::hex << vStatus[i] << MSG::dec);
	  Status = 1;
	} else if (i == 1) {
	  Status = 2;
	} else {
	  continue;
	}
	for ( int bit = 0; bit < 24; ++bit ) {
	  if (vStatus[i] & (1 << bit)){
	    if (Status == 1) {
	      ctpStatus1X = bit;
              fill(m_packageName, ctpStatus1X);
	    }
            else if (Status == 2) {
	      ctpStatus2X = bit;
              fill(m_packageName, ctpStatus2X);
	    }
	  }
	}
      }
    }
  }
}


void
TrigT1CTMonitoring::BSMonitoringAlgorithm::doCtpMuctpi( const CTP_RDO* theCTP_RDO, 
							//const CTP_RIO* theCTP_RIO,
							const MuCTPI_RDO* theMuCTPI_RDO, 
							//const MuCTPI_RIO* theMuCTPI_RIO,
							const EventContext& ctx) const
{
  ATH_MSG_DEBUG( "CTPMON begin doCtpMuctpi()");
  //ERROR histos
  auto errorSummaryX = Monitored::Scalar<int>("errorSummaryX",0);
  auto errorSummaryY = Monitored::Scalar<int>("errorSummaryY",0);
  auto errorSummaryPerLumiBlockX = Monitored::Scalar<int>("errorSummaryPerLumiBlockX",0);
  auto errorSummaryPerLumiBlockY = Monitored::Scalar<int>("errorSummaryPerLumiBlockY",0);
  auto errorPerLumiBlockX = Monitored::Scalar<int>("errorPerLumiBlockX",0);
  //CTPMUCTPI
  auto headerL1IdDifferenceX = Monitored::Scalar<int>("headerL1IdDifferenceX",0);
  auto headerBCIDDifferenceX = Monitored::Scalar<int>("headerBCIDDifferenceX",0);
  auto bcidDifferenceX = Monitored::Scalar<int>("bcidDifferenceX",0);

  auto eventInfo = GetEventInfo(ctx);
  /*
  if (theCTP_RIO && theMuCTPI_RIO) {
    uint32_t ctp_evid = theCTP_RIO->getLvl1Id();
    uint32_t ctp_bcid = (theCTP_RIO->getBCID() & 0xf);
    uint32_t muctpi_evid = theMuCTPI_RIO->getHeaderLVL1ID();
    uint32_t muctpi_bcid = theMuCTPI_RIO->getHeaderBCID();
    int diffValBcid = (static_cast < int >(ctp_bcid) -
		       static_cast < int >((muctpi_bcid)&0xf));
    int diffValEvid = static_cast < int >(ctp_evid) -
      static_cast < int >(muctpi_evid);
    headerL1IdDifferenceX = diffValEvid;
    fill(m_packageName, headerL1IdDifferenceX);
    headerBCIDDifferenceX = diffValBcid;
    fill(m_packageName, headerBCIDDifferenceX);
    uint32_t currentLumiBlock = eventInfo->lumiBlock();
    if (diffValBcid!=0) {
      ATH_MSG_WARNING( "BCID mismatch between CTP and MuCTPI RIOs, filling error histograms");
      errorSummaryX = 3;
      errorSummaryY = 1;
      fill(m_packageName, errorSummaryX, errorSummaryY);
      errorSummaryPerLumiBlockX = currentLumiBlock;
      errorSummaryPerLumiBlockY = 3;
      fill(m_packageName, errorSummaryPerLumiBlockX, errorSummaryPerLumiBlockY);
      errorPerLumiBlockX = currentLumiBlock;
      fill(m_packageName, errorPerLumiBlockX);
    }
    else {
      errorSummaryX = 3;
      errorSummaryY = 0;
      fill(m_packageName, errorSummaryX, errorSummaryY);
    }
    if (diffValBcid < -200.5
	|| diffValBcid > 200.5) {
      ATH_MSG_WARNING( "BCID difference out of range. CTP_RODHeader_BCID: " << ctp_bcid << " MuCTPI_RODHeader_BCID: " << (muctpi_bcid));      
    }

  }
*/
  MuCTPI_MultiplicityWord_Decoder multWord(theMuCTPI_RDO->candidateMultiplicity(), m_inclusiveTriggerThresholds);
  MuCTPI_DataWord_Decoder dataWord(0);

  CTP_Decoder ctp;
  ctp.setRDO(theCTP_RDO);
  uint32_t numberBC = theCTP_RDO->getNumberOfBunches();
  if (numberBC > 0) {
    unsigned int storeBunch = theCTP_RDO->getL1AcceptBunchPosition();
    const std::vector<CTP_BC> &BCs = ctp.getBunchCrossings();
    const CTP_BC & bunch = BCs[storeBunch];
    uint16_t mictp_bcid = multWord.getBCID();
    bcidDifferenceX = static_cast < int >(mictp_bcid) -
      static_cast < int >(bunch.getBCID() & 7);
    fill(m_packageName, bcidDifferenceX);
  }
}

void
TrigT1CTMonitoring::BSMonitoringAlgorithm::doMuonRoI( const MuCTPI_RDO* theMuCTPI_RDO, 
						      //const MuCTPI_RIO* theMuCTPI_RIO,
						      const ROIB::RoIBResult* roib,
						      const EventContext& ctx) const
{
  ATH_MSG_DEBUG( "CTPMON begin doMuonRoI()");
  //ERROR histos
  auto errorSummaryX = Monitored::Scalar<int>("errorSummaryX",0);
  auto errorSummaryY = Monitored::Scalar<int>("errorSummaryY",0);
  auto errorSummaryPerLumiBlockX = Monitored::Scalar<int>("errorSummaryPerLumiBlockX",0);
  auto errorSummaryPerLumiBlockY = Monitored::Scalar<int>("errorSummaryPerLumiBlockY",0);
  auto errorPerLumiBlockX = Monitored::Scalar<int>("errorPerLumiBlockX",0);
  //ROI
  auto nCandidates_secLocX = Monitored::Scalar<int>("nCandidates_secLocX",0);
  //  auto roiEtaPhiX = Monitored::Scalar<int>("roiEtaPhiX",0);
  ///  auto roiEtaPhiY = Monitored::Scalar<int>("roiEtaPhiY",0);
  //  auto roiEtaX = Monitored::Scalar<int>("roiEtaX",0);
  //  auto roiPhiX = Monitored::Scalar<int>("roiPhiX",0);
  auto barrelSectorIDRoiX = Monitored::Scalar<int>("barrelSectorIDRoiX",0);
  auto barrelSectorIDRoiY = Monitored::Scalar<int>("barrelSectorIDRoiY",0);
  auto endcapSectorIDRoiX = Monitored::Scalar<int>("endcapSectorIDRoiX",0);
  auto endcapSectorIDRoiY = Monitored::Scalar<int>("endcapSectorIDRoiY",0);
  auto forwardSectorIDRoiX = Monitored::Scalar<int>("forwardSectorIDRoiX",0);
  auto forwardSectorIDRoiY = Monitored::Scalar<int>("forwardSectorIDRoiY",0);
  auto barrelSectorIDRoiEtaX = Monitored::Scalar<int>("barrelSectorIDRoiEtaX",0);
  //  auto barrelSectorIDRoiEtaY = Monitored::Scalar<int>("barrelSectorIDRoiEtaY",0);
  auto endcapSectorIDRoiEtaX = Monitored::Scalar<int>("endcapSectorIDRoiEtaX",0);
  //  auto endcapSectorIDRoiEtaY = Monitored::Scalar<int>("endcapSectorIDRoiEtaY",0);
  auto forwardSectorIDRoiEtaX = Monitored::Scalar<int>("forwardSectorIDRoiEtaX",0);
  //  auto forwardSectorIDRoiEtaY = Monitored::Scalar<int>("forwardSectorIDRoiEtaY",0);
  auto barrelSectorIDRoiPhiX = Monitored::Scalar<int>("barrelSectorIDRoiPhiX",0);
  //  auto barrelSectorIDRoiPhiY = Monitored::Scalar<int>("barrelSectorIDRoiPhiY",0);
  auto endcapSectorIDRoiPhiX = Monitored::Scalar<int>("endcapSectorIDRoiPhiX",0);
  //  auto endcapSectorIDRoiPhiY = Monitored::Scalar<int>("endcapSectorIDRoiPhiY",0);
  auto forwardSectorIDRoiPhiX = Monitored::Scalar<int>("forwardSectorIDRoiPhiX",0);
  //  auto forwardSectorIDRoiPhiY = Monitored::Scalar<int>("forwardSectorIDRoiPhiY",0);

  auto eventInfo = GetEventInfo(ctx);

  const std::vector<ROIB::MuCTPIRoI> roiVec = roib->muCTPIResult().roIVec();
  //int bcidMismatch = 0;
  //uint16_t bcId;
  //uint16_t pTval;
  //uint16_t pTnum;
  //double eta;
  //double phi;
  uint16_t secID;
  //uint16_t sysID;
  uint16_t hemisphere;
  uint16_t roInum;
  //bool accepted;
  //bool first;
  //bool duplicatedRoI;
  //bool duplicatedSector;
  //int candNum=0;
  /*
  if (theMuCTPI_RIO) {
    for ( int i = 0; i < theMuCTPI_RIO->getNRoI(); i++ ) {
      if (!theMuCTPI_RIO->getRoI(i, bcId, pTval, pTnum, eta, phi, secID, sysID, hemisphere, 
				 roInum, accepted, first, duplicatedRoI, duplicatedSector)) {
	ATH_MSG_WARNING( "RoI with index " << i  << " not found, skipping this RoI");
	continue;
      }
      roiEtaPhiX = eta;
      roiEtaPhiY = phi;
      fill(m_packageName, roiEtaPhiX, roiEtaPhiY);
      roiEtaX = eta;
      fill(m_packageName, roiEtaX);
      roiPhiX = phi;
      fill(m_packageName, roiPhiX);
      if (theMuCTPI_RIO->getBCID() != bcId) bcidMismatch++;
    }
  */
  //uint32_t currentLumiBlock = eventInfo->lumiBlock();
    for ( unsigned int j = 0; j < roiVec.size(); j++ ) {
      //bool isCand = false;
      /*
      int cnt = 0;
      for(auto it=0; it != theMuCTPI_RIO->getNRoI()  && cnt < theMuCTPI_RIO->getNRoI() ; ++it,++cnt) {
	ATH_MSG_WARNING("test: " << it << " cnt: " << cnt);
	}
      for ( int i = 0; i < theMuCTPI_RIO->getNRoI(); i++ ) {
	theMuCTPI_RIO->getRoI(i, bcId, pTval, pTnum, eta, phi, secID, sysID, hemisphere, 
			      roInum, accepted, first, duplicatedRoI, duplicatedSector);
	if ((roiVec[j].getRoiNumber() == roInum) && 
	    (roiVec[j].pt() == pTnum) && 
	    (roiVec[j].getSectorID() == secID) &&
	    (roiVec[j].getSectorLocation() == sysID) &&
	    (roiVec[j].getHemisphere() == hemisphere) &&
	    (theMuCTPI_RIO->getBCID() == bcId)) {
	  isCand=true;
	  candNum=i;
	}
      }
      if (isCand == false) {
	ATH_MSG_WARNING( "No DAQ muon for RoI number " 
			 << roiVec[j].getRoiNumber() << ", pT " << roiVec[j].pt() << ", and sector ID " 
			 << roiVec[j].getSectorID());
	errorSummaryX = 7;
	errorSummaryY = 1;
	fill(m_packageName, errorSummaryX, errorSummaryY);
	errorSummaryPerLumiBlockX = currentLumiBlock;
	errorSummaryPerLumiBlockY = 7;
	fill(m_packageName, errorSummaryPerLumiBlockX, errorSummaryPerLumiBlockY);
	errorPerLumiBlockX = currentLumiBlock;
	fill(m_packageName, errorPerLumiBlockX);
      }
      else {      
	errorSummaryX = 7;
	errorSummaryY = 0;
	fill(m_packageName, errorSummaryX, errorSummaryY);
	theMuCTPI_RIO->getRoI(candNum, bcId, pTval, pTnum, eta, phi, secID, sysID, hemisphere, 
			      roInum, accepted, first, duplicatedRoI, duplicatedSector);
      */
	if (theMuCTPI_RDO) {// go back to data words to check sector locations
	  MuCTPI_DataWord_Decoder dataWord(0);
	  const std::vector<uint32_t> &vDataWords = theMuCTPI_RDO->dataWord();
	  for ( std::vector<uint32_t>::const_iterator it = vDataWords.begin(); it != vDataWords.end(); ++it ) {
	    dataWord.setWord(*it);

	    // MuCTPI_DataWord_Decoder only corrects sector ID for hemisphere if candidate is barrel
	    // correct back for this by providing an argument to getSectorID
	    //uint16_t sectorID = dataWord.getSectorID();
	    //if (dataWord.getSectorLocation() == MuCTPI_RDO::BARREL) sectorID=dataWord.getSectorID(1);
         
	    /*
	    if ((sectorID == secID)
		&& (dataWord.getRoiNumber() == roInum)
		&& (dataWord.getSectorLocation() == sysID)
		&& (dataWord.getHemisphere() == hemisphere)) {
	    */
	    
	    //sysID = dataWord.getSectorLocation();
	    roInum = dataWord.getRoiNumber();
	    hemisphere = dataWord.getHemisphere();
	    secID = dataWord.getSectorID();

	      nCandidates_secLocX = dataWord.getSectorLocation();
	      fill(m_packageName, nCandidates_secLocX);
            
	      if (dataWord.getSectorLocation() == MuCTPI_RDO::BARREL) {
		uint16_t secID1;
		secID1 = dataWord.getSectorID(1);
		barrelSectorIDRoiX = secID1+32*hemisphere;
		barrelSectorIDRoiY = roInum;
		fill(m_packageName, barrelSectorIDRoiX, barrelSectorIDRoiY);
		barrelSectorIDRoiEtaX = secID1+32*hemisphere;
		fill(m_packageName, barrelSectorIDRoiEtaX);
		barrelSectorIDRoiPhiX = secID1+32*hemisphere;
		fill(m_packageName, barrelSectorIDRoiPhiX);
	      }
	      else if (dataWord.getSectorLocation() == MuCTPI_RDO::ENDCAP) {
		endcapSectorIDRoiX = secID+48*hemisphere;
		endcapSectorIDRoiY = roInum;
		fill(m_packageName, endcapSectorIDRoiX, endcapSectorIDRoiY);
		endcapSectorIDRoiEtaX = secID+48*hemisphere;
		fill(m_packageName, endcapSectorIDRoiEtaX);
		endcapSectorIDRoiPhiX = secID+48*hemisphere;
		fill(m_packageName, endcapSectorIDRoiPhiX);
	      }
	      else if (dataWord.getSectorLocation() == MuCTPI_RDO::FORWARD) {
		forwardSectorIDRoiX = secID+24*hemisphere;
		forwardSectorIDRoiY = roInum;
		fill(m_packageName, forwardSectorIDRoiX, forwardSectorIDRoiY);
		forwardSectorIDRoiEtaX = secID+24*hemisphere;
		fill(m_packageName, forwardSectorIDRoiEtaX);
		forwardSectorIDRoiPhiX = secID+24*hemisphere;
		fill(m_packageName, forwardSectorIDRoiPhiX);
	      }
	    }
	  }
    }
    //}
    //}
    /*
    int lvl2Expected = static_cast<int>(theMuCTPI_RIO->getNRoI()) - bcidMismatch;
    if (lvl2Expected != static_cast<int>(roiVec.size()) && lvl2Expected <= 14) {
      //patrick ATH_MSG_WARNING 
      ATH_MSG_DEBUG( "Expected " << lvl2Expected
		       << " LVL2 RoIs, but found " << roiVec.size()
		       << "!");
      errorSummaryX = 6;
      errorSummaryY = 1;
      fill(m_packageName, errorSummaryX, errorSummaryY);
      errorSummaryPerLumiBlockX = currentLumiBlock;
      errorSummaryPerLumiBlockY = 6;
      fill(m_packageName, errorSummaryPerLumiBlockX, errorSummaryPerLumiBlockY);
      errorPerLumiBlockX = currentLumiBlock;
      fill(m_packageName, errorPerLumiBlockX);
    }
    else if (static_cast<int>(roiVec.size()) != 14 && lvl2Expected >= 14) {
      //patrick ATH_MSG_WARNING
      ATH_MSG_DEBUG( "Expected 14 RoI's from " << lvl2Expected
		       << " MuCTPI RoI's, but found " << roiVec.size()
		       << "!");

      errorSummaryX = 6;
      errorSummaryY = 1;
      fill(m_packageName, errorSummaryX, errorSummaryY);
      errorSummaryPerLumiBlockX = currentLumiBlock;
      errorSummaryPerLumiBlockY = 6;
      fill(m_packageName, errorSummaryPerLumiBlockX, errorSummaryPerLumiBlockY);
      errorPerLumiBlockX = currentLumiBlock;
      fill(m_packageName, errorPerLumiBlockX);
    }
    else {
      errorSummaryX = 6;
      errorSummaryY = 0;
      fill(m_packageName, errorSummaryX, errorSummaryY);
    }
    */
}
//}

StatusCode
TrigT1CTMonitoring::BSMonitoringAlgorithm::compareRerun(const CTP_BC &bunchCrossing,
							const EventContext& ctx) const
{
  bool itemMismatch{false};
  //ERROR histos
  auto errorSummaryX = Monitored::Scalar<int>("errorSummaryX",0);
  auto errorSummaryY = Monitored::Scalar<int>("errorSummaryY",0);
  auto errorSummaryPerLumiBlockX = Monitored::Scalar<int>("errorSummaryPerLumiBlockX",0);
  auto errorSummaryPerLumiBlockY = Monitored::Scalar<int>("errorSummaryPerLumiBlockY",0);
  auto errorPerLumiBlockX = Monitored::Scalar<int>("errorPerLumiBlockX",0);
  //compareRerun
  auto l1ItemsBPSimMismatchX = Monitored::Scalar<int>("l1ItemsBPSimMismatchX",0);
  auto l1ItemsBPSimMismatchY = Monitored::Scalar<int>("l1ItemsBPSimMismatchY",0);
  //the following is char - correctly filled?
  auto l1ItemsBPSimMismatchItemsX = Monitored::Scalar<std::string>("l1ItemsBPSimMismatchItemsX","");
  auto l1ItemsBPSimMismatchItemsY = Monitored::Scalar<int>("l1ItemsBPSimMismatchItemsY",0);

  auto eventInfo = GetEventInfo(ctx);

  const CTP_RDO* theCTP_RDO_Rerun = nullptr;
  ATH_MSG_DEBUG( "Retrieving CTP_RDO from SG with key CTP_RDO_Rerun");
  //ATH_MSG_INFO( "Retrieving CTP_RDO from SG with key CTP_RDO_Rerun");
  CHECK( (theCTP_RDO_Rerun = SG::get(m_CTP_RDO_RerunKey, ctx)) != nullptr );
  //https://gitlab.cern.ch/atlas/athena/-/blob/master/Trigger/TrigT1/TrigT1Result/src/CTP_RDO.cxx#L19
  // as I see: https://gitlab.cern.ch/atlas/athena/-/blob/master/Trigger/TrigT1/TrigT1Result/src/CTP_RDO.cxx#L51
  // CTPVersion has not been set, no information about data format available, please fix your code
  //CTP_RDO::CTP_RDO(unsigned int ctpVersionNumber, const uint32_t nBCs, uint32_t nExtraWords)
  //const uint32_t nBCs = 0;
  //uint32_t nExtraWords = 0;
  //CHECK( (theCTP_RDO_Rerun = SG::get(m_CTP_RDO_RerunKey, ctx)) != nullptr );
  //CHECK( (theCTP_RDO_Rerun = SG::get(m_CTP_RDO_RerunKey, 4, nBCs, nExtraWords, ctx)) != nullptr );
  //ATH_MSG_WARNING( "theCTP_RDO_Rerun->getCTPVersionNumber " << theCTP_RDO_Rerun->getCTPVersionNumber());
  //CTP_RDO* theCTP_RDO_Rerun2 = (CTP_RDO*)theCTP_RDO_Rerun;
  //theCTP_RDO_Rerun2->setCTPVersionNumber(4);
  //theCTP_RDO_Rerun->setCTPVersionNumber(4);
  //ATH_MSG_WARNING( "theCTP_RDO_Rerun2->getCTPVersionNumber " << theCTP_RDO_Rerun2->getCTPVersionNumber());

  CTP_Decoder ctp_rerun;
  ctp_rerun.setRDO(theCTP_RDO_Rerun);

  const std::vector<CTP_BC> ctp_bc_rerun=ctp_rerun.getBunchCrossings();
  if (ctp_bc_rerun.size() != 1) {
    ATH_MSG_ERROR( "Rerun simulation has non unity number of bunch crossings ");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG( "In compareRerun: dumping data for BC " << bunchCrossing.getBCID());
  bunchCrossing.dumpData(msg());

  ATH_MSG_DEBUG( "In compareRerun: dumping rerun data for BC 0");
  ctp_bc_rerun.at(0).dumpData(msg());
   
  ATH_MSG_DEBUG( "Comparing TBP from CTP_RDO objects with keys CTP_RDO (from data) and CTP_RDO_Rerun (from simulation)");
   
  const std::bitset<512> currentTBP(bunchCrossing.getTBP());
  const std::bitset<512> currentTBP_rerun(ctp_bc_rerun.at(0).getTBP());
   
  if ( currentTBP != currentTBP_rerun ) {
    const TrigConf::L1Menu * l1menu = nullptr;
    ATH_CHECK(detStore()->retrieve(l1menu));
    for(const TrigConf::L1Item & item : *l1menu) {

      //do not include random and non-simulated triggers in this test (can be configured)
      bool skip = item.definition().find("RNDM") != std::string::npos;
      for(const std::string & p : m_ignorePatterns) {
	if(item.name().find(p) != std::string::npos) {
	  skip = true;
	  break;
	}
      }
      if( skip ) continue;

      bool tbp       = currentTBP.test( item.ctpId() );
      bool tbp_rerun = currentTBP_rerun.test( item.ctpId() );
      if ( tbp !=  tbp_rerun) {
	ATH_MSG_WARNING( "CTPSimulation TBP / TPB_rerun mismatch!! For L1Item '" << item.name()
			 << "' (CTP ID " << item.ctpId() << "): data="
			 << (tbp?"pass":"fail") << " != simulation=" << (tbp_rerun?"pass":"fail"));
	itemMismatch=true;
	l1ItemsBPSimMismatchX = item.ctpId();
	l1ItemsBPSimMismatchY = 1;
	fill(m_packageName, l1ItemsBPSimMismatchX, l1ItemsBPSimMismatchY);
	l1ItemsBPSimMismatchItemsX = (item.name()).c_str();
	l1ItemsBPSimMismatchItemsY = 1;
	fill(m_packageName, l1ItemsBPSimMismatchItemsX, l1ItemsBPSimMismatchItemsY);
      }
    }
  }
  uint32_t currentLumiBlock = eventInfo->lumiBlock();
  if (itemMismatch) {
    ATH_MSG_WARNING( "Mismatch between CTP data and simulation in BC " << bunchCrossing.getBCID());
    errorSummaryX = 14;
    errorSummaryY = 1;
    fill(m_packageName, errorSummaryX, errorSummaryY);
    errorSummaryPerLumiBlockX = currentLumiBlock;
    errorSummaryPerLumiBlockY = 14;
    fill(m_packageName, errorSummaryPerLumiBlockX, errorSummaryPerLumiBlockY);
    errorPerLumiBlockX = currentLumiBlock;
    fill(m_packageName, errorPerLumiBlockX);
  } else {
    errorSummaryX = 14;
    errorSummaryY = 0;
    fill(m_packageName, errorSummaryX, errorSummaryY);
  }
  return StatusCode::SUCCESS;
}

void
TrigT1CTMonitoring::BSMonitoringAlgorithm::dumpData( const CTP_RDO* theCTP_RDO, 
						     //const CTP_RIO* theCTP_RIO,
						     const MuCTPI_RDO* theMuCTPI_RDO, 
						     //const MuCTPI_RIO* theMuCTPI_RIO,
						     const ROIB::RoIBResult* roib,
						     const EventContext& ctx) const
{

  auto eventInfo = GetEventInfo(ctx);

  if ( !msgLvl(MSG::DEBUG) )
    return;
  ATH_MSG_DEBUG( "Run number: " << eventInfo->runNumber() << ", Event: " << eventInfo->eventNumber() << ", LB: " << eventInfo->lumiBlock() ); 
  if ( m_processMuctpi ) {
    // MuCTPI Multiplicity data
    MuCTPI_MultiplicityWord_Decoder multWord(theMuCTPI_RDO->
					     candidateMultiplicity(),
					     m_inclusiveTriggerThresholds);
    ATH_MSG_DEBUG("MuCTPI_Multiplicity data :");
    multWord.dumpData();
    // MuCTPI candidate data
    MuCTPI_DataWord_Decoder dataWord(0);
    ATH_MSG_DEBUG( "MIOCT candidate data :");
    std::vector<uint32_t>::const_iterator it = theMuCTPI_RDO->dataWord().begin();
    int count = 1;
    for ( ; it != theMuCTPI_RDO->dataWord().end(); ++it ) {
      ATH_MSG_DEBUG( "Candidate " << count);
      ATH_MSG_DEBUG("datawordold: " <<*it);
      dataWord.setWord(*it);
      dataWord.dumpData();
      ++count;
    }
  }

  if ( m_processCTP ) {
    // CTP information
    CTP_Decoder ctp;
    ctp.setRDO(theCTP_RDO);
    ATH_MSG_DEBUG("CTP data from CTP_RDO:");
    ctp.dumpData();

    //Misc. information
    //if ( theCTP_RIO )
    //  theCTP_RIO->dumpData();
    //if ( theMuCTPI_RIO )
    //  theMuCTPI_RIO->dumpData();
  }

  if ( roib )
    roib->dumpData();
}
