/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:        L1CaloCTPMon.cxx
// PACKAGE:     TrigT1CaloMonitoring  
//
// AUTHOR:      Johanna Fleckner (Johanna.Fleckner@uni-mainz.de)
//              Kate Whalen (Run 2 version) <kate.whalen@cern.ch>
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

#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigT1CaloMonitoringTools/ITrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/TIP.h"
#include "TrigConfL1Data/TriggerThreshold.h"

#include "TrigT1Result/CTP_RDO.h"
#include "TrigT1Result/CTP_Decoder.h"

#include "xAODTrigL1Calo/CMXCPHitsContainer.h" 
#include "xAODTrigL1Calo/CMXJetHitsContainer.h"
#include "xAODTrigL1Calo/CMXEtSumsContainer.h" 

#include "AthenaMonitoring/AthenaMonManager.h"

#include "L1CaloCTPMon.h"
// ============================================================================
namespace LVL1 {
// ============================================================================

// *********************************************************************
// Public Methods
// *********************************************************************

/*---------------------------------------------------------*/
L1CaloCTPMon::L1CaloCTPMon( const std::string & type, const std::string & name,
  		            const IInterface* parent )
  : ManagedMonitorToolBase( type, name, parent ),
    m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
    m_errorTool("LVL1::TrigT1CaloMonErrorTool/TrigT1CaloMonErrorTool"),
    m_histTool("LVL1::TrigT1CaloLWHistogramTool/TrigT1CaloLWHistogramTool"),
    m_debug(false), m_histBooked(false),
    m_tipMap(0),
    m_h_ctp_1d_L1CaloNeCTPSummary(0),
    m_h_ctp_1d_L1CaloEqCTPSummary(0),
    m_h_ctp_1d_TIPMatches(0),
    m_h_ctp_1d_HitNoTIPMismatch(0),
    m_h_ctp_1d_TIPNoHitMismatch(0),
    m_h_ctp_2d_MismatchEvents(0),
    m_h_ctp_1d_EM_HitNoTIPMismatch(0),
    m_h_ctp_1d_TAU_HitNoTIPMismatch(0),
    m_h_ctp_1d_JET_HitNoTIPMismatch(0),
    m_h_ctp_1d_TE_HitNoTIPMismatch(0),
    m_h_ctp_1d_XE_HitNoTIPMismatch(0),
    m_h_ctp_1d_XS_HitNoTIPMismatch(0)
/*---------------------------------------------------------*/
{
  // This is how you declare the parameters to Gaudi so that
  // they can be over-written via the job options file

  declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
  declareProperty( "CMXJetHitsLocation",
      m_cmxJetHitsLocation = LVL1::TrigT1CaloDefs::CMXJetHitsLocation);  
  declareProperty( "CMXEtSumsLocation",
      m_cmxEtSumsLocation = LVL1::TrigT1CaloDefs::CMXEtSumsLocation);
  declareProperty("CMXCPHitsLocation",
    m_cmxCpHitsLocation = LVL1::TrigT1CaloDefs::CMXCPHitsLocation);      
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

    m_h_ctp_1d_TIPMatches = m_histTool->book1F("ctp_1d_TIPMatches",
      "CTP/L1Calo TIP Matches;TIP", m_nTIP, 0., m_nTIP);
    m_h_ctp_1d_HitNoTIPMismatch = m_histTool->book1F("ctp_1d_HitNoTIPMismatch",
      "L1Calo Hit but no CTP TIP Mismatches;TIP", m_nTIP, 0., m_nTIP);
    m_h_ctp_1d_TIPNoHitMismatch = m_histTool->book1F("ctp_1d_TIPNoHitMismatch",
      "CTP TIP but no L1Calo Hit Mismatches;TIP", m_nTIP, 0., m_nTIP);

    m_histTool->setMonGroup(&L1CaloCTPEvents);

    m_h_ctp_2d_MismatchEvents = m_histTool->bookEventNumbers("ctp_2d_MismatchEvents",
      "Transmission Errors between L1Calo and CTP Event Numbers",
      NumberOfHitTypes, 0, NumberOfHitTypes);
    setLabels(m_h_ctp_2d_MismatchEvents, false);

    m_histTool->unsetMonGroup();
    m_histBooked = true;

    //---------------------- get TIP mappings from COOL ----------------------

    TrigConf::L1DataDef def;
     
     const int l1caloBits =
      3*(def.max_EM_Threshold_Number() + def.max_TAU_Threshold_Number() + max_JET_3bit_Threshold_Number)  +
      2*max_JET_2bit_Threshold_Number +
      def.max_XE_Threshold_Number() + def.max_TE_Threshold_Number() + def.max_XS_Threshold_Number();
 
    msg(MSG::DEBUG) << "Max threshold number = EM: " << def.max_EM_Threshold_Number() << " JET (3-bit): " <<  max_JET_3bit_Threshold_Number
                    << " JET (2-bit): " <<  max_JET_2bit_Threshold_Number << " TE: " << def.max_TE_Threshold_Number()
                    << " XE: " <<  def.max_XE_Threshold_Number() << " XS: " << def.max_XS_Threshold_Number()   << endreq;

    m_tipMap.clear(); 
    msg(MSG::DEBUG) << "Number of l1calo bits: " << l1caloBits << endreq; 
    m_tipMap.assign(l1caloBits, std::make_pair(" ", -1));
    
    if (m_configSvc->ctpConfig()) { 
      std::map<std::string, int> threshMap;
      const std::vector<TrigConf::TriggerThreshold*>&
            thresholds(m_configSvc->ctpConfig()->menu().thresholdVector());
      std::vector<TrigConf::TriggerThreshold*>::const_iterator it;
      msg(MSG::DEBUG) << "Size of thresholds vector: " << thresholds.size() << endreq; 
      
      for (it = thresholds.begin(); it != thresholds.end(); ++it) {
        int offset = 0;
        int nbits = 3;
        const int threshNumber = (*it)->thresholdNumber();
        int fixedThreshNumber  = threshNumber;

        while (true) {
          if ( (*it)->type() == def.emType() ) {
            if (threshNumber >= (int)def.max_EM_Threshold_Number()/2) {  // Cable EM2; else cable EM1
              offset += nbits*def.max_EM_Threshold_Number()/2;
              fixedThreshNumber -= def.max_EM_Threshold_Number()/2;
            }
            break;
          }
          offset += nbits*def.max_EM_Threshold_Number(); 
          if ( (*it)->type() == def.tauType() ) {
            if (threshNumber >= (int)def.max_TAU_Threshold_Number()/2) { // Cable TAU2; else cable TAU1
              offset += nbits*def.max_TAU_Threshold_Number()/2;
              fixedThreshNumber -= def.max_TAU_Threshold_Number()/2;
            }
            break;
          }
          offset += nbits*def.max_TAU_Threshold_Number(); 
          if ( (*it)->type() == def.jetType() ) {
            if (threshNumber >= (int)max_JET_3bit_Threshold_Number) {   // Cable JET2 (2-bit thresholds); else JET1 (3-bit)
              offset += 3*max_JET_3bit_Threshold_Number;
              fixedThreshNumber -= max_JET_3bit_Threshold_Number;
              nbits--;
            }
            break;
          }
          offset += 3*max_JET_3bit_Threshold_Number;
          nbits--;
          offset += 2*max_JET_2bit_Threshold_Number;
          nbits--;
          if ( (*it)->type() == def.teType()) {
            if (threshNumber >= (int)def.max_TE_Threshold_Number()/2) {  // Restricted eta TE threshold: jump to cable EN2
              offset += nbits*def.max_TE_Threshold_Number()/2 + nbits*def.max_XE_Threshold_Number()/2 + nbits*def.max_XS_Threshold_Number(); // 8+8+8 bits on cable EN1
              fixedThreshNumber -= def.max_TE_Threshold_Number()/2;
            }
            break;  // Full eta & restricted eta thresholds separated on two cables
          }
          offset += nbits*def.max_TE_Threshold_Number()/2; 
          if ( (*it)->type() == def.xeType() ) {
            if (threshNumber >= (int)def.max_XE_Threshold_Number()/2) { // Restricted eta XE threshold: jump to cable EN2
              offset += nbits*def.max_TE_Threshold_Number()/2 + nbits*def.max_XE_Threshold_Number()/2 + nbits*def.max_XS_Threshold_Number();
              fixedThreshNumber -= def.max_XE_Threshold_Number()/2;
            }
            break;
          }
          offset += nbits*def.max_XE_Threshold_Number()/2;
          if ( (*it)->type() == def.xsType() ) break;
          offset += nbits*def.max_XS_Threshold_Number();
          nbits--;
          break;
        }
        if (nbits == 0) continue;
        if (threshNumber < 0) continue;
        threshMap.insert(std::make_pair((*it)->name(),
	                 offset + fixedThreshNumber*nbits));
        msg(MSG::DEBUG) << "threshMap: name, offset, threshNumber, nbits  "
                          << (*it)->name() << " " << offset << " "
			  << fixedThreshNumber << " " << nbits << endreq;
      } // End loop over thresholds vector
      msg(MSG::DEBUG) << "Size of threshMap = " << threshMap.size() << endreq;
      msg(MSG::DEBUG) << "Size of tipMap    = " << m_tipMap.size() << endreq;

      const std::vector<TrigConf::TIP*>&
                       tips(m_configSvc->ctpConfig()->menu().tipVector());
      msg(MSG::DEBUG) << "Size of tipVector = " << tips.size() << endreq;
      
      int count = 0;
      std::vector<TrigConf::TIP*>::const_iterator itp;
      
      for (itp = tips.begin(); itp != tips.end(); ++itp) {
        std::map<std::string, int>::const_iterator miter =
	                              threshMap.find((*itp)->thresholdName());
        if (miter != threshMap.end()) {
          const int pos = miter->second + (*itp)->thresholdBit();
          if (pos < l1caloBits) {
	    m_tipMap[pos] = std::make_pair((*itp)->thresholdName(), (*itp)->tipNumber());
            count++;
          } else msg(MSG::WARNING) << "Error setting up TIP map" << endreq;
        } else msg(MSG::DEBUG) << "TIP name " << (*itp)->thresholdName()
	                       << " not found in threshMap" << endreq;
      } // End loop over tips 
      msg(MSG::DEBUG) << "Entries in tipMap = " << count << endreq;

      if (m_debug) {
        int icount = 0;
        for ( auto it = m_tipMap.begin(); it != m_tipMap.end(); ++it) {
          msg(MSG::DEBUG) << "tipmap[" << icount << "] " << (*it).first << " TIP number: " << (*it).second <<  endreq;
          icount++;
        }
      }
    } else msg(MSG::WARNING) << "No ctpConfig found" << endreq;  // End if ctpConfig
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
  //---------------- get information sent from CP CMXs to CTP ----------------
  //--------------------------------------------------------------------------
 
  // Retrieve CMX CP hits from StoreGate
  const xAOD::CMXCPHitsContainer* cmxCpHitsTES = 0;                                
  StatusCode sc = evtStore()->retrieve(cmxCpHitsTES, m_cmxCpHitsLocation);        
  if (sc == StatusCode::FAILURE || !cmxCpHitsTES) {                              
    msg(MSG::INFO) << "No CMX CP Hits found in TES at " << m_cmxCpHitsLocation    
                   << endreq;                                                     
    return StatusCode::SUCCESS;                                                  
  }                                                                             

  uint32_t EMHits0  = 0;
  uint32_t EMHits1  = 0;
  uint32_t TauHits0 = 0;
  uint32_t TauHits1 = 0;    
 
  // CMX information for transmission check: System CMX -> CTP 
  if (cmxCpHitsTES) { 
    xAOD::CMXCPHitsContainer::const_iterator cmxIterator    = cmxCpHitsTES->begin();
    xAOD::CMXCPHitsContainer::const_iterator cmxIteratorEnd = cmxCpHitsTES->end();   
 
    for (; cmxIterator != cmxIteratorEnd; ++cmxIterator) {
      
      const uint8_t source = (*cmxIterator)->sourceComponent();
      const uint8_t crate  = (*cmxIterator)->crate();
      const uint8_t cmx    = (*cmxIterator)->cmx();

      if (source == xAOD::CMXCPHits::TOTAL && crate == 3) {                       
        if (cmx == 1) {
          EMHits0 = (*cmxIterator)->hits0();
          EMHits1 = (*cmxIterator)->hits1();
        }
        else {
          TauHits0 = (*cmxIterator)->hits0();
          TauHits1 = (*cmxIterator)->hits1();
        }
      } 
    } 
  }   
  
  //--------------------------------------------------------------------------
  //----------------- get information sent from JEP CMXs to CTP --------------
  //--------------------------------------------------------------------------
  
  // Retrieve CMX Jet Hits from Storegate
  const xAOD::CMXJetHitsContainer* cmxJetHitsTES = 0;                               
  sc = evtStore()->retrieve(cmxJetHitsTES, m_cmxJetHitsLocation);                   
  if (sc == StatusCode::FAILURE || !cmxJetHitsTES) {                                
    msg(MSG::INFO) << "No CMX Jet Hits found in TES at " << m_cmxJetHitsLocation    
                   << endreq;                                                       
    return StatusCode::SUCCESS;                                                     
  }                                                                                 

  int Jet3BitHits = 0;  // 3-bit multiplicities, thresholds 0-9, cable JET1
  int Jet2BitHits = 0;  // 2-bit multiplicities, thresholds 10-24, cable JET2
  
  int Jet3BitHits0 = 0; // 3-bit multiplicities, thresholds 0-4, cable JET1
  int Jet3BitHits1 = 0; // 3-bit multiplicities, thresholds 5-9, cable JET1
  int Jet2BitHits0 = 0; // 2-bit multiplicities, thresholds 10-17, cable JET2
  int Jet2BitHits1 = 0; // 2-bit multiplicities, thresholds 18-24, cable JET2

  xAOD::CMXJetHitsContainer::const_iterator cmxJetIterator    = cmxJetHitsTES->begin(); 
  xAOD::CMXJetHitsContainer::const_iterator cmxJetIteratorEnd = cmxJetHitsTES->end(); 

  // Transmission check: system CMX -> CTP
  for (; cmxJetIterator != cmxJetIteratorEnd; ++cmxJetIterator) {
    const uint32_t source = (*cmxJetIterator)->sourceComponent();
    const uint32_t crate  = (*cmxJetIterator)->crate(); 
    
    if (source == xAOD::CMXJetHits::TOTAL_MAIN && crate == 1) {                                               
      Jet3BitHits0 = (*cmxJetIterator)->hits0();
      Jet3BitHits1 = (*cmxJetIterator)->hits1();
      Jet3BitHits  = Jet3BitHits0 + (Jet3BitHits1<<15);
    }
    if (source == xAOD::CMXJetHits::TOTAL_FORWARD && crate == 1) {
      Jet2BitHits0 = (*cmxJetIterator)->hits0();
      Jet2BitHits1 = (*cmxJetIterator)->hits1();
      Jet2BitHits  = Jet2BitHits0 + (Jet2BitHits1<<16);
    }
  }

  // Retrieve CMX Et sums from Storegate
  const xAOD::CMXEtSumsContainer* cmxEtSumsTES = 0;                  
  sc = evtStore()->retrieve(cmxEtSumsTES, m_cmxEtSumsLocation);
  if (sc == StatusCode::FAILURE || !cmxEtSumsTES) {
    msg(MSG::INFO) << "No CMXEtSums found in TES at " << m_cmxEtSumsLocation
                   << endreq;
    return StatusCode::SUCCESS;
  }

  int TEHits     = 0; // Cable EN1 (full eta)
  int XEHits     = 0;
  int XSHits     = 0;
  int TERestHits = 0; // Cable EN2 (restricted eta)
  int XERestHits = 0;

  xAOD::CMXEtSumsContainer::const_iterator cmxEtSumsIterator    = cmxEtSumsTES->begin();
  xAOD::CMXEtSumsContainer::const_iterator cmxEtSumsIteratorEnd = cmxEtSumsTES->end(); 

  for (; cmxEtSumsIterator != cmxEtSumsIteratorEnd; ++ cmxEtSumsIterator) {
    const uint32_t source = (*cmxEtSumsIterator)->sourceComponent();
    const uint32_t crate  = (*cmxEtSumsIterator)->crate();

    // Sum Et hits 
    if (source == xAOD::CMXEtSums::SUM_ET_STANDARD && crate == 1) { // KW crate check might not be needed here...
      TEHits = (*cmxEtSumsIterator)->et();
    }

    // Missing Et hits 
    if (source == xAOD::CMXEtSums::MISSING_ET_STANDARD && crate == 1) {
      XEHits = (*cmxEtSumsIterator)->et();
    }

    // Missing Et significance hits 
    if (source == xAOD::CMXEtSums::MISSING_ET_SIG_STANDARD && crate == 1) {
      XSHits = (*cmxEtSumsIterator)->et();
    }

    // Sum Et hits (restricted eta)
    if (source == xAOD::CMXEtSums::SUM_ET_RESTRICTED && crate == 1) {  
      TERestHits = (*cmxEtSumsIterator)->et();
    }

    // Missing Et hits (restricted eta)
    if (source == xAOD::CMXEtSums::MISSING_ET_RESTRICTED && crate == 1) {
      XERestHits = (*cmxEtSumsIterator)->et();
    }
  }

  //--------------------------------------------------------------------------
  //----------------------- get the TIPs (input for CTP) ---------------------
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
  int totalBits = threshBits*def.max_EM_Threshold_Number()/2;  
  msg(MSG::DEBUG) << "totalBits = " << totalBits << endreq;

  compare(bunch, EMHits0, totalBits, offset, EM1Type); // Cable EM1

  offset += totalBits;
  compare(bunch, EMHits1, totalBits, offset, EM2Type); // Cable EM2

  //----------------------- Tau Hits (3 bits / thresh) ---------------------
  offset += totalBits;
  compare(bunch, TauHits0, totalBits, offset, Tau1Type); // Cable TAU1

  offset += totalBits; 
  compare(bunch, TauHits1, totalBits, offset, Tau2Type); // Cable TAU2

  //------------------------ Jet Hits (3 bits / thresh) ---------------------
  offset += totalBits;
  totalBits = threshBits*max_JET_3bit_Threshold_Number;    
  compare(bunch, Jet3BitHits, totalBits, offset, Jet3BitType); // Cable JET1
  
  //----------------------- Jet Hits (2 bits / thresh) -------------------
  offset += totalBits;
  threshBits--;
  totalBits = threshBits*max_JET_2bit_Threshold_Number;
  compare(bunch, Jet2BitHits, totalBits, offset, Jet2BitType); // Cable JET2       

  //---------------------- TE Hits (1 bit / thresh) ------------------
  offset += totalBits;
  threshBits--;
  totalBits = threshBits*def.max_TE_Threshold_Number()/2;  // Cable EN1 
  compare(bunch, TEHits, totalBits, offset, TEFullEtaType);
  
  //------------------------ XE Hits (1 bit / thresh) --------------------
  offset += totalBits;
  totalBits = threshBits*def.max_XE_Threshold_Number()/2;  
  compare(bunch, XEHits, totalBits, offset, XEFullEtaType);
  
  //---------------------- XS Hits (1 bit / thresh) ------------------
  offset += totalBits;
  totalBits = threshBits*def.max_XS_Threshold_Number();   
  compare(bunch, XSHits, totalBits, offset, XSType);
  
  //------------------------ Restricted Eta TE Hits (1 bit / thresh) --------------------
  offset += totalBits;
  totalBits = threshBits*def.max_TE_Threshold_Number()/2;  // Cable EN2
  compare(bunch, TERestHits, totalBits, offset, TERestrictedEtaType);
  
  //---------------------- Restricted Eta XE Hits (1 bit / thresh) ------------------
  offset += totalBits;
  totalBits = threshBits*def.max_XE_Threshold_Number()/2; 
  compare(bunch, XERestHits, totalBits, offset, XERestrictedEtaType);

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
    msg(MSG::DEBUG) << "offset: " << offset << " totalBits: " << totalBits << endreq;  
    std::string name(m_h_ctp_1d_L1CaloNeCTPSummary->GetXaxis()->GetBinLabel(1+type));
    std::string subdet((type == EM1Type  || type == EM2Type ||
                        type == Tau1Type || type == Tau2Type) ? " CP:  "
                                                              : " JEP: ");
    msg(MSG::DEBUG) << name << subdet
                    << m_histTool->thresholdString(hits, totalBits)
		    << endreq;
    msg(MSG::DEBUG) << name << " CTP: ";
  }
  
  int mask    = 0;
  int tipHits = 0;
  
  for (int bit = 0; bit < totalBits; ++bit) {
    const int TIPid = (m_tipMap[offset + bit]).second;
    if (TIPid < 0) {
      if (m_debug) msg(MSG::DEBUG) << "X ";
      continue;
    }
    const int HITbit= ((hits >> bit) & 0x1);
    const int TIPbit = bunch.getTIP().test( TIPid );

    if (m_debug) msg(MSG::DEBUG) << TIPbit << " ";  
    tipHits |= (TIPbit << bit);
    mask    |= (1 << bit);
    
    if (HITbit && HITbit == TIPbit) m_h_ctp_1d_TIPMatches->Fill(TIPid);
    else if (HITbit != TIPbit) {
      if (HITbit) m_h_ctp_1d_HitNoTIPMismatch->Fill(TIPid);
      else        m_h_ctp_1d_TIPNoHitMismatch->Fill(TIPid);
    }
  }
  if (m_debug) msg(MSG::DEBUG) << endreq;
  
  if (tipHits != (hits&mask)) {
    m_h_ctp_1d_L1CaloNeCTPSummary->Fill(type);
    m_histTool->fillEventNumber(m_h_ctp_2d_MismatchEvents, type);
   } 
  else if (tipHits) m_h_ctp_1d_L1CaloEqCTPSummary->Fill(type);
 
}


void L1CaloCTPMon::setLabels(LWHist* hist, bool xAxis)
{
  LWHist::LWHistAxis* axis = (xAxis) ? hist->GetXaxis() : hist->GetYaxis();
  axis->SetBinLabel(1+EM1Type,             "EM1");
  axis->SetBinLabel(1+EM2Type,             "EM2");
  axis->SetBinLabel(1+Tau1Type,            "Tau1");
  axis->SetBinLabel(1+Tau2Type,            "Tau2");
  axis->SetBinLabel(1+Jet3BitType,         "Jet1 (3-bit)");
  axis->SetBinLabel(1+Jet2BitType,         "Jet2 (2-bit)"); 
  axis->SetBinLabel(1+TEFullEtaType,       "TE (full eta)");  
  axis->SetBinLabel(1+XEFullEtaType,       "XE (full eta)");
  axis->SetBinLabel(1+XSType,              "XS");
  axis->SetBinLabel(1+TERestrictedEtaType, "TE (restr. eta)");
  axis->SetBinLabel(1+XERestrictedEtaType, "XE (restr. eta)");
}
// ============================================================================
}  // end namespace
// ============================================================================
