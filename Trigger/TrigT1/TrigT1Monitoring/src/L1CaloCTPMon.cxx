/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:        L1CaloCTPMon.cxx
// PACKAGE:     TrigT1CaloMonitoring  
//
// AUTHOR:      Johanna Fleckner (Johanna.Fleckner@uni-mainz.de)
//           
// DESCRIPTION: Monitoring of L1Calo --> CTP transmission
//
// ********************************************************************

#include <map>
#include <utility>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "LWHists/LWHist.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2I_LW.h"

#include "TrigT1CaloEvent/CMMCPHits.h"
#include "TrigT1CaloEvent/CMMJetHits.h"
#include "TrigT1CaloEvent/CMMEtSums.h"
#include "TrigConfigSvc/ILVL1ConfigSvc.h"
#include "TrigT1Monitoring/L1CaloCTPMon.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/PIT.h"
#include "TrigConfL1Data/TriggerThreshold.h"

#include "TrigT1Result/CTP_RDO.h"
#include "TrigT1Result/CTP_Decoder.h"

#include "AthenaMonitoring/AthenaMonManager.h"


// *********************************************************************
// Public Methods
// *********************************************************************

/*---------------------------------------------------------*/
L1CaloCTPMon::L1CaloCTPMon( const std::string & type, const std::string & name,
  		            const IInterface* parent )
  : ManagedMonitorToolBase( type, name, parent ),
    m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
    m_errorTool("TrigT1CaloMonErrorTool"),
    m_histTool("TrigT1CaloLWHistogramTool"),
    m_debug(false), m_histBooked(false),
    m_pitMap(0),
    m_h_ctp_1d_L1CaloNeCTPSummary(0),
    m_h_ctp_1d_L1CaloEqCTPSummary(0),
    m_h_ctp_1d_PITMatches(0),
    m_h_ctp_1d_HitNoPITMismatch(0),
    m_h_ctp_1d_PITNoHitMismatch(0),
    m_h_ctp_2d_MismatchEvents(0)
/*---------------------------------------------------------*/
{
  // This is how you declare the parameters to Gaudi so that
  // they can be over-written via the job options file

  declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");

  declareProperty( "CMMJetHitsLocation",
    m_CMMJetHitsLocation = LVL1::TrigT1CaloDefs::CMMJetHitsLocation);
  declareProperty( "CMMEtSumsLocation",
    m_CMMEtSumsLocation = LVL1::TrigT1CaloDefs::CMMEtSumsLocation);  
  declareProperty( "CMMCPHitsLocation",
    m_CMMCPHitsLocation = LVL1::TrigT1CaloDefs::CMMCPHitsLocation);
  declareProperty( "CTP_RDOLocation", m_CTPRDOLocation = "CTP_RDO");

  declareProperty( "PathInRootFile", m_PathInRootFile = "LVL1_Interfaces/CTP");
}


/*---------------------------------------------------------*/
L1CaloCTPMon::~L1CaloCTPMon()
/*---------------------------------------------------------*/
{
}

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

/*---------------------------------------------------------*/
StatusCode L1CaloCTPMon::initialize()
/*---------------------------------------------------------*/
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endreq;
  m_debug = msgLvl(MSG::DEBUG);

  StatusCode sc = ManagedMonitorToolBase::initialize();
  if (sc.isFailure()) return sc;
  
  // Connect to the TrigConfigSvc for the trigger configuration:

  sc = m_configSvc.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Couldn't connect to " << m_configSvc.typeAndName()
                    << endreq;
    return sc;
  } else {
    msg(MSG::INFO) << "Connected to " << m_configSvc.typeAndName() << endreq;
  }

  sc = m_errorTool.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloMonErrorTool"
                    << endreq;
    return sc;
  }

  sc = m_histTool.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloLWHistogramTool"
                    << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode L1CaloCTPMon::bookHistogramsRecurrent()
/*---------------------------------------------------------*/
{
  msg(MSG::DEBUG) << "in L1CaloCTPMon::bookHistograms" << endreq;
  
  if( m_environment == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
  }
  
  if( m_dataType == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }
  
  if( newLumiBlock ) {}

  if( newRun ) {	

    MgmtAttr_t attr = ATTRIB_UNMANAGED;
    MonGroup L1CaloCTP( this, m_PathInRootFile, run, attr );
    MonGroup L1CaloCTPEvents( this, m_PathInRootFile, run, attr,
                                                    "", "eventSample" );

    //-------------- transmission checks L1Calo <-> CTP ----------------------

    m_histTool->setMonGroup(&L1CaloCTP);

    m_h_ctp_1d_L1CaloNeCTPSummary = m_histTool->book1F("ctp_1d_L1CaloNeCTPSummary",
      "Transmission Errors between L1Calo and CTP",
      NumberOfHitTypes, 0, NumberOfHitTypes);
    setLabels(m_h_ctp_1d_L1CaloNeCTPSummary);
    m_h_ctp_1d_L1CaloEqCTPSummary = m_histTool->book1F("ctp_1d_L1CaloEqCTPSummary",
      "Transmission Matches between L1Calo and CTP",
      NumberOfHitTypes, 0, NumberOfHitTypes);
    setLabels(m_h_ctp_1d_L1CaloEqCTPSummary);

    m_h_ctp_1d_PITMatches = m_histTool->book1F("ctp_1d_PITMatches",
      "CTP/L1Calo PIT Matches;PIT", 160, 0., 160.);
    m_h_ctp_1d_HitNoPITMismatch = m_histTool->book1F("ctp_1d_HitNoPITMismatch",
      "L1Calo Hit but no CTP PIT Mismatches;PIT", 160, 0., 160.);
    m_h_ctp_1d_PITNoHitMismatch = m_histTool->book1F("ctp_1d_PITNoHitMismatch",
      "CTP PIT but no L1Calo Hit Mismatches;PIT", 160, 0., 160.);

    m_histTool->setMonGroup(&L1CaloCTPEvents);

    m_h_ctp_2d_MismatchEvents = m_histTool->bookEventNumbers("ctp_2d_MismatchEvents",
      "Transmission Errors between L1Calo and CTP Event Numbers",
      NumberOfHitTypes, 0, NumberOfHitTypes);
    setLabels(m_h_ctp_2d_MismatchEvents, false);

    m_histTool->unsetMonGroup();
    m_histBooked = true;

    //---------------------- get PIT mappings from COOL ----------------------

    TrigConf::L1DataDef def;
    const int l1caloBits =
      3*(def.max_EM_Threshold_Number() + def.max_JET_Threshold_Number())  +
      2*(def.max_JF_Threshold_Number() + def.max_JB_Threshold_Number()) +
         def.max_XE_Threshold_Number() + def.max_JE_Threshold_Number()  +
	 def.max_TE_Threshold_Number() + def.max_XS_Threshold_Number();
    m_pitMap.clear();
    m_pitMap.assign(l1caloBits, -1);
    if (m_configSvc->ctpConfig()) {
      std::map<std::string, int> threshMap;
      const std::vector<TrigConf::TriggerThreshold*>&
            thresholds(m_configSvc->ctpConfig()->menu().thresholdVector());
      std::vector<TrigConf::TriggerThreshold*>::const_iterator it;
      for (it = thresholds.begin(); it != thresholds.end(); ++it) {
        int offset = 0;
        int nbits = 3;
        while (true) {
          if ( (*it)->type() == def.emType() ) break;
          if ( (*it)->type() == def.tauType() ) break;
          offset += nbits*def.max_EM_Threshold_Number(); // Includes Em/Tau
          if ( (*it)->type() == def.jetType() ) break;
          offset += nbits*def.max_JET_Threshold_Number();
          nbits--;
          if ( (*it)->type() == def.jfType() ) break;
          offset += nbits*def.max_JF_Threshold_Number();
          if ( (*it)->type() == def.jbType() ) break;
          offset += nbits*def.max_JB_Threshold_Number();
          nbits--;
          if ( (*it)->type() == def.jeType() ) break;
          offset += nbits*def.max_JE_Threshold_Number();
          if ( (*it)->type() == def.xeType() ) break;
          offset += nbits*def.max_XE_Threshold_Number();
          if ( (*it)->type() == def.teType() ) break;
          offset += nbits*def.max_TE_Threshold_Number();
          if ( (*it)->type() == def.xsType() ) break;
          nbits--;
          break;
        }
        if (nbits == 0) continue;
        const int threshNumber = (*it)->thresholdNumber();
        if (threshNumber < 0) continue;
        threshMap.insert(std::make_pair((*it)->name(),
	                 offset + threshNumber*nbits));
        msg(MSG::VERBOSE) << "threshMap: name, offset, threshNumber, nbits  "
                          << (*it)->name() << " " << offset << " "
			  << threshNumber << " " << nbits << endreq;
      }
      msg(MSG::DEBUG) << "Size of threshMap = " << threshMap.size() << endreq;
      msg(MSG::DEBUG) << "Size of pitMap    = " << m_pitMap.size() << endreq;
      const std::vector<TrigConf::PIT*>&
                        pits(m_configSvc->ctpConfig()->menu().pitVector());
      msg(MSG::DEBUG) << "Size of pitVector = " << pits.size() << endreq;
      int count = 0;
      std::vector<TrigConf::PIT*>::const_iterator itp;
      for (itp = pits.begin(); itp != pits.end(); ++itp) {
        std::map<std::string, int>::const_iterator miter =
	                              threshMap.find((*itp)->thresholdName());
        if (miter != threshMap.end()) {
          const int pos = miter->second + (*itp)->thresholdBit();
          if (pos < l1caloBits) {
            m_pitMap[pos] = (*itp)->pitNumber();
	    count++;
	    msg(MSG::VERBOSE) << "pitMap[" << pos << "] = " << m_pitMap[pos]
	                      << "  name " << (*itp)->thresholdName() << endreq;
          } else msg(MSG::WARNING) << "Error setting up PIT map" << endreq;
        } else msg(MSG::VERBOSE) << "PIT name " << (*itp)->thresholdName()
	                         << " not found in threshMap" << endreq;
      }
      msg(MSG::DEBUG) << "Entries in pitMap = " << count << endreq;
    } else msg(MSG::WARNING) << "No ctpConfig found" << endreq;

  }
  
  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode L1CaloCTPMon::fillHistograms()
/*---------------------------------------------------------*/
{
  if (m_debug) msg(MSG::DEBUG) << "in L1CaloCTPMon::fillHistograms" << endreq;

  if (!m_histBooked) {
    if (m_debug) msg(MSG::DEBUG) << "Histogram(s) not booked" << endreq;
    return StatusCode::SUCCESS;
  }

  // Skip events believed to be corrupt or with ROB errors

  if (m_errorTool->corrupt() || m_errorTool->robOrUnpackingError()) {
    if (m_debug) msg(MSG::DEBUG) << "Skipping corrupt event" << endreq;
    return StatusCode::SUCCESS;
  }

  //--------------------------------------------------------------------------
  //---------------- get information send from CP CMMs to CTP ----------------
  //--------------------------------------------------------------------------
 
  // retrieve CMM CP Hits from Storegate
  const CMMCPHitsCollection* CMMCPHits = 0;
  StatusCode sc = evtStore()->retrieve(CMMCPHits, m_CMMCPHitsLocation);
  if (sc == StatusCode::FAILURE || !CMMCPHits) {
    msg(MSG::INFO) << "No CMM CPHits found in TES at " << m_CMMCPHitsLocation
                   << endreq;
    return StatusCode::SUCCESS;
  }
  CMMCPHitsCollection::const_iterator it_CMMCPHits = CMMCPHits->begin();
  int EMHits    = 0;
  int EMTauHits = 0;
 
  for (; it_CMMCPHits != CMMCPHits->end(); ++it_CMMCPHits) {      
    // CMM information for transmission check System CMM -> CTP
    if (((*it_CMMCPHits)->dataID() == LVL1::CMMCPHits::TOTAL) &&
                                          ((*it_CMMCPHits)->crate() == 3)) {
      EMHits    = (*it_CMMCPHits)->HitWord0();
      EMTauHits = (*it_CMMCPHits)->HitWord1();
    }
  }
  
  //--------------------------------------------------------------------------
  //----------------- get information send from JEP CMMs to CTP --------------
  //--------------------------------------------------------------------------

  // retrieve CMM Jet Hits from Storegate
  const CMMJetHitsCollection* CMMJetHits = 0;
  sc = evtStore()->retrieve(CMMJetHits, m_CMMJetHitsLocation);
  if (sc == StatusCode::FAILURE || !CMMJetHits) {
    msg(MSG::INFO) << "No CMM JetHits found in TES at " << m_CMMJetHitsLocation
                   << endreq;
    return StatusCode::SUCCESS;
  }
  CMMJetHitsCollection::const_iterator it_CMMJetHits = CMMJetHits->begin();
  int MainHits  = 0;
  int FwdHits   = 0;
  int BwdHits   = 0;
  int JetEtHits = 0;

  for (; it_CMMJetHits != CMMJetHits->end(); ++it_CMMJetHits) {      
    // CMM information for transmission check System CMM -> CTP
    // main hits of system CMM
    if (((*it_CMMJetHits)->dataID() == LVL1::CMMJetHits::TOTAL_MAIN) &&
        ((*it_CMMJetHits)->crate() == 1)) MainHits = (*it_CMMJetHits)->Hits();
    // fwd hits of system CMM
    if (((*it_CMMJetHits)->dataID() == LVL1::CMMJetHits::TOTAL_FORWARD) &&
                                           ((*it_CMMJetHits)->crate() == 1)) {
      BwdHits = (*it_CMMJetHits)->Hits();
      FwdHits = (BwdHits >> 8);
      BwdHits &= 0xff;
    }
    // Jet Et Hits system CMM
    if (((*it_CMMJetHits)->dataID() == LVL1::CMMJetHits::ET_MAP) &&
        ((*it_CMMJetHits)->crate() == 1)) JetEtHits = (*it_CMMJetHits)->Hits();
  }
  
  // retrieve CMM Et Sums from Storegate
  const CMMEtSumsCollection* CMMEtSums = 0;
  sc = evtStore()->retrieve(CMMEtSums, m_CMMEtSumsLocation);
  if (sc == StatusCode::FAILURE || !CMMEtSums) {
    msg(MSG::INFO) << "No CMMEtSums found in TES at " << m_CMMEtSumsLocation
                   << endreq;
    return StatusCode::SUCCESS;
  }
  CMMEtSumsCollection::const_iterator it_CMMEtSums = CMMEtSums->begin();
  int MissingEtHits    = 0;
  int SumEtHits        = 0;
  int MissingEtSigHits = 0;

  for (; it_CMMEtSums != CMMEtSums->end(); ++it_CMMEtSums) {
    // Missing Et Hits
    if (((*it_CMMEtSums)->dataID() == LVL1::CMMEtSums::MISSING_ET_MAP) &&
        ((*it_CMMEtSums)->crate() == 1)) MissingEtHits = (*it_CMMEtSums)->Et();
    // Sum Et Hits
    if (((*it_CMMEtSums)->dataID() == LVL1::CMMEtSums::SUM_ET_MAP) &&
        ((*it_CMMEtSums)->crate() == 1)) SumEtHits = (*it_CMMEtSums)->Et();  
    // Missing Et Sig Hits
    if (((*it_CMMEtSums)->dataID() == LVL1::CMMEtSums::MISSING_ET_SIG_MAP) &&
        ((*it_CMMEtSums)->crate() == 1)) MissingEtSigHits =
	                                                 (*it_CMMEtSums)->Et();
  }

  //--------------------------------------------------------------------------
  //----------------------- get the PITs (input for CTP) ---------------------
  //--------------------------------------------------------------------------

  const CTP_RDO* ctpRDO = 0;
  sc = evtStore()->retrieve(ctpRDO, m_CTPRDOLocation); 
  if (sc == StatusCode::FAILURE || !ctpRDO) {
    msg(MSG::INFO) << "No CTP_RDO found in TES at " << m_CTPRDOLocation
                   << endreq;
    return StatusCode::SUCCESS;
  }

  CTP_Decoder ctp;
  ctp.setRDO(ctpRDO);

  const uint16_t l1aPos = ctpRDO->getL1AcceptBunchPosition();
  if (l1aPos >= ctp.getBunchCrossings().size()) {
    msg(MSG::WARNING) << "Invalid l1aPos" << endreq;
    return StatusCode::SUCCESS;
  }
  if (m_debug) msg(MSG::DEBUG) << "l1aPos, size : " << l1aPos << ", "
                               << ctp.getBunchCrossings().size() << endreq;
  const CTP_BC& bunch = ctp.getBunchCrossings().at(l1aPos);

  
  //--------------------------------------------------------------------------
  //---------------------- compare L1Calo data with CTP ----------------------
  //--------------------------------------------------------------------------

  TrigConf::L1DataDef def;

  //------------------------ EM Hits (3 bits / thresh) -----------------------
  int offset = 0;
  int threshBits = 3;
  int totalBits = threshBits*def.max_EM_Threshold_Number()/2; // max_EM includes Tau
  compare(bunch, EMHits, totalBits, offset, EMType);

  //----------------------- EMTau Hits (3 bits / thresh) ---------------------
  offset += totalBits;
  compare(bunch, EMTauHits, totalBits, offset, EMTauType);

  //------------------------ Main Hits (3 bits / thresh) ---------------------
  offset += totalBits;
  totalBits = threshBits*def.max_JET_Threshold_Number();
  compare(bunch, MainHits, totalBits, offset, MainType);
  
  //----------------------- Forward Jets (2 bits / thresh) -------------------
  offset += totalBits;
  threshBits--;
  totalBits = threshBits*def.max_JF_Threshold_Number();
  compare(bunch, FwdHits, totalBits, offset, ForwardType);

  //---------------------- Backwards Jets (2 bits / thresh) ------------------
  offset += totalBits;
  totalBits = threshBits*def.max_JB_Threshold_Number();
  compare(bunch, BwdHits, totalBits, offset, BackwardType);
  
  //------------------------ Jet Et Hits (1 bit / thresh) --------------------
  offset += totalBits;
  threshBits--;
  totalBits = threshBits*def.max_JE_Threshold_Number();
  compare(bunch, JetEtHits, totalBits, offset, JetEtType);
  
  //---------------------- Missing ET Hits (1 bit / thresh) ------------------
  offset += totalBits;
  totalBits = threshBits*def.max_XE_Threshold_Number();
  compare(bunch, MissingEtHits, totalBits, offset, MissingEtType);
  
  //------------------------ Sum Et Hits (1 bit / thresh) --------------------
  offset += totalBits;
  totalBits = threshBits*def.max_TE_Threshold_Number();
  compare(bunch, SumEtHits, totalBits, offset, SumEtType);
  
  //---------------------- Missing ET Sig Hits (1 bit / thresh) ------------------
  offset += totalBits;
  totalBits = threshBits*def.max_XS_Threshold_Number();
  compare(bunch, MissingEtSigHits, totalBits, offset, MissingEtSigType);

  return StatusCode::SUCCESS;
 }

/*---------------------------------------------------------*/
StatusCode L1CaloCTPMon::procHistograms()
/*---------------------------------------------------------*/
{
  if( endOfLumiBlock ) { }
	
  if( endOfRun ) { }
  
  return StatusCode::SUCCESS;
}

// *********************************************************************
// Private Methods
// *********************************************************************

void L1CaloCTPMon::compare(const CTP_BC& bunch, int hits, int totalBits,
                           int offset, L1CaloCTPHitTypes type)
{
  if (m_debug) {
    std::string name(m_h_ctp_1d_L1CaloNeCTPSummary->GetXaxis()->GetBinLabel(1+type));
    std::string subdet((type == EMType || type == EMTauType) ? " CP:  "
                                                             : " JEP: ");
    msg(MSG::DEBUG) << name << subdet
                    << m_histTool->thresholdString(hits, totalBits)
		    << endreq;
    msg(MSG::DEBUG) << name << " CTP: ";
  }

  int mask    = 0;
  int pitHits = 0;
  for (int bit = 0; bit < totalBits; ++bit) {
    const int PITid = m_pitMap[offset + bit];
    if (PITid < 0) {
      if (m_debug) msg(MSG::DEBUG) << "X ";
      continue;
    }
    const int HITbit= ((hits >> bit) & 0x1);
    const int PITbit = bunch.getPIT().test( PITid );
    if (m_debug) msg(MSG::DEBUG) << PITbit << " ";  
    pitHits |= (PITbit << bit);
    mask    |= (1 << bit);
      
    if (HITbit && HITbit == PITbit) m_h_ctp_1d_PITMatches->Fill(PITid);
    else if (HITbit != PITbit) {
      if (HITbit) m_h_ctp_1d_HitNoPITMismatch->Fill(PITid);
      else        m_h_ctp_1d_PITNoHitMismatch->Fill(PITid);
    }
  }
  if (m_debug) msg(MSG::DEBUG) << endreq;
  if (pitHits != (hits&mask)) {
    m_h_ctp_1d_L1CaloNeCTPSummary->Fill(type);
    m_histTool->fillEventNumber(m_h_ctp_2d_MismatchEvents, type);
  } else if (pitHits) m_h_ctp_1d_L1CaloEqCTPSummary->Fill(type);
 
}

void L1CaloCTPMon::setLabels(LWHist* hist, bool xAxis)
{
  LWHist::LWHistAxis* axis = (xAxis) ? hist->GetXaxis() : hist->GetYaxis();
  axis->SetBinLabel(1+EMType,           "EMHits");
  axis->SetBinLabel(1+EMTauType,        "EMTauHits");
  axis->SetBinLabel(1+MainType,         "MainHits");
  axis->SetBinLabel(1+ForwardType,      "ForwardHits");
  axis->SetBinLabel(1+BackwardType,     "BackwardHits");
  axis->SetBinLabel(1+JetEtType,        "JetEtHits");
  axis->SetBinLabel(1+MissingEtType,    "MissingEtHits");
  axis->SetBinLabel(1+SumEtType,        "SumEtHits");
  axis->SetBinLabel(1+MissingEtSigType, "MissEtSigHits");
}
