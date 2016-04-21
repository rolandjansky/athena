/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// NAME:     CaloCellVecMon.cxx
//
// AUTHORS:   R. Kehoe    06/15/2006 (LArg) 
//            S.Chekanov  08/03/2008 (TileCal) 
// MODIFICATIONS: F Spano' (LArg)  Dec2006 -Feb 2007  -Aug 2007 - Feb 2008 - March 2008 - Nov 2008 
//                E Wulf (LArg) Sep-Nov 2008 Feb-Apr 2009
//                D Urbaniec (LArg) Jan-Feb 2009
//                S Chekanov (TileCal and LArg noise histograms), Apr 2009 
//                S Chekanov. Corected binning for TileCal May 2009   
//                S Chekanov. Added 3 2D histograms for each sampling |E|>sigma(DB) 
//		  F TIAN (LArg) Jun 2010 - May 2011
//		  D Hu  Jun 2011 - May 2012
//                K ALGhadeer August 2012/2013 add 1D CellEnergyVsTime histograms with diffrent energy thresholds 
//                L Sawyer 12/2013 Modified booking to reflect new managed histograms
//                K ALGhadeer 08/19 Moved bookProcHists under bookHistogram
//                Dilip Jana 06/2015 Fixed histogram binning issue
// ********************************************************************
#include "CaloMonitoring/CaloCellVecMon.h"

//#include "xAODEventInfo/EventInfo.h"

#include "LArRecConditions/LArBadChannel.h"
#include "LArRecConditions/LArBadChanBitPacking.h"

#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloIdentifier/CaloGain.h"
#include "Identifier/Identifier.h"
#include "Identifier/HWIdentifier.h"

#include "AthenaMonitoring/DQBadLBFilterTool.h"
#include "AthenaMonitoring/DQAtlasReadyFilterTool.h"

#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloCellContainer.h"
#include "AthenaKernel/Units.h"


using namespace CaloMonitoring;
using Athena::Units::GeV;

////////////////////////////////////////////
CaloCellVecMon::CaloCellVecMon(const std::string& type, const std::string& name,const IInterface* parent) 
  :CaloMonToolBase(type, name, parent),
   //m_BadLBTool("DQBadLBFilterTool"),
   //m_ReadyFilterTool("DQAtlasReadyFilterTool"),
   m_noiseTool("CaloNoiseTool"),
   m_trigDec("Trig::TrigDecisionTool"),
   m_rndmTriggerEvent(0),
   m_caloTriggerEvent(0),
   m_minBiasTriggerEvent(0),
   m_metTriggerEvent(0),
   m_miscTriggerEvent(0),
   m_badChannelMask("BadLArRawChannelMask"),
   m_badChannelTool("LArBadChanTool"),
   m_LArOnlineIDHelper(0),
   m_LArCablingService("LArCablingService"),
   m_caloIdMgr(0),
   m_LArEM_IDHelper(0),
   m_LArFCAL_IDHelper(0),
   m_LArHEC_IDHelper(0),
   m_tile_id(0),
   m_calo_id(0),
   m_em_id(0)
{    
  declareInterface<IMonitorToolBase>(this);

  // CaloCellContainer name 
  declareProperty("CaloCellContainer", m_cellContainerName="AllCalo");

  // Time granularity 
  declareProperty("TimeGran",m_timeGran="run");

  // tools 
  declareProperty("isOnline", m_isOnline=false);
  declareProperty("useElectronicNoiseOnly",m_useElectronicNoiseOnly=false);
  declareProperty("useTwoGaus", m_useTwoGaus=true);
  declareProperty("CaloNoiseTool",m_noiseTool,"Tool Handle for noise Tool");

  // Trigger Awareness:
  declareProperty("useTrigger",m_useTrigger=true);
  declareProperty("rndmTriggerNames",m_rndmTriggerNames);
  declareProperty("caloTriggerNames",m_caloTriggerNames);
  declareProperty("minBiasTriggerNames",m_minBiasTriggerNames);
  declareProperty("metTriggerNames",m_metTriggerNames);
  declareProperty("miscTriggerNames",m_miscTriggerNames);
  
  // Bad channel masking options 
  declareProperty("LArBadChannelMask",m_badChannelMask);
  declareProperty("LArBadChannelTool",m_badChannelTool);
  declareProperty("MaskBadChannels",m_maskKnownBadChannels=false); 
  declareProperty("MaskNoCondChannels", m_maskNoCondChannels=false);
  declareProperty("doInverseMasking", m_doInverseMasking=false);
  declareProperty("doMaskingOnline", m_doMaskingOnline=false);
  declareProperty("problemsToMaskOffline",m_problemNamesToMask);

  // Switch on/off additional (non thresholded) histograms:
  declareProperty("doDatabaseNoiseVsEtaPhi",m_doDatabaseNoiseVsEtaPhi=true);
  declareProperty("doKnownBadChannelsVsEtaPhi",m_doKnownBadChannelsVsEtaPhi);
  declareProperty("doDBNoiseNormalized1DEnergy", m_doDBNormalized1DEnergy=false);
  declareProperty("doEnergyVsTime", m_doEnergyVsTime=true);
  declareProperty("doUnnormalized1DEnergy", m_doUnnormalized1DEnergy=false);
  declareProperty("useLogarithmicEnergyBinning", m_useLogarithmicEnergyBinning=false);

  // Switch to use WeightedEff rather than WeightedAvg merging where appropriate:
  declareProperty("useWeightedEffMerging",m_useWeightedEffMerging=true);

  // min energy for tilecal cells 
  declareProperty("energyThreshold",m_tileThreshold=300.); //Threshold in GeV

  // sporadic threshold 
  declareProperty("Sporadic_switch",m_sporadic_switch=true);
  declareProperty("Threshold_EM_S0S1",m_threshold_em_S0S1 = 5000);
  declareProperty("Threshold_HECFCALEMS2S3",m_threshold_HECFCALEMS2S3=15000);
  declareProperty("EventNumber_thres",m_EventNumber_thres=10);
  declareProperty("NsporadicPlotLimit",m_sporadicPlotLimit=300);
  declareProperty("Sporadic_protection",m_sporadic_protc = 4000);

  // lumi plots switch
  declareProperty("DoEtaLumi",m_doEtaLumi=true);
  declareProperty("DoPhiLumi",m_doPhiLumi=true);

  // Switch on/off thresholded histograms for each threshold:
  declareProperty("ThresholdType", m_thresholdTypeTemp);
  declareProperty("ThresholdDirection", m_thresholdDirectionTemp);
  declareProperty("TriggersToInclude", m_triggersToIncludeTemp);
  declareProperty("TriggersToExclude", m_triggersToExcludeTemp);

  declareProperty("DoPercentageOccupancy",m_doPercentageOccupancyTemp);
  declareProperty("DoEtaPhiOccupancy",m_doEtaPhiOccupancyTemp);
  declareProperty("DoEtaOccupancy",m_doEtaOccupancyTemp);
  declareProperty("DoPhiOccupancy",m_doPhiOccupancyTemp);

  declareProperty("DoEtaPhiAverageEnergy",m_doEtaPhiAverageEnergyTemp);
  declareProperty("DoEtaPhiTotalEnergy",m_doEtaPhiTotalEnergyTemp);
  declareProperty("DoEtaPhiEnergyRMS",m_doEtaPhiEnergyRMSTemp);
  declareProperty("DoEtaPhiRMSvsDBnoise",m_doEtaPhiDeviationFromDBnoiseTemp);

  declareProperty("DoEtaPhiAverageQuality",m_doEtaPhiAverageQualityTemp);
  declareProperty("DoEtaPhiFractionOverQth", m_doEtaPhiFractionOverQthTemp);
  declareProperty("QualityFactorThreshold",m_qualityFactorThresholdTemp);

  declareProperty("DoEtaPhiAverageTime",m_doEtaPhiAverageTimeTemp);
  declareProperty("DoEtaPhiFractionPastTth", m_doEtaPhiFractionPastTthTemp);
  declareProperty("TimeThreshold",m_timeThresholdTemp);  

  declareProperty("MaskEmptyBins",m_maskEmptyBinsTemp);
  declareProperty("DoBeamBackgroundRemoval",m_doBeamBackgroundRemovalTemp);

  declareProperty("DefaultThresholdTitles", m_defaultThresholdTitles);
  declareProperty("DefaultThresholdNames", m_defaultThresholdNames);
  declareProperty("DefaultThresholds", m_defaultThresholds);
  declareProperty("DefaultUseNoiseTool", m_defaultUseNoiseTool);

  declareProperty("ThresholdColumnType", m_thresholdColumnType);

  // Lar Thresholds
  // Em Barrel
  declareProperty("EMBP_Thresh",m_thresholdsTemp[EMBPNS]);
  declareProperty("EMBP_Names", m_threshNamesTemp[EMBPNS]);
  declareProperty("EMBP_Titles",m_threshTitlesTemp[EMBPNS]);
  declareProperty("EMBP_UseNoiseTool", m_useNoiseToolTemp[EMBPNS]);

  declareProperty("EMB1_Thresh",m_thresholdsTemp[EMB1NS]);
  declareProperty("EMB1_Names", m_threshNamesTemp[EMB1NS]);
  declareProperty("EMB1_Titles",m_threshTitlesTemp[EMB1NS]);
  declareProperty("EMB1_UseNoiseTool", m_useNoiseToolTemp[EMB1NS]);

  declareProperty("EMB2_Thresh",m_thresholdsTemp[EMB2NS]);
  declareProperty("EMB2_Names", m_threshNamesTemp[EMB2NS]);
  declareProperty("EMB2_Titles",m_threshTitlesTemp[EMB2NS]);
  declareProperty("EMB2_UseNoiseTool", m_useNoiseToolTemp[EMB2NS]);

  declareProperty("EMB3_Thresh",m_thresholdsTemp[EMB3NS]);
  declareProperty("EMB3_Names", m_threshNamesTemp[EMB3NS]);
  declareProperty("EMB3_Titles",m_threshTitlesTemp[EMB3NS]);
  declareProperty("EMB3_UseNoiseTool", m_useNoiseToolTemp[EMB3NS]);

  // EM Endcap
  declareProperty("EMECP_Thresh",m_thresholdsTemp[EMECPNS]);
  declareProperty("EMECP_Names", m_threshNamesTemp[EMECPNS]);
  declareProperty("EMECP_Titles",m_threshTitlesTemp[EMECPNS]);
  declareProperty("EMECP_UseNoiseTool", m_useNoiseToolTemp[EMECPNS]);

  declareProperty("EMEC1_Thresh",m_thresholdsTemp[EMEC1NS]);
  declareProperty("EMEC1_Names", m_threshNamesTemp[EMEC1NS]);
  declareProperty("EMEC1_Titles",m_threshTitlesTemp[EMEC1NS]);
  declareProperty("EMEC1_UseNoiseTool", m_useNoiseToolTemp[EMEC1NS]);

  declareProperty("EMEC2_Thresh",m_thresholdsTemp[EMEC2NS]);
  declareProperty("EMEC2_Names", m_threshNamesTemp[EMEC2NS]);
  declareProperty("EMEC2_Titles",m_threshTitlesTemp[EMEC2NS]);
  declareProperty("EMEC2_UseNoiseTool", m_useNoiseToolTemp[EMEC2NS]);

  declareProperty("EMEC3_Thresh",m_thresholdsTemp[EMEC3NS]);
  declareProperty("EMEC3_Names", m_threshNamesTemp[EMEC3NS]);
  declareProperty("EMEC3_Titles",m_threshTitlesTemp[EMEC3NS]);
  declareProperty("EMEC3_UseNoiseTool", m_useNoiseToolTemp[EMEC3NS]);

  // Hadronic Endcap
  declareProperty("HEC0_Thresh",m_thresholdsTemp[HEC0NS]);
  declareProperty("HEC0_Names", m_threshNamesTemp[HEC0NS]);
  declareProperty("HEC0_Titles",m_threshTitlesTemp[HEC0NS]);
  declareProperty("HEC0_UseNoiseTool", m_useNoiseToolTemp[HEC0NS]);

  declareProperty("HEC1_Thresh",m_thresholdsTemp[HEC1NS]);
  declareProperty("HEC1_Names", m_threshNamesTemp[HEC1NS]);
  declareProperty("HEC1_Titles",m_threshTitlesTemp[HEC1NS]);
  declareProperty("HEC1_UseNoiseTool", m_useNoiseToolTemp[HEC1NS]);

  declareProperty("HEC2_Thresh",m_thresholdsTemp[HEC2NS]);
  declareProperty("HEC2_Names", m_threshNamesTemp[HEC2NS]);
  declareProperty("HEC2_Titles",m_threshTitlesTemp[HEC2NS]);
  declareProperty("HEC2_UseNoiseTool", m_useNoiseToolTemp[HEC2NS]);

  declareProperty("HEC3_Thresh",m_thresholdsTemp[HEC3NS]);
  declareProperty("HEC3_Names", m_threshNamesTemp[HEC3NS]);
  declareProperty("HEC3_Titles",m_threshTitlesTemp[HEC3NS]);
  declareProperty("HEC3_UseNoiseTool", m_useNoiseToolTemp[HEC3NS]);

  // Forward Calorimeters
  declareProperty("FCAL1_Thresh",m_thresholdsTemp[FCAL1NS]);
  declareProperty("FCAL1_Names", m_threshNamesTemp[FCAL1NS]);
  declareProperty("FCAL1_Titles",m_threshTitlesTemp[FCAL1NS]);
  declareProperty("FCAL1_UseNoiseTool", m_useNoiseToolTemp[FCAL1NS]);

  declareProperty("FCAL2_Thresh",m_thresholdsTemp[FCAL2NS]);
  declareProperty("FCAL2_Names", m_threshNamesTemp[FCAL2NS]);
  declareProperty("FCAL2_Titles",m_threshTitlesTemp[FCAL2NS]);
  declareProperty("FCAL2_UseNoiseTool", m_useNoiseToolTemp[FCAL2NS]);

  declareProperty("FCAL3_Thresh",m_thresholdsTemp[FCAL3NS]);
  declareProperty("FCAL3_Names", m_threshNamesTemp[FCAL3NS]);
  declareProperty("FCAL3_Titles",m_threshTitlesTemp[FCAL3NS]);
  declareProperty("FCAL3_UseNoiseTool", m_useNoiseToolTemp[FCAL3NS]);

  initHists();

  initPointers();
}

void CaloCellVecMon::initHists(){
  m_h_NoiseBurstLumiblock = 0;
  m_h_sporad = 0;
  m_h_sporad_embc = 0;
  m_h_sporad_emba = 0;
  m_h_sporad_emecc = 0;
  m_h_sporad_emeca = 0;
  m_h_sporad_hecc = 0;
  m_h_sporad_heca = 0;
  m_h_sporad_fcalc  = 0;
  m_h_sporad_fcala = 0;

  m_h_CellsNoiseE = 0;

  m_h_Ncells = 0;
  m_h_CellsE = 0;
  m_h_CellsEtaPhi = 0;
  m_h_CellsEta = 0;
  m_h_CellsPhi = 0;
  m_h_CellsModuleS1 = 0;
  m_h_CellsModuleS2 = 0;
  m_h_CellsModuleS3 = 0;
  m_h_CellsModuleS4 = 0;
  m_h_CellsTower = 0;

  m_h_CellsNoiseEtaPhi = 0;
  m_h_CellsNoiseEtaPhiA = 0;
  m_h_CellsNoiseEtaPhiBC = 0;
  m_h_CellsNoiseEtaPhiD = 0;
  m_h_CellsNoiseHash = 0;

  m_h_CellsNoiseEta = 0;
  m_h_CellsNoisePhi = 0;
  m_h_CellsRMSPhi = 0;
  m_h_CellsRMSEta = 0;
  m_h_CellsRMSdivDBnoiseEta = 0;
  m_h_CellsRMSdivDBnoisePhi = 0;

  m_h_n_trigEvent = 0;

  for(int ilyr=EMBPA; ilyr<MAXLAYER; ilyr++){
    m_h_cellOccupancyEtaLumi[ilyr] = 0;
    m_h_cellOccupancyPhiLumi[ilyr] = 0;
    m_h_cellAvgEnergyEtaLumi[ilyr] = 0;
    m_h_cellAvgEnergyPhiLumi[ilyr] = 0;

    m_h_energy[ilyr] = 0;
    m_h_dbNoiseNormalizedEnergy[ilyr] = 0;
    m_h_energyVsTime[ilyr] = 0;
    m_h_energyVsTime_DifferThresholds[ilyr] = 0;
    
    m_h_energy_imask[ilyr] = 0;
    m_h_energyVsTime_imask[ilyr] = 0;

    m_h_occupancy_etaphi[ilyr] = 0;

    m_h_occupancy_eta[ilyr] = 0;
    m_h_occupancy_phi[ilyr] = 0;

    m_h_occupancy_etaphi_imask[ilyr] = 0;
    m_h_occupancy_eta_imask[ilyr] = 0;
    m_h_occupancy_phi_imask[ilyr] = 0;

    m_h_percentOccupancy_etaphi[ilyr] = 0;
    m_h_percentOccupancy_eta[ilyr] = 0;
    m_h_percentOccupancy_phi[ilyr] = 0;

    m_h_percentOccupancy_etaphi_imask[ilyr] = 0;
    m_h_percentOccupancy_eta_imask[ilyr] = 0;
    m_h_percentOccupancy_phi_imask[ilyr] = 0;

    m_h_energyProfile_etaphi[ilyr] = 0;
    m_h_totalEnergy_etaphi[ilyr] = 0;
    m_h_averageEnergy_etaphi[ilyr] = 0;
    m_h_noise_etaphi[ilyr] = 0;
    m_h_deviationFromDBnoise_etaphi[ilyr] = 0;

    m_h_energyProfile_etaphi_imask[ilyr] = 0;
    m_h_totalEnergy_etaphi_imask[ilyr] = 0;
    m_h_averageEnergy_etaphi_imask[ilyr] = 0;
    m_h_noise_etaphi_imask[ilyr] = 0;

    m_h_poorQualityOccupancy_etaphi[ilyr] = 0;
    m_h_totalQuality_etaphi[ilyr] = 0;
    m_h_fractionOverQth_etaphi[ilyr] = 0;
    m_h_averageQuality_etaphi[ilyr] = 0;

    m_h_poorTimeOccupancy_etaphi[ilyr] = 0;
    m_h_totalTime_etaphi[ilyr] = 0;
    m_h_fractionPastTth_etaphi[ilyr] = 0;
    m_h_averageTime_etaphi[ilyr] = 0;

    m_h_badChannels_etaphi[ilyr] = 0;
    m_h_dbnoise_etaphi[ilyr] = 0;
    m_h_missingCellMask[ilyr] = 0;

    m_h_nActiveCellsFirstEvent_eta[ilyr] = 0;
    m_h_nActiveCellsFirstEvent_phi[ilyr] = 0; 
  }

}

void CaloCellVecMon::initPointers(){
  m_summaryGroup=0;
  m_tile_cells_shift = 0;
  m_tempMonGroup=0;

  for(int iht=KnownBadChannels; iht<MaxHistType; iht++) {
    m_shifterMonGroup[iht] = 0;
    //ExpertMonGroup[iht]  = 0;
  }
  
  m_badCell=0;
  m_badCell_EMBC=0;
  m_badCell_EMBA=0;
  m_badCell_EMECC=0;
  m_badCell_EMECA=0;
  m_badCell_HECC=0;
  m_badCell_HECA=0;
  m_badCell_FCALC=0;
  m_badCell_FCALA=0;

  for(int ilyr=EMBPA; ilyr<MAXLAYER; ilyr++) {
    m_layer[ilyr] = 0;
  }

  for(int ilyrns=EMBPNS; ilyrns<MAXLYRNS; ilyrns++) {

    m_nEventsPassingTrigger[ilyrns] = 0;
    m_nEventsPassingBeamBG[ilyrns] = 0;

    m_thresholds[ilyrns] = 0;
    m_threshNames[ilyrns] = 0;
    m_threshTitles[ilyrns] = 0;
    m_thresholdDirection[ilyrns] = 0;
    m_threshTriggersToInclude[ilyrns] = 0;
    m_threshTriggersToExclude[ilyrns] = 0;
    m_threshTriggerDecision[ilyrns] = 0;
    m_useNoiseTool[ilyrns] = 0;

    m_qualityThresholds[ilyrns] = 0;
    m_timeThresholds[ilyrns] = 0;

    m_maskEmptyBins[ilyrns] = 0;
    m_doBeamBackgroundRemoval[ilyrns] = 0;

    m_doPercentageOccupancy[ilyrns] = 0;
    m_doEtaPhiEnergyProfile[ilyrns] = 0;
    m_doEtaPhiAverageEnergy[ilyrns] = 0;

    m_fillEtaPhiOccupancy[ilyrns] = 0;
    m_tempEtaPhiOccupancy[ilyrns] = 0;
    m_procEtaPhiOccupancy[ilyrns] = 0;
    m_fillEtaPhiTotalEnergy[ilyrns] = 0;
    m_tempEtaPhiTotalEnergy[ilyrns] = 0;
    m_procEtaPhiTotalEnergy[ilyrns] = 0;

    m_fillEtaOccupancy[ilyrns] = 0;
    m_procEtaOccupancy[ilyrns] = 0;
    m_fillPhiOccupancy[ilyrns] = 0;
    m_procPhiOccupancy[ilyrns] = 0;

    m_fillEtaPhiQualityOccupancy[ilyrns] = 0;
    m_fillEtaPhiTotalQuality[ilyrns] = 0;
    m_fillEtaPhiTimeOccupancy[ilyrns] = 0;
    m_fillEtaPhiTotalTime[ilyrns] = 0;

    m_procEtaPhiEnergyRMS[ilyrns] = 0;
    m_procEtaPhiDeviationFromDBnoise[ilyrns] = 0;
    m_procEtaPhiAverageQuality[ilyrns] = 0;
    m_procEtaPhiFractionOverQth[ilyrns] = 0;
    m_procEtaPhiAverageTime[ilyrns] = 0;
    m_procEtaPhiFractionPastTth[ilyrns] = 0;

    m_procPercentOccEtaPhi[ilyrns] = 0;
    m_procPercentOccEta[ilyrns] = 0;
    m_procPercentOccPhi[ilyrns] = 0;
    m_procAbsOccEtaPhi[ilyrns] = 0;
    m_procAbsOccEta[ilyrns] = 0;
    m_procAbsOccPhi[ilyrns] = 0;
    m_doEtaPhiAvgEnNoTh[ilyrns] = 0;
    m_doEtaPhiAvgEnTh[ilyrns] = 0;

  }
}

CaloCellVecMon::~CaloCellVecMon() {

}

////////////////////////////////////////////
StatusCode CaloCellVecMon::initialize() {

  ATH_MSG_INFO("CaloCellVecMon::initialize() start");

  ATH_CHECK(  retrieveTools() );

  // Sets the threshold value arrays
  initThreshValues();

  m_ifSaveAndDeleteHists = (!m_isOnline);
  m_bookProcHists=true;
  
  m_firstcallofBook = true;

  ATH_CHECK(  ManagedMonitorToolBase::initialize() );
  ATH_CHECK( CaloMonToolBase::initialize() );

  ATH_MSG_INFO("CaloCellVecMon::initialize() is done!");
  return StatusCode::SUCCESS;
}

StatusCode CaloCellVecMon::retrieveTools(){

  ATH_MSG_INFO("in retrieveTools()");

  ATH_CHECK( detStore()->retrieve(m_LArOnlineIDHelper, "LArOnlineID") );
  ATH_CHECK( m_LArCablingService.retrieve() );
  ATH_CHECK( detStore()->retrieve( m_caloIdMgr ) );

  m_LArEM_IDHelper   = m_caloIdMgr->getEM_ID();
  m_LArHEC_IDHelper  = m_caloIdMgr->getHEC_ID();
  m_LArFCAL_IDHelper = m_caloIdMgr->getFCAL_ID();

  // Helpers for retrieving cell information --------------------

  ATH_CHECK(  detStore()->retrieve(m_em_id) );
  ATH_CHECK( detStore()->retrieve(m_calo_id) );
  ATH_CHECK( detStore()->retrieve(m_tile_id) );
  ATH_CHECK( m_badChannelTool.retrieve() );

  // Retrieve bad channel masker tool (Necessary to have gain dependent online style masking, otherwise, LArBadChanTool is ample)
  if(m_maskKnownBadChannels && m_doMaskingOnline){
    StatusCode sc=m_badChannelMask.retrieve();
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Could not retrieve BadChannelMask tool - no online bad channel masking or inverse masking will be done");
      m_doMaskingOnline = false;
      return sc;
    }
  }
  else {
    if(!m_maskNoCondChannels){
      m_doInverseMasking=false;
    }
  }

 // Retrieve bad channel tool----------------- Used for building eta phi map of known bad channels:
  if( m_doKnownBadChannelsVsEtaPhi || ( m_maskKnownBadChannels && (!m_doMaskingOnline) ) ) {
    StatusCode sc = m_badChannelTool.retrieve();
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Could not retrieve LArBadChanTool - detailed bad channel histograms will not be produced");
      if(!m_doMaskingOnline) {
        m_maskKnownBadChannels = false;
        m_doInverseMasking=false;
      }

      m_doKnownBadChannelsVsEtaPhi = false;
      return sc;
    }
    else {
      ATH_MSG_INFO("LArBadChanTool is retrieved");
    }
  }

  // retrieve calonoise tool
  m_useNoiseToolGlobal=true;
  StatusCode sc = m_noiseTool.retrieve();
  if(sc.isFailure()){
    ATH_MSG_WARNING("Unable to find tool for CaloNoiseTool - Use absolute value for energy thresholds");
    m_useNoiseToolGlobal=false;
    return StatusCode::SUCCESS;
  }
  else {
    ATH_MSG_INFO("CaloNoiseTool retrieved");
  }

  // retrieve trigger decision tool:
  if( m_useTrigger) {
    if( (!m_rndmTriggerNames.empty()) || (!m_caloTriggerNames.empty()) || (!m_minBiasTriggerNames.empty()) ) {
      sc = m_trigDec.retrieve();
      if( !sc.isSuccess() ) {
        ATH_MSG_ERROR("Unable to retrieve the TrigDecisionTool");
        m_useTrigger = false;
        return sc;
      }
      ATH_MSG_INFO("TrigDecisionTool retrieved");
      if( !m_rndmTriggerNames.empty() ){
        m_rndmTriggerEvent = m_trigDec->getChainGroup(m_rndmTriggerNames.c_str());
        m_isRndmTrigDefined = true;
      }
      else {
        m_isRndmTrigDefined = false;
      }
      if( !m_caloTriggerNames.empty() ) {
        m_caloTriggerEvent = m_trigDec->getChainGroup(m_caloTriggerNames.c_str());
        m_isCaloTrigDefined = true;
      }
      else {
        m_isCaloTrigDefined = false;
      }
      if( !m_minBiasTriggerNames.empty() ) {
        m_minBiasTriggerEvent = m_trigDec->getChainGroup(m_minBiasTriggerNames.c_str());
        m_isMinBiasTrigDefined = true;
      }
      else {
        m_isMinBiasTrigDefined = false;
      }
      if( !m_metTriggerNames.empty() ) {
        m_metTriggerEvent = m_trigDec->getChainGroup(m_metTriggerNames.c_str());
        m_isMetTrigDefined = true;
      }
      else {
        m_isMetTrigDefined = false;
      }
      if( !m_miscTriggerNames.empty() ) {
        m_miscTriggerEvent = m_trigDec->getChainGroup(m_miscTriggerNames.c_str());
        m_isMiscTrigDefined = true;
      }
      else {
        m_isMiscTrigDefined = false;
      }
    }
    else {
      m_useTrigger = false;
    }
  }


 ATH_MSG_INFO("end of retrieveTools()");
 return sc; 
} 

void CaloCellVecMon::initThreshValues() {
 
  /*
   * Define properties of each threshold type, including histograms to generate, default use noise tool choice, trigger, default name, etc.
   */
//  ATH_MSG_INFO("in initThreshValues()");
  
  std::map<std::string, int> threshTypeToDirection;
  std::map<std::string, int> threshTypeToTriggersToInclude;
  std::map<std::string, int> threshTypeToTriggersToExclude;

  std::map<std::string, bool> threshTypeToMaskEmptyBins;
  std::map<std::string, bool> threshTypeToDoBeamBackgroundRemoval;

  std::map<std::string, bool> threshTypeToDoPercentageOccupancy;
  std::map<std::string, bool> threshTypeToDoEtaPhiOccupancy;
  std::map<std::string, bool> threshTypeToDoEtaOccupancy;
  std::map<std::string, bool> threshTypeToDoPhiOccupancy;

  std::map<std::string, bool> threshTypeToDoEtaPhiTotalEnergy;
  std::map<std::string, bool> threshTypeToDoEtaPhiAverageEnergy;
  std::map<std::string, bool> threshTypeToDoEtaPhiEnergyRMS;
  std::map<std::string, bool> threshTypeToDoEtaPhiDeviationFromDBnoise;

  std::map<std::string, bool> threshTypeToDoEtaPhiAverageQuality;
  std::map<std::string, bool> threshTypeToDoEtaPhiFractionOverQth;
  std::map<std::string, bool> threshTypeToDoEtaPhiAverageTime;
  std::map<std::string, bool> threshTypeToDoEtaPhiFractionPastTth;
  std::map<std::string, float> threshTypeToQualityThreshold;
  std::map<std::string, float> threshTypeToTimeThreshold;

  std::map<std::string, std::string> threshTypeToDefaultThresholdName;
  std::map<std::string, std::string> threshTypeToDefaultThresholdTitle;
  std::map<std::string, float>  threshTypeToDefaultThreshold;
  std::map<std::string, bool>   threshTypeToDefaultUseNoiseTool;

 
  //Set hard coded defaults:
  threshTypeToDirection["noEth"]   = NONE;
  threshTypeToDirection["medEth"]  = OVER;
  threshTypeToDirection["hiEth"]   = OVER;
  threshTypeToDirection["3Sigma"]  = BOTH;
  threshTypeToDirection["4Sigma"]  = BOTH;
  threshTypeToDirection["5Sigma"]  = OVER;
  threshTypeToDirection["negEth"]  = UNDER;
  threshTypeToDirection["default"] = OVER;

  threshTypeToTriggersToInclude["default"] = NOTA;
  threshTypeToTriggersToExclude["default"] = NOTA;

  threshTypeToMaskEmptyBins["default"] = false;
  threshTypeToDoBeamBackgroundRemoval["default"] = true;

  threshTypeToDoPercentageOccupancy["default"] = true;
  threshTypeToDoEtaPhiOccupancy["default"]     = true;
  threshTypeToDoEtaOccupancy["default"]        = false;
  threshTypeToDoPhiOccupancy["default"]        = false;

  threshTypeToDoEtaPhiTotalEnergy["default"]   = false;
  threshTypeToDoEtaPhiAverageEnergy["default"] = false;
  threshTypeToDoEtaPhiEnergyRMS["default"] = false;
  threshTypeToDoEtaPhiDeviationFromDBnoise["default"] = false;

  threshTypeToDoEtaPhiAverageQuality["default"]  = false;
  threshTypeToDoEtaPhiFractionOverQth["default"] = false;
  threshTypeToDoEtaPhiAverageTime["default"]     = false;
  threshTypeToDoEtaPhiFractionPastTth["default"] = false;

  threshTypeToQualityThreshold["default"] = 4000.;
  threshTypeToTimeThreshold["default"]    = 5.;
 
  threshTypeToDefaultUseNoiseTool["noEth"]   = false;
  threshTypeToDefaultUseNoiseTool["medEth"]  = true;
  threshTypeToDefaultUseNoiseTool["hiEth"]   = false;
  threshTypeToDefaultUseNoiseTool["default"] = false;

  std::string firstNoiseType; // The first type specified with doEtaPhiRMS has different 
                              // default triggers and will be syncronized with other noise plots.
 // The first threshold type m_doEtaPhiEnergyRMSTemp=1 or m_doEtaPhiDeviationFromDBnoiseTemp =1 
 // Then m_triggersToIncludeInNoisePlot is m_triggerToIncludeTemp corresponding to  this threshold type or RNDM if no m_triggerToIncludeTemp found to this threshold type or firstNoiseType=0 . 
// m_triggersToExcludeInNoisePlot is m_triggerToExcludeTemp corresponding to  this threshold type, or corresponding to default, or NONA if no m_triggerToExclude found to this threshold type or firstNoiseType =0. 
// Then m_triggersToIncludeInNoisePlot and m_triggersToExcludeInNoisePlot need to be checked when fill m_h_n_trigEvent bin 7 and h_dbNoiseNormalizedEnerg.
  
  
  for( int ity = 0; ity < (int)m_thresholdTypeTemp.value().size(); ity++ ) {
    std::string type =  m_thresholdTypeTemp.value()[ity];

    //Set the direction of each energy threshold:
    if( ity < (int)m_thresholdDirectionTemp.value().size() ) {
      if( (m_thresholdDirectionTemp.value()[ity]).compare("over") == 0) {
	threshTypeToDirection[type] = OVER;
      }
      else if((m_thresholdDirectionTemp.value()[ity]).compare("under") == 0) {
	threshTypeToDirection[type] = UNDER;
      }
      else if((m_thresholdDirectionTemp.value()[ity]).compare("none") == 0) {
	threshTypeToDirection[type] = NONE;
      }
      else if((m_thresholdDirectionTemp.value()[ity]).compare("both") == 0) {
	threshTypeToDirection[type] = BOTH;
      }
    }

    //Set which triggers are to be used for each threshold type:
    if( ity < (int)m_triggersToIncludeTemp.value().size() ) {
      if( ( (m_triggersToIncludeTemp.value()[ity]).compare("nota") == 0 ) || 
	  ( (m_triggersToIncludeTemp.value()[ity]).compare("all")  == 0 )    ) {
	threshTypeToTriggersToInclude[type] = NOTA;
      }
      else if ( (m_triggersToIncludeTemp.value()[ity]).compare("rndm") == 0 ) {	
	threshTypeToTriggersToInclude[type] = RNDM;
      }
      else if ( (m_triggersToIncludeTemp.value()[ity]).compare("calo") == 0 ) {	
	threshTypeToTriggersToInclude[type] = CALO;
      }
      else if ( (m_triggersToIncludeTemp.value()[ity]).compare("minBias") == 0 ) {	
	threshTypeToTriggersToInclude[type] = MINBIAS;
      }
      else if ( (m_triggersToIncludeTemp.value()[ity]).compare("met") == 0 ) {	
	threshTypeToTriggersToInclude[type] = MET;
      }
      else if ( (m_triggersToIncludeTemp.value()[ity]).compare("misc") == 0 ) {	
	threshTypeToTriggersToInclude[type] = MISC;
      }
    }
    
    //Set which triggers are not to be used for each threshold type:
    if( ity < (int)m_triggersToExcludeTemp.value().size() ) {
      if( (m_triggersToExcludeTemp.value()[ity]).compare("nota") == 0 ||
	  (m_triggersToExcludeTemp.value()[ity]).compare("none") == 0    ) {
	threshTypeToTriggersToExclude[type] = NOTA;
      }
      else if ( (m_triggersToExcludeTemp.value()[ity]).compare("rndm") == 0 ) {	
	threshTypeToTriggersToExclude[type] = RNDM;
      }
      else if ( (m_triggersToExcludeTemp.value()[ity]).compare("calo") == 0 ) {	
	threshTypeToTriggersToExclude[type] = CALO;
      }
      else if ( (m_triggersToExcludeTemp.value()[ity]).compare("minBias") == 0 ) {	
	threshTypeToTriggersToExclude[type] = MINBIAS;
      }
      else if ( (m_triggersToExcludeTemp.value()[ity]).compare("met") == 0 ) {	
	threshTypeToTriggersToExclude[type] = MET;
      }
      else if ( (m_triggersToExcludeTemp.value()[ity]).compare("misc") == 0 ) {	
	threshTypeToTriggersToExclude[type] = MISC;
      }
    }    

    if( ity < (int)m_maskEmptyBinsTemp.value().size() ) {
      threshTypeToMaskEmptyBins[type] = m_maskEmptyBinsTemp.value()[ity];
    }

    if( ity < (int)m_doBeamBackgroundRemovalTemp.value().size() ) {
      threshTypeToDoBeamBackgroundRemoval[type] = m_doBeamBackgroundRemovalTemp.value()[ity];
    }
   
    if( ity < (int)m_doPercentageOccupancyTemp.value().size() ) {
      threshTypeToDoPercentageOccupancy[type] = m_doPercentageOccupancyTemp.value()[ity];
    }

    if( ity < (int)m_doEtaPhiOccupancyTemp.value().size() ){
      threshTypeToDoEtaPhiOccupancy[type] = m_doEtaPhiOccupancyTemp.value()[ity];
    }
    if( ity < (int)m_doEtaOccupancyTemp.value().size() ){
      threshTypeToDoEtaOccupancy[type] = m_doEtaOccupancyTemp.value()[ity];
    }
    if( ity < (int)m_doPhiOccupancyTemp.value().size() ){
      threshTypeToDoPhiOccupancy[type] = m_doPhiOccupancyTemp.value()[ity];
    }
    if( ity < (int)m_doEtaPhiAverageEnergyTemp.value().size() ){
      threshTypeToDoEtaPhiAverageEnergy[type] = m_doEtaPhiAverageEnergyTemp.value()[ity];
    }
    if( ity < (int)m_doEtaPhiTotalEnergyTemp.value().size() ){
      threshTypeToDoEtaPhiTotalEnergy[type] = m_doEtaPhiTotalEnergyTemp.value()[ity];
    }
    if( ity < (int)m_doEtaPhiEnergyRMSTemp.value().size() ){
      threshTypeToDoEtaPhiEnergyRMS[type] = m_doEtaPhiEnergyRMSTemp.value()[ity];
      if( m_doEtaPhiEnergyRMSTemp.value()[ity] && (firstNoiseType.size() == 0) ) {
	firstNoiseType = type;
      }
    }
    if( ity < (int)m_doEtaPhiDeviationFromDBnoiseTemp.value().size() ){
      threshTypeToDoEtaPhiDeviationFromDBnoise[type] = m_doEtaPhiDeviationFromDBnoiseTemp.value()[ity];
      if( m_doEtaPhiDeviationFromDBnoiseTemp.value()[ity] && (firstNoiseType.size() == 0) ) {
	firstNoiseType = type;
      }
    }
    if( ity < (int)m_doEtaPhiAverageQualityTemp.value().size() ){
      threshTypeToDoEtaPhiAverageQuality[type] = m_doEtaPhiAverageQualityTemp.value()[ity];
    }
    if( ity < (int)m_doEtaPhiFractionOverQthTemp.value().size() ){
      threshTypeToDoEtaPhiFractionOverQth[type] = m_doEtaPhiFractionOverQthTemp.value()[ity];
    }
    if( ity < (int)m_doEtaPhiAverageTimeTemp.value().size() ){
      threshTypeToDoEtaPhiAverageTime[type] = m_doEtaPhiAverageTimeTemp.value()[ity];
    }
    if( ity < (int)m_doEtaPhiFractionPastTthTemp.value().size() ){
      threshTypeToDoEtaPhiFractionPastTth[type] = m_doEtaPhiFractionPastTthTemp.value()[ity];
    }

    //Set the quality threshold: (note, this is for quality plots only)
    if( ity < (int)m_qualityFactorThresholdTemp.value().size() ) {
      threshTypeToQualityThreshold[type] = m_qualityFactorThresholdTemp.value()[ity];
    }
    
    //Set the time threshold: (note, this is for time plots only)
    if( ity < (int)m_timeThresholdTemp.value().size() ) {
      threshTypeToTimeThreshold[type] = m_timeThresholdTemp.value()[ity];
    }
  
    //Set default name:
    if( ity < (int)m_defaultThresholdNames.value().size() ) {
      threshTypeToDefaultThresholdName[type] = m_defaultThresholdNames.value()[ity];
    }
  
    //Set default title
    if( ity < (int)m_defaultThresholdTitles.value().size() ) {
      threshTypeToDefaultThresholdTitle[type] = m_defaultThresholdTitles.value()[ity];
    }

    //Set default use noise tool choice
    if( ity < (int)m_defaultUseNoiseTool.value().size() ){
      threshTypeToDefaultUseNoiseTool[type] = m_defaultUseNoiseTool.value()[ity];
    }

    //Set the default energy threshold for this threshold type:
    if( ity < (int)m_defaultThresholds.value().size() ) {
      if( m_defaultThresholds.value()[ity] != -9999999. ) {
	threshTypeToDefaultThreshold[type] = m_defaultThresholds.value()[ity];
      }
    }
  }
  
  // Syncronize non-thresholded noise plots (dbNormalizedEnergy) with the first type of thresholded noise plot
  // Set special hard coded defaults if no trigger chain groups were specified.
  if(firstNoiseType.size() != 0){
    std::map<std::string, int>::const_iterator iitr = threshTypeToTriggersToInclude.find(firstNoiseType);
    if (iitr != threshTypeToTriggersToInclude.end()) {
      m_triggersToIncludeInNoisePlots = iitr->second;
    }
    else {
      threshTypeToTriggersToInclude[firstNoiseType] = RNDM;
      m_triggersToIncludeInNoisePlots = RNDM;
    }
    iitr = threshTypeToTriggersToExclude.find(firstNoiseType);
    if (iitr != threshTypeToTriggersToExclude.end()) {
      m_triggersToExcludeFromNoisePlots = iitr->second;
    }
    else {
      // if there is a default set of triggers to exclude, use this.
      iitr = threshTypeToTriggersToExclude.find("default");
      if (iitr != threshTypeToTriggersToExclude.end()) {
	threshTypeToTriggersToInclude[firstNoiseType] = iitr->second; //Default should be picked up later, but do this to be sure.
	m_triggersToExcludeFromNoisePlots = iitr->second;
      }
      else {
	// Assume random trigger is clean and exlude nothing by default.
	threshTypeToTriggersToInclude[firstNoiseType] = NOTA;
	m_triggersToExcludeFromNoisePlots = NOTA;
      }
    }
  }
  else {
    //Since there are no thresholded noise plots, just set reasonable defaults for the unthresholded ones.
    m_triggersToIncludeInNoisePlots = RNDM;
    // if there is a default set of triggers to exclude, use this.
    std::map<std::string, int>::const_iterator iitr = threshTypeToTriggersToExclude.find("default");
    if (iitr != threshTypeToTriggersToExclude.end()) {
      m_triggersToExcludeFromNoisePlots = iitr->second;
    }
    else {
      m_triggersToExcludeFromNoisePlots = NOTA;
    }
  }
 

  /*
   * Define default explicit values for each layer:
   */

  float defaultThresh[MAXLYRNS][3];
  float defaultNoise[MAXLYRNS][3];

  // EMB
  defaultThresh[EMBPNS][0]=-4000000.; 
  defaultThresh[EMBPNS][1]=150.;
  if( m_dataType == AthenaMonManager::cosmics ) defaultThresh[EMBPNS][2]=1000.;
  else defaultThresh[EMBPNS][2]=1000.;

  defaultThresh[EMB1NS][0]=-4000000.; 
  defaultThresh[EMB1NS][1]=150.;
  if( m_dataType == AthenaMonManager::cosmics ) defaultThresh[EMB1NS][2]=1000.;
  else defaultThresh[EMB1NS][2]=1000.;

  defaultThresh[EMB2NS][0]=-4000000.; 
  defaultThresh[EMB2NS][1]=150.;
  if( m_dataType == AthenaMonManager::cosmics ) defaultThresh[EMB2NS][2]=1000.;
  else defaultThresh[EMB2NS][2]=1000.;

  defaultThresh[EMB3NS][0]=-4000000.; 
  defaultThresh[EMB3NS][1]=150.;
  if( m_dataType == AthenaMonManager::cosmics ) defaultThresh[EMB3NS][2]=1000.;
  else defaultThresh[EMB3NS][2]=1000.;


  // HEC
  defaultThresh[HEC0NS][0]=-4000000.; 
  defaultThresh[HEC0NS][1]=600.; // ~202x3 MeV
  if( m_dataType == AthenaMonManager::cosmics ) defaultThresh[HEC0NS][2]=1000.;
  else defaultThresh[HEC0NS][2]=20000.;
  
  defaultThresh[HEC1NS][0]=-4000000.; 
  defaultThresh[HEC1NS][1]=840.; // ~281x3
  if( m_dataType == AthenaMonManager::cosmics ) defaultThresh[HEC1NS][2]=2000.;
  else defaultThresh[HEC1NS][2]=20000.;
  
  defaultThresh[HEC2NS][0]=-4000000.; 
  defaultThresh[HEC2NS][1]=1270.; // ~423x3
  if( m_dataType == AthenaMonManager::cosmics ) defaultThresh[HEC2NS][2]=3000.;
  else defaultThresh[HEC2NS][2]=20000.;
  
  defaultThresh[HEC3NS][0]=-4000000.; 
  defaultThresh[HEC3NS][1]=1800.; // ~450x4 and probably more...
  if( m_dataType == AthenaMonManager::cosmics ) defaultThresh[HEC3NS][2]=5000.;
  else defaultThresh[HEC3NS][2]=20000.;

  // EMEC
  defaultThresh[EMECPNS][0]=-4000000.; 
  defaultThresh[EMECPNS][1]=340.; // ~114x3
  if( m_dataType == AthenaMonManager::cosmics ) defaultThresh[EMECPNS][2]=1000.;
  else defaultThresh[EMECPNS][2]=5000.;
  
  defaultThresh[EMEC1NS][0]=-4000000.; 
  defaultThresh[EMEC1NS][1]=75.; // ~25x3
  if( m_dataType == AthenaMonManager::cosmics ) defaultThresh[EMEC1NS][2]=300.;
  else defaultThresh[EMEC1NS][2]=5000.;
  
  defaultThresh[EMEC2NS][0]=-4000000.; 
  defaultThresh[EMEC2NS][1]=90.; // ~31x3
  if( m_dataType == AthenaMonManager::cosmics ) defaultThresh[EMEC2NS][2]=300.;
  else defaultThresh[EMEC2NS][2]=5000.;

  defaultThresh[EMEC3NS][0]=-4000000.; 
  defaultThresh[EMEC3NS][1]=90.; // ~27x3
  if( m_dataType == AthenaMonManager::cosmics ) defaultThresh[EMEC3NS][2]=300.;
  else defaultThresh[EMEC3NS][2]=5000.;
  
  // FCAL
  defaultThresh[FCAL1NS][0]=-4000000.; 
  defaultThresh[FCAL1NS][1]=540.; // ~180x3 MeV
  if( m_dataType == AthenaMonManager::cosmics ) defaultThresh[FCAL1NS][2]=500.;
  else defaultThresh[FCAL1NS][2]=60000.;
  
  defaultThresh[FCAL2NS][0]=-4000000.; 
  defaultThresh[FCAL2NS][1]=1090.; // ~362x3
  if( m_dataType == AthenaMonManager::cosmics ) defaultThresh[FCAL2NS][2]=5000.;
  else defaultThresh[FCAL2NS][2]=60000.;
  
  defaultThresh[FCAL3NS][0]=-4000000.; 
  defaultThresh[FCAL3NS][1]=1240.; // ~414x3
  if( m_dataType == AthenaMonManager::cosmics ) defaultThresh[FCAL3NS][2]=5000.;
  else defaultThresh[FCAL3NS][2]=60000.;
  
 /*
   * Define default noise values
   */


  for(int ilyr=0; ilyr<MAXLYRNS; ilyr++) {
    //lastDefaultUseNoiseTool[ilyr][0] = false; // use explicit value
    //lastDefaultUseNoiseTool[ilyr][1] = true;  // use noise
    //lastDefaultUseNoiseTool[ilyr][2]  = false; // use explicit value

    defaultNoise[ilyr][0]=-4000000;
    defaultNoise[ilyr][1]=5;
    if( m_dataType == AthenaMonManager::cosmics ) {
      defaultNoise[ilyr][2]=10;
    }
    else {
      defaultNoise[ilyr][2]=20;
    }
  }

 
  /*
   * Now set the array values accordingly
   */

  //Test Variable:
  bool noiseBasedThresholdsUsed = false;

  m_doNActiveCellsFirstEventVsEta = false;
  m_doNActiveCellsFirstEventVsPhi = false;

  for(int ilyrns = EMBPNS; ilyrns < MAXLYRNS; ilyrns++) {

    // Determine number of thresholds to use:
    // Should correspond to the number of (unique) threshold types for this layer:
    // If no thresholds were given at all, use the three default types hardcoded above:
    std::vector<std::string> types;
    if( (m_thresholdsTemp[ilyrns].value().size() == 0) && (m_defaultThresholds.value().size() == 0) ){
      types.push_back( "noEth" );
      types.push_back( "medEth" );
      types.push_back( "hiEth" );
    }
    else {
      //First add the types specifically given thresholds for this layer
      for( int i = 0; i < (int)m_thresholdsTemp[ilyrns].value().size(); i++) {
	//Use the type for this column if one is given
	if ( i < (int)m_thresholdColumnType.value().size() ) {
	  types.push_back( (m_thresholdColumnType.value())[i] );
	}
	//Otherwise if the name is specified, use this for the type
	else if( i < (int)m_threshNamesTemp[ilyrns].value().size() ) {
	  types.push_back( (m_threshNamesTemp[ilyrns].value())[i] );
	}
	//Otherwise, use a default treshold type:
	else {
	  if ( i < (int)m_thresholdTypeTemp.value().size() ) {
	    types.push_back( (m_thresholdTypeTemp.value()[i]) );
	  }
	  else {
	    types.push_back( "default" );
	  }
	}
      }
      //Now add types for which a default threshold is defined (not equal to -9999999.):
      for( int i = 0; i < (int)m_defaultThresholds.value().size(); i++ ) {
	if( m_defaultThresholds.value()[i] == -9999999. ) {
	  continue;
	}
	if ( i < (int)m_thresholdTypeTemp.value().size() ) {
	  if ( find(types.begin(),types.end(), (m_thresholdTypeTemp.value())[i] ) == types.end() ) {
	    types.push_back( (m_thresholdTypeTemp.value())[i] );
	  }
	}
	else {
	  types.push_back( "default" );
	}
      }
    }
   
    m_nThresholds[ilyrns] = types.size();
    
    const int nThr = m_nThresholds[ilyrns];
    
  
    // Allocate memory
      
    m_thresholdDirection[ilyrns]      = new int[nThr];
    if( m_useTrigger ) {
      m_threshTriggersToInclude[ilyrns] = new int[nThr];
      m_threshTriggersToExclude[ilyrns] = new int[nThr];
      m_threshTriggerDecision[ilyrns]   = new bool[nThr];
      m_nEventsPassingTrigger[ilyrns] = new int[nThr];
    }

    m_nEventsPassingBeamBG[ilyrns] = new int[nThr];
    m_useNoiseTool[ilyrns]            = new bool[nThr]; 
    m_thresholds[ilyrns]   =    new float[nThr];
    m_threshNames[ilyrns]  =    new std::string[nThr];
    m_threshTitles[ilyrns] =    new std::string[nThr];

    m_maskEmptyBins[ilyrns]         = new bool[nThr];
    m_doBeamBackgroundRemoval[ilyrns]         = new bool[nThr];

    m_doPercentageOccupancy[ilyrns] = new bool[nThr];
    m_fillEtaPhiOccupancy[ilyrns]   =   new bool[nThr];
    m_tempEtaPhiOccupancy[ilyrns]   =   new bool[nThr];
    m_procEtaPhiOccupancy[ilyrns]     =   new bool[nThr];
    m_fillEtaOccupancy[ilyrns]      =   new bool[nThr];
    m_procEtaOccupancy[ilyrns]        =   new bool[nThr];
    m_fillPhiOccupancy[ilyrns]      =   new bool[nThr];
    m_procPhiOccupancy[ilyrns]        =   new bool[nThr];

    m_fillEtaPhiTotalEnergy[ilyrns] =   new bool[nThr];
    m_tempEtaPhiTotalEnergy[ilyrns] =   new bool[nThr];
    m_procEtaPhiTotalEnergy[ilyrns]   =   new bool[nThr];
    m_doEtaPhiEnergyProfile[ilyrns] =   new bool[nThr];
    m_doEtaPhiAverageEnergy[ilyrns] =   new bool[nThr];
    m_procEtaPhiEnergyRMS[ilyrns]     =   new bool[nThr];
    m_procEtaPhiDeviationFromDBnoise[ilyrns] =   new bool[nThr];

    m_fillEtaPhiQualityOccupancy[ilyrns] =   new bool[nThr];
    m_fillEtaPhiTotalQuality[ilyrns]     =   new bool[nThr];
    m_procEtaPhiAverageQuality[ilyrns]  =   new bool[nThr];
    m_procEtaPhiFractionOverQth[ilyrns] =   new bool[nThr];
    m_fillEtaPhiTimeOccupancy[ilyrns]    =   new bool[nThr];
    m_fillEtaPhiTotalTime[ilyrns]        =   new bool[nThr];
    m_procEtaPhiAverageTime[ilyrns]     =   new bool[nThr];
    m_procEtaPhiFractionPastTth[ilyrns] =   new bool[nThr];

    m_qualityThresholds[ilyrns] = new float[nThr];
    m_timeThresholds[ilyrns]    = new float[nThr];

    //Loop over the thresholds which are defined for this layer:
    for(int ti=0; ti<nThr; ti++) {
      
      if( m_useTrigger ) {
	m_nEventsPassingTrigger[ilyrns][ti] = 0;
      }
      m_nEventsPassingBeamBG[ilyrns][ti] = 0;

      std::string type = types[ti];
      std::string name;
      std::string title;
 
      // Set threshold direction:
      std::map<std::string, int>::const_iterator iitr = threshTypeToDirection.find(type);
      if (iitr == threshTypeToDirection.end()) {
	iitr = threshTypeToDirection.find("default");
      }
      m_thresholdDirection[ilyrns][ti]= iitr->second;

      if( m_useTrigger ) {
	// Set threshold triggers to include:
	iitr = threshTypeToTriggersToInclude.find(type);
	if (iitr == threshTypeToTriggersToInclude.end()) {
	  iitr = threshTypeToTriggersToInclude.find("default");
	}
	m_threshTriggersToInclude[ilyrns][ti]= iitr->second;
	
	// Set threshold triggers to include:
	iitr = threshTypeToTriggersToExclude.find(type);
	if (iitr == threshTypeToTriggersToExclude.end()) {
	  iitr = threshTypeToTriggersToExclude.find("default");
	}
	m_threshTriggersToExclude[ilyrns][ti]= iitr->second;

	// Initialize the threshTriggerDecision array to true, actual decisions will be made in fill.
	// The expectation is that most thresholds will pass most triggers, so this should be the efficient way to do this:
	m_threshTriggerDecision[ilyrns][ti] = true;
      }

      //Decide whether this is an absolute or noise based threshold:
      if(m_useNoiseToolGlobal) {
   
	if(ti < (int)m_useNoiseToolTemp[ilyrns].value().size()) { 
	  m_useNoiseTool[ilyrns][ti] = (m_useNoiseToolTemp[ilyrns].value())[ti];
	}
	else {
	  std::map<std::string, bool>::const_iterator ntitr = threshTypeToDefaultUseNoiseTool.find(type);
	  if (ntitr == threshTypeToDefaultUseNoiseTool.end()) {
	    ntitr = threshTypeToDefaultUseNoiseTool.find("default");
	  }
	  m_useNoiseTool[ilyrns][ti] = ntitr->second;
	} 
      }
      else { 
	// if the global useNoiseTool flag is false, set to false:
	m_useNoiseTool[ilyrns][ti] = false;
      }
      
      if (m_useNoiseTool[ilyrns][ti]) {
	noiseBasedThresholdsUsed = true;
      }
   

      // Set threshold name, will be added to histogram name for each threshold/layer
      if( ti < (int)m_threshNamesTemp[ilyrns].value().size()) {
	//If there is a layer specific threshold name then use it:
	name = m_thresholdTypeTemp.value()[ti];
      }
      else {
	//If a default name was specified for this type number, use that:
	std::map<std::string,std::string>::iterator itr = threshTypeToDefaultThresholdName.find(type);
	if(itr != threshTypeToDefaultThresholdName.end()) {
	  name = itr->second;
	}
	else {
	  if( type.compare("default") == 0 ) {
	    char temp[32];
	    sprintf(temp,"threshold_%i",ti+1);
	    name = temp;
	  }
	  else {
	    name = type;
	  }
	}
      }

      m_threshNames[ilyrns][ti] = name;
  

      // If the thresholds themselves are not explicitly set in the jo, we set them to the defaults:
      if( (ti < 3) && (m_thresholdsTemp[ilyrns].value().size() == 0) && (m_defaultThresholds.value().size() == 0) ) {      
      	if(m_useNoiseTool[ilyrns][ti]) {
	  m_thresholds[ilyrns][ti] = defaultThresh[ilyrns][ti];
	}
	else {
	  m_thresholds[ilyrns][ti] = defaultNoise[ilyrns][ti];
	}
      }
      else {
	//Set to layer specific threhsolds first, if any
	if(ti < (int)m_thresholdsTemp[ilyrns].value().size()) {
	  m_thresholds[ilyrns][ti] = (m_thresholdsTemp[ilyrns].value())[ti];
	}
	//Otherwise, use defaults set for this layer type (first check JO, then hard coded):
	else{
	  std::map<std::string, float>::iterator itr = threshTypeToDefaultThreshold.find(type);
	  if( itr == threshTypeToDefaultThreshold.end() ) {
	    itr = threshTypeToDefaultThreshold.find("default");
	  }
	  if( itr != threshTypeToDefaultThreshold.end() ) {
	    m_thresholds[ilyrns][ti] = itr->second;
	  }
	  else {
	    if( type.compare("hiEth") == 0 ) {
	      if(m_useNoiseTool[ilyrns][ti]) {
		m_thresholds[ilyrns][ti] = defaultThresh[ilyrns][2];
	      }
	      else {
		m_thresholds[ilyrns][ti] = defaultNoise[ilyrns][2];
	      }
	    }
	    else if ( type.compare("medEth") == 0 ) {
	      if(m_useNoiseTool[ilyrns][ti]) {
		m_thresholds[ilyrns][ti] = defaultThresh[ilyrns][1];
	      }
	      else {
		m_thresholds[ilyrns][ti] = defaultNoise[ilyrns][1];
	      }
	    }
	    else {
	      if(m_useNoiseTool[ilyrns][ti]) {
		  m_thresholds[ilyrns][ti] = defaultThresh[ilyrns][0];
	      }
	      else {
		m_thresholds[ilyrns][ti] = defaultNoise[ilyrns][0];
	      }     
	    }
	  }
	}
      }

      // Set threhsold title (will be added to histogram title for each threshold/layer)
      if(ti < (int)m_threshTitlesTemp[ilyrns].value().size()) {
	//If there is a layer specific title for this threshold then use it:
	title = m_threshTitlesTemp[ilyrns].value()[ti];
      }
      else {
	std::map<std::string,std::string>::iterator itr = threshTypeToDefaultThresholdTitle.find(type);
       	if(itr != threshTypeToDefaultThresholdTitle.end()) {
	  title = itr->second;
	}
	else {
	  if(m_useNoiseTool[ilyrns][ti]) {
	    title = "E_{cell} > %0.f#sigma_{noise}^{database}";
	  }
	  else {
	    title = "E_{cell} > %0.f MeV";
	  }
	}
      } 
     
      size_t stringPos = title.find( "%0.f" );
      if( stringPos != std::string::npos ) {
	char temp[8];
	sprintf(temp,"%0.f",m_thresholds[ilyrns][ti]);
	title.replace(stringPos, 4, temp);
      }
   
      //Insert the trigger name, if there is a place for it: (we use sprintf type syntax for interoperability)
      stringPos = title.find( "%s" );
      if(stringPos != std::string::npos) {
	if( m_useTrigger ) {
	  switch (m_threshTriggersToInclude[ilyrns][ti]) {
	  case RNDM :
	    title.replace( stringPos, 2, "random" );
	    break;
	  case CALO :
	    title.replace( stringPos, 2, "L1Calo" );
	    break;
	  case MINBIAS :
	    title.replace( stringPos, 2, "minimum bias" );
	    break;
	  case MET :
	    title.replace( stringPos, 2, "missing energy" );
	    break;
	  case MISC :
	    title.replace( stringPos, 2, "miscellaneous" );
	    break;
	  default:
	    title.replace( stringPos, 2, "all" );
	  }
	}
	else {
	  title.replace( stringPos, 2, "all" );
	}
      }
      //Check if there is a place to include excluded triggers:
      stringPos = title.find( "%s" );
      if(stringPos != std::string::npos) {
	if( m_useTrigger ) {
	  switch (m_threshTriggersToExclude[ilyrns][ti]) {
	  case RNDM :
	    title.replace( stringPos, 2, "excluding random" );
	    break;
	  case CALO :
	    title.replace( stringPos, 2, "excluding L1Calo" );
	    break;
	  case MINBIAS :
	    title.replace( stringPos, 2, "excluding minimum bias" );
	    break;
	  case MET :
	    title.replace( stringPos, 2, "excluding missing energy" );
	    break;
	  case MISC :
	    title.replace( stringPos, 2, "excluding miscellaneous" );
	    break;
	  default:
	    title.replace( stringPos, 2, "" );
	  }
	}
	else {
	  title.replace( stringPos, 2, "" );
	}
      }
      m_threshTitles[ilyrns][ti] = title;
            
      // Set quality threshold: (for quality plots only)
      std::map<std::string,float>::iterator ditr = threshTypeToQualityThreshold.find(type);
      
      if( ditr == threshTypeToQualityThreshold.end() ) {
	ditr = threshTypeToQualityThreshold.find("default");
      }
      m_qualityThresholds[ilyrns][ti] = ditr->second;
      
      // Set the time threshold: (for time plots only)
      ditr = threshTypeToTimeThreshold.find(type);
      if( ditr == threshTypeToTimeThreshold.end() ) {
	ditr = threshTypeToTimeThreshold.find("default");
      }
      m_timeThresholds[ilyrns][ti] = ditr->second;
    
      bool doPercentageOccupancy = threshTypeToDoPercentageOccupancy["default"];
      bool maskEmptyBins = threshTypeToMaskEmptyBins["default"];
      bool doBeamBackgroundRemoval = threshTypeToDoBeamBackgroundRemoval["default"];
       
      // Turn histograms on or off for each threshold in this layer:

      // Initialize to defaults:
      bool doEtaPhiOccupancy     = threshTypeToDoEtaPhiOccupancy["default"]; 
      bool doEtaOccupancy        = threshTypeToDoEtaOccupancy["default"];   
      bool doPhiOccupancy        = threshTypeToDoPhiOccupancy["default"];
      bool doEtaPhiAverageEnergy = threshTypeToDoEtaPhiAverageEnergy["default"];
      bool doEtaPhiTotalEnergy   = threshTypeToDoEtaPhiTotalEnergy["default"];
      bool doEtaPhiEnergyRMS     = threshTypeToDoEtaPhiEnergyRMS["default"];
      bool doEtaPhiDeviationFromDBnoise = threshTypeToDoEtaPhiDeviationFromDBnoise["default"];
      
      bool doEtaPhiAverageQuality  = threshTypeToDoEtaPhiAverageQuality["default"];
      bool doEtaPhiFractionOverQth = threshTypeToDoEtaPhiFractionOverQth["default"];
      bool doEtaPhiAverageTime     = threshTypeToDoEtaPhiAverageTime["default"];
      bool doEtaPhiFractionPastTth = threshTypeToDoEtaPhiFractionPastTth["default"];
     
      // Set value for this trigger type, if one was assigned:
      std::map<std::string, bool>::const_iterator fitr = threshTypeToDoPercentageOccupancy.find(type);
      if ( fitr != threshTypeToDoPercentageOccupancy.end()) {
	doPercentageOccupancy = fitr->second;
      }
      fitr = threshTypeToMaskEmptyBins.find(type);
      if (fitr != threshTypeToMaskEmptyBins.end()) {
	maskEmptyBins = fitr->second;
      }
      fitr = threshTypeToDoBeamBackgroundRemoval.find(type);
      if (fitr != threshTypeToDoBeamBackgroundRemoval.end()) {
        doBeamBackgroundRemoval = fitr->second;
      }
      fitr = threshTypeToDoEtaPhiOccupancy.find(type);
      if (fitr != threshTypeToDoEtaPhiOccupancy.end()) {
	doEtaPhiOccupancy = fitr->second;
      }
      
      fitr = threshTypeToDoEtaOccupancy.find(type);
      if (fitr != threshTypeToDoEtaOccupancy.end()) {
	doEtaOccupancy = fitr->second;
      }
      
      fitr = threshTypeToDoPhiOccupancy.find(type);
      if (fitr != threshTypeToDoPhiOccupancy.end()) {
	doPhiOccupancy = fitr->second;
      }

      fitr = threshTypeToDoEtaPhiTotalEnergy.find(type);
      if (fitr != threshTypeToDoEtaPhiTotalEnergy.end()) {
	doEtaPhiTotalEnergy = fitr->second;
      }

      fitr = threshTypeToDoEtaPhiAverageEnergy.find(type);
      if (fitr != threshTypeToDoEtaPhiAverageEnergy.end()) {
	doEtaPhiAverageEnergy = fitr->second;
      }
     
      fitr = threshTypeToDoEtaPhiEnergyRMS.find(type);
      if (fitr != threshTypeToDoEtaPhiEnergyRMS.end()) {
	doEtaPhiEnergyRMS = fitr->second;
      }

      fitr = threshTypeToDoEtaPhiDeviationFromDBnoise.find(type);
      if (fitr != threshTypeToDoEtaPhiDeviationFromDBnoise.end()) {
	doEtaPhiDeviationFromDBnoise = fitr->second;
      }

      fitr = threshTypeToDoEtaPhiAverageQuality.find(type);
      if (fitr != threshTypeToDoEtaPhiAverageQuality.end()) {
	doEtaPhiAverageQuality = fitr->second;
      }

      fitr = threshTypeToDoEtaPhiFractionOverQth.find(type);
      if (fitr != threshTypeToDoEtaPhiFractionOverQth.end()) {
	doEtaPhiFractionOverQth = fitr->second;
      }

      fitr = threshTypeToDoEtaPhiAverageTime.find(type);
      if (fitr != threshTypeToDoEtaPhiAverageTime.end()) {
	doEtaPhiAverageTime = fitr->second;
      }

      fitr = threshTypeToDoEtaPhiFractionPastTth.find(type);
      if (fitr != threshTypeToDoEtaPhiFractionPastTth.end()) {
	doEtaPhiFractionPastTth = fitr->second;
      }

      if (!m_useNoiseToolGlobal) {
	doEtaPhiDeviationFromDBnoise = false;
      }
     
      // Set the array values for each layer/threshold combination:

      m_doPercentageOccupancy[ilyrns][ti] = doPercentageOccupancy;
      m_maskEmptyBins[ilyrns][ti] = maskEmptyBins;
      m_doBeamBackgroundRemoval[ilyrns][ti] = doBeamBackgroundRemoval;

      // Based on the above decisions, decide where histograms should be produced: in fillHistograms or procHistograms?
      // These decisions are designed to use the minimum memory/cpu in fill

      if( doEtaPhiDeviationFromDBnoise && (doEtaPhiEnergyRMS || doEtaPhiTotalEnergy || doEtaPhiOccupancy) ) {
	m_doDatabaseNoiseVsEtaPhi = true;
      }
     
      m_procEtaPhiEnergyRMS[ilyrns][ti]     = doEtaPhiEnergyRMS;
      m_doEtaPhiEnergyProfile[ilyrns][ti]   = ( doEtaPhiEnergyRMS || doEtaPhiDeviationFromDBnoise );
      m_doEtaPhiAverageEnergy[ilyrns][ti]   = doEtaPhiAverageEnergy;
      m_procEtaPhiDeviationFromDBnoise[ilyrns][ti] = doEtaPhiDeviationFromDBnoise;

      m_procEtaPhiAverageQuality[ilyrns][ti]  = doEtaPhiAverageQuality;
      m_procEtaPhiFractionOverQth[ilyrns][ti] = doEtaPhiFractionOverQth;
      m_procEtaPhiAverageTime[ilyrns][ti]  = doEtaPhiAverageTime;
      m_procEtaPhiFractionPastTth[ilyrns][ti] = doEtaPhiFractionPastTth;
      
      if(doEtaPhiEnergyRMS || (doEtaPhiDeviationFromDBnoise && m_doDatabaseNoiseVsEtaPhi) ) {
	//Much can be proc'ed from a TProfile2D...
	m_fillEtaPhiOccupancy[ilyrns][ti]   = false;
	m_tempEtaPhiOccupancy[ilyrns][ti]   = false;
	m_procEtaPhiOccupancy[ilyrns][ti]   = doEtaPhiOccupancy;
	m_fillEtaOccupancy[ilyrns][ti]      = false;
	m_procEtaOccupancy[ilyrns][ti]      = doEtaOccupancy;
	m_fillPhiOccupancy[ilyrns][ti]      = false;
	m_procPhiOccupancy[ilyrns][ti]      = doPhiOccupancy;
	m_fillEtaPhiTotalEnergy[ilyrns][ti] = false;
	m_tempEtaPhiTotalEnergy[ilyrns][ti] = false;
	m_procEtaPhiTotalEnergy[ilyrns][ti] = doEtaPhiTotalEnergy;
      }
      else if(doEtaPhiAverageEnergy) {
	//Need total energy and occupancy to proc average energy:
	m_fillEtaPhiOccupancy[ilyrns][ti]   = true;
	m_tempEtaPhiOccupancy[ilyrns][ti]   = ( (!doEtaPhiOccupancy) || doPercentageOccupancy ) ;
	m_procEtaPhiOccupancy[ilyrns][ti]   = false;
	m_fillEtaOccupancy[ilyrns][ti]      = false;
	m_procEtaOccupancy[ilyrns][ti]      = doEtaOccupancy;
	m_fillPhiOccupancy[ilyrns][ti]      = false;
	m_procPhiOccupancy[ilyrns][ti]      = doPhiOccupancy;
	m_fillEtaPhiTotalEnergy[ilyrns][ti] = true;
        m_tempEtaPhiTotalEnergy[ilyrns][ti] = !doEtaPhiTotalEnergy;
	m_procEtaPhiTotalEnergy[ilyrns][ti] = false;
      }
      else {
	if(doEtaPhiOccupancy) {
	  m_fillEtaPhiOccupancy[ilyrns][ti] = true;
	  m_tempEtaPhiOccupancy[ilyrns][ti] = doPercentageOccupancy;
	  m_fillEtaOccupancy[ilyrns][ti]    = false;
	  m_fillPhiOccupancy[ilyrns][ti]    = false;
	  m_procEtaPhiOccupancy[ilyrns][ti] = false;
	  m_procEtaOccupancy[ilyrns][ti]    = doEtaOccupancy;
	  m_procPhiOccupancy[ilyrns][ti]    = doPhiOccupancy;
	}
	else {
	  m_fillEtaPhiOccupancy[ilyrns][ti] = false;
	  m_tempEtaPhiOccupancy[ilyrns][ti] = false;
	  m_fillEtaOccupancy[ilyrns][ti]    = doEtaOccupancy;
	  m_fillPhiOccupancy[ilyrns][ti]    = doPhiOccupancy;
	  m_procEtaPhiOccupancy[ilyrns][ti] = false;
	  m_procEtaOccupancy[ilyrns][ti]    = false;
	  m_procPhiOccupancy[ilyrns][ti]    = false;
	}

	m_fillEtaPhiTotalEnergy[ilyrns][ti] = doEtaPhiTotalEnergy;
	m_tempEtaPhiTotalEnergy[ilyrns][ti] = false;
	m_procEtaPhiTotalEnergy[ilyrns][ti] = false;
      }

      m_fillEtaPhiTotalQuality[ilyrns][ti]     = doEtaPhiAverageQuality;
      m_fillEtaPhiQualityOccupancy[ilyrns][ti] = doEtaPhiFractionOverQth;
      m_fillEtaPhiTotalTime[ilyrns][ti]        = doEtaPhiAverageTime;
      m_fillEtaPhiTimeOccupancy[ilyrns][ti]    = doEtaPhiFractionPastTth;
     
      if( doEtaPhiAverageQuality || doEtaPhiFractionOverQth || doEtaPhiAverageTime || doEtaPhiFractionPastTth ) {
	if( ! m_fillEtaPhiOccupancy[ilyrns][ti] ) {
	  m_fillEtaPhiOccupancy[ilyrns][ti] = true;
	  m_tempEtaPhiOccupancy[ilyrns][ti] = true;
	}
      }

      if( doPercentageOccupancy ) {
	m_tempEtaPhiOccupancy[ilyrns][ti] = true;	
	m_procEtaPhiOccupancy[ilyrns][ti] = doEtaPhiOccupancy;

	if(doEtaOccupancy) {
	  m_procEtaOccupancy[ilyrns][ti] = true;
	  m_doNActiveCellsFirstEventVsEta = true;
	}
	if(doPhiOccupancy) {
	  m_procPhiOccupancy[ilyrns][ti] = true;
	  m_doNActiveCellsFirstEventVsPhi = true;
	}
      }
    }
  }
 
  //Switch off the noise tool if it will not be used anywhere:
  if( (!noiseBasedThresholdsUsed) && (!m_doDatabaseNoiseVsEtaPhi) && (!m_doDBNormalized1DEnergy) ) {
    m_useNoiseToolGlobal = false;
  }
//  ATH_MSG_INFO("end of initThreshValues()");
}

////////////////////////////////////////////
StatusCode CaloCellVecMon::bookHistogramsRecurrent(){

// LS - Method for old-style (unmanaged) histogram booking. Not used.
  StatusCode sc = StatusCode::SUCCESS; 
  return sc;
}

////////////////////////////////////////////
StatusCode CaloCellVecMon::bookHistograms(){

//  ATH_MSG_INFO("CaloCellVecMon::bookHistograms() starts");
  StatusCode sc = StatusCode::SUCCESS; 

//  12/2013 LS For managed histograms, no longer need to check the interval 
  Interval_t theinterval = run;
//  bool isNewTimeGran = checkTimeGran( isNewEventsBlock, isNewLumiBlock, isNewRun, theinterval);

//  if( isNewTimeGran ) {    
 

//    if(!m_firstcallofBook && !isNewRun){
////     ATH_MSG_INFO("before call deleteHistograms() in bookHistograms");
//     sc = deleteHistograms(); 
////     ATH_MSG_INFO("after call deleteHistograms() in bookHistograms");
//     if(sc.isFailure()) {
//       ATH_MSG_ERROR("fail to deleteHistograms before booking new ones ");
//       return sc;
//     }
//    }
    m_firstcallofBook = false; 

    initHists();
    
    initParam();    
    
    // Decide on  MMTB (global) trigger awareness 
    std::string TheTrigger;
    if (m_triggerChainProp == "")  TheTrigger="NoTrigSel";
    else TheTrigger = m_triggerChainProp;
    
    bookMonGroups(TheTrigger,theinterval);
 
    bookTileHists(); 

    bookCaloLayers(); 

    bookLarMultThreHists();

    if(m_sporadic_switch) bookSporHists();

    bookLarNonThreHists();  

    bookSummHists();
   

    //processTileHists(); // khadeejah add
    
    setBookSwitch();  //Khadeejah add

    
    //if(m_maskKnownBadChannels || m_maskNoCondChannels) m_procimask = m_doInverseMasking;
    //else m_procimask = false; 

    // Hack by Benjamin Trocme, as the m_procimask was never defined and led to spurious TH2D declaration...
    // At some point, all the code relate to inverse masking should be removed (totally useless) but it is not
    // so straightforward...
    m_procimask = false;
    m_doInverseMasking = false;

    if ( m_maskKnownBadChannels || m_maskNoCondChannels ){
       if(!m_doMaskingOnline) {
         //Book inverse masked histograms that would have been booked and filled in fillHistograms if
         // doMaskingOnline was on and which can be constructed here.
         if(m_doInverseMasking) {
           if(m_bookProcHists) bookImaskHists();
         }
         maskBadChanHists();
       }
    }

    if(m_bookProcHists) bookProcHists();  //Khadeejah 

   // processHists(); // Khadeejah 

   // if(m_procimask) processMaskHists();


    deleteBookSwitch();

    deleteCaloLayers(); // Khadeejah comment out
    
    m_bookProcHists = m_ifSaveAndDeleteHists;  // Khadeejah add
//  }

//  ATH_MSG_INFO("CaloCellVecMon::bookHistograms() is done");
  return sc;
}

bool CaloCellVecMon::checkTimeGran(bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun, Interval_t& theinterval){
   
  // Method to check if this is a new time period, as specified by m_timeGran. Useful for histogram booking.
  // 12/2013 LS - No longer need for managed histograms.
  //  In ManagedMonitorToolBase, we have:
  //  enum Interval_t { file = 0, eventsBlock, lumiBlock,
  //                   lowStat, medStat, higStat,
  //                   run, fill, all };
  
  bool isNewTimeGran=isNewRun; 
  theinterval=run;
  // but if the choice is different
  if (m_timeGran.compare("run") !=0) {  
    if (m_timeGran.compare("eventsblock") ==0) { 
      isNewTimeGran=isNewEventsBlock;
      theinterval= eventsBlock; 
    }
    else if (m_timeGran.compare("lumiblock") ==0) { 
      isNewTimeGran=isNewLumiBlock;
      theinterval=lumiBlock;
    }
    //... check if it is low statistics interval
    else if (m_timeGran.compare("lowStat") ==0){
      isNewTimeGran=newLowStatInterval;
      theinterval=lowStat; 
    }
    //... or medium statistics
    else if (m_timeGran.compare("medStat") ==0) {  
      isNewTimeGran=newMedStatInterval;
      theinterval=medStat;
    }
    else if (m_timeGran.compare("higStat") ==0){  
      isNewTimeGran=newHigStatInterval;
      theinterval=higStat;
    }
    else if (m_timeGran.compare("fill") ==0){  
      isNewTimeGran=newLowStatInterval;
      theinterval=fill;
    }
    else if (m_timeGran.compare("all") ==0){ 
      isNewTimeGran=newLowStatInterval;
      theinterval=all;
    }
    // if it is not global, but it is something else stick to medium stat
    else  { 
      isNewTimeGran=newLowStatInterval;
      theinterval=medStat;
    }
  }

  return isNewTimeGran;
}

void CaloCellVecMon::initParam(){
 
//   ATH_MSG_INFO("in initParam()");
// resetEventCounter
   m_eventsCounter = 0;
    if( m_useTrigger ) {
      for( int ilyrns = EMBPNS; ilyrns < MAXLYRNS; ilyrns++) {
        const int nThr = m_nThresholds[ilyrns];
        for(int ti=0; ti<nThr; ti++) {
          m_nEventsPassingTrigger[ilyrns][ti] = 0;
        }
      }
    }
    for( int ilyrns = EMBPNS; ilyrns < MAXLYRNS; ilyrns++) {
      const int nThr = m_nThresholds[ilyrns];
      for(int ti=0; ti<nThr; ti++) {
        m_nEventsPassingBeamBG[ilyrns][ti] = 0;
      }
    }


    m_counter_sporadic_protc=0;
    m_nbh_embc=0;
    m_nbh_emba=0;
    m_nbh_emecc=0;
    m_nbh_emeca=0;
    m_nbh_hecc=0;
    m_nbh_heca=0;
    m_nbh_fcalc=0;
    m_nbh_fcala=0;

    m_badCell_flag = true;
    m_badCell_flag_embc = true;
    m_badCell_flag_emba = true;
    m_badCell_flag_emecc = true;
    m_badCell_flag_emeca = true;
    m_badCell_flag_hecc = true;
    m_badCell_flag_heca = true;
    m_badCell_flag_fcalc = true;
    m_badCell_flag_fcala = true;


    m_b_Ncells.reserve(3);
    m_b_Ncells[0]=250;
    m_b_Ncells[1]=0;
    m_b_Ncells[2]=500;
    
    m_b_CellsE.reserve(3);
    m_b_CellsE[0]=50;
    m_b_CellsE[1]=0.;
    m_b_CellsE[2]=20.;
    
    m_b_CellsEta.reserve(3);
    m_b_CellsEta[0]=100;
    m_b_CellsEta[1]=-5.0;
    m_b_CellsEta[2]=5.0;

    m_b_CellsEtaTile.reserve(3);
    m_b_CellsEtaTile[0]=17;
    m_b_CellsEtaTile[1]=-1.7;
    m_b_CellsEtaTile[2]=1.7;
 
    m_b_CellsPhi.reserve(3);
    m_b_CellsPhi[0]=64;
    m_b_CellsPhi[1]=-M_PI;
    m_b_CellsPhi[2]=M_PI;
    
    
    m_b_CellsEtaPhi.reserve(6);
    m_b_CellsEtaPhi[0]=17;
    m_b_CellsEtaPhi[1]=-1.7;
    m_b_CellsEtaPhi[2]=1.7;
    m_b_CellsEtaPhi[3]=64;
    m_b_CellsEtaPhi[4]=-M_PI;
    m_b_CellsEtaPhi[5]=M_PI;
    
    
    m_b_CellsModule.reserve(3);
    m_b_CellsModule[0]=70;
    m_b_CellsModule[1]=0.0;
    m_b_CellsModule[2]=70;
    
    m_b_CellsTower.reserve(3);
    m_b_CellsTower[0]=18;
    m_b_CellsTower[1]=0.0;
    m_b_CellsTower[2]=18;

//    ATH_MSG_INFO("end of initParam()");
}

void CaloCellVecMon::bookMonGroups(const std::string& TheTrigger, Interval_t theinterval) {

//  ATH_MSG_INFO("in bookMonGroups()");

  std::string TheLArCellDir("/CaloMonitoring/LArCellMon_");
  
  m_summaryGroup = new MonGroup( this,TheLArCellDir+TheTrigger+"/Summary", theinterval );

  m_tempMonGroup = new MonGroup( this,TheLArCellDir+TheTrigger+"/Temp", theinterval );
   
  m_shifterMonGroup[KnownBadChannels] = new MonGroup( this,TheLArCellDir+TheTrigger+"/KnownBadChannels", theinterval, ATTRIB_MANAGED,"","weightedAverage");
  
  m_shifterMonGroup[DatabaseNoise] = new MonGroup( this,TheLArCellDir+TheTrigger+"/DatabaseNoise", theinterval, ATTRIB_MANAGED,"","weightedAverage"); 
  
  if( m_useWeightedEffMerging ) {
    m_shifterMonGroup[PercentOccupancy_2d] = new MonGroup( this,TheLArCellDir+TheTrigger+"/2d_Occupancy", theinterval, ATTRIB_MANAGED,"","weightedEff");
    
    m_shifterMonGroup[PercentOccupancy_1d] = new MonGroup( this,TheLArCellDir+TheTrigger+"/1d_Occupancy", theinterval, ATTRIB_MANAGED,"","weightedEff");
    
  }
  else {
    m_shifterMonGroup[PercentOccupancy_2d] = new MonGroup( this,TheLArCellDir+TheTrigger+"/2d_Occupancy", theinterval, ATTRIB_MANAGED,"","weightedAverage");
    
    m_shifterMonGroup[PercentOccupancy_1d] = new MonGroup( this,TheLArCellDir+TheTrigger+"/1d_Occupancy", theinterval, ATTRIB_MANAGED,"","weightedAverage");
  }
  
  m_shifterMonGroup[Occupancy_2d] = new MonGroup( this,TheLArCellDir+TheTrigger+"/2d_Occupancy", theinterval ); 
  
  m_shifterMonGroup[Occupancy_1d] = new MonGroup( this,TheLArCellDir+TheTrigger+"/1d_Occupancy", theinterval );
  
  m_shifterMonGroup[TotalEnergy] = new MonGroup( this,TheLArCellDir+TheTrigger+"/2d_TotalEnergy", theinterval ); 
  
  m_shifterMonGroup[AvgEnergy] = new MonGroup( this,TheLArCellDir+TheTrigger+"/2d_AvgEnergy", theinterval, ATTRIB_MANAGED,"","weightedAverage"); 
  
  m_shifterMonGroup[AvgEnergyNoTh] = new MonGroup( this,TheLArCellDir+TheTrigger+"/2d_AvgEnergy", theinterval, ATTRIB_MANAGED,"","weightedEff"); 
  
  m_shifterMonGroup[Noise] = new MonGroup( this,TheLArCellDir+TheTrigger+"/2d_Noise", theinterval, ATTRIB_MANAGED,"","mergeRMS"); 
  
  m_shifterMonGroup[NoiseVsDB] = new MonGroup( this,TheLArCellDir+TheTrigger+"/2d_Noise", theinterval, ATTRIB_MANAGED,"","RMSpercentDeviation"); 
  
  m_shifterMonGroup[Energy] = new MonGroup( this,TheLArCellDir+TheTrigger+"/Energy", theinterval );
  
  m_shifterMonGroup[EnergyVsTime] = new MonGroup( this,TheLArCellDir+TheTrigger+"/EnergyVsTime", theinterval );
  
  m_shifterMonGroup[AvgTime] = new MonGroup( this,TheLArCellDir+TheTrigger+"/2d_AvgTime", theinterval, ATTRIB_MANAGED,"","weightedAverage" );
  
  m_shifterMonGroup[AvgQuality] = new MonGroup( this,TheLArCellDir+TheTrigger+"/2d_AvgQuality", theinterval, ATTRIB_MANAGED,"","weightedAverage" );
  
  m_shifterMonGroup[PoorTimeFraction] = new MonGroup( this,TheLArCellDir+TheTrigger+"/2d_FractionOutOfTime", theinterval, ATTRIB_MANAGED,"","weightedAverage" );
  
  m_shifterMonGroup[PoorQualityFraction] = new MonGroup( this,TheLArCellDir+TheTrigger+"/2d_PoorQualityFraction", theinterval, ATTRIB_MANAGED,"","weightedAverage" );
  
  m_shifterMonGroup[CosmicsROI] = new MonGroup( this,TheLArCellDir+TheTrigger+"/CosmicsROI", theinterval );

  if(m_sporadic_switch){
    char sporadic_dir[100];
    float sporadic1 = m_threshold_em_S0S1*1e-3;
    float sporadic2 = m_threshold_HECFCALEMS2S3*1e-3;
    sprintf(sporadic_dir,"/CaloMonitoring/LArCellMon_NoTrigSel/Sporadic_%.0f_%.0fGeV",sporadic1,sporadic2);
    std::string BADCellDir(sporadic_dir);
    m_badCell = new MonGroup(this,BADCellDir,  theinterval );
    m_badCell_EMBC = new MonGroup(this,BADCellDir+"/EMBC",  theinterval );
    m_badCell_EMBA = new MonGroup(this,BADCellDir+"/EMBA",  theinterval );
    m_badCell_EMECC = new MonGroup(this,BADCellDir+"/EMECC",  theinterval );
    m_badCell_EMECA = new MonGroup(this,BADCellDir+"/EMECA",  theinterval );
    m_badCell_HECC = new MonGroup(this,BADCellDir+"/HECC",  theinterval );
    m_badCell_HECA = new MonGroup(this,BADCellDir+"/HECA",  theinterval );
    m_badCell_FCALC = new MonGroup(this,BADCellDir+"/FCALC",  theinterval );
    m_badCell_FCALA = new MonGroup(this,BADCellDir+"/FCALA",  theinterval );
  }

  std::string TheTileCellDir("/CaloMonitoring/TileCellMon_");
  m_tile_cells_shift = new MonGroup( this, TheTileCellDir+TheTrigger+"/General", theinterval);    
  
//  ATH_MSG_INFO("end of bookMonGroups()");
}

void CaloCellVecMon::bookTileHists(){
//    ATH_MSG_INFO("in bookTileHists()");

    m_h_Ncells    = new TH1F("CellsXN","Number of tile cells",(int)m_b_Ncells[0],m_b_Ncells[1], m_b_Ncells[2]);
    m_h_Ncells->GetXaxis()->SetTitle("Number" );
    m_h_Ncells->GetYaxis()->SetTitle("Events");
    m_tile_cells_shift->regHist(  m_h_Ncells  ).ignore();

    m_h_CellsE    = new TH1F("CellsXE","Energy of tile cells",(int)m_b_CellsE[0],m_b_CellsE[1], m_b_CellsE[2]);
    m_h_CellsE->GetXaxis()->SetTitle("Cell Energy [GeV]");
    m_h_CellsE->GetYaxis()->SetTitle("Events");
    m_tile_cells_shift->regHist(  m_h_CellsE  ).ignore();

    m_h_CellsNoiseE    = new TH1F("CellsNoiseTile","Energy/Noise(DB) of TileCal",200, -10., 10.);
    m_h_CellsNoiseE->GetXaxis()->SetTitle("Cell Energy / sigma (DB)");
    m_h_CellsNoiseE->GetYaxis()->SetTitle("Events");
    m_tile_cells_shift->regHist(  m_h_CellsNoiseE  ).ignore();
 
    m_h_CellsEta    = new TProfile("CellsXEta","Tile cell energy vs Eta",(int)m_b_CellsEtaTile[0],m_b_CellsEtaTile[1], m_b_CellsEtaTile[2]);
    m_h_CellsEta->GetXaxis()->SetTitle("Eta");
    m_h_CellsEta->GetYaxis()->SetTitle("Mean energy [GeV]");
    m_tile_cells_shift->regHist(  m_h_CellsEta  ).ignore();


    m_h_CellsEtaPhi    = new TH2F("CellsXEtaVSPhi","Nr TileCal cells above threshold",(int)m_b_CellsEtaPhi[0],m_b_CellsEtaPhi[1], m_b_CellsEtaPhi[2],
                                (int)m_b_CellsEtaPhi[3],m_b_CellsEtaPhi[4], m_b_CellsEtaPhi[5]);
    m_h_CellsEtaPhi->GetXaxis()->SetTitle("Eta");
    m_h_CellsEtaPhi->GetYaxis()->SetTitle("Phi [rad]");
    m_h_CellsEtaPhi->GetXaxis()->SetNdivisions(530); 
    m_h_CellsEtaPhi->GetYaxis()->SetNdivisions(530);
    m_tile_cells_shift->regHist(  m_h_CellsEtaPhi   ).ignore();


    m_h_CellsPhi    = new TProfile("CellsXPhi","Tile cell energy vs Phi",(int)m_b_CellsPhi[0],m_b_CellsPhi[1], m_b_CellsPhi[2]);
    m_h_CellsPhi->GetXaxis()->SetTitle("Phi");
    m_h_CellsPhi->GetYaxis()->SetTitle("Mean energy [GeV]");
    m_tile_cells_shift->regHist(  m_h_CellsPhi   ).ignore();

    m_h_CellsModuleS1    = new TProfile("CellsXModuleS1","Sampling Nr 1",(int)m_b_CellsModule[0],m_b_CellsModule[1], m_b_CellsModule[2]);
    m_h_CellsModuleS1->GetXaxis()->SetTitle("Module");
    m_h_CellsModuleS1->GetYaxis()->SetTitle("Mean cell energy [GeV]");
    m_tile_cells_shift->regHist(  m_h_CellsModuleS1   ).ignore();

    m_h_CellsModuleS2    = new TProfile("CellsXModuleS2","Sampling Nr 2",(int)m_b_CellsModule[0],m_b_CellsModule[1], m_b_CellsModule[2]);
    m_h_CellsModuleS2->GetXaxis()->SetTitle("Module");
    m_h_CellsModuleS2->GetYaxis()->SetTitle("Mean cell energy [GeV]");
    m_tile_cells_shift->regHist(  m_h_CellsModuleS2   ).ignore();

    m_h_CellsModuleS3    = new TProfile("CellsXModuleS3","Sampling Nr 3",(int)m_b_CellsModule[0],m_b_CellsModule[1], m_b_CellsModule[2]);
    m_h_CellsModuleS3->GetXaxis()->SetTitle("Module");
    m_h_CellsModuleS3->GetYaxis()->SetTitle("Mean cell energy [GeV]");
    m_tile_cells_shift->regHist(  m_h_CellsModuleS3   ).ignore();


    m_h_CellsModuleS4    = new TProfile("CellsXModuleS4","Sampling Nr 4",(int)m_b_CellsModule[0],m_b_CellsModule[1], m_b_CellsModule[2]);
    m_h_CellsModuleS4->GetXaxis()->SetTitle("Module");
    m_h_CellsModuleS4->GetYaxis()->SetTitle("Mean cell energy [GeV]");
    m_tile_cells_shift->regHist(  m_h_CellsModuleS4   ).ignore();

    m_h_CellsTower    = new TProfile("CellsXTower","Towers",(int)m_b_CellsTower[0],m_b_CellsTower[1], m_b_CellsTower[2]);
    m_h_CellsTower->GetXaxis()->SetTitle("Towers");
    m_h_CellsTower->GetYaxis()->SetTitle("Mean cell energy [GeV]");
    m_tile_cells_shift->regHist(  m_h_CellsTower   ).ignore();


    // tilecal
    m_h_CellsNoiseEta    = new TProfile("CellsNoiseXEta","Tile cell sigma(DB) vs Eta",(int)m_b_CellsEtaTile[0],m_b_CellsEtaTile[1], m_b_CellsEtaTile[2]);
    m_h_CellsNoiseEta->GetXaxis()->SetTitle("Eta");
    m_h_CellsNoiseEta->GetYaxis()->SetTitle("Mean cell noise (DB) [MeV]");
    m_tile_cells_shift->regHist(  m_h_CellsNoiseEta  ).ignore();

    m_h_CellsNoisePhi    = new TProfile("CellsNoiseXPhi","Tile cell sigma(DB) vs Phi",(int)m_b_CellsPhi[0],m_b_CellsPhi[1], m_b_CellsPhi[2]);
    m_h_CellsNoisePhi->GetXaxis()->SetTitle("Phi");
    m_h_CellsNoisePhi->GetYaxis()->SetTitle("Mean cell noise (DB) [MeV]");
    m_tile_cells_shift->regHist(  m_h_CellsNoisePhi   ).ignore();

    m_h_CellsNoiseEtaPhi    = new TH2F("CellsNoiseXEtaVSPhi","Nr of TileCells with E.gt.4 sigma(DB)",(int)m_b_CellsEtaPhi[0],m_b_CellsEtaPhi[1], m_b_CellsEtaPhi[2],
                                     (int)m_b_CellsEtaPhi[3],m_b_CellsEtaPhi[4], m_b_CellsEtaPhi[5]);
    m_h_CellsNoiseEtaPhi->GetXaxis()->SetTitle("Eta");
    m_h_CellsNoiseEtaPhi->GetYaxis()->SetTitle("Phi [rad]");
    m_h_CellsNoiseEtaPhi->GetXaxis()->SetNdivisions(530);
    m_h_CellsNoiseEtaPhi->GetYaxis()->SetNdivisions(530);
    m_tile_cells_shift->regHist(  m_h_CellsNoiseEtaPhi   ).ignore();


    m_h_CellsNoiseEtaPhiA    = new TH2F("CellsNoiseXEtaVSPhiAcells","Nr of Tile cells-A with E.gt.4 sigma(DB)",(int)(2*m_b_CellsEtaPhi[0]),m_b_CellsEtaPhi[1], m_b_CellsEtaPhi[2],
                                     (int)m_b_CellsEtaPhi[3],m_b_CellsEtaPhi[4], m_b_CellsEtaPhi[5]);
    m_h_CellsNoiseEtaPhiA->GetXaxis()->SetTitle("Eta");
    m_h_CellsNoiseEtaPhiA->GetYaxis()->SetTitle("Phi [rad]");
    m_h_CellsNoiseEtaPhiA->GetXaxis()->SetNdivisions(530);
    m_h_CellsNoiseEtaPhiA->GetYaxis()->SetNdivisions(530);
    m_tile_cells_shift->regHist(  m_h_CellsNoiseEtaPhiA   ).ignore();


    m_h_CellsNoiseEtaPhiBC    = new TH2F("CellsNoiseXEtaVSPhiBCcells","Nr of TileCells-BC with E.gt.4 sigma(DB)",(int)(2*m_b_CellsEtaPhi[0]),m_b_CellsEtaPhi[1], m_b_CellsEtaPhi[2],
                                     (int)m_b_CellsEtaPhi[3],m_b_CellsEtaPhi[4], m_b_CellsEtaPhi[5]);
    m_h_CellsNoiseEtaPhiBC->GetXaxis()->SetTitle("Eta");
    m_h_CellsNoiseEtaPhiBC->GetYaxis()->SetTitle("Phi [rad]");
    m_h_CellsNoiseEtaPhiBC->GetXaxis()->SetNdivisions(530);
    m_h_CellsNoiseEtaPhiBC->GetYaxis()->SetNdivisions(530);
    m_tile_cells_shift->regHist(  m_h_CellsNoiseEtaPhiBC   ).ignore();


    m_h_CellsNoiseEtaPhiD    = new TH2F("CellsNoiseXEtaVSPhiDcells","Nr of TileCells-D with E.gt.4 sigma(DB)",(int)m_b_CellsEtaPhi[0],m_b_CellsEtaPhi[1], m_b_CellsEtaPhi[2],
                                     (int)m_b_CellsEtaPhi[3],m_b_CellsEtaPhi[4], m_b_CellsEtaPhi[5]);
    m_h_CellsNoiseEtaPhiD->GetXaxis()->SetTitle("Eta");
    m_h_CellsNoiseEtaPhiD->GetYaxis()->SetTitle("Phi [rad]");
    m_h_CellsNoiseEtaPhiD->GetXaxis()->SetNdivisions(530);
    m_h_CellsNoiseEtaPhiD->GetYaxis()->SetNdivisions(530);
    m_tile_cells_shift->regHist(  m_h_CellsNoiseEtaPhiD   ).ignore();

    // cell E vs Eta and Phi. Errors show RMS (not error on the mean) 
    m_h_CellsRMSEta    = new TProfile("CellsRMSXEta","Tile cell energy with RMS errors for Eta",(int)m_b_CellsEtaTile[0],m_b_CellsEtaTile[1], m_b_CellsEtaTile[2],"s");
    m_h_CellsRMSEta->GetXaxis()->SetTitle("Eta");
    m_h_CellsRMSEta->GetYaxis()->SetTitle("Mean cell energy+RMS [GeV]");
    m_tile_cells_shift->regHist(  m_h_CellsRMSEta  ).ignore();

    m_h_CellsRMSPhi    = new TProfile("CellsRMSXPhi","Tile cell energy with RMS errors vs Phi",(int)m_b_CellsPhi[0],m_b_CellsPhi[1], m_b_CellsPhi[2],"s");
    m_h_CellsRMSPhi->GetXaxis()->SetTitle("Phi");
    m_h_CellsRMSPhi->GetYaxis()->SetTitle("Mean cell energy+RMS [GeV]");
    m_tile_cells_shift->regHist(  m_h_CellsRMSPhi   ).ignore();


    char name[256];
    char title[256];
    sprintf(name,"%s_%s",(char*)m_h_CellsRMSEta->GetName(),"divNoiseDB");
    sprintf(title,"%s - %s",(char*)m_h_CellsRMSEta->GetTitle(),"divNoiseDB");
    m_h_CellsRMSdivDBnoiseEta = new TH1F(name,title,(int)m_b_CellsEtaTile[0],m_b_CellsEtaTile[1], m_b_CellsEtaTile[2]);
    m_h_CellsRMSdivDBnoiseEta->SetNameTitle(name,title);
    m_h_CellsRMSdivDBnoiseEta ->GetXaxis()->SetTitle("Eta");
    m_h_CellsRMSdivDBnoiseEta ->GetYaxis()->SetTitle("Energy cell RMS / Cell noise (DB)");
    m_tile_cells_shift->regHist(  m_h_CellsRMSdivDBnoiseEta  ).ignore();

    sprintf(name,"%s_%s",(char*)m_h_CellsRMSPhi->GetName(),"divNoiseDB");
    sprintf(title,"%s - %s",(char*)m_h_CellsRMSPhi->GetTitle(),"divNoiseDB");
    m_h_CellsRMSdivDBnoisePhi = new TH1F(name,title,(int)m_b_CellsPhi[0],m_b_CellsPhi[1], m_b_CellsPhi[2]);
    m_h_CellsRMSdivDBnoisePhi->SetNameTitle(name,title);
    m_h_CellsRMSdivDBnoisePhi ->GetXaxis()->SetTitle("Phi");
    m_h_CellsRMSdivDBnoisePhi ->GetYaxis()->SetTitle("Energy cell RMS / Cell noise (DB)");
    m_tile_cells_shift->regHist(  m_h_CellsRMSdivDBnoisePhi  ).ignore();

    m_h_CellsNoiseHash    = new TH1F("CellsXNoiseXHash","Number of 4 sigma seeds per hash",5184,-0.5, 5183.5);
    m_h_CellsNoiseHash->GetXaxis()->SetTitle("HashId" );
    m_h_CellsNoiseHash->GetYaxis()->SetTitle("Events");
    m_tile_cells_shift->regHist(  m_h_CellsNoiseHash  ).ignore();

//    ATH_MSG_INFO("end of bookTileHists()");
}

void CaloCellVecMon::bookCaloLayers() {
//  ATH_MSG_INFO("in bookCaloLayers()");

  //EMB Pre-sampler 
  m_layer[EMBPA] = new CaloLayer("EMBPA",CaloLayer::EMB,0,CaloLayer::A);
  const int embpaNEtaReg = 1;
  double embpaEtaBreakPts[embpaNEtaReg+1]={0.,1.52};
  int embpaEtaBins[embpaNEtaReg]={61};
  m_layer[EMBPA]->doEtaBinning(embpaNEtaReg,embpaEtaBreakPts,embpaEtaBins);
  m_layer[EMBPA]->doPhiBinning(64);
  
  m_layer[EMBPC] = new CaloLayer("EMBPC",CaloLayer::EMB,0,CaloLayer::C);
  m_layer[EMBPC]->doSideCBinning(m_layer[EMBPA]);
  
  
  // EMB Layer 1 
  m_layer[EMB1A] = new CaloLayer("EMB1A",CaloLayer::EMB,1,CaloLayer::A);
  const int emb1aNEtaReg = 2;
  double emb1aEtaBreakPts[emb1aNEtaReg+1]={0.,1.4,1.475};
  int emb1aEtaBins[emb1aNEtaReg]={448,12};
  m_layer[EMB1A]->doEtaBinning(emb1aNEtaReg,emb1aEtaBreakPts,emb1aEtaBins);
  m_layer[EMB1A]->doPhiBinning(64);
  
  m_layer[EMB1C] = new CaloLayer("EMB1C",CaloLayer::EMB,1,CaloLayer::C);
  m_layer[EMB1C]->doSideCBinning(m_layer[EMB1A]);    
  
  
  // EMB Layer 2
  m_layer[EMB2A] = new CaloLayer("EMB2A",CaloLayer::EMB,2,CaloLayer::A);
  const int emb2aNEtaReg = 2;
  double emb2aEtaBreakPts[emb2aNEtaReg+1]={0.,1.4,1.475};
  int emb2aEtaBins[emb2aNEtaReg]={56,1};
  m_layer[EMB2A]->doEtaBinning(emb2aNEtaReg,emb2aEtaBreakPts,emb2aEtaBins);
  m_layer[EMB2A]->doPhiBinning(256);
  
  m_layer[EMB2C] = new CaloLayer("EMB2C",CaloLayer::EMB,2,CaloLayer::C);
  m_layer[EMB2C]->doSideCBinning(m_layer[EMB2A]);
  
  
  // EMB Layer 3
  m_layer[EMB3A] = new CaloLayer("EMB3A",CaloLayer::EMB,3,CaloLayer::A);
  const int emb3aNEtaReg = 1;
  double emb3aEtaBreakPts[emb3aNEtaReg+1]={0.00,1.35};
  int emb3aEtaBins[emb3aNEtaReg]={27};
  m_layer[EMB3A]->doEtaBinning(emb3aNEtaReg,emb3aEtaBreakPts,emb3aEtaBins);
  m_layer[EMB3A]->doPhiBinning(256);
  
  m_layer[EMB3C] = new CaloLayer("EMB3C",CaloLayer::EMB,3,CaloLayer::C);
  m_layer[EMB3C]->doSideCBinning(m_layer[EMB3A]);
  
  
  // HEC Layer 0 
  m_layer[HEC0A] = new CaloLayer("HEC0A",CaloLayer::HEC,0,CaloLayer::A);
  const int hec0aNEtaReg = 2;
  double hec0aEtaBreakPts[hec0aNEtaReg+1]={1.5,2.5,3.3};
  int hec0aEtaBins[hec0aNEtaReg]={10,4};
  m_layer[HEC0A]->doEtaBinning(hec0aNEtaReg,hec0aEtaBreakPts,hec0aEtaBins);
  m_layer[HEC0A]->doPhiBinning(64);
  
  m_layer[HEC0C] = new CaloLayer("HEC0C",CaloLayer::HEC,0,CaloLayer::C);
  m_layer[HEC0C]->doSideCBinning(m_layer[HEC0A]);
  
  
  // HEC Layer 1 
  m_layer[HEC1A] = new CaloLayer("HEC1A",CaloLayer::HEC,1,CaloLayer::A);
  const int hec1aNEtaReg = 2;
  double hec1aEtaBreakPts[hec1aNEtaReg+1]={1.5,2.5,3.1};
  int hec1aEtaBins[hec1aNEtaReg]={10,3};
  m_layer[HEC1A]->doEtaBinning(hec1aNEtaReg,hec1aEtaBreakPts,hec1aEtaBins);
  m_layer[HEC1A]->doPhiBinning(64);
  
  m_layer[HEC1C] = new CaloLayer("HEC1C",CaloLayer::HEC,1,CaloLayer::C);
  m_layer[HEC1C]->doSideCBinning(m_layer[HEC1A]);
  
  
  // HEC Layer 2 
  m_layer[HEC2A] = new CaloLayer("HEC2A",CaloLayer::HEC,2,CaloLayer::A);
  const int hec2aNEtaReg = 2;
  double hec2aEtaBreakPts[hec2aNEtaReg+1]={1.6,2.5,3.1};
  int hec2aEtaBins[hec2aNEtaReg]={9,3};
  m_layer[HEC2A]->doEtaBinning(hec2aNEtaReg,hec2aEtaBreakPts,hec2aEtaBins);
  m_layer[HEC2A]->doPhiBinning(64);
  
  m_layer[HEC2C] = new CaloLayer("HEC2C",CaloLayer::HEC,2,CaloLayer::C);
  m_layer[HEC2C]->doSideCBinning(m_layer[HEC2A]);
  
  
  // HEC Layer 3 
  m_layer[HEC3A] = new CaloLayer("HEC3A",CaloLayer::HEC,3,CaloLayer::A);
  const int hec3aNEtaReg = 2;
  double hec3aEtaBreakPts[hec3aNEtaReg+1]={1.7,2.5,3.3};
  int hec3aEtaBins[hec3aNEtaReg]={8,4};
  m_layer[HEC3A]->doEtaBinning(hec3aNEtaReg,hec3aEtaBreakPts,hec3aEtaBins);
  m_layer[HEC3A]->doPhiBinning(64);
  
  m_layer[HEC3C] = new CaloLayer("HEC3C",CaloLayer::HEC,3,CaloLayer::C);
  m_layer[HEC3C]->doSideCBinning(m_layer[HEC3A]);
  
  
  // EMEC Pre-sampler (Layer 0) 
  m_layer[EMECPA] = new CaloLayer("EMECPA",CaloLayer::EMEC,0,CaloLayer::A);
  const int emecpaNEtaReg = 1;
  double emecpaEtaBreakPts[emecpaNEtaReg+1]={1.5,1.8};
  int emecpaEtaBins[emecpaNEtaReg]={12};
  m_layer[EMECPA]->doEtaBinning(emecpaNEtaReg,emecpaEtaBreakPts,emecpaEtaBins);
  m_layer[EMECPA]->doPhiBinning(64);
  
  m_layer[EMECPC] = new CaloLayer("EMECPC",CaloLayer::EMEC,0,CaloLayer::C);
  m_layer[EMECPC]->doSideCBinning(m_layer[EMECPA]);
  
  
  // EMEC Layer 1 
  m_layer[EMEC1A] = new CaloLayer("EMEC1A",CaloLayer::EMEC,1,CaloLayer::A);
  const int emec1aNEtaReg = 7;
  double emec1aEtaBreakPts[emec1aNEtaReg+1]={1.375,1.425,1.5,1.8,2.0,2.4,2.5,3.2};
  int emec1aEtaBins[emec1aNEtaReg]={1,3,96,48,64,4,7};
  m_layer[EMEC1A]->doEtaBinning(emec1aNEtaReg,emec1aEtaBreakPts,emec1aEtaBins);
  m_layer[EMEC1A]->doPhiBinning(64);
  
  m_layer[EMEC1C] = new CaloLayer("EMEC1C",CaloLayer::EMEC,1,CaloLayer::C);
  m_layer[EMEC1C]->doSideCBinning(m_layer[EMEC1A]);


  // EMEC Layer 2 
  m_layer[EMEC2A] = new CaloLayer("EMEC2A",CaloLayer::EMEC,2,CaloLayer::A);
  const int emec2aNEtaReg = 3;
  double emec2aEtaBreakPts[emec2aNEtaReg+1]={1.375,1.425,2.5,3.2};
  int emec2aEtaBins[emec2aNEtaReg]={1,43,7};
  m_layer[EMEC2A]->doEtaBinning(emec2aNEtaReg,emec2aEtaBreakPts,emec2aEtaBins);
  m_layer[EMEC2A]->doPhiBinning(256);
  
  m_layer[EMEC2C] = new CaloLayer("EMEC2C",CaloLayer::EMEC,2,CaloLayer::C);
  m_layer[EMEC2C]->doSideCBinning(m_layer[EMEC2A]);
  
  
  // EMEC Layer 3 
  m_layer[EMEC3A] = new CaloLayer("EMEC3A",CaloLayer::EMEC,3,CaloLayer::A);
  const int emec3aNEtaReg = 1;
  double emec3aEtaBreakPts[emec3aNEtaReg+1]={1.5,2.5};
  int emec3aEtaBins[emec3aNEtaReg]={20};
  m_layer[EMEC3A]->doEtaBinning(emec3aNEtaReg,emec3aEtaBreakPts,emec3aEtaBins);
  m_layer[EMEC3A]->doPhiBinning(256);
  
  m_layer[EMEC3C] = new CaloLayer("EMEC3C",CaloLayer::EMEC,3,CaloLayer::C);
  m_layer[EMEC3C]->doSideCBinning(m_layer[EMEC3A]);
  
  
  // FCAL Layer 1  (0th layer in the code) 
  m_layer[FCAL1A] = new CaloLayer("FCAL1A",CaloLayer::FCAL,0,CaloLayer::A);
  
  const int fcal1aNEtaReg = 4;
  double fcal1aEtaBreakPts[fcal1aNEtaReg+1]={3.08,3.2,3.8,4.6,4.82};
  int fcal1aEtaBins[fcal1aNEtaReg]={3,6,4,2};
  m_layer[FCAL1A]->doEtaBinning(fcal1aNEtaReg,fcal1aEtaBreakPts,fcal1aEtaBins);
  
  const int fcal1aNPhiReg = 95;
  double fcal1aPhiBreakPts[fcal1aNPhiReg+1]=
    {-M_PI,(-250.0/256)*M_PI,(-248.0/256)*M_PI,(-243.0/256)*M_PI,(-240.0/256)*M_PI,(-234.0/256)*M_PI,
     (-230.0/256)*M_PI,(-225.0/256)*M_PI,(-219.0/256)*M_PI,(-195.0/256)*M_PI,(-190.0/256)*M_PI,(-186.0/256)*M_PI,
     (-183.0/256)*M_PI,(-178.0/256)*M_PI,(-175.0/256)*M_PI,(-173.0/256)*M_PI,(-167.0/256)*M_PI,(-161.0/256)*M_PI,
     (-159.0/256)*M_PI,(-156.0/256)*M_PI,(-151.0/256)*M_PI,(-148.0/256)*M_PI,(-146.0/256)*M_PI,(-141.0/256)*M_PI,
     (-139.0/256)*M_PI,(-135.0/256)*M_PI,(-132.0/256)*M_PI,(-120.0/256)*M_PI,(-117.0/256)*M_PI,(-114.0/256)*M_PI,
     (-110.0/256)*M_PI,(-105.0/256)*M_PI,(-97.0/256)*M_PI,(-94.0/256)*M_PI,(-86.0/256)*M_PI,(-78.0/256)*M_PI,
     (-63.0/256)*M_PI,(-60.0/256)*M_PI,(-56.0/256)*M_PI,(-46.0/256)*M_PI,(-44.0/256)*M_PI,(-41.0/256)*M_PI,
     (-32.0/256)*M_PI,(-28.0/256)*M_PI,(-22.0/256)*M_PI,(-10.0/256)*M_PI,(-7.0/256)*M_PI,(-3.0/256)*M_PI,
     (6.0/256)*M_PI,(8.0/256)*M_PI,(13.0/256)*M_PI,(16.0/256)*M_PI,(22.0/256)*M_PI,(26.0/256)*M_PI,(31.0/256)*M_PI,
     (37.0/256)*M_PI,(61.0/256)*M_PI,(66.0/256)*M_PI,(70.0/256)*M_PI,(73.0/256)*M_PI,(78.0/256)*M_PI,(81.0/256)*M_PI,
     (83.0/256)*M_PI,(89.0/256)*M_PI,(95.0/256)*M_PI,(97.0/256)*M_PI,(100.0/256)*M_PI,(105.0/256)*M_PI,
     (108.0/256)*M_PI,(110.0/256)*M_PI,(115.0/256)*M_PI,(117.0/256)*M_PI,(121.0/256)*M_PI,(124.0/256)*M_PI,
     (136.0/256)*M_PI,(139.0/256)*M_PI,(142.0/256)*M_PI,(146.0/256)*M_PI,(151.0/256)*M_PI,(159.0/256)*M_PI,
     (162.0/256)*M_PI,(170.0/256)*M_PI,(178.0/256)*M_PI,(193.0/256)*M_PI,(196.0/256)*M_PI,(200.0/256)*M_PI,
     (210.0/256)*M_PI,(212.0/256)*M_PI,(215.0/256)*M_PI,(224.0/256)*M_PI,(228.0/256)*M_PI,(234.0/256)*M_PI,
     (246.0/256)*M_PI,(249.0/256)*M_PI,(253.0/256)*M_PI,M_PI};
  int fcal1aPhiBins[fcal1aNPhiReg+1] =
    {2,1,1,1,1,1,1,2,6,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,3,1,1,2,1,2,1,2,4,3,1,1,2,1,1,3,1,2,2,1,2,3,
     1,1,1,1,1,1,2,6,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,3,1,1,2,1,2,1,2,4,3,1,1,2,1,1,3,1,2,2,1,2,1};
  m_layer[FCAL1A]->doPhiBinning(fcal1aNPhiReg,fcal1aPhiBreakPts,fcal1aPhiBins);
  
  m_layer[FCAL1C] = new CaloLayer("FCAL1C",CaloLayer::FCAL,0,CaloLayer::C);
  m_layer[FCAL1C]->doSideCBinning(m_layer[FCAL1A]);
  
  
  // FCAL Layer 2  (1st layer in the code);
  m_layer[FCAL2A] = new CaloLayer("FCAL2A",CaloLayer::FCAL,1,CaloLayer::A);
  
  const int fcal2aNEtaReg = 15;
  double fcal2aEtaBreakPts[fcal2aNEtaReg+1]=
    {3.192,3.1987,3.206,3.22,3.26,3.34,3.37,3.39,3.52,3.57,3.835,4.1,4.365,4.5,4.65,4.81};
  int fcal2aEtaBins[fcal2aNEtaReg]={1,1,1,2,2,1,1,3,1,4,2,1,1,1,1};
  m_layer[FCAL2A]->doEtaBinning(fcal2aNEtaReg,fcal2aEtaBreakPts,fcal2aEtaBins);
  
  const int fcal2aNPhiReg = 32;
  double fcal2aPhiBreakPts[fcal2aNPhiReg+1]=
    {-M_PI,(-62.0/64)*M_PI,(-59.0/64)*M_PI,(-55.0/64)*M_PI,(-52.0/64)*M_PI,(-50.0/64)*M_PI,(-48.0/64)*M_PI,
     (-45.0/64)*M_PI,(-41.0/64)*M_PI,(-38.0/64)*M_PI,(-22.0/64)*M_PI,(-18.0/64)*M_PI,(-12.0/64)*M_PI,
     (-8.0/64)*M_PI,(-5.0/64)*M_PI,(-1.0/64)*M_PI,0,(2.0/64)*M_PI,(5.0/64)*M_PI,(9.0/64)*M_PI,(12.0/64)*M_PI,
     (14.0/64)*M_PI,(16.0/64)*M_PI,(19.0/64)*M_PI,(23.0/64)*M_PI,(26.0/64)*M_PI,(42.0/64)*M_PI,(46.0/64)*M_PI,
     (52.0/64)*M_PI,(56.0/64)*M_PI,(59.0/64)*M_PI,(63.0/64)*M_PI,M_PI};
  int fcal2aPhiBins[fcal2aNPhiReg+1] = 
    {1,1,1,1,1,1,1,1,1,8,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,8,1,3,1,1,1,1};
  m_layer[FCAL2A]->doPhiBinning(fcal2aNPhiReg,fcal2aPhiBreakPts,fcal2aPhiBins);
  
  m_layer[FCAL2C] = new CaloLayer("FCAL2C",CaloLayer::FCAL,1,CaloLayer::C);
  m_layer[FCAL2C]->doSideCBinning(m_layer[FCAL2A]);
  

  // FCAL Layer 3  (2nd layer in the code) 
  m_layer[FCAL3A] = new CaloLayer("FCAL3A",CaloLayer::FCAL,2,CaloLayer::A);
  
  const int fcal3aNEtaReg = 5;
  double fcal3aEtaBreakPts[fcal3aNEtaReg+1]={3.27,3.43,3.7,3.91,4.33,4.75};
  int fcal3aEtaBins[fcal3aNEtaReg]={2,2,1,1,2};
  m_layer[FCAL3A]->doEtaBinning(fcal3aNEtaReg,fcal3aEtaBreakPts,fcal3aEtaBins);
  
  const int fcal3aNPhiReg = 22;
  double fcal3aPhiBreakPts[fcal3aNPhiReg+1]=
    {-M_PI,(-40.0/64)*M_PI,(-39.0/64)*M_PI,(-35.0/64)*M_PI,(-29.0/64)*M_PI,(-24.0/64)*M_PI,(-22.0/64)*M_PI,
     (-16.0/64)*M_PI,(-11.0/64)*M_PI,(-9.0/64)*M_PI,(-6.0/64)*M_PI,0,(24.0/64)*M_PI,(25.0/64)*M_PI,
     (29.0/64)*M_PI,(35.0/64)*M_PI,(40.0/64)*M_PI,(42.0/64)*M_PI,(48.0/64)*M_PI,(53.0/64)*M_PI,(55.0/64)*M_PI,
     (58.0/64)*M_PI,M_PI};
  int fcal3aPhiBins[fcal3aNPhiReg+1] = {10,1,1,4,2,1,2,2,1,1,2,10,1,1,4,2,1,2,2,1,1,2};
  m_layer[FCAL3A]->doPhiBinning(fcal3aNPhiReg,fcal3aPhiBreakPts,fcal3aPhiBins);
  
  m_layer[FCAL3C] = new CaloLayer("FCAL3C",CaloLayer::FCAL,2,CaloLayer::C);
  m_layer[FCAL3C]->doSideCBinning(m_layer[FCAL3A]);

//  ATH_MSG_INFO("end of bookCaloLayers()");
}

void CaloCellVecMon::bookLarMultThreHists(){
//    ATH_MSG_INFO("in bookLarMultThreHists()");
    

    book2DEtaPhiHists(m_h_occupancy_etaphi,Occupancy_2d,"CellOccupancyVsEtaPhi_%s_%s",
                      "No. of events in (#eta,#phi) for %s - %s",m_fillEtaPhiOccupancy,
                      m_tempEtaPhiOccupancy);
    book1DEtaPhiHists(m_h_occupancy_eta,Occupancy_1d,"CellOccupancyVsEta_%s_%s",
                      "No. of events in #eta for %s - %s","e",m_fillEtaOccupancy,
                      m_doPercentageOccupancy);
    book1DEtaPhiHists(m_h_occupancy_phi,Occupancy_1d,"CellOccupancyVsPhi_%s_%s",
                      "No. of events in #phi for %s - %s","p",m_fillPhiOccupancy,
                      m_doPercentageOccupancy);

    book2DEtaPhiHists(m_h_energyProfile_etaphi,Temporary,"TempEnergyNoiseTProfile_%s_%s",
                      "Average Cell Energy vs (#eta,#phi) in %s - %s",m_doEtaPhiEnergyProfile);
                    
    
    book2DEtaPhiHists(m_h_totalEnergy_etaphi,TotalEnergy,"TotalEnergyVsEtaPhi_%s_%s",
                      "Total Cell Energy vs (#eta,#phi) in %s - %s",m_fillEtaPhiTotalEnergy,
                      m_tempEtaPhiTotalEnergy);

    book2DEtaPhiHists(m_h_poorQualityOccupancy_etaphi,Temporary,"TempPoorQualityOccupancy_%s_%s",
                      "Occupancy of Cells beyond quality threshold vs (#eta,#phi) in %s - %s",m_fillEtaPhiQualityOccupancy);
    book2DEtaPhiHists(m_h_totalQuality_etaphi,Temporary,"TempTotalQuality_%s_%s",
                      "Total quality factor of Cells vs (#eta,#phi) in %s - %s",m_fillEtaPhiTotalQuality);
    book2DEtaPhiHists(m_h_poorTimeOccupancy_etaphi,Temporary,"TempPoorTimeOccupancy_%s_%s",
                      "Occupancy of Cells beyond quality threshold vs (#eta,#phi) in %s - %s",m_fillEtaPhiTimeOccupancy);
    book2DEtaPhiHists(m_h_totalTime_etaphi,Temporary,"TempTotalTime_%s_%s",
                      "Total time for Cells vs (#eta,#phi) in %s - %s",m_fillEtaPhiTotalTime);

    if( (m_doKnownBadChannelsVsEtaPhi || ((!m_doMaskingOnline) && m_maskKnownBadChannels )) ) {
      book2DEtaPhiHists(m_h_badChannels_etaphi,KnownBadChannels,"KnownBadChannelsVsEtaPhi_%s",
                        "Map of Known Bad Channels in (#eta,#phi) for %s");
    }
    if(m_doDatabaseNoiseVsEtaPhi) {
      book2DEtaPhiHists(m_h_dbnoise_etaphi,DatabaseNoise,"DatabaseNoiseVsEtaPhi_%s","Map of Noise Values from the Database vs (#eta,#phi) for %s");
    }

    if(m_doNActiveCellsFirstEventVsEta) {
      book1DEtaPhiHists(m_h_nActiveCellsFirstEvent_eta,Temporary,"NCellsActiveVsEta_%s",
                        "No. of Active Cells in #eta for %s","e");
                       
    }
    if(m_doNActiveCellsFirstEventVsPhi){
      book1DEtaPhiHists(m_h_nActiveCellsFirstEvent_phi,Temporary,"NCellsActiveVsPhi_%s",
                        "No. of Active Cells in #phi for %s","p");
      
    }

    if(m_doInverseMasking && m_doMaskingOnline) {

      book2DEtaPhiHists(m_h_occupancy_etaphi_imask,Occupancy_2d,"CellOccupancyVsEtaPhi_%s_%s_inverseMasked",
                        "No. of events  in (#eta,#phi) in %s - %s Inverse Masked",m_fillEtaPhiOccupancy,
                        m_tempEtaPhiOccupancy);
      book1DEtaPhiHists(m_h_occupancy_eta_imask,Occupancy_1d,"CellOccupancyVsEta_%s_%s_inverseMasked",
                        "No. of Events in #eta for %s - %s Inverse Masked","e",m_fillEtaOccupancy,
                        m_doPercentageOccupancy);
      book1DEtaPhiHists(m_h_occupancy_phi_imask,Occupancy_1d,"CellOccupancyVsPhi_%s_%s_inverseMasked",
                        "No. of Events in #phi for %s - %s Inverse Masked","p",m_fillPhiOccupancy,
                        m_doPercentageOccupancy);
      book2DEtaPhiHists(m_h_energyProfile_etaphi_imask,Temporary,"TempEnergyNoiseTProfile_%s_%s_inverseMasked",
                        "Average Cell Energy vs (#eta,#phi) in %s - %s Inverse Masked",m_doEtaPhiEnergyProfile);
      book2DEtaPhiHists(m_h_totalEnergy_etaphi_imask,TotalEnergy,"TotalEnergyVsEtaPhi_%s_%s_inverseMasked",
                        "Total Cell Energy vs (#eta,#phi) in %s - %s Inverse Masked",m_fillEtaPhiTotalEnergy,
                        m_tempEtaPhiTotalEnergy);
      for(int ilyr=EMBPA; ilyr<MAXLAYER; ilyr++) {
        for(int ti = 0; ti<m_nThresholds[ilyr/2]; ti++) {
          if(m_doEtaPhiEnergyProfile[ilyr/2][ti]){
            m_h_energyProfile_etaphi_imask[ilyr][ti]->SetErrorOption("s");
          }
        }
      }
    }

    for(int ilyr = 0; ilyr < MAXLAYER; ilyr++) {
      int ilyrns = ilyr/2;
      int nThr = m_nThresholds[ilyrns];
      for(int ti = 0; ti < nThr; ti++) {
        if(m_maskEmptyBins[ilyrns][ti] && (m_h_missingCellMask[ilyr] == 0) ) {
          bookLayerEtaPhiHists2D((LayerEnum)ilyr,m_h_missingCellMask,Temporary,"missingCellMask_%s","Missing Cell Mask - %s");
        }
      }
    }
//    ATH_MSG_INFO("end of bookLarMultThreHists()");
}

void CaloCellVecMon::bookSporHists(){
//    ATH_MSG_INFO("in bookSporHists()");
    m_h_NoiseBurstLumiblock =  new TH1F("lumiblock_of_NB","LB affected by cells sporadically noisy with CSC veto", 2000,0.5,2000.5);
    m_badCell->regHist(m_h_NoiseBurstLumiblock).ignore();
    m_h_sporad= new TH1F("test","test with CSC veto",10,0,10);
    m_badCell->regHist(m_h_sporad).ignore();
    m_h_sporad_embc = new TH1F("Nsporadic_EMBC","Number of sporadic-cell plots with CSC veto",10,0,10);
    m_badCell_EMBC->regHist(m_h_sporad_embc).ignore();
    m_h_sporad_emba = new TH1F("Nsporadic_EMBA","Number of sporadic-cell plots with CSC veto",10,0,10);
    m_badCell_EMBA->regHist(m_h_sporad_emba).ignore();
    m_h_sporad_emecc = new TH1F("Nsporadic_EMECC","Number of sporadic-cell plots with CSC veto",10,0,10);
    m_badCell_EMECC->regHist(m_h_sporad_emecc).ignore();
    m_h_sporad_emeca = new TH1F("Nsporadic_EMECA","Number of sporadic-cell plots with CSC veto",10,0,10);
    m_badCell_EMECA->regHist(m_h_sporad_emeca).ignore();
    m_h_sporad_hecc = new TH1F("Nsporadic_HECC","Number of sporadic-cell plots with CSC veto",10,0,10);
    m_badCell_HECC->regHist(m_h_sporad_hecc).ignore();
    m_h_sporad_heca = new TH1F("Nsporadic_HECA","Number of sporadic-cell plots with CSC veto",10,0,10);
    m_badCell_HECA->regHist(m_h_sporad_heca).ignore();
    m_h_sporad_fcalc = new TH1F("Nsporadic_FCALC","Number of sporadic-cell plots with CSC veto",10,0,10);
    m_badCell_FCALC->regHist(m_h_sporad_fcalc).ignore();
    m_h_sporad_fcala = new TH1F("Nsporadic_FCALA","Number of sporadic-cell plots with CSC veto",10,0,10);
    m_badCell_FCALA->regHist(m_h_sporad_fcala).ignore();
//    ATH_MSG_INFO("end of bookSporHists()");
}

void CaloCellVecMon::bookLarNonThreHists(){
//    ATH_MSG_INFO("in bookLarNonThreHists()");

    double logEnergyBins[55];
    if (m_useLogarithmicEnergyBinning) {
      double step = pow(10,0.1);
      logEnergyBins[0]=10;
      for(int i = 1; i<55; i++){  //Cover full range of energy specification, 10 MeV to 3 TeV
        logEnergyBins[i] = step * logEnergyBins[i-1];
      }
    }

    //int nbintime = 26;
    //double timescale[27] = {-200,-150,-100,-50,-40,-30,-20,-10,-5,-4,-3,-2,-1,0,1,2,3,4,5,10,20,30,40,50,100,150,200};
    int nbintime = 92;
    double timescale[93] = {-200,-195,-190,-185,-180,-175,-170,-165,-160,-155,-150,-145,-140,-135,-130,-125,-120,-115,-110,-105,-100,-95,-90,-85,-80,-75,-70,-65,-60,-55,-50,-45,-40,-35,-30,-25,-20,-15,-10,-8,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,8,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125,130,135,140,145,150,155,160,165,170,175,180,185,190,195,200};
    int nbinenergy = 11;
    double energyscale[12] = {250,500,1000,2500,5000,10000,25000,50000,100000,250000,500000,1000000};
    int Thresholds[]={1000,1000,1000,1000,3000,3000,1500,1500,3500,3500,3500,3500,3500,3500,3500,3500,1500,1500, 1000,1000,3000,3000,2000,2000,1000,1000,1000,1000,1000,1000};  
    
    for(int ilyr=EMBPA; ilyr<MAXLAYER; ilyr++) {
      char name[256];
      char title[256];

      if(m_doEtaLumi){
       CaloLayer *lyr = m_layer[ilyr];
       sprintf(name,"Cell_Occupancy_EtaLumi_%s_4Sigma",lyr->getName());
       sprintf(title,"Cell Occupancy Eta Vs LumiBlock in %s with |E_{cell}|>4#sigma_{noise}^{database} with CSC veto",lyr->getName());
       char name_en[100];
       char title_en[200];
       sprintf(name_en,"Cell_AvgEnergy_EtaLumi_%s_4Sigma",lyr->getName());
       sprintf(title_en,"Cell Average Energy Eta Vs LumiBlock in %s with |E_{cell}|>4#sigma_{noise}^{database} with CSC veto",lyr->getName());
       m_h_cellOccupancyEtaLumi[ilyr] = new TH2F(name,title,2000,0.5,2000.5,lyr->getNTotEtaBins(),lyr->getEtaBinArray());
       m_h_cellOccupancyEtaLumi[ilyr]->SetXTitle("LumiBlock");
       m_h_cellOccupancyEtaLumi[ilyr]->SetYTitle("#eta");
       m_h_cellAvgEnergyEtaLumi[ilyr] = new TProfile2D(name_en,title_en,2000,0.5,2000.5,lyr->getNTotEtaBins(),lyr->getEtaBinArray());
       m_h_cellAvgEnergyEtaLumi[ilyr]->SetXTitle("LumiBlock");
       m_h_cellAvgEnergyEtaLumi[ilyr]->SetYTitle("#eta");
       m_shifterMonGroup[Occupancy_2d]->regHist(m_h_cellOccupancyEtaLumi[ilyr]).ignore();
       m_shifterMonGroup[AvgEnergy]->regHist(m_h_cellAvgEnergyEtaLumi[ilyr]).ignore();
     }
    if(m_doPhiLumi){
       CaloLayer *lyr = m_layer[ilyr];
       sprintf(name,"Cell_Occupancy_PhiLumi_%s_4Sigma",lyr->getName());
       sprintf(title,"Cell Occupancy Phi Vs LumiBlock in %s with |E_{cell}|>4#sigma_{noise}^{database} with CSC veto",lyr->getName());
       char name_en[100];
       char title_en[200];
       sprintf(name_en,"Cell_AvgEnergy_PhiLumi_%s_4Sigma",lyr->getName());
       sprintf(title_en,"Cell Average Energy Phi Vs LumiBlock in %s with |E_{cell}|>4#sigma_{noise}^{database} with CSC veto",lyr->getName());
       m_h_cellOccupancyPhiLumi[ilyr] = new TH2F(name,title,2000,0.5,2000.5,lyr->getNTotPhiBins(),lyr->getPhiBinArray());
       m_h_cellOccupancyPhiLumi[ilyr]->SetXTitle("LumiBlock");
       m_h_cellOccupancyPhiLumi[ilyr]->SetYTitle("#phi");
       m_h_cellAvgEnergyPhiLumi[ilyr] = new TProfile2D(name_en,title_en,2000,0.5,2000.5,lyr->getNTotPhiBins(),lyr->getPhiBinArray());
       m_h_cellAvgEnergyPhiLumi[ilyr]->SetXTitle("LumiBlock");
       m_h_cellAvgEnergyPhiLumi[ilyr]->SetYTitle("#phi");
       m_shifterMonGroup[Occupancy_2d]->regHist(m_h_cellOccupancyPhiLumi[ilyr]).ignore();
       m_shifterMonGroup[AvgEnergy]->regHist(m_h_cellAvgEnergyPhiLumi[ilyr]).ignore();
     }

     if ( m_doUnnormalized1DEnergy ) {

        sprintf(name,"CellEnergy_%s",m_layer[ilyr]->getName());
        sprintf(title,"Cell Energy in %s with CSC veto",m_layer[ilyr]->getName());
        // Note on (fine) energy binning: as energy comes in integer multiples of 1MeV, plots of
        // energy distributions which range from negative to positive must have a bin size (range /nbins)
        // which is an odd number multiple of 1MeV.
        if( !m_useLogarithmicEnergyBinning ) {
          if(m_dataType == AthenaMonManager::cosmics) {
            if(ilyr>=EMBPA && ilyr<=EMB3C) {
              m_h_energy[ilyr] = new TH1F(name,title,201,-318.5,1088.5); //bin size 7 MeV
            }
            if(ilyr>=EMECPA && ilyr<=EMEC3C) {
              m_h_energy[ilyr] = new TH1F(name,title,201,-591.5,2021.5); //bin size 13 MeV
            }
            if(ilyr>=HEC0A && ilyr<=HEC3C) {
              m_h_energy[ilyr] = new TH1F(name,title,201,-2047.5,6997.5); //bin size 45 MeV
            }
            if(ilyr>=FCAL1A && ilyr<=FCAL3C) {
              m_h_energy[ilyr] = new TH1F(name,title,201,-1865.5,6375.5); // bin size 41 MeV
            }
          } 
          else {
            m_h_energy[ilyr] = new TH1F(name,title,401,-1912.5,28162.5); // bin size 75 MeV         
          }
          int nbins   = m_h_energy[ilyr]->GetNbinsX();
          double binlow  = m_h_energy[ilyr]->GetXaxis()->GetXmin();
          double binhigh = m_h_energy[ilyr]->GetXaxis()->GetXmax();
          double binsize = abs(int(binhigh)-int(binlow))/nbins;
          char axisTitle[64];
          sprintf(axisTitle,"# Cell Events/%0.f MeV",binsize);
          m_h_energy[ilyr]->GetYaxis()->SetTitle(axisTitle);
          m_h_energy[ilyr]->GetYaxis()->SetTitleOffset(2);
        } // end of if( !m_useLogarithmicEnergyBinning )
        else {
          m_h_energy[ilyr] = new TH1F(name,title,54,logEnergyBins);
          m_h_energy[ilyr]->GetYaxis()->SetTitle("Cell Events");
          m_h_energy[ilyr]->GetYaxis()->SetTitleOffset(2);
        }

        m_h_energy[ilyr]->GetXaxis()->SetTitle("Cell Energy (MeV)");
        m_h_energy[ilyr]->GetXaxis()->SetLabelSize(0.02);
        m_h_energy[ilyr]->GetXaxis()->SetLabelOffset(0.01);
        m_h_energy[ilyr]->GetXaxis()->SetTitleSize(0.03);
        m_h_energy[ilyr]->GetXaxis()->SetTitleOffset(1.5);
        m_shifterMonGroup[Energy]->regHist(m_h_energy[ilyr]).ignore();

        if(m_doInverseMasking) {
          sprintf(name,"CellEnergy_%s_inverseMasked",m_layer[ilyr]->getName());
          sprintf(title,"Cell Energy in %s - Inverse Masked with CSC veto",m_layer[ilyr]->getName());
          if( !m_useLogarithmicEnergyBinning ) {
            if(m_dataType == AthenaMonManager::cosmics) {
              if(ilyr>=EMBPA && ilyr<=EMB3C) {
                m_h_energy_imask[ilyr] = new TH1F(name,title,201,-318.5,1088.5);
              }
              if(ilyr>=HEC0A && ilyr<=HEC3C) {
                m_h_energy_imask[ilyr] = new TH1F(name,title,201,-591.5,2021.5);
              }
              if(ilyr>=EMECPA && ilyr<=EMEC3C) {
                m_h_energy_imask[ilyr] = new TH1F(name,title,201,-2047.5,6997.5);
              }
              if(ilyr>=FCAL1A && ilyr<=FCAL3C) {
                m_h_energy_imask[ilyr] = new TH1F(name,title,201,-1865.5,6375.5);
              }
            }
            else {
              m_h_energy_imask[ilyr] = new TH1F(name,title,401,-1912.5,28162.5);
            }
            int nbins   = m_h_energy_imask[ilyr]->GetNbinsX();
            double binlow  = m_h_energy_imask[ilyr]->GetXaxis()->GetXmin();
            double binhigh = m_h_energy_imask[ilyr]->GetXaxis()->GetXmax();
            double binsize = abs(int(binhigh)-int(binlow))/nbins;
            char axisTitle[64];
            sprintf(axisTitle,"# Cell Events/%0.f MeV",binsize);
            m_h_energy_imask[ilyr]->GetYaxis()->SetTitle(axisTitle);
          } // end of if( !m_useLogarithmicEnergyBinning )
          else {
            m_h_energy_imask[ilyr] = new TH1F(name,title,54,logEnergyBins);
            m_h_energy_imask[ilyr]->GetYaxis()->SetTitle("Cell Events");
          }
          m_h_energy_imask[ilyr]->GetXaxis()->SetTitle("Cell Energy (MeV)");
//          ExpertMonGroup[Energy]->regHist(m_h_energy_imask[ilyr]).ignore();
          m_shifterMonGroup[Energy]->regHist(m_h_energy_imask[ilyr]).ignore();
        } // end of if(m_doInverseMasking)
      } // end of if ( m_doUnnormalized1DEnergy )

      if( m_doDBNormalized1DEnergy ) {
        sprintf(name,"DBNoiseNormalizedEnergy_%s",m_layer[ilyr]->getName());
        sprintf(title,"Cell Energy/Noise(DB) %s with CSC veto",m_layer[ilyr]->getName());
        m_h_dbNoiseNormalizedEnergy[ilyr] = new TH1D(name,title,31, -15.5, 15.5);
        m_h_dbNoiseNormalizedEnergy[ilyr]->GetXaxis()->SetTitle("Cell Energy / sigma (DB)");
        m_h_dbNoiseNormalizedEnergy[ilyr]->GetYaxis()->SetTitle("Cell Events");
        m_shifterMonGroup[Energy]->regHist(m_h_dbNoiseNormalizedEnergy[ilyr]).ignore();
      }

      if (m_doEnergyVsTime){
	sprintf(name,"CellEnergyVsTime_%s",m_layer[ilyr]->getName());
	sprintf(title,"Cell Energy vs Cell Time in %s with CSC veto",m_layer[ilyr]->getName());
	m_h_energyVsTime[ilyr] = new TH2F(name,title,nbintime,timescale,nbinenergy,energyscale);
	m_h_energyVsTime[ilyr]->GetXaxis()->SetTitle("Cell Time (ns)");
	m_h_energyVsTime[ilyr]->GetYaxis()->SetTitle("Energy (MeV)");
	m_shifterMonGroup[EnergyVsTime]->regHist(m_h_energyVsTime[ilyr]).ignore();
	
	sprintf(name,"CellEnergyVsTime_%s_%d",m_layer[ilyr]->getName(),Thresholds[ilyr]);
	sprintf(title,"Cell Energy vs Cell Time in %s_%d with CSC veto",m_layer[ilyr]->getName(),Thresholds[ilyr]);
	m_h_energyVsTime_DifferThresholds[ilyr] = new TH1F(name,title,nbintime,timescale);
	m_h_energyVsTime_DifferThresholds[ilyr]->GetXaxis()->SetTitle("Cell Time (ns)");
	m_h_energyVsTime_DifferThresholds[ilyr]->GetYaxis()->SetTitle("Energy (MeV)");
	m_shifterMonGroup[EnergyVsTime]->regHist(m_h_energyVsTime_DifferThresholds[ilyr]).ignore();
	
	if(m_doInverseMasking) {
	  sprintf(name,"CellEnergyVsTime_%s_inverseMasked",m_layer[ilyr]->getName());
	  sprintf(title,"Cell Energy vs Cell Time in %s - Inverse Masked with CSC veto",m_layer[ilyr]->getName());
	  m_h_energyVsTime_imask[ilyr] = new TH2F(name,title,nbintime,timescale,nbinenergy,energyscale);
	  m_h_energyVsTime_imask[ilyr]->GetXaxis()->SetTitle("Cell Time (ns)");
	  m_h_energyVsTime_imask[ilyr]->GetYaxis()->SetTitle("Energy (MeV)");
	  //        ExpertMonGroup[EnergyVsTime]->regHist(m_h_energyVsTime_imask[ilyr]).ignore();
	  m_shifterMonGroup[EnergyVsTime]->regHist(m_h_energyVsTime_imask[ilyr]).ignore();
	}
      }
    } // end of ilyr loop
//    ATH_MSG_INFO("end of bookLarNonThreHists()");

}

void CaloCellVecMon::bookSummHists(){
//    ATH_MSG_INFO("in bookSummHists()");
    m_h_n_trigEvent = new TH1F("nEvtsByTrigger","Total Events: bin 0, RNDM Trigger: 1, Calo Trigger: 2, MinBias Trigger: 3, MET Trigger: 4, Misc Trigger: 5, Events Selected for Noise Plots: 6 ",7,1.,8.);
    m_summaryGroup->regHist( m_h_n_trigEvent ).ignore();

    bookBaseHists(m_summaryGroup).ignore(); //from base class
}
 
 
////////////////////////////////////////////////////////////////////////////
StatusCode CaloCellVecMon::fillHistograms(){  

//  ATH_MSG_INFO("CaloCellVecMon::fillHistograms() starts");
  StatusCode sc = StatusCode::SUCCESS;

  bool ifPass = 1;
  sc = checkFilters(ifPass);
  if(sc.isFailure() || !ifPass) return StatusCode::SUCCESS;


  fillTrigPara();

  //=====================
  // CaloCell info
  //============================

  const CaloCellContainer* cellCont = 0;
  sc=evtStore()->retrieve(cellCont, m_cellContainerName);
  if( sc.isFailure()  ||  !cellCont ) {
    ATH_MSG_WARNING("No CaloCell container found in TDS"); 
    return sc;
  }  

  CaloCellContainer::const_iterator it = cellCont->begin(); 
  CaloCellContainer::const_iterator it_e = cellCont->end(); 

  // loop over cells -------------

  m_newevent = true;  
  m_ncells=0;

  // counting events
  m_eventsCounter++;
//  ATH_MSG_INFO("CaloCellVecMon::m_eventsCounter="<<m_eventsCounter);

//  ATH_MSG_INFO("before it loop in fillHistograms() ");
  for ( ; it!=it_e;++it) { 

    // cell info
    const CaloCell* cell = *it; 
    Identifier id = cell->ID();
    bool is_lar=m_em_id->is_lar(id);
    bool is_tile=m_tile_id->is_tile(id) ;
    const CaloDetDescrElement* caloDDEl=  cell->caloDDE();
    double cellphi = caloDDEl->phi_raw();
    
    if(m_sporadic_switch) {
      if(!caloDDEl->is_tile()) 
       if(m_passBeamBackgroundRemoval)
         fillSporHists(cell);
    }  

    if(is_tile) fillTileHists(cell);

    if(is_lar) fillLarHists(cell);

    if(  m_eventsCounter == 1 && fabs(cellphi) > M_PI) {
      ATH_MSG_WARNING("CaloCellVecMon: Cell Phi values outside of -Pi to Pi range were found and not corrected: check CaloDetDescr."); 
    }
  } // end of it loop 
//  ATH_MSG_INFO("after it loop in fillHistograms() ");

  m_h_Ncells->Fill( (float) m_ncells ); // count TileCal cells above threshold

//  if(m_eventsCounter == 1) maskMissCell();
  maskMissCell();
   
  if(m_sporadic_switch)  m_h_sporad->Fill(1);

  return sc;
}




void CaloCellVecMon::fillTrigPara(){
//  ATH_MSG_INFO("CaloCellVecMon::fillTrigPara() starts");
  m_h_n_trigEvent->Fill(0.5);

  bool isRndmEvent    = false;
  bool isCaloEvent    = false;
  bool isMinBiasEvent = false;
  bool isMiscEvent    = false;
  bool isMetEvent     = false;
  m_fillNoThreshNoisePlots = true;

  if (m_useTrigger){
    if( m_isRndmTrigDefined ) {
      isRndmEvent = m_rndmTriggerEvent->isPassed();
    }
    if( m_isCaloTrigDefined ) {
      isCaloEvent = m_caloTriggerEvent->isPassed();
    }
    if( m_isMinBiasTrigDefined ) {
      isMinBiasEvent = m_minBiasTriggerEvent->isPassed();
    }
    if( m_isMetTrigDefined ) {
      isMetEvent = m_metTriggerEvent->isPassed();
    }
    if( m_isMiscTrigDefined ) {
      isMiscEvent = m_miscTriggerEvent->isPassed();
    }

    switch (m_triggersToIncludeInNoisePlots) {
    case RNDM:
      m_fillNoThreshNoisePlots = isRndmEvent;
      break;
    case MINBIAS:
      m_fillNoThreshNoisePlots = isMinBiasEvent;
      break;
    case CALO:
      m_fillNoThreshNoisePlots = isCaloEvent;
      break;
    case MET:
      m_fillNoThreshNoisePlots = isMetEvent;
      break;
    case MISC:
      m_fillNoThreshNoisePlots = isMiscEvent;
      break;
    case NOTA:
    default:
      m_fillNoThreshNoisePlots = true;
      break;
    }

    switch (m_triggersToExcludeFromNoisePlots) {
    case RNDM:
      if( isRndmEvent ) {
        m_fillNoThreshNoisePlots = false;
      }
      break;
    case MINBIAS:
      if( isMinBiasEvent ) {
        m_fillNoThreshNoisePlots = false;
      }
      break;
    case CALO:
      if( isCaloEvent ) {
        m_fillNoThreshNoisePlots = false;
      }
      break;
    case MET:
      if( isMetEvent ) {
        m_fillNoThreshNoisePlots = false;
      }
      break;
    case MISC:
      if( isMiscEvent ) {
        m_fillNoThreshNoisePlots = false;
      }
      break;
    }

    if(isRndmEvent) {
      m_h_n_trigEvent->Fill(1.5);
    }
    if(isCaloEvent) {
      m_h_n_trigEvent->Fill(2.5);
    }
    if(isMinBiasEvent) {
      m_h_n_trigEvent->Fill(3.5);
    }
    if(isMetEvent) {
      m_h_n_trigEvent->Fill(4.5);
    }
    if(isMiscEvent) {
      m_h_n_trigEvent->Fill(5.5);
    }
    if(m_fillNoThreshNoisePlots) {
      m_h_n_trigEvent->Fill(6.5);
    }
   
  
 
    for(int ilyrns=EMBPNS; ilyrns<MAXLYRNS; ilyrns++) {
      for(int ti=0; ti < m_nThresholds[ilyrns]; ti++) {
        if ( (m_threshTriggersToInclude[ilyrns][ti] == NOTA) && (m_threshTriggersToExclude[ilyrns][ti] == NOTA) ) {
          if(m_doBeamBackgroundRemoval[ilyrns][ti]) {
           if(m_passBeamBackgroundRemoval)   m_nEventsPassingTrigger[ilyrns][ti]++;
          }
          else m_nEventsPassingTrigger[ilyrns][ti]++;
          m_threshTriggerDecision[ilyrns][ti] = true;
          continue; //This element was already initialized to true, so nothing to do.
        }
        switch (m_threshTriggersToInclude[ilyrns][ti]) {
         case RNDM:
          m_threshTriggerDecision[ilyrns][ti] = isRndmEvent;
          break;
         case MINBIAS:
          m_threshTriggerDecision[ilyrns][ti] = isMinBiasEvent;
          break;
         case CALO:
          m_threshTriggerDecision[ilyrns][ti] = isCaloEvent;
          break;
         case MET:
          m_threshTriggerDecision[ilyrns][ti] = isMetEvent;
          break;
         case MISC:
          m_threshTriggerDecision[ilyrns][ti] = isMiscEvent;
          break;
        }
        switch (m_threshTriggersToExclude[ilyrns][ti]) {
         case RNDM:
          if( isRndmEvent ) {
            m_threshTriggerDecision[ilyrns][ti] = false;
          }
          break;
         case MINBIAS:
          if( isMinBiasEvent ) {
            m_threshTriggerDecision[ilyrns][ti] = false;
          }
          break;
         case CALO:
          if( isCaloEvent ) {
            m_threshTriggerDecision[ilyrns][ti] = false;
          }
          break;
         case MET:
          if( isMetEvent ) {
            m_threshTriggerDecision[ilyrns][ti] = false;
          }
          break;
         case MISC:
          if( isMiscEvent ) {
            m_threshTriggerDecision[ilyrns][ti] = false;
          }
          break;
        }
        if( m_threshTriggerDecision[ilyrns][ti] ) {
          if(m_doBeamBackgroundRemoval[ilyrns][ti]) { 
           if(m_passBeamBackgroundRemoval)   m_nEventsPassingTrigger[ilyrns][ti]++;
          }
          else m_nEventsPassingTrigger[ilyrns][ti]++;
        }
      } // end of ti loop 
    }  // end of ilyrns loop
  } // end of m_useTrigger if
  else {
    m_h_n_trigEvent->Fill(6.5);
  }


  //if (m_useTrigger) h_EvtRejSumm->Fill(6); 
  

  for(int ilyrns=EMBPNS; ilyrns<MAXLYRNS; ilyrns++) {
   for(int ti=0; ti < m_nThresholds[ilyrns]; ti++) {
      if(m_doBeamBackgroundRemoval[ilyrns][ti]){
        if(m_passBeamBackgroundRemoval) m_nEventsPassingBeamBG[ilyrns][ti]++;
      }
      else m_nEventsPassingBeamBG[ilyrns][ti]++;
   }
  }

//  ATH_MSG_INFO("CaloCellVecMon::fillTrigPara() is done");

}

void CaloCellVecMon::fillSporHists(const CaloCell* cell ){
//   ATH_MSG_INFO("CaloCellVecMon::fillSporHists() starts");
   const CaloDetDescrElement* caloDDEl=  cell->caloDDE();
   Identifier id_s = cell->ID();
   float energy_s = cell->energy();
   double phi_s = cell->phi();
   double eta_s = cell->eta();
   float quality_s = cell->quality(); 

   HWIdentifier onlID;
   try {
     onlID = m_LArCablingService->createSignalChannelID(id_s);
   }
   catch(LArID_Exception& except) {
     msg(MSG::ERROR) << "LArID_Exception " << m_LArEM_IDHelper->show_to_string(id_s) << " " << (std::string) except << endreq ;
     msg(MSG::ERROR) << "LArID_Exception " << m_LArHEC_IDHelper->show_to_string(id_s) << endreq;
     msg(MSG::ERROR) << "LArID_Exception " << m_LArFCAL_IDHelper->show_to_string(id_s) << endreq;
   }

   int posneg= m_LArOnlineIDHelper->pos_neg(onlID);
   int ft = m_LArOnlineIDHelper->feedthrough(onlID);
   int slot = m_LArOnlineIDHelper->slot(onlID);
   int channel = m_LArOnlineIDHelper->channel(onlID);
   IdentifierHash channelHash = m_LArOnlineIDHelper->channel_Hash(onlID);

   bool known = false;
   bool EMBorEMEC = (caloDDEl->is_lar_em_barrel()||caloDDEl->is_lar_em_endcap());
   bool sampling0or1=(m_em_id->sampling(id_s)==0 || m_em_id->sampling(id_s)==1);
   if(EMBorEMEC && sampling0or1) m_threshold = m_threshold_em_S0S1;
   else m_threshold = m_threshold_HECFCALEMS2S3;

   std::string part_name = "";
   int sample_s = -1;
   int name_flag=0;
   m_badCell_flag = true;
   MonGroup * tmpBadCell = 0;
  
   if(caloDDEl->is_lar_em_barrel() && posneg == 0) {
      part_name = "EMBC";
      sample_s = m_em_id->sampling(id_s);
      name_flag=1;
      m_badCell_flag = m_badCell_flag_embc;
      tmpBadCell = m_badCell_EMBC;
   }
   if(caloDDEl->is_lar_em_barrel() && posneg != 0) {
     part_name = "EMBA";
     sample_s = m_em_id->sampling(id_s);;
     name_flag=2;
     m_badCell_flag = m_badCell_flag_emba;
     tmpBadCell = m_badCell_EMBA;
   }
   if(caloDDEl->is_lar_em_endcap() && posneg == 0)  {
     part_name = "EMECC";
     sample_s = m_em_id->sampling(id_s);;
     name_flag=3;
     m_badCell_flag = m_badCell_flag_emecc;
     tmpBadCell = m_badCell_EMECC;
   }
   if(caloDDEl->is_lar_em_endcap() && posneg != 0)  {
     part_name = "EMECA";
     sample_s = m_em_id->sampling(id_s);;
     name_flag=4;
     m_badCell_flag = m_badCell_flag_emeca;
     tmpBadCell = m_badCell_EMECA;
   }
   if(caloDDEl->is_lar_hec() && posneg == 0) {
     part_name = "HECC";
     sample_s = m_calo_id->sampling(id_s);
     name_flag=5;
     m_badCell_flag = m_badCell_flag_hecc;
     tmpBadCell = m_badCell_HECC;
   }
   if(caloDDEl->is_lar_hec() && posneg != 0) {
     part_name = "HECA";
     sample_s = m_calo_id->sampling(id_s);
     name_flag=6;
     m_badCell_flag = m_badCell_flag_heca;
     tmpBadCell = m_badCell_HECA;
   }
   if(caloDDEl->is_lar_fcal() && posneg == 0)  {
     part_name = "FCALC";
     sample_s = m_calo_id->sampling(id_s);;
     name_flag=7;
     m_badCell_flag = m_badCell_flag_fcalc;
     tmpBadCell = m_badCell_FCALC;
   }
   if(caloDDEl->is_lar_fcal() && posneg != 0)  {
     part_name = "FCALA";
     sample_s = m_calo_id->sampling(id_s);;
     name_flag=8;
     m_badCell_flag = m_badCell_flag_fcala;
     tmpBadCell = m_badCell_FCALA;
   }

   if(energy_s>m_threshold){
     for (uint iMon=0;iMon<m_monitored.size();iMon++){
       if (channelHash == m_monitored[iMon]){
         known = true;
         m_occ[iMon]++;

         if(m_h_energy_s[iMon]) m_h_energy_s[iMon]->Fill(energy_s);
         if(m_h_energy_lb[iMon]) m_h_energy_lb[iMon]->Fill(m_lb,energy_s);
         if(m_h_quality[iMon]) m_h_quality[iMon]->Fill(quality_s);
      
         if(!m_h_energy_s[iMon] || !m_h_energy_lb[iMon] || !m_h_quality[iMon]){
           if(m_counter_sporadic_protc < m_sporadic_protc && m_badCell_flag) {
            std::string name_s_tmp = part_name + "_" + format("FT%dSl%dCh%d",ft,slot,channel) + "_" + format
("Phi%.2f_Eta%.2f",phi_s,eta_s);
            std::string name_s = part_name + "_" + format("FT%dSl%dCh%d",ft,slot,channel) + "_" + format
("Phi%.2f_Eta%.2f",phi_s,eta_s) +format("_sampling%d",sample_s)+"_"+ "EN";
            std::string namelb =  name_s + "LB" ;
            std::string nameQ = name_s_tmp + "Quality";
            float eMin = -10000;
            float eMax = 140000;
            uint nBins = uint ((eMax-eMin)*(1./2000)); // Bins of 2000MeV

            if(!m_h_energy_s[iMon]){
              std::string title_s = name_s+"::"+format("%.0fGeV with CSC veto",m_threshold*1e-3);
              m_h_energy_s[iMon] = new TH1F(name_s.c_str(),title_s.c_str(),nBins,eMin,eMax);
              if(tmpBadCell) tmpBadCell->regHist(m_h_energy_s[iMon]).ignore();
              m_h_energy_s[iMon]->GetXaxis()->SetTitle("MeV");
              m_h_energy_s[iMon]->GetYaxis()->SetTitle("Number of Events");
              m_h_energy_s[iMon]->Fill(energy_s);
            }
            if(!m_h_energy_lb[iMon]){
              std::string titlelb= namelb +"::"+format("%.0fGeV",m_threshold*1e-3);
              m_h_energy_lb[iMon] = new TProfile(namelb.c_str(),titlelb.c_str(),1400,0.5,1400.5);
              if(tmpBadCell) tmpBadCell->regHist(m_h_energy_lb[iMon]).ignore(); 
              m_h_energy_lb[iMon]->GetXaxis()->SetTitle("LumiBlocks");
              m_h_energy_lb[iMon]->GetYaxis()->SetTitle("MeV");
              m_h_energy_lb[iMon]->Fill(m_lb,energy_s);
            }
            if(!m_h_quality[iMon]){
              std::string titleQ = nameQ +"::"+format("%.0fGeV",m_threshold*1e-3);
              m_h_quality[iMon] = new TH1F(nameQ.c_str(),titleQ.c_str(),20,0,20000);
              if(tmpBadCell) tmpBadCell->regHist(m_h_quality[iMon]).ignore();
              m_h_quality[iMon] ->GetXaxis()->SetTitle("Quality Factor");
              m_h_quality[iMon] ->GetYaxis()->SetTitle("Number of Events");
              m_h_quality[iMon] ->Fill(quality_s); 
            }
 
            m_counter_sporadic_protc++; 
           } // end of if(counter_sporadic_protc < m_sporadic_protc && m_badCell_flag)
         } // end of if(!m_h_energy_s[iMon] || !m_h_energy_lb[iMon] || !m_h_quality[iMon])
  
         if(m_occ[iMon] == (m_EventNumber_thres) && !m_hasAdded[iMon]){
           switch(name_flag){
             case 1: m_h_sporad_embc->Fill(1); m_nbh_embc++;m_hasAdded[iMon]=1;break;
             case 2: m_h_sporad_emba->Fill(1); m_nbh_emba++;m_hasAdded[iMon]=1;break;
             case 3: m_h_sporad_emecc->Fill(1);m_nbh_emecc++;m_hasAdded[iMon]=1;break;
             case 4: m_h_sporad_emeca->Fill(1);m_nbh_emeca++;m_hasAdded[iMon]=1;break;
             case 5: m_h_sporad_hecc->Fill(1);m_nbh_hecc++;m_hasAdded[iMon]=1;break;
             case 6: m_h_sporad_heca->Fill(1);m_nbh_heca++;m_hasAdded[iMon]=1;break;
             case 7: m_h_sporad_fcalc->Fill(1); m_nbh_fcalc++;m_hasAdded[iMon]=1;break;
             case 8: m_h_sporad_fcala->Fill(1); m_nbh_fcala++;m_hasAdded[iMon]=1;break;
           }

          if(!m_badCell_flag_embc && name_flag==1) break;
          if(!m_badCell_flag_emba && name_flag==2 ) break;
          if(!m_badCell_flag_emecc && name_flag==3) break;
          if(!m_badCell_flag_emeca && name_flag==4) break;
          if(!m_badCell_flag_hecc && name_flag==5 ) break;
          if(!m_badCell_flag_heca && name_flag==6) break;
          if(!m_badCell_flag_fcalc && name_flag==7) break;
          if(!m_badCell_flag_fcala && name_flag==8) break;

          if(m_badCell_flag_embc && m_nbh_embc >=m_sporadicPlotLimit){
           char message[100];
           sprintf(message,"More than %i EMBC histos booked",m_sporadicPlotLimit);
           ATH_MSG_INFO(message);
           m_badCell_flag_embc = false;
          }
          if(m_badCell_flag_emba && m_nbh_emba>=m_sporadicPlotLimit){
           char message[100];
           sprintf(message,"More than %i EMBA histos booked",m_sporadicPlotLimit);
           ATH_MSG_INFO(message);
           m_badCell_flag_emba = false;
          }
          if(m_badCell_flag_emecc && m_nbh_emecc>=m_sporadicPlotLimit){
           char message[100];
           sprintf(message,"More than %i EMECC histos booked",m_sporadicPlotLimit);
           ATH_MSG_INFO(message);
           m_badCell_flag_emecc = false;
          }
          if(m_badCell_flag_emeca && m_nbh_emeca>=m_sporadicPlotLimit){
           char message[100];
           sprintf(message,"More than %i EMECA histos booked",m_sporadicPlotLimit);
           ATH_MSG_INFO(message);
           m_badCell_flag_emeca = false;
          }
          if(m_badCell_flag_hecc && m_nbh_hecc>=m_sporadicPlotLimit){
           char message[100];
           sprintf(message,"More than %i HECC histos booked",m_sporadicPlotLimit);
           ATH_MSG_INFO(message);
           m_badCell_flag_hecc = false;
          }
          if(m_badCell_flag_heca && m_nbh_heca>=m_sporadicPlotLimit){
           char message[100];
           sprintf(message,"More than %i HECA histos booked",m_sporadicPlotLimit);
           ATH_MSG_INFO(message);
           m_badCell_flag_heca = false;
          }
          if(m_badCell_flag_fcalc && m_nbh_fcalc>=m_sporadicPlotLimit){
           char message[100];
           sprintf(message,"More than %i FCALC histos booked",m_sporadicPlotLimit);
           ATH_MSG_INFO(message);
           m_badCell_flag_fcalc = false;
          }
          if(m_badCell_flag_fcala && m_nbh_fcala>=m_sporadicPlotLimit){
           char message[100];
           sprintf(message,"More than %i FCALA histos booked",m_sporadicPlotLimit);
           ATH_MSG_INFO(message);
           m_badCell_flag_fcala = false;
          }
         }  // end of if(m_occ[iMon] == (m_EventNumber_thres) && !m_hasAdded[iMon])
         
       } // end of if (channelHash == m_monitored[iMon])   
     } // end of iMon loop
  
     if(!known){
      m_monitored.push_back(channelHash);
      m_occ.push_back(1);
      m_hasAdded.push_back(0);
      m_h_energy_s.resize(m_monitored.size());
      m_h_energy_lb.resize(m_monitored.size());
      m_h_quality.resize(m_monitored.size()); 
      m_h_energy_s[m_monitored.size()-1] = 0;
      m_h_energy_lb[m_monitored.size()-1] = 0;
      m_h_quality[m_monitored.size()-1] = 0;
     } 

     if(!known) m_listOfNoiseBurstLumiblock[channelHash]=1;
     else m_listOfNoiseBurstLumiblock[channelHash]++;
     if(m_listOfNoiseBurstLumiblock[channelHash] > m_EventNumber_thres && m_newevent){
       m_h_NoiseBurstLumiblock->Fill(m_lb);
       m_newevent=false;
     } 

   } // end of energy_s>m_threshold
//   ATH_MSG_INFO("CaloCellVecMon::fillSporHists() is done");

}
  
void CaloCellVecMon::fillTileHists(const CaloCell* cell ){
//  ATH_MSG_INFO("CaloCellVecMon::fillTileHists() starts");

  Identifier id = cell->ID();
  IdentifierHash hash = m_tile_id->cell_hash(id);
  long sample=m_tile_id->sample(id);
  long module=m_tile_id->module(id);
  long tower=m_tile_id->tower(id);
  int side=m_tile_id->side(id); 
  float cellen = cell->energy();
  float cellenGeV=cellen/GeV;
  double eta = cell->eta();
  double phi = cell->phi();
  bool   badc=cell->badcell();
  float cellnoisedb = 0.;
  if (m_useNoiseToolGlobal) {
    if (m_useElectronicNoiseOnly){
       if (m_useTwoGaus==false) {
          cellnoisedb = m_noiseTool->getNoise(cell,ICalorimeterNoiseTool::ELECTRONICNOISE);
        } else {
          cellnoisedb = m_noiseTool->getEffectiveSigma(cell,ICalorimeterNoiseTool::MAXSYMMETRYHANDLING,ICalorimeterNoiseTool::ELECTRONICNOISE);
        }
    }
    else {
        if (m_useTwoGaus==false) {
          cellnoisedb = m_noiseTool->getNoise(cell,ICalorimeterNoiseTool::TOTALNOISE);
        } else {
          cellnoisedb = m_noiseTool->getEffectiveSigma(cell,ICalorimeterNoiseTool::MAXSYMMETRYHANDLING,ICalorimeterNoiseTool::TOTALNOISE);
        }
    }
  }

  double rs=999;
  if (std::isfinite(cellnoisedb) && cellnoisedb > 0 &&  cellen != 0 && badc == false) rs= cellen / cellnoisedb;

  if (sample < 3) { // Sample E not relevant for topoclusters
     if (badc == false && rs != 999 ) m_h_CellsNoiseE->Fill( rs );
     if (fabs(rs)>4.0 && badc == false && rs != 999 ) {
         m_h_CellsNoiseEtaPhi->Fill( eta, phi);
         m_h_CellsNoiseHash->Fill(hash);
         if (sample ==0 ) m_h_CellsNoiseEtaPhiA->Fill(  eta, phi  );
         else if (sample ==1 ) m_h_CellsNoiseEtaPhiBC->Fill(  eta, phi );
         else  m_h_CellsNoiseEtaPhiD->Fill(  eta, phi  );

         if (msgLvl(MSG::DEBUG)) {
            msg(MSG::DEBUG) << "CaloCellVecMon: Cell rs=" << rs << " e=" << cellen << " eta=" << eta << " phi="<< phi << endreq;
            msg(MSG::DEBUG) << "CaloCellVecMon: hash=" << hash << " module= " << module+1 << " sample=" << sample
                              << " tower=" << side*tower<<endreq;
            }
     }// end of if (fabs(rs)>4.0 && badc == false && rs != 999 )

     if ( badc == false) {
         m_h_CellsNoiseEta->Fill(  eta, cellnoisedb, 1.0  );
         m_h_CellsNoisePhi->Fill(  phi, cellnoisedb, 1.0  );
         m_h_CellsRMSEta->Fill(  eta, cellenGeV, 1.0  );
         m_h_CellsRMSPhi->Fill(  phi, cellenGeV, 1.0  );
     }
  } // end of if (sample < 3)

  if (cellen > m_tileThreshold) {
      m_ncells++;
      m_h_CellsE->Fill(   cellenGeV  );
      m_h_CellsEta->Fill(  eta, cellenGeV, 1.0  );
      m_h_CellsPhi->Fill(  phi, cellenGeV, 1.0  );
      if (sample ==0 ) m_h_CellsModuleS1->Fill(  module+1, cellenGeV  );
      if (sample ==1 ) m_h_CellsModuleS2->Fill(  module+1, cellenGeV  );
      if (sample ==2 ) m_h_CellsModuleS3->Fill(  module+1, cellenGeV  );
      if (sample ==3 ) m_h_CellsModuleS4->Fill(  module+1, cellenGeV  );
      m_h_CellsTower->Fill(  tower+1, cellenGeV  );
      m_h_CellsEtaPhi->Fill( eta, phi );
  } 
//  ATH_MSG_INFO("CaloCellVecMon::fillTileHists() is done");
}

void CaloCellVecMon::fillLarHists(const CaloCell* cell ){
//  ATH_MSG_INFO("CaloCellVecMon::fillLarHists() starts ");
  Identifier id = cell->ID();
  const CaloDetDescrElement* caloDDEl=  cell->caloDDE();
  CaloGain::CaloGain  gain= cell->gain();
  float celleta = caloDDEl->eta_raw();
  float cellphi = caloDDEl->phi_raw();
  float cellen = cell->energy();
  float celltime = cell->time();
  uint16_t cellquality = cell->quality();
  uint16_t cellprovenance = cell->provenance();
  bool celltqavailable = ( cellprovenance & 0x2000 );
  float emb1PhiGran = M_PI/32;

  CaloLayer::Partition partition = CaloLayer::MAXPART;
  CaloLayer::Side side;
  int ilayer = -1;

  bool is_em_barrel=m_em_id->is_em_barrel(id);
  bool is_emec=m_em_id->is_em_endcap(id);
  bool is_hec= m_calo_id->is_hec(id);
  bool is_fcal= m_calo_id->is_fcal(id);

  if(is_em_barrel) {
    partition = CaloLayer::EMB;
    ilayer = m_em_id->sampling(id);
  }
  if(is_emec) {
    partition = CaloLayer::EMEC;
    ilayer = m_em_id->sampling(id);
  }
  if(is_hec) {
    partition = CaloLayer::HEC;
    ilayer = m_calo_id->sampling(id);
  }
  if(is_fcal) {
    partition = CaloLayer::FCAL;
      // important m_calo_id->sampling corresponds to the LAr_FCALId::module    
      // function for the FCAL so it is not 0,1,2 but 1,2 3                     
      // to match it to the way we count the layers we need to bring it to 0,1,2
    ilayer = m_calo_id->sampling(id) - 1;
  }

  if(celleta > 0)  side = CaloLayer::A;
  else  side = CaloLayer::C;

  int caloLyr = hashToLayer(partition, ilayer, side);
  int caloLyrNS = caloLyr/2;

   // do a little shift in eta for EMB sampling 1 to facilitate filling cheat in eta and eta phi plot
  if (ilayer == 1 && is_em_barrel) {
    if ( ((celleta > 1.4) && (celleta < 1.475)) || ((celleta < -1.4) && (celleta > -1.475))) {
       double phiMod;
        //this is the eta region where we mess around:                                            
        //take phi modulus the phi granularity we have in our histogram, then shift eta accordingly:
       phiMod = fmod( fabs(cellphi), emb1PhiGran);
       celleta = celleta + (phiMod * 0.2551) - 0.0125;
    }
  }

  float cellnoisedb = 0.;
  if (m_useNoiseToolGlobal) {
    if (m_useElectronicNoiseOnly){
       if (m_useTwoGaus==false) {
          cellnoisedb = m_noiseTool->getNoise(cell,ICalorimeterNoiseTool::ELECTRONICNOISE);
        } else {
          cellnoisedb = m_noiseTool->getEffectiveSigma(cell,ICalorimeterNoiseTool::MAXSYMMETRYHANDLING,ICalorimeterNoiseTool::ELECTRONICNOISE);
        }
    }
    else {
        if (m_useTwoGaus==false) {
          cellnoisedb = m_noiseTool->getNoise(cell,ICalorimeterNoiseTool::TOTALNOISE);
        } else {
          cellnoisedb = m_noiseTool->getEffectiveSigma(cell,ICalorimeterNoiseTool::MAXSYMMETRYHANDLING,ICalorimeterNoiseTool::TOTALNOISE);
        }
    }
  }

  if(fabs(cellen)>4*fabs(cellnoisedb) && m_passBeamBackgroundRemoval) {
    if(m_doEtaLumi) m_h_cellOccupancyEtaLumi[caloLyr]->Fill(m_lb,celleta);
    if(m_doPhiLumi) m_h_cellOccupancyPhiLumi[caloLyr]->Fill(m_lb,cellphi);
    if(m_doEtaLumi) m_h_cellAvgEnergyEtaLumi[caloLyr]->Fill(m_lb,celleta,cellen);
    if(m_doPhiLumi) m_h_cellAvgEnergyPhiLumi[caloLyr]->Fill(m_lb,cellphi,cellen);
  }

  LArBadChannel larBadChannel = m_badChannelTool->offlineStatus(id); //should we check if offline? will this work online?

  // masknoconditions == true  --- the cell needs to be masked as noconditions
  bool noconditions = (cellprovenance & 0x00FF) != 0x00A5;
  bool masknoconditions = false;
  if (m_maskNoCondChannels) masknoconditions = noconditions; 
  else  masknoconditions = false;

  // Bad channels: known plot always filled irrespective of selected inverse masking option
  // LArBadChannel chanstatus = m_badChannelTool->offlineStatus(id);
  // badcell==true -- the cell is bad from m_badChannelTool 
  bool badcell = !larBadChannel.good(); //<-- Known Bad Channels histogram based on this
  // maskbadcell == true ---  the cell needs to be masked as bad cell from m_badChannelMask 
  bool maskbadcell  = false;
  if (m_maskKnownBadChannels) 
      maskbadcell = m_badChannelMask->cellShouldBeMasked(id,gain); //<-- (Online) cell masking based on this
  else  maskbadcell = false; 
  
  // decide if cell should be masked: masksel==1: neither badcell, nor nocondition; masksel==0: either badcell, or noconditions 
  bool masksel = !maskbadcell && !masknoconditions; 

  // Fill known bad channel and db noise plots, if this is the first event:
  if( m_eventsCounter == 1 ) {
    // Known bad channel plots:
    if(badcell && (m_doKnownBadChannelsVsEtaPhi || ((!m_doMaskingOnline) && m_maskKnownBadChannels )) ) {
       uint32_t badCellWord = larBadChannel.packedData();
       m_h_badChannels_etaphi[caloLyr]->Fill(celleta,cellphi,badCellWord);
//       ATH_MSG_INFO("CaloCellVecMon::fill h_badChannels_etaphi");
//       ATH_MSG_INFO("CaloCellVecMon::fill h_badChannels_etaphi:badCellWord="<<badCellWord);
    }
//    else ATH_MSG_INFO("CaloCellVecMon:: not fill h_badChannels_etaphi");

    // DB noise plots:
    if ( m_doDatabaseNoiseVsEtaPhi ) {
       m_h_dbnoise_etaphi[caloLyr]->Fill(celleta,cellphi,cellnoisedb);
    }

    // 1D reference plots of active cells versus eta and phi: (needed for nomalization)
    if ( m_doNActiveCellsFirstEventVsEta ) {
       m_h_nActiveCellsFirstEvent_eta[caloLyr]->Fill(celleta);
    }
    if ( m_doNActiveCellsFirstEventVsPhi ) {
       m_h_nActiveCellsFirstEvent_phi[caloLyr]->Fill(cellphi);
    }

    // 2D plots of active cells versus etaphi: (used to mark missing cells in other histograms)
    if ( m_h_missingCellMask[caloLyr] != 0 ) {
       m_h_missingCellMask[caloLyr]->Fill(celleta,cellphi);
    }
  }

  bool   dbNormEnergyIsDefined = false;
  double dbNormEnergy = 0;
  const float inv_cellnoisedb = (cellnoisedb != 0 ? 1./cellnoisedb : 1);

  for(int ti=0; ti < m_nThresholds[caloLyrNS]; ti++) {
    // determine whether to use noise threshold or not
    float thresholdVal;
    if(m_useNoiseTool[caloLyrNS][ti]) {
       thresholdVal = m_thresholds[caloLyrNS][ti]*cellnoisedb;
    }
    else {
       thresholdVal = m_thresholds[caloLyrNS][ti];
    }

    bool threshIsPassed;
    switch (m_thresholdDirection[caloLyrNS][ti]) {
      case OVER :
        threshIsPassed = (cellen > thresholdVal);
        break;
      case UNDER :
        threshIsPassed = (cellen < thresholdVal);
        break;
      case NONE :
        threshIsPassed = true;
        break;
      case BOTH :
        threshIsPassed = ( (cellen > thresholdVal) || (cellen < -thresholdVal) );
        break;
      default :
        threshIsPassed = (cellen > thresholdVal);
    }

    // Check if the trigger for this threshold was passed:
    if(m_useTrigger && threshIsPassed) {
       threshIsPassed = m_threshTriggerDecision[caloLyrNS][ti];
    }

    if(m_doBeamBackgroundRemoval[caloLyrNS][ti] && threshIsPassed) {
        threshIsPassed = m_passBeamBackgroundRemoval;
    }

    if(threshIsPassed) {
       if(masksel || (!m_doMaskingOnline) ) {

          if(m_fillEtaPhiOccupancy[caloLyrNS][ti]) m_h_occupancy_etaphi[caloLyr][ti]->Fill(celleta,cellphi);
          
          if(m_fillEtaOccupancy[caloLyrNS][ti]) m_h_occupancy_eta[caloLyr][ti]->Fill(celleta); 

          if(m_fillPhiOccupancy[caloLyrNS][ti]) m_h_occupancy_phi[caloLyr][ti]->Fill(cellphi); 

          if(m_fillEtaPhiTotalEnergy[caloLyrNS][ti]) m_h_totalEnergy_etaphi[caloLyr][ti]->Fill(celleta,cellphi,cellen); 

          if(m_doEtaPhiEnergyProfile[caloLyrNS][ti]) {
            if(m_doDatabaseNoiseVsEtaPhi) {
                m_h_energyProfile_etaphi[caloLyr][ti]->Fill(celleta,cellphi,cellen);
            }
            else {
              if( cellnoisedb != 0 ) {
                  if( !dbNormEnergyIsDefined ) {
                    dbNormEnergy = cellen*inv_cellnoisedb;
                    dbNormEnergyIsDefined = true;
                  }
                  m_h_energyProfile_etaphi[caloLyr][ti]->Fill(celleta,cellphi,dbNormEnergy);
                }
              }
          } // end of if(m_doEtaPhiEnergyProfile[caloLyrNS][ti]) 

          if(m_fillEtaPhiTotalQuality[caloLyrNS][ti]) m_h_totalQuality_etaphi[caloLyr][ti]->Fill(celleta,cellphi,cellquality); 

          if(m_fillEtaPhiQualityOccupancy[caloLyrNS][ti]) {
            if(cellquality > m_qualityThresholds[caloLyrNS][ti]) {
                m_h_poorQualityOccupancy_etaphi[caloLyr][ti]->Fill(celleta,cellphi);
            }
          }

          if(m_fillEtaPhiTotalTime[caloLyrNS][ti]) {
            m_h_totalTime_etaphi[caloLyr][ti]->Fill(celleta,cellphi,celltime);
          }

          if(m_fillEtaPhiTimeOccupancy[caloLyrNS][ti]) {
            if( fabs(celltime) > m_timeThresholds[caloLyrNS][ti]) {
              m_h_poorTimeOccupancy_etaphi[caloLyr][ti]->Fill(celleta,cellphi);
            }
          }

       } // end of if(masksel || (!m_doMaskingOnline) )

       if( (maskbadcell || masknoconditions) && m_doInverseMasking && m_doMaskingOnline ) {

            if(m_fillEtaPhiOccupancy[caloLyrNS][ti]) m_h_occupancy_etaphi_imask[caloLyr][ti]->Fill(celleta,cellphi); 

            if(m_fillEtaOccupancy[caloLyrNS][ti]) m_h_occupancy_eta_imask[caloLyr][ti]->Fill(celleta); 

            if(m_fillPhiOccupancy[caloLyrNS][ti])  m_h_occupancy_phi_imask[caloLyr][ti]->Fill(cellphi);
       } // end of  if( (maskbadcell || masknoconditions) && m_doInverseMasking && m_doMaskingOnline ) 
    
    } // end of if(threshIsPassed)
 } // end of ti loop

 // non multi-threshold histograms that will be masked online when masking is activated (cannot be masked in finalize):
 if(masksel && m_passBeamBackgroundRemoval) {
   // 1D Energy distribution:
   if (m_doUnnormalized1DEnergy) m_h_energy[caloLyr]->Fill(cellen); 

   //Noise Plots: (plots that work best with empty triggers)
   if (m_fillNoThreshNoisePlots && m_doDBNormalized1DEnergy) {// 18/4:Bug fix by Benjamin Trocme
      // Energy normalized to database noise:
      if( dbNormEnergyIsDefined ){
         m_h_dbNoiseNormalizedEnergy[caloLyr]->Fill(dbNormEnergy);
      }
      else if (cellnoisedb != 0) {
        m_h_dbNoiseNormalizedEnergy[caloLyr]->Fill(cellen*inv_cellnoisedb);
      }
   }

   // Time vs Energy:
   if (m_doEnergyVsTime){
     if(celltqavailable)  m_h_energyVsTime[caloLyr]->Fill(celltime,cellen); 
   
     if((celltqavailable) && (cellen > 1000) && (caloLyr == EMBPA)) m_h_energyVsTime_DifferThresholds[caloLyr]->Fill(celltime);
     if((celltqavailable) && (cellen > 1000) && (caloLyr == EMBPC)) m_h_energyVsTime_DifferThresholds[caloLyr]->Fill(celltime);
     
     if((celltqavailable) && (cellen > 1000) && (caloLyr == EMB1A)) m_h_energyVsTime_DifferThresholds[caloLyr]->Fill(celltime);
     if((celltqavailable) && (cellen > 1000) && (caloLyr == EMB1C)) m_h_energyVsTime_DifferThresholds[caloLyr]->Fill(celltime);
     
     if((celltqavailable) && (cellen > 3000) && (caloLyr == EMB2A)) m_h_energyVsTime_DifferThresholds[caloLyr]->Fill(celltime);
     if((celltqavailable) && (cellen > 3000) && (caloLyr == EMB2C)) m_h_energyVsTime_DifferThresholds[caloLyr]->Fill(celltime);
     
     if((celltqavailable) && (cellen > 1500) && (caloLyr == EMB3A)) m_h_energyVsTime_DifferThresholds[caloLyr]->Fill(celltime);
     if((celltqavailable) && (cellen > 1500) && (caloLyr == EMB3C)) m_h_energyVsTime_DifferThresholds[caloLyr]->Fill(celltime);
     
     if((celltqavailable) && (cellen > 1500) && (caloLyr == EMECPA)) m_h_energyVsTime_DifferThresholds[caloLyr]->Fill(celltime);
     if((celltqavailable) && (cellen > 1500) && (caloLyr == EMECPC)) m_h_energyVsTime_DifferThresholds[caloLyr]->Fill(celltime);
     
     if((celltqavailable) && (cellen > 1000) && (caloLyr == EMEC1A)) m_h_energyVsTime_DifferThresholds[caloLyr]->Fill(celltime);
     if((celltqavailable) && (cellen > 1000) && (caloLyr == EMEC1C)) m_h_energyVsTime_DifferThresholds[caloLyr]->Fill(celltime);
     
     if((celltqavailable) && (cellen > 3000) && (caloLyr == EMEC2A)) m_h_energyVsTime_DifferThresholds[caloLyr]->Fill(celltime);
     if((celltqavailable) && (cellen > 3000) && (caloLyr == EMEC2C)) m_h_energyVsTime_DifferThresholds[caloLyr]->Fill(celltime);
     
     if((celltqavailable) && (cellen > 2000) && (caloLyr == EMEC3A)) m_h_energyVsTime_DifferThresholds[caloLyr]->Fill(celltime);
     if((celltqavailable) && (cellen > 2000) && (caloLyr == EMEC3C)) m_h_energyVsTime_DifferThresholds[caloLyr]->Fill(celltime);
     
     if((celltqavailable) && (cellen > 3500) && (caloLyr == HEC0A)) m_h_energyVsTime_DifferThresholds[caloLyr]->Fill(celltime);
     if((celltqavailable) && (cellen > 3500) && (caloLyr == HEC0C)) m_h_energyVsTime_DifferThresholds[caloLyr]->Fill(celltime);
     
     if((celltqavailable) && (cellen > 3500) && (caloLyr == HEC1A)) m_h_energyVsTime_DifferThresholds[caloLyr]->Fill(celltime);
     if((celltqavailable) && (cellen > 3500) && (caloLyr == HEC1C)) m_h_energyVsTime_DifferThresholds[caloLyr]->Fill(celltime);
     
     if((celltqavailable) && (cellen > 3500) && (caloLyr == HEC2A)) m_h_energyVsTime_DifferThresholds[caloLyr]->Fill(celltime);
     if((celltqavailable) && (cellen > 3500) && (caloLyr == HEC2C)) m_h_energyVsTime_DifferThresholds[caloLyr]->Fill(celltime);
     
     if((celltqavailable) && (cellen > 3500) && (caloLyr == HEC3A)) m_h_energyVsTime_DifferThresholds[caloLyr]->Fill(celltime);
     if((celltqavailable) && (cellen > 3500) && (caloLyr == HEC3C)) m_h_energyVsTime_DifferThresholds[caloLyr]->Fill(celltime);
     
     if((celltqavailable) && (cellen > 1000) && (caloLyr == FCAL1A)) m_h_energyVsTime_DifferThresholds[caloLyr]->Fill(celltime);
     if((celltqavailable) && (cellen > 1000) && (caloLyr == FCAL1C)) m_h_energyVsTime_DifferThresholds[caloLyr]->Fill(celltime);
     
     if((celltqavailable) && (cellen > 1000) && (caloLyr == FCAL2A)) m_h_energyVsTime_DifferThresholds[caloLyr]->Fill(celltime);
     if((celltqavailable) && (cellen > 1000) && (caloLyr == FCAL2C)) m_h_energyVsTime_DifferThresholds[caloLyr]->Fill(celltime);
     
     if((celltqavailable) && (cellen > 1000) && (caloLyr == FCAL3A)) m_h_energyVsTime_DifferThresholds[caloLyr]->Fill(celltime);
     if((celltqavailable) && (cellen > 1000) && (caloLyr == FCAL3C)) m_h_energyVsTime_DifferThresholds[caloLyr]->Fill(celltime);
   }


 } // end of if(masksel)

 if(m_doInverseMasking && m_passBeamBackgroundRemoval) {
   if(maskbadcell || masknoconditions) {
     // 1D Energy Distribution:
     if (m_doUnnormalized1DEnergy) m_h_energy_imask[caloLyr]->Fill(cellen); 

     // Time vs Energy:
     if(celltqavailable) m_h_energyVsTime_imask[caloLyr]->Fill(celltime,cellen); 
     
   }
 } // end of if(m_doInverseMasking)


// ATH_MSG_INFO("CaloCellVecMon::fillLarHists() is done");

}

void CaloCellVecMon::maskMissCell(){

// ATH_MSG_INFO("CaloCellVecMon::maskMissCell() starts");
 for(int ilyr = 0; ilyr != MAXLAYER; ilyr++) {
   if(m_h_missingCellMask[ilyr] == 0) continue;
      
   int nbinsx = m_h_missingCellMask[ilyr]->GetNbinsX();
   int nbinsy = m_h_missingCellMask[ilyr]->GetNbinsY();
      
   for(int ix = 1; ix <= nbinsx; ix++) {
    for(int iy = 1; iy <= nbinsy; iy++) {
      int bin = m_h_missingCellMask[ilyr]->GetBin(ix,iy);
	  
	//This bin was does not correspond to a cell that was read out, it should be marked where possible
	if( m_h_missingCellMask[ilyr]->GetBinContent(bin) == 0 ) {
	   int ilyrns = ilyr / 2;
	   for(int ti = 0; ti < m_nThresholds[ilyrns]; ti++) {
	      if(!m_maskEmptyBins[ilyrns][ti]) continue;
	   
	      if(m_fillEtaPhiOccupancy[ilyrns][ti]) {
		m_h_occupancy_etaphi[ilyr][ti]->SetBinContent(bin,-1.0);
		m_h_occupancy_etaphi[ilyr][ti]->SetBinError(bin,0);
	      }
	      if(m_doEtaPhiEnergyProfile[ilyrns][ti]) {
		m_h_energyProfile_etaphi[ilyr][ti]->SetBinEntries(bin,-1.0);
	      }
	      if(m_fillEtaPhiTotalEnergy[ilyrns][ti]) {
		m_h_totalEnergy_etaphi[ilyr][ti]->SetBinContent(bin,-1.0);
		m_h_totalEnergy_etaphi[ilyr][ti]->SetBinError(bin,0);
	      }
	      if(m_fillEtaPhiQualityOccupancy[ilyrns][ti]) {
		m_h_poorQualityOccupancy_etaphi[ilyr][ti]->SetBinContent(bin,-1.0);
		m_h_poorQualityOccupancy_etaphi[ilyr][ti]->SetBinError(bin,0);
	      }
	      if(m_fillEtaPhiTotalQuality[ilyrns][ti]) {
		m_h_totalQuality_etaphi[ilyr][ti]->SetBinContent(bin,-1.0);
		m_h_totalQuality_etaphi[ilyr][ti]->SetBinError(bin,0);
	      }
	      if(m_fillEtaPhiTimeOccupancy[ilyrns][ti]) {
		m_h_poorTimeOccupancy_etaphi[ilyr][ti]->SetBinContent(bin,-1.0);
		m_h_poorTimeOccupancy_etaphi[ilyr][ti]->SetBinError(bin,0);
	      }
	      if(m_fillEtaPhiTotalTime[ilyrns][ti]) {
		m_h_totalTime_etaphi[ilyr][ti]->SetBinContent(bin,-1.0);
		m_h_totalTime_etaphi[ilyr][ti]->SetBinError(bin,0);
	      }

	   } // end of ti loop

	}  // end of if( m_h_missingCellMask[ilyr]->GetBinContent(bin) == 0 )

    } // end of iy loop
   }  // end of ix lopp

 } // end of ilyr loop

// ATH_MSG_INFO("CaloCellVecMon::maskMissCell() is done ");

}

//////////////////////////////////////////////////////////
StatusCode CaloCellVecMon::procHistograms( ){  

//  ATH_MSG_INFO("CaloCellVecMon procHistograms() is starting");

  StatusCode sc = StatusCode::SUCCESS;

//  Interval_t theinterval = run;
//  bool isNewTimeGran = checkTimeGran( isNewEventsBlock, isNewLumiBlock,  isNewRun, theinterval);

//  if(m_isOnline) isNewTimeGran = (isNewEventsBlock || isNewLumiBlock || isNewRun || newLowStatInterval );

//  if( isNewTimeGran && !isNewRun ) {
    sc  =realProcHistograms();
//  }
  
//  ATH_MSG_INFO("CaloCellVecMon procHistograms() is done");
  return sc;
}

StatusCode CaloCellVecMon::realProcHistograms(){  

//    ATH_MSG_INFO("CaloCellVecMon realProcHistograms() is starting");

    StatusCode sc = StatusCode::SUCCESS;

    processTileHists(); // khadeejah comment out
   
   // bookCaloLayers(); //Khadeejah comment out
  
  //  setBookSwitch();  //Khadeejah comment out

    if(m_maskKnownBadChannels || m_maskNoCondChannels) m_procimask = m_doInverseMasking;
    else m_procimask = false; 

/*    if ( m_maskKnownBadChannels || m_maskNoCondChannels ){
       if(!m_doMaskingOnline) {
         //Book inverse masked histograms that would have been booked and filled in fillHistograms if
         // doMaskingOnline was on and which can be constructed here.
         if(m_doInverseMasking) {
           if(m_bookProcHists) bookImaskHists();
         }
         maskBadChanHists();
       }
    }*/

  //  if(m_bookProcHists) bookProcHists();  //Khadeejah Comment out

    processHists(); // Khadeejah comment out

    if(m_procimask) processMaskHists();


    /*if(m_maskKnownBadChannels || m_maskNoCondChannels){
      if(!m_doMaskingOnline) {
         if(m_doInverseMasking)  {
          if(m_ifSaveAndDeleteHists)  sc = deleteImaskHists();
          if(sc.isFailure()){
           ATH_MSG_ERROR("fail in deleteImaskHists ");
           return sc;
          }
         }
      } 
    }*/

  /*  if(m_ifSaveAndDeleteHists)  sc=deleteProcHists(); // Khadeejah comment out and declear it under StatusCode CaloCellVecMon::deleteHistograms(){
    if(sc.isFailure()){
     ATH_MSG_ERROR("fail in deleteProcHists ");
     return sc;
    }*/


  //  deleteBookSwitch();
 
   // deleteCaloLayers();

    //m_bookProcHists = m_ifSaveAndDeleteHists;

//    ATH_MSG_INFO("CaloCellVecMon realProcHistograms() is done");
    return sc;
}

void CaloCellVecMon::processTileHists(){
//  ATH_MSG_INFO("processTileHists() starts.");  
  getRMSdevNoise(m_h_CellsRMSEta,m_h_CellsNoiseEta,m_h_CellsRMSdivDBnoiseEta);
  getRMSdevNoise(m_h_CellsRMSPhi,m_h_CellsNoisePhi,m_h_CellsRMSdivDBnoisePhi);
//  ATH_MSG_INFO("processTileHists() is done.");  
}


void CaloCellVecMon::bookImaskHists(){
   
//   ATH_MSG_INFO("bookImaskHists() starts .");  
   book2DEtaPhiHists(m_h_occupancy_etaphi_imask,Occupancy_2d,"CellOccupancyVsEtaPhi_%s_%s_inverseMasked",
                     "No. of events  in (#eta,#phi) in %s - %s Inverse Masked",m_fillEtaPhiOccupancy,
                     m_tempEtaPhiOccupancy);
   book1DEtaPhiHists(m_h_occupancy_eta_imask,Occupancy_1d,"CellOccupancyVsEta_%s_%s_inverseMasked",
                     "No. of Events in #eta for %s - %s Inverse Masked","e",m_fillEtaOccupancy,
                     m_doPercentageOccupancy);
   book1DEtaPhiHists(m_h_occupancy_phi_imask,Occupancy_1d,"CellOccupancyVsPhi_%s_%s_inverseMasked",
                     "No. of Events in #phi for %s - %s Inverse Masked","p",m_fillPhiOccupancy,
                     m_doPercentageOccupancy);

   book2DEtaPhiHists(m_h_energyProfile_etaphi_imask,Temporary,"TempEnergyNoiseTProfile_%s_%s_inverseMasked",
                     "Average Cell Energy vs (#eta,#phi) in %s - %s Inverse Masked",m_doEtaPhiEnergyProfile);
   book2DEtaPhiHists(m_h_totalEnergy_etaphi_imask,TotalEnergy,"TotalEnergyVsEtaPhi_%s_%s_inverseMasked",
                     "Total Cell Energy vs (#eta,#phi) in %s - %s Inverse Masked",m_fillEtaPhiTotalEnergy,
                     m_tempEtaPhiTotalEnergy);

//   ATH_MSG_INFO("bookImaskHists() is done .");  
}

StatusCode CaloCellVecMon::deleteImaskHists(){
//  ATH_MSG_INFO("deleteImaskHists() starts .");  
  ATH_CHECK( saveAndDeleteHistsInLayers(m_h_occupancy_etaphi_imask,Occupancy_2d,m_fillEtaPhiOccupancy,m_tempEtaPhiOccupancy) );
  ATH_CHECK( saveAndDeleteHistsInLayers(m_h_occupancy_eta_imask,Occupancy_1d,m_fillEtaOccupancy,m_doPercentageOccupancy) );
  ATH_CHECK( saveAndDeleteHistsInLayers(m_h_occupancy_phi_imask,Occupancy_1d,m_fillPhiOccupancy,m_doPercentageOccupancy) );
  ATH_CHECK( saveAndDeleteHistsInLayers(m_h_energyProfile_etaphi_imask,Temporary,m_doEtaPhiEnergyProfile) );
  ATH_CHECK( saveAndDeleteHistsInLayers(m_h_totalEnergy_etaphi_imask,TotalEnergy,m_fillEtaPhiTotalEnergy, m_tempEtaPhiTotalEnergy) );
//  ATH_MSG_INFO("deleteImaskHists() is done .");  
  return StatusCode::SUCCESS;
} 

void CaloCellVecMon::setBookSwitch() {
   for(int ilyrns = EMBPNS; ilyrns < MAXLYRNS; ilyrns++) {
        int nThr = m_nThresholds[ilyrns];
        m_procPercentOccEtaPhi[ilyrns] = new bool[nThr];
        m_procPercentOccEta[ilyrns] = new bool[nThr];
        m_procPercentOccPhi[ilyrns] = new bool[nThr];
        m_procAbsOccEtaPhi[ilyrns] = new bool[nThr];
        m_procAbsOccEta[ilyrns] = new bool[nThr];
        m_procAbsOccPhi[ilyrns] = new bool[nThr];
        m_doEtaPhiAvgEnNoTh[ilyrns] = new bool[nThr];
        m_doEtaPhiAvgEnTh[ilyrns] = new bool[nThr];

        for(int ti = 0; ti < nThr; ti++) {
          bool doPercentageOccupancy  = m_doPercentageOccupancy[ilyrns][ti];
          bool procEtaPhiOccupancy = m_procEtaPhiOccupancy[ilyrns][ti];
          bool procEtaOccupancy    = m_procEtaOccupancy[ilyrns][ti];
          bool procPhiOccupancy    = m_procPhiOccupancy[ilyrns][ti];
          m_procPercentOccEtaPhi[ilyrns][ti] = (procEtaPhiOccupancy && doPercentageOccupancy);
          m_procPercentOccEta[ilyrns][ti]    = (procEtaOccupancy    && doPercentageOccupancy);
          m_procPercentOccPhi[ilyrns][ti]    = (procPhiOccupancy    && doPercentageOccupancy);
          m_procAbsOccEtaPhi[ilyrns][ti]     = (procEtaPhiOccupancy && !doPercentageOccupancy);
          m_procAbsOccEta[ilyrns][ti]        = (procEtaOccupancy    && !doPercentageOccupancy);
          m_procAbsOccPhi[ilyrns][ti]        = (procPhiOccupancy    && !doPercentageOccupancy);

          if( m_doEtaPhiAverageEnergy[ilyrns][ti] ) {
            if( m_thresholdDirection[ilyrns][ti] == NONE ) {
              m_doEtaPhiAvgEnNoTh[ilyrns][ti] = true;
              m_doEtaPhiAvgEnTh[ilyrns][ti] = false;
            }
            else {
              m_doEtaPhiAvgEnNoTh[ilyrns][ti] = false;
              m_doEtaPhiAvgEnTh[ilyrns][ti] = true;
            }
          }
          else {
            m_doEtaPhiAvgEnNoTh[ilyrns][ti] = false;
            m_doEtaPhiAvgEnTh[ilyrns][ti] = false;
          }

        } // end of ti loop
   } // end of ilyrns loop
  
}

void CaloCellVecMon::deleteBookSwitch(){
  for(int ilyrns = 0; ilyrns < MAXLYRNS;ilyrns++) {
    delete [] m_doEtaPhiAvgEnTh[ilyrns];
    delete [] m_doEtaPhiAvgEnNoTh[ilyrns];

    delete [] m_procPercentOccEtaPhi[ilyrns];
    delete [] m_procPercentOccEta[ilyrns];
    delete [] m_procPercentOccPhi[ilyrns];
    delete [] m_procAbsOccEtaPhi[ilyrns];
    delete [] m_procAbsOccEta[ilyrns];
    delete [] m_procAbsOccPhi[ilyrns];
  }

}


void CaloCellVecMon::maskBadChanHists(){
//        ATH_MSG_INFO("maskBadChanHists() starts .");  
	uint32_t bitsToMask = buildBitMask(m_problemNamesToMask);
	
	if (m_doInverseMasking) {

	  for (int ilyr=EMBPA; ilyr<MAXLAYER; ilyr++) {
	    int ilyrns = ilyr/2;
	    const int NTHR = m_nThresholds[ilyrns];
	    for(int ti=0; ti<NTHR; ti++) {
	      
	      if(m_fillEtaPhiOccupancy[ilyrns][ti]) {
		maskHistogram(m_h_occupancy_etaphi[ilyr][ti], m_h_badChannels_etaphi[ilyr], bitsToMask, m_h_occupancy_etaphi_imask[ilyr][ti]);
		if(m_fillEtaOccupancy[ilyrns][ti]) {
		  makeEtaHist(m_h_occupancy_etaphi[ilyr][ti], m_h_occupancy_eta[ilyr][ti]);
		  makeEtaHist(m_h_occupancy_etaphi_imask[ilyr][ti], m_h_occupancy_eta_imask[ilyr][ti]);
		}
		if(m_fillPhiOccupancy[ilyrns][ti]) {
		  makePhiHist(m_h_occupancy_etaphi[ilyr][ti], m_h_occupancy_phi[ilyr][ti]);
		  makePhiHist(m_h_occupancy_etaphi_imask[ilyr][ti], m_h_occupancy_phi_imask[ilyr][ti]);
		}
	      }
	      if(m_doEtaPhiEnergyProfile[ilyrns][ti]) {
		maskHistogram(m_h_energyProfile_etaphi[ilyr][ti], m_h_badChannels_etaphi[ilyr], bitsToMask, m_h_energyProfile_etaphi_imask[ilyr][ti]);
	      }
	      if(m_fillEtaPhiTotalEnergy[ilyrns][ti]) {
		maskHistogram(m_h_totalEnergy_etaphi[ilyr][ti], m_h_badChannels_etaphi[ilyr], bitsToMask, m_h_totalEnergy_etaphi_imask[ilyr][ti]);
	      }

	    } // end of ti loop  
	  } // end of ilyr loop
	} // end of if (m_doInverseMasking)
	else { //Just do masking, no inverse masking:
	  for (int ilyr=EMBPA; ilyr<MAXLAYER; ilyr++) {
	    int ilyrns = ilyr/2;
	    const int NTHR = m_nThresholds[ilyrns];
	    for(int ti=0; ti<NTHR; ti++) {
	      if(m_fillEtaPhiOccupancy[ilyrns][ti]) {
		maskHistogram(m_h_occupancy_etaphi[ilyr][ti], m_h_badChannels_etaphi[ilyr], bitsToMask );
		if(m_fillEtaOccupancy[ilyrns][ti]) {
		  makeEtaHist(m_h_occupancy_etaphi[ilyr][ti], m_h_occupancy_eta[ilyr][ti]);
		}
		if(m_fillPhiOccupancy[ilyrns][ti]) {
		  makePhiHist(m_h_occupancy_etaphi[ilyr][ti], m_h_occupancy_phi[ilyr][ti]);
		}
	      }
	      if(m_doEtaPhiEnergyProfile[ilyrns][ti]) {
		maskHistogram(m_h_energyProfile_etaphi[ilyr][ti], m_h_badChannels_etaphi[ilyr], bitsToMask);
	      }
	      if(m_fillEtaPhiTotalEnergy[ilyrns][ti]) {
		maskHistogram(m_h_totalEnergy_etaphi[ilyr][ti], m_h_badChannels_etaphi[ilyr], bitsToMask);
	      }
	    }  // end of ti loop
	  } // end of ilyr loop
	} // end of else
//        ATH_MSG_INFO("maskBadChanHists() is done .");  
}

void CaloCellVecMon::bookProcHists(){
//      ATH_MSG_INFO("bookProcHists() starts .");  

      book2DEtaPhiHists(m_h_percentOccupancy_etaphi,PercentOccupancy_2d,"CellOccupancyVsEtaPhi_%s_%s",
			"Percentage of events in (#eta,#phi) for %s - %s",m_procPercentOccEtaPhi);
      book1DEtaPhiHists(m_h_percentOccupancy_eta,PercentOccupancy_1d,"CellOccupancyVsEta_%s_%s",
			"Percentage of events in #eta for %s - %s","e",m_procPercentOccEta);
      book1DEtaPhiHists(m_h_percentOccupancy_phi,PercentOccupancy_1d,"CellOccupancyVsPhi_%s_%s",
			"Percentage of events in #phi for %s - %s","p",m_procPercentOccPhi);
      
      book2DEtaPhiHists(m_h_occupancy_etaphi,Occupancy_2d,"CellOccupancyVsEtaPhi_%s_%s",
			"No. of events in (#eta,#phi) for %s - %s",m_procAbsOccEtaPhi);
      book1DEtaPhiHists(m_h_occupancy_eta,Occupancy_1d,"CellOccupancyVsEta_%s_%s",
			"No. of events in #eta for %s - %s","e",m_procAbsOccEta);
      book1DEtaPhiHists(m_h_occupancy_phi,Occupancy_1d,"CellOccupancyVsPhi_%s_%s",
			"No. of events in #phi for %s - %s","p",m_procAbsOccPhi);
    
      book2DEtaPhiHists(m_h_averageEnergy_etaphi,AvgEnergy,"CellAvgEnergyVsEtaPhi_%s_%s",
			"Average Cell Energy vs (#eta,#phi) for %s - %s",m_doEtaPhiAvgEnTh);
      book2DEtaPhiHists(m_h_averageEnergy_etaphi,AvgEnergyNoTh,"CellAvgEnergyVsEtaPhi_%s_%s",
			"Average Cell Energy vs (#eta,#phi) for %s - %s",m_doEtaPhiAvgEnNoTh);

      book2DEtaPhiHists(m_h_totalEnergy_etaphi,TotalEnergy,"CellTotalEnergyVsEtaPhi_%s_%s",
			"Total Cell Energy vs (#eta,#phi) for %s - %s",m_procEtaPhiTotalEnergy);
      book2DEtaPhiHists(m_h_noise_etaphi,Noise,"CellNoiseVsEtaPhi_%s_%s","Cell Energy RMS vs (#eta,#phi) for %s - %s",
			m_procEtaPhiEnergyRMS);
      book2DEtaPhiHists(m_h_deviationFromDBnoise_etaphi,NoiseVsDB,"EnergyRMSvsDB_%s_%s",
			"Percentage Deviation: (Energy RMS - #sigma_{noise}^{database})/#sigma_{noise}^{database} vs (#eta,#phi) for %s - %s",
			m_procEtaPhiDeviationFromDBnoise);
      book2DEtaPhiHists(m_h_averageQuality_etaphi,AvgQuality,"AverageQualityVsEtaPhi_%s_%s","Average Cell Quality vs (#eta,#phi) for %s - %s",
			m_procEtaPhiAverageQuality);
      book2DEtaPhiHists(m_h_averageTime_etaphi,AvgTime,"AverageTimeVsEtaPhi_%s_%s","Average Cell Time vs (#eta,#phi) for %s - %s",
			m_procEtaPhiAverageTime);
      book2DEtaPhiHists(m_h_fractionOverQth_etaphi,PoorQualityFraction,"fractionOverQthVsEtaPhi_%s_%s",
			"Fraction of Events in %s with %s for which the Quality Factor Exceeds QFACTOR",
			m_procEtaPhiFractionOverQth);
      book2DEtaPhiHists(m_h_fractionPastTth_etaphi,PoorTimeFraction,"fractionPastTthVsEtaPhi_%s_%s",
			"Fraction of Events in %s with %s for which the Time is Further than TIME from Zero",
			m_procEtaPhiFractionPastTth);
    
      if( m_procimask ) {
	book2DEtaPhiHists(m_h_percentOccupancy_etaphi_imask,PercentOccupancy_2d,"CellOccupancyVsEtaPhi_%s_%s_inverseMasked",
			  "Percentage of events in (#eta,#phi) for %s - %s Inverse Masked",m_procPercentOccEtaPhi);
	book1DEtaPhiHists(m_h_percentOccupancy_eta_imask,PercentOccupancy_1d,"CellOccupancyVsEta_%s_%s_inverseMasked",
			  "Percentage of events in #eta for %s - %s Inverse Masked","e",m_procPercentOccEta);
	book1DEtaPhiHists(m_h_percentOccupancy_phi_imask,PercentOccupancy_1d,"CellOccupancyVsPhi_%s_%s_inverseMasked",
			  "Percentage of events in #phi for %s - %s Inverse Masked","p",m_procPercentOccPhi);
	
	book2DEtaPhiHists(m_h_occupancy_etaphi_imask,Occupancy_2d,"CellOccupancyVsEtaPhi_%s_%s_inverseMasked",
			  "No. of events in (#eta,#phi) for %s - %s Inverse Masked",m_procAbsOccEtaPhi);
	book1DEtaPhiHists(m_h_occupancy_eta_imask,Occupancy_1d,"CellOccupancyVsEta_%s_%s_inverseMasked",
			  "No. of events in #eta for %s - %s Inverse Masked","e",m_procAbsOccEta);
	book1DEtaPhiHists(m_h_occupancy_phi_imask,Occupancy_1d,"CellOccupancyVsPhi_%s_%s_inverseMasked",
			  "No. of events in #phi for %s - %s Inverse Masked","p",m_procAbsOccPhi);
     
	book2DEtaPhiHists(m_h_noise_etaphi_imask,Noise,"CellNoiseVsEtaPhi_%s_%s_inverseMasked",
			  "Cell Energy RMS vs (#eta,#phi) in %s - %s Inverse Masked, ", m_procEtaPhiEnergyRMS);
	book2DEtaPhiHists(m_h_averageEnergy_etaphi_imask,AvgEnergy,"CellAvgEnergyVsEtaPhi_%s_%s_inverseMasked",
			  "Average Cell Energy vs (#eta,#phi) in %s - %s Inverse Masked", m_doEtaPhiAvgEnTh);
	book2DEtaPhiHists(m_h_averageEnergy_etaphi_imask,AvgEnergyNoTh,"CellAvgEnergyVsEtaPhi_%s_%s_inverseMasked",
			  "Average Cell Energy vs (#eta,#phi) in %s - %s Inverse Masked", m_doEtaPhiAvgEnNoTh);
	book2DEtaPhiHists(m_h_totalEnergy_etaphi_imask,TotalEnergy,"CellTotalEnergyVsEtaPhi_%s_%s",
			  "Total Cell Energy vs (#eta,#phi) for %s - %s",m_procEtaPhiTotalEnergy);
      }
//      ATH_MSG_INFO("bookProcHists() is done .");  
 
}

void CaloCellVecMon::processHists(){
//    ATH_MSG_INFO("processHists() starts .");  

    for(int ilyr = 0; ilyr<MAXLAYER; ilyr++){
      int ilyrns = ilyr/2;
      for(int ti = 0; ti < m_nThresholds[ilyrns]; ti++) {
        
//        ATH_MSG_INFO("ilyr="<<ilyr<<";ti="<<ti);  
	if(m_doPercentageOccupancy[ilyrns][ti]) {
	  if(m_doEtaPhiEnergyProfile[ilyrns][ti]) {
	    //Fill Noise  from energy profile, aswell as averageEnergy and occupancy, if they are defined for this threshold:
	    // Note, we are filling the percentOccupancy plots with the unnormalized occupancy temporarily, so we can use the
            // unnormalized occupancy for normalizing the Q factor and time plots.
	   
	    unpackTProfile2D(m_h_energyProfile_etaphi[ilyr][ti],m_h_noise_etaphi[ilyr][ti],
			     m_h_averageEnergy_etaphi[ilyr][ti],m_h_percentOccupancy_etaphi[ilyr][ti],m_h_totalEnergy_etaphi[ilyr][ti]);
	    
	    if(m_doEtaPhiAverageEnergy[ilyrns][ti]){
	      if(m_thresholdDirection[ilyrns][ti] == NONE) {
		setCorrectEntries(m_h_averageEnergy_etaphi,ilyr,ti);
	      }
	    }

	    //Use m_h_percentOccupancy here to get the raw occupancy, as it has yet to be normalized:
	    if(m_procEtaPhiAverageQuality[ilyrns][ti]) {
	      divideHistograms(m_h_totalQuality_etaphi[ilyr][ti],m_h_percentOccupancy_etaphi[ilyr][ti],m_h_averageQuality_etaphi[ilyr][ti]);
	    }
	    if(m_procEtaPhiAverageTime[ilyrns][ti]) {
//              ATH_MSG_INFO("divideHistograms to get m_h_averageTime_etaphi 1");  
	      divideHistograms(m_h_totalTime_etaphi[ilyr][ti],m_h_percentOccupancy_etaphi[ilyr][ti],m_h_averageTime_etaphi[ilyr][ti]);
	    }
	    if(m_procEtaPhiFractionOverQth[ilyrns][ti]) {
	      divideHistograms(m_h_poorQualityOccupancy_etaphi[ilyr][ti],m_h_percentOccupancy_etaphi[ilyr][ti],m_h_fractionOverQth_etaphi[ilyr][ti]);
	      std::string title = m_h_fractionOverQth_etaphi[ilyr][ti]->GetTitle();
	      size_t finding = title.find("QFACTOR");
	      if( finding != std::string::npos ) {
		char qthresh[64];
		sprintf(qthresh, "%0.f", m_qualityThresholds[ilyrns][ti]);
		title.replace(finding, 7, qthresh);
		m_h_fractionOverQth_etaphi[ilyr][ti]->SetTitle(title.c_str());
	      }
	    }
	    if(m_procEtaPhiFractionPastTth[ilyrns][ti]) {
	      divideHistograms(m_h_poorTimeOccupancy_etaphi[ilyr][ti],m_h_percentOccupancy_etaphi[ilyr][ti],m_h_fractionPastTth_etaphi[ilyr][ti]);
	      std::string title = m_h_fractionPastTth_etaphi[ilyr][ti]->GetTitle();
	      size_t finding = title.find("TIME");
	      if( finding != std::string::npos ) {
		char tthresh[64];
		sprintf(tthresh, "%0.f", m_timeThresholds[ilyrns][ti]);
		title.replace(finding, 4, tthresh);
		m_h_fractionPastTth_etaphi[ilyr][ti]->SetTitle(title.c_str());
	      }
	    }
	    
	    //Fill percentage occupancy with occupancy divided by number of events in this trigger (times 100)
	    if(m_useTrigger) {
//              ATH_MSG_INFO("m_nEventsPassingTrigger[ilyrns][ti]="<<m_nEventsPassingTrigger[ilyrns][ti]);
	      divideHistogram(m_h_percentOccupancy_etaphi[ilyr][ti], ((m_nEventsPassingTrigger[ilyrns][ti])*0.01), m_h_percentOccupancy_etaphi[ilyr][ti]);
	    }
	    else {
	    //  divideHistogram(m_h_percentOccupancy_etaphi[ilyr][ti], (m_eventsCounter*0.01), m_h_percentOccupancy_etaphi[ilyr][ti]);
//              ATH_MSG_INFO("m_nEventsPassingBeamBG[ilyrns][ti]="<<m_nEventsPassingBeamBG[ilyrns][ti]);
	      divideHistogram(m_h_percentOccupancy_etaphi[ilyr][ti], ((m_nEventsPassingBeamBG[ilyrns][ti])*0.01), m_h_percentOccupancy_etaphi[ilyr][ti]);
	    }
	  } // end of if(m_doEtaPhiEnergyProfile[ilyrns][ti]) 
	  else {
	    //m_fillEtaPhiOccupancy should always be true in the case (filled as Temporary), so we use m_h_occupancy_etaphi here
	    if(m_doEtaPhiAverageEnergy[ilyrns][ti]) {
	      //Fill average energy histogram with the result of dividing the total energy by the occupancy:
	      divideHistograms(m_h_totalEnergy_etaphi[ilyr][ti],m_h_occupancy_etaphi[ilyr][ti],m_h_averageEnergy_etaphi[ilyr][ti]);
              m_h_averageEnergy_etaphi[ilyr][ti]->GetZaxis()->SetLabelSize(0.02);
	      if(m_thresholdDirection[ilyrns][ti] == NONE) {
		setCorrectEntries(m_h_averageEnergy_etaphi,ilyr,ti);
	      }
	    } 
	    //Fill percentage occupancy with occupancy divided by number of events in this trigger (times 100)
	    if(m_useTrigger) {
//              ATH_MSG_INFO("m_nEventsPassingTrigger[ilyrns][ti]="<<m_nEventsPassingTrigger[ilyrns][ti]);
	      divideHistogram(m_h_occupancy_etaphi[ilyr][ti], ((m_nEventsPassingTrigger[ilyrns][ti])*0.01), m_h_percentOccupancy_etaphi[ilyr][ti]);
	    }
	    else {
//              ATH_MSG_INFO("m_nEventsPassingBeamBG[ilyrns][ti]="<<m_nEventsPassingBeamBG[ilyrns][ti]);
//	      divideHistogram(m_h_occupancy_etaphi[ilyr][ti], (m_eventsCounter*0.01), m_h_percentOccupancy_etaphi[ilyr][ti]);
	      divideHistogram(m_h_occupancy_etaphi[ilyr][ti], ((m_nEventsPassingBeamBG[ilyrns][ti])*0.01), m_h_percentOccupancy_etaphi[ilyr][ti]);
	    }

	    if(m_procEtaPhiAverageQuality[ilyrns][ti]) {
	      divideHistograms(m_h_totalQuality_etaphi[ilyr][ti],m_h_occupancy_etaphi[ilyr][ti],m_h_averageQuality_etaphi[ilyr][ti]);
	    }
	    if(m_procEtaPhiAverageTime[ilyrns][ti]) {
//              ATH_MSG_INFO("divideHistograms to get m_h_averageTime_etaphi 2");  
	      divideHistograms(m_h_totalTime_etaphi[ilyr][ti],m_h_occupancy_etaphi[ilyr][ti],m_h_averageTime_etaphi[ilyr][ti]);
	    }
	    if(m_procEtaPhiFractionOverQth[ilyrns][ti]) {
	      divideHistograms(m_h_poorQualityOccupancy_etaphi[ilyr][ti],m_h_occupancy_etaphi[ilyr][ti],m_h_fractionOverQth_etaphi[ilyr][ti]);
	      std::string title = m_h_fractionOverQth_etaphi[ilyr][ti]->GetTitle();
	      size_t finding = title.find("QFACTOR");
	      if( finding != std::string::npos ) {
		char qthresh[64];
		sprintf(qthresh, "%0.f", m_qualityThresholds[ilyrns][ti]);
		title.replace(finding, 7, qthresh);
		m_h_fractionOverQth_etaphi[ilyr][ti]->SetTitle(title.c_str());
	      }
	    }
	    if(m_procEtaPhiFractionPastTth[ilyrns][ti]) {
	      divideHistograms(m_h_poorTimeOccupancy_etaphi[ilyr][ti],m_h_occupancy_etaphi[ilyr][ti],m_h_fractionPastTth_etaphi[ilyr][ti]);
	      std::string title = m_h_fractionPastTth_etaphi[ilyr][ti]->GetTitle();
	      size_t finding = title.find("TIME");
	      if( finding != std::string::npos ) {
		char tthresh[64];
		sprintf(tthresh, "%0.f", m_timeThresholds[ilyrns][ti]);
		title.replace(finding, 4, tthresh);
		m_h_fractionPastTth_etaphi[ilyr][ti]->SetTitle(title.c_str());
	      }
	    }
	  } // end of else (m_doEtaPhiEnergyProfile==0)
	  
	  if(m_procEtaOccupancy[ilyrns][ti]) {
	    if(m_fillEtaOccupancy[ilyrns][ti]) {
	      //Normalize the 1D occupancy plot filled in fillhistograms
	      if(m_useTrigger) {
		divideHistogram(m_h_occupancy_eta[ilyr][ti],((m_nEventsPassingTrigger[ilyrns][ti])*0.01),m_h_percentOccupancy_eta[ilyr][ti]);
	      }
	      else {
//		divideHistogram(m_h_occupancy_eta[ilyr][ti],(m_eventsCounter*0.01),m_h_percentOccupancy_eta[ilyr][ti]);
		divideHistogram(m_h_occupancy_eta[ilyr][ti],((m_nEventsPassingBeamBG[ilyrns][ti])*0.01),m_h_percentOccupancy_eta[ilyr][ti]);
	      }
	    }
	    else  {
	      //Extract the 1D occupancy plot from the 2D plot:
	      makeEtaHist(m_h_percentOccupancy_etaphi[ilyr][ti],m_h_percentOccupancy_eta[ilyr][ti]);
	    }
	    //Now normailize to account for the different number of active cells in each eta bin:
	    divideHistograms(m_h_percentOccupancy_eta[ilyr][ti],m_h_nActiveCellsFirstEvent_eta[ilyr],m_h_percentOccupancy_eta[ilyr][ti]);
	    setCorrectEntries(m_h_percentOccupancy_eta,ilyr,ti);
	  }
	  
	  if(m_procPhiOccupancy[ilyrns][ti]) {
	    if(m_fillPhiOccupancy[ilyrns][ti]) {
	      //Normalize the 1D occupancy plot filled in fillhistograms
	      if (m_useTrigger) {
		divideHistogram(m_h_occupancy_phi[ilyr][ti],((m_nEventsPassingTrigger[ilyrns][ti])*0.01),m_h_percentOccupancy_phi[ilyr][ti]);
	      }
	      else {
	//	divideHistogram(m_h_occupancy_phi[ilyr][ti],(m_eventsCounter*0.01),m_h_percentOccupancy_phi[ilyr][ti]);
		divideHistogram(m_h_occupancy_phi[ilyr][ti],((m_nEventsPassingBeamBG[ilyrns][ti])*0.01),m_h_percentOccupancy_phi[ilyr][ti]);
	      }
	    }
	    else  {
	      //Extract the 1D occupancy plot from the 2D plot:
	      makePhiHist(m_h_percentOccupancy_etaphi[ilyr][ti],m_h_percentOccupancy_phi[ilyr][ti]);
	    }
	    //Now normailize to account for the different number of active cells in each phi bin:
	    divideHistograms(m_h_percentOccupancy_phi[ilyr][ti],m_h_nActiveCellsFirstEvent_phi[ilyr],m_h_percentOccupancy_phi[ilyr][ti]);
	    setCorrectEntries(m_h_percentOccupancy_phi,ilyr,ti);
	  }
	  
	  if(m_procEtaPhiOccupancy[ilyrns][ti]) {
	    setCorrectEntries(m_h_percentOccupancy_etaphi,ilyr,ti);
	  }
	} // end of if(m_doPercentageOccupancy[ilyrns][ti])
	else {
	  if(m_doEtaPhiEnergyProfile[ilyrns][ti]) {
	    //Fill Noise  from energy profile, aswell as averageEnergy and occupancy, if they are defined for this threshold:
	    unpackTProfile2D(m_h_energyProfile_etaphi[ilyr][ti],m_h_noise_etaphi[ilyr][ti],
			     m_h_averageEnergy_etaphi[ilyr][ti],m_h_occupancy_etaphi[ilyr][ti],m_h_totalEnergy_etaphi[ilyr][ti] );

#if 0
	    if(m_procEtaPhiEnergyRMS[ilyrns][ti]){
	    }
	    if(m_procEtaPhiTotalEnergy[ilyrns][ti]){
	    }
#endif
	    if(m_doEtaPhiAverageEnergy[ilyrns][ti]){
	      if(m_thresholdDirection[ilyrns][ti] == NONE) {
		setCorrectEntries(m_h_averageEnergy_etaphi,ilyr,ti);
	      }
	    }
	  }  // end of if(m_doEtaPhiEnergyProfile[ilyrns][ti]) 
	  else if(m_doEtaPhiAverageEnergy[ilyrns][ti]) {
	    //Fill average energy histogram with the result of dividing the total energy by the occupancy:
	    divideHistograms(m_h_totalEnergy_etaphi[ilyr][ti],m_h_occupancy_etaphi[ilyr][ti],m_h_averageEnergy_etaphi[ilyr][ti]);
	    if(m_thresholdDirection[ilyrns][ti] == NONE) {
	      setCorrectEntries(m_h_averageEnergy_etaphi,ilyr,ti);
	    }
	  } // end of else if(m_doEtaPhiAverageEnergy[ilyrns][ti]) 
	
	  if(m_procEtaOccupancy[ilyrns][ti]) {
	    makeEtaHist(m_h_occupancy_etaphi[ilyr][ti],m_h_occupancy_eta[ilyr][ti]);
	  }
	  if(m_procPhiOccupancy[ilyrns][ti]) {
	    makePhiHist(m_h_occupancy_etaphi[ilyr][ti],m_h_occupancy_phi[ilyr][ti]);
	  }
	  if(m_procEtaPhiAverageQuality[ilyrns][ti]) {
	    divideHistograms(m_h_totalQuality_etaphi[ilyr][ti],m_h_occupancy_etaphi[ilyr][ti],m_h_averageQuality_etaphi[ilyr][ti]);
	  }
	  if(m_procEtaPhiAverageTime[ilyrns][ti]) {
//            ATH_MSG_INFO("divideHistograms to get m_h_averageTime_etaphi 3");  
	    divideHistograms(m_h_totalTime_etaphi[ilyr][ti],m_h_occupancy_etaphi[ilyr][ti],m_h_averageTime_etaphi[ilyr][ti]);
	  }
	  if(m_procEtaPhiFractionOverQth[ilyrns][ti]) {
	    divideHistograms(m_h_poorQualityOccupancy_etaphi[ilyr][ti],m_h_occupancy_etaphi[ilyr][ti],m_h_fractionOverQth_etaphi[ilyr][ti]);
	    std::string title = m_h_fractionOverQth_etaphi[ilyr][ti]->GetTitle();
	    size_t finding = title.find("QFACTOR");
	    if( finding != std::string::npos ) {
	      char qthresh[64];
	      sprintf(qthresh, "%0.f", m_qualityThresholds[ilyrns][ti]);
	      title.replace(finding, 7, qthresh);
	      m_h_fractionOverQth_etaphi[ilyr][ti]->SetTitle(title.c_str());
	    }
	  }
	  if(m_procEtaPhiFractionPastTth[ilyrns][ti]) {
	    divideHistograms(m_h_poorTimeOccupancy_etaphi[ilyr][ti],m_h_occupancy_etaphi[ilyr][ti],m_h_fractionPastTth_etaphi[ilyr][ti]);
	    std::string title = m_h_fractionPastTth_etaphi[ilyr][ti]->GetTitle();
	    size_t finding = title.find("TIME");
	    if( finding != std::string::npos ) {
	      char tthresh[64];
	      sprintf(tthresh, "%0.f", m_timeThresholds[ilyrns][ti]);
	      title.replace(finding, 4, tthresh);
	      m_h_fractionPastTth_etaphi[ilyr][ti]->SetTitle(title.c_str());
	    }
	  }

	}  // end of else (m_doPercentageOccupancy[ilyrns][ti]==0)

	if(m_procEtaPhiDeviationFromDBnoise[ilyrns][ti]) {
	  if(m_doDatabaseNoiseVsEtaPhi) {
	    unpackToPercentageDifference(m_h_energyProfile_etaphi[ilyr][ti],m_h_dbnoise_etaphi[ilyr],m_h_deviationFromDBnoise_etaphi[ilyr][ti]);
	  }
	  else {
	    unpackToPercentageDifference(m_h_energyProfile_etaphi[ilyr][ti],m_h_deviationFromDBnoise_etaphi[ilyr][ti]);
	  }
	} // end of if(m_procEtaPhiDeviationFromDBnoise[ilyrns][ti])

      } // end of ti loop

    } // end of ilyr loop
//    ATH_MSG_INFO("processHists() is done .");  

}

void CaloCellVecMon::processMaskHists(){

//      ATH_MSG_INFO("processMaskHists() starts .");  
      for(int ilyr = 0; ilyr<MAXLAYER; ilyr++){
	int ilyrns = ilyr/2;
	for(int ti = 0; ti < m_nThresholds[ilyrns]; ti++) {
	  if(m_doPercentageOccupancy[ilyrns][ti]) { 
	    if(m_doEtaPhiEnergyProfile[ilyrns][ti]) {
	      //Fill inverse masked plots:
	      unpackTProfile2D(m_h_energyProfile_etaphi_imask[ilyr][ti],m_h_noise_etaphi_imask[ilyr][ti],
			       m_h_averageEnergy_etaphi_imask[ilyr][ti],m_h_percentOccupancy_etaphi_imask[ilyr][ti],
			       m_h_totalEnergy_etaphi_imask[ilyr][ti]);
	      
	      if(m_useTrigger) {
		divideHistogram(m_h_percentOccupancy_etaphi_imask[ilyr][ti], ((m_nEventsPassingTrigger[ilyrns][ti])*0.01), m_h_percentOccupancy_etaphi_imask[ilyr][ti]);
	      }
	      else {
//		divideHistogram(m_h_percentOccupancy_etaphi_imask[ilyr][ti], (m_eventsCounter*0.01), m_h_percentOccupancy_etaphi_imask[ilyr][ti]);
		divideHistogram(m_h_percentOccupancy_etaphi_imask[ilyr][ti], ((m_nEventsPassingBeamBG[ilyrns][ti])*0.01), m_h_percentOccupancy_etaphi_imask[ilyr][ti]);
	      }

	    } // end of if(m_doEtaPhiEnergyProfile[ilyrns][ti])
	    else {
	      if(m_useTrigger) {
                divideHistogram(m_h_occupancy_etaphi_imask[ilyr][ti], ((m_nEventsPassingTrigger[ilyrns][ti])*0.01), m_h_percentOccupancy_etaphi_imask[ilyr][ti]);
              }
              else {
        //        divideHistogram(m_h_occupancy_etaphi_imask[ilyr][ti], (m_eventsCounter*0.01), m_h_percentOccupancy_etaphi_imask[ilyr][ti]);
                divideHistogram(m_h_occupancy_etaphi_imask[ilyr][ti], ((m_nEventsPassingBeamBG[ilyrns][ti])*0.01), m_h_percentOccupancy_etaphi_imask[ilyr][ti]);
              }
	      
	      if (m_doEtaPhiAverageEnergy[ilyrns][ti]) {
		divideHistograms(m_h_totalEnergy_etaphi_imask[ilyr][ti],m_h_occupancy_etaphi_imask[ilyr][ti],
				 m_h_averageEnergy_etaphi_imask[ilyr][ti]);
		if(m_thresholdDirection[ilyrns][ti] == NONE) {
		  setCorrectEntries(m_h_averageEnergy_etaphi_imask,ilyr,ti);
		}
	      }
	    } // end of else (m_doEtaPhiEnergyProfile[ilyrns][ti]==0)

	    if(m_procEtaOccupancy[ilyrns][ti]) {
              if(m_fillEtaOccupancy[ilyrns][ti]) {
                //Normalize the 1D occupancy plot filled in fillhistograms                                                                                                                     
                if (m_useTrigger) {
                  divideHistogram(m_h_occupancy_eta_imask[ilyr][ti],((m_nEventsPassingTrigger[ilyrns][ti])*0.01),m_h_percentOccupancy_eta_imask[ilyr][ti]);
                }
                else {
         //         divideHistogram(m_h_occupancy_eta_imask[ilyr][ti],(m_eventsCounter*0.01),m_h_percentOccupancy_eta_imask[ilyr][ti]);
                  divideHistogram(m_h_occupancy_eta_imask[ilyr][ti],((m_nEventsPassingBeamBG[ilyrns][ti])*0.01),m_h_percentOccupancy_eta_imask[ilyr][ti]);
                }
              }
              else {
                //Extract the 1D occupancy plot from the 2D plot:                                                                                                                              
                makeEtaHist(m_h_percentOccupancy_etaphi_imask[ilyr][ti],m_h_percentOccupancy_eta_imask[ilyr][ti]);
              }
              //Now normailize to account for the different number of active cells in each eta bin:                                                                                            
              divideHistograms(m_h_percentOccupancy_eta_imask[ilyr][ti],m_h_nActiveCellsFirstEvent_eta[ilyr],m_h_percentOccupancy_eta_imask[ilyr][ti]);
              setCorrectEntries(m_h_percentOccupancy_eta_imask,ilyr,ti);
            } // end of if(m_procEtaOccupancy[ilyrns][ti])

	    if(m_procPhiOccupancy[ilyrns][ti]) {
	      if(m_fillPhiOccupancy[ilyrns][ti]) {
		//Normalize the 1D occupancy plot filled in fillhistograms                                                                                                            
		if (m_useTrigger) {
		  divideHistogram(m_h_occupancy_phi_imask[ilyr][ti],((m_nEventsPassingTrigger[ilyrns][ti])*0.01),m_h_percentOccupancy_phi_imask[ilyr][ti]);
		}
		else {
//		  divideHistogram(m_h_occupancy_phi_imask[ilyr][ti],(m_eventsCounter*0.01),m_h_percentOccupancy_phi_imask[ilyr][ti]);
		  divideHistogram(m_h_occupancy_phi_imask[ilyr][ti],((m_nEventsPassingBeamBG[ilyrns][ti])*0.01),m_h_percentOccupancy_phi_imask[ilyr][ti]);
		}
	      }
	      else {
		//Extract the 1D occupancy plot from the 2D plot:                                                                                                                     
		makePhiHist(m_h_percentOccupancy_etaphi_imask[ilyr][ti],m_h_percentOccupancy_phi_imask[ilyr][ti]);
	      }
	      //Now normailize to account for the different number of active cells in each phi bin:
	      divideHistograms(m_h_percentOccupancy_phi_imask[ilyr][ti],m_h_nActiveCellsFirstEvent_phi[ilyr],m_h_percentOccupancy_phi_imask[ilyr][ti]);
	      setCorrectEntries(m_h_percentOccupancy_phi_imask,ilyr,ti);
	    } // end of if(m_procPhiOccupancy[ilyrns][ti]) 

	    setCorrectEntries(m_h_percentOccupancy_etaphi_imask,ilyr,ti);
	  } // end of if(m_doPercentageOccupancy[ilyrns][ti])
	  else {
	    if(m_doEtaPhiEnergyProfile[ilyrns][ti]) {
	      //Fill inverse masked plots:
	      unpackTProfile2D(m_h_energyProfile_etaphi_imask[ilyr][ti],m_h_noise_etaphi_imask[ilyr][ti],
			       m_h_averageEnergy_etaphi_imask[ilyr][ti],m_h_occupancy_etaphi_imask[ilyr][ti],
			       m_h_totalEnergy_etaphi_imask[ilyr][ti]);
	    }
	    else if (m_doEtaPhiAverageEnergy[ilyrns][ti]) {
	      divideHistograms(m_h_totalEnergy_etaphi_imask[ilyr][ti],m_h_occupancy_etaphi_imask[ilyr][ti],
			       m_h_averageEnergy_etaphi_imask[ilyr][ti]);
	      if(m_thresholdDirection[ilyrns][ti] == NONE) {
		setCorrectEntries(m_h_averageEnergy_etaphi_imask,ilyr,ti);
	      }
	    }
	    if(m_procEtaOccupancy[ilyrns][ti]) {
	      makeEtaHist(m_h_occupancy_etaphi_imask[ilyr][ti],m_h_occupancy_eta_imask[ilyr][ti]);
	      setCorrectEntries(m_h_occupancy_eta_imask,ilyr,ti);
	    }
	    if(m_procPhiOccupancy[ilyrns][ti]) {
	      makePhiHist(m_h_occupancy_etaphi_imask[ilyr][ti],m_h_occupancy_phi_imask[ilyr][ti]);
	      setCorrectEntries(m_h_occupancy_phi_imask,ilyr,ti);
	    }
	    setCorrectEntries(m_h_occupancy_etaphi_imask,ilyr,ti);
	  } // end of else (m_doPercentageOccupancy[ilyrns][ti]==0) 

	} // end of ti loop

      } // end of ilyr loop
//      ATH_MSG_INFO("processMaskHists() is done .");  

}

  // Khadeejah move the definition after deleteHistograms() deleteLarMultThreHists(){
StatusCode CaloCellVecMon::deleteProcHists(){
//      ATH_MSG_INFO("deleteProcHists() starts .");  
  
      ATH_CHECK( saveAndDeleteHistsInLayers(m_h_percentOccupancy_etaphi,PercentOccupancy_2d, m_procPercentOccEtaPhi) );
      ATH_CHECK( saveAndDeleteHistsInLayers(m_h_percentOccupancy_eta,PercentOccupancy_1d,m_procPercentOccEta) );
      ATH_CHECK( saveAndDeleteHistsInLayers(m_h_percentOccupancy_phi,PercentOccupancy_1d,m_procPercentOccPhi) );
      ATH_CHECK( saveAndDeleteHistsInLayers(m_h_occupancy_etaphi,Occupancy_2d,m_procAbsOccEtaPhi) );
      ATH_CHECK( saveAndDeleteHistsInLayers(m_h_occupancy_eta,Occupancy_1d,m_procAbsOccEta) );
      ATH_CHECK( saveAndDeleteHistsInLayers(m_h_occupancy_phi,Occupancy_1d,m_procAbsOccPhi) );
      ATH_CHECK( saveAndDeleteHistsInLayers(m_h_averageEnergy_etaphi,AvgEnergy,m_doEtaPhiAvgEnTh) );
      ATH_CHECK( saveAndDeleteHistsInLayers(m_h_averageEnergy_etaphi,AvgEnergyNoTh,m_doEtaPhiAvgEnNoTh) );
      ATH_CHECK( saveAndDeleteHistsInLayers(m_h_totalEnergy_etaphi,TotalEnergy,m_procEtaPhiTotalEnergy) );
      ATH_CHECK( saveAndDeleteHistsInLayers(m_h_noise_etaphi,Noise,m_procEtaPhiEnergyRMS) );
      ATH_CHECK( saveAndDeleteHistsInLayers(m_h_deviationFromDBnoise_etaphi,NoiseVsDB,m_procEtaPhiDeviationFromDBnoise) );
      ATH_CHECK( saveAndDeleteHistsInLayers(m_h_averageQuality_etaphi,AvgQuality,m_procEtaPhiAverageQuality) );
      ATH_CHECK( saveAndDeleteHistsInLayers(m_h_averageTime_etaphi,AvgTime,m_procEtaPhiAverageTime) );
      ATH_CHECK( saveAndDeleteHistsInLayers(m_h_fractionOverQth_etaphi,PoorQualityFraction,m_procEtaPhiFractionOverQth) );
      ATH_CHECK( saveAndDeleteHistsInLayers(m_h_fractionPastTth_etaphi,PoorTimeFraction,m_procEtaPhiFractionPastTth) );

      if( m_procimask ) {
        ATH_CHECK( saveAndDeleteHistsInLayers(m_h_percentOccupancy_etaphi_imask,PercentOccupancy_2d, m_procPercentOccEtaPhi) );
        ATH_CHECK( saveAndDeleteHistsInLayers(m_h_percentOccupancy_eta_imask,PercentOccupancy_1d,m_procPercentOccEta) );
        ATH_CHECK( saveAndDeleteHistsInLayers(m_h_percentOccupancy_phi_imask,PercentOccupancy_1d,m_procPercentOccPhi) );
        ATH_CHECK( saveAndDeleteHistsInLayers(m_h_occupancy_etaphi_imask,Occupancy_2d,m_procAbsOccEtaPhi) );
        ATH_CHECK( saveAndDeleteHistsInLayers(m_h_occupancy_eta_imask,Occupancy_1d,m_procAbsOccEta) );
        ATH_CHECK( saveAndDeleteHistsInLayers(m_h_occupancy_phi_imask,Occupancy_1d,m_procAbsOccPhi) );
        ATH_CHECK( saveAndDeleteHistsInLayers(m_h_averageEnergy_etaphi_imask,AvgEnergy,m_doEtaPhiAvgEnTh) );
        ATH_CHECK( saveAndDeleteHistsInLayers(m_h_averageEnergy_etaphi_imask,AvgEnergyNoTh,m_doEtaPhiAvgEnNoTh) );
        ATH_CHECK( saveAndDeleteHistsInLayers(m_h_totalEnergy_etaphi_imask,TotalEnergy,m_procEtaPhiTotalEnergy) );
        ATH_CHECK( saveAndDeleteHistsInLayers(m_h_noise_etaphi,Noise,m_procEtaPhiEnergyRMS) );
      }
//      ATH_MSG_INFO("deleteProcHists() is done .");  
      return StatusCode::SUCCESS;
} 

//////////////////////////////////////////////////////////////////
StatusCode CaloCellVecMon::deleteHistograms(){
//  ATH_MSG_INFO("inside deleteHistograms ");

  ATH_CHECK( deleteSummHists() );
  ATH_CHECK( deleteLarNonThreHists() );
  ATH_CHECK( deleteLarMultThreHists() );

  deleteLarMultThreHistVectors();

  if(m_sporadic_switch) {
    ATH_CHECK( deleteSporHists() );
  }

  ATH_CHECK( deleteTileHists() );
//Khadeejah add

  ATH_CHECK( deleteProcHists() );
 
//Khadeejah add
  if(m_maskKnownBadChannels || m_maskNoCondChannels){
      if(!m_doMaskingOnline) {
         if(m_doInverseMasking)  {
           ATH_CHECK(  deleteImaskHists() );
         }
      } 
    }

  deleteMonGroups();

//  ATH_MSG_INFO("end of deleteHistograms ");
  return StatusCode::SUCCESS;
}

void CaloCellVecMon::deleteMonGroups() {
//  ATH_MSG_INFO("inside deleteMonGroups ");

  safeDelete(m_summaryGroup);
  safeDelete(m_tempMonGroup);

  for(int iht=KnownBadChannels; iht<MaxHistType; iht++) {
    safeDelete(m_shifterMonGroup[iht]);
//    safeDelete(ExpertMonGroup[iht]);
  }

  if(m_sporadic_switch){
    safeDelete(m_badCell);
    safeDelete(m_badCell_EMBC);
    safeDelete(m_badCell_EMBA);
    safeDelete(m_badCell_EMECC);
    safeDelete(m_badCell_EMECA);
    safeDelete(m_badCell_HECC);
    safeDelete(m_badCell_HECA);
    safeDelete(m_badCell_FCALC);
    safeDelete(m_badCell_FCALA);
  }

  safeDelete(m_tile_cells_shift);
//  ATH_MSG_INFO("end of deleteMonGroups ");
}

StatusCode CaloCellVecMon::deleteTileHists(){
//  ATH_MSG_INFO("inside deleteTileHists ");

  ATH_CHECK( saveAndDeleteHistFromGroup(m_h_Ncells,m_tile_cells_shift,1) );
  ATH_CHECK( saveAndDeleteHistFromGroup(m_h_CellsE,m_tile_cells_shift,1) );
  ATH_CHECK( saveAndDeleteHistFromGroup(m_h_CellsNoiseE,m_tile_cells_shift,1) );
  ATH_CHECK( saveAndDeleteHistFromGroup(m_h_CellsEta,m_tile_cells_shift,1) );
  ATH_CHECK( saveAndDeleteHistFromGroup(m_h_CellsPhi,m_tile_cells_shift,1) );
  ATH_CHECK( saveAndDeleteHistFromGroup(m_h_CellsEtaPhi,m_tile_cells_shift,1) );
  ATH_CHECK( saveAndDeleteHistFromGroup(m_h_CellsModuleS1,m_tile_cells_shift,1) );
  ATH_CHECK( saveAndDeleteHistFromGroup(m_h_CellsModuleS2,m_tile_cells_shift,1) );
  ATH_CHECK( saveAndDeleteHistFromGroup(m_h_CellsModuleS3,m_tile_cells_shift,1) );
  ATH_CHECK( saveAndDeleteHistFromGroup(m_h_CellsModuleS4,m_tile_cells_shift,1) );
  ATH_CHECK( saveAndDeleteHistFromGroup(m_h_CellsTower,m_tile_cells_shift,1) );
  ATH_CHECK( saveAndDeleteHistFromGroup(m_h_CellsNoiseEta,m_tile_cells_shift,1) );
  ATH_CHECK( saveAndDeleteHistFromGroup(m_h_CellsNoisePhi,m_tile_cells_shift,1) );
  ATH_CHECK( saveAndDeleteHistFromGroup(m_h_CellsNoiseEtaPhi,m_tile_cells_shift,1) );
  ATH_CHECK( saveAndDeleteHistFromGroup(m_h_CellsNoiseEtaPhiA,m_tile_cells_shift,1) );
  ATH_CHECK( saveAndDeleteHistFromGroup(m_h_CellsNoiseEtaPhiBC,m_tile_cells_shift,1) );
  ATH_CHECK( saveAndDeleteHistFromGroup(m_h_CellsNoiseEtaPhiD,m_tile_cells_shift,1) );
  ATH_CHECK( saveAndDeleteHistFromGroup(m_h_CellsRMSEta,m_tile_cells_shift,1) );
  ATH_CHECK( saveAndDeleteHistFromGroup(m_h_CellsRMSdivDBnoiseEta,m_tile_cells_shift,1) );
  ATH_CHECK( saveAndDeleteHistFromGroup(m_h_CellsRMSPhi,m_tile_cells_shift,1) );
  ATH_CHECK( saveAndDeleteHistFromGroup(m_h_CellsRMSdivDBnoisePhi,m_tile_cells_shift,1) );
  ATH_CHECK( saveAndDeleteHistFromGroup(m_h_CellsNoiseHash,m_tile_cells_shift,1) );
  
//  ATH_MSG_INFO("end of deleteTileHists ");
  return StatusCode::SUCCESS;

}

void CaloCellVecMon::deleteCaloLayers() {
  for(int ilyr=EMBPA; ilyr<MAXLAYER; ilyr++) {
    safeDelete(m_layer[ilyr]);
  }
}

StatusCode CaloCellVecMon::deleteLarMultThreHists(){
//  ATH_MSG_INFO("inside deleteLarMultThreHists ");

//  ATH_MSG_INFO("before m_h_occupancy_etaphi");
  ATH_CHECK( saveAndDeleteHistsInLayers(m_h_occupancy_etaphi,Occupancy_2d,m_fillEtaPhiOccupancy,m_tempEtaPhiOccupancy) );


//  ATH_MSG_INFO("before m_h_occupancy_eta");
  ATH_CHECK( saveAndDeleteHistsInLayers(m_h_occupancy_eta,Occupancy_1d,m_fillEtaOccupancy,m_doPercentageOccupancy) );

//  ATH_MSG_INFO("before m_h_occupancy_phi");
  ATH_CHECK( saveAndDeleteHistsInLayers(m_h_occupancy_phi,Occupancy_1d,m_fillPhiOccupancy,m_doPercentageOccupancy) );

//  ATH_MSG_INFO("before m_h_energyProfile_etaphi");
  ATH_CHECK( saveAndDeleteHistsInLayers(m_h_energyProfile_etaphi,Temporary,m_doEtaPhiEnergyProfile) );

//  ATH_MSG_INFO("before h_totalEnergy_etaphi");
  ATH_CHECK( saveAndDeleteHistsInLayers(m_h_totalEnergy_etaphi,TotalEnergy,m_fillEtaPhiTotalEnergy,m_tempEtaPhiTotalEnergy) );

//  ATH_MSG_INFO("before h_poorQualityOccupanc_etaphi");
  ATH_CHECK( saveAndDeleteHistsInLayers(m_h_poorQualityOccupancy_etaphi,Temporary,m_fillEtaPhiQualityOccupancy) );

//  ATH_MSG_INFO("before m_h_totalQuality_etaphi");
  ATH_CHECK( saveAndDeleteHistsInLayers(m_h_totalQuality_etaphi,Temporary,m_fillEtaPhiTotalQuality) );

//  ATH_MSG_INFO("before m_h_poorTimeOccupancy_etaphi");
  ATH_CHECK( saveAndDeleteHistsInLayers(m_h_poorTimeOccupancy_etaphi,Temporary,m_fillEtaPhiTimeOccupancy) );

//  ATH_MSG_INFO("before m_h_totalTime_etaphi");
  ATH_CHECK( saveAndDeleteHistsInLayers(m_h_totalTime_etaphi,Temporary,m_fillEtaPhiTotalTime) );

  if( (m_doKnownBadChannelsVsEtaPhi || ((!m_doMaskingOnline) && m_maskKnownBadChannels )) ) {
//    ATH_MSG_INFO("before m_h_badChannels_etaphi");
    ATH_CHECK( saveAndDeleteHistsInLayers(m_h_badChannels_etaphi,KnownBadChannels) );
  }  

  if(m_doDatabaseNoiseVsEtaPhi) {
//    ATH_MSG_INFO("before m_h_dbnoise_etaphi");
    ATH_CHECK( saveAndDeleteHistsInLayers(m_h_dbnoise_etaphi,DatabaseNoise) );
  }

  if(m_doNActiveCellsFirstEventVsEta){
//   ATH_MSG_INFO("before h_nActiveCellsFirstEven_eta");
    ATH_CHECK( saveAndDeleteHistsInLayers(m_h_nActiveCellsFirstEvent_eta,Temporary) );
  }

  if(m_doNActiveCellsFirstEventVsPhi){
//   ATH_MSG_INFO("before h_nActiveCellsFirstEven_phi");
    ATH_CHECK( saveAndDeleteHistsInLayers(m_h_nActiveCellsFirstEvent_phi,Temporary) );
  } 

  if(m_doInverseMasking && m_doMaskingOnline) {
//   ATH_MSG_INFO("before m_h_occupancy_etaphi_imask");
    ATH_CHECK( saveAndDeleteHistsInLayers(m_h_occupancy_etaphi_imask,Occupancy_2d,m_fillEtaPhiOccupancy,m_tempEtaPhiOccupancy) );
//   ATH_MSG_INFO("before m_h_occupancy_eta_imask");
    ATH_CHECK( saveAndDeleteHistsInLayers(m_h_occupancy_eta_imask,Occupancy_1d,m_fillEtaOccupancy,m_doPercentageOccupancy) );
//   ATH_MSG_INFO("before m_h_occupancy_phi_imask");
    ATH_CHECK( saveAndDeleteHistsInLayers(m_h_occupancy_phi_imask,Occupancy_1d,m_fillPhiOccupancy,m_doPercentageOccupancy) );
//   ATH_MSG_INFO("before m_h_energyProfile_etaphi_imask");
    ATH_CHECK( saveAndDeleteHistsInLayers(m_h_energyProfile_etaphi_imask,Temporary,m_doEtaPhiEnergyProfile) );
//   ATH_MSG_INFO("before m_h_totalEnergy_etaphi_imask");
    ATH_CHECK( saveAndDeleteHistsInLayers(m_h_totalEnergy_etaphi_imask,TotalEnergy,m_fillEtaPhiTotalEnergy,m_tempEtaPhiTotalEnergy) );
  }
  
//  ATH_MSG_INFO("before m_h_missingCellMask");
  ATH_CHECK( saveAndDeleteHistsInLayers(m_h_missingCellMask,Temporary) );
//  ATH_MSG_INFO("end of deleteLarMultThreHists ");
  return StatusCode::SUCCESS;
}
/*StatusCode CaloCellVecMon::deleteProcHists(){
//      ATH_MSG_INFO("deleteProcHists() starts .");  
  
      ATH_CHECK( saveAndDeleteHistsInLayers(m_h_percentOccupancy_etaphi,PercentOccupancy_2d, m_procPercentOccEtaPhi) );
      ATH_CHECK( saveAndDeleteHistsInLayers(m_h_percentOccupancy_eta,PercentOccupancy_1d,m_procPercentOccEta) );
      ATH_CHECK( saveAndDeleteHistsInLayers(m_h_percentOccupancy_phi,PercentOccupancy_1d,m_procPercentOccPhi) );
      ATH_CHECK( saveAndDeleteHistsInLayers(m_h_occupancy_etaphi,Occupancy_2d,m_procAbsOccEtaPhi) );
      ATH_CHECK( saveAndDeleteHistsInLayers(m_h_occupancy_eta,Occupancy_1d,m_procAbsOccEta) );
      ATH_CHECK( saveAndDeleteHistsInLayers(m_h_occupancy_phi,Occupancy_1d,m_procAbsOccPhi) );
      ATH_CHECK( saveAndDeleteHistsInLayers(m_h_averageEnergy_etaphi,AvgEnergy,m_doEtaPhiAvgEnTh) );
      ATH_CHECK( saveAndDeleteHistsInLayers(m_h_averageEnergy_etaphi,AvgEnergyNoTh,m_doEtaPhiAvgEnNoTh) );
      ATH_CHECK( saveAndDeleteHistsInLayers(m_h_totalEnergy_etaphi,TotalEnergy,m_procEtaPhiTotalEnergy) );
      ATH_CHECK( saveAndDeleteHistsInLayers(m_h_noise_etaphi,Noise,m_procEtaPhiEnergyRMS) );
      ATH_CHECK( saveAndDeleteHistsInLayers(m_h_deviationFromDBnoise_etaphi,NoiseVsDB,m_procEtaPhiDeviationFromDBnoise) );
      ATH_CHECK( saveAndDeleteHistsInLayers(m_h_averageQuality_etaphi,AvgQuality,m_procEtaPhiAverageQuality) );
      ATH_CHECK( saveAndDeleteHistsInLayers(m_h_averageTime_etaphi,AvgTime,m_procEtaPhiAverageTime) );
      ATH_CHECK( saveAndDeleteHistsInLayers(m_h_fractionOverQth_etaphi,PoorQualityFraction,m_procEtaPhiFractionOverQth) );
      ATH_CHECK( saveAndDeleteHistsInLayers(m_h_fractionPastTth_etaphi,PoorTimeFraction,m_procEtaPhiFractionPastTth) );

      if( m_procimask ) {
        ATH_CHECK( saveAndDeleteHistsInLayers(m_h_percentOccupancy_etaphi_imask,PercentOccupancy_2d, m_procPercentOccEtaPhi) );
        ATH_CHECK( saveAndDeleteHistsInLayers(m_h_percentOccupancy_eta_imask,PercentOccupancy_1d,m_procPercentOccEta) );
        ATH_CHECK( saveAndDeleteHistsInLayers(m_h_percentOccupancy_phi_imask,PercentOccupancy_1d,m_procPercentOccPhi) );
        ATH_CHECK( saveAndDeleteHistsInLayers(m_h_occupancy_etaphi_imask,Occupancy_2d,m_procAbsOccEtaPhi) );
        ATH_CHECK( saveAndDeleteHistsInLayers(m_h_occupancy_eta_imask,Occupancy_1d,m_procAbsOccEta) );
        ATH_CHECK( saveAndDeleteHistsInLayers(m_h_occupancy_phi_imask,Occupancy_1d,m_procAbsOccPhi) );
        ATH_CHECK( saveAndDeleteHistsInLayers(m_h_averageEnergy_etaphi_imask,AvgEnergy,m_doEtaPhiAvgEnTh) );
        ATH_CHECK( saveAndDeleteHistsInLayers(m_h_averageEnergy_etaphi_imask,AvgEnergyNoTh,m_doEtaPhiAvgEnNoTh) );
        ATH_CHECK( saveAndDeleteHistsInLayers(m_h_totalEnergy_etaphi_imask,TotalEnergy,m_procEtaPhiTotalEnergy) );
        ATH_CHECK( saveAndDeleteHistsInLayers(m_h_noise_etaphi,Noise,m_procEtaPhiEnergyRMS) );
      }
//      ATH_MSG_INFO("deleteProcHists() is done .");  
      return sc;
} */



void CaloCellVecMon::deleteLarMultThreHistVectors(){
//  ATH_MSG_INFO("inside deleteLarMultThreHistsVectors ");

  for(int ilyr=EMBPA; ilyr<MAXLAYER; ilyr++) {
    if(m_h_occupancy_etaphi[ilyr]){
      delete [] m_h_occupancy_etaphi[ilyr];
      m_h_occupancy_etaphi[ilyr]=0;
    } 
    
    if(m_h_occupancy_eta[ilyr]){
      delete [] m_h_occupancy_eta[ilyr];
      m_h_occupancy_eta[ilyr]=0;
    }

    if(m_h_occupancy_phi[ilyr]){
      delete [] m_h_occupancy_phi[ilyr];
      m_h_occupancy_phi[ilyr]=0;
    }

    if(m_h_occupancy_etaphi_imask[ilyr]){
      delete [] m_h_occupancy_etaphi_imask[ilyr];
      m_h_occupancy_etaphi_imask[ilyr]=0;
    }

    if(m_h_occupancy_eta_imask[ilyr]){
      delete [] m_h_occupancy_eta_imask[ilyr];
      m_h_occupancy_eta_imask[ilyr]=0;
    }

    if(m_h_occupancy_phi_imask[ilyr]){
      delete [] m_h_occupancy_phi_imask[ilyr];
      m_h_occupancy_phi_imask[ilyr]=0;
    }

    if(m_h_percentOccupancy_etaphi[ilyr]){
      delete [] m_h_percentOccupancy_etaphi[ilyr];
      m_h_percentOccupancy_etaphi[ilyr]=0;
    }

    if(m_h_percentOccupancy_eta[ilyr]){
      delete [] m_h_percentOccupancy_eta[ilyr];
      m_h_percentOccupancy_eta[ilyr]=0;
    }

    if(m_h_percentOccupancy_phi[ilyr]){
      delete [] m_h_percentOccupancy_phi[ilyr];
      m_h_percentOccupancy_phi[ilyr]=0;
    }

    if(m_h_percentOccupancy_etaphi_imask[ilyr]){
      delete [] m_h_percentOccupancy_etaphi_imask[ilyr];
      m_h_percentOccupancy_etaphi_imask[ilyr]=0;
    }

    if(m_h_percentOccupancy_phi_imask[ilyr]){
      delete [] m_h_percentOccupancy_phi_imask[ilyr];
      m_h_percentOccupancy_phi_imask[ilyr]=0;
    }

    if(m_h_percentOccupancy_eta_imask[ilyr]){
      delete [] m_h_percentOccupancy_eta_imask[ilyr];
      m_h_percentOccupancy_eta_imask[ilyr]=0;
    }

    if(m_h_energyProfile_etaphi[ilyr]){
      delete [] m_h_energyProfile_etaphi[ilyr];
      m_h_energyProfile_etaphi[ilyr]=0;
    } 

    if(m_h_totalEnergy_etaphi[ilyr]){
      delete [] m_h_totalEnergy_etaphi[ilyr];
      m_h_totalEnergy_etaphi[ilyr]=0;
    }

    if(m_h_averageEnergy_etaphi[ilyr]){
      delete [] m_h_averageEnergy_etaphi[ilyr];
      m_h_averageEnergy_etaphi[ilyr]=0;
    }

    if(m_h_noise_etaphi[ilyr]){
      delete [] m_h_noise_etaphi[ilyr];
      m_h_noise_etaphi[ilyr]=0;
    }

    if(m_h_deviationFromDBnoise_etaphi[ilyr]){
      delete [] m_h_deviationFromDBnoise_etaphi[ilyr];
      m_h_deviationFromDBnoise_etaphi[ilyr]=0;
    }

    if(m_h_energyProfile_etaphi_imask[ilyr]){
      delete [] m_h_energyProfile_etaphi_imask[ilyr];
      m_h_energyProfile_etaphi_imask[ilyr]=0;
    }

    if(m_h_totalEnergy_etaphi_imask[ilyr]){
      delete [] m_h_totalEnergy_etaphi_imask[ilyr];
      m_h_totalEnergy_etaphi_imask[ilyr]=0;
    }

    if(m_h_averageEnergy_etaphi_imask[ilyr]){
      delete [] m_h_averageEnergy_etaphi_imask[ilyr];
      m_h_averageEnergy_etaphi_imask[ilyr]=0;
    }

    if(m_h_noise_etaphi_imask[ilyr]){
      delete [] m_h_noise_etaphi_imask[ilyr];
      m_h_noise_etaphi_imask[ilyr]=0;
    }

    if(m_h_poorQualityOccupancy_etaphi[ilyr]){
      delete [] m_h_poorQualityOccupancy_etaphi[ilyr];
      m_h_poorQualityOccupancy_etaphi[ilyr]=0;
    }

    if(m_h_totalQuality_etaphi[ilyr]){
      delete [] m_h_totalQuality_etaphi[ilyr];
      m_h_totalQuality_etaphi[ilyr]=0;
    }

    if(m_h_fractionOverQth_etaphi[ilyr]){
      delete [] m_h_fractionOverQth_etaphi[ilyr];
      m_h_fractionOverQth_etaphi[ilyr]=0;
    }

    if(m_h_averageQuality_etaphi[ilyr]){
      delete [] m_h_averageQuality_etaphi[ilyr];
      m_h_averageQuality_etaphi[ilyr]=0;
    }

    if(m_h_poorTimeOccupancy_etaphi[ilyr]){
      delete [] m_h_poorTimeOccupancy_etaphi[ilyr];
      m_h_poorTimeOccupancy_etaphi[ilyr]=0;
    }

    if(m_h_totalTime_etaphi[ilyr]){
      delete [] m_h_totalTime_etaphi[ilyr];
      m_h_totalTime_etaphi[ilyr]=0;
    }

    if(m_h_fractionPastTth_etaphi[ilyr]){
      delete [] m_h_fractionPastTth_etaphi[ilyr];
      m_h_fractionPastTth_etaphi[ilyr]=0;
    } 
      
    if(m_h_averageTime_etaphi[ilyr]){
      delete [] m_h_averageTime_etaphi[ilyr];
      m_h_averageTime_etaphi[ilyr]=0;
    } 

  }
//  ATH_MSG_INFO("end of deleteLarMultThreHistsVectors ");

}

StatusCode CaloCellVecMon::deleteSporHists(){
//  ATH_MSG_INFO("inside deleteSporHists");

  for (uint iHisto=0; iHisto<m_occ.size(); iHisto++){
    if(!m_h_energy_s[iHisto] && !m_h_quality[iHisto] && !m_h_energy_lb[iHisto]) continue; 

    int name_flag=0;
    if(m_h_energy_s[iHisto]) find_n(m_h_energy_s[iHisto]->GetName(),name_flag);
    else if(m_h_quality[iHisto]) find_n(m_h_quality[iHisto]->GetName(),name_flag);
    else find_n(m_h_energy_lb[iHisto]->GetName(),name_flag);

    if(m_h_energy_s[iHisto]) adjust_n_1d(m_h_energy_s[iHisto]);
    if(m_h_quality[iHisto]) adjust_n_1d(m_h_quality[iHisto]);
    if(m_h_energy_lb[iHisto]) adjust_n_2d(m_h_energy_lb[iHisto]);
    MonGroup * tmpBadCell = 0;
    switch(name_flag){
        case 1:
                tmpBadCell = m_badCell_EMBC;
                break;
        case 2:
                tmpBadCell = m_badCell_EMBA;
                break;
        case 3:
                tmpBadCell = m_badCell_EMECC;
                break;
        case 4:
                tmpBadCell = m_badCell_EMECA;
                break;
        case 5:
                tmpBadCell = m_badCell_HECC;
                break;
        case 6:
                tmpBadCell = m_badCell_HECA;
                break;
        case 7:
                tmpBadCell = m_badCell_FCALC;
                break;
        case 8:
                tmpBadCell = m_badCell_FCALA;
                break;
        default:
                break;
    }

    if (m_occ[iHisto]>=m_EventNumber_thres){
      if(tmpBadCell) {
        if(m_h_energy_s[iHisto]){
          ATH_CHECK( saveAndDeleteHistFromGroup(m_h_energy_s[iHisto],tmpBadCell,1) );
        }
        if(m_h_energy_lb[iHisto]){
          ATH_CHECK( saveAndDeleteHistFromGroup(m_h_energy_lb[iHisto],tmpBadCell,1) );
        }
        if(m_h_quality[iHisto]){
          ATH_CHECK( saveAndDeleteHistFromGroup(m_h_quality[iHisto],tmpBadCell,1) );
        }
      }
    } // end of if (m_occ[iHisto]>=m_EventNumber_thres) 
    else{
     if(tmpBadCell) {
      if(m_h_energy_s[iHisto]){
        ATH_CHECK( saveAndDeleteHistFromGroup(m_h_energy_s[iHisto],tmpBadCell,0) );
      }
      if(m_h_energy_lb[iHisto]){
        ATH_CHECK( saveAndDeleteHistFromGroup(m_h_energy_lb[iHisto],tmpBadCell,0) );
      }
      if(m_h_quality[iHisto]){
        ATH_CHECK( saveAndDeleteHistFromGroup(m_h_quality[iHisto],tmpBadCell,0) );
      }
     }

    } //end of else (m_occ[iHisto]<m_EventNumber_thres) 

  } // end of loop iHisto 

  if(m_badCell){
    ATH_CHECK( saveAndDeleteHistFromGroup(m_h_NoiseBurstLumiblock,m_badCell,1) );
    ATH_CHECK( saveAndDeleteHistFromGroup(m_h_sporad,m_badCell,1) );
  }
  if(m_badCell_EMBC) {
    ATH_CHECK( saveAndDeleteHistFromGroup(m_h_sporad_embc,m_badCell_EMBC,1) );
  } 
  if(m_badCell_EMBA) {
    ATH_CHECK( saveAndDeleteHistFromGroup(m_h_sporad_emba,m_badCell_EMBA,1) );
  }
  if(m_badCell_EMECC) {
    ATH_CHECK( saveAndDeleteHistFromGroup(m_h_sporad_emecc,m_badCell_EMECC,1) );
  }
  if(m_badCell_EMECA) {
    ATH_CHECK( saveAndDeleteHistFromGroup(m_h_sporad_emeca,m_badCell_EMECA,1) );
  }
  if(m_badCell_HECC) {
    ATH_CHECK( saveAndDeleteHistFromGroup(m_h_sporad_hecc,m_badCell_HECC,1) );
  } 
  if(m_badCell_HECA) {
    ATH_CHECK( saveAndDeleteHistFromGroup(m_h_sporad_heca,m_badCell_HECA,1) );
  } 
  if(m_badCell_FCALC) {
    ATH_CHECK( saveAndDeleteHistFromGroup(m_h_sporad_fcalc,m_badCell_FCALC,1) );
  } 
  if(m_badCell_FCALA) {
    ATH_CHECK( saveAndDeleteHistFromGroup(m_h_sporad_fcala,m_badCell_FCALA,1) );
  }
//  ATH_MSG_INFO("end of deleteSporHists");
  return StatusCode::SUCCESS;
}

StatusCode CaloCellVecMon::deleteLarNonThreHists(){
//  ATH_MSG_INFO("inside deleteLarNonThreHists");

  for(int ilyr=EMBPA; ilyr<MAXLAYER; ilyr++) {
   if(m_doEtaLumi){ 
     ATH_CHECK( saveAndDeleteHistFromGroup(m_h_cellOccupancyEtaLumi[ilyr],m_shifterMonGroup[Occupancy_2d],1) );
     ATH_CHECK( saveAndDeleteHistFromGroup(m_h_cellAvgEnergyEtaLumi[ilyr],m_shifterMonGroup[AvgEnergy],1) );
   }
   if(m_doPhiLumi){
     ATH_CHECK( saveAndDeleteHistFromGroup(m_h_cellOccupancyPhiLumi[ilyr],m_shifterMonGroup[Occupancy_2d],1) );
     ATH_CHECK( saveAndDeleteHistFromGroup(m_h_cellAvgEnergyPhiLumi[ilyr],m_shifterMonGroup[AvgEnergy],1) );
   }

   if( m_doUnnormalized1DEnergy){
     ATH_CHECK( saveAndDeleteHistFromGroup(m_h_energy[ilyr],m_shifterMonGroup[Energy],1) );
     if(m_doInverseMasking) {
//       sc=saveAndDeleteHistFromGroup(m_h_energy_imask[ilyr],ExpertMonGroup[Energy],1);
       ATH_CHECK( saveAndDeleteHistFromGroup(m_h_energy_imask[ilyr],m_shifterMonGroup[Energy],1) );
     }
   }

   if( m_doDBNormalized1DEnergy ){
     ATH_CHECK( saveAndDeleteHistFromGroup(m_h_dbNoiseNormalizedEnergy[ilyr],m_shifterMonGroup[Energy],1) );
   }
   
   ATH_CHECK( saveAndDeleteHistFromGroup(m_h_energyVsTime[ilyr],m_shifterMonGroup[EnergyVsTime],1) );
   
   ATH_CHECK( saveAndDeleteHistFromGroup(m_h_energyVsTime_DifferThresholds[ilyr],m_shifterMonGroup[EnergyVsTime],1) );

   if(m_doInverseMasking) {
//     sc=saveAndDeleteHistFromGroup(m_h_energyVsTime_imask[ilyr],ExpertMonGroup[EnergyVsTime],1);
     ATH_CHECK( saveAndDeleteHistFromGroup(m_h_energyVsTime_imask[ilyr],m_shifterMonGroup[EnergyVsTime],1) );
   }

  }
//  ATH_MSG_INFO("end of deleteLarNonThreHists");
  return StatusCode::SUCCESS;

}

StatusCode CaloCellVecMon::deleteSummHists(){
//  ATH_MSG_INFO("inside deleteSummHists");
  ATH_CHECK( saveAndDeleteHistFromGroup(m_h_n_trigEvent,m_summaryGroup,1) );
  ATH_CHECK( saveAndDeleteHistFromGroup(m_h_EvtRejSumm,m_summaryGroup,1) ); // km add

//  ATH_MSG_INFO("end of deleteSummHists");
  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////
StatusCode CaloCellVecMon::finalHists()
{

//  ATH_MSG_INFO("CaloCellVecMon finalHists() is starting");
  // This assumes that the end of a file will naturally end a run, which is not always true.
  // A merging application run afterwards should be able to put parts of a run together.

/*
  ATH_MSG_INFO("before call procHistograms in finalHists()");
  ATH_CHECK( procHistograms(true,true,true) );
  ATH_MSG_INFO("after call procHistograms in finalHists()");

  ATH_MSG_INFO("before call deleteHistograms in finalHists()");
  ATH_CHECK(  deleteHistograms() );
  ATH_MSG_INFO("after call deleteHistograms in finalHists()");
*/

//  ATH_MSG_INFO("before call deleteArray in finalHists()");
  deleteArray();
//  ATH_MSG_INFO("after call deleteArray in finalHists()");

  return StatusCode::SUCCESS;
//  ATH_MSG_INFO("CaloCellVecMon finalHists() is done");
}

void CaloCellVecMon::deleteArray(){

//  ATH_MSG_INFO("in deleteArray()");
  for(int ilyrns=EMBPNS; ilyrns<MAXLYRNS; ilyrns++) {
    

    safeDeleteArray(m_thresholdDirection[ilyrns]);
    safeDeleteArray(m_threshTriggersToInclude[ilyrns]);
    safeDeleteArray(m_threshTriggersToExclude[ilyrns]);
    safeDeleteArray(m_threshTriggerDecision[ilyrns]);
    safeDeleteArray(m_nEventsPassingTrigger[ilyrns]);
    safeDeleteArray(m_nEventsPassingBeamBG[ilyrns]);

    safeDeleteArray(m_useNoiseTool[ilyrns]);
    safeDeleteArray(m_thresholds[ilyrns]);
    safeDeleteArray(m_threshNames[ilyrns]);
    safeDeleteArray(m_threshTitles[ilyrns]);
 
    safeDeleteArray(m_maskEmptyBins[ilyrns]);
    safeDeleteArray(m_doBeamBackgroundRemoval[ilyrns]);

    safeDeleteArray(m_doPercentageOccupancy[ilyrns]);
    safeDeleteArray(m_fillEtaPhiOccupancy[ilyrns]);
    safeDeleteArray(m_tempEtaPhiOccupancy[ilyrns]);
    safeDeleteArray(m_procEtaPhiOccupancy[ilyrns]);
    safeDeleteArray(m_fillEtaOccupancy[ilyrns]);
    safeDeleteArray(m_procEtaOccupancy[ilyrns]);
    safeDeleteArray(m_fillPhiOccupancy[ilyrns]);
    safeDeleteArray(m_procPhiOccupancy[ilyrns]);

    safeDeleteArray(m_fillEtaPhiTotalEnergy[ilyrns]);
    safeDeleteArray(m_tempEtaPhiTotalEnergy[ilyrns]);
    safeDeleteArray(m_procEtaPhiTotalEnergy[ilyrns]);
    safeDeleteArray(m_doEtaPhiEnergyProfile[ilyrns]);
    safeDeleteArray(m_doEtaPhiAverageEnergy[ilyrns]);
    safeDeleteArray(m_procEtaPhiEnergyRMS[ilyrns]);
    safeDeleteArray(m_procEtaPhiDeviationFromDBnoise[ilyrns]);

    safeDeleteArray(m_fillEtaPhiQualityOccupancy[ilyrns]);
    safeDeleteArray(m_fillEtaPhiTotalQuality[ilyrns]);
    safeDeleteArray(m_procEtaPhiAverageQuality[ilyrns]);
    safeDeleteArray(m_procEtaPhiFractionOverQth[ilyrns]);
    safeDeleteArray(m_fillEtaPhiTimeOccupancy[ilyrns]);
    safeDeleteArray(m_fillEtaPhiTotalTime[ilyrns]);
    safeDeleteArray(m_procEtaPhiAverageTime[ilyrns]);
    safeDeleteArray(m_procEtaPhiFractionPastTth[ilyrns]);

    safeDeleteArray(m_qualityThresholds[ilyrns]);
    safeDeleteArray(m_timeThresholds[ilyrns]);
  }
//  ATH_MSG_INFO("end of deleteArray()");

}

////////////////////////////////////////////////////////////////////////////////////
//
// book1(2)DEtaPhiHists - books eta-phi histograms for all thresholds for all layers
// for a specific variable. axisOptions are "e" for eta or "p" for phi.
// The genHistName is a general name that will change for each threshold and will 
// appear in the root file. See the "bookHistograms" method for an example of use.

//////////////////////////////////////////////////////////
// book 1d multi_threshold histograms for all layers.
template< class THist1D >
void CaloCellVecMon::book1DEtaPhiHists(THist1D** hist[],HistTypes histType,const char* genHistName,
				       const char* genHistTitle,const char* axisOptions,bool* doHist[],bool* isTemp[]) {
  
  if( (strcmp(axisOptions,"e")!=0) && (strcmp(axisOptions,"p")!=0)) {
    ATH_MSG_ERROR("Options for CaloCellVecMon::book1DEtaPhiHists(...,const char* axisOptions are \"e\" for eta or \"p\" for phi");
    ATH_MSG_ERROR("Resulting histograms for " << genHistName << "  may not fill properly");
    
    axisOptions = "e";
  }
  
  for(int ilyr = 0; ilyr < MAXLAYER; ilyr++) {
    LayerEnum layerNumber = (LayerEnum) ilyr;
    int ilyrns = ilyr / 2;
    bookLayerEtaPhiHists1D( layerNumber, hist, histType, genHistName, genHistTitle, axisOptions, doHist[ilyrns],isTemp[ilyrns]);
  }
}

// book 2d multi_threshold histograms for all layers.
template< class THist1D >
void CaloCellVecMon::book1DEtaPhiHists(THist1D** hist[],HistTypes histType,const char* genHistName,
                                       const char* genHistTitle,const char* axisOptions,bool* doHist[]) {

  if( (strcmp(axisOptions,"e")!=0) && (strcmp(axisOptions,"p")!=0)) {
    ATH_MSG_ERROR("Options for CaloCellVecMon::book1DEtaPhiHists(...,const char* axisOptions are \"e\" for eta or \"p\" for phi");
    ATH_MSG_ERROR("Resulting histograms for " << genHistName << "  may not fill properly");

    axisOptions = "e";
  }

  for(int ilyr = 0; ilyr < MAXLAYER; ilyr++) {
    LayerEnum layerNumber = (LayerEnum) ilyr;
    bookLayerEtaPhiHists1D( layerNumber, hist, histType, genHistName, genHistTitle, axisOptions, doHist[ilyr/2]);
  }
}

// book 2d multi_threshold histograms for one layer.
template< class THist1D >
void CaloCellVecMon::bookLayerEtaPhiHists1D(LayerEnum layerNumber , THist1D** hist[],HistTypes histType,
					    const char* genHistName,const char* genHistTitle,const char* axisOptions, bool* doHist, bool * isTemp) {

  if((strcmp(axisOptions,"e")!=0) && (strcmp(axisOptions,"p")!=0)) {
    ATH_MSG_ERROR("Options for CaloCellVecMon::bookLayerEtaPhiHists1D(...,const char* axisOptions are \"e\" for eta or \"p\" for phi");
    ATH_MSG_ERROR("Resulting histograms for " << genHistName << "  may not fill properly");
    
    axisOptions = "e";
  }
   
  CaloLayer *lyr = m_layer[layerNumber];
  int layerNumberNS = layerNumber/2;

  char uniqueName[256];
  char uniqueTitle[256];

  const int nThresholds = m_nThresholds[layerNumberNS];
  
  bool newHistArray = ( hist[layerNumber] == 0 );
  if( newHistArray ) {
    hist[layerNumber] = new THist1D*[nThresholds];
    for(int ti = 0; ti< nThresholds; ti++) hist[layerNumber][ti] = 0;
  }
    
  for(int ti = 0; ti< nThresholds; ti++) {

    if( doHist[ti] ) {

      bool registerHist = false;
      if( histType != Temporary ) {
	if( isTemp != 0 ) {
	  registerHist = (!isTemp[ti]);
	}
	else {
	  registerHist = true;
	}
      }
    
      if(m_doBeamBackgroundRemoval[layerNumberNS][ti]){
       sprintf(uniqueName,genHistName,lyr->getName(),m_threshNames[layerNumberNS][ti].c_str());
       sprintf(uniqueTitle,genHistTitle,lyr->getName(),m_threshTitles[layerNumberNS][ti].c_str()," with CSC veto");
      }
      else{
       sprintf(uniqueName,genHistName,lyr->getName(),m_threshNames[layerNumberNS][ti].c_str());
       sprintf(uniqueTitle,genHistTitle,lyr->getName(),m_threshTitles[layerNumberNS][ti].c_str()," w/o CSC veto");
      }

      if( !registerHist ) {
	sprintf(uniqueName,"%s_temp",uniqueName);
      }

      if(strcmp(axisOptions,"e") == 0) {
	hist[layerNumber][ti] = bookHistogram1D(hist[layerNumber][ti],uniqueName,uniqueTitle,lyr->getNTotEtaBins(),
						lyr->getEtaBinArray(),"#eta");
      }
      if(strcmp(axisOptions,"p") == 0) {
	hist[layerNumber][ti] = bookHistogram1D(hist[layerNumber][ti],uniqueName,uniqueTitle,lyr->getNTotPhiBins(),
						lyr->getPhiBinArray(),"#phi");
      }

      if( registerHist ){
//        ATH_MSG_INFO("before 1 reg 1D histograms in ShifterMonGroup for " << uniqueName );
/*
        if(hist[layerNumber][ti]) {
          TDirectory* dir = hist[layerNumber][ti]->GetDirectory();
          const char* dirName = dir->GetName();
          ATH_MSG_INFO("Directory: " << dirName );
        }
*/
	//If this is not a temporary histogram, then it should be registered so it can be saved:
	m_shifterMonGroup[histType]->regHist(hist[layerNumber][ti]).ignore();
//        ATH_MSG_INFO("after 1 reg 1D histograms in ShifterMonGroup for " << uniqueName );
/*
        if(hist[layerNumber][ti]) {
          TDirectory* dir = hist[layerNumber][ti]->GetDirectory();
          const char* dirName = dir->GetName();
//          ATH_MSG_INFO("Directory: " << dirName );
        }
*/
      }  // end of if(registerHist) 
      else {
//       ATH_MSG_INFO("before 1 reg 1D histograms in TempMonGroup for " << uniqueName );
/*
       if(hist[layerNumber][ti]) {
          TDirectory* dir = hist[layerNumber][ti]->GetDirectory();
          const char* dirName = dir->GetName();
//          ATH_MSG_INFO("Directory: " << dirName );
       }
*/
       m_tempMonGroup->regHist(hist[layerNumber][ti]).ignore();
//       ATH_MSG_INFO("after 1 reg 1D histograms in TempMonGroup for " << uniqueName );
/*
       if(hist[layerNumber][ti]) {
          TDirectory* dir = hist[layerNumber][ti]->GetDirectory();
          const char* dirName = dir->GetName();
//          ATH_MSG_INFO("Directory: " << dirName );
       }
*/
      } // end of else (!registerHist)

    }  // end of if( doHist[ti] )

  } // end of ti loop

} 


//////////////////////////////////////////////////////////
// book 2d multi_threshold histograms for all layers.
template< class THist2D >
void CaloCellVecMon::book2DEtaPhiHists(THist2D** hist[],HistTypes histType,const char* genHistName,
				       const char* genHistTitle, bool* doHist[], bool* isTemp[]) {

  for(int ilyr = 0; ilyr < MAXLAYER; ilyr++) {
    LayerEnum layerNumber = (LayerEnum) ilyr;
    int ilyrns = ilyr/2;
    bookLayerEtaPhiHists2D(layerNumber, hist, histType, genHistName, genHistTitle, doHist[ilyrns], isTemp[ilyrns]);
  }
}

// book 2d multi_threshold histograms for all layers.
template< class THist2D >
void CaloCellVecMon::book2DEtaPhiHists(THist2D** hist[],HistTypes histType,const char* genHistName,
				       const char* genHistTitle, bool* doHist[]) {
  // Loop over all layers
  for(int ilyr = 0; ilyr < MAXLAYER; ilyr++) {
    LayerEnum layerNumber = (LayerEnum) ilyr;
    bookLayerEtaPhiHists2D(layerNumber, hist, histType, genHistName, genHistTitle, doHist[ilyr/2]);
  }
}

// book 2d multi_threshold histograms for one layers.
template< class THist2D >
void CaloCellVecMon::bookLayerEtaPhiHists2D(LayerEnum layerNumber, THist2D** hist[],HistTypes histType,
					    const char* genHistName,const char* genHistTitle, bool* doHist, bool* isTemp) {
  char uniqueName[256];
  char uniqueTitle[256];

  CaloLayer * lyr = m_layer[layerNumber];
  int layerNumberNS = layerNumber/2;
  
  // allocating second dimension of array
  const int nThresholds = m_nThresholds[layerNumberNS];
  bool newHistArray = ( hist[layerNumber] == 0 );
  if( newHistArray ) {
    hist[layerNumber] = new THist2D*[nThresholds];
    for(int ti=0; ti<nThresholds; ti++) hist[layerNumber][ti]=0;
  }

  for(int ti=0; ti<nThresholds; ti++) {
    if( doHist[ti] ) {

      bool registerHist = false;
      if( histType != Temporary ) {
	if( isTemp != 0 ) {
	  registerHist = (!isTemp[ti]);
	}
	else {
	  registerHist = true;
	}
      }

      if(m_doBeamBackgroundRemoval[layerNumberNS][ti]){
       sprintf(uniqueName,genHistName,lyr->getName(),m_threshNames[layerNumberNS][ti].c_str());
       sprintf(uniqueTitle,genHistTitle,lyr->getName(),(m_threshTitles[layerNumberNS][ti]+" with CSC veto").c_str());
      }
      else{
       sprintf(uniqueName,genHistName,lyr->getName(),m_threshNames[layerNumberNS][ti].c_str());
       sprintf(uniqueTitle,genHistTitle,lyr->getName(),(m_threshTitles[layerNumberNS][ti]+" w/o CSC veto").c_str());
      }

      if( !registerHist ) {
	sprintf(uniqueName,"%s_temp",uniqueName);
      }
      
      hist[layerNumber][ti] = bookHistogram2D(hist[layerNumber][ti],uniqueName,uniqueTitle,lyr->getNTotEtaBins(),lyr->getEtaBinArray(),
					      lyr->getNTotPhiBins(),lyr->getPhiBinArray(),"#eta","#phi");

      if( registerHist ){
//        ATH_MSG_INFO("before 1 reg 2D histograms in ShifterMonGroup for " << uniqueName );
/*
        if(hist[layerNumber][ti]) {
          TDirectory* dir = hist[layerNumber][ti]->GetDirectory();
          const char* dirName = dir->GetName();
//          ATH_MSG_INFO("Directory: " << dirName );
        }
*/
	m_shifterMonGroup[histType]->regHist(hist[layerNumber][ti]).ignore();
//        ATH_MSG_INFO("after 1 reg 2D histograms in ShifterMonGroup for " << uniqueName );
/*
        if(hist[layerNumber][ti]) {
          TDirectory* dir = hist[layerNumber][ti]->GetDirectory();
          const char* dirName = dir->GetName();
//          ATH_MSG_INFO("Directory: " << dirName );
        }
*/
      } // end of if(registerHist) 
      else {
//        ATH_MSG_INFO("before 1 reg 2D histograms in TempMonGroup for " << uniqueName );
/*
        if(hist[layerNumber][ti]) {
          TDirectory* dir = hist[layerNumber][ti]->GetDirectory();
          const char* dirName = dir->GetName();
//          ATH_MSG_INFO("Directory: " << dirName );
        }
*/
        m_tempMonGroup->regHist(hist[layerNumber][ti]).ignore();
//        ATH_MSG_INFO("after 1 reg 2D histograms in TempMonGroup for " << uniqueName );
/*
        if(hist[layerNumber][ti]) {
          TDirectory* dir = hist[layerNumber][ti]->GetDirectory();
          const char* dirName = dir->GetName();
//          ATH_MSG_INFO("Directory: " << dirName );
        }
*/
      }  // end of else (!registerHist)
    } // end of if( doHist[ti] ) 
  } // end of ti loop

} 

///////////////////////////////////////////////////
// save and delete multi_threshold histograms for all layers 
template< class THist >
StatusCode CaloCellVecMon::saveAndDeleteHistsInLayers(THist** hist[], HistTypes histType, bool* doHist[], bool* isTemp[]){
//  ATH_MSG_INFO("inside saveAndDeleteHistsInLayers 1");
  for(int ilyr = 0; ilyr < MAXLAYER; ilyr++) {
    LayerEnum layerNumber = (LayerEnum) ilyr;
    int ilyrns = ilyr/2;
//    ATH_MSG_INFO("before call saveAndDeleteHistsInLayer for layer="<<ilyr);
    ATH_CHECK( saveAndDeleteHistsInLayer(layerNumber, hist, histType, doHist[ilyrns], isTemp[ilyrns]) );
  }
  return StatusCode::SUCCESS;
}

// save and delete multi_threshold histograms for all layers 
template< class THist >
StatusCode CaloCellVecMon::saveAndDeleteHistsInLayers(THist** hist[], HistTypes histType, bool* doHist[]){
//  ATH_MSG_INFO("inside saveAndDeleteHistsInLayers 2");
  for(int ilyr = 0; ilyr < MAXLAYER; ilyr++) {
    LayerEnum layerNumber = (LayerEnum) ilyr;
//    ATH_MSG_INFO("before call saveAndDeleteHistsInLayer for layer="<<ilyr);
    ATH_CHECK( saveAndDeleteHistsInLayer(layerNumber, hist, histType, doHist[ilyr/2]) );
  }
  return StatusCode::SUCCESS;
}

// save and delete multi_threshold histograms for one layer 
template< class THist >
StatusCode CaloCellVecMon::saveAndDeleteHistsInLayer(LayerEnum ilyr, THist** hists[], HistTypes histType, bool* doHist, bool* isTemp){
//      ATH_MSG_INFO("inside saveAndDeleteHistsInLayer 1");
      int layerNumberNS = ilyr/2;
      const int nThresholds = m_nThresholds[layerNumberNS];

//      if(!hists[ilyr]) ATH_MSG_INFO("inside saveAndDeleteHistsInLayer 1: Hists[ilyr] = 0");
      if(hists[ilyr]) {
        for(int ti = 0; ti<nThresholds; ti++) {
//          ATH_MSG_INFO("inside saveAndDeleteHistsInLayer 1: ti="<<ti);
//          if(!doHist[ti]) ATH_MSG_INFO("inside saveAndDeleteHistsInLayer 1: doHist = 0");
          if(doHist[ti]) {
            bool registerHist = false;
            if( histType != Temporary ) {
             if(isTemp!=0){
               registerHist = (!isTemp[ti]);
             }
             else {
              registerHist = true;
             }
            }
            
//            if(!hists[ilyr][ti])  ATH_MSG_INFO("inside saveAndDeleteHistsInLayer 1: Hists[ilyr][ti] = 0");
            if(hists[ilyr][ti]){
             if(registerHist) {
//              ATH_MSG_INFO("inside saveAndDeleteHistsInLayer 1: before saveAndDeleteHistFromGroup true");
               ATH_CHECK( saveAndDeleteHistFromGroup(hists[ilyr][ti], m_shifterMonGroup[histType], true ) );
             } // end of if (registerHist)
             else { 
//              ATH_MSG_INFO("inside saveAndDeleteHistsInLayer 1: before saveAndDeleteHistFromGroup false");
               ATH_CHECK( saveAndDeleteHistFromGroup(hists[ilyr][ti],m_tempMonGroup,false) );
             } //end of else (!registerHist)
            } // end of if(hists[ilyr][ti])
          } // end of if(doHist[ti]) 
        } // end of ti loop     
      } // end of if(hists[ilyr])
      
      return StatusCode::SUCCESS;
}

// save and delete histogram from monGroup
template< class THist >
StatusCode CaloCellVecMon::saveAndDeleteHistFromGroup(THist* &hist, MonGroup* &monGroup, bool if_write) {
  //ATH_MSG_INFO("inside saveAndDeleteHistFromGroup");
  StatusCode sc  = StatusCode::SUCCESS;
  if( hist != 0 &&  monGroup != 0 ) {
    if(if_write){
/*
     const char* histName = hist->GetName(); 
//     ATH_MSG_INFO("inside saveAndDeleteHistFromGroup before write"<< histName);
//     sc=(monGroup->writeAndDelete(hist)).ignore();
     if(hist) {
      TDirectory* dir = hist->GetDirectory();
      const char* dirName = dir->GetName();
//      ATH_MSG_INFO("Directory: " << dirName );
     }
*/
     sc=monGroup->writeAndDelete(hist);
     hist = 0;
    }
    else {
/*
     const char* histName = hist->GetName();
//     ATH_MSG_INFO("inside saveAndDeleteHistFromGroup before dereg" << histName);
     if(hist) {
      TDirectory* dir = hist->GetDirectory();
      const char* dirName = dir->GetName();
//      ATH_MSG_INFO("Directory: " << dirName );
     }
*/
     if(hist) {
      hist->SetDirectory(0);
     }
//     sc=monGroup->deregHist(hist);
//     ATH_MSG_INFO("inside saveAndDeleteHistFromGroup after SetDirectory to 0");
/*
     if(hist) {
      TDirectory* dir = hist->GetDirectory();
      if(dir){
       const char* dirName = dir->GetName();
//       ATH_MSG_INFO("Directory: " << dirName );
      }
//      else ATH_MSG_INFO("Directory: 0" );
     }
*/
     sc=monGroup->writeAndDelete(hist);
 //    ATH_MSG_INFO("inside saveAndDeleteHistFromGroup after delete");
     hist=0;
//     safeDelete(hist);
    }
  }
  return sc;
}

//////////////////////////////////////////////////////////
// book 2d unthresholded histograms for all layers.
template< class THist2D >
void CaloCellVecMon::book2DEtaPhiHists(THist2D* hist[],HistTypes histType,const char* genHistName,
                                       const char* genHistTitle) {

  // Loop over all layers
  for(int ilyr = 0; ilyr < MAXLAYER; ilyr++) {
    LayerEnum layerNumber = (LayerEnum) ilyr;
    bookLayerEtaPhiHists2D(layerNumber, hist, histType, genHistName, genHistTitle);
  }
}

template< class THist2D >
void CaloCellVecMon::bookLayerEtaPhiHists2D(LayerEnum layerNumber, THist2D* hist[],HistTypes histType,
					    const char* genHistName,const char* genHistTitle) {
  CaloLayer *lyr = m_layer[layerNumber];

  char uniqueName[256];
  char uniqueTitle[256];

  sprintf(uniqueName,genHistName,lyr->getName());
  sprintf(uniqueTitle,genHistTitle,lyr->getName());
  
  hist[layerNumber] = bookHistogram2D(hist[layerNumber],uniqueName,uniqueTitle,lyr->getNTotEtaBins(),lyr->getEtaBinArray(),
					 lyr->getNTotPhiBins(),lyr->getPhiBinArray(),"#eta","#phi");
  
  if(histType != Temporary){
    //If this is not a temporary histogram, then it should be registered so it can be saved:
//    ATH_MSG_INFO("before 2 reg 2D histograms in ShifterMonGroup for " << uniqueName );
/*
    if(hist[layerNumber]) {
      TDirectory* dir = hist[layerNumber]->GetDirectory();
      const char* dirName = dir->GetName();
//      ATH_MSG_INFO("Directory: " << dirName );
    }
*/
    m_shifterMonGroup[histType]->regHist(hist[layerNumber]).ignore();
//    ATH_MSG_INFO("after 2 reg 2D histograms in ShifterMonGroup for " << uniqueName );
/*
    if(hist[layerNumber]) {
      TDirectory* dir = hist[layerNumber]->GetDirectory();
      const char* dirName = dir->GetName();
//      ATH_MSG_INFO("Directory: " << dirName );
    }
*/
  } // end of if(histType != Temporary)
  else {
//    ATH_MSG_INFO("before 2 reg 2D histograms in TempMonGroup for " << uniqueName );
/*
    if(hist[layerNumber]) {
      TDirectory* dir = hist[layerNumber]->GetDirectory();
      const char* dirName = dir->GetName();
//      ATH_MSG_INFO("Directory: " << dirName );
    }
*/
    m_tempMonGroup->regHist(hist[layerNumber]).ignore();
//    ATH_MSG_INFO("after 2 reg 2D histograms in TempMonGroup for " << uniqueName );
/*
    if(hist[layerNumber]) {
      TDirectory* dir = hist[layerNumber]->GetDirectory();
      const char* dirName = dir->GetName();
//      ATH_MSG_INFO("Directory: " << dirName );
    }
*/
  }  //end of else (histType = Temporary)
  
}

//////////////////////////////////////////////////////////
// book 1d unthresholded histograms for all layers.
template< class THist1D >
void CaloCellVecMon::book1DEtaPhiHists(THist1D* hist[],HistTypes histType,const char* genHistName,
				       const char* genHistTitle,const char* axisOptions) {
  
  if( (strcmp(axisOptions,"e")!=0) && (strcmp(axisOptions,"p")!=0)) {
    ATH_MSG_ERROR("Options for CaloCellVecMon::book1DEtaPhiHists(...,const char* axisOptions are \"e\" for eta or \"p\" for phi");
    ATH_MSG_ERROR("Resulting histograms for " << genHistName << "  may not fill properly");
    
    axisOptions = "e";
  }
  
  // Loop over layers, calling the bookLayerHists1D method for each layer
  for(int ilyr = 0; ilyr < MAXLAYER; ilyr++) {
    LayerEnum layerNumber = (LayerEnum) ilyr;
    bookLayerEtaPhiHists1D( layerNumber, hist, histType, genHistName, genHistTitle, axisOptions);
  }
}

// Book 1d unthresholded  histograms for a single CaloLayer:
template< class THist1D >
void CaloCellVecMon::bookLayerEtaPhiHists1D(LayerEnum layerNumber , THist1D* hist[],HistTypes histType,
					    const char* genHistName,const char* genHistTitle,const char* axisOptions) {

  if((strcmp(axisOptions,"e")!=0) && (strcmp(axisOptions,"p")!=0)) {
    ATH_MSG_ERROR("Options for CaloCellVecMon::bookLayerEtaPhiHists1D(...,const char* axisOptions are \"e\" for eta or \"p\" for phi");
    ATH_MSG_ERROR("Resulting histograms for " << genHistName << "  may not fill properly");
    
    axisOptions = "e";
  }
   
  CaloLayer *lyr = m_layer[layerNumber];

  char uniqueName[256];
  char uniqueTitle[256];
  
  sprintf(uniqueName,genHistName,lyr->getName());
  sprintf(uniqueTitle,genHistTitle,lyr->getName());
  
  if(strcmp(axisOptions,"e") == 0) {
    hist[layerNumber] = bookHistogram1D(hist[layerNumber],uniqueName,uniqueTitle,lyr->getNTotEtaBins(),
					   lyr->getEtaBinArray(),"#eta");
  }
  if(strcmp(axisOptions,"p") == 0) {
    hist[layerNumber] = bookHistogram1D(hist[layerNumber],uniqueName,uniqueTitle,lyr->getNTotPhiBins(),
					   lyr->getPhiBinArray(),"#phi");
  }
  if(histType != Temporary){
    //If this is not a temporary histogram, then it should be registered so it can be saved:
//    ATH_MSG_INFO("before 2 reg 1D histograms in ShifterMonGroup for " << uniqueName );
/*
    if(hist[layerNumber]) {
      TDirectory* dir = hist[layerNumber]->GetDirectory();
      const char* dirName = dir->GetName();
//      ATH_MSG_INFO("Directory: " << dirName );
    }
*/
    m_shifterMonGroup[histType]->regHist(hist[layerNumber]).ignore();
//    ATH_MSG_INFO("after 2 reg 1D histograms in ShifterMonGroup for " << uniqueName );
/*
    if(hist[layerNumber]) {
      TDirectory* dir = hist[layerNumber]->GetDirectory();
      const char* dirName = dir->GetName();
//      ATH_MSG_INFO("Directory: " << dirName );
    }
*/
  }
  else {
//    ATH_MSG_INFO("before 2 reg 1D histograms in TempMonGroup for " << uniqueName );
/*
    if(hist[layerNumber]) {
      TDirectory* dir = hist[layerNumber]->GetDirectory();
      const char* dirName = dir->GetName(); 
//      ATH_MSG_INFO("Directory: " << dirName );
    }
*/
    m_tempMonGroup->regHist(hist[layerNumber]).ignore();
//    ATH_MSG_INFO("after 2 reg 1D histograms in TempMonGroup for " << uniqueName );
/*
    if(hist[layerNumber]) {
      TDirectory* dir = hist[layerNumber]->GetDirectory();
      const char* dirName = dir->GetName();
//      ATH_MSG_INFO("Directory: " << dirName );
    }
*/
  }

}

///////////////////////////////////////////////////
// save and delete unthresholded histograms for all layers 
template< class THist >
StatusCode CaloCellVecMon::saveAndDeleteHistsInLayers(THist* hist[], HistTypes histType){
//  ATH_MSG_INFO("inside saveAndDeleteHistsInLayers 3");
  for(int ilyr = 0; ilyr < MAXLAYER; ilyr++) {
    LayerEnum layerNumber = (LayerEnum) ilyr;
//    int ilyrns = ilyr/2;
//    ATH_MSG_INFO("before call saveAndDeleteHistsInLayer for layer="<<ilyr);
    ATH_CHECK( saveAndDeleteHistsInLayer(layerNumber, hist, histType) );
  }
  return StatusCode::SUCCESS;
}

// save and delete unthresholded histograms for one layer 
template< class THist >
StatusCode CaloCellVecMon::saveAndDeleteHistsInLayer(LayerEnum ilyr, THist* hists[], HistTypes histType){
//  ATH_MSG_INFO("inside saveAndDeleteHistsInLayer 3");
//      int layerNumberNS = ilyr/2;
//      const int nThresholds = m_nThresholds[layerNumberNS];
   
//     if(!hists[ilyr]) ATH_MSG_INFO("inside saveAndDeleteHistsInLayer 3: hists[ilyr] = 0");

      if(hists[ilyr]) {
             if(histType != Temporary) {
//              ATH_MSG_INFO("inside saveAndDeleteHistsInLayer 3: before saveAndDeleteHistFromGroup true");
               ATH_CHECK( saveAndDeleteHistFromGroup(hists[ilyr], m_shifterMonGroup[histType], true ) );
             }
             else { 
//              ATH_MSG_INFO("inside saveAndDeleteHistsInLayer 3: before saveAndDeleteHistFromGroup false");
               ATH_CHECK( saveAndDeleteHistFromGroup(hists[ilyr],m_tempMonGroup,false) );
             }
      }
      
      return StatusCode::SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////////
// Templated histogram booking methods and specialized overloaded methods for light weight histograming:
template< class THist1D >
  THist1D* CaloCellVecMon::bookHistogram1D(THist1D* histogram, const char* name, const char* title, int nbinsx, const double* xbins, 
					 const char* xTitle) {
  histogram = new THist1D(name,title,nbinsx,xbins);
  histogram->GetXaxis()->SetTitle(xTitle);
  if( !(histogram->InheritsFrom("TProfile2D")) ) {
    histogram->Sumw2();
  }
  return histogram;
}
 
template< class THist2D >
  THist2D* CaloCellVecMon::bookHistogram2D(THist2D* histogram, const char* name, const char* title, int nbinsx, const double* xbins, 
					    int nbinsy, const double* ybins, const char* xTitle, const char* yTitle) {
  histogram = new THist2D(name,title,nbinsx,xbins,nbinsy,ybins);
  histogram->GetXaxis()->SetTitle(xTitle);
  histogram->GetYaxis()->SetTitle(yTitle);
  histogram->GetZaxis()->SetLabelSize(0.02); //pb-pb
  if( !(histogram->InheritsFrom("TProfile2D")) ) {
    histogram->Sumw2();
  }
  return histogram;
}

TH2F_LW* CaloCellVecMon::bookHistogram2D(TH2F_LW* histogram, const char* name, const char* title, int nbinsx, const double* xbins, 
					 int nbinsy, const double* ybins, const char* xTitle, const char* yTitle) {
  histogram =  TH2F_LW::create(name,title,nbinsx,xbins,nbinsy,ybins);
  histogram->GetXaxis()->SetTitle(xTitle);
  histogram->GetYaxis()->SetTitle(yTitle);
  histogram->GetZaxis()->SetLabelSize(0.02); //pb-pb
  return histogram;
}


//////////////////////////////////////////// 
//safe delete functions
template< class THists >
void CaloCellVecMon::safeDelete(THists** &hists) {
  //Helper method to delete array of THists if they were ever actually booked:
  if( hists != 0 ) {
    for(int i=0; i<hists->size();i++)  {
     if(hists->at(i) !=0){
      delete hists->at(i);
      hists->at(i)=0;
     }
    }
    delete [] hists;
    hists = 0;
  }
}

template< class THist >
void CaloCellVecMon::safeDelete(THist* &hist) {
  //Helper method to delete THist if it was ever booked:
  if( hist != 0 ) {
    delete hist;
    hist = 0;
  }
}

template< class dynArray >
void CaloCellVecMon::safeDeleteArray(dynArray* &array) {
  if( array != 0 ) {
    delete[] array;
    array = 0;
  }
}

void CaloCellVecMon::safeDelete(TH2F_LW* &hist) {
  if( hist != 0 ) {
    LWHist::safeDelete(hist);
    hist = 0;
  }
}

////////////////////////////////////////////////////////////
// helpers for process histograms 

template< class THist2D >
void CaloCellVecMon::maskHistogram( THist2D *hist, TH2F_LW *maskHisto, uint32_t bitsToMask, THist2D *imaskedHist ) {
  //Method to mask histogram "hist" based on maskHisto and bitsToMask, also fill inverse masked histogram if a defined inverse masked
  // histogram is given. (Templated to accept TProfiles aswell as TH2Fs)
  
  
  //Check that the input histograms are defined, if not, return:
  if( (!hist) || (!maskHisto) ){
    return;
  }
  
  double nHistEntries = hist->GetEntries();
  double nMaskedEntries = 0;
  
  unsigned ix = 0;
  unsigned iy = 0;
  double maskCont = 0;
  double error = 0;
  maskHisto->resetActiveBinLoop();
  if( !imaskedHist  ) {
    //just mask, don't fill inverse mask
    while(maskHisto->getNextActiveBin(ix,iy,maskCont,error)) {    
      uint32_t cellInfo = (uint32_t) maskCont;
      if ( ( cellInfo & bitsToMask ) != 0 ) {
	double bincont = hist->GetBinContent(ix,iy);
	hist->SetBinContent(ix,iy,0);
	hist->SetBinError(ix,iy,0);
	nMaskedEntries += bincont;
	nHistEntries -= bincont;
      }
    }
  }
  else {
    //mask and fill inverse masked histogram:
    while(maskHisto->getNextActiveBin(ix,iy,maskCont,error)) {    
      uint32_t cellInfo = (uint32_t) maskCont;
      if ( ( cellInfo & bitsToMask ) != 0 ) {
	double bincont = hist->GetBinContent(ix,iy);
	imaskedHist->SetBinContent(ix,iy, bincont );
	imaskedHist->SetBinError(ix,iy, hist->GetBinError(ix,iy) );
	hist->SetBinContent(ix,iy,0);
	hist->SetBinError(ix,iy,0);
	nMaskedEntries += bincont;
	nHistEntries -= bincont;
      }
    }
    imaskedHist->SetEntries(nMaskedEntries);
  }
  char title[128];
  sprintf(title,"%s - %s",(char*)hist->GetTitle(),"Masked");
  hist->SetTitle(title);
  hist->SetEntries(nHistEntries);
}

template< class THist2D >
void CaloCellVecMon::maskHistogram( THist2D *hist, TH2F *maskHisto, uint32_t bitsToMask, THist2D *imaskedHist ) {
  //Method to mask histogram "hist" based on maskHisto and bitsToMask, also fill inverse masked histogram if a defined inverse masked
  // histogram is given. (Templated to accept TProfiles aswell as TH2Fs)
  
  
  //Check that the input histograms are defined, if not, return:
  if( (!hist) || (!maskHisto) ){
    return;
  }
  
  double nHistEntries = hist->GetEntries();
  double nMaskedEntries = 0;

  int nbinx = maskHisto->GetNbinsX();
  int nbiny = maskHisto->GetNbinsY(); 

  if(!imaskedHist){
   for(int ix=0; ix<nbinx; ix++){
    for(int iy=0; iy<nbiny; iy++){
      int bin = maskHisto->GetBin(ix,iy);
      double maskCont=0;
      maskCont=maskHisto->GetBinContent(bin);
      uint32_t cellInfo = (uint32_t) maskCont;
      if((cellInfo & bitsToMask)!=0){
       double bincont = 0;
       //double binerror=0; 
       bincont=hist->GetBinContent(bin);
       //binerror=hist->GetBinError(bin);
       hist->SetBinContent(bin,0);
       hist->SetBinError(bin,0);
       nMaskedEntries +=bincont;
       nHistEntries -=bincont;
      } 
    }
   }
  }
  else{
   for(int ix=0; ix<nbinx; ix++){
    for(int iy=0; iy<nbiny; iy++){
      int bin = maskHisto->GetBin(ix,iy);
      double maskCont=0;
      maskCont=maskHisto->GetBinContent(bin);
      uint32_t cellInfo = (uint32_t) maskCont;
      if((cellInfo&bitsToMask)!=0){
       double bincont=0;
       double binerror=0;
       bincont=hist->GetBinContent(bin);
       binerror=hist->GetBinError(bin);
       hist->SetBinContent(bin,0);
       hist->SetBinError(bin,0);
       imaskedHist->SetBinContent(bin, bincont );
       imaskedHist->SetBinError(bin, binerror );
       nMaskedEntries +=bincont;
       nHistEntries -=bincont;
      }
    }
   }   
   imaskedHist->SetEntries(nMaskedEntries);
  }
  
  char title[128];
  sprintf(title,"%s - %s",(char*)hist->GetTitle(),"Masked");
  hist->SetTitle(title);
  hist->SetEntries(nHistEntries);
}

uint32_t CaloCellVecMon::buildBitMask(std::vector<std::string> namedProblemsToMask) {
  //Largely copied from LArBadChannelMasker, perhaps not a permanent solution?
  uint32_t bitsToMask = 0;
  static LArBadChanBitPacking  larBadChanBitPacking;

  for(std::vector<std::string>::const_iterator it = namedProblemsToMask.begin(); it != namedProblemsToMask.end(); ++it) {
    std::pair<bool, LArBadChannelEnum::ProblemType> result = larBadChanBitPacking.enumName(*it);
    if(result.first) {
      larBadChanBitPacking.setBit(result.second, bitsToMask);
    }
    else {
      ATH_MSG_WARNING("CaloCellVecMon:: The problematic cell name '" << it->c_str() << "' was not recognized.");
    }
  }
  return bitsToMask;
}


/* The aim of this function is calculate RMS/NoiseDB histogram from 2 profile
 *  histograms. 
 */ 
void CaloCellVecMon::getRMSdevNoise(TProfile *t1, TProfile *t2, TH1F *t3) { 
  // t1 histogram with RMS as errors
  // t2 histogram with noise DB
  // resulting histogram 

  //Check if the histograms are actually defined, if not, return:
  if( (!t1) || (!t2) || (!t3) ) {
    return;
  }

  int nb=t1->GetNbinsX();
  for(int ix=1;ix<nb+1;ix++){
    Double_t ierr1=(t1->GetBinError(ix))*1000; // convert to MeV 
    Double_t ent = t1->GetBinEntries(ix);
    Double_t ierr2=t2->GetBinError(ix);
    Double_t ienn2=t2->GetBinContent(ix);
    double Rat=0.0;
    if (ienn2>0) Rat=ierr1/ienn2; // ratio: RMS/noiseDB  
    double er1=0;
    if (ent>0) er1=ierr1/sqrt(ent); // error on RMS 
    double er2=0;
    if (ienn2>0) er2=ierr2/ienn2;
    double Err=Rat*sqrt( (er1*er1) + (er2*er2) );
    t3->SetBinContent(ix,Rat);
    t3->SetBinError(ix,Err);
  }
  t3->SetEntries( nb );
  t3->SetStats(0);
}
 
void CaloCellVecMon::unpackTProfile2D(TProfile2D * input, TH2F* noise, TH2F* average, TH2F* occupancy, TH2F* total, float norm) {
  //Method for unpacking a TProfile 2D with option "s" set into three seperate histograms:
  //Verbosely written for the sake of efficiency and safety; this method can accept any combination of its arguements
  //being a null pointer. Optional arguement "norm" allows for occupancy to be normalized by some constant.

  if( input == 0 ) {
    return;
  }

  if(norm == 0) {
    norm = 1;
  }
  const float inv_norm = 1. / norm;

  int nbinx = input->GetNbinsX();
  int nbiny = input->GetNbinsY(); 
  double nEntries = input->GetEntries();

  if ( (average != 0) && (noise != 0) ) {
    if( occupancy != 0 ) {
      if( total != 0 ) {
	//Make everything, average, noise, occupancy, total:
	for(int ix=1;ix<nbinx+1;ix++){	
	  for(int iy=1;iy<nbiny+1;iy++){
	    Int_t bin = input->GetBin(ix,iy);
	    double inputEntries = input->GetBinEntries(bin);
	    //Pass error code and continue:
	    if( inputEntries == -1 ) {
	      average->SetBinContent(bin,-1.);
	      average->SetBinError(bin,0.);
	      noise->SetBinContent(bin,-1.);
	      noise->SetBinError(bin,0.);
	      occupancy->SetBinContent(bin,-1.);
	      occupancy->SetBinError(bin,0.);
	      total->SetBinContent(bin,-1.);
	      total->SetBinError(bin,0.);
	      continue;
	    }
	    double inputContent =  input->GetBinContent(bin);
	    double inputRMS = input->GetBinError(bin);
		    
	    average->SetBinContent(bin,inputContent);
	    noise->SetBinContent(bin,inputRMS);
	    occupancy->SetBinContent(bin,inputEntries*inv_norm);
	    occupancy->SetBinError(bin,sqrt(inputEntries)*inv_norm);       
	    total->SetBinContent(bin, (inputContent * inputEntries) );
	    if (inputEntries>0) {
	      double sqrtEntries = sqrt(inputEntries);
	      average->SetBinError(bin,inputRMS/sqrtEntries);
	      noise->SetBinError(bin,inputRMS/(sqrtEntries*sqrt(2)) );
	      total->SetBinError(bin,inputRMS * sqrtEntries);
	    }
	  }      
	}
	total->SetEntries(nEntries);
      }
      else {
	// average, noise, occupancy
	for(int ix=1;ix<nbinx+1;ix++){	
	  for(int iy=1;iy<nbiny+1;iy++){
	    
	    Int_t bin = input->GetBin(ix,iy);
	    double inputEntries = input->GetBinEntries(bin);
	    if( inputEntries == -1 ) {
	      average->SetBinContent(bin,-1);
	      average->SetBinError(bin, 0.);
	      noise->SetBinContent(bin,-1);
	      noise->SetBinError(bin, 0.);
	      occupancy->SetBinContent(bin,-1.);
	      occupancy->SetBinError(bin,0.);
	      continue;
	    }
	    double inputContent =  input->GetBinContent(bin);
	    double inputRMS = input->GetBinError(bin);
    
	    average->SetBinContent(bin,inputContent);
	    noise->SetBinContent(bin,inputRMS);
	    occupancy->SetBinContent(bin,inputEntries*inv_norm);
	    occupancy->SetBinError(bin,sqrt(inputEntries)*inv_norm);       
	    
	    if (inputEntries>0) {
	      average->SetBinError(bin,inputRMS/sqrt(inputEntries));
	      noise->SetBinError(bin,inputRMS/sqrt(2*inputEntries));
	    }
	  }      
	}
      }
      occupancy->SetEntries(nEntries);
    }
    else {
      if( total != 0) {
	// average, noise, total
	for(int ix=1;ix<nbinx+1;ix++){	
	  for(int iy=1;iy<nbiny+1;iy++){
	    Int_t bin = input->GetBin(ix,iy);

	    double inputEntries = input->GetBinEntries(bin);
	    if( inputEntries == -1 ) {
	      average->SetBinContent(bin,-1.);
	      average->SetBinError(bin,0.);
	      noise->SetBinContent(bin,-1.);
	      noise->SetBinError(bin,0.);
	      total->SetBinContent(bin,-1.);
	      total->SetBinError(bin,0.);
	      continue;
	    }
	    double inputContent =  input->GetBinContent(bin);
	    double inputRMS = input->GetBinError(bin);
		
	    average->SetBinContent(bin,inputContent);
	    noise->SetBinContent(bin,inputRMS);
	    total->SetBinContent(bin, (inputContent * inputEntries) );
	    if (inputEntries>0) {
	      double sqrtEntries = sqrt(inputEntries);
	      average->SetBinError(bin,inputRMS/sqrtEntries);
	      noise->SetBinError(bin,inputRMS/(sqrtEntries*sqrt(2)) );
	      total->SetBinError(bin,inputRMS * sqrtEntries);
	    }
	  }      
	}
	total->SetEntries(nEntries);
      }
      else {
	// average, noise only:
	for(int ix=1;ix<nbinx+1;ix++){	
	  for(int iy=1;iy<nbiny+1;iy++){
	    
	    Int_t bin = input->GetBin(ix,iy);
	    double inputEntries = input->GetBinEntries(bin);
	    if( inputEntries == -1 ) {
	      average->SetBinContent(bin,-1.);
	      average->SetBinError(bin,0.);
	      noise->SetBinContent(bin,-1.);
	      noise->SetBinError(bin,0.);
	      continue;
	    }
	    double inputContent =  input->GetBinContent(bin);
	    double inputRMS = input->GetBinError(bin);
	    	    
	    average->SetBinContent(bin,inputContent);
	    noise->SetBinContent(bin,inputRMS);
	    
	    if (inputEntries>0) {
	      average->SetBinError(bin,inputRMS/sqrt(inputEntries));
	      noise->SetBinError(bin,inputRMS/sqrt(2*inputEntries));
	    }
	  }
	}      
      }
    }
    average->SetEntries(nEntries);
    noise->SetEntries(nEntries);
  }
  else {
    //Handle all other combinations in a slightly less effiecient way, checking each bin:
    for(int ix=1;ix<nbinx+1;ix++){	
      for(int iy=1;iy<nbiny+1;iy++){

	Int_t bin = input->GetBin(ix,iy);
	double inputContent =  input->GetBinContent(bin);
	double inputRMS = input->GetBinError(bin);
	if( input->GetBinEntries(bin) == -1 ) {
	  if(average != 0) {
	    average->SetBinContent(bin,-1.);
	    average->SetBinError(bin,0.);
	  }
	  if(noise != 0) {
	    noise->SetBinContent(bin,-1.);
	    noise->SetBinError(bin,0.);
	  }
	  if(total != 0) {
	    total->SetBinContent(bin,-1.);
	    total->SetBinError(bin,0.);
	  }
	  if(occupancy != 0) {
	    occupancy->SetBinContent(bin,-1.);
	    occupancy->SetBinError(bin,0.);
	  }
	  continue;
	}

	double inputEntries = input->GetBinEntries(bin);

	if( average != 0 ) {
	  average->SetBinContent(bin,inputContent);
	  if (inputEntries>0) {
	    average->SetBinError(bin,inputRMS/sqrt(inputEntries));
	  }
	}
	if( noise != 0 ) {
	  noise->SetBinContent(bin,inputRMS);
	  if (inputEntries>0) {
	    noise->SetBinError(bin,inputRMS/sqrt(2*inputEntries));
	  }
	}
	if( occupancy != 0 ) {
	  occupancy->SetBinContent(bin,inputEntries*inv_norm);
	  occupancy->SetBinError(bin,sqrt(inputEntries)*inv_norm);       
	}
	if( total != 0 ) {
	  total->SetBinContent(bin, (inputContent * inputEntries) );
	  if (inputEntries>0) {
	    total->SetBinError(bin,inputRMS * sqrt(inputEntries));
	  }
	}
      }      
    }
    if( average != 0 ) {
      average->SetEntries(nEntries);
    }
    if( occupancy != 0 ) {
      occupancy->SetEntries(nEntries);
    }
    if ( noise != 0 ) {
      noise->SetEntries(nEntries);
    }
    if( total != 0 ) {
      total->SetEntries(nEntries);
    }
  }
}

void CaloCellVecMon::makeEtaHist(TH2F* input, TH1F* etaHist){
  //Method for building an eta histogram by integrating an etaphi histogram over phi
  
  //Check that hists are defined:
  if( (input == 0) || (etaHist == 0) ) {
    return;
  }

  int nbinx = input->GetNbinsX();
  int nbiny = input->GetNbinsY(); 
  double nEntries = input->GetEntries();
  for(int ix = 1;ix <= nbinx+1; ix++){
    double phiSummedContent = 0;
    double phiSummedError2 = 0;
    for(int iy = 1;iy <= nbiny+1; iy++){
      int bin = input->GetBin(ix,iy);
      double binContent = input->GetBinContent(bin);
      double binError = input->GetBinError(bin);
      if( (binContent == -1) && (binError == 0) ) continue;
      phiSummedContent += binContent;
      phiSummedError2  += pow(binError,2);
    }
    etaHist->SetBinContent(ix,phiSummedContent);
    etaHist->SetBinError(ix,sqrt(phiSummedError2));
  }
  etaHist->SetEntries(nEntries);
}

void CaloCellVecMon::makePhiHist(TH2F* input, TH1F* phiHist){
  //Method for building a phi histogram by integrating an etaphi histogram over eta

  //Check that this input histogram is defined, if not, return:
  if( (input == 0) || (phiHist == 0) ){
    return;
  }
 
  int nbinx = input->GetNbinsX();
  int nbiny = input->GetNbinsY(); 
  double nEntries = input->GetEntries();

  for(int iy = 1; iy <= nbiny; iy++){
    double etaSummedContent = 0;
    double etaSummedError2 = 0;
    for(int ix = 1; ix <= nbinx; ix++){
      int bin = input->GetBin(ix,iy);
      double binContent = input->GetBinContent(bin);
      double binError = input->GetBinError(bin);
      if((binContent == -1) && (binError == 0)) continue;
      etaSummedContent += binContent;
      etaSummedError2  += pow(binError,2);
    }
    phiHist->SetBinContent(iy,etaSummedContent);
    phiHist->SetBinError(iy,sqrt(etaSummedError2));
  }
  phiHist->SetEntries(nEntries);
}

void CaloCellVecMon::divideHistograms(TH2F* numerator, TH2F* denominator, TH2F* quotient){
  //Method for dividing one histogram by another where errors are fully correlated,
  //Used error, number of entries for numerator only
  //Only to be used if quotient, divisor, numerator all have the same range, etc.
  
  if( (!numerator) || (!denominator) || (!quotient) ) {
    return;
  }
  
  int nbinx = numerator->GetNbinsX();
  int nbiny = numerator->GetNbinsY(); 
  
  for(int ix = 1; ix <= nbinx; ix++) {
    for(int iy = 1; iy <= nbiny; iy++) {
      int bin = numerator->GetBin(ix,iy);
  
      double denom = denominator->GetBinContent(bin);

      double numError = numerator->GetBinError(bin);
      double denomError = denominator->GetBinError(bin);

      double numCont = numerator->GetBinContent(bin);
      //Using bin content = -1 and error = 0 as an error code, just pass it through:
      if( (numCont == -1) && (numError == 0) ) {
	quotient->SetBinContent(bin, numCont);
	quotient->SetBinError(bin, numError);
	continue;
      }
      else {
	if( (denom == -1) && (denomError == 0) ) {
	  quotient->SetBinContent(bin, denom);
	  quotient->SetBinError(bin,denomError);
	  continue;
	}
      }
      
      if(denom != 0 ) {
	quotient->SetBinContent(bin, (numCont / denom) );
	//if(numError != 0) {
	//  quotient->SetBinError(bin, (numError / denom) );
	//}
	//else {
	  quotient->SetBinError(bin, denomError / denom);
	  //}
      }
      else {
	quotient->SetBinContent(bin, 0.);
	quotient->SetBinError(bin, 0.);
      }
    }
  }
  quotient->SetEntries( numerator->GetEntries() );
}

void CaloCellVecMon::divideHistograms(TH1F* numerator, TH1F* denominator, TH1F* quotient){
  //Method for dividing one histogram by another where errors are fully correlated,
  //Used error, number of entries for numerator only
  //Only to be used if quotient, divisor, numerator all have the same range, etc.
  
  if( (!numerator) || (!denominator) || (!quotient) ) {
    return;
  }
  
  int nbinx = numerator->GetNbinsX();
  
  for(int ix = 1; ix <= nbinx; ix++) {

    double denom = denominator->GetBinContent(ix);

    double numError = numerator->GetBinError(ix);
    double numCont = numerator->GetBinContent(ix);
    //Using bin content = -1 and error = 0 as an error code, just pass it through:                            
    if( (numCont == -1) && (numError == 0) ) {
      quotient->SetBinContent(ix, numCont);
      quotient->SetBinError(ix, numError);
      continue;
    }
    else {
      double denomError = denominator->GetBinError(ix);
      if( (denom == -1) && (denomError == 0) ) {
	quotient->SetBinContent(ix, denom);
	quotient->SetBinError(ix,denomError);
	continue;
      }
    }
 
    if(denom != 0 ) {
      quotient->SetBinContent(ix, (numCont / denom) );
      quotient->SetBinError(ix, (numError / denom) );
    } 
    else {
      quotient->SetBinContent(ix,0.);
      quotient->SetBinError(ix,-1.);
    }
  }
  quotient->SetEntries( numerator->GetEntries() );
}

void CaloCellVecMon::divideHistogram(TH2F* numerator, float denominator, TH2F* quotient){
  //Method for dividing one histogram by another where errors are fully correlated,
  //Used error, number of entries for numerator only
  //Only to be used if quotient, divisor, numerator all have the same range, etc.
  
  if( (!numerator) || (denominator == 0) || (!quotient) ) {
    return;
  }
  
  int nbinx = numerator->GetNbinsX();
  int nbiny = numerator->GetNbinsY(); 
  

  const double inv_denominator = 1. / denominator;
  for(int ix = 1; ix <= nbinx; ix++) {
    for(int iy = 1; iy <= nbiny; iy++) {
      int bin = numerator->GetBin(ix,iy);
      double binError = numerator->GetBinError(bin);
      double binContent = numerator->GetBinContent(bin);
      if( (binContent == -1) && (binError == 0) ) {
	quotient->SetBinContent(bin, binContent );
	quotient->SetBinError(bin, binError);
      }
      else {
	quotient->SetBinContent(bin, (binContent * inv_denominator) );
	quotient->SetBinError(bin, (binError * inv_denominator) );
      }
    }
  }
  quotient->SetEntries( numerator->GetEntries() );
}

void CaloCellVecMon::divideHistogram(TH1F* numerator, float denominator, TH1F* quotient){
  //Method for dividing one histogram by another where errors are fully correlated,
  //Used error, number of entries for numerator only
  //Only to be used if quotient, divisor, numerator all have the same range, etc.
  
  if( (!numerator) || (denominator == 0) || (!quotient) ) {
    return;
  }
  
  int nbinx = numerator->GetNbinsX();
  const double inv_denominator = 1. / denominator;
  
  for(int ix = 1; ix <= nbinx; ix++) {
    double binCont = numerator->GetBinContent(ix);
    double binError = numerator->GetBinError(ix);
    if( (binCont == -1) && (binError == 0) ) {
      //For binCont == -1 and binError == 0 bin content and error as an error code: just pass it through
      quotient->SetBinContent(ix, binCont);
      quotient->SetBinError(ix,binError);
    }
    else {
      quotient->SetBinContent(ix, (binCont * inv_denominator) );
      quotient->SetBinError(ix, (binError * inv_denominator) );
    }
  }
  quotient->SetEntries( numerator->GetEntries() );
}

template< class THist > 
void CaloCellVecMon::setCorrectEntries(THist ** hist[],int layer, int thresh) {
  
  if(hist[layer] == 0) {
    return;
  }
  if(hist[layer][thresh] == 0) {
    return;
  }
  
  if(m_useWeightedEffMerging) {
    int nEntries = 0;
    if(m_useTrigger) {
      nEntries = m_nEventsPassingTrigger[layer/2][thresh];
    }
    else {
      nEntries = m_nEventsPassingBeamBG[layer/2][thresh];
//      nEntries = m_eventsCounter;
    }
    hist[layer][thresh]->SetEntries(nEntries);
  }
}

void CaloCellVecMon::unpackToPercentageDifference(TProfile2D* input, TH2F* baseline, TH2F* output) {
  // find the percentage difference of rms noise from TProfile from baseline,

  if( (!input) || (!baseline) || (!output) ) {
    return;
  }

  int nbinx = input->GetNbinsX();
  int nbiny = input->GetNbinsY(); 
   
  for(int ix=1;ix<nbinx+1;ix++){	
    for(int iy=1;iy<nbiny+1;iy++){
      
      Int_t bin = input->GetBin(ix,iy);
      double inputEntries = input->GetBinEntries(bin);

      //Check for empty bin error code:
      if( inputEntries == -1 ) {
	output->SetBinContent(bin, -101.);
	output->SetBinError(bin, 0.);
	continue;
      }
      double inputRMS = input->GetBinError(bin);
      double baseValue = baseline->GetBinContent(bin);

      if( (baseValue != 0) && (inputEntries != 0) ) {
	output->SetBinContent(bin, 100 * (inputRMS - baseValue) / baseValue );
	output->SetBinError(bin, 100 * inputRMS/( sqrt(2*inputEntries) * baseValue ) );
      }
      else {
	output->SetBinContent(bin, -100.);
	output->SetBinError(bin, 0.);
      }
    }
  }
  output->SetEntries( input->GetEntries() );
}

void CaloCellVecMon::unpackToPercentageDifference(TProfile2D* input, TH2F* output) {
  // find the percentage difference of rms noise from TProfile assuming that the
  // baseline has already been divided out.
  if( (!input) || (!output) ) {
    return;
  }

  int nbinx = input->GetNbinsX();
  int nbiny = input->GetNbinsY(); 
   
  for(int ix=1;ix<nbinx+1;ix++){	
    for(int iy=1;iy<nbiny+1;iy++){
      
      Int_t bin = input->GetBin(ix,iy);

      double inputEntries = input->GetBinEntries(bin);
      if( inputEntries == -1 ) {
	output->SetBinContent(bin, -101.);
	output->SetBinError(bin, 0.);
      }
      double inputRMS = input->GetBinError(bin);
      
      if(inputEntries != 0) {
	output->SetBinContent(bin, 100 * (inputRMS - 1 ) );
	output->SetBinError(bin, 100 * inputRMS/sqrt(2*inputEntries) );
      }
      else {
	output->SetBinContent(bin, -100.);
	output->SetBinError(bin, 0.);
      }
    }
  }
  output->SetEntries( input->GetEntries() );
}

//////////////////////////////////////////
//helpers
void CaloCellVecMon::find_n(std::string partition_name,int & name_flag){
  std::string::size_type where = partition_name.find_first_of("_");
  if (where != std::string::npos)
    partition_name.erase (where, std::string::npos);
  if(partition_name.compare("EMBC")==0) name_flag = 1;
  if(partition_name.compare("EMBA")==0) name_flag = 2;
  if(partition_name.compare("EMECC")==0) name_flag = 3;
  if(partition_name.compare("EMECA")==0) name_flag = 4;
  if(partition_name.compare("HECC")==0)  name_flag = 5;
  if(partition_name.compare("HECA")==0)  name_flag = 6;
  if(partition_name.compare("FCALC")==0) name_flag = 7;
  if(partition_name.compare("FCALA")==0) name_flag = 8;
}

void CaloCellVecMon::adjust_n_1d(TH1F* & tmp_h){
 if(tmp_h){
  std::string name_tmp;
  std::string name_tmp2;
  name_tmp = tmp_h->GetName();
  int where = name_tmp.find_first_of("_");
  name_tmp2 = name_tmp.substr(where+1);
  tmp_h->SetName(name_tmp2.c_str());
 }
}

void CaloCellVecMon::adjust_n_2d(TProfile*& tmp_h){
 if(tmp_h){
  std::string name_tmp;
  std::string name_tmp2;
  name_tmp = tmp_h->GetName();
  int where = name_tmp.find_first_of("_");
  name_tmp2 = name_tmp.substr(where+1);
  tmp_h->SetName(name_tmp2.c_str());
 }
}

// Hashes to the Layer enumeration determine threshold properties in threshold arrays 
int CaloCellVecMon::hashToLayer(CaloLayer::Partition partition, int layerNum, CaloLayer::Side side) {
  if(partition != CaloLayer::MAXPART)
    return 8*partition+2*layerNum+side;
  else
    return CaloCellVecMon::MAXLAYER;
}


