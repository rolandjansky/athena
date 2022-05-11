/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// Package includes
#include "BSMonitoring.h"
#include "TrigT1Result/MuCTPI_DataWord_Decoder.h"
#include "TrigT1Result/MuCTPI_MultiplicityWord_Decoder.h"
#include "TrigT1Result/CTP_Decoder.h"
#include "TrigT1MuctpiBits/MuCTPI_Bits.h"

// TrigConf includes
#include "TrigConfData/L1Menu.h"

// COOL includes
#include "CoraCool/CoraCoolDatabaseSvcFactory.h"
#include "CoraCool/CoraCoolDatabaseSvc.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/ChannelSelection.h"
#include "CoolKernel/IObjectIterator.h"
#include "CoolKernel/IObject.h"
#include "CoraCool/CoraCoolDatabase.h"
#include "CoraCool/CoraCoolFolder.h"
#include "CoraCool/CoraCoolObjectIter.h"
#include "CoraCool/CoraCoolObject.h"
#include "CoralBase/Blob.h"

// ROOT includes
#include "Rtypes.h" // for Int_t
#include "TH1F.h" // for SetRangeUser
#include "TH2F.h" // for SetRangeUser
#include "TMath.h" // for Pi()

//LWHists includes:
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TProfile_LW.h"

#include "AthenaMonitoring/AthenaMonManager.h" //for environment
#include <cmath>


using namespace std;

TrigT1CTMonitoring::BSMonitoring::BSMonitoring(const std::string & type, const std::string & name, const IInterface * parent)
  : ManagedMonitorToolBase(type, name, parent)
{}

StatusCode
TrigT1CTMonitoring::BSMonitoring::initialize()
{
  // make sure to call initialize of the base class!
  ATH_CHECK( ManagedMonitorToolBase::initialize() );

  // connect to RPC and TGC RoI tools
  if ( m_processMuctpi ) {
    ATH_CHECK( m_rpcRoiTool.retrieve() );
    ATH_CHECK( m_tgcRoiTool.retrieve() );
  }

  ATH_CHECK( m_MuCTPI_RDOKey.initialize(m_processMuctpi) );
  ATH_CHECK( m_MuCTPI_RIOKey.initialize(m_processMuctpi && m_processMuctpiRIO && ! m_runOnESD) );
  ATH_CHECK( m_CTP_RDOKey.initialize(m_processCTP) );
  ATH_CHECK( m_CTP_RIOKey.initialize(m_processCTP && ! m_runOnESD) );
  ATH_CHECK( m_CTP_RDO_RerunKey.initialize(m_processCTP && m_compareRerun) );
  ATH_CHECK( m_RoIBResultKey.initialize(m_processRoIB && m_processMuctpiRIO) );
  ATH_CHECK( m_RPCContainerKey.initialize(m_processMuctpi) );
  ATH_CHECK( m_TGCContainerKey.initialize(m_processMuctpi) );
  ATH_CHECK( m_EventInfoKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode
TrigT1CTMonitoring::BSMonitoring::bookHistograms()
{
   try {
      ATH_MSG_DEBUG( "bookHistograms()");
      ATH_MSG_DEBUG( "Clearing HistMap and resetting event counters for new run");

      m_histNames.clear();
      m_eventCount = 0;
      m_firstEventTime = 0;
      m_firstEventBcid = 0;
      m_firstEventTC = 0;
      m_lumiBlockOfPreviousEvent = 0;
      m_filledEventCount = 0;
      m_maxLumiBlock = 0;
      m_currentLumiBlock = 0;
      m_runNumber = 0;
      m_eventNumber = 0;

      CHECK( initCtpHistograms() );
      CHECK( initMuctpiHistograms() );
      CHECK( initCtpMuctpiHistograms() );
      CHECK( initRoIHistograms() );

      m_histogramsBooked = true;

      if (m_retrieveCoolData) {
         ATH_MSG_DEBUG( "Will now try to access LB times in folder " << m_lbTimeCoolFolderName);
         m_lumiBlocks.clear();
         m_lbStartTimes.clear();
         m_lbEndTimes.clear();
         m_lbStartFreqMeasurements.clear();
         m_freqMeasurements.clear();
         m_beamMode.clear();
         const CTP_RIO* theCTP_RIO = SG::get(m_CTP_RIOKey);
         if (theCTP_RIO!=nullptr) {
            getCoolData(theCTP_RIO->getRunNumber());
            ATH_MSG_DEBUG( m_lumiBlocks.size() << " lumi blocks found");
            for ( std::vector<uint32_t>::const_iterator lbIt = m_lumiBlocks.begin(); 
                  lbIt != m_lumiBlocks.end(); ++lbIt ) {
               ATH_MSG_DEBUG( "LB " << *lbIt << ": start time = " << m_lbStartTimes[*lbIt] 
                              << ", end time = " << m_lbEndTimes[*lbIt]);
            }
            if (m_lbStartTimes.size() == 0) {
               ATH_MSG_WARNING( "Lumi block timestamps not found!");
            }
            else {
               m_retrievedLumiBlockTimes = true;
            }
         }
      }
   }
   catch(const std::exception & e) {
      std::cerr << "Caught standard C++ exception: " << e.what() << " from bookHistograms()" << std::endl;
      return StatusCode::FAILURE;
   }
   return StatusCode::SUCCESS;
}

StatusCode
TrigT1CTMonitoring::BSMonitoring::fillHistograms()
{
   //evtStore()->dump();
   try {
      if (!m_histogramsBooked) {
         ATH_MSG_ERROR( "fillHistograms() called before bookHistograms(*,*,1)!");
         ATH_MSG_ERROR( "calling bookHistograms(1,1,1) manually...");
         CHECK( bookHistograms() );
      }

      ATH_MSG_DEBUG( "begin fillHistograms()");

      // Now see what exists in StoreGate...
      const MuCTPI_RDO* theMuCTPI_RDO = 0;
      const MuCTPI_RIO* theMuCTPI_RIO = 0;
      const CTP_RDO* theCTP_RDO = 0;
      const CTP_RIO* theCTP_RIO = 0;
      const ROIB::RoIBResult* roIBResult = 0;
      const Muon::TgcCoinDataContainer* theTGCContainer = 0;
      const RpcSectorLogicContainer* theRPCContainer = 0;
      const xAOD::EventInfo* eventInfo = 0; 

      bool validMuCTPI_RIO = true;
      bool validMuCTPI_RDO = true;
      bool validCTP_RIO = true;
      bool validCTP_RDO = true;
      bool validRoIBResult = true;
      bool validTGCContainer = true;
      bool validRPCContainer = true;
      int numberOfInvalidFragments = 0;

      TProfile_LW* errorSummary = getTProfile("errorSummary");
      TH1F_LW* errorPerLumiBlock = getTH1("errorPerLumiBlock");
      TH2F_LW* errorSummaryPerLumiBlock = getTH2("errorSummaryPerLumiBlock");
      TH1F_LW* incompleteFragmentType = getTH1("incompleteFragmentType");

      if ( !errorSummary || !errorSummaryPerLumiBlock || !errorPerLumiBlock || !incompleteFragmentType ) {
         ATH_MSG_FATAL("Problems finding error histograms!");
         return StatusCode::FAILURE;
      }

      if (m_processMuctpi) {
         theMuCTPI_RDO = SG::get(m_MuCTPI_RDOKey);
         if (!theMuCTPI_RDO) {
	   ATH_MSG_WARNING( "Could not find \"" << m_MuCTPI_RDOKey.key() << "\" in StoreGate");
	   validMuCTPI_RDO = false;
	   ++numberOfInvalidFragments;
         }
      	 // now try to get RPC and TGC SL output for comparisons
	       theRPCContainer = SG::get(m_RPCContainerKey);
         if (!theRPCContainer) {
	   ATH_MSG_WARNING( "Could not find RPC container in StoreGate");
            validRPCContainer = false;
         }
	       theTGCContainer = SG::get(m_TGCContainerKey);
         if (!theTGCContainer) {
	   ATH_MSG_WARNING( "Could not find TGC container in StoreGate");
            validTGCContainer = false;
         }
         if (m_processMuctpiRIO && !m_runOnESD) {
            theMuCTPI_RIO = SG::get(m_MuCTPI_RIOKey);
            if (!theMuCTPI_RIO) {
               ATH_MSG_WARNING( "Could not find \"" << m_MuCTPI_RIOKey.key() << "\" in StoreGate");
               validMuCTPI_RIO = false;
               ++numberOfInvalidFragments;
            }
         }
      }

      if (m_processCTP) {
         theCTP_RDO = SG::get(m_CTP_RDOKey);
         if (!theCTP_RDO) {
            ATH_MSG_WARNING( "Could not find \"" << m_CTP_RDOKey.key() << "\" in StoreGate");
            validCTP_RDO = false;
            ++numberOfInvalidFragments;
         }
         if (!m_runOnESD) {
            theCTP_RIO = SG::get(m_CTP_RIOKey);
            if (!theCTP_RIO) {
               ATH_MSG_WARNING( "Could not find \"" << m_CTP_RIOKey.key() << "\" in StoreGate");
               validCTP_RIO = false;
               ++numberOfInvalidFragments;
            }
         }
      }
      if (m_processRoIB && m_processMuctpiRIO) {
         roIBResult = SG::get(m_RoIBResultKey);
         if (!roIBResult) {
            ATH_MSG_WARNING( "Could not find \"" << m_RoIBResultKey.key() << "\" in StoreGate");
            validRoIBResult = false;
            ++numberOfInvalidFragments;
         }
      }

      bool incompleteEvent = false;
      eventInfo = SG::get(m_EventInfoKey);
      if (eventInfo) {
         m_runNumber = eventInfo->runNumber();
         m_eventNumber = eventInfo->eventNumber();
         m_lumiBlockOfPreviousEvent = m_currentLumiBlock;
         m_currentLumiBlock = eventInfo->lumiBlock();
         incompleteEvent = eventInfo->eventFlags(xAOD::EventInfo::Core) & 0x40000;
         ATH_MSG_DEBUG( "Successfully retrieved EventInfo (run: " << m_runNumber << ", event: " << m_eventNumber << ")");
      }
      else {
         ATH_MSG_WARNING( "Could not retrieve EventInfo from StoreGate => run# = event# = 0, LB# = 99");
         m_lumiBlockOfPreviousEvent = m_currentLumiBlock;
         m_currentLumiBlock = 99; // dummy LB in case EventInfo is not available - prevents DQ defect flagging with LB# 0...
      }

      if ( incompleteEvent ) {
         ATH_MSG_WARNING( "Incomplete event according to EventInfo flag");
         incompleteFragmentType->Fill(5,1);
      }

      bool l1ctObjectMissingInStoreGate = ( !validCTP_RDO || !validCTP_RIO || !validMuCTPI_RDO || !validMuCTPI_RIO || !validRoIBResult );
      if ( l1ctObjectMissingInStoreGate ) {
         ATH_MSG_WARNING( "At least one L1CT object is missing in SG");
      }

      dumpData(theCTP_RDO, theCTP_RIO, theMuCTPI_RDO, theMuCTPI_RIO, roIBResult);

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
               ATH_MSG_INFO("MuCTPI_RDO reports " << multWord.getNCandidates() 
                  << "  candidates, but there are only " << theMuCTPI_RDO->dataWord().size()
                  << " data words, ignoring MuCTPI");
               validMuCTPI_RDO = false;
            }
         }
         // Note: there's no simple way of checking the validity of the MUCTPI_RIO, so we don't for now.
      }

      // if at least one fragment is missing/incomplete, print out a summary
      if (!validCTP_RDO || !validCTP_RIO || !validMuCTPI_RDO || !validMuCTPI_RIO || !validRoIBResult) {
         ATH_MSG_WARNING( "At least one missing/invalid L1CT fragment detected");
         ATH_MSG_WARNING( "CTP_RDO: " << validCTP_RDO << ", CTP_RIO: " << validCTP_RIO 
            << ", MuCTPI_RIO: " << validMuCTPI_RIO << ", MuCTPI_RDO: " << validMuCTPI_RDO 
            << ", RoIBResult: " << validRoIBResult);
         //ATH_MSG_WARNING( getEventInfoString());
         if (validCTP_RIO) {
            ATH_MSG_WARNING( "CTP_RIO says LB: " << (theCTP_RIO->getDetectorEventType() & 0xffff) 
               << ", L1ID: " << std::dec << theCTP_RIO->getLvl1Id() 
               << " (HEX: " << std::hex << theCTP_RIO->getLvl1Id() << ")" << std::dec 
               << ", BCID: " << theCTP_RIO->getBCID());
         }
         else if (eventInfo) {
            ATH_MSG_WARNING( "CTP_RIO missing, EventInfo says LB: " << eventInfo->lumiBlock()
                             << ", BCID: " << eventInfo->bcid()); // no L1ID available
         }
         else {
            ATH_MSG_WARNING( "Not printing event details since both CTP_RIO and EventInfo objects are missing");
         }

         // only fill error-per-LB histograms if L1CT fragments are missing and global incomplete-event flag 
         // from EventInfo does not say that the event is incomplete
         if ( !incompleteEvent ) {
            errorSummaryPerLumiBlock->Fill(m_currentLumiBlock, 15);
            errorPerLumiBlock->Fill(m_currentLumiBlock);
         }
         errorSummary->Fill(15, 1);

         if (!validCTP_RIO) {
            incompleteFragmentType->Fill(0,1);
         }
         if (!validCTP_RDO) {
            incompleteFragmentType->Fill(1,1);
         }
         if (!validMuCTPI_RIO) {
            incompleteFragmentType->Fill(2,1);
         }
         if (!validMuCTPI_RDO) {
            incompleteFragmentType->Fill(3,1);
         }
         if (!validRoIBResult) {
            incompleteFragmentType->Fill(4,1);
         }
         if (!validTGCContainer) {
            incompleteFragmentType->Fill(6,1);
         }
         if (!validRPCContainer) {
            incompleteFragmentType->Fill(7,1);
         }
      }
      else { // since errorSummary holds error _rate_, also fill when there are no errors
         errorSummary->Fill(15, 0);
      }

      // if the event is incomplete (missing L1CT objects or according to EventInfo), skip filling the rest of the histograms
      if ( !validCTP_RDO || !validCTP_RIO || !validMuCTPI_RDO || !validMuCTPI_RIO || !validRoIBResult || incompleteEvent ) {
         ATH_MSG_WARNING( "Event incomplete, will skip filling of all non-error histograms");
         return StatusCode::SUCCESS;
      }

      /*
       * Process and fill data 
       */

      if (m_processCTP && validCTP_RDO && validCTP_RIO) {
         doCtp(theCTP_RDO, theCTP_RIO);
      }

      if (m_processMuctpi && m_processCTP && validCTP_RDO && validCTP_RIO && validMuCTPI_RDO && validMuCTPI_RIO) {
      	doCtpMuctpi(theCTP_RDO, theCTP_RIO, theMuCTPI_RDO, theMuCTPI_RIO);
      }

      if (m_processMuctpi && validMuCTPI_RDO && validMuCTPI_RIO && validTGCContainer && validRPCContainer) {
         doMuctpi(theMuCTPI_RDO, theMuCTPI_RIO, theRPCContainer, theTGCContainer);
         if (m_processRoIB && m_processMuctpiRIO) {
            doMuonRoI(theMuCTPI_RDO, theMuCTPI_RIO, roIBResult);
         }
      }
      ++m_eventCount;

      ATH_MSG_DEBUG( "end fillHistograms()");
      return StatusCode::SUCCESS;
   }
   catch(const std::exception & e) {
      std::cerr << "Caught standard C++ exception: " << e.what() << " from fillHistograms()" << std::endl;
      return StatusCode::FAILURE;
   }
}

StatusCode
TrigT1CTMonitoring::BSMonitoring::procHistograms()
{

  ATH_MSG_DEBUG( "begin procHistograms()");
  
  updateRangeUser();
  
  TH1F_LW* h1;
  if ((h1=getTH1("l1ItemsBPSimMismatchItems"))) {
    int lastFilledXBin =  h1->getROOTHist()->FindLastBinAbove(0,1); // find last bin above 0 on x-axis
    h1->getROOTHist()->GetXaxis()->SetRangeUser(0, lastFilledXBin);
  }

  ATH_MSG_DEBUG( "processed " << m_eventCount << " events ("
		 << m_filledEventCount << " filled )");
  ATH_MSG_DEBUG( "end procHistograms()");
  return StatusCode::SUCCESS;
}

StatusCode
TrigT1CTMonitoring::BSMonitoring::initCtpHistograms()
{

  // error histograms
   m_dirName=m_baseDirName+"/";

   CHECK( registerTProfile("errorSummary", "CTP and MuCTPI errors; ; Error rate", 20, 0.5, 20.5, -1, 2) );
   CHECK( registerTH2("errorSummaryPerLumiBlock", "Errors per lumi block; LB number; Errors", 2000, 0.5, 2000.5, 20, 0.5, 20.5) );
   CHECK( registerTH1("errorPerLumiBlock", "Number of errors per lumi block; LB number; Errors", 2001, -0.5, 2000.5) );
   CHECK( registerTH1("incompleteFragmentType", "Number of missing fragments per type; Fragment type; Number of incomplete fragments", 8, -0.5, 7.5) );

   // CTP histograms
   m_dirName=m_baseDirName+"CTP/";
   CHECK( registerTH1("deltaBcid", "CTP Data BCID - ROD Header BCID; #DeltaBCID; Entries", 401, -200.5, 200.5) );
   CHECK( registerTH1("triggerType", "Trigger Type; Trigger Type; Entries", 256, -0.5, 255.5) );
   CHECK( registerTH1("timeSinceLBStart", "Time Since LB Start; Time After New LB (ms); Entries", 1000, -500, 1500) );
   CHECK( registerTH1("timeUntilLBEnd", "Time Until LB End; Time Until Next LB (ms); Entries", 1000, -500, 1500) );
   CHECK( registerTH1("timeSinceL1A", "Time since last L1A; Time since last L1A (ms); Entries", 2000, -1, 30) );
   CHECK( registerTH1("turnCounterTimeError", "Error of time based on turn counter and BCID; t_{TC+BCID}-t_{GPS} [#mus]; Entries", 2000, -1000., 1000.) );
   CHECK( registerTProfile("turnCounterTimeErrorVsLb", "Error of (TC+BCID)-based time vs. LB; LB; t_{TC+BCID}-t_{GPS} [#mus]", 2001, -0.5, 2000.5, -1000., 1000.) );
   CHECK( registerTH2("pitBC", "CTP BC vs. PIT; PIT; BC", 320, -0.5, 319.5, 127, -63.5, 63.5) );
   CHECK( registerTH2("pitFirstBC", "First CTP BC vs. PIT; PIT; BC", 320, -0.5, 319.5, 127, -63.5, 63.5) );
   CHECK( registerTH1("tav", "Trigger Items After Veto; CTP TAV; Entries", 512, -0.5, 511.5) );
   CHECK( registerTH1("ctpStatus1", "CTP Status Word 1; Bit; Number of times ON", 24, -0.5, 23.5) );
   CHECK( registerTH1("ctpStatus2", "CTP Status Word 2; Bit; Number of times ON", 24, -0.5, 23.5) );

   CHECK( registerTH1("l1ItemsBPSimMismatch","Sim mismatch L1 Items before prescale", 512, 0, 512) );
   CHECK( registerTH1("l1ItemsBPSimMismatchItems","Sim mismatch L1 Items before prescale, mismatched ones only", 512, 0, 512) );

   // bin lables
   TH1F_LW* l1ItemsBPSimMismatch = getTH1("l1ItemsBPSimMismatch");
   const TrigConf::L1Menu * l1menu = nullptr;
   ATH_CHECK(detStore()->retrieve(l1menu));
   for(const TrigConf::L1Item & item : *l1menu) {
      string label = item.name() + " (CTP ID " + std::to_string(item.ctpId())+ ")";
      l1ItemsBPSimMismatch->GetXaxis()->SetBinLabel(item.ctpId()+1,label.c_str());
   }
   std::map<int, std::string> errorSummaryBinLabels;
   errorSummaryBinLabels[1] = "CTP/ROD BCID Offset";
   errorSummaryBinLabels[2] = "No BCs in Readout";
   errorSummaryBinLabels[3] = "CTP/MuCTPI BCID Offset";
   errorSummaryBinLabels[4] = "Wrong Cand Word Number";
   errorSummaryBinLabels[5] = "Wrong Pt in Mult Word";
   errorSummaryBinLabels[6] = "Wrong Num of RoI";
   errorSummaryBinLabels[7] = "No Cand for RoI";
   errorSummaryBinLabels[8] = "Cand BC Out of Range";
   errorSummaryBinLabels[9] = "Invalid Lumi Block";
   errorSummaryBinLabels[10] = "LB Out of Time";
   errorSummaryBinLabels[11] = "Nanosec > 1e9";
   errorSummaryBinLabels[12] = "TAP w/out TBP";
   errorSummaryBinLabels[13] = "TAV w/out TAP";
   errorSummaryBinLabels[14] = "CTP sim. mismatch";
   errorSummaryBinLabels[15] = "Incomplete fragment";
   errorSummaryBinLabels[16] = "Missing orbit pulse";
   errorSummaryBinLabels[17] = "MuCTPI/noRPC candidate mismatch";
   errorSummaryBinLabels[18] = "MuCTPI/noTGC candidate mismatch";
   errorSummaryBinLabels[19] = "RPC/noMuCTPI candidate mismatch";
   errorSummaryBinLabels[20] = "TGC/noMuCTPI candidate mismatch";

   TProfile_LW* errorSummary = getTProfile("errorSummary");
   TH2F_LW* errorSummaryPerLumiBlock = getTH2("errorSummaryPerLumiBlock");
   if (errorSummary) {
      errorSummary->GetXaxis()->SetLabelSize(0.03);
      errorSummaryPerLumiBlock->GetYaxis()->SetLabelSize(0.03);
      for (unsigned int bin = 1; bin < errorSummaryBinLabels.size()+1; ++bin) {
         errorSummary->GetXaxis()->SetBinLabel(bin, errorSummaryBinLabels[bin].c_str());
         errorSummaryPerLumiBlock->GetYaxis()->SetBinLabel(bin, errorSummaryBinLabels[bin].c_str());
      }
   }

   TH1F_LW* incompleteFragmentType = getTH1("incompleteFragmentType");
   if (incompleteFragmentType) {
      incompleteFragmentType->GetXaxis()->SetBinLabel(1, "CTP RIO");
      incompleteFragmentType->GetXaxis()->SetBinLabel(2, "CTP RDO");
      incompleteFragmentType->GetXaxis()->SetBinLabel(3, "MuCTPI RIO");
      incompleteFragmentType->GetXaxis()->SetBinLabel(4, "MuCTPI RDO");
      incompleteFragmentType->GetXaxis()->SetBinLabel(5, "RoIBResult");
      incompleteFragmentType->GetXaxis()->SetBinLabel(6, "TGC SL RDO");
      incompleteFragmentType->GetXaxis()->SetBinLabel(7, "RPC SL RDO");
   }

   return StatusCode::SUCCESS;
}

StatusCode
TrigT1CTMonitoring::BSMonitoring::initMuctpiHistograms()
{

  m_dirName=m_baseDirName+"MUCTPI/";

  CHECK( registerTH1("nCandidates", "Number of MuCTPI candidates from mult word; Multiplicity; Entries", 11, -0.5, 10.5) );
  CHECK( registerTH1("nCandidatesDataWord", "Number of MuCTPI candidates from data word; Multiplicity; Entries", 100, -0.5, 99.5) );
  CHECK( registerTH1("pt", "Candidate p_{T} distribution from mult word; p_{T} threshold; Entries", 6, 0.5, 6.5) );
  CHECK( registerTH2("nCandidatesPt", "Number of MuCTPI candidates from mult word vs p_{T}; p_{T} threshold; MuCTPI candidates", 6, 0.5, 6.5, 8, -0.5, 7.5) );
  CHECK( registerTH2("nCandidatesDataWordPt", "Number of MuCTPI candidates from data word vs p_{T}; p_{T} threshold; MuCTPI candidates", 6, 0.5, 6.5, 25, -0.5, 99.5) );
  CHECK( registerTH1("nCandidatesMictpMioct", "Diff between MICTP and MIOCT candidates (same BC); MICTP - MIOCT; Entries", 21, -10.5, 10.5) );
  CHECK( registerTH1("bcidMictpMioct", "BCID diff between MICTP and all MIOCT candidate words; MIOCT - MICTP; Entries", 8, -3.5, 4.5) );
  CHECK( registerTH1("bcidMictpHeader", "BCID Diff between MICTP and ROD header; MICTP - ROD header; Entries", 15, -7.5, 7.5) );
  CHECK( registerTH1("muctpiStatus1", "MuCTPI Status Word 1; Bit; Number of times ON", 24, -0.5, 23.5) );
  CHECK( registerTH1("muctpiStatus2", "MuCTPI Status Word 2; Bit; Number of times ON", 24, -0.5, 23.5) );

  //RoI numbers
  CHECK( registerTH2("barrelRoiSectorID", "MIOCT RoI Number vs Barrel Sector ID (Trigger BC); Sector ID; RoI Number", 64, -0.5, 63.5, 32, -0.5, 31.5) );
  CHECK( registerTH2("endcapRoiSectorID", "MIOCT RoI Number vs Endcap Sector ID (Trigger BC); Sector ID; RoI Number", 96, -0.5, 95.5, 32, -0.5, 31.5) );
  CHECK( registerTH2("forwardRoiSectorID", "MIOCT RoI Number vs Forward Sector ID (Trigger BC); Sector ID; RoI Number", 48, -0.5, 47.5, 32, -0.5, 31.5) );
  CHECK( registerTH2("barrelRoiSectorIDAll", "MIOCT RoI Number vs Barrel Sector ID (All BC); Sector ID; RoI Number", 64, -0.5, 63.5, 32, -0.5, 31.5) );
  CHECK( registerTH2("endcapRoiSectorIDAll", "MIOCT RoI Number vs Endcap Sector ID (All BC); Sector ID; RoI Number", 96, -0.5, 95.5, 32, -0.5, 31.5) );
  CHECK( registerTH2("forwardRoiSectorIDAll", "MIOCT RoI Number vs Forward Sector ID (All BC); Sector ID; RoI Number", 48, -0.5, 47.5, 32, -0.5, 31.5) );

  //Candidate multiplicity within sectors
  CHECK( registerTH2("barrelNCandSectorID", "MIOCT Candidate mult vs Sector ID (barrel); Sector ID; Candidates", 64, -0.5, 63.5, 8, -0.5, 7.5) );
  CHECK( registerTH2("endcapNCandSectorID", "MIOCT Candidate mult vs Sector ID (endcap); Sector ID; Candidates", 96, -0.5, 95.5, 8, -0.5, 7.5) );
  CHECK( registerTH2("forwardNCandSectorID", "MIOCT Candidate mult vs Sector ID (forward); Sector ID; Candidates", 48, -0.5, 47.5, 8, -0.5, 7.5) );

  //Overlap regions
  CHECK( registerTH2("barrelSectorIDOverlapBits", "Barrel Sector ID vs overlap bits; Sector ID; Overlap bits", 64, -0.5, 63.5, 3, 0.5, 3.5) );
  CHECK( registerTH1("endcapSectorIDOverlapBit", "Endcap Sector ID vs number of candidates with overlap; Sector ID; Candidates", 48, -0.5, 47.5) );
  
  //Sector occupancy
  CHECK( registerTH1("barrelSectorID", "Barrel Sector ID (Trigger BC); Sector ID; Candidates", 64, -0.5, 63.5) );
  CHECK( registerTH1("endcapSectorID", "Endcap Sector ID (Trigger BC); Sector ID; Candidates", 96, -0.5, 95.5) );
  CHECK( registerTH1("forwardSectorID", "Forward Sector ID (Trigger BC); Sector ID; Candidates", 48, -0.5, 47.5) );  
  CHECK( registerTH1("barrelSectorIDAll", "Barrel Sector ID (All BC); Sector ID; Candidates", 64, -0.5, 63.5) );
  CHECK( registerTH1("endcapSectorIDAll", "Endcap Sector ID (All BC); Sector ID; Candidates", 96, -0.5, 95.5) );
  CHECK( registerTH1("forwardSectorIDAll", "Forward Sector ID (All BC); Sector ID; Candidates", 48, -0.5, 47.5) );
  
  //Candidate word pT
  CHECK( registerTH1("barrelPt", "Barrel candidate p_{T} (all candidates); p_{T} threshold; Candidates", 6, 0.5, 6.5) );
  CHECK( registerTH1("endcapPt", "Endcap candidate p_{T} (all candidates); p_{T} threshold; Candidates", 6, 0.5, 6.5) );
  CHECK( registerTH1("forwardPt", "Forward candidate p_{T} (all candidates); p_{T} threshold; Candidates", 6, 0.5, 6.5) );

  //Number of candidates in each region
  CHECK( registerTH1("nCandidates_secLoc", "All candidates by sector location; Sector location; Candidates", 3, -0.5, 2.5) );
  
  //Plots for comparing MuCTPI output to output of RPC and TGC SL
  CHECK( registerTH2("muctpiNoRPCCandfound", "MuCTPI BA candidate with no equivalent candidate in RPC SL r/o, Sector ID vs. RoI number", 30, -0.5, 29.5, 64, -0.5, 63.5) );
  CHECK( registerTH2("rpcNoMuCTPICandfound", "RPC SL r/o candidate with no equivalent candidate in MuCTPI r/o, Sector ID vs. RoI number", 30, -0.5, 29.5, 64, -0.5, 63.5) );

  CHECK( registerTH2("muctpiNoTGCecCandfound", "MuCTPI EC candidate with no equivalent candidate in TGC SL r/o, Sector ID vs. RoI number", 150, -0.5, 149.5, 100, -0.5, 99.5) );
  CHECK( registerTH2("tgcecNoMuCTPICandfound", "TGC EC SL r/o candidate with no equivalent candidate in MuCTPI r/o, Sector ID vs. RoI number", 150, -0.5, 149.5, 100, -0.5, 99.5) );
  CHECK( registerTH2("muctpiNoTGCfwCandfound", "MuCTPI FW candidate with no equivalent candidate in TGC SL r/o, Sector ID vs. RoI number", 64, -0.5, 63.5, 50, -0.5, 49.5) );
  CHECK( registerTH2("tgcfwNoMuCTPICandfound", "TGC FW SL r/o candidate with no equivalent candidate in MuCTPI r/o, Sector ID vs. RoI number", 64, -0.5, 63.5, 50, -0.5, 49.5) );

 

  //Fix cosmetics for some plots, i.e. bin labels
  TH1F_LW* nCandidates_secLoc = getTH1("nCandidates_secLoc");
  if (nCandidates_secLoc) {
    nCandidates_secLoc->GetXaxis()->SetBinLabel(1, "Barrel");
    nCandidates_secLoc->GetXaxis()->SetBinLabel(2, "Endcap");
    nCandidates_secLoc->GetXaxis()->SetBinLabel(3, "Forward");
  }

  return StatusCode::SUCCESS;
}

StatusCode
TrigT1CTMonitoring::BSMonitoring::initCtpMuctpiHistograms()
{

  m_dirName=m_baseDirName+"CTPMUCTPI/";

  CHECK( registerTH1("headerL1IdDifference", "ROD header LVL1 ID CTP - MuCTPI; #DeltaL1ID; Entries", 101, -50.5, 50.5) );
  CHECK( registerTH1("headerBCIDDifference", "ROD header BCID CTP - MuCTPI; #DeltaBCID; Entries", 401, -200.5, 200.5) );
  CHECK( registerTH1("bcidDifference", "BCID CTP - MICTP; #DeltaBCID; Entries", 15, -7.5, 7.5) );

  return StatusCode::SUCCESS;
}

StatusCode
TrigT1CTMonitoring::BSMonitoring::initRoIHistograms()
{

  m_dirName=m_baseDirName+"ROI/";

  //RoI eta / phi
  CHECK( registerTH2("roiEtaPhi","Coordinates of all RoIs; #eta; #phi",32,-2.5,2.5,32,-TMath::Pi(),TMath::Pi()) );
  CHECK( registerTH1("roiEta","#eta of all RoI's; #eta; Entries",32,-2.5,2.5) );
  CHECK( registerTH1("roiPhi","#phi of all RoI's; #phi; Entries",32,-TMath::Pi(),TMath::Pi()) );

  //Sector vs. RoI
  CHECK( registerTH2("barrelSectorIDRoi","Sector ID vs. RoI for barrel muons (matched cand only); Sector ID; RoI", 64,-0.5,63.5,32,-0.5,31.5) );
  CHECK( registerTH2("endcapSectorIDRoi","Sector ID vs. RoI for endcap muons (matched cand only); Sector ID; RoI", 96,-0.5,95.5,32,-0.5,31.5) );
  CHECK( registerTH2("forwardSectorIDRoi","Sector ID vs. RoI for forward muons (matched cand only); Sector ID; RoI", 48,-0.5,47.5,32,-0.5,31.5) );
  
  //Sector vs. RoI Eta
  CHECK( registerTH2("barrelSectorIDRoiEta","Sector ID vs. RoI #eta for barrel muons (matched cand only); Sector ID; #eta", 64,-0.5,63.5,32,-2.5,2.5) );
  CHECK( registerTH2("endcapSectorIDRoiEta","Sector ID vs. RoI #eta for endcap muons (matched cand only); Sector ID; #eta", 96,-0.5,95.5,32,-2.5,2.5) );
  CHECK( registerTH2("forwardSectorIDRoiEta","Sector ID vs. RoI #eta for forward muons (matched cand only); Sector ID; #eta", 48,-0.5,47.5,32,-2.5,2.5) );

  //Sector vs. RoI Phi
  CHECK( registerTH2("barrelSectorIDRoiPhi","Sector ID vs. RoI #phi for barrel (matched cand only); Sector ID; #phi", 64,-0.5,63.5,32,-TMath::Pi(),TMath::Pi()) );
  CHECK( registerTH2("endcapSectorIDRoiPhi","Sector ID vs. RoI #phi for endcap (matched cand only); Sector ID; #phi", 96,-0.5,95.5,32,-TMath::Pi(),TMath::Pi()) );
  CHECK( registerTH2("forwardSectorIDRoiPhi","Sector ID vs. RoI #phi for forward (matched cand only); Sector ID; #phi", 48,-0.5,47.5,32,-TMath::Pi(),TMath::Pi()) );
  
  return StatusCode::SUCCESS;
}

void
TrigT1CTMonitoring::BSMonitoring::doMuctpi(const MuCTPI_RDO* theMuCTPI_RDO, const MuCTPI_RIO* theMuCTPI_RIO, 
                                            const RpcSectorLogicContainer* theRPCContainer, 
                                            const Muon::TgcCoinDataContainer* theTGCContainer)
{

  TProfile_LW* errorSummary = getTProfile("errorSummary");
  TH2F_LW* errorSummaryPerLumiBlock = getTH2("errorSummaryPerLumiBlock");
  TH1F_LW* errorPerLumiBlock = getTH1("errorPerLumiBlock");
  TH1F_LW* nCandidates = getTH1("nCandidates");
  TH1F_LW* nCandidatesDataWord = getTH1("nCandidatesDataWord");
  TH1F_LW* pt = getTH1("pt");
  TH1F_LW* muctpiStatus1 = getTH1("muctpiStatus1");
  TH1F_LW* muctpiStatus2 = getTH1("muctpiStatus2");
  TH1F_LW* bcidMictpMioct = getTH1("bcidMictpMioct");
  TH1F_LW* bcidMictpHeader = getTH1("bcidMictpHeader");
  TH1F_LW* nCandidatesMictpMioct = getTH1("nCandidatesMictpMioct");
  TH1F_LW* endcapSectorIDOverlapBit = getTH1("endcapSectorIDOverlapBit");
  TH1F_LW* barrelSectorID = getTH1("barrelSectorID");
  TH1F_LW* endcapSectorID = getTH1("endcapSectorID");
  TH1F_LW* forwardSectorID = getTH1("forwardSectorID");
  TH1F_LW* barrelSectorIDAll = getTH1("barrelSectorIDAll");
  TH1F_LW* endcapSectorIDAll = getTH1("endcapSectorIDAll");
  TH1F_LW* forwardSectorIDAll = getTH1("forwardSectorIDAll");
  TH1F_LW* barrelPt = getTH1("barrelPt");
  TH1F_LW* endcapPt = getTH1("endcapPt");
  TH1F_LW* forwardPt = getTH1("forwardPt");

   if ( !errorSummary || !errorSummaryPerLumiBlock || !errorPerLumiBlock || !nCandidates || !nCandidatesDataWord || !pt || !muctpiStatus1
      || !muctpiStatus2 || !nCandidatesMictpMioct || !bcidMictpMioct || !bcidMictpHeader
      || !endcapSectorIDOverlapBit || !barrelSectorID || !endcapSectorID || !forwardSectorID 
      || !barrelPt || !endcapSectorIDAll || !forwardSectorIDAll || !barrelSectorIDAll 
      || !endcapPt || !forwardPt )
   {
      ATH_MSG_FATAL("Problems finding 1D histograms for MuCTPI!");
      return;
   }
  TH2F_LW *barrelRoiSectorID = getTH2("barrelRoiSectorID");
  TH2F_LW *endcapRoiSectorID = getTH2("endcapRoiSectorID");
  TH2F_LW *forwardRoiSectorID = getTH2("forwardRoiSectorID");
  TH2F_LW *barrelRoiSectorIDAll = getTH2("barrelRoiSectorIDAll");
  TH2F_LW *endcapRoiSectorIDAll = getTH2("endcapRoiSectorIDAll");
  TH2F_LW *forwardRoiSectorIDAll = getTH2("forwardRoiSectorIDAll");
  TH2F_LW *barrelNCandSectorID = getTH2("barrelNCandSectorID");
  TH2F_LW *endcapNCandSectorID = getTH2("endcapNCandSectorID");
  TH2F_LW *forwardNCandSectorID = getTH2("forwardNCandSectorID");
  TH2F_LW *nCandidatesPt = getTH2("nCandidatesPt");
  TH2F_LW *nCandidatesDataWordPt = getTH2("nCandidatesDataWordPt");
  TH2F_LW *barrelSectorIDOverlapBits = getTH2("barrelSectorIDOverlapBits");
  TH2F_LW *muctpiNoRPCCandfound = getTH2("muctpiNoRPCCandfound");
  TH2F_LW *rpcNoMuCTPICandfound = getTH2("rpcNoMuCTPICandfound");
  TH2F_LW *muctpiNoTGCecCandfound = getTH2("muctpiNoTGCecCandfound");
  TH2F_LW *tgcecNoMuCTPICandfound = getTH2("tgcecNoMuCTPICandfound");
  TH2F_LW *muctpiNoTGCfwCandfound = getTH2("muctpiNoTGCfwCandfound");
  TH2F_LW *tgcfwNoMuCTPICandfound = getTH2("tgcfwNoMuCTPICandfound");

  if ( !barrelRoiSectorID || !endcapRoiSectorID || !forwardRoiSectorID || 
       !barrelRoiSectorIDAll || !endcapRoiSectorIDAll || !forwardRoiSectorIDAll || 
       !barrelNCandSectorID || !endcapNCandSectorID || !forwardNCandSectorID || 
       !nCandidatesPt || !barrelSectorIDOverlapBits || !muctpiNoRPCCandfound ||
       !rpcNoMuCTPICandfound || !muctpiNoTGCecCandfound || !tgcecNoMuCTPICandfound ||
       !muctpiNoTGCfwCandfound || !tgcfwNoMuCTPICandfound) {
    ATH_MSG_FATAL("Problems finding 2D histograms for MuCTPI!");
    return;
   }

  // maps for comapring MuCTPI and RPC/TGC candidates
  std::map <std::string,MuCTPI_DataWord_Decoder> muctpiCandidates;
  std::map <std::string, const RpcSLTriggerHit* > rpcCandidates;
  std::map <std::string, const Muon::TgcCoinData* > tgcCandidates;


  // Get the data
  MuCTPI_MultiplicityWord_Decoder multWord(theMuCTPI_RDO->candidateMultiplicity(), m_inclusiveTriggerThresholds);
  MuCTPI_DataWord_Decoder dataWord(0);
  const std::vector<uint32_t> &vDataWords = theMuCTPI_RDO->dataWord();

  /*
   * Number of muon candidates 
   */
  uint16_t numberCandidates = multWord.getNCandidates();
  if (numberCandidates > 0) ++m_filledEventCount;
  nCandidates->Fill(numberCandidates);

  for ( unsigned int i(0); i < LVL1::MuCTPIBits::MULT_THRESH_NUM; ++i ) {
    nCandidatesPt->Fill(i+1u,multWord.getMultiplicity(i));
  }

  /*
   * Consistency check: number of MICTP candidates vs. no of MIOCT
   * candidate words 
   */
  int numberDataWords = 0;
  int ncand[3][96]; //count number of candidates per event (0 barrel, 1 endcap, 2 forward)
  for ( int x = 0; x < 3; x++ ) {
    for ( int y = 0; y < 96; y++ ) {
      ncand[x][y]=0;
    }
  }

  // because of overlap removal (and saturation) the number of data words may be different
  // from the reported candidate multiplicity => also check data words!

  // bookkeeping array to count per pT threshold
  int nCandsPerPtThreshold[6] = {0, 0, 0, 0, 0, 0};

  for ( std::vector<uint32_t>::const_iterator it = vDataWords.begin();
        it != vDataWords.end(); ++it ) {
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

    /*
    if (dataWord.getBCID() == multWord.getBCID()) {
      // Fill the muctpi map for later comparision to RPC and TGC
      muctpiCandidates.insert ( std::pair<std::string,MuCTPI_DataWord_Decoder>(keystring.str(),dataWord) );
    
     //Use only candidates from the same BCID for overlap histograms
      if (dataWord.getOverlapBits()) {
	if (dataWord.getSectorLocation() == MuCTPI_RDO::ENDCAP) {
	  endcapSectorIDOverlapBit->Fill(dataWord.getSectorID()+48*dataWord.getHemisphere());
	} 
	else if (dataWord.getSectorLocation() == MuCTPI_RDO::BARREL) {
	  barrelSectorIDOverlapBits->Fill(dataWord.getSectorID(1)+32*dataWord.getHemisphere(), dataWord.getOverlapBits());
	}
      }
      if (!dataWord.getVetoed()) {
	dataWord.dumpData();
	++numberDataWords;
	// increment counters for all thresholds up to and including the one passed by this candidate
	for (int i = 1; i <= dataWord.getPt(); ++i) {
	  ++nCandsPerPtThreshold[i-1];
	}
      }
    }
*/
  }

  for ( int y = 0; y < 96; y++ ) {
    if ( y < 64 ) barrelNCandSectorID->Fill(y, ncand[0][y]);
    endcapNCandSectorID->Fill(y, ncand[1][y] );
    if ( y < 48 ) forwardNCandSectorID->Fill(y, ncand[2][y]);
  }
  
  // multiplicities based on the data words
  nCandidatesDataWord->Fill(numberDataWords);
  for (int i = 0; i < 6; ++i) {
    nCandidatesDataWordPt->Fill(i+1, nCandsPerPtThreshold[i]);
  }

  if (m_inclusiveTriggerThresholds && (numberDataWords > 7)) numberDataWords = 7;

  int diffNCandidates = (static_cast<int>(numberCandidates) - numberDataWords);

  if (diffNCandidates != 0) {
    ATH_MSG_WARNING( "Number of candidates in multiplicity word " << numberCandidates 
		     << " != number of candidates " << numberDataWords << " from " << vDataWords.size() << " data words");
    // dump multiplicity and data words (DEBUG level)
    multWord.dumpData();
    for ( std::vector<uint32_t>::const_iterator it = vDataWords.begin(); it != vDataWords.end(); ++it ) {
      dataWord.setWord(*it);
      dataWord.dumpData();
    }
    errorSummary->Fill(4,1);
    errorSummaryPerLumiBlock->Fill(m_currentLumiBlock, 4);
    errorPerLumiBlock->Fill(m_currentLumiBlock);
  }
  else {
    errorSummary->Fill(4,0);
  }
  nCandidatesMictpMioct->Fill(diffNCandidates);

  /*
   * BCIDs 
   */
  uint16_t mictpBcid = multWord.getBCID();
  //uint16_t candidateBcid = 0;
  uint16_t headerBcid =
    (theMuCTPI_RIO) ? theMuCTPI_RIO->getHeaderBCID() : 0;
  bcidMictpHeader->Fill(mictpBcid - (headerBcid & 7));

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
      barrelSectorIDAll->Fill(dataWord.getSectorID(1)+32*dataWord.getHemisphere());
      barrelRoiSectorIDAll->Fill(dataWord.getSectorID(1)+32*dataWord.getHemisphere(), dataWord.getRoiNumber());
    }
    else if (dataWord.getSectorLocation() == MuCTPI_RDO::ENDCAP) {
      endcapSectorIDAll->Fill(dataWord.getSectorID()+48*dataWord.getHemisphere());
      endcapRoiSectorIDAll->Fill(dataWord.getSectorID()+48*dataWord.getHemisphere(), dataWord.getRoiNumber());
    }
    else if (dataWord.getSectorLocation() == MuCTPI_RDO::FORWARD) {
      forwardSectorIDAll->Fill(dataWord.getSectorID()+24*dataWord.getHemisphere());
      forwardRoiSectorIDAll->Fill(dataWord.getSectorID()+24*dataWord.getHemisphere(), dataWord.getRoiNumber());
    }

    // Use only non-vetoed candidates from the same BCID for multiplicity calculation
    if ( ! dataWord.getVetoed() ) {
      uint16_t candPt = dataWord.getPt();
      pt->Fill(candPt);
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
        barrelSectorID->Fill(dataWord.getSectorID(1)+32*dataWord.getHemisphere());
        barrelRoiSectorID->Fill(dataWord.getSectorID(1)+32*dataWord.getHemisphere(), dataWord.getRoiNumber());
      }
      else if (dataWord.getSectorLocation() == MuCTPI_RDO::ENDCAP) {
        endcapSectorID->Fill(dataWord.getSectorID()+48*dataWord.getHemisphere());
        endcapRoiSectorID->Fill(dataWord.getSectorID()+48*dataWord.getHemisphere(), dataWord.getRoiNumber());
      }
      else if (dataWord.getSectorLocation() == MuCTPI_RDO::FORWARD) {
        forwardSectorID->Fill(dataWord.getSectorID()+24*dataWord.getHemisphere());
        forwardRoiSectorID->Fill(dataWord.getSectorID()+24*dataWord.getHemisphere(), dataWord.getRoiNumber());
      }
    }
    if (dataWord.getSectorLocation() == MuCTPI_RDO::BARREL) {
      barrelPt->Fill(dataWord.getPt());
    }
    else if (dataWord.getSectorLocation() == MuCTPI_RDO::ENDCAP) {
      endcapPt->Fill(dataWord.getPt());
    }
    else if (dataWord.getSectorLocation() == MuCTPI_RDO::FORWARD) {
      forwardPt->Fill(dataWord.getPt());
    }
  }

  /*
   * Consistency check: MICTP vs MIOCT multiplicity 
   */
  bool anyMismatch = false;
  for ( uint16_t i = 0; i < LVL1::MuCTPIBits::MULT_THRESH_NUM; ++i ) {
    if ( multWord.getMultiplicity(i) != MioctPtCount[i] ) {
      anyMismatch = true;
      ATH_MSG_WARNING( "pT threshold " << i+1 << ": MICTP multiplicity (" << multWord.getMultiplicity(i)
                        << ") not equal MIOCT multiplicity (" << MioctPtCount[i] << ")");
      errorSummary->Fill(5,1);
      errorSummaryPerLumiBlock->Fill(m_currentLumiBlock, 5);
      errorPerLumiBlock->Fill(m_currentLumiBlock);
    }
  }
  if ( !anyMismatch ) {
    errorSummary->Fill(5,0);
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
  if (theMuCTPI_RIO) {
    uint32_t num = theMuCTPI_RIO->getHeaderNumberStatusWords();
    std::vector<uint32_t> vStatus =
      theMuCTPI_RIO->getHeaderStatusWords();
    for ( uint32_t i = 0; i < num; ++i ) {
      if (vStatus[i] == 0) {
        continue;
      }
      TH1F_LW *hist = 0;
      if (i == 0) {
        ATH_MSG_DEBUG( "MuCTPI error status word #" << i << ": 0x" << MSG::hex << vStatus[i] << MSG::dec);
        hist = muctpiStatus1;
      } else if (i == 1) {
        hist = muctpiStatus2;
      } else {
        continue;
      }
      for ( int bit = 0; bit < 24; ++bit ) {
        if (vStatus[i] & (1 << bit))
          hist->Fill(bit);
      }
    }
  }

  const xAOD::EventInfo* eventInfo = SG::get(m_EventInfoKey); 
  if (!eventInfo) {
    ATH_MSG_WARNING( "Could not retrieve EventInfo from StoreGate");
  }

  // Get candidates from TGC and RPC SLs for comparisons

  RpcSectorLogicContainer::const_iterator it_rpc = theRPCContainer->begin();
  for ( ; it_rpc !=theRPCContainer-> end() ; ++it_rpc )
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
	  if (secID == 49 ) secID = 1;
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
      if ( (it_mui->first).substr(0,2) == "BA" )  { 
	int baSecID = (it_mui->second).getSectorID(1)+32*(it_mui->second).getHemisphere();
	int baRoIID = (it_mui->second).getRoiNumber();
	muctpiNoRPCCandfound->Fill(baRoIID,baSecID);
	miRPCmismatch = true;
      } else if ( (it_mui->first).substr(0,2) == "EC" ) {
	int ecSecID = (it_mui->second).getSectorID()+48*(it_mui->second).getHemisphere();
	int ecRoIID = (it_mui->second).getRoiNumber();
	muctpiNoTGCecCandfound->Fill(ecRoIID,ecSecID);
	miTGCmismatch= true;
      }else if ( (it_mui->first).substr(0,2) == "FW" ) {
	int fwSecID = (it_mui->second).getSectorID()+24*(it_mui->second).getHemisphere();
	int fwRoIID = (it_mui->second).getRoiNumber();
	muctpiNoTGCfwCandfound->Fill(fwRoIID,fwSecID);
	miTGCmismatch= true;
      } else {
	ATH_MSG_WARNING( "Invalid string label in MuCTPI to RPC/TGC map: " 
			 << (it_mui->first).substr(0,2));
      }
      ATH_MSG_WARNING( "No Muctpi to RPC/TGC match found: MuCTPI key / MuCTPI BCID: " 
		       << it_mui->first  << " / ");
    }
  }

  if (miRPCmismatch) {
     errorSummary->Fill(17,1);
     errorSummaryPerLumiBlock->Fill(m_currentLumiBlock, 17);
     errorPerLumiBlock->Fill(m_currentLumiBlock);
  } else {
    errorSummary->Fill(17,0);
  }
  if (miTGCmismatch) {
     errorSummary->Fill(18,1);
     errorSummaryPerLumiBlock->Fill(m_currentLumiBlock, 18);
     errorPerLumiBlock->Fill(m_currentLumiBlock);
  } else {
    errorSummary->Fill(18,0);
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
      rpcNoMuCTPICandfound->Fill(roiID,secID);
      ATH_MSG_WARNING( "No RPC to Muctpi  match found: RPC key / RPC BCID: " 
		       << it_rpc->first << " / " <<  it_rpc->second->rowinBcid());
      rpcMImismatch =true;
    }
  }
  if (rpcMImismatch) {
     errorSummary->Fill(19,1);
     errorSummaryPerLumiBlock->Fill(m_currentLumiBlock, 19);
     errorPerLumiBlock->Fill(m_currentLumiBlock);
  } else {
    errorSummary->Fill(19,0);
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
      if ( (it_tgc->first).substr(0,2) == "EC" ) {
	int ecRoIID = (it_tgc->second)->roi();
	tgcecNoMuCTPICandfound->Fill(ecRoIID,secID);
      }else if ( (it_tgc->first).substr(0,2) == "FW" ) {
	int fwRoIID = (it_tgc->second)->roi();
	tgcfwNoMuCTPICandfound->Fill(fwRoIID,secID);
      } else {
	ATH_MSG_WARNING( "Invalid string label in TGC to MuCTPI map: " 
			 << (it_tgc->first).substr(0,2));
      }
      ATH_MSG_WARNING( "No TGC to Muctpi match found: TGC key: " << it_tgc->first);
      tgcMImismatch =true;
    }
  }
  if (tgcMImismatch) {
     errorSummary->Fill(20,1);
     errorSummaryPerLumiBlock->Fill(m_currentLumiBlock, 20);
     errorPerLumiBlock->Fill(m_currentLumiBlock);
  } else {
    errorSummary->Fill(20,0);
  }
}

void
TrigT1CTMonitoring::BSMonitoring::doCtp(const CTP_RDO* theCTP_RDO,const CTP_RIO* theCTP_RIO)
{
   TProfile_LW *errorSummary = getTProfile("errorSummary");
   TH1F_LW *errorPerLumiBlock = getTH1("errorPerLumiBlock");
   TH1F_LW *deltaBcid = getTH1("deltaBcid");
   TH1F_LW *triggerType = getTH1("triggerType");
   TH1F_LW *timeSinceLBStart = getTH1("timeSinceLBStart");
   TH1F_LW *timeUntilLBEnd = getTH1("timeUntilLBEnd");
   TH1F_LW *timeSinceL1A = getTH1("timeSinceL1A");
   TH1F_LW *tcTimeError = getTH1("turnCounterTimeError");
   TProfile_LW *tcTimeErrorVsLb = getTProfile("turnCounterTimeErrorVsLb");
   TH1F_LW *ctpStatus1 = getTH1("ctpStatus1");
   TH1F_LW *ctpStatus2 = getTH1("ctpStatus2");
   TH1F_LW *tav = getTH1("tav");

   if ( !errorSummary || !errorPerLumiBlock || !deltaBcid || !triggerType || !timeSinceLBStart || 
        !timeUntilLBEnd || !timeSinceL1A || !tcTimeError || !tcTimeErrorVsLb || !ctpStatus1 || !tav || !ctpStatus2 ) {
      ATH_MSG_FATAL("Problems finding 1D histograms for CTP!");
      return;
   }

   TH2F_LW* errorSummaryPerLumiBlock = getTH2("errorSummaryPerLumiBlock");
   TH2F_LW *pitBC = getTH2("pitBC");
   TH2F_LW *pitFirstBC = getTH2("pitFirstBC");

   if ( !errorSummaryPerLumiBlock || !pitBC || !pitFirstBC ) {
      ATH_MSG_FATAL("Problems finding 2D histograms for CTP!");
      return;
   }

   uint32_t evId = 0;
   uint32_t headerBcid = 0;
   int ttype=0;

   CTP_Decoder ctp;
   ctp.setRDO(theCTP_RDO);

   if (theCTP_RIO) {
      evId = theCTP_RIO->getLvl1Id();
      ttype = theCTP_RIO->getLvl1TriggerType();
      triggerType->Fill(ttype);

      headerBcid = (theCTP_RIO->getBCID() & 0xf);

      // check that the LB number is the same in the EventInfo and the CTP_RIO. TODO: add error for this?
      if (m_currentLumiBlock != (theCTP_RIO->getDetectorEventType() & 0xffff)) {
         ATH_MSG_WARNING( "LB number in EventInfo (" << m_currentLumiBlock 
                          << ") does not match the one in the CTP_RIO object (" 
                          << (theCTP_RIO->getDetectorEventType() & 0xffff) << ")");
      }
    
      //ATH_MSG_DEBUG( getEventInfoString());

      if (m_currentLumiBlock > m_maxLumiBlock) m_maxLumiBlock = m_currentLumiBlock;
      if (m_currentLumiBlock < 1 || 
          (m_retrievedLumiBlockTimes && (find(m_lumiBlocks.begin(), m_lumiBlocks.end(), m_currentLumiBlock) == m_lumiBlocks.end()))) {
         ATH_MSG_WARNING( "Invalid lumi block: " << m_currentLumiBlock);
         errorSummary->Fill(9,1);
         errorSummaryPerLumiBlock->Fill(m_currentLumiBlock, 9);
         errorPerLumiBlock->Fill(m_currentLumiBlock);
      }
      else {
         errorSummary->Fill(9,0);
         if (m_retrievedLumiBlockTimes) {
            uint64_t eventTime = static_cast<uint64_t>(theCTP_RDO->getTimeSec()*1e09 + theCTP_RDO->getTimeNanoSec());
            if (eventTime < m_lbStartTimes[m_currentLumiBlock] || eventTime > m_lbEndTimes[m_currentLumiBlock]) {
               ATH_MSG_WARNING( "Event time (" << eventTime 
                                << ") not within time interval for current lumi block (LB: " << m_currentLumiBlock 
                                << ", start: " <<  m_lbStartTimes[m_currentLumiBlock] 
                                << ", stop: " << m_lbEndTimes[m_currentLumiBlock] << ")");
               errorSummary->Fill(10,1);
               errorSummaryPerLumiBlock->Fill(m_currentLumiBlock, 10);
               errorPerLumiBlock->Fill(m_currentLumiBlock);
            }
            else {
               errorSummary->Fill(10,0);
               timeSinceLBStart->Fill((eventTime-m_lbStartTimes[m_currentLumiBlock])/1e06);
               timeUntilLBEnd->Fill((m_lbEndTimes[m_currentLumiBlock]-eventTime)/1e06);
            }

            // turn counter monitoring - first store turn counter, bcid and times for the first processed event

            // use the best available bunch-crossing interval
            double bcDurationInNs = m_defaultBcIntervalInNs;
            double freqFromCool = getFrequencyMeasurement(eventTime);
            if (freqFromCool > 40.078e6 && freqFromCool < 40.079e6) { // f prop. to beta, ok from HI injection to pp @ sqrt(s) = 14 TeV
               // use average frequency since start of LB
               double lbStartFreqFromCool = getFrequencyMeasurement(m_lbStartTimes[m_currentLumiBlock]);
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
               ATH_MSG_DEBUG( "No valid frequency measurements found in COOL, will use hardcoded BC interval: t_BC = " 
                              << bcDurationInNs << " ns"); //TODO: make this a WARNING when everything is in place in COOL
            }

            // set the reference variables for the turn counter monitoring if this is the first processed event of the run/LB
            if ( !m_eventCount || (m_lumiBlockOfPreviousEvent != 0 && m_lumiBlockOfPreviousEvent != m_currentLumiBlock) ) {
               m_firstEventTime = eventTime;
               m_firstEventBcid = static_cast<int64_t>(theCTP_RIO->getBCID());
               m_firstEventTC = static_cast<int64_t>(theCTP_RDO->getTurnCounter());
            }

            // calculate the time passed since the first processed event, based on GPS clock and turn counter+bcid
            int64_t timeDiff_GPS = eventTime - m_firstEventTime; 
            int64_t firstEventTimeInBc_TC = m_firstEventTC*m_bcsPerTurn+m_firstEventBcid;
            int64_t eventTimeInBc_TC = static_cast<int64_t>(theCTP_RDO->getTurnCounter())*m_bcsPerTurn+theCTP_RIO->getBCID();
            int64_t timeDiffInBc_TC = eventTimeInBc_TC-firstEventTimeInBc_TC;

            // fill turn counter monitoring plots if at least one of first and current turn-counter values are non-zero
            if ( !(m_firstEventTC == 0 && theCTP_RDO->getTurnCounter() == 0) ) {
               BeamMode bm = getBeamMode(eventTime);
               double tDiffInNs = timeDiffInBc_TC*bcDurationInNs-timeDiff_GPS;
               // flag an error if the offset for the timestamp calculated from TC+BCID is off by > half an LHC turn
               // (if we're in STABLE BEAMS and did not just transition to ATLAS_READY in this LB)
               if ( (bm == STABLEBEAMS) && 
                    !((m_dataTakingMode.find(m_currentLumiBlock) != m_dataTakingMode.end()) &&
                      (m_dataTakingMode[m_currentLumiBlock] == true)) && 
                    (fabs(tDiffInNs) > 45000) ) { 
                  ATH_MSG_WARNING( "Turn-counter based time off by " << tDiffInNs 
                                   << " ns (> 0.5 LHC turn) during stable beams - missing orbit pulse?");
                  errorSummary->Fill(16,1);
                  errorSummaryPerLumiBlock->Fill(m_currentLumiBlock, 16);
                  errorPerLumiBlock->Fill(m_currentLumiBlock);
               }
               else {
                  errorSummary->Fill(16,0);
               }
               tcTimeError->Fill(tDiffInNs/1e03);
               tcTimeErrorVsLb->Fill(m_currentLumiBlock, tDiffInNs/1e03);
            }
            else {
               ATH_MSG_DEBUG( "Turn counter = 0 for both first processed and current event, not filling TC histograms");
            }
         }
      }
   }

   if (theCTP_RDO->getTimeNanoSec() > 1e09) {
      ATH_MSG_WARNING( "Nanosecond timestamp too large: " << theCTP_RDO->getTimeNanoSec());
      errorSummary->Fill(11,1);
      errorSummaryPerLumiBlock->Fill(m_currentLumiBlock, 11);
      errorPerLumiBlock->Fill(m_currentLumiBlock);
   } else {
      errorSummary->Fill(11,0); 
   }


   std::vector<uint32_t> vec=theCTP_RDO->getEXTRAWords();
   timeSinceL1A->Fill(theCTP_RDO->getTimeSinceLastL1A()*25*10E-6);

   uint32_t numberBC = theCTP_RDO->getNumberOfBunches();
   if (numberBC > 0) {
      unsigned int storeBunch = theCTP_RDO->getL1AcceptBunchPosition();
      const std::vector<CTP_BC> &BCs = ctp.getBunchCrossings();
      const CTP_BC & bunch = BCs[storeBunch];
      unsigned int bcid = bunch.getBCID();

      double bcid_offset = (static_cast < int >((bcid)&0xf) - static_cast < int >(headerBcid));
      deltaBcid->Fill(bcid_offset);
      if (bcid_offset != 0) {
         if (!m_runOnESD) {
            ATH_MSG_WARNING( "Found BCID offset of "<< bcid_offset << " between ROD Header (" 
                             << headerBcid << ") and data (" << (bcid&0xf) << ")");
            errorSummary->Fill(1,1);
            errorSummaryPerLumiBlock->Fill(m_currentLumiBlock, 1);
            errorPerLumiBlock->Fill(m_currentLumiBlock);
         }
      }
      else errorSummary->Fill(1,0);
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

      m_tbpItems.clear();
      m_tapItems.clear();
      m_tavItems.clear();
      m_tbpBC.clear();
      m_tapBC.clear();
      m_tavBC.clear();

      int minbc=bunchIndex;
      for ( std::vector<CTP_BC>::const_iterator it = BCs.begin();
            it != BCs.end(); ++it, ++bunchIndex ) {
         bcid = it->getBCID();

         const std::bitset<512> currentTIP(it->getTIP());
      
         if (currentTIP.any()) {
            for ( size_t tipNum = 0; tipNum < currentTIP.size(); ++tipNum ) {
               if (currentTIP.test(tipNum)) {
                  pitBC->Fill(tipNum, bunchIndex);
                  if (TIPfirst.test(tipNum)) {
                     TIPfirst.set(tipNum,0);
                     pitFirstBC->Fill(tipNum, bunchIndex);
                  }
               }
            }
         }

         const std::bitset<512> currentTBP(it->getTBP());

         if (currentTBP.any()) {
            for ( size_t item = 0; item < currentTBP.size(); ++item ) {
               if (currentTBP.test(item)) {
                  m_tbpItems.push_back(item);
                  m_tbpBC.push_back(bunchIndex);
                  if (TBPfirst.test(item)) {
                     TBPfirst.set(item,0);
                  }
               }
            }
         }
      
         if ( (!bunchIndex) && (m_compareRerun) ) {//gives position of L1A
            StatusCode sc = compareRerun(*it);
            if ( sc.isFailure() ) {
               ATH_MSG_WARNING( "compareRerun() returned failure");
            }
         }

         const std::bitset<512> currentTAP(it->getTAP());

         if (currentTAP.any()) {
            for ( size_t item = 0; item < currentTAP.size(); ++item ) {
               if (currentTAP.test(item)) {
                  m_tapItems.push_back(item);
                  m_tapBC.push_back(bunchIndex);
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
                  tav->Fill(item);
                  m_tavItems.push_back(item);
                  m_tavBC.push_back(bunchIndex);
               }
            }
         }
      }
      int maxbc=bunchIndex-1;

      bool allTAPFine=true;
      bool allTAVFine=true;
      for ( unsigned int i=0; i<m_tapItems.size(); i++ ) {
         ATH_MSG_DEBUG( m_tapItems.at(i) << " TAP fired at BC " << m_tapBC.at(i));
         bool isTBP=false;
         for ( unsigned int j=0; j<m_tbpItems.size() && isTBP==false; j++ ) {
            if ( m_tbpItems.at(j)==m_tapItems.at(i) && m_tbpBC.at(j)==m_tapBC.at(i) ) isTBP=true;
         }
         if ( isTBP==false ) {
            allTAPFine=false;
            ATH_MSG_WARNING( "TAP item " << m_tapItems.at(i) << " at BC " << m_tapBC.at(i) << " not found in TBP");
         }
      }
      for ( unsigned int i=0; i<m_tavItems.size(); i++ ) {
         ATH_MSG_DEBUG( m_tavItems.at(i) << " TAV fired at BC " << m_tavBC.at(i));
         bool isTAP=false;
         for ( unsigned int j=0; j<m_tapItems.size() && isTAP==false; j++ ) {
            if ( m_tapItems.at(j)==m_tavItems.at(i) && m_tapBC.at(j)==m_tavBC.at(i) ) isTAP=true;
         }
         if ( isTAP==false ) {
            allTAVFine=false;
            ATH_MSG_WARNING( "TAV item " << m_tavItems.at(i) << " at BC " << m_tavBC.at(i) << " not found in TAP");
         }
      }

      //Fill Error Hist
      if (allTAPFine==false) {
         errorSummary->Fill(12,1);
         errorSummary->Fill(m_currentLumiBlock, 12);
         errorPerLumiBlock->Fill(m_currentLumiBlock);
      }
      else errorSummary->Fill(12,0);

      if (allTAVFine==false) {
         errorSummary->Fill(13,1);
         errorSummaryPerLumiBlock->Fill(m_currentLumiBlock, 13);
         errorPerLumiBlock->Fill(m_currentLumiBlock);
      }
      else errorSummary->Fill(13,0);

      m_lastminbc=minbc;
      m_lastmaxbc=maxbc;

      // TODO: is this really the check we want to do? Doesn't offline in general have more resources..? /CO
      if (m_environment==AthenaMonManager::online)
         updateRangeUser();//Triggers LW->ROOT conversion so should certainly not be done offline

      errorSummary->Fill(2,0);

      std::vector<unsigned int> triggersFired = ctp.getAllTriggers(storeBunch);
      std::stringstream str;
      for ( unsigned int i = 0; i < triggersFired.size(); ++i ) {
         str << triggersFired[i] << " ";
      }
      ATH_MSG_DEBUG( triggersFired.size() << " trigger items fired: " << str.str());
   } 
   else {
      if (!m_runOnESD) {
         ATH_MSG_WARNING( "Zero bunches in CTP data for ext. LVL1 ID 0x" << MSG::hex << evId << MSG::dec);
         errorSummary->Fill(2,1);
         errorSummary->Fill(m_currentLumiBlock, 2);
         errorPerLumiBlock->Fill(m_currentLumiBlock);
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
            TH1F_LW *hist = 0;
            if (i == 0) {
               ATH_MSG_DEBUG( "CTP error status word #" << i << ": 0x" << MSG::hex << vStatus[i] << MSG::dec);
               hist = ctpStatus1;
            } else if (i == 1) {
               hist = ctpStatus2;
            } else {
               continue;
            }
            for ( int bit = 0; bit < 24; ++bit ) {
               if (vStatus[i] & (1 << bit))
                  hist->Fill(bit);
            }
         }
      }
   }
}

void
TrigT1CTMonitoring::BSMonitoring::updateRangeUser()
{
  //Make 2D plots readable
  if (m_lastminbc!=m_lastmaxbc) {
    TH2F_LW* h;
    if ((h=getTH2("pitBC"))) h->getROOTHist()->GetYaxis()->SetRangeUser(m_lastminbc, m_lastmaxbc);
    if ((h=getTH2("pitFirstBC"))) h->getROOTHist()->GetYaxis()->SetRangeUser(m_lastminbc, m_lastmaxbc);
  }
  //LB plot
  TH1F_LW* h1;
  if ((h1=getTH1("errorPerLumiBlock"))) h1->getROOTHist()->GetXaxis()->SetRangeUser(-0.5, m_maxLumiBlock+0.5);
}

void
TrigT1CTMonitoring::BSMonitoring::doCtpMuctpi( const CTP_RDO* theCTP_RDO, const CTP_RIO* theCTP_RIO,
                                                const MuCTPI_RDO* theMuCTPI_RDO, const MuCTPI_RIO* theMuCTPI_RIO)
{
  TProfile_LW *errorSummary = getTProfile("errorSummary");
  TH2F_LW* errorSummaryPerLumiBlock = getTH2("errorSummaryPerLumiBlock");
  TH1F_LW *errorPerLumiBlock = getTH1("errorPerLumiBlock");
  TH1F_LW *headerL1IdDifference = getTH1("headerL1IdDifference");
  TH1F_LW *headerBCIDDifference = getTH1("headerBCIDDifference");
  TH1F_LW *bcidDifference = getTH1("bcidDifference");
  if ( !errorSummary || !errorSummaryPerLumiBlock || !errorPerLumiBlock || !headerL1IdDifference || !headerBCIDDifference || !bcidDifference ) {
    ATH_MSG_FATAL("Problems finding histograms for CTP_MuCTPI!");
    return;
  }
  if (theCTP_RIO && theMuCTPI_RIO) {
    uint32_t ctp_evid = theCTP_RIO->getLvl1Id();
    uint32_t ctp_bcid = (theCTP_RIO->getBCID() & 0xf);
    uint32_t muctpi_evid = theMuCTPI_RIO->getHeaderLVL1ID();
    uint32_t muctpi_bcid = theMuCTPI_RIO->getHeaderBCID();
    int diffValBcid = (static_cast < int >(ctp_bcid) -
		       static_cast < int >((muctpi_bcid)&0xf));
    int diffValEvid = static_cast < int >(ctp_evid) -
      static_cast < int >(muctpi_evid);
    headerL1IdDifference->Fill(diffValEvid);
    headerBCIDDifference->Fill(diffValBcid);
    if (diffValBcid!=0) {
      ATH_MSG_WARNING( "BCID mismatch between CTP and MuCTPI RIOs, filling error histograms");
      errorSummary->Fill(3,1);
      errorSummaryPerLumiBlock->Fill(m_currentLumiBlock, 3);
      errorPerLumiBlock->Fill(m_currentLumiBlock);
    }
    else errorSummary->Fill(3,0);

    if (diffValBcid < headerBCIDDifference->getXMin()
	|| diffValBcid > headerBCIDDifference->getXMax()) {
      ATH_MSG_WARNING( "BCID difference out of range. CTP_RODHeader_BCID: "
		       << ctp_bcid << " MuCTPI_RODHeader_BCID: " << (muctpi_bcid));      
    }
  }
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
    bcidDifference->Fill(static_cast < int >(mictp_bcid) -
			 static_cast < int >(bunch.getBCID() & 7)); 
  }
}

void
TrigT1CTMonitoring::BSMonitoring::doMuonRoI( const MuCTPI_RDO* theMuCTPI_RDO, const MuCTPI_RIO* theMuCTPI_RIO,
                                             const ROIB::RoIBResult* roib)
{
   TProfile_LW *errorSummary = getTProfile("errorSummary");
   TH2F_LW* errorSummaryPerLumiBlock = getTH2("errorSummaryPerLumiBlock");
   TH1F_LW *errorPerLumiBlock = getTH1("errorPerLumiBlock");
   TH1F_LW *roiEta = getTH1("roiEta");
   TH1F_LW *roiPhi = getTH1("roiPhi");
   TH1F_LW *nCandidates_secLoc = getTH1("nCandidates_secLoc");

   if ( !errorSummary || !errorSummaryPerLumiBlock || !errorPerLumiBlock || !roiEta || !roiPhi || !nCandidates_secLoc ) {
      ATH_MSG_FATAL("Problems finding 1D histograms for RoI!");
      return;
   }
   TH2F_LW *roiEtaPhi = getTH2("roiEtaPhi");
   TH2F_LW *barrelSectorIDRoi = getTH2("barrelSectorIDRoi");
   TH2F_LW *endcapSectorIDRoi = getTH2("endcapSectorIDRoi");
   TH2F_LW *forwardSectorIDRoi = getTH2("forwardSectorIDRoi");
   TH2F_LW *barrelSectorIDRoiEta = getTH2("barrelSectorIDRoiEta");
   TH2F_LW *endcapSectorIDRoiEta = getTH2("endcapSectorIDRoiEta");
   TH2F_LW *forwardSectorIDRoiEta = getTH2("forwardSectorIDRoiEta");
   TH2F_LW *barrelSectorIDRoiPhi = getTH2("barrelSectorIDRoiPhi");
   TH2F_LW *endcapSectorIDRoiPhi = getTH2("endcapSectorIDRoiPhi");
   TH2F_LW *forwardSectorIDRoiPhi = getTH2("forwardSectorIDRoiPhi");
  if ( !roiEtaPhi || !barrelSectorIDRoi || !endcapSectorIDRoi || !forwardSectorIDRoi || 
       !barrelSectorIDRoiEta || !endcapSectorIDRoiEta || !forwardSectorIDRoiEta || 
       !barrelSectorIDRoiPhi || !endcapSectorIDRoiPhi || !forwardSectorIDRoiPhi ) {
      ATH_MSG_FATAL("Problems finding 2D histograms for RoI!");
    return;
  }

   const std::vector<ROIB::MuCTPIRoI> roiVec = roib->muCTPIResult().roIVec();
   int bcidMismatch = 0;
   uint16_t bcId;
   uint16_t pTval;
   uint16_t pTnum;
   double eta;
   double phi;
   uint16_t secID;
   uint16_t sysID;
   uint16_t hemisphere;
   uint16_t roInum;
   bool accepted;
   bool first;
   bool duplicatedRoI;
   bool duplicatedSector;
   int candNum=0;

  if (theMuCTPI_RIO) {
    for ( int i = 0; i < theMuCTPI_RIO->getNRoI(); i++ ) {
      if (!theMuCTPI_RIO->getRoI(i, bcId, pTval, pTnum, eta, phi, secID, sysID, hemisphere, 
         roInum, accepted, first, duplicatedRoI, duplicatedSector)) {
      ATH_MSG_WARNING( "RoI with index " << i  << " not found, skipping this RoI");
      continue;
         }
         roiEtaPhi->Fill(eta, phi);
         roiEta->Fill(eta);
         roiPhi->Fill(phi);
         if (theMuCTPI_RIO->getBCID() != bcId) bcidMismatch++;
      }

    for ( unsigned int j = 0; j < roiVec.size(); j++ ) {
      bool isCand = false;
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
      errorSummary->Fill(7,1);
      errorSummaryPerLumiBlock->Fill(m_currentLumiBlock, 7);
      errorPerLumiBlock->Fill(m_currentLumiBlock);
         }
         else {
      errorSummary->Fill(7,0);
      theMuCTPI_RIO->getRoI(candNum, bcId, pTval, pTnum, eta, phi, secID, sysID, hemisphere, 
                  roInum, accepted, first, duplicatedRoI, duplicatedSector);
      if (theMuCTPI_RDO) {// go back to data words to check sector locations
      MuCTPI_DataWord_Decoder dataWord(0);
      const std::vector<uint32_t> &vDataWords = theMuCTPI_RDO->dataWord();
      for ( std::vector<uint32_t>::const_iterator it = vDataWords.begin(); it != vDataWords.end(); ++it ) {
         dataWord.setWord(*it);

         // MuCTPI_DataWord_Decoder only corrects sector ID for hemisphere if candidate is barrel
         // correct back for this by providing an argument to getSectorID
         uint16_t sectorID = dataWord.getSectorID();
         if (dataWord.getSectorLocation() == MuCTPI_RDO::BARREL)
            sectorID=dataWord.getSectorID(1);
         
         if (//(dataWord.getBCID() == theMuCTPI_RIO->getBCID())
         //&& 
         (sectorID == secID)
         && (dataWord.getRoiNumber() == roInum)
         && (dataWord.getSectorLocation() == sysID)
         && (dataWord.getHemisphere() == hemisphere)) {

            nCandidates_secLoc->Fill(dataWord.getSectorLocation());
            
            if (dataWord.getSectorLocation() == MuCTPI_RDO::BARREL) {
         barrelSectorIDRoi->Fill(dataWord.getSectorID(1)+32*dataWord.getHemisphere(), roInum); 
         barrelSectorIDRoiEta->Fill(dataWord.getSectorID(1)+32*dataWord.getHemisphere(), eta);
         barrelSectorIDRoiPhi->Fill(dataWord.getSectorID(1)+32*dataWord.getHemisphere(), phi);
            }
            else if (dataWord.getSectorLocation() == MuCTPI_RDO::ENDCAP) {
         endcapSectorIDRoi->Fill(dataWord.getSectorID()+48*dataWord.getHemisphere(), roInum);
         endcapSectorIDRoiEta->Fill(dataWord.getSectorID()+48*dataWord.getHemisphere(), eta);
         endcapSectorIDRoiPhi->Fill(dataWord.getSectorID()+48*dataWord.getHemisphere(), phi);
            }
            else if (dataWord.getSectorLocation() == MuCTPI_RDO::FORWARD) {
         forwardSectorIDRoi->Fill(dataWord.getSectorID()+24*dataWord.getHemisphere(), roInum);
         forwardSectorIDRoiEta->Fill(dataWord.getSectorID()+24*dataWord.getHemisphere(), eta);
         forwardSectorIDRoiPhi->Fill(dataWord.getSectorID()+24*dataWord.getHemisphere(), phi);
            }
         }
      }
      }
         }
      }

      int lvl2Expected = static_cast<int>(theMuCTPI_RIO->getNRoI()) - bcidMismatch;
      if (lvl2Expected != static_cast<int>(roiVec.size()) && lvl2Expected <= 14) {
         ATH_MSG_WARNING( "Expected " << lvl2Expected
      << " LVL2 RoIs, but found " << roiVec.size()
      << "!");
         errorSummary->Fill(6,1);
         errorSummaryPerLumiBlock->Fill(m_currentLumiBlock, 6);
         errorPerLumiBlock->Fill(m_currentLumiBlock);
      }
      else if (static_cast<int>(roiVec.size()) != 14 && lvl2Expected >= 14) {
         ATH_MSG_WARNING( "Expected 14 RoI's from " << lvl2Expected
      << " MuCTPI RoI's, but found " << roiVec.size()
      << "!");
         errorSummary->Fill(6,1);
         errorSummaryPerLumiBlock->Fill(m_currentLumiBlock, 6);
         errorPerLumiBlock->Fill(m_currentLumiBlock);
      }
      else errorSummary->Fill(6,0);
   }
}

StatusCode
TrigT1CTMonitoring::BSMonitoring::compareRerun(const CTP_BC &bunchCrossing)
{
   bool itemMismatch{false};

   TProfile_LW *errorSummary = getTProfile("errorSummary");
   TH2F_LW* errorSummaryPerLumiBlock = getTH2("errorSummaryPerLumiBlock");
   TH1F_LW *errorPerLumiBlock = getTH1("errorPerLumiBlock");
   TH1F_LW* l1ItemsBPSimMismatch = getTH1("l1ItemsBPSimMismatch");
   TH1F_LW* l1ItemsBPSimMismatchItems = getTH1("l1ItemsBPSimMismatchItems");

   const CTP_RDO* theCTP_RDO_Rerun = nullptr;
   ATH_MSG_DEBUG( "Retrieving CTP_RDO from SG with key CTP_RDO_Rerun");
   CHECK( (theCTP_RDO_Rerun = SG::get(m_CTP_RDO_RerunKey)) != nullptr );
   
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
         bool skip = item.definition().find("RNDM") != string::npos;
         for(const std::string & p : m_ignorePatterns) {
            if(item.name().find(p) != string::npos) {
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
            l1ItemsBPSimMismatch->Fill(item.ctpId(),1);
            l1ItemsBPSimMismatchItems->getROOTHist()->Fill( (item.name()).c_str(), 1 );
         }
      }
   }

   if (itemMismatch) {
      ATH_MSG_WARNING( "Mismatch between CTP data and simulation in BC " << bunchCrossing.getBCID());
      errorSummary->Fill(14,1);
      errorSummaryPerLumiBlock->Fill(m_currentLumiBlock, 14);
      errorPerLumiBlock->Fill(m_currentLumiBlock);
   } else {
      errorSummary->Fill(14,0);
   }
   return StatusCode::SUCCESS;
}

void
TrigT1CTMonitoring::BSMonitoring::dumpData( const CTP_RDO* theCTP_RDO, const CTP_RIO* theCTP_RIO,
                                            const MuCTPI_RDO* theMuCTPI_RDO, const MuCTPI_RIO* theMuCTPI_RIO,
                                            const ROIB::RoIBResult* roib)
{
   if ( !msgLvl(MSG::DEBUG) )
      return;
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
    std::vector<uint32_t>::const_iterator it =
      theMuCTPI_RDO->dataWord().begin();
    int count = 1;
    for ( ; it != theMuCTPI_RDO->dataWord().end(); ++it ) {
      ATH_MSG_DEBUG( "Candidate " << count);
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
    if ( theCTP_RIO )
      theCTP_RIO->dumpData();
    if ( theMuCTPI_RIO )
      theMuCTPI_RIO->dumpData();
  }

  if ( roib )
    roib->dumpData();
}

//------------
StatusCode
TrigT1CTMonitoring::BSMonitoring::registerTH1(const std::string& histName, const std::string& histTitle, int nbins, float minX, float maxX)
{
  HistMap_t::iterator it = m_histNames.find(histName);
  if (it != m_histNames.end()) {
    ATH_MSG_ERROR( "Histogram " << histName << " already registered");
    return StatusCode::FAILURE;
  }
  TH1F_LW *hist = TH1F_LW::create(histName.c_str(), histTitle.c_str(), nbins, minX, maxX);
  m_histNames[histName] = hist;
  return regHist(hist, m_dirName, run);
}

//------------
StatusCode
TrigT1CTMonitoring::BSMonitoring::registerTProfile(const std::string& histName, const std::string& histTitle, int nbins, float minX, float maxX, float minY, float maxY)
{
  HistMap_t::iterator it = m_histNames.find(histName);
  if (it != m_histNames.end()) {
    ATH_MSG_ERROR( "Histogram " << histName << " already registered");
    return StatusCode::FAILURE;
  }
  TProfile_LW *hist = TProfile_LW::create(histName.c_str(), histTitle.c_str(), nbins, minX, maxX, minY, maxY);
  m_histNames[histName] = hist;
  return regHist(hist, m_dirName, run);
}

//------------
StatusCode
TrigT1CTMonitoring::BSMonitoring::registerTH2( const std::string& histName, const std::string& histTitle, int nbinsX,
                                                float minX, float maxX, int nbinsY, float minY, float maxY)
{
  HistMap_t::iterator it = m_histNames.find(histName);
  if (it != m_histNames.end()) {
    ATH_MSG_ERROR( "Histogram " << histName << " already registered");
    return StatusCode::FAILURE;
  }
  TH2F_LW *hist = TH2F_LW::create(histName.c_str(), histTitle.c_str(), nbinsX, minX, maxX, nbinsY, minY, maxY);
  m_histNames[histName] = hist;
  return regHist(hist, m_dirName, run);
}

//------------
TH1F_LW *
TrigT1CTMonitoring::BSMonitoring::getTH1(const char* histLabel) 
{
  m_tmpstr = histLabel;
  HistMap_t::iterator it = m_histNames.find(m_tmpstr);
  if (it == m_histNames.end()) {
    ATH_MSG_ERROR( "Histogram under " << histLabel << " not found ");
    return 0;
  }
  TH1F_LW* h = dynamic_cast<TH1F_LW*>(it->second);
  if (!h)
    ATH_MSG_ERROR( "Problems converting histogram " << histLabel << " to TH1F_LW ");
  return h;
}

//------------
TH2F_LW *
TrigT1CTMonitoring::BSMonitoring::getTH2(const char* histLabel)
{
  m_tmpstr = histLabel;
  HistMap_t::iterator it = m_histNames.find(m_tmpstr);
  if (it == m_histNames.end()) {
    ATH_MSG_ERROR( "Histogram under " << histLabel << " not found ");
    return 0;
  }
  TH2F_LW* h = dynamic_cast<TH2F_LW*>(it->second);
  if (!h)
    ATH_MSG_ERROR( "Problems converting histogram " << histLabel << " to TH2F_LW ");
  return h;
}

//------------
TProfile_LW *
TrigT1CTMonitoring::BSMonitoring::getTProfile(const char* histLabel)
{
  m_tmpstr = histLabel;
  HistMap_t::iterator it = m_histNames.find(m_tmpstr);
  if (it == m_histNames.end()) {
    ATH_MSG_ERROR( "Histogram under " << histLabel << " not found ");
    return 0;
  }
  TProfile_LW* h = dynamic_cast<TProfile_LW*>(it->second);
  if (!h)
    ATH_MSG_ERROR( "Problems converting histogram " << histLabel << " to TProfile_LW ");
  return h;
}

//------------
void
TrigT1CTMonitoring::BSMonitoring::getCoolData(unsigned int runNumber)
{

  // establish a connection to COOLONL_TRIGGER
  CoraCoolDatabaseSvc& corasvc = CoraCoolDatabaseSvcFactory::databaseService();
  CoraCoolDatabasePtr coradb;
  try {
    coradb = corasvc.openDatabase("COOLONL_TRIGGER/CONDBR2",true);
    ATH_MSG_DEBUG( "Database connection open OK");
  }
  catch (std::exception& e) {
    ATH_MSG_WARNING( "Problem opening CORAL database:" << e.what());
  }
  
  // get DB pointer
  cool::IDatabasePtr cooldb = coradb->coolDatabase();

  // list the COOL folders found in the database
  //cool::IDatabasePtr cooldb = coradb->coolDatabase();
  //const std::vector<std::string>& folders = cooldb->listAllNodes();
  //for ( std::vector<std::string>::const_iterator itr = folders.begin(); itr != folders.end(); ++itr ) 
  //  ATH_MSG_DEBUG( *itr);

  cool::ValidityKey since(runNumber);
  cool::ValidityKey until(since+1);
  since <<= 32; // upper 31 bits are runnumber
  until <<= 32;
  since += 1;

  //--------------------------------//
  // get the LB start and end times //
  //--------------------------------//

  cool::IFolderPtr lbTimeFolder;
  try {
    ATH_MSG_DEBUG( "Will now try to retrieve LB times in folder " << m_lbTimeCoolFolderName);
    lbTimeFolder = cooldb->getFolder(m_lbTimeCoolFolderName);
  }
  catch (cool::Exception& e) {
    ATH_MSG_WARNING( "Exception during cool:IDatabasePtr::getFolder(): " << e.what());
    return;
  }

  // printout specification of folder
  //const cool::IRecordSpecification& spec = lbTimeFolder->payloadSpecification();
  //for ( unsigned int i = 0; i < spec.size(); ++i ) {
  //  ATH_MSG_WARNING( "Payload column: " << spec[i].name() << " type: " << 
  //    spec[i].storageType().name());
  //}

  cool::IObjectIteratorPtr lbTimeObjects = lbTimeFolder->browseObjects( since, until, 0 );
  if ( lbTimeObjects->size() != 0 ) {
    while ( lbTimeObjects->goToNext() ) {
      const cool::IObject& obj = lbTimeObjects->currentRef();
      const cool::IRecord & payload = obj.payload();
      uint32_t startRun = (obj.since() >> 32) & 0x7fffffff; // mask out highest bit, timestamp is "UInt63"
      uint32_t startLumiBlock = (obj.since() & 0xffffffff);
      uint32_t endRun = (obj.until() >> 32) & 0x7fffffff; // same here
      uint32_t endLumiBlock = (obj.until() & 0xffffffff);
      ATH_MSG_DEBUG( "LB " << " since: run = " << startRun << ", LB = " << startLumiBlock << ", "
                     << " until: run = " << endRun << ", LB = " << endLumiBlock << ")" << payload);
      auto stime = static_cast<uint64_t>(payload["StartTime"].data<cool::UInt63>());
      auto etime = static_cast<uint64_t>(payload["EndTime"].data<cool::UInt63>());
      m_lumiBlocks.push_back(startLumiBlock);
      m_lbStartTimes[startLumiBlock] = stime;
      m_lbEndTimes[startLumiBlock] = etime;
    }
  }
  else {
    ATH_MSG_DEBUG( "No lumi block start and stop times found in COOL for this run!");
  }

  //--------------------------------//
  // get the frequency measurements //
  //--------------------------------//

  cool::IFolderPtr freqMeasFolder;
  try {
    freqMeasFolder = cooldb->getFolder(m_rfPhaseCoolFolderName);
  }
  catch (cool::Exception& e) {
    ATH_MSG_WARNING( "Exception caught when retrieving LHC timing signal info from COOL: " << e.what());
  }

  double frequency = 0;
  uint64_t startTime = 0;

  cool::IObjectIteratorPtr freqMeasObjects = freqMeasFolder->browseObjects( since, until, 0 );
  if ( freqMeasObjects->size() != 0 ) {
    while ( freqMeasObjects->goToNext() ) {
      const cool::IObject& obj = freqMeasObjects->currentRef();
      //ATH_MSG_WARNING( obj);
      const cool::IRecord& payload = obj.payload();
      startTime = payload["StartTimeSeconds"].data<uint32_t>()*1e9+payload["StartTimeNanoSeconds"].data<uint32_t>();
      frequency = payload["FreqBCctp"].data<double>();
      ATH_MSG_DEBUG( "StartTime: " << startTime << " ns, FreqBCctp: " << frequency << " Hz");
      m_freqMeasurements[startTime] = frequency;
    }
  }
  else {
    ATH_MSG_DEBUG( "No frequency measurements found in COOL for this run!");
  }

  // now fill the measured frequencies closest to the start of each LB
  for ( std::vector<uint32_t>::const_iterator lbIt = m_lumiBlocks.begin(); 
	lbIt != m_lumiBlocks.end(); ++lbIt ) {
    double freq = getFrequencyMeasurement(m_lbStartTimes[*lbIt]);
    m_lbStartFreqMeasurements[*lbIt] = freq;
    ATH_MSG_DEBUG( "Frequency for LB " << *lbIt << " (start time: " << m_lbStartTimes[*lbIt] << "): " << freq << " Hz");
  }

  //-------------------------------//
  // get the beam mode information //
  //-------------------------------//

  // establish a connection to COOLOFL_DCS
  try {
    coradb = corasvc.openDatabase("COOLOFL_DCS/CONDBR2",true);
    ATH_MSG_DEBUG( "Database connection open OK");
  }
  catch (std::exception& e) {
    ATH_MSG_WARNING( "Problem opening CORAL database:" << e.what());
  }
  // get DB pointer
  cool::IDatabasePtr cooldb2 = coradb->coolDatabase();
  if(m_fillStateCoolFolderName!="Unavailable"){
    cool::IFolderPtr fillStateFolder;
    try {
      ATH_MSG_DEBUG( "Will now try to retrieve LHC fill state info in folder " << m_fillStateCoolFolderName);
      fillStateFolder = cooldb2->getFolder(m_fillStateCoolFolderName);
    }
    catch (cool::Exception& e) {
      ATH_MSG_WARNING( "Exception from cool::IDatabasePtr::getFolder(): " << e.what());
    return;
    }
    
    if(m_lumiBlocks.size() > 0){
      cool::ValidityKey sinceTime(m_lbStartTimes[m_lumiBlocks[0]]);
      cool::ValidityKey untilTime(m_lbStartTimes[m_lbStartTimes.size()-1]);
      cool::IObjectIteratorPtr fillStateObjects = fillStateFolder->browseObjects( sinceTime, untilTime, 1 );
      uint64_t endTime = 0;
      std::string currentBeamMode;
    
      if ( fillStateObjects->size() != 0 ) {
	while ( fillStateObjects->goToNext() ) {
	  const cool::IObject& obj = fillStateObjects->currentRef();
	  const cool::IRecord & payload = obj.payload();
	  currentBeamMode = payload["BeamMode"].data<cool::String4k>();
	  startTime = obj.since();
	  endTime = obj.until();
	  ATH_MSG_DEBUG( "BEAM MODE: " << currentBeamMode << ", start time: " 
			 << startTime << ", stop time: " << endTime);
	  
	  // fill the beam mode map, cases ordered by how often they appear
	  if (!strcmp(currentBeamMode.c_str(), "RAMP"))
	    m_beamMode[startTime] = RAMP;
	  else if (!strcmp(currentBeamMode.c_str(), "RAMP DOWN"))
	    m_beamMode[startTime] = RAMPDOWN;
	  else if (!strcmp(currentBeamMode.c_str(), "CYCLING"))
	    m_beamMode[startTime] = CYCLING;
	  else if (!strcmp(currentBeamMode.c_str(), "SETUP"))
	    m_beamMode[startTime] = SETUP;
	  else if (!strcmp(currentBeamMode.c_str(), "STABLE BEAMS"))
	    m_beamMode[startTime] = STABLEBEAMS;
	  else if (!strcmp(currentBeamMode.c_str(), "INJECTION PHYSICS BEAM"))
	    m_beamMode[startTime] = INJECTIONPHYSICSBEAM;
	  else if (!strcmp(currentBeamMode.c_str(), "ABORT"))
	    m_beamMode[startTime] = ABORT;
	  else if (!strcmp(currentBeamMode.c_str(), "INJECTION PROBE BEAM"))
	    m_beamMode[startTime] = INJECTIONPROBEBEAM;
	  else if (!strcmp(currentBeamMode.c_str(), "INJECTION SETUP BEAM"))
	    m_beamMode[startTime] = INJECTIONSETUPBEAM;
	  else if (!strcmp(currentBeamMode.c_str(), "PREPARE RAMP"))
	    m_beamMode[startTime] = PREPARERAMP;
	  else if (!strcmp(currentBeamMode.c_str(), "FLAT TOP"))
	    m_beamMode[startTime] = FLATTOP;
	  else if (!strcmp(currentBeamMode.c_str(), "SQUEEZE"))
	    m_beamMode[startTime] = SQUEEZE;
	  else if (!strcmp(currentBeamMode.c_str(), "ADJUST"))
	    m_beamMode[startTime] = ADJUST;
	  else if (!strcmp(currentBeamMode.c_str(), "UNSTABLE BEAMS"))
	    m_beamMode[startTime] = UNSTABLEBEAMS;
	  else if (!strcmp(currentBeamMode.c_str(), "BEAM DUMP"))
	    m_beamMode[startTime] = BEAMDUMP;
	  else if (!strcmp(currentBeamMode.c_str(), "RECOVERY"))
	    m_beamMode[startTime] = RECOVERY;
	  else if (!strcmp(currentBeamMode.c_str(), "INJECT AND DUMP"))
	    m_beamMode[startTime] = INJECTANDDUMP;
	  else if (!strcmp(currentBeamMode.c_str(), "CIRCULATE AND DUMP"))
	    m_beamMode[startTime] = CIRCULATEANDDUMP;
	  else if (!strcmp(currentBeamMode.c_str(), "NO BEAM"))
	    m_beamMode[startTime] = NOBEAM;
	  else {
	    m_beamMode[startTime] = UNKNOWN;
	    ATH_MSG_WARNING( "Unknown LHC beam mode read from COOL: " << payload["BeamMode"] 
			     << " (will treat as unstable)");
	  }
	  // fill an entry with unknown after the validity of the last real entry is over
	  m_beamMode[endTime] = UNKNOWN;
	}
	
	// now clean out all the entries that repeat the previous beam mode and only keep the ones where the beam mode changes
	BeamMode previousBeamMode = UNKNOWN;
	BeamMode currentBeamMode = UNKNOWN;
	// loop over the time-ordered beam mode entries and remove unnecessary entries
	std::map<uint64_t,BeamMode>::iterator bm = m_beamMode.begin();
	std::map<uint64_t,BeamMode>::iterator previousbm;
	
	while (bm != m_beamMode.end()) {
	  currentBeamMode = bm->second;
	  ATH_MSG_DEBUG( "Beam mode at time " << bm->first << ": " << currentBeamMode);
	  if (currentBeamMode == previousBeamMode) {
	    previousbm = bm;
	    ++bm;
	    m_beamMode.erase(previousbm);
	    ATH_MSG_DEBUG( " => Removing!");
	  }
	  else {
	    ++bm;
	  }
	  previousBeamMode = currentBeamMode;
	}
      }
      else {
	ATH_MSG_DEBUG( "No Beam Mode info found in COOL for this run!");
      }
    }
    else{ //cool publication is not available
      ATH_MSG_DEBUG( "No Beam Mode info found in COOL for this run as folder is unavailable!");
    }
  }
  else{ //no lumi block available
    ATH_MSG_DEBUG( "No lumi block available for this run!");
  }

  //-----------------------------------------------//
  // get the data taking mode (ATLAS_READY or not) //
  //-----------------------------------------------//

  // establish a connection to COOLOFL_DCS
  try {
    coradb = corasvc.openDatabase("COOLONL_TDAQ/CONDBR2",true);
    ATH_MSG_DEBUG( "Database connection open OK");
  }
  catch (std::exception& e) {
    ATH_MSG_WARNING( "Problem opening CORAL database:" << e.what());
  }
  // get DB pointer
  cool::IDatabasePtr cooldb3 = coradb->coolDatabase();
  cool::IFolderPtr dataTakingModeFolder;
  try {
    dataTakingModeFolder = cooldb3->getFolder(m_dataTakingModeCoolFolderName);
  }
  catch (cool::Exception& e) {
    ATH_MSG_WARNING( "Exception caught when retrieving ATLAS data taking mode info from COOL: " << e.what());
  }

  uint32_t readyForPhysics = 0;
  cool::IObjectIteratorPtr dataTakingModeObjects = dataTakingModeFolder->browseObjects( since, until, 0 );
  if ( dataTakingModeObjects->size() != 0 ) {
    while ( dataTakingModeObjects->goToNext() ) {
      const cool::IObject& obj = dataTakingModeObjects->currentRef();
      const cool::IRecord & payload = obj.payload();
      uint32_t startRun = (obj.since() >> 32) & 0x7fffffff; // mask out highest bit, timestamp is "UInt63"
      uint32_t startLumiBlock = (obj.since() & 0xffffffff);
      uint32_t endRun = (obj.until() >> 32) & 0x7fffffff; // same here
      uint32_t endLumiBlock = (obj.until() & 0xffffffff);
      ATH_MSG_DEBUG( "Current entry "
		       << " since: run = " << startRun << ", LB = " << startLumiBlock << ", "
		       << " until: run = " << endRun << ", LB = " << endLumiBlock << ")"
		       << payload);
      readyForPhysics = payload["ReadyForPhysics"].data<uint32_t>();
      if (readyForPhysics == 0)
	m_dataTakingMode[startLumiBlock] = false;
      else {
	m_dataTakingMode[startLumiBlock] = true;
	if (readyForPhysics != 1) {
	  ATH_MSG_WARNING( "ReadyForPhysics is neither 0 nor 1, retrieved value was: " 
			   << readyForPhysics << " => will interpret as true");
	}
      }
    }
  }
  else {
    ATH_MSG_DEBUG( "No data taking mode info found in COOL for this run!");
  }

  // now clean out all the entries that repeat the previous data taking mode and only keep the ones where it changes
  bool previousDataTakingMode = false;
  bool currentDataTakingMode = false;
  // loop over the LBN-ordered data taking mode entries and remove unnecessary entries
  std::map<uint32_t,bool>::iterator dtm = m_dataTakingMode.begin();
  std::map<uint32_t,bool>::iterator previousdtm;
  
  while (dtm != m_dataTakingMode.end()) {
    currentDataTakingMode = dtm->second;
    ATH_MSG_DEBUG( "Data taking mode starting at LB " << dtm->first << ": " << currentDataTakingMode);
    if (currentDataTakingMode == previousDataTakingMode && dtm != m_dataTakingMode.begin()) { // don't remove 1st
      previousdtm = dtm;
      ++dtm;
      m_dataTakingMode.erase(previousdtm);
      ATH_MSG_DEBUG( " => Removing!");
    }
    else {
      ++dtm;
    }
    previousDataTakingMode = currentDataTakingMode;
  }

  return;
}

std::string
TrigT1CTMonitoring::BSMonitoring::getEventInfoString()
{
  std::stringstream ss;
  ss << "Run number: " << m_runNumber << ", event: " << m_eventNumber << ", LB: " << m_currentLumiBlock;
  return std::string(ss.str());
}

double
TrigT1CTMonitoring::BSMonitoring::getFrequencyMeasurement(uint64_t eventTimeInNs) const
{
  if (m_freqMeasurements.size() == 0) {
    return -1.0; // if there are no frequency measurements available for this run, return invalid frequency
  }
  else if (m_freqMeasurements.size() == 1) {
    return m_freqMeasurements.begin()->second; // if there is only one available freq measurement, return that
  }
  // get the pairs for the measurements right before and after the provided timestamp
  std::map<uint64_t,double>::const_iterator freqMeasAfter = m_freqMeasurements.lower_bound(eventTimeInNs);
  std::map<uint64_t,double>::const_iterator freqMeasBefore = freqMeasAfter; --freqMeasBefore; // this is a bit hacky...
  std::map<uint64_t,double>::const_iterator fm = freqMeasAfter;

  // determine if this measurement or the previous one is closer in time, and the return the closest
  if ( freqMeasAfter != m_freqMeasurements.begin() && (fabs(eventTimeInNs - freqMeasBefore->first) < fabs(eventTimeInNs - freqMeasAfter->first)) )
    fm = freqMeasBefore;

  ATH_MSG_DEBUG( "Closest frequency measurement for time " << eventTimeInNs 
		 << "): " << fm->second << " Hz (at " << fm->first << ")");
  return fm->second;
}

TrigT1CTMonitoring::BeamMode 
TrigT1CTMonitoring::BSMonitoring::getBeamMode(uint64_t eventTimeInNs) const
{
  // get the pair for the first beam mode transition *after* the provided timestamp
  std::map<uint64_t,BeamMode>::const_iterator bm = m_beamMode.lower_bound(eventTimeInNs);
  BeamMode beamMode = UNKNOWN;
  // if this is not the first entry retrieved from COOL, take the previous one
  if ( bm != m_beamMode.begin() && bm != m_beamMode.end() ) {
    beamMode = (--bm)->second;
  }
  ATH_MSG_DEBUG( "Beam mode at time " << eventTimeInNs << ": " << beamMode);
  return beamMode;
}
