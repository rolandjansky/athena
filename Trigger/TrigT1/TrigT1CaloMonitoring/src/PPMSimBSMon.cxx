/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     PPMSimBSMon.cxx
// PACKAGE:  TrigT1CaloMonitoring  
//
// AUTHORS:  Peter Faulkner
//           Sky French          
//
// ********************************************************************

#include <cmath>

#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "SGTools/StlVectorClids.h"

#include "AthenaMonitoring/AthenaMonManager.h"

#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigT1CaloUtils/TriggerTowerKey.h"
#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"

#include "TrigT1CaloMonitoring/PPMSimBSMon.h"

/*---------------------------------------------------------*/
PPMSimBSMon::PPMSimBSMon(const std::string & type, 
			 const std::string & name,
			 const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent),
    m_ttTool("LVL1::L1TriggerTowerTool/L1TriggerTowerTool"), 
    m_errorTool("TrigT1CaloMonErrorTool"),
    m_histTool("TrigT1CaloLWHistogramTool"),
    m_debug(false), m_events(0),
    m_histBooked(false),
    m_h_ppm_em_2d_etaPhi_tt_lut_SimEqData(0),
    m_h_ppm_em_2d_etaPhi_tt_lut_SimNeData(0),
    m_h_ppm_em_2d_etaPhi_tt_lut_SimNoData(0),
    m_h_ppm_em_2d_etaPhi_tt_lut_DataNoSim(0),
    m_h_ppm_had_2d_etaPhi_tt_lut_SimEqData(0),
    m_h_ppm_had_2d_etaPhi_tt_lut_SimNeData(0),
    m_h_ppm_had_2d_etaPhi_tt_lut_SimNoData(0),
    m_h_ppm_had_2d_etaPhi_tt_lut_DataNoSim(0),
    m_h_ppm_2d_LUT_MismatchEvents_cr0cr1(0),
    m_h_ppm_2d_LUT_MismatchEvents_cr2cr3(0),
    m_h_ppm_2d_LUT_MismatchEvents_cr4cr5(0),
    m_h_ppm_2d_LUT_MismatchEvents_cr6cr7(0)
/*---------------------------------------------------------*/
{
  declareProperty("TriggerTowerLocation",
                 m_triggerTowerLocation =
		  LVL1::TrigT1CaloDefs::TriggerTowerLocation);
  
  declareProperty("RootDirectory", m_rootDir = "L1Calo");

  declareProperty("SimulationADCCut", m_simulationADCCut = 36,
                  "Minimum ADC cut to avoid unnecessary simulation");
}

/*---------------------------------------------------------*/
PPMSimBSMon::~PPMSimBSMon()
/*---------------------------------------------------------*/
{
}

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

/*---------------------------------------------------------*/
StatusCode PPMSimBSMon:: initialize()
/*---------------------------------------------------------*/
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endreq;
  m_debug = msgLvl(MSG::DEBUG);

  StatusCode sc;

  sc = ManagedMonitorToolBase::initialize();
  if (sc.isFailure()) return sc;

  sc = m_ttTool.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool L1TriggerTowerTool" << endreq;
    return sc;
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
StatusCode PPMSimBSMon:: finalize()
/*---------------------------------------------------------*/
{
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode PPMSimBSMon::bookHistogramsRecurrent()
/*---------------------------------------------------------*/
{
  msg(MSG::DEBUG) << "bookHistograms entered" << endreq;

  if( m_environment == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
  }
  	
  if( m_dataType == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }

  if ( newLumiBlock ) { }
  
  if ( newRun ) {

  MgmtAttr_t attr = ATTRIB_UNMANAGED;
  std::string dir(m_rootDir + "/PPM/Errors/Data_Simulation");
  MonGroup monPPM   ( this, dir + "/PPMLUTSim", run, attr );
  MonGroup monEvent ( this, dir + "/MismatchEventNumbers", run, attr, "", "eventSample" );

  // LUT

  m_histTool->setMonGroup(&monPPM);

  m_h_ppm_em_2d_etaPhi_tt_lut_SimEqData = m_histTool->bookPPMEmEtaVsPhi(
    "ppm_em_2d_etaPhi_tt_lut_SimEqData",
    "PPM LUT EM Data/Simulation Non-zero Matches");
  m_h_ppm_em_2d_etaPhi_tt_lut_SimNeData = m_histTool->bookPPMEmEtaVsPhi(
    "ppm_em_2d_etaPhi_tt_lut_SimNeData",
    "PPM LUT EM Data/Simulation Non-zero Mismatches");
  m_h_ppm_em_2d_etaPhi_tt_lut_SimNoData = m_histTool->bookPPMEmEtaVsPhi(
    "ppm_em_2d_etaPhi_tt_lut_SimNoData",
    "PPM LUT EM Simulation but no Data");
  m_h_ppm_em_2d_etaPhi_tt_lut_DataNoSim = m_histTool->bookPPMEmEtaVsPhi(
    "ppm_em_2d_etaPhi_tt_lut_DataNoSim",
    "PPM LUT EM Data but no Simulation");
  m_h_ppm_had_2d_etaPhi_tt_lut_SimEqData = m_histTool->bookPPMHadEtaVsPhi(
    "ppm_had_2d_etaPhi_tt_lut_SimEqData",
    "PPM LUT HAD Data/Simulation Non-zero Matches");
  m_h_ppm_had_2d_etaPhi_tt_lut_SimNeData = m_histTool->bookPPMHadEtaVsPhi(
    "ppm_had_2d_etaPhi_tt_lut_SimNeData",
    "PPM LUT HAD Data/Simulation Non-zero Mismatches");
  m_h_ppm_had_2d_etaPhi_tt_lut_SimNoData = m_histTool->bookPPMHadEtaVsPhi(
    "ppm_had_2d_etaPhi_tt_lut_SimNoData",
    "PPM LUT HAD Simulation but no Data");
  m_h_ppm_had_2d_etaPhi_tt_lut_DataNoSim = m_histTool->bookPPMHadEtaVsPhi(
    "ppm_had_2d_etaPhi_tt_lut_DataNoSim",
    "PPM LUT HAD Data but no Simulation");
							
  // Mismatch Event Number Histograms

  m_histTool->setMonGroup(&monEvent);

  m_h_ppm_2d_LUT_MismatchEvents_cr0cr1 = m_histTool->bookPPMEventVsCrateModule(
    "ppm_2d_LUT_MismatchEvents_cr0cr1","PPM LUT Mismatch Event Numbers",0,1);
  m_h_ppm_2d_LUT_MismatchEvents_cr2cr3 = m_histTool->bookPPMEventVsCrateModule(
    "ppm_2d_LUT_MismatchEvents_cr2cr3","PPM LUT Mismatch Event Numbers",2,3);
  m_h_ppm_2d_LUT_MismatchEvents_cr4cr5 = m_histTool->bookPPMEventVsCrateModule(
    "ppm_2d_LUT_MismatchEvents_cr4cr5","PPM LUT Mismatch Event Numbers",4,5);
  m_h_ppm_2d_LUT_MismatchEvents_cr6cr7 = m_histTool->bookPPMEventVsCrateModule(
    "ppm_2d_LUT_MismatchEvents_cr6cr7","PPM LUT Mismatch Event Numbers",6,7);

  m_histTool->unsetMonGroup();
  m_histBooked = true;

  } // end if (newRun ...

  msg(MSG::DEBUG) << "Leaving bookHistograms" << endreq;
  
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode PPMSimBSMon::fillHistograms()
/*---------------------------------------------------------*/
{
  if (m_debug) msg(MSG::DEBUG) << "fillHistograms entered" << endreq;

  if (!m_histBooked) {
    if (m_debug) msg(MSG::DEBUG) << "Histogram(s) not booked" << endreq;
    return StatusCode::SUCCESS;
  }

  // Skip events believed to be corrupt

  if (m_errorTool->corrupt()) {
    if (m_debug) msg(MSG::DEBUG) << "Skipping corrupt event" << endreq;
    return StatusCode::SUCCESS;
  }

  StatusCode sc;

  //Retrieve Trigger Towers from SG
  const TriggerTowerCollection* triggerTowerTES = 0; 
  sc = evtStore()->retrieve(triggerTowerTES, m_triggerTowerLocation); 
  if( sc.isFailure()  ||  !triggerTowerTES ) {
    if (m_debug) msg(MSG::DEBUG) << "No Trigger Tower container found"<< endreq; 
  }
  ++m_events;

  // Compare LUT simulated from FADC with LUT from data
 
  if (triggerTowerTES) {
    simulateAndCompare(triggerTowerTES);
  }
  
  if (m_debug) msg(MSG::DEBUG) << "Leaving fillHistograms" << endreq;

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode PPMSimBSMon::procHistograms()
/*---------------------------------------------------------*/
{
  msg(MSG::DEBUG) << "procHistograms entered" << endreq;

  if (endOfLumiBlock) {
  }

  if (endOfRun) {
  }

  return StatusCode::SUCCESS;
}

void PPMSimBSMon::simulateAndCompare(const TriggerTowerCollection* ttIn)
{
  if (m_debug) msg(MSG::DEBUG) << "Simulate LUT data from FADC data" << endreq;

  StatusCode sc = m_ttTool->retrieveConditions();
  if (sc.isFailure()) return;

  const int nCrates = 8;
  ErrorVector crateError(nCrates);
  ErrorVector moduleError(nCrates);
 
  std::vector<int> emLut;
  std::vector<int> emBcidR;
  std::vector<int> emBcidD;
  std::vector<int> hadLut;
  std::vector<int> hadBcidR;
  std::vector<int> hadBcidD;
  
  m_ttTool->setDebug(false);
  TriggerTowerCollection::const_iterator iter  = ttIn->begin();
  TriggerTowerCollection::const_iterator iterE = ttIn->end();

  for (; iter != iterE; ++iter) {
    
    const LVL1::TriggerTower* tt = *iter;
    const std::vector<int>& emADC(tt->emADC());
    const std::vector<int>& hadADC(tt->hadADC());
    const double eta = tt->eta();
    const double phi = tt->phi();

    int simEm = 0;
    const int datEm = tt->emEnergy();
    const int emSlices = emADC.size();
    bool keep = true;
    if (datEm == 0) {
      keep = false;
      std::vector<int>::const_iterator it1 = emADC.begin();
      std::vector<int>::const_iterator itE = emADC.end();
      for (;it1 != itE; ++it1) {
        if (*it1 >= m_simulationADCCut) {
          keep = true;
	  break;
        }
      }
    }
    if (keep) {
      emLut.clear();
      emBcidR.clear();
      emBcidD.clear();
      const int emPeak = tt->emADCPeak();
      const L1CaloCoolChannelId em_coolId(m_ttTool->channelID(eta, phi, 0));
      m_ttTool->process(emADC, em_coolId, emLut, emBcidR, emBcidD);
      if (emSlices < 7 || emBcidD[emPeak]) simEm = emLut[emPeak];
      if (m_debug && simEm != datEm && (emSlices >= 7 || datEm != 0)) { // mismatch - repeat with debug on
        std::vector<int> emLut2; 
        std::vector<int> emBcidR2;
        std::vector<int> emBcidD2;
        m_ttTool->setDebug(true);
        m_ttTool->process(emADC, em_coolId, emLut2, emBcidR2, emBcidD2);
        m_ttTool->setDebug(false);
      }
    }
    
    int simHad = 0;
    const int datHad = tt->hadEnergy();
    const int hadSlices = hadADC.size();
    keep = true;
    if (datHad == 0) {
      keep = false;
      std::vector<int>::const_iterator it1 = hadADC.begin();
      std::vector<int>::const_iterator itE = hadADC.end();
      for (;it1 != itE; ++it1) {
        if (*it1 >= m_simulationADCCut) {
	  keep = true;
	  break;
        }
      }
    }
    if (keep) {
      hadLut.clear();
      hadBcidR.clear();
      hadBcidD.clear();
      const int hadPeak = tt->hadADCPeak();
      const L1CaloCoolChannelId had_coolId(m_ttTool->channelID(eta, phi, 1));
      m_ttTool->process(hadADC, had_coolId, hadLut, hadBcidR, hadBcidD);
      if (hadSlices < 7 || hadBcidD[hadPeak]) simHad = hadLut[hadPeak];
      if (m_debug && simHad != datHad && (hadSlices >= 7 || datHad !=0 )) {
        std::vector<int> hadLut2;
        std::vector<int> hadBcidR2;
        std::vector<int> hadBcidD2;
        m_ttTool->setDebug(true);
        m_ttTool->process(hadADC, had_coolId, hadLut2, hadBcidR2, hadBcidD2);
        m_ttTool->setDebug(false);
      }
    }
    
    if (!simEm && !simHad && !datEm && !datHad) continue;
    
    //  Fill in error plots
    
    int em_mismatch = 0;
    int had_mismatch = 0;
    
    TH2F_LW* hist1 = 0;
    if (simEm && simEm == datEm) { // non-zero match
      hist1 = m_h_ppm_em_2d_etaPhi_tt_lut_SimEqData;
    } else if (simEm != datEm) {  // mis-match
      em_mismatch = 1;
      if (simEm && datEm) {       // non-zero mis-match
        hist1 = m_h_ppm_em_2d_etaPhi_tt_lut_SimNeData;
      } else if (!datEm) {        // no data
	if (emSlices >= 7) {
	  hist1 = m_h_ppm_em_2d_etaPhi_tt_lut_SimNoData;
	} else em_mismatch = 0;
      } else {                    // no sim
	hist1 = m_h_ppm_em_2d_etaPhi_tt_lut_DataNoSim;
      }
      if (m_debug) {
        msg(MSG::DEBUG) << " EMTowerMismatch eta/phi/sim/dat: "
              << eta << "/" << phi << "/" << simEm << "/" << datEm << endreq;
      }
    }
    
    if (hist1) m_histTool->fillPPMEmEtaVsPhi(hist1, eta, phi);
    
    if (em_mismatch == 1) {
      const L1CaloCoolChannelId em_coolId(m_ttTool->channelID(eta, phi, 0));
      const int em_crate  = em_coolId.crate();
      const int em_module = em_coolId.module();
      crateError[em_crate] = 1;
      if (!((moduleError[em_crate]>>em_module)&0x1)) {
	fillEventSample(em_crate, em_module);
	moduleError[em_crate] |= (1 << em_module);
      }
    }
    
    hist1 = 0;
    if (simHad && simHad == datHad) { // non-zero match
      hist1 = m_h_ppm_had_2d_etaPhi_tt_lut_SimEqData;
    } else if (simHad != datHad) {   // mis-match
      had_mismatch = 1;
      if (simHad && datHad) {        // non-zero mis-match
        hist1 = m_h_ppm_had_2d_etaPhi_tt_lut_SimNeData;
      } else if (!datHad) {          // no data
	if (hadSlices >= 7) {
	  hist1 = m_h_ppm_had_2d_etaPhi_tt_lut_SimNoData;
	} else had_mismatch = 0;
      } else {                       // no sim
	hist1 = m_h_ppm_had_2d_etaPhi_tt_lut_DataNoSim;
      }
      if (m_debug) {
        msg(MSG::DEBUG) << " HadTowerMismatch eta/phi/sim/dat: "
              << eta << "/" << phi << "/" << simHad << "/" << datHad << endreq;
      }
    }

    if (hist1) m_histTool->fillPPMHadEtaVsPhi(hist1, eta, phi);
      
    if (had_mismatch == 1) {
      const L1CaloCoolChannelId had_coolId(m_ttTool->channelID(eta, phi, 1));
      const int had_crate  = had_coolId.crate();
      const int had_module = had_coolId.module();
      crateError[had_crate] = 1;
      if (!((moduleError[had_crate]>>had_module)&0x1)) {
	fillEventSample(had_crate, had_module);
	moduleError[had_crate] |= (1 << had_module);
      }
    }
  
  }    
    
  ErrorVector* save = new ErrorVector(crateError);
  sc = evtStore()->record(save, "L1CaloPPMMismatchVector");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Error recording PPM mismatch vector in TES "
	            << endreq;
  }
  
  m_ttTool->setDebug(true);
  
}

void PPMSimBSMon::fillEventSample(int crate, int module)
{
  const int y = module + 16*(crate%2);
  TH2I_LW* hist = 0;
  if     (crate==0 || crate==1) hist = m_h_ppm_2d_LUT_MismatchEvents_cr0cr1;
  else if(crate==2 || crate==3) hist = m_h_ppm_2d_LUT_MismatchEvents_cr2cr3;
  else if(crate==4 || crate==5) hist = m_h_ppm_2d_LUT_MismatchEvents_cr4cr5;
  else if(crate==6 || crate==7) hist = m_h_ppm_2d_LUT_MismatchEvents_cr6cr7;
  if (hist) m_histTool->fillEventNumber(hist, y);
}


