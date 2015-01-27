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
    m_h_ppm_em_2d_etaPhi_tt_lutCp_SimEqData(0),
    m_h_ppm_em_2d_etaPhi_tt_lutCp_SimNeData(0),
    m_h_ppm_em_2d_etaPhi_tt_lutCp_SimNoData(0),
    m_h_ppm_em_2d_etaPhi_tt_lutCp_DataNoSim(0),
    m_h_ppm_had_2d_etaPhi_tt_lutCp_SimEqData(0),
    m_h_ppm_had_2d_etaPhi_tt_lutCp_SimNeData(0),
    m_h_ppm_had_2d_etaPhi_tt_lutCp_SimNoData(0),
    m_h_ppm_had_2d_etaPhi_tt_lutCp_DataNoSim(0),
    m_h_ppm_em_2d_etaPhi_tt_lutJep_SimEqData(0),
    m_h_ppm_em_2d_etaPhi_tt_lutJep_SimNeData(0),
    m_h_ppm_em_2d_etaPhi_tt_lutJep_SimNoData(0),
    m_h_ppm_em_2d_etaPhi_tt_lutJep_DataNoSim(0),
    m_h_ppm_had_2d_etaPhi_tt_lutJep_SimEqData(0),
    m_h_ppm_had_2d_etaPhi_tt_lutJep_SimNeData(0),
    m_h_ppm_had_2d_etaPhi_tt_lutJep_SimNoData(0),
    m_h_ppm_had_2d_etaPhi_tt_lutJep_DataNoSim(0),
    m_h_ppm_2d_LUT_MismatchEvents_cr0cr1(0),
    m_h_ppm_2d_LUT_MismatchEvents_cr2cr3(0),
    m_h_ppm_2d_LUT_MismatchEvents_cr4cr5(0),
    m_h_ppm_2d_LUT_MismatchEvents_cr6cr7(0)
/*---------------------------------------------------------*/
{
  declareProperty("TriggerTowerLocation",
                 m_triggerTowerLocation =
		  LVL1::TrigT1CaloDefs::xAODTriggerTowerLocation);
  
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

  m_h_ppm_em_2d_etaPhi_tt_lutCp_SimEqData = m_histTool->bookPPMEmEtaVsPhi(
    "ppm_em_2d_etaPhi_tt_lutCp_SimEqData",
    "PPM LUT-CP EM Data/Simulation Non-zero Matches");
  m_h_ppm_em_2d_etaPhi_tt_lutCp_SimNeData = m_histTool->bookPPMEmEtaVsPhi(
    "ppm_em_2d_etaPhi_tt_lutCp_SimNeData",
    "PPM LUT-CP EM Data/Simulation Non-zero Mismatches");
  m_h_ppm_em_2d_etaPhi_tt_lutCp_SimNoData = m_histTool->bookPPMEmEtaVsPhi(
    "ppm_em_2d_etaPhi_tt_lutCp_SimNoData",
    "PPM LUT-CP EM Simulation but no Data");
  m_h_ppm_em_2d_etaPhi_tt_lutCp_DataNoSim = m_histTool->bookPPMEmEtaVsPhi(
    "ppm_em_2d_etaPhi_tt_lutCp_DataNoSim",
    "PPM LUT-CP EM Data but no Simulation");
  m_h_ppm_had_2d_etaPhi_tt_lutCp_SimEqData = m_histTool->bookPPMHadEtaVsPhi(
    "ppm_had_2d_etaPhi_tt_lutCp_SimEqData",
    "PPM LUT-CP HAD Data/Simulation Non-zero Matches");
  m_h_ppm_had_2d_etaPhi_tt_lutCp_SimNeData = m_histTool->bookPPMHadEtaVsPhi(
    "ppm_had_2d_etaPhi_tt_lutCp_SimNeData",
    "PPM LUT-CP HAD Data/Simulation Non-zero Mismatches");
  m_h_ppm_had_2d_etaPhi_tt_lutCp_SimNoData = m_histTool->bookPPMHadEtaVsPhi(
    "ppm_had_2d_etaPhi_tt_lutCp_SimNoData",
    "PPM LUT-CP HAD Simulation but no Data");
  m_h_ppm_had_2d_etaPhi_tt_lutCp_DataNoSim = m_histTool->bookPPMHadEtaVsPhi(
    "ppm_had_2d_etaPhi_tt_lutCp_DataNoSim",
    "PPM LUT-CP HAD Data but no Simulation");

  m_h_ppm_em_2d_etaPhi_tt_lutJep_SimEqData = m_histTool->bookPPMEmEtaVsPhi(
    "ppm_em_2d_etaPhi_tt_lutJep_SimEqData",
    "PPM LUT-JEP EM Data/Simulation Non-zero Matches");
  m_h_ppm_em_2d_etaPhi_tt_lutJep_SimNeData = m_histTool->bookPPMEmEtaVsPhi(
    "ppm_em_2d_etaPhi_tt_lutJep_SimNeData",
    "PPM LUT-JEP EM Data/Simulation Non-zero Mismatches");
  m_h_ppm_em_2d_etaPhi_tt_lutJep_SimNoData = m_histTool->bookPPMEmEtaVsPhi(
    "ppm_em_2d_etaPhi_tt_lutJep_SimNoData",
    "PPM LUT-JEP EM Simulation but no Data");
  m_h_ppm_em_2d_etaPhi_tt_lutJep_DataNoSim = m_histTool->bookPPMEmEtaVsPhi(
    "ppm_em_2d_etaPhi_tt_lutJep_DataNoSim",
    "PPM LUT-JEP EM Data but no Simulation");
  m_h_ppm_had_2d_etaPhi_tt_lutJep_SimEqData = m_histTool->bookPPMHadEtaVsPhi(
    "ppm_had_2d_etaPhi_tt_lutJep_SimEqData",
    "PPM LUT-JEP HAD Data/Simulation Non-zero Matches");
  m_h_ppm_had_2d_etaPhi_tt_lutJep_SimNeData = m_histTool->bookPPMHadEtaVsPhi(
    "ppm_had_2d_etaPhi_tt_lutJep_SimNeData",
    "PPM LUT-JEP HAD Data/Simulation Non-zero Mismatches");
  m_h_ppm_had_2d_etaPhi_tt_lutJep_SimNoData = m_histTool->bookPPMHadEtaVsPhi(
    "ppm_had_2d_etaPhi_tt_lutJep_SimNoData",
    "PPM LUT-JEP HAD Simulation but no Data");
  m_h_ppm_had_2d_etaPhi_tt_lutJep_DataNoSim = m_histTool->bookPPMHadEtaVsPhi(
    "ppm_had_2d_etaPhi_tt_lutJep_DataNoSim",
    "PPM LUT-JEP HAD Data but no Simulation");
							
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
  const xAOD::TriggerTowerContainer* triggerTowerTES = 0; 
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

void PPMSimBSMon::simulateAndCompare(const xAOD::TriggerTowerContainer* ttIn)
{
  if (m_debug) msg(MSG::DEBUG) << "Simulate LUT data from FADC data" << endreq;

  StatusCode sc = m_ttTool->retrieveConditions();
  if (sc.isFailure()) return;

  const int nCrates = 8;
  ErrorVector crateError(nCrates);
  ErrorVector moduleError(nCrates);
 
  std::vector<int> Lut;
  std::vector<int> BcidR;
  std::vector<int> BcidD;
  
  m_ttTool->setDebug(false);
  xAOD::TriggerTowerContainer::const_iterator iter  = ttIn->begin();
  xAOD::TriggerTowerContainer::const_iterator iterE = ttIn->end();

  for (; iter != iterE; ++iter) {
    
    const xAOD::TriggerTower* tt = *iter;
    const double eta = tt->eta();
    const double phi = tt->phi(); 
    const int datCp = tt->cpET();
    const int datJep = tt->jepET();
    const std::vector<uint_least16_t>& ADC(tt->adc());
    const int Slices = ADC.size();
    const int Peak = tt->adcPeak();
    int simCp = 0;
    int simJep = 0;
    
    bool keep = true;
    if (datCp == 0) {
      keep = false;
      std::vector<uint_least16_t>::const_iterator it1 = ADC.begin();
      std::vector<uint_least16_t>::const_iterator itE = ADC.end();
      for (;it1 != itE; ++it1) {
	if (*it1 >= m_simulationADCCut) {
	  keep = true;
	  break;
	}
      }
    }
    
    if (keep) {
      Lut.clear();
      BcidR.clear();
      BcidD.clear();
      const L1CaloCoolChannelId coolId(m_ttTool->channelID(eta, phi, tt->layer()));
      m_ttTool->process(PPMSimBSMon::convertVectorType<int>(ADC), coolId, Lut, BcidR, BcidD);
      if (Slices < 7 || BcidD[Peak]) simCp = Lut[Peak];
      if (m_debug && simCp != datCp && (Slices >= 7 || datCp != 0)) { // mismatch - repeat with debug on
	std::vector<int> Lut2; 
	std::vector<int> BcidR2;
	std::vector<int> BcidD2;
	m_ttTool->setDebug(true);
	m_ttTool->process(PPMSimBSMon::convertVectorType<int>(ADC), coolId, Lut2, BcidR2, BcidD2);
	m_ttTool->setDebug(false);
      }
    }
    
    if (!simCp && !datCp) continue;
    
    simJep = simCp*2;
    
    //=====================FOR ELECTROMAGNETIC LAYER============================
    if (tt->layer() == 0) {

      //  Fill in error plots
      //------------------For LUT-CP------------------------------------------------
      int mismatch = 0;
      TH2F_LW* hist1 = 0;
      if (simCp && simCp == datCp) { // non-zero match
	hist1 = m_h_ppm_em_2d_etaPhi_tt_lutCp_SimEqData;
      } else if (simCp != datCp) {  // mis-match
	mismatch = 1;
	if (simCp && datCp) {       // non-zero mis-match
	  hist1 = m_h_ppm_em_2d_etaPhi_tt_lutCp_SimNeData;
	} else if (!datCp) {        // no data
	  if (Slices >= 7) {
	    hist1 = m_h_ppm_em_2d_etaPhi_tt_lutCp_SimNoData;
	  } else mismatch = 0;
	} else {                    // no sim
	  hist1 = m_h_ppm_em_2d_etaPhi_tt_lutCp_DataNoSim;
	}
	if (m_debug) {
	  msg(MSG::DEBUG) << " EMTowerMismatch eta/phi/sim/dat: "
		<< eta << "/" << phi << "/" << simCp << "/" << datCp << endreq;
	}
      }
      
      if (hist1) m_histTool->fillPPMEmEtaVsPhi(hist1, eta, phi);
      
      if (mismatch == 1) {
	const L1CaloCoolChannelId coolId(m_ttTool->channelID(eta, phi, tt->layer()));
	const int crate  = coolId.crate();
	const int module = coolId.module();
	crateError[crate] = 1;
	if (!((moduleError[crate]>>module)&0x1)) {
	  fillEventSample(crate, module);
	  moduleError[crate] |= (1 << module);
	}
      }    
      
      //--------------FOR LUT-JEP------------------------------------------------------
      mismatch = 0;
      hist1 = 0;
      if (simJep && simJep == datJep) { // non-zero match
	hist1 = m_h_ppm_em_2d_etaPhi_tt_lutJep_SimEqData;
      } else if (simJep != datJep) {  // mis-match
	mismatch = 1;
	if (simJep && datJep) {       // non-zero mis-match
	  hist1 = m_h_ppm_em_2d_etaPhi_tt_lutJep_SimNeData;
	} else if (!datJep) {        // no data
	  if (Slices >= 7) {
	    hist1 = m_h_ppm_em_2d_etaPhi_tt_lutJep_SimNoData;
	  } else mismatch = 0;
	} else {                    // no sim
	  hist1 = m_h_ppm_em_2d_etaPhi_tt_lutJep_DataNoSim;
	}
	if (m_debug) {
	  msg(MSG::DEBUG) << " EMTowerMismatch eta/phi/sim/dat: "
		<< eta << "/" << phi << "/" << simJep << "/" << datJep << endreq;
	}
      }
      
      if (hist1) m_histTool->fillPPMEmEtaVsPhi(hist1, eta, phi);
      
      if (mismatch == 1) {
	const L1CaloCoolChannelId coolId(m_ttTool->channelID(eta, phi, tt->layer()));
	const int crate  = coolId.crate();
	const int module = coolId.module();
	crateError[crate] = 1;
	if (!((moduleError[crate]>>module)&0x1)) {
	  fillEventSample(crate, module);
	  moduleError[crate] |= (1 << module);
	}
      } 

    }
    
    //=====================FOR HADRONIC LAYER============================
    if (tt->layer() == 1) {

      //  Fill in error plots
      //------------------FOR LUT-CPP----------------------------------------
      int mismatch = 0;
      
      TH2F_LW* hist1 = 0;
      if (simCp && simCp == datCp) { // non-zero match
	hist1 = m_h_ppm_had_2d_etaPhi_tt_lutCp_SimEqData;
      } else if (simCp != datCp) {  // mis-match
	mismatch = 1;
	if (simCp && datCp) {       // non-zero mis-match
	  hist1 = m_h_ppm_had_2d_etaPhi_tt_lutCp_SimNeData;
	} else if (!datCp) {        // no data
	  if (Slices >= 7) {
	    hist1 = m_h_ppm_had_2d_etaPhi_tt_lutCp_SimNoData;
	  } else mismatch = 0;
	} else {                    // no sim
	  hist1 = m_h_ppm_had_2d_etaPhi_tt_lutCp_DataNoSim;
	}
	if (m_debug) {
	  msg(MSG::DEBUG) << " EMTowerMismatch eta/phi/sim/dat: "
		<< eta << "/" << phi << "/" << simCp << "/" << datCp << endreq;
	}
      }
      
      if (hist1) m_histTool->fillPPMHadEtaVsPhi(hist1, eta, phi);
      
      if (mismatch == 1) {
	const L1CaloCoolChannelId coolId(m_ttTool->channelID(eta, phi, tt->layer()));
	const int crate  = coolId.crate();
	const int module = coolId.module();
	crateError[crate] = 1;
	if (!((moduleError[crate]>>module)&0x1)) {
	  fillEventSample(crate, module);
	  moduleError[crate] |= (1 << module);
	}
      }

      //-----------------------FOR LUT-JEP--------------------------------
      mismatch = 0;
      hist1 = 0;
      if (simJep && simJep == datJep) { // non-zero match
	hist1 = m_h_ppm_had_2d_etaPhi_tt_lutJep_SimEqData;
      } else if (simJep != datJep) {  // mis-match
	mismatch = 1;
	if (simJep && datJep) {       // non-zero mis-match
	  hist1 = m_h_ppm_had_2d_etaPhi_tt_lutJep_SimNeData;
	} else if (!datJep) {        // no data
	  if (Slices >= 7) {
	    hist1 = m_h_ppm_had_2d_etaPhi_tt_lutJep_SimNoData;
	  } else mismatch = 0;
	} else {                    // no sim
	  hist1 = m_h_ppm_had_2d_etaPhi_tt_lutJep_DataNoSim;
	}
	if (m_debug) {
	  msg(MSG::DEBUG) << " EMTowerMismatch eta/phi/sim/dat: "
		<< eta << "/" << phi << "/" << simJep << "/" << datJep << endreq;
	}
      }
      
      if (hist1) m_histTool->fillPPMEmEtaVsPhi(hist1, eta, phi);
      
      if (mismatch == 1) {
	const L1CaloCoolChannelId coolId(m_ttTool->channelID(eta, phi, tt->layer()));
	const int crate  = coolId.crate();
	const int module = coolId.module();
	crateError[crate] = 1;
	if (!((moduleError[crate]>>module)&0x1)) {
	  fillEventSample(crate, module);
	  moduleError[crate] |= (1 << module);
	}
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
