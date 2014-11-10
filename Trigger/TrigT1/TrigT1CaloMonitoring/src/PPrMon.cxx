/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     PPrMon.cxx
// PACKAGE:  TrigT1CaloMonitoring  
//
// AUTHOR:   Johanna Fleckner (Johanna.Fleckner@uni-mainz.de)
//           
//
// ********************************************************************

#include <cmath>
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "SGTools/StlVectorClids.h"

#include "LWHists/LWHist.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"
#include "LWHists/TProfile_LW.h"
#include "LWHists/TProfile2D_LW.h"

#include "AthenaMonitoring/AthenaMonManager.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigT1CaloMonitoring/PPrMon.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"
#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"
//#include "TrigConfigSvc/ILVL1ConfigSvc.h"

#include "TrigT1CaloEvent/TriggerTowerCollection.h"
#include "TrigT1CaloEvent/TriggerTower_ClassDEF.h"
#include "TrigT1CaloUtils/DataError.h"


/*---------------------------------------------------------*/
PPrMon::PPrMon(const std::string & type, const std::string & name,
					 const IInterface* parent)
  : ManagedMonitorToolBase ( type, name, parent ),
    m_SliceNo(15),
    m_histBooked(false),
    m_errorTool("TrigT1CaloMonErrorTool"),
    m_histTool("TrigT1CaloLWHistogramTool"),
    m_ttTool("LVL1::L1TriggerTowerTool/L1TriggerTowerTool"),
    m_h_ppm_em_2d_etaPhi_tt_adc_HitMap(0),
    m_h_ppm_had_2d_etaPhi_tt_adc_HitMap(0),
    m_h_ppm_had_1d_tt_adc_MaxTimeslice(0),
    m_h_ppm_em_1d_tt_adc_MaxTimeslice(0),
    m_h_ppm_em_2d_etaPhi_tt_adc_ProfileHitMap(0),
    m_h_ppm_had_2d_etaPhi_tt_adc_ProfileHitMap(0),
    m_h_ppm_em_2d_etaPhi_tt_adc_MaxTimeslice(0),
    m_h_ppm_had_2d_etaPhi_tt_adc_MaxTimeslice(0),
    m_v_ppm_1d_tt_adc_SignalProfile(0),
    m_v_ppm_em_2d_etaPhi_tt_lutcp_Threshold(0),
    m_v_ppm_had_2d_etaPhi_tt_lutcp_Threshold(0),
    m_h_ppm_em_2d_etaPhi_tt_lutcp_AverageEt(0),
    m_h_ppm_had_2d_etaPhi_tt_lutcp_AverageEt(0),
    m_v_ppm_em_2d_etaPhi_tt_lutjep_Threshold(0),
    m_v_ppm_had_2d_etaPhi_tt_lutjep_Threshold(0),
    m_h_ppm_em_2d_etaPhi_tt_lutjep_AverageEt(0),
    m_h_ppm_had_2d_etaPhi_tt_lutjep_AverageEt(0),
    m_h_ppm_em_1d_tt_lutcp_Et(0),
    m_h_ppm_em_1d_tt_lutcp_Eta(0),
    m_h_ppm_em_1d_tt_lutcp_Phi(0),
    m_h_ppm_had_1d_tt_lutcp_Et(0),
    m_h_ppm_had_1d_tt_lutcp_Eta(0),
    m_h_ppm_had_1d_tt_lutcp_Phi(0),
    m_h_ppm_1d_tt_lutcp_LutPerBCN(0),
    m_h_ppm_2d_tt_lutcp_BcidBits(0),
    m_h_ppm_em_1d_tt_lutjep_Et(0),
    m_h_ppm_em_1d_tt_lutjep_Eta(0),
    m_h_ppm_em_1d_tt_lutjep_Phi(0),
    m_h_ppm_had_1d_tt_lutjep_Et(0),
    m_h_ppm_had_1d_tt_lutjep_Eta(0),
    m_h_ppm_had_1d_tt_lutjep_Phi(0),
    m_h_ppm_1d_tt_lutjep_LutPerBCN(0),
    m_h_ppm_2d_tt_lutjep_BcidBits(0),
    m_h_ppm_1d_ErrorSummary(0),
    m_h_ppm_2d_Status03(0),
    m_h_ppm_2d_Status47(0),
    m_h_ppm_2d_ErrorField03(0),
    m_h_ppm_2d_ErrorField47(0),
    m_v_ppm_2d_ASICErrorsDetail(0),
    m_h_ppm_2d_ErrorEventNumbers(0),
    m_h_ppm_2d_ASICErrorEventNumbers(0),
    m_h_ppm_em_1d_tt_adc_TriggeredSlice(0),
    m_h_ppm_had_1d_tt_adc_TriggeredSlice(0)
/*---------------------------------------------------------*/
{
  declareProperty("BS_TriggerTowerContainer",
                  m_TriggerTowerContainerName = "LVL1TriggerTowers");
  declareProperty("LUTHitMap_LumiBlocks",  m_TT_HitMap_LumiBlocks = 10,
        "The number of back lumiblocks for separate LUT hitmaps online");
  declareProperty("ADCHitMap_Thresh",      m_TT_ADC_HitMap_Thresh = 15,
                  "ADC cut for hitmaps");
  declareProperty("MaxEnergyRange",        m_MaxEnergyRange       = 256,
                  "Maximum energy for LUT Et plots");
  declareProperty("ADCPedestal",           m_TT_ADC_Pedestal      = 32,
                  "Nominal pedestal value");
  declareProperty("HADFADCCut",            m_HADFADCCut           = 40,
                  "HAD FADC cut for signal");
  declareProperty("EMFADCCut",             m_EMFADCCut            = 40,
                  "EM FADC cut for signal");

  declareProperty("PathInRootFile", m_PathInRootFile="L1Calo/PPM") ;
  declareProperty("ErrorPathInRootFile",
                  m_ErrorPathInRootFile="L1Calo/PPM/Errors") ;
  declareProperty("OnlineTest", m_onlineTest = false,
                  "Test online code when running offline");

  // note: threshold vector index (not value) is preferred 
  // to name PPM LUT histograms (see below, buffer_name) to 
  // spare some Data Quality configuration file changes 
  unsigned int defaultThresh[] = {0,1,2,3,4,5,6,7,10,15,20,33,45,50};
  std::vector<unsigned int> defaultThreshVec (defaultThresh, 
      defaultThresh + sizeof(defaultThresh) / sizeof(unsigned int) );
  declareProperty("LUTHitMap_ThreshVec", 
                  m_TT_HitMap_ThreshVec = defaultThreshVec);

}

/*---------------------------------------------------------*/
PPrMon::~PPrMon()
/*---------------------------------------------------------*/
{
}

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

/*---------------------------------------------------------*/
StatusCode PPrMon::initialize()
/*---------------------------------------------------------*/
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endreq;

  StatusCode sc;

  sc = ManagedMonitorToolBase::initialize();
  if (sc.isFailure()) return sc;

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

  sc = m_ttTool.retrieve();
  if( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool L1TriggerTowerTool" << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode PPrMon::bookHistogramsRecurrent()
/*---------------------------------------------------------*/
{
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in PPrMon::bookHistograms" << endreq;

  if( m_environment == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
  }
	
  if( m_dataType == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }

  MgmtAttr_t attr = ATTRIB_UNMANAGED;

  if ( newRun ) {

    MonGroup TT_LutCpHitMaps(this, m_PathInRootFile+"/LUT-CP/EtaPhiMaps", run, attr);
    MonGroup TT_LutJepHitMaps(this, m_PathInRootFile+"/LUT-JEP/EtaPhiMaps", run, attr);
    MonGroup TT_ADC(this, m_PathInRootFile+"/ADC/EtaPhiMaps", run, attr);
    MonGroup TT_ADCSlices(this, m_PathInRootFile+"/ADC/Timeslices", run, attr);
    MonGroup TT_LutCpPeakDist(this, m_PathInRootFile+"/LUT-CP/Distributions",
                                                                  run, attr);
    MonGroup TT_LutJepPeakDist(this, m_PathInRootFile+"/LUT-JEP/Distributions",
                                                                  run, attr);
    MonGroup TT_Error(this, m_ErrorPathInRootFile, run, attr);
    MonGroup TT_ErrorEvents(this, m_ErrorPathInRootFile, run, attr, "",
                                                                 "eventSample");
    MonGroup TT_ErrorDetail(this, m_ErrorPathInRootFile+"/Detail", run, attr);


    //-------------------- ADC Hitmaps for Triggered Timeslice ---------------

    std::string name,title;
    std::stringstream buffer;

    buffer.str("");
    buffer << m_TT_ADC_HitMap_Thresh;

    m_histTool->setMonGroup(&TT_ADC);
    

    title = "#eta - #phi Map of EM FADC > "+ buffer.str()
                                           + " for triggered timeslice";
    m_h_ppm_em_2d_etaPhi_tt_adc_HitMap = m_histTool->bookPPMEmEtaVsPhi(
                                "ppm_em_2d_etaPhi_tt_adc_HitMap", title);

    title = "#eta - #phi Profile Map of EM FADC > "+ buffer.str()
                                           + " for triggered timeslice";
    m_h_ppm_em_2d_etaPhi_tt_adc_ProfileHitMap = m_histTool->bookProfilePPMEmEtaVsPhi(
                                "ppm_em_2d_etaPhi_tt_adc_ProfileHitMap", title);

    title = "#eta - #phi Map of HAD FADC > "+ buffer.str()
                                            + " for triggered timeslice";
    m_h_ppm_had_2d_etaPhi_tt_adc_HitMap = m_histTool->bookPPMHadEtaVsPhi(
                                 "ppm_had_2d_etaPhi_tt_adc_HitMap",title);

    title = "#eta - #phi Profile Map of HAD FADC > "+ buffer.str()
                                            + " for triggered timeslice";
    m_h_ppm_had_2d_etaPhi_tt_adc_ProfileHitMap = m_histTool->bookProfilePPMHadEtaVsPhi(
                               "ppm_had_2d_etaPhi_tt_adc_ProfileHitMap", title);

    //---------------------------------------------------------//
    m_histTool->setMonGroup(&TT_ADCSlices);

    m_h_ppm_had_1d_tt_adc_MaxTimeslice = m_histTool->book1F(
                       "ppm_had_1d_tt_adc_MaxTimeslice",
		       " HAD Distribution of Maximum Timeslice;time slice",
		       m_SliceNo, 0, m_SliceNo);
    m_histTool->numbers(m_h_ppm_had_1d_tt_adc_MaxTimeslice, 0, m_SliceNo-1);
    m_h_ppm_em_1d_tt_adc_MaxTimeslice = m_histTool->book1F(
                       "ppm_em_1d_tt_adc_MaxTimeslice",
		       " EM Distribution of Maximum Timeslice;time slice",
		       m_SliceNo, 0, m_SliceNo);
    m_histTool->numbers(m_h_ppm_em_1d_tt_adc_MaxTimeslice, 0, m_SliceNo-1);
	  
    //------------------------Average Maximum Timeslice-----------------------
      
    m_h_ppm_had_2d_etaPhi_tt_adc_MaxTimeslice = m_histTool->bookProfilePPMHadEtaVsPhi(
      "ppm_had_2d_etaPhi_tt_adc_MaxTimeslice",
      "Average Maximum TimeSlice for HAD Signal (TS:1-15)");
    m_h_ppm_em_2d_etaPhi_tt_adc_MaxTimeslice = m_histTool->bookProfilePPMEmEtaVsPhi(
      "ppm_em_2d_etaPhi_tt_adc_MaxTimeslice",
      "Average Maximum TimeSlice for EM Signal (TS:1-15)");

    //---------------------------- Signal shape ------------------------------

    m_v_ppm_1d_tt_adc_SignalProfile.clear();
    const int emPart = MaxPartitions/2;
    for (int p = 0; p < MaxPartitions; ++p) {
      if (p < emPart) name = "ppm_em_1d_tt_adc_SignalProfile"
                             + partitionName(p);
      else            name = "ppm_had_1d_tt_adc_SignalProfile"
                             + partitionName(p);
      title = "Signal Shape Profile for " + partitionName(p) + ";Timeslice";
      m_v_ppm_1d_tt_adc_SignalProfile.push_back(m_histTool->bookProfile(name, title,
	                                            m_SliceNo, 0, m_SliceNo));
    }

    //----------------------- LUT Hitmaps per threshold ----------------------

    std::stringstream buffer_name;

    // Per run and last N lumiblocks - online only
    if (m_environment == AthenaMonManager::online || m_onlineTest) {
      m_v_ppm_em_2d_etaPhi_tt_lutcp_Threshold.clear();
      m_v_ppm_had_2d_etaPhi_tt_lutcp_Threshold.clear();
      m_v_ppm_em_2d_etaPhi_tt_lutjep_Threshold.clear();
      m_v_ppm_had_2d_etaPhi_tt_lutjep_Threshold.clear();
      m_histTool->setMonGroup(&TT_LutCpHitMaps);
      for (unsigned int thresh = 0; thresh < m_TT_HitMap_ThreshVec.size(); ++thresh) {
        buffer.str("");
        buffer_name.str("");
        buffer << m_TT_HitMap_ThreshVec[thresh];
        buffer_name << std::setw(2) << std::setfill('0') << thresh;
	TH2F_LW* hist = m_histTool->bookPPMEmEtaVsPhi(
	       "ppm_em_2d_etaPhi_tt_lutcp_Threshold"+buffer_name.str(),
	       "#eta - #phi Map of EM LUT-CP > "+buffer.str());
	m_v_ppm_em_2d_etaPhi_tt_lutcp_Threshold.push_back(hist);
	hist = m_histTool->bookPPMHadEtaVsPhi(
	       "ppm_had_2d_etaPhi_tt_lutcp_Threshold"+buffer_name.str(),
	       "#eta - #phi Map of HAD LUT-CP > "+buffer.str());
	m_v_ppm_had_2d_etaPhi_tt_lutcp_Threshold.push_back(hist);
	hist = m_histTool->bookPPMEmEtaVsPhi(
	       "ppm_em_2d_etaPhi_tt_lutjep_Threshold"+buffer_name.str(),
	       "#eta - #phi Map of EM LUT-JEP > "+buffer.str());
	m_v_ppm_em_2d_etaPhi_tt_lutjep_Threshold.push_back(hist);
	hist = m_histTool->bookPPMHadEtaVsPhi(
	       "ppm_had_2d_etaPhi_tt_lutjep_Threshold"+buffer_name.str(),
	       "#eta - #phi Map of HAD LUT-JEP > "+buffer.str());
	m_v_ppm_had_2d_etaPhi_tt_lutjep_Threshold.push_back(hist);
      }
      for (int block = 0; block <= m_TT_HitMap_LumiBlocks; ++block) {
        buffer.str("");
	buffer << block;
        MonGroup lumiGroup(this,
	  m_PathInRootFile+"/LUT-CP/EtaPhiMaps/lumi_"+buffer.str(), run, attr);
        m_histTool->setMonGroup(&lumiGroup);
	for (unsigned int thresh = 0; thresh < m_TT_HitMap_ThreshVec.size(); ++thresh) {
          buffer_name.str("");
	  buffer_name << std::setw(2) << std::setfill('0') << thresh << "Lumi" << block;
	  std::string name = "ppm_em_2d_etaPhi_tt_lutcp_Thresh"+buffer_name.str();
	  buffer.str("");
	  if (block == 0) buffer << m_TT_HitMap_ThreshVec[thresh] << ", Current Lumi-block";
	  else            buffer << m_TT_HitMap_ThreshVec[thresh] << ", Lumi-block -" << block;
	  std::string title = "#eta - #phi Map of EM LUT-CP > "+buffer.str();
	  TH2F_LW* hist = m_histTool->bookPPMEmEtaVsPhi(name, title);
	  m_v_ppm_em_2d_etaPhi_tt_lutcp_Threshold.push_back(hist);
	  title = "#eta - #phi Map of HAD LUT-CP > "+buffer.str();
	  buffer_name.str("");
	  buffer_name << std::setw(2) << std::setfill('0') << thresh << "Lumi" << block;
	  name = "ppm_had_2d_etaPhi_tt_lutcp_Thresh"+buffer_name.str();
	  hist = m_histTool->bookPPMHadEtaVsPhi(name, title);
	  m_v_ppm_had_2d_etaPhi_tt_lutcp_Threshold.push_back(hist);
	}
      }
       for (int block = 0; block <= m_TT_HitMap_LumiBlocks; ++block) {
        buffer.str("");
	buffer << block;
        MonGroup lumiGroup(this,
	  m_PathInRootFile+"/LUT-JEP/EtaPhiMaps/lumi_"+buffer.str(), run, attr);
        m_histTool->setMonGroup(&lumiGroup);
	for (unsigned int thresh = 0; thresh < m_TT_HitMap_ThreshVec.size(); ++thresh) {
          buffer_name.str("");
	  buffer_name << std::setw(2) << std::setfill('0') << thresh << "Lumi" << block;
	  std::string name = "ppm_em_2d_etaPhi_tt_lutjep_Thresh"+buffer_name.str();
	  buffer.str("");
	  if (block == 0) buffer << m_TT_HitMap_ThreshVec[thresh] << ", Current Lumi-block";
	  else            buffer << m_TT_HitMap_ThreshVec[thresh] << ", Lumi-block -" << block;
	  std::string title = "#eta - #phi Map of EM LUT-JEP > "+buffer.str();
	  TH2F_LW* hist = m_histTool->bookPPMEmEtaVsPhi(name, title);
	  m_v_ppm_em_2d_etaPhi_tt_lutjep_Threshold.push_back(hist);
	  title = "#eta - #phi Map of HAD LUT-JEP > "+buffer.str();
	  buffer_name.str("");
	  buffer_name << std::setw(2) << std::setfill('0') << thresh << "Lumi" << block;
	  name = "ppm_had_2d_etaPhi_tt_lutjep_Thresh"+buffer_name.str();
	  hist = m_histTool->bookPPMHadEtaVsPhi(name, title);
	  m_v_ppm_had_2d_etaPhi_tt_lutjep_Threshold.push_back(hist);
	}
      }
    }

    m_histTool->setMonGroup(&TT_LutCpHitMaps);

    m_h_ppm_em_2d_etaPhi_tt_lutcp_AverageEt = m_histTool->bookProfilePPMEmEtaVsPhi(
      "ppm_em_2d_etaPhi_tt_lutcp_AverageEt","EM Average LUT-CP Et for Et > 5");
    m_h_ppm_had_2d_etaPhi_tt_lutcp_AverageEt = m_histTool->bookProfilePPMHadEtaVsPhi(
      "ppm_had_2d_etaPhi_tt_lutcp_AverageEt","HAD Average LUT-CP Et for Et > 5");
      
    m_histTool->setMonGroup(&TT_LutJepHitMaps);

    m_h_ppm_em_2d_etaPhi_tt_lutjep_AverageEt = m_histTool->bookProfilePPMEmEtaVsPhi(
      "ppm_em_2d_etaPhi_tt_lutjep_AverageEt","EM Average LUT-JEP Et for Et > 5");
    m_h_ppm_had_2d_etaPhi_tt_lutjep_AverageEt = m_histTool->bookProfilePPMHadEtaVsPhi(
      "ppm_had_2d_etaPhi_tt_lutjep_AverageEt","HAD Average LUT-JEP Et for Et > 5");
    
    //--------------- distribution of LUT-CP peak per detector region -----------

    m_histTool->setMonGroup(&TT_LutCpPeakDist);

    m_h_ppm_em_1d_tt_lutcp_Et = m_histTool->book1F("ppm_em_1d_tt_lutcp_Et",
      "EM LUT-CP: Distribution of Peak;em LUT Peak [GeV]",
      m_MaxEnergyRange-1, 1, m_MaxEnergyRange);
    m_h_ppm_em_1d_tt_lutcp_Eta = m_histTool->bookPPMEmEta("ppm_em_1d_tt_lutcp_Eta",
      "EM LUT-CP: Distribution of Peak per #eta");
    m_h_ppm_em_1d_tt_lutcp_Phi = m_histTool->book1F("ppm_em_1d_tt_lutcp_Phi",
      "EM LUT-CP: Distribution of Peak per #phi;phi", 64, 0., 2.*M_PI);
      
    m_h_ppm_had_1d_tt_lutcp_Et = m_histTool->book1F("ppm_had_1d_tt_lutcp_Et",
      "HAD LUT-CP: Distribution of Peak;had LUT Peak [GeV]",
      m_MaxEnergyRange-1, 1, m_MaxEnergyRange); 
    m_h_ppm_had_1d_tt_lutcp_Eta = m_histTool->bookPPMHadEta("ppm_had_1d_tt_lutcp_Eta",
      "HAD LUT-CP: Distribution of Peak per #eta");
    m_h_ppm_had_1d_tt_lutcp_Phi = m_histTool->book1F("ppm_had_1d_tt_lutcp_Phi",
      "HAD LUT-CP: Distribution of Peak per #phi;phi", 64, 0., 2.*M_PI);

    m_h_ppm_1d_tt_lutcp_LutPerBCN = m_histTool->book1F("ppm_1d_tt_lutcp_LutPerBCN",
      "Num of LUT-CP > 5 per BC;Bunch Crossing;Num. of LUT above limit",
      0xdec, 0, 0xdec);
    m_h_ppm_2d_tt_lutcp_BcidBits = m_histTool->book2F("ppm_2d_tt_lutcp_BcidBits",
      "PPM: Bits of BCID Logic Word Vs. LUT", 8, 0., 8., 256, 0., 256.);
    LWHist::LWHistAxis* axis = m_h_ppm_2d_tt_lutcp_BcidBits->GetXaxis();
    axis->SetBinLabel(1, "none");
    axis->SetBinLabel(2, "extBC only");
    axis->SetBinLabel(3, "satBC only");
    axis->SetBinLabel(4, "extBC & satBC");
    axis->SetBinLabel(5, "peakF only");
    axis->SetBinLabel(6, "extBC & peakF");
    axis->SetBinLabel(7, "satBC & peakF");
    axis->SetBinLabel(8, "all");
    
    //--------------- distribution of LUT-JEP peak per detector region -----------

    m_histTool->setMonGroup(&TT_LutJepPeakDist);

    m_h_ppm_em_1d_tt_lutjep_Et = m_histTool->book1F("ppm_em_1d_tt_lutjep_Et",
      "EM LUT-JEP: Distribution of Peak;em LUT Peak [GeV]",
      m_MaxEnergyRange-1, 1, m_MaxEnergyRange);
    m_h_ppm_em_1d_tt_lutjep_Eta = m_histTool->bookPPMEmEta("ppm_em_1d_tt_lutjep_Eta",
      "EM LUT-JEP: Distribution of Peak per #eta");
    m_h_ppm_em_1d_tt_lutjep_Phi = m_histTool->book1F("ppm_em_1d_tt_lutjep_Phi",
      "EM LUT-JEP: Distribution of Peak per #phi;phi", 64, 0., 2.*M_PI);
      
    m_h_ppm_had_1d_tt_lutjep_Et = m_histTool->book1F("ppm_had_1d_tt_lutjep_Et",
      "HAD LUT-JEP: Distribution of Peak;had LUT Peak [GeV]",
      m_MaxEnergyRange-1, 1, m_MaxEnergyRange); 
    m_h_ppm_had_1d_tt_lutjep_Eta = m_histTool->bookPPMHadEta("ppm_had_1d_tt_lutjep_Eta",
      "HAD LUT-JEP: Distribution of Peak per #eta");
    m_h_ppm_had_1d_tt_lutjep_Phi = m_histTool->book1F("ppm_had_1d_tt_lutjep_Phi",
      "HAD LUT-JEP: Distribution of Peak per #phi;phi", 64, 0., 2.*M_PI);

    m_h_ppm_1d_tt_lutjep_LutPerBCN = m_histTool->book1F("ppm_1d_tt_lutjep_LutPerBCN",
      "Num of LUT-JEP > 5 per BC;Bunch Crossing;Num. of LUT above limit",
      0xdec, 0, 0xdec);
    m_h_ppm_2d_tt_lutjep_BcidBits = m_histTool->book2F("ppm_2d_tt_lutjep_BcidBits",
      "PPM: Bits of BCID Logic Word Vs. LUT", 8, 0., 8., 256, 0., 256.);
    LWHist::LWHistAxis* axis2 = m_h_ppm_2d_tt_lutjep_BcidBits->GetXaxis();
    axis2->SetBinLabel(1, "none");
    axis2->SetBinLabel(2, "extBC only");
    axis2->SetBinLabel(3, "satBC only");
    axis2->SetBinLabel(4, "extBC & satBC");
    axis2->SetBinLabel(5, "peakF only");
    axis2->SetBinLabel(6, "extBC & peakF");
    axis2->SetBinLabel(7, "satBC & peakF");
    axis2->SetBinLabel(8, "all");
 
    //-------------------------Summary of Errors------------------------------

    m_histTool->setMonGroup(&TT_Error);

    m_h_ppm_1d_ErrorSummary = m_histTool->book1F("ppm_1d_ErrorSummary",
                                    "Summary of SubStatus Errors", 8, 0., 8.);
    m_histTool->subStatus(m_h_ppm_1d_ErrorSummary);

    //---------------------------- SubStatus Word errors ---------------------
      
    //L1Calo Substatus word
    m_h_ppm_2d_Status03 = m_histTool->bookPPMSubStatusVsCrateModule(
      "ppm_2d_Status03", "Errors from TT SubStatus Word (crates 0-3)", 0, 3);
    m_h_ppm_2d_Status47 = m_histTool->bookPPMSubStatusVsCrateModule(
      "ppm_2d_Status47", "Errors from TT SubStatus Word (crates 4-7)", 4, 7);

    //error bit field from ASIC data
    m_h_ppm_2d_ErrorField03 = m_histTool->bookPPMErrorsVsCrateModule(
      "ppm_2d_ErrorField03", "Errors from ASIC error field (crates 0-3)", 0, 3);
    m_h_ppm_2d_ErrorField47 = m_histTool->bookPPMErrorsVsCrateModule(
      "ppm_2d_ErrorField47", "Errors from ASIC error field (crates 4-7)", 4, 7);

    m_histTool->setMonGroup(&TT_ErrorEvents);

    m_h_ppm_2d_ErrorEventNumbers = m_histTool->bookEventNumbers(
      "ppm_2d_ErrorEventNumbers", "SubStatus Error Event Numbers", 8, 0., 8.);
    m_histTool->subStatus(m_h_ppm_2d_ErrorEventNumbers, 0, false);
    m_h_ppm_2d_ASICErrorEventNumbers = m_histTool->bookEventNumbers(
      "ppm_2d_ASICErrorEventNumbers", "ASIC Error Field Event Numbers",
                                                                   8, 0., 8.);
    m_histTool->ppmErrors(m_h_ppm_2d_ASICErrorEventNumbers, 0, false);

    m_histTool->setMonGroup(&TT_ErrorDetail);

    m_v_ppm_2d_ASICErrorsDetail.clear();
    std::vector<std::string> errNames;
    errNames.push_back("Channel0Disabled");
    errNames.push_back("Channel1Disabled");
    errNames.push_back("Channel2Disabled");
    errNames.push_back("Channel3Disabled");
    errNames.push_back("MCMAbsent");
    errNames.push_back("");
    errNames.push_back("Timeout");
    errNames.push_back("ASICFull");
    errNames.push_back("EventMismatch");
    errNames.push_back("BunchMismatch");
    errNames.push_back("FIFOCorrupt");
    errNames.push_back("PinParity");
    for (int error = 0; error < 12; error+=2) {
      for (int crate = 0; crate < 8; crate+=2) {
        buffer.str("");
	buffer << crate;
	std::string name = "ppm_2d_"+errNames[error]+errNames[error+1]
	                            +"Crate"+buffer.str();
	std::string title = "ASIC Errors "+errNames[error]+" "
	                     +errNames[error+1]+" for Crates "+buffer.str();
	buffer.str("");
	buffer << (crate+1);
	name += buffer.str();
	title += "-"+buffer.str();
	const int nbins = (error != 4) ? 32 : 16;
	TH2F_LW* hist = m_histTool->book2F(name,title,nbins,0,nbins,32,0,32);
	m_histTool->numbers(hist, 0, 15, 2);
	LWHist::LWHistAxis* axis = hist->GetXaxis();
	axis->SetBinLabel(1, errNames[error].c_str());
	if (error != 4) {
	  m_histTool->numbers(hist, 0, 15, 2, 16);
	  axis->SetBinLabel(17, errNames[error+1].c_str());
	}
	axis->SetTitle("MCM");
	m_histTool->ppmCrateModule(hist, crate, crate+1, 0, false);
	m_v_ppm_2d_ASICErrorsDetail.push_back(hist);
      }
    }

    //---------------------------- number of triggered slice -----------------
    m_histTool->setMonGroup(&TT_ADCSlices);

    m_h_ppm_em_1d_tt_adc_TriggeredSlice = m_histTool->book1F(
      "ppm_em_1d_tt_adc_TriggeredSlice",
      "Number of the EM Triggered Slice;#Slice", m_SliceNo, 0, m_SliceNo);
    m_histTool->numbers(m_h_ppm_em_1d_tt_adc_TriggeredSlice, 0, m_SliceNo-1);
    m_h_ppm_had_1d_tt_adc_TriggeredSlice = m_histTool->book1F(
      "ppm_had_1d_tt_adc_TriggeredSlice",
      "Number of the HAD Triggered Slice;#Slice", m_SliceNo, 0, m_SliceNo);
    m_histTool->numbers(m_h_ppm_had_1d_tt_adc_TriggeredSlice, 0, m_SliceNo-1);
	     
  }	

  if ( newLumiBlock ) {

    //---------------------------- LUT-CP Hitmaps per threshold -----------------
    if (m_environment == AthenaMonManager::online || m_onlineTest) {

      // Current lumi copied to lumi-1 and so on
      for (int block = m_TT_HitMap_LumiBlocks-1; block >= 0; --block) {
	for (unsigned int thresh = 0; thresh < m_TT_HitMap_ThreshVec.size(); ++thresh) {
	  TH2F_LW* hist1 = m_v_ppm_em_2d_etaPhi_tt_lutcp_Threshold[
	                           (block+1)*m_TT_HitMap_ThreshVec.size() + thresh];
	  TH2F_LW* hist2 = m_v_ppm_em_2d_etaPhi_tt_lutcp_Threshold[
	                           (block+2)*m_TT_HitMap_ThreshVec.size() + thresh];
	  TH2F_LW* hist3 = m_v_ppm_had_2d_etaPhi_tt_lutcp_Threshold[
	                           (block+1)*m_TT_HitMap_ThreshVec.size() + thresh];
	  TH2F_LW* hist4 = m_v_ppm_had_2d_etaPhi_tt_lutcp_Threshold[
	                           (block+2)*m_TT_HitMap_ThreshVec.size() + thresh];
	  hist2->Reset();
	  hist4->Reset();
	  unsigned int ix = 0;
	  unsigned int iy = 0;
	  double content = 0.;
	  double error   = 0.;
	  hist1->resetActiveBinLoop();
	  while (hist1->getNextActiveBin(ix, iy, content, error)) {
	    if (content > 0.) hist2->SetBinContent(ix, iy, content);
	  }
	  hist3->resetActiveBinLoop();
	  while (hist3->getNextActiveBin(ix, iy, content, error)) {
	    if (content > 0.) hist4->SetBinContent(ix, iy, content);
	  }
	  if (block == 0) {
            hist1->Reset();
	    hist3->Reset();
	  }
        }
      }
    } else {

      // Offline - per lumiblock - merge will give per run
      m_v_ppm_em_2d_etaPhi_tt_lutcp_Threshold.clear();
      m_v_ppm_had_2d_etaPhi_tt_lutcp_Threshold.clear();
      MonGroup TT_LumiHitMaps(this, m_PathInRootFile+"/LUT-CP/EtaPhiMaps",
                                                          lumiBlock, attr);
      m_histTool->setMonGroup(&TT_LumiHitMaps);
      std::stringstream buffer;
      std::stringstream buffer_name;
      for (unsigned int thresh = 0; thresh < m_TT_HitMap_ThreshVec.size(); ++thresh) {
        buffer.str("");
        buffer_name.str("");
        buffer << m_TT_HitMap_ThreshVec[thresh];
        buffer_name << std::setw(2) << std::setfill('0') << thresh;
	TH2F_LW* hist = m_histTool->bookPPMEmEtaVsPhi(
	  "ppm_em_2d_etaPhi_tt_lutcp_Threshold"+buffer_name.str(),
	  "#eta - #phi Map of EM LUT-CP > "+buffer.str());
	m_v_ppm_em_2d_etaPhi_tt_lutcp_Threshold.push_back(hist);
	hist = m_histTool->bookPPMHadEtaVsPhi(
	  "ppm_had_2d_etaPhi_tt_lutcp_Threshold"+buffer_name.str(),
	  "#eta - #phi Map of Had LUT-CP > "+buffer.str());
	m_v_ppm_had_2d_etaPhi_tt_lutcp_Threshold.push_back(hist);
      }
    }

    m_histTool->unsetMonGroup();
    if (newRun) m_histBooked = true;
    
    //---------------------------- LUT-JEP Hitmaps per threshold -----------------
    if (m_environment == AthenaMonManager::online || m_onlineTest) {

      // Current lumi copied to lumi-1 and so on
      for (int block = m_TT_HitMap_LumiBlocks-1; block >= 0; --block) {
	for (unsigned int thresh = 0; thresh < m_TT_HitMap_ThreshVec.size(); ++thresh) {
	  TH2F_LW* hist1 = m_v_ppm_em_2d_etaPhi_tt_lutjep_Threshold[
	                           (block+1)*m_TT_HitMap_ThreshVec.size() + thresh];
	  TH2F_LW* hist2 = m_v_ppm_em_2d_etaPhi_tt_lutjep_Threshold[
	                           (block+2)*m_TT_HitMap_ThreshVec.size() + thresh];
	  TH2F_LW* hist3 = m_v_ppm_had_2d_etaPhi_tt_lutjep_Threshold[
	                           (block+1)*m_TT_HitMap_ThreshVec.size() + thresh];
	  TH2F_LW* hist4 = m_v_ppm_had_2d_etaPhi_tt_lutjep_Threshold[
	                           (block+2)*m_TT_HitMap_ThreshVec.size() + thresh];
	  hist2->Reset();
	  hist4->Reset();
	  unsigned int ix = 0;
	  unsigned int iy = 0;
	  double content = 0.;
	  double error   = 0.;
	  hist1->resetActiveBinLoop();
	  while (hist1->getNextActiveBin(ix, iy, content, error)) {
	    if (content > 0.) hist2->SetBinContent(ix, iy, content);
	  }
	  hist3->resetActiveBinLoop();
	  while (hist3->getNextActiveBin(ix, iy, content, error)) {
	    if (content > 0.) hist4->SetBinContent(ix, iy, content);
	  }
	  if (block == 0) {
            hist1->Reset();
	    hist3->Reset();
	  }
        }
      }
    } else {

      // Offline - per lumiblock - merge will give per run
      m_v_ppm_em_2d_etaPhi_tt_lutjep_Threshold.clear();
      m_v_ppm_had_2d_etaPhi_tt_lutjep_Threshold.clear();
      MonGroup TT_LumiHitMaps(this, m_PathInRootFile+"/LUT-JEP/EtaPhiMaps",
                                                          lumiBlock, attr);
      m_histTool->setMonGroup(&TT_LumiHitMaps);
      std::stringstream buffer;
      std::stringstream buffer_name;
      for (unsigned int thresh = 0; thresh < m_TT_HitMap_ThreshVec.size(); ++thresh) {
        buffer.str("");
        buffer_name.str("");
        buffer << m_TT_HitMap_ThreshVec[thresh];
        buffer_name << std::setw(2) << std::setfill('0') << thresh;
	TH2F_LW* hist = m_histTool->bookPPMEmEtaVsPhi(
	  "ppm_em_2d_etaPhi_tt_lutjep_Threshold"+buffer_name.str(),
	  "#eta - #phi Map of EM LUT-JEP > "+buffer.str());
	m_v_ppm_em_2d_etaPhi_tt_lutjep_Threshold.push_back(hist);
	hist = m_histTool->bookPPMHadEtaVsPhi(
	  "ppm_had_2d_etaPhi_tt_lutjep_Threshold"+buffer_name.str(),
	  "#eta - #phi Map of Had LUT-JEP > "+buffer.str());
	m_v_ppm_had_2d_etaPhi_tt_lutjep_Threshold.push_back(hist);
      }
    }

    m_histTool->unsetMonGroup();
    if (newRun) m_histBooked = true;
    
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode PPrMon::fillHistograms()
/*---------------------------------------------------------*/
{
  const bool debug = msgLvl(MSG::DEBUG);
  if (debug) msg(MSG::DEBUG) << "in fillHistograms()" << endreq;

  if (!m_histBooked) {
    if (debug) msg(MSG::DEBUG) << "Histogram(s) not booked" << endreq;
    return StatusCode::SUCCESS;
  }

  // Skip events believed to be corrupt

  if (m_errorTool->corrupt()) {
    if (debug) msg(MSG::DEBUG) << "Skipping corrupt event" << endreq;
    return StatusCode::SUCCESS;
  }

  // Error vector for global overview
  std::vector<int> overview(8);
  
  //Retrieve TriggerTowers from SG
  const TriggerTowerCollection* TriggerTowerTES = 0; 
  StatusCode sc = evtStore()->retrieve(TriggerTowerTES,
                                                m_TriggerTowerContainerName); 
  if (sc.isFailure() || !TriggerTowerTES) {
    if (debug) msg(MSG::DEBUG) << "No TriggerTower found in TES at "
                               << m_TriggerTowerContainerName << endreq ;
    return StatusCode::SUCCESS;
  }

  // Get Bunch crossing number from EventInfo
  uint32_t bunchCrossing = 0;
  const EventInfo* evInfo = 0;
  sc = evtStore()->retrieve(evInfo);
  if (sc.isFailure() || !evInfo) {
    if (debug) msg(MSG::DEBUG) << "No EventInfo found" << endreq;
  } else {
    const EventID* evID = evInfo->event_ID();
    if (evID) bunchCrossing = evID->bunch_crossing_id();
  }

    
  // =====================================================================
  // ================= Container: TriggerTower ===========================
  // =====================================================================

  TriggerTowerCollection::const_iterator TriggerTowerIterator =
                                                      TriggerTowerTES->begin(); 
  TriggerTowerCollection::const_iterator TriggerTowerIteratorEnd =
                                                      TriggerTowerTES->end(); 
 
  for (; TriggerTowerIterator != TriggerTowerIteratorEnd;
                                                ++TriggerTowerIterator) {
    
    //---------------------------- EM Energy -----------------------------
    // em LUT Peak per channel
    const int EmEnergy = (*TriggerTowerIterator)->emEnergy();
    const int EmEnergy2 = EmEnergy/2;
    const double eta   = (*TriggerTowerIterator)->eta();
    const double phi   = (*TriggerTowerIterator)->phi();

    // em energy distributions per detector region
    if (EmEnergy > 0) {
      m_h_ppm_em_1d_tt_lutcp_Eta->Fill(eta, 1);
      m_histTool->fillPPMPhi(m_h_ppm_em_1d_tt_lutcp_Phi, eta, phi);
      m_h_ppm_em_1d_tt_lutcp_Et->Fill(EmEnergy, 1);
      if (EmEnergy > 5) {
        m_histTool->fillPPMEmEtaVsPhi(m_h_ppm_em_2d_etaPhi_tt_lutcp_AverageEt, eta, phi,
	                                                             EmEnergy);
        // Bunch crossing and BCID bits
        m_h_ppm_1d_tt_lutcp_LutPerBCN->Fill(bunchCrossing);
      }
      m_h_ppm_2d_tt_lutcp_BcidBits->Fill((*TriggerTowerIterator)->emBCID(), EmEnergy);
    }
    if (EmEnergy2 > 0) {
      m_h_ppm_em_1d_tt_lutjep_Eta->Fill(eta, 1);
      m_histTool->fillPPMPhi(m_h_ppm_em_1d_tt_lutjep_Phi, eta, phi);
      m_h_ppm_em_1d_tt_lutjep_Et->Fill(EmEnergy2, 1);
      if (EmEnergy2 > 5) {
        m_histTool->fillPPMEmEtaVsPhi(m_h_ppm_em_2d_etaPhi_tt_lutjep_AverageEt, eta, phi,
	                                                             EmEnergy2);
        // Bunch crossing and BCID bits
        m_h_ppm_1d_tt_lutjep_LutPerBCN->Fill(bunchCrossing);
      }
      m_h_ppm_2d_tt_lutjep_BcidBits->Fill((*TriggerTowerIterator)->emBCID(), EmEnergy2);
    }
	 
    //---------------------------- EM LUT HitMaps -----------------------------
    for (unsigned int thresh = 0; thresh < m_TT_HitMap_ThreshVec.size(); ++thresh) {
      if (EmEnergy > 0) {
        unsigned int u_EmEnergy = static_cast<unsigned int>(EmEnergy); 
        if (u_EmEnergy > m_TT_HitMap_ThreshVec[thresh]) {
   	  m_histTool->fillPPMEmEtaVsPhi(m_v_ppm_em_2d_etaPhi_tt_lutcp_Threshold[thresh],
	                                                            eta, phi, 1);
	  if (m_environment == AthenaMonManager::online || m_onlineTest) {
	    m_histTool->fillPPMEmEtaVsPhi(
	      m_v_ppm_em_2d_etaPhi_tt_lutcp_Threshold[thresh+m_TT_HitMap_ThreshVec.size()],
	      eta, phi, 1);
          }
        }
      }
      if (EmEnergy2 > 0) {
        unsigned int u_EmEnergy2 = static_cast<unsigned int>(EmEnergy2); 
        if (u_EmEnergy2 > m_TT_HitMap_ThreshVec[thresh]) {
   	  m_histTool->fillPPMEmEtaVsPhi(m_v_ppm_em_2d_etaPhi_tt_lutjep_Threshold[thresh],
	                                                            eta, phi, 1);
	  if (m_environment == AthenaMonManager::online || m_onlineTest) {
	    m_histTool->fillPPMEmEtaVsPhi(
	      m_v_ppm_em_2d_etaPhi_tt_lutjep_Threshold[thresh+m_TT_HitMap_ThreshVec.size()],
	      eta, phi, 1);
          }
        }
      }
    }
    
    //---------------------------- HAD Energy -----------------------------
    // had LUT peak per channel
    const int HadEnergy = (*TriggerTowerIterator)->hadEnergy();
    const int HadEnergy2 = HadEnergy*2;
	
    // had energy distribution per detector region
    if (HadEnergy>0) {
      m_h_ppm_had_1d_tt_lutcp_Eta->Fill(eta, 1);
      m_histTool->fillPPMPhi(m_h_ppm_had_1d_tt_lutcp_Phi, eta, phi);
      m_h_ppm_had_1d_tt_lutcp_Et->Fill(HadEnergy,1);
      if (HadEnergy>5) {
        m_histTool->fillPPMHadEtaVsPhi(m_h_ppm_had_2d_etaPhi_tt_lutcp_AverageEt, eta, phi,
	                                                            HadEnergy);
        // Bunch crossing and BCID bits
        m_h_ppm_1d_tt_lutcp_LutPerBCN->Fill(bunchCrossing);
      }
      m_h_ppm_2d_tt_lutcp_BcidBits->Fill((*TriggerTowerIterator)->hadBCID(), HadEnergy);
    }
    if (HadEnergy2>0) {
      m_h_ppm_had_1d_tt_lutjep_Eta->Fill(eta, 1);
      m_histTool->fillPPMPhi(m_h_ppm_had_1d_tt_lutjep_Phi, eta, phi);
      m_h_ppm_had_1d_tt_lutjep_Et->Fill(HadEnergy2,1);
      if (HadEnergy>5) {
        m_histTool->fillPPMHadEtaVsPhi(m_h_ppm_had_2d_etaPhi_tt_lutjep_AverageEt, eta, phi,
	                                                            HadEnergy2);
        // Bunch crossing and BCID bits
        m_h_ppm_1d_tt_lutjep_LutPerBCN->Fill(bunchCrossing);
      }
      m_h_ppm_2d_tt_lutjep_BcidBits->Fill((*TriggerTowerIterator)->hadBCID(), HadEnergy2);
    }
    
    //---------------------------- had LUT HitMaps -----------------------------
    for (unsigned int thresh = 0; thresh < m_TT_HitMap_ThreshVec.size(); ++thresh) {
      if (HadEnergy > 0) {
        unsigned int u_HadEnergy = static_cast<unsigned int>(HadEnergy);
        if (u_HadEnergy > m_TT_HitMap_ThreshVec[thresh]) {
	  m_histTool->fillPPMHadEtaVsPhi(m_v_ppm_had_2d_etaPhi_tt_lutcp_Threshold[thresh],
	                                                            eta, phi, 1);
          if (m_environment == AthenaMonManager::online || m_onlineTest) {
	    m_histTool->fillPPMHadEtaVsPhi(
	      m_v_ppm_had_2d_etaPhi_tt_lutcp_Threshold[thresh+m_TT_HitMap_ThreshVec.size()],
	      eta, phi, 1);
          }
        }
      }
      if (HadEnergy2 > 0) {
        unsigned int u_HadEnergy2 = static_cast<unsigned int>(HadEnergy2);
        if (u_HadEnergy2 > m_TT_HitMap_ThreshVec[thresh]) {
	  m_histTool->fillPPMHadEtaVsPhi(m_v_ppm_had_2d_etaPhi_tt_lutjep_Threshold[thresh],
	                                                            eta, phi, 1);
          if (m_environment == AthenaMonManager::online || m_onlineTest) {
	    m_histTool->fillPPMHadEtaVsPhi(
	      m_v_ppm_had_2d_etaPhi_tt_lutjep_Threshold[thresh+m_TT_HitMap_ThreshVec.size()],
	      eta, phi, 1);
          }
        }
      }
    }

    //---------------------------- ADC HitMaps per timeslice -----------------

    unsigned int tslice = (*TriggerTowerIterator)->emADCPeak();
    
    if (tslice < ((*TriggerTowerIterator)->emADC()).size()) {
      const int temADC = ((*TriggerTowerIterator)->emADC())[tslice];
      if (temADC > m_TT_ADC_HitMap_Thresh) {
	m_histTool->fillPPMEmEtaVsPhi(m_h_ppm_em_2d_etaPhi_tt_adc_HitMap, eta, phi, 1);
	m_histTool->fillPPMEmEtaVsPhi(m_h_ppm_em_2d_etaPhi_tt_adc_ProfileHitMap, eta, phi,
	                                                              temADC);
      }
    }

    tslice = (*TriggerTowerIterator)->hadADCPeak();

    if (tslice < ((*TriggerTowerIterator)->hadADC()).size()) {
      const int thadADC = ((*TriggerTowerIterator)->hadADC())[tslice];
      if (thadADC > m_TT_ADC_HitMap_Thresh) {
        m_histTool->fillPPMHadEtaVsPhi(m_h_ppm_had_2d_etaPhi_tt_adc_HitMap, eta, phi, 1);
        m_histTool->fillPPMHadEtaVsPhi(m_h_ppm_had_2d_etaPhi_tt_adc_ProfileHitMap, eta, phi,
	                                                               thadADC);
      }
    }
	             
    //---------------------------- Timing of FADC Signal ---------------------

    const std::vector<int>& emADC((*TriggerTowerIterator)->emADC());
    const std::vector<int>& hadADC((*TriggerTowerIterator)->hadADC());

    double max = recTime(emADC, m_EMFADCCut);
    //log << MSG::INFO << "TimeSlice of Maximum "<< max<< endreq ;
    if (max >= 0.) {
      m_histTool->fillPPMEmEtaVsPhi(m_h_ppm_em_2d_etaPhi_tt_adc_MaxTimeslice, eta, phi,
                                                                       max+1.);
      m_h_ppm_em_1d_tt_adc_MaxTimeslice->Fill(max);
    }

    max = recTime(hadADC, m_HADFADCCut);
    //log << MSG::INFO << "TimeSlice of Maximum "<< max<< endreq ;
    if (max >= 0.) {
      m_histTool->fillPPMHadEtaVsPhi(m_h_ppm_had_2d_etaPhi_tt_adc_MaxTimeslice, eta, phi,
                                                                       max+1.);
      m_h_ppm_had_1d_tt_adc_MaxTimeslice->Fill(max);
    }

    //------------------------ Signal shape profile --------------------------

    if (EmEnergy > 0) {
      const int emPart  = partition(0, eta);
      std::vector<int>::const_iterator it  = emADC.begin();
      std::vector<int>::const_iterator itE = emADC.end();
      for (int slice = 0; it != itE && slice < m_SliceNo; ++it, ++slice) {
        m_v_ppm_1d_tt_adc_SignalProfile[emPart]->Fill(slice, *it);
      }
    }
    if (HadEnergy > 0) {
      const int hadPart = partition(1, eta);
      std::vector<int>::const_iterator it  = hadADC.begin();
      std::vector<int>::const_iterator itE = hadADC.end();
      for (int slice = 0; it != itE && slice < m_SliceNo; ++it, ++slice) {
        m_v_ppm_1d_tt_adc_SignalProfile[hadPart]->Fill(slice, *it);
      }
    }

    //---------------------------- SubStatus Word errors ---------------------
    //----------------------------- em ---------------------------------------

    using LVL1::DataError;

    if ((*TriggerTowerIterator)-> emError()) {

      const DataError emerr((*TriggerTowerIterator)-> emError());

      const L1CaloCoolChannelId emCoolId(m_ttTool->channelID(eta, phi, 0));
      int crate     = emCoolId.crate();
      int module    = emCoolId.module();
      int submodule = emCoolId.subModule();
      int channel   = emCoolId.channel();

      // em signals Crate 0-3
      //em+had FCAL signals get processed in one crate (Crates 4-7)

      int ypos = (crate < 4) ? module+crate*16 : module+(crate-4)*16;

      for (int bit = 0; bit < 8; ++bit) {
        if (emerr.get(bit + DataError::ChannelDisabled)) {
          if (crate < 4) m_h_ppm_2d_ErrorField03->Fill(bit, ypos);
  	  else           m_h_ppm_2d_ErrorField47->Fill(bit, ypos);
	  m_histTool->fillEventNumber(m_h_ppm_2d_ASICErrorEventNumbers, bit);
        }
        if (emerr.get(bit + DataError::GLinkParity)) {
	  if (crate < 4) m_h_ppm_2d_Status03->Fill(bit, ypos);
	  else           m_h_ppm_2d_Status47->Fill(bit, ypos);
	  m_h_ppm_1d_ErrorSummary->Fill(bit);
	  m_histTool->fillEventNumber(m_h_ppm_2d_ErrorEventNumbers, bit);
        }
      }

      if (emerr.get(DataError::ChannelDisabled) ||
          emerr.get(DataError::MCMAbsent)) overview[crate] |= 1;

      if (emerr.get(DataError::Timeout)       ||
          emerr.get(DataError::ASICFull)      ||
          emerr.get(DataError::EventMismatch) ||
	  emerr.get(DataError::BunchMismatch) ||
          emerr.get(DataError::FIFOCorrupt)   ||
	  emerr.get(DataError::PinParity)) overview[crate] |= (1 << 1);

      if (emerr.get(DataError::GLinkParity)   ||
          emerr.get(DataError::GLinkProtocol) ||
          emerr.get(DataError::FIFOOverflow)  ||
	  emerr.get(DataError::ModuleError)   ||
          emerr.get(DataError::GLinkDown)     ||
	  emerr.get(DataError::GLinkTimeout)  ||
	  emerr.get(DataError::BCNMismatch)) overview[crate] |= (1 << 2);

      // Detailed plots by MCM
      ypos = (crate%2)*16+module;
      if (emerr.get(DataError::ChannelDisabled)) {
        m_v_ppm_2d_ASICErrorsDetail[(channel/2)*4+crate/2]->Fill((channel%2)*16+submodule,
                                                                          ypos);
      }
      if (emerr.get(DataError::MCMAbsent)) {
        m_v_ppm_2d_ASICErrorsDetail[8+crate/2]->Fill(submodule, ypos);
      }
      if (emerr.get(DataError::Timeout)) {
        m_v_ppm_2d_ASICErrorsDetail[12+crate/2]->Fill(submodule, ypos);
      }
      if (emerr.get(DataError::ASICFull)) {
        m_v_ppm_2d_ASICErrorsDetail[12+crate/2]->Fill(16+submodule, ypos);
      }
      if (emerr.get(DataError::EventMismatch)) {
        m_v_ppm_2d_ASICErrorsDetail[16+crate/2]->Fill(submodule, ypos);
      }
      if (emerr.get(DataError::BunchMismatch)) {
        m_v_ppm_2d_ASICErrorsDetail[16+crate/2]->Fill(16+submodule, ypos);
      }
      if (emerr.get(DataError::FIFOCorrupt)) {
        m_v_ppm_2d_ASICErrorsDetail[20+crate/2]->Fill(submodule, ypos);
      }
      if (emerr.get(DataError::PinParity)) {
        m_v_ppm_2d_ASICErrorsDetail[20+crate/2]->Fill(16+submodule, ypos);
      }

    }
    
    //had

    if ((*TriggerTowerIterator)-> hadError()) {

      const DataError haderr((*TriggerTowerIterator)-> hadError());

      const L1CaloCoolChannelId hadCoolId(m_ttTool->channelID(eta, phi, 1));
      int crate     = hadCoolId.crate();
      int module    = hadCoolId.module();
      int submodule = hadCoolId.subModule();
      int channel   = hadCoolId.channel();
      
      int ypos = (crate < 4) ? module+crate*16 : module+(crate-4)*16;

      for (int bit = 0; bit < 8; ++bit) {
        if (haderr.get(bit + DataError::ChannelDisabled)) {
	  if (crate < 4) m_h_ppm_2d_ErrorField03->Fill(bit, ypos);
	  else           m_h_ppm_2d_ErrorField47->Fill(bit, ypos);
	  m_histTool->fillEventNumber(m_h_ppm_2d_ASICErrorEventNumbers, bit);
        }
        if (haderr.get(bit + DataError::GLinkParity)) {
	  if (crate < 4) m_h_ppm_2d_Status03->Fill(bit, ypos);
	  else           m_h_ppm_2d_Status47->Fill(bit, ypos);
	  m_h_ppm_1d_ErrorSummary->Fill(bit);
	  m_histTool->fillEventNumber(m_h_ppm_2d_ErrorEventNumbers, bit);
        }
      }

      if (haderr.get(DataError::ChannelDisabled) ||
          haderr.get(DataError::MCMAbsent)) overview[crate] |= 1;

      if (haderr.get(DataError::Timeout)       ||
          haderr.get(DataError::ASICFull)      ||
          haderr.get(DataError::EventMismatch) ||
	  haderr.get(DataError::BunchMismatch) ||
          haderr.get(DataError::FIFOCorrupt)   ||
	  haderr.get(DataError::PinParity)) overview[crate] |= (1 << 1);

      if (haderr.get(DataError::GLinkParity)   ||
          haderr.get(DataError::GLinkProtocol) ||
          haderr.get(DataError::FIFOOverflow)  ||
	  haderr.get(DataError::ModuleError)   ||
          haderr.get(DataError::GLinkDown)     ||
	  haderr.get(DataError::GLinkTimeout)  ||
	  haderr.get(DataError::BCNMismatch)) overview[crate] |= (1 << 2);
    
      // Detailed plots by MCM
      ypos = (crate%2)*16+module;
      if (haderr.get(DataError::ChannelDisabled)) {
        m_v_ppm_2d_ASICErrorsDetail[(channel/2)*4+crate/2]->Fill((channel%2)*16+submodule,
                                                                          ypos);
      }
      if (haderr.get(DataError::MCMAbsent)) {
        m_v_ppm_2d_ASICErrorsDetail[8+crate/2]->Fill(submodule, ypos);
      }
      if (haderr.get(DataError::Timeout)) {
        m_v_ppm_2d_ASICErrorsDetail[12+crate/2]->Fill(submodule, ypos);
      }
      if (haderr.get(DataError::ASICFull)) {
        m_v_ppm_2d_ASICErrorsDetail[12+crate/2]->Fill(16+submodule, ypos);
      }
      if (haderr.get(DataError::EventMismatch)) {
        m_v_ppm_2d_ASICErrorsDetail[16+crate/2]->Fill(submodule, ypos);
      }
      if (haderr.get(DataError::BunchMismatch)) {
        m_v_ppm_2d_ASICErrorsDetail[16+crate/2]->Fill(16+submodule, ypos);
      }
      if (haderr.get(DataError::FIFOCorrupt)) {
        m_v_ppm_2d_ASICErrorsDetail[20+crate/2]->Fill(submodule, ypos);
      }
      if (haderr.get(DataError::PinParity)) {
        m_v_ppm_2d_ASICErrorsDetail[20+crate/2]->Fill(16+submodule, ypos);
      }

    }
      
    // number of triggered slice
    m_h_ppm_em_1d_tt_adc_TriggeredSlice->Fill((*TriggerTowerIterator)->emADCPeak(), 1);
    m_h_ppm_had_1d_tt_adc_TriggeredSlice->Fill((*TriggerTowerIterator)->hadADCPeak(), 1);

  }	     
	     
  // Write overview vector to StoreGate
  std::vector<int>* save = new std::vector<int>(overview);
  sc = evtStore()->record(save, "L1CaloPPMErrorVector");
  if (sc != StatusCode::SUCCESS) {
    msg(MSG::ERROR) << "Error recording PPM error vector in TES "
                    << endreq;
    return sc;
  }
  
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
double PPrMon::recTime(const std::vector<int>& vFAdc, int cut) {
/*---------------------------------------------------------*/

  int max = -1;
  const int slices = vFAdc.size();
  if (slices > 0) {
    max = 0.;
    int maxAdc = vFAdc[0];
    for (int sl = 1; sl < slices; ++sl) {
      if (vFAdc[sl] > maxAdc) {
        maxAdc = vFAdc[sl];
        max = sl;
      } else if (vFAdc[sl] == maxAdc) max = -1;
    }
    if (maxAdc == 0) max = -1;
  }
  if (max >= 0) {
    int slbeg = max - 2;
    if (slbeg < 0) slbeg = 0;
    int slend = max + 3;
    if (slend > slices) slend = slices;
    int sum = 0;
    int min = 999999;
    for (int sl = slbeg; sl < slend; ++sl) {
      int val = vFAdc[sl];
      if (val < m_TT_ADC_Pedestal) val = m_TT_ADC_Pedestal;
      sum += val;
      if (val < min) min = val;
    }
    sum -= (slend-slbeg)*min;
    if (sum <= cut) max = -1;
  }
  
  return double(max);
}

/*---------------------------------------------------------*/
int PPrMon::partition(int layer, double eta) {
/*---------------------------------------------------------*/

  int part = 0;
  if (layer == 0) {
    if      (eta < -3.2) part = LArFCAL1C;
    else if (eta < -1.5) part = LArEMECC;
    else if (eta < -1.4) part = LArOverlapC;
    else if (eta <  0.0) part = LArEMBC;
    else if (eta <  1.4) part = LArEMBA;
    else if (eta <  1.5) part = LArOverlapA;
    else if (eta <  3.2) part = LArEMECA;
    else                 part = LArFCAL1A;
  } else {
    if      (eta < -3.2) part = LArFCAL23C;
    else if (eta < -1.5) part = LArHECC;
    else if (eta < -0.9) part = TileEBC;
    else if (eta <  0.0) part = TileLBC;
    else if (eta <  0.9) part = TileLBA;
    else if (eta <  1.5) part = TileEBA;
    else if (eta <  3.2) part = LArHECA;
    else                 part = LArFCAL23A;
  }
  return part;
}

/*---------------------------------------------------------*/
std::string PPrMon::partitionName(int part) {
/*---------------------------------------------------------*/

  std::string name = "";
  switch (part) {
    case LArFCAL1C:   name = "LArFCAL1C";   break;
    case LArEMECC:    name = "LArEMECC";    break;
    case LArOverlapC: name = "LArOverlapC"; break;
    case LArEMBC:     name = "LArEMBC";     break;
    case LArEMBA:     name = "LArEMBA";     break;
    case LArOverlapA: name = "LArOverlapA"; break;
    case LArEMECA:    name = "LArEMECA";    break;
    case LArFCAL1A:   name = "LArFCAL1A";   break;
    case LArFCAL23C:  name = "LArFCAL23C";  break;
    case LArHECC:     name = "LArHECC";     break;
    case TileEBC:     name = "TileEBC";     break;
    case TileLBC:     name = "TileLBC";     break;
    case TileLBA:     name = "TileLBA";     break;
    case TileEBA:     name = "TileEBA";     break;
    case LArHECA:     name = "LArHECA";     break;
    case LArFCAL23A:  name = "LArFCAL23A";  break;
    default:          name = "Unknown";     break;
  }
  return name;
}

