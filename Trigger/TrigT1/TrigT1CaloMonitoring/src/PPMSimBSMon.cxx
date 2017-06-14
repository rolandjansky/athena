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
#include "PPMSimBSMon.h"

#include <algorithm>
#include <cmath>

#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"

#include "SGTools/StlVectorClids.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "AthenaMonitoring/AthenaMonManager.h"

#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"
#include "TrigT1CaloMonitoringTools/ITrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

// ============================================================================
namespace LVL1 {
// ============================================================================
PPMSimBSMon::PPMSimBSMon(const std::string & type,
                         const std::string & name,
                         const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent),
    m_ttTool("LVL1::L1TriggerTowerTool/L1TriggerTowerTool"),
    m_errorTool("LVL1::TrigT1CaloMonErrorTool/TrigT1CaloMonErrorTool"),
    m_histTool("LVL1::TrigT1CaloLWHistogramTool/TrigT1CaloLWHistogramTool"),
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
    m_h_ppm_2d_etaPhi_tt_peakf_NonZeroMatches(0),
    m_h_ppm_2d_etaPhi_tt_peakf_ZeroMatches(0),
    m_h_ppm_2d_etaPhi_tt_peakf_SimNoData(0),
    m_h_ppm_2d_etaPhi_tt_peakf_DataNoSim(0),    
    m_h_ppm_2d_etaPhi_tt_satBcid_NonZeroMatches(0),
    m_h_ppm_2d_etaPhi_tt_satBcid_ZeroMatches(0),
    m_h_ppm_2d_etaPhi_tt_satBcid_SimNoData(0),
    m_h_ppm_2d_etaPhi_tt_satBcid_DataNoSim(0),  
    m_h_ppm_2d_LUT_MismatchEvents_cr0cr1(0),
    m_h_ppm_2d_LUT_MismatchEvents_cr2cr3(0),
    m_h_ppm_2d_LUT_MismatchEvents_cr4cr5(0),
    m_h_ppm_2d_LUT_MismatchEvents_cr6cr7(0)
    //m_h_ppm_2d_LUTJEP_MismatchEvents_cr0cr1(0),
    //m_h_ppm_2d_LUTJEP_MismatchEvents_cr2cr3(0),
    //m_h_ppm_2d_LUTJEP_MismatchEvents_cr4cr5(0),
    //m_h_ppm_2d_LUTJEP_MismatchEvents_cr6cr7(0)
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
  ATH_MSG_INFO("Initializing " << name() << " - package version "
                 << PACKAGE_VERSION);

  CHECK(ManagedMonitorToolBase::initialize());
  CHECK(m_ttTool.retrieve());
  CHECK(m_errorTool.retrieve());
  CHECK(m_histTool.retrieve());

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
  ATH_MSG_DEBUG("bookHistograms entered");

  if ( m_environment == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
  }

  if ( m_dataType == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }

  //if ( newLumiBlock ) { }

  if ( newRunFlag() ) {

    MgmtAttr_t attr = ATTRIB_UNMANAGED;
    std::string dir(m_rootDir + "/PPM/Errors/Data_Simulation");
    MonGroup monPPM   ( this, dir + "/PPMLUTSim", run, attr );
    MonGroup monPPMBcid ( this, dir + "/PPMBcidSim", run, attr );
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

    // BCID Sim Histograms

    m_histTool->setMonGroup(&monPPMBcid);    

    m_h_ppm_2d_etaPhi_tt_peakf_NonZeroMatches = m_histTool->bookPPMHadEtaVsPhi(
          "ppm_2d_etaPhi_tt_peakf_NonZeroMatches",
          "PPM Peakfinder Data/Simulation Non-Zero Matches");
    m_h_ppm_2d_etaPhi_tt_peakf_ZeroMatches = m_histTool->bookPPMHadEtaVsPhi(
          "ppm_2d_etaPhi_tt_peakf_ZeroMatches",
          "PPM Peakfinder Data/Simulation Zero Matches");
    m_h_ppm_2d_etaPhi_tt_peakf_SimNoData = m_histTool->bookPPMHadEtaVsPhi(
          "ppm_2d_etaPhi_tt_peakf_SimNoData",
          "PPM Peakfinder Simulation but no Data");
    m_h_ppm_2d_etaPhi_tt_peakf_DataNoSim = m_histTool->bookPPMHadEtaVsPhi(
          "ppm_2d_etaPhi_tt_peakf_DataNoSim",
          "PPM Peakfinder Data but no Simulation");
          
    m_h_ppm_2d_etaPhi_tt_satBcid_NonZeroMatches = m_histTool->bookPPMHadEtaVsPhi(
          "ppm_2d_etaPhi_tt_satBcid_NonZeroMatches",
          "PPM SaturatedBCID Data/Simulation Non-Zero Matches");
    m_h_ppm_2d_etaPhi_tt_satBcid_ZeroMatches = m_histTool->bookPPMHadEtaVsPhi(
          "ppm_2d_etaPhi_tt_satBcid_ZeroMatches",
          "PPM SaturatedBCID Data/Simulation Zero Matches");
    m_h_ppm_2d_etaPhi_tt_satBcid_SimNoData = m_histTool->bookPPMHadEtaVsPhi(
          "ppm_2d_etaPhi_tt_satBcid_SimNoData",
          "PPM SaturatedBCID Simulation but no Data");
    m_h_ppm_2d_etaPhi_tt_satBcid_DataNoSim = m_histTool->bookPPMHadEtaVsPhi(
          "ppm_2d_etaPhi_tt_satBcid_DataNoSim",
          "PPM SaturatedBCID Data but no Simulation");


    // Mismatch Event Number Histograms

    m_histTool->setMonGroup(&monEvent);

    m_h_ppm_2d_LUT_MismatchEvents_cr0cr1 = m_histTool->bookPPMEventVsCrateModule(
        "ppm_2d_LUT_MismatchEvents_cr0cr1", "PPM LUT Mismatch Event Numbers", 0, 1);
    m_h_ppm_2d_LUT_MismatchEvents_cr2cr3 = m_histTool->bookPPMEventVsCrateModule(
        "ppm_2d_LUT_MismatchEvents_cr2cr3", "PPM LUT Mismatch Event Numbers", 2, 3);
    m_h_ppm_2d_LUT_MismatchEvents_cr4cr5 = m_histTool->bookPPMEventVsCrateModule(
        "ppm_2d_LUT_MismatchEvents_cr4cr5", "PPM LUT Mismatch Event Numbers", 4, 5);
    m_h_ppm_2d_LUT_MismatchEvents_cr6cr7 = m_histTool->bookPPMEventVsCrateModule(
        "ppm_2d_LUT_MismatchEvents_cr6cr7", "PPM LUT Mismatch Event Numbers", 6, 7);

    m_histTool->unsetMonGroup();
    m_histBooked = true;

  } // end if (newRun ...

  ATH_MSG_DEBUG("Leaving bookHistograms");

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode PPMSimBSMon::fillHistograms()
/*---------------------------------------------------------*/
{
  ATH_MSG_DEBUG("fillHistograms entered");

  if (!m_histBooked) {
    ATH_MSG_DEBUG("Histogram(s) not booked");
    return StatusCode::SUCCESS;
  }

  // Skip events believed to be corrupt

  if (m_errorTool->corrupt()) {
    ATH_MSG_DEBUG("Skipping corrupt event");
    return StatusCode::SUCCESS;
  }

  //Retrieve Trigger Towers from SG
  const xAOD::TriggerTowerContainer* triggerTowerTES = nullptr;
  if (evtStore()->retrieve(triggerTowerTES, m_triggerTowerLocation).isFailure() ||
      !triggerTowerTES ) {
    ATH_MSG_WARNING("No xAOD::TriggerTowerContainer container found at " << m_triggerTowerLocation);
  }

  // Compare LUT simulated from FADC with LUT from data
  if (triggerTowerTES) {
    simulateAndCompare(triggerTowerTES);
  }

  ATH_MSG_DEBUG("Leaving fillHistograms");

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode PPMSimBSMon::procHistograms()
/*---------------------------------------------------------*/
{
  ATH_MSG_DEBUG("procHistograms entered");

  //if (endOfLumiBlock) {
  //}

  //if (endOfRun) {
  //}

  return StatusCode::SUCCESS;
}

namespace {
  template <typename DST, typename SRC>
  std::vector<DST> convertVectorType(const std::vector<SRC>& s) {
    using std::begin;
    using std::end;
    std::vector<DST> d(s.size());
    std::transform(begin(s), end(s), begin(d), [](SRC v){return static_cast<DST>(v);}); 
    return d;
  } 
} // anonymous namespace

void PPMSimBSMon::simulateAndCompare(const xAOD::TriggerTowerContainer* ttIn)
{
  ATH_MSG_DEBUG("Simulate LUT data from FADC data");

  if(m_ttTool->retrieveConditions().isFailure()) {
      REPORT_MESSAGE(MSG::WARNING);
      return;
  }

  bool isRun2 = true;
  const EventInfo* evInfo = nullptr;
  if (evtStore()->retrieve(evInfo).isFailure() || !evInfo) {
    ATH_MSG_DEBUG("No EventInfo found assuming Run-2");
  } else {
    const EventID* evID = evInfo->event_ID();
    if (evID && evID->run_number() < 253377) {isRun2 = false;}
  }

  const int nCrates = 8;
  ErrorVector crateError(nCrates);
  ErrorVector moduleError(nCrates);

  std::vector<int> LutCp; // == Lut for Run-1
  std::vector<int> LutJep; // unused for Run-1
  std::vector<int> BcidR;
  std::vector<int> BcidD;

  m_ttTool->setDebug(false);
  
  for (const xAOD::TriggerTower* tt : *ttIn) {
    const double eta = tt->eta();
    const double phi = tt->phi();
    const int datCp = tt->cpET();
    const int datJep = tt->lut_jep().empty() ? 0 : tt->jepET();
    const std::vector<uint_least16_t>& ADC = tt->adc();
    const int Slices = ADC.size();
    const int Peak = tt->adcPeak();
    bool pedCorrOverflow = false;
    const std::size_t nPedCorr = tt->correction().size();
    int simCp = 0;
    int simJep = 0;
    int simBcid = 0;
    uint8_t datBcid = 0;
    std::vector<uint8_t> datBcidVec = (tt->bcidVec());
    if ( datBcidVec.size() > 0) {
      datBcid = datBcidVec[tt->peak()];
    }

    //Check for over-/underflow of pedestalCorrection
    for(std::size_t i = 0; i < nPedCorr; ++i) {
      if(tt->correction()[i]>=511 or tt->correction()[i]<=-512){
        pedCorrOverflow = true;
        break;
       }
    }


    // only run simulation for non-empty TTs
    if(datCp || datJep || *std::max_element(std::begin(ADC), std::end(ADC)) >= m_simulationADCCut) {
      BcidR.clear();
      BcidD.clear();
      if(isRun2) {
	m_ttTool->simulateChannel(*tt, LutCp, LutJep, BcidR, BcidD);
	simBcid = BcidR[Peak];
	if (Slices < 7 || nPedCorr < 3) {
	  simJep = LutJep[Peak];
	  simCp = LutCp[Peak];
	}
	if (msgLvl(MSG::DEBUG) &&
	    (simJep != datJep || simCp != datCp) &&
	    (Slices >= 7 || datJep != 0)) { // mismatch - repeat with debug on
	  m_ttTool->setDebug(true);
	  m_ttTool->simulateChannel(*tt, LutCp, LutJep, BcidR, BcidD);
	  m_ttTool->setDebug(false);
	}
      } else { // Run-1
	m_ttTool->process(convertVectorType<int>(ADC), tt->coolId(), LutCp, BcidR, BcidD);
	if (Slices < 7 || BcidD[Peak]) simCp = LutCp[Peak];
	if (msgLvl(MSG::DEBUG) && simCp != datCp && (Slices >= 7 || datCp != 0)) {
	  // mismatch - repeat with debug on
	  LutCp.clear();
	  BcidR.clear();
	  BcidD.clear();
	  m_ttTool->setDebug(true);
	  m_ttTool->process(convertVectorType<int>(ADC), tt->coolId(), LutCp, BcidR, BcidD);
	  m_ttTool->setDebug(false);
	}
      }
    }
    
    // ---------------------Fill in error plots------------------------
    int mismatch = 0;
    TH2F_LW* hist1 = 0;
    
    // Compare BCID simulation to data
    if (datCp || datJep || *std::max_element(std::begin(ADC), std::end(ADC)) >= m_simulationADCCut) {
      std::bitset<3> simBcidBits(simBcid);
      std::bitset<3> datBcidBits(datBcid);
      if ((Slices >= 7) && (nPedCorr >= 3)) { // compare simulation of peak finder to data (sim not possible in 5+1 readout mode)
        if (simBcidBits[2] && datBcidBits[2]) { //non-zero match
          hist1 = m_h_ppm_2d_etaPhi_tt_peakf_NonZeroMatches;
        } else if (!simBcidBits[2] && !datBcidBits[2]) { // zero match
          hist1 = m_h_ppm_2d_etaPhi_tt_peakf_ZeroMatches;
        } else if (simBcidBits[2] != datBcidBits[2]) { // mismatch
          mismatch = 1;
          if (!simBcidBits[2]) { //data no sim
            hist1 = m_h_ppm_2d_etaPhi_tt_peakf_DataNoSim;
          } else if (!datBcidBits[2]) { //sim no data
            hist1 = m_h_ppm_2d_etaPhi_tt_peakf_SimNoData;
          } 
          ATH_MSG_DEBUG(" PeakfinderBCID sim/data Mismatch eta/phi/sim/dat: "
		        << eta << "/" << phi << "/" << simBcidBits[2] << "/" << datBcidBits[2]);
        }
        if (hist1) {
          m_histTool->fillPPMEmEtaVsPhi(hist1, eta, phi);
          hist1 = 0;
        }
      }
      if (ADC[0] < 1020 && ADC[1] < 1020) { // compare simulation of saturated bcid algorithm to data
        if (simBcidBits[1] && datBcidBits[1]) { // non-zero match
          hist1 = m_h_ppm_2d_etaPhi_tt_satBcid_NonZeroMatches; 
        } else if (!simBcidBits[1] && !datBcidBits[1]) { // zero match
          hist1 = m_h_ppm_2d_etaPhi_tt_satBcid_ZeroMatches;
        } else if (simBcidBits[1] != datBcidBits[1]) { // mismatch
          mismatch = 1;
          if (!simBcidBits[1]) { // data no sim
              //Temporarily disabled
              //hist1 = m_h_ppm_2d_etaPhi_tt_satBcid_DataNoSim;
               mismatch=0;
          } else if (!datBcidBits[1]) { // sim no data
              hist1 = m_h_ppm_2d_etaPhi_tt_satBcid_SimNoData;
          }
          ATH_MSG_DEBUG(" SaturatedBCID sim/data Mismatch eta/phi/sim/dat: "
		        << eta << "/" << phi << "/" << simBcidBits[1] << "/" << datBcidBits[1]);  
        }
        if (hist1) {
          m_histTool->fillPPMEmEtaVsPhi(hist1, eta, phi);
          hist1 = 0;
        }   
      }    
    }
      
    // Compare LUT simulation to data
    // only fill histograms for non-empty towers (simulation or data)
    if (!simCp && !datCp && !simJep && !datJep) continue;

    if (tt->layer() == 0) { // For EM layer
      // For LUT-CP
      hist1 = 0;
      if (simCp && simCp == datCp) { // non-zero match
        hist1 = m_h_ppm_em_2d_etaPhi_tt_lutCp_SimEqData;
      } else if (simCp != datCp) {  // mis-match
        mismatch = 1;
        if (simCp && datCp) {       // non-zero mis-match
          if (!pedCorrOverflow){		// no pedCorr over- or underflow
	          hist1 = m_h_ppm_em_2d_etaPhi_tt_lutCp_SimNeData;
          }else{
            mismatch = 0;           //If the pedestal Correction overflows do not fill mismatch Event Histogram
          }    
        } else if (!datCp) {        // no data
          if ((Slices >= 7) && (nPedCorr >= 3)) {
            hist1 = m_h_ppm_em_2d_etaPhi_tt_lutCp_SimNoData;
          } else mismatch = 0;
        } else {                    // no sim
          hist1 = m_h_ppm_em_2d_etaPhi_tt_lutCp_DataNoSim;
        }
	ATH_MSG_DEBUG(" EMTowerMismatch eta/phi/sim/dat: "
		      << eta << "/" << phi << "/" << simCp << "/" << datCp);
      }
      if (hist1) m_histTool->fillPPMEmEtaVsPhi(hist1, eta, phi);

      // For LUT-JEP
      hist1 = 0;
      if (simJep && simJep == datJep) { // non-zero match
        hist1 = m_h_ppm_em_2d_etaPhi_tt_lutJep_SimEqData;
      } else if (simJep != datJep) {  // mis-match
        mismatch = 1;
        if (simJep && datJep) {       // non-zero mis-match
          if (!pedCorrOverflow){		// no pedCorr over- or underflow
	          hist1 = m_h_ppm_em_2d_etaPhi_tt_lutJep_SimNeData;
          }else{
            mismatch = 0;           //If the pedestal Correction overflows do not fill mismatch Event Histogram
          }
        } else if (!datJep) {        // no data
          if ((Slices >= 7) && (nPedCorr >= 3)) {
            hist1 = m_h_ppm_em_2d_etaPhi_tt_lutJep_SimNoData;
          } else mismatch = 0;
        } else {                    // no sim
          hist1 = m_h_ppm_em_2d_etaPhi_tt_lutJep_DataNoSim;
        }
	ATH_MSG_DEBUG(" EMTowerMismatch eta/phi/sim/dat: "
		      << eta << "/" << phi << "/" << simJep << "/" << datJep);
      }
      if (hist1) m_histTool->fillPPMEmEtaVsPhi(hist1, eta, phi);
    }

    if (tt->layer() == 1) { // For HAD layer
      // For LUT-CP
      hist1 = 0;
      if (simCp && simCp == datCp) { // non-zero match
        hist1 = m_h_ppm_had_2d_etaPhi_tt_lutCp_SimEqData;
      } else if (simCp != datCp) {  // mis-match
        mismatch = 1;
        if (simCp && datCp) {       // non-zero mis-match
           if (!pedCorrOverflow){		// no pedCorr over- or underflow
            hist1 = m_h_ppm_had_2d_etaPhi_tt_lutCp_SimNeData;
           }else{
            mismatch = 0;           //If the pedestal Correction overflows do not fill mismatch Event Histogram
          }
        } else if (!datCp) {        // no data
          if ((Slices >= 7) && (nPedCorr >= 3)) {
            hist1 = m_h_ppm_had_2d_etaPhi_tt_lutCp_SimNoData;
          } else mismatch = 0;
        } else {                    // no sim
          hist1 = m_h_ppm_had_2d_etaPhi_tt_lutCp_DataNoSim;
        }
	ATH_MSG_DEBUG(" HADTowerMismatch eta/phi/sim/dat: "
		      << eta << "/" << phi << "/" << simCp << "/" << datCp);
      }
      if (hist1) m_histTool->fillPPMHadEtaVsPhi(hist1, eta, phi);
      
      // For LUT-JEP
      hist1 = 0;
      if (simJep && simJep == datJep) { // non-zero match
        hist1 = m_h_ppm_had_2d_etaPhi_tt_lutJep_SimEqData;
      } else if (simJep != datJep) {  // mis-match
        mismatch = 1;
        if (simJep && datJep) {       // non-zero mis-match
           if (!pedCorrOverflow){		// no pedCorr over- or underflow
            hist1 = m_h_ppm_had_2d_etaPhi_tt_lutJep_SimNeData;
           }else{
            mismatch = 0;           //If the pedestal Correction overflows do not fill mismatch Event Histogram
          }
        } else if (!datJep) {        // no data
          if ((Slices >= 7) && (nPedCorr >= 3)) {
            hist1 = m_h_ppm_had_2d_etaPhi_tt_lutJep_SimNoData;
          } else mismatch = 0;
        } else {                    // no sim
          hist1 = m_h_ppm_had_2d_etaPhi_tt_lutJep_DataNoSim;
        }
	ATH_MSG_DEBUG(" HADTowerMismatch eta/phi/sim/dat: "
		      << eta << "/" << phi << "/" << simJep << "/" << datJep);
      }      
      if (hist1) m_histTool->fillPPMHadEtaVsPhi(hist1, eta, phi);
    }
    
    if (mismatch == 1) {
      const L1CaloCoolChannelId coolId(tt->coolId());
      const int crate  = coolId.crate();
      const int module = coolId.module();
      crateError[crate] = 1;
      if (!((moduleError[crate] >> module) & 0x1)) {
        fillEventSample(crate, module);
        moduleError[crate] |= (1 << module);
      }
    }
  }
  ErrorVector* save = new ErrorVector(crateError);
  if (evtStore()->record(save, "L1CaloPPMMismatchVector").isFailure()) {
    ATH_MSG_ERROR("Error recording PPM mismatch vector in TES");
  }

  m_ttTool->setDebug(true);
}

void PPMSimBSMon::fillEventSample(int crate, int module)
{
  const int y = module + 16 * (crate % 2);
  TH2I_LW* hist = 0;
  if     (crate == 0 || crate == 1) hist = m_h_ppm_2d_LUT_MismatchEvents_cr0cr1;
  else if (crate == 2 || crate == 3) hist = m_h_ppm_2d_LUT_MismatchEvents_cr2cr3;
  else if (crate == 4 || crate == 5) hist = m_h_ppm_2d_LUT_MismatchEvents_cr4cr5;
  else if (crate == 6 || crate == 7) hist = m_h_ppm_2d_LUT_MismatchEvents_cr6cr7;
  if (hist) m_histTool->fillEventNumber(hist, y);
}

// ============================================================================
}  // end namespace
// ============================================================================
