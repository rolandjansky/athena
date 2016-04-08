/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// This tool is the head of the monitoring. It is called by the job options and
// given all the flags. This tool the does the common overhead jobs and calls
// functions which do each individual job. One function to book and fill each 
// storegate type for each environment.
// For example, filling cluster histograms for Details modules.
//
// Written by: Nathan Triplett (nathan.triplett@cern.ch), Cecile Lapoire (cecile.lapoire@cern.ch)
// Maintained by: Louise Skinnari (louise.skinnari@cern.ch)
///////////////////////////////////////////////////////////////////////////////

#include "PixelMonitoring/PixelMainMon.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
//#include "TrkParameters/MeasuredAtaPlane.h"
//#include "TrkParameters/MeasuredPerigee.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSpacePoint/SpacePointContainer.h"
#include "TrkTrack/TrackCollection.h"
//#include "StoreGate/StoreGateSvc.h"
//#include "StoreGate/StoreGate.h"
#include "PixelConditionsServices/IPixelByteStreamErrorsSvc.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "TH2S.h"
#include "TProfile2D.h"
#include "PixelMonitoring/DBMMon2DMaps.h"
#include "PixelMonitoring/PixelMon2DMaps.h"
#include "PixelMonitoring/PixelMonProfiles.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH1I_LW.h"
#include "LWHists/TProfile_LW.h"
#include "LWHists/TProfile2D_LW.h"

#include <stdint.h>
#include <vector>
#include <sstream>
#include <fstream> // kazuki
#include <cstdlib> // kazuki
#include <map> // kazuki

#include "GaudiKernel/StatusCode.h"
//#include "Identifier/Identifier.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetIdentifier/PixelID.h"
#include "PixelCabling/IPixelCablingSvc.h"
#include "LumiBlockComps/ILuminosityTool.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"

//////////////////////////////////////////////////////////////////////////////

std::vector<std::string> &splitter(const std::string &s, char delim, std::vector<std::string> &elems) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

std::vector<std::string> splitter(const std::string &s, char delim) {
  std::vector<std::string> elems;
  splitter(s, delim, elems);
  return elems;
}

bool is_file_exist(const char *fileName)
{
  std::ifstream infile(fileName);
  return infile.good();
}

PixelMainMon::PixelMainMon(const std::string & type, 
      const std::string & name,
      const IInterface* parent) :
   ManagedMonitorToolBase(type, name, parent),
   m_pixelCondSummarySvc("PixelConditionsSummarySvc",name),
   m_ErrorSvc("PixelByteStreamErrorsSvc",name),
   m_pixelCableSvc("PixelCablingSvc",name),
   m_IBLParameterSvc("IBLParameterSvc",name),
   m_holeSearchTool("InDet::InDetTrackHoleSearchTool/InDetHoleSearchTool"),
   m_lumiTool("LuminosityTool"),
   m_moduleTemperature(new dcsDataHolder()),
   m_coolingPipeTemperatureInlet(new dcsDataHolder()),
   m_coolingPipeTemperatureOutlet(new dcsDataHolder()),
   m_HV(new dcsDataHolder()),
   m_HV_current(new dcsDataHolder()),
   m_LV_voltage(new dcsDataHolder()),
   m_LV_current(new dcsDataHolder()),
   m_FSM_state(new dcsDataHolder()),
   m_FSM_status(new dcsDataHolder()),
   m_moduleDCSDataHolder(new moduleDcsDataHolder())
   //m_trkSummaryTool("Trk::TrackSummaryTool/InDetTrackSummaryTool")
{                                                                        //all job options flags go here
   declareProperty("PixelConditionsSummarySvc", m_pixelCondSummarySvc);
   declareProperty("PixelByteStreamErrorsSvc",  m_ErrorSvc);
   declareProperty("PixelCablingSvc",           m_pixelCableSvc);
   declareProperty("HoleSearchTool",  m_holeSearchTool);
   declareProperty("LuminosityTool",  m_lumiTool);

   //declareProperty("TrkSummaryTool",            m_trkSummaryTool);

   declareProperty("RDOName",         m_Pixel_RDOName         = "PixelRDOs");  //storegate container names
   declareProperty("RODErrorName",    m_detector_error_name   = "pixel_error_summary");   
   declareProperty("SpacePointName",  m_Pixel_SpacePointsName = "PixelSpacePoints"); 
   declareProperty("ClusterName",     m_Pixel_SiClustersName  = "PixelClusters");  
   declareProperty("TrackName",       m_TracksName            = "Pixel_Cosmic_Tracks");         

   declareProperty("onTrack",         m_doOnTrack        = false); //using inner detector tracks
   declareProperty("onPixelTrack",    m_doOnPixelTrack   = false); //using pixel only tracks
   declareProperty("do2DMaps",        m_do2DMaps         = false);   
   declareProperty("doModules",       m_doModules        = false); 
   declareProperty("doFEChipSummary", m_doFEChipSummary  = false);
   declareProperty("doOffline",       m_doOffline        = false);
   declareProperty("doOnline",        m_doOnline         = false);
   declareProperty("doLowOccupancy",  m_doLowOccupancy   = false);
   declareProperty("doHighOccupancy", m_doHighOccupancy  = false);
   declareProperty("doPixelOccupancy",m_doPixelOccupancy = false); 
   declareProperty("doRodSim",        m_doRodSim         = false);
   declareProperty("doDetails",       m_doDetails        = false);
   declareProperty("doSpectrum",      m_doSpectrum       = false);
   declareProperty("doNoiseMap",      m_doNoiseMap       = false);
   declareProperty("doTiming",        m_doTiming         = false);
   declareProperty("doLumiBlock",     m_doLumiBlock      = false);

   declareProperty("doRDO",           m_doRDO        = false); //flags to turn on/off parts of the code
   declareProperty("doErrors",        m_doRODError   = false);
   declareProperty("doSpacePoint",    m_doSpacePoint = false);
   declareProperty("doCluster",       m_doCluster    = false);
   declareProperty("doTrack",         m_doTrack      = false);
   declareProperty("doHoleSearch",    m_doHoleSearch   = false);
   declareProperty("doStatus",        m_doStatus     = false);
   declareProperty("doDCS",           m_doDCS        = false);

   declareProperty("doDegFactorMap",   m_doDegFactorMap = true);
   declareProperty("doHeavyIonMon",   m_doHeavyIonMon = false);

   declareProperty("doIBL",           m_doIBL = false);
   declareProperty("doESD",           m_doESD = false);

   declareProperty("DetailsMod1",     m_DetailsMod1 = "");
   declareProperty("DetailsMod2",     m_DetailsMod2 = "");
   declareProperty("DetailsMod3",     m_DetailsMod3 = "");
   declareProperty("DetailsMod4",     m_DetailsMod4 = "");
   declareProperty("OccupancyCut",    m_occupancy_cut = 1e-5);

   m_isNewRun = false;
   m_isNewLumiBlock = false;
   m_newLowStatInterval = false;
   m_doRefresh = false;
   m_firstBookTime = 0;
   isFirstBook = false;
   m_nRefresh = 0;

   //initalize all the histograms to 0 to start
   m_events_per_lumi = 0;
   m_storegate_errors = 0;
   m_mu_vs_lumi = 0;
   m_hiteff_mod = 0;
   m_hits_per_lumi = 0;

   m_avgocc_per_lumi = 0;

   m_Lvl1A = 0;
   m_Lvl1ID_PIX = 0;                
   m_Lvl1ID_IBL = 0;                
   m_Lvl1ID_diff_mod_ATLAS_per_LB = 0;
   m_Lvl1ID_absdiff_mod_ATLAS_per_LB = 0;
   m_BCID = 0;                  
   m_Atlas_BCID = 0;            
   m_Atlas_BCID_hits = 0;            
   m_BCID_Profile = 0;          
   m_diff_ROD_BCID = 0;         
   m_occupancy = 0;
   m_occupancy_10min = 0;
   m_average_pixocc = 0;
   m_avgocc_ratioIBLB0_per_lumi = 0;
   m_hitmap_tmp = 0;
   m_FE_chip_hit_summary = 0;
   m_occupancy_time1 = 0;
   m_occupancy_time2 = 0;
   m_occupancy_time3 = 0;
   m_num_hits = 0;
   m_num_hits_low = 0;
   m_RodSim_BCID_minus_ToT = 0;
   m_RodSim_FrontEnd_minus_Lvl1ID = 0;
   m_Details_mod1_num_hits = 0;
   m_Details_mod2_num_hits = 0;
   m_Details_mod3_num_hits = 0;
   m_Details_mod4_num_hits = 0;
   m_Details_mod1_occupancy = 0;
   m_Details_mod2_occupancy = 0;
   m_Details_mod3_occupancy = 0;
   m_Details_mod4_occupancy = 0;
   m_Details_mod1_ToT = 0;     
   m_Details_mod2_ToT = 0;     
   m_Details_mod3_ToT = 0;     
   m_Details_mod4_ToT = 0;  
   m_pixel_occupancy = 0;
   m_pixel_noise_map = 0;
   m_pixel_occupancy_spectrum = 0;
   m_hit_num_mod = 0;

   //-------------------
   // track histograms
   //-------------------
   m_degFactorMap = 0;
   m_degFactorMap_per_lumi = 0;
   m_degFactorMap_eta_per_lumi = 0;
   //m_degFactorMap_phi_per_lumi = 0;
   m_track_res_phi = 0; 
   m_track_pull_phi = 0;
   m_track_res_eta = 0; 
   m_track_pull_eta = 0;
   m_track_chi2 = 0;
   m_track_chi2_LB = 0;
   m_track_chi2_bcl1 = 0;
   m_track_chi2_bcl0 = 0;
   m_track_chi2_bclgt1 = 0;
   m_track_chi2_bcl1_highpt = 0;
   m_track_chi2_bcl0_highpt = 0;
   m_track_chi2_bclgt1_highpt = 0;
   //m_track_clusToT = 0;
   //m_track_qOverP = 0;  
   //m_track_qOverP_inv = 0;  
   //m_track_pt = 0;  
   //m_track_d0 = 0;      
   //m_track_z0 = 0;     
   //m_track_phi0 = 0;    
   //m_track_theta = 0;   
   //m_track_eta = 0;    
   m_tracks_per_lumi = 0;
   m_trackRate_per_lumi = 0;
   m_tracksPerEvt_per_lumi = 0;
   m_tracksPerEvtPerMu_per_lumi = 0;
   //m_track_dedx = 0;
   //m_track_mass_dedx = 0;
   m_clustot_vs_pt = 0;
   m_clustot_lowpt = 0;
   m_1hitclustot_lowpt = 0;
   m_2hitclustot_lowpt = 0;
   m_clustot_highpt = 0;
   m_1hitclustot_highpt = 0;
   m_2hitclustot_highpt = 0;
   m_tsos_hiteff_vs_lumi = 0;
   m_tsos_hitmap = 0;
   m_tsos_holemap = 0;
   m_tsos_outliermap = 0;
   m_clusters_per_lumi = 0;
   m_largeclusters_per_lumi = 0;
   m_verylargeclusters_per_lumi = 0;
   m_totalclusters_per_lumi = 0;
   m_highNclusters_per_lumi = 0;
   m_clussize_map = 0;
   m_cluscharge_map = 0;
   m_clusToT_map = 0;
   m_cluster_groupsize = 0;
   m_cluster_col_width = 0;
   m_cluster_row_width = 0;
   m_cluster_row_width_IBL = 0;
   m_cluster_LVL1A = 0;
   m_clus_LVL1A_sizenot1 = 0; 
   m_cluster_LVL1A_highToT = 0;
   m_clusterSize_eta = 0;
   m_cluster_occupancy = 0;
   m_clusocc_DBM = 0;
   m_clusocc_sizenot1 = 0; 
   m_cluseff_mod = 0;
   m_cluster_ToT_mod = 0;
   m_cluster_size_mod = 0;
   m_cluster_num_mod = 0;
   m_cluster_occupancy_time1 = 0;
   m_cluster_occupancy_time2 = 0;
   m_cluster_occupancy_time3 = 0;
   m_num_clusters = 0;
   m_num_clusters_PIX = 0;
   m_num_clusters_low = 0;
   m_num_clusters_ECA = 0;
   m_num_clusters_ECC = 0;
   m_num_clusters_IBL = 0;
   m_num_clusters_B0 = 0;
   m_num_clusters_B1 = 0;
   m_num_clusters_B2 = 0;
   m_ecA_cluster_occupancy_summary = 0;
   m_ecC_cluster_occupancy_summary = 0;
   m_bar_layI_cluster_occupancy_summary = 0;
   m_bar_lay0_cluster_occupancy_summary = 0;
   m_bar_lay1_cluster_occupancy_summary = 0;
   m_bar_lay2_cluster_occupancy_summary = 0;
   m_ecA_cluster_occupancy_summary_low = 0;
   m_ecC_cluster_occupancy_summary_low = 0;
   m_bar_layI_cluster_occupancy_summary_low = 0;
   m_bar_lay0_cluster_occupancy_summary_low = 0;
   m_bar_lay1_cluster_occupancy_summary_low = 0;
   m_bar_lay2_cluster_occupancy_summary_low = 0;
   m_cluster_LVL1A_mod = 0;
   m_clustersOnOffTrack_per_lumi = 0;
   m_Status_modules = 0;
   m_status = 0;
   m_dqStatus = 0;
   m_error1 = 0;            
   m_errorTypes = 0;        
   m_otherROD_per_lumi_IBL = 0; 
   m_chip_per_lumi_IBL = 0; 
   m_error_time1 = 0;       
   m_error_time2 = 0;       
   m_error_time3 = 0;       
   m_errors = 0;
   m_OpticalErrors = 0;
   m_SEU_Errors = 0;
   m_TimeoutErrors = 0;
   m_SyncErrorsIBL = 0; 
   m_SyncErrors = 0; 
   m_SyncCategory = 0; 
   m_TruncationCategory = 0; 
   m_OpticalCategory = 0; 
   m_SEUCategory = 0; 
   m_TimeoutCategory = 0; 
   m_TruncationErrors = 0;
   m_sync_mod_BCID1_per_LB = 0;
   m_sync_mod_BCID1_int_LB = 0;
   m_sync_mod_BCID2_per_LB = 0;
   m_sync_mod_BCID2_int_LB = 0;
   m_sync_mod_LVL1ID_per_LB = 0;
   m_sync_mod_LVL1ID_int_LB = 0;
   m_sync_rod_BCID_per_LB = 0;
   m_sync_rod_BCID_int_LB = 0;
   m_sync_rod_LVL1ID_per_LB = 0;
   m_sync_rod_LVL1ID_int_LB = 0;
   m_trunc_mod_EOC_per_LB = 0;
   m_trunc_mod_EOC_int_LB = 0;
   m_trunc_mod_hitOF_per_LB = 0;
   m_trunc_mod_hitOF_int_LB = 0;
   m_trunc_mod_EoEOF_per_LB = 0;
   m_trunc_mod_EoEOF_int_LB = 0;
   m_trunc_rod_HTlim_per_LB = 0;
   m_trunc_rod_HTlim_int_LB = 0;
   m_trunc_rod_FIFOOF_per_LB = 0;
   m_trunc_rod_FIFOOF_int_LB = 0;
   m_optical_error_per_LB = 0;
   m_optical_error_int_LB = 0;
   m_seu_hit_parity_per_LB = 0;
   m_seu_hit_parity_int_LB = 0;
   m_seu_reg_parity_per_LB = 0;
   m_seu_reg_parity_int_LB = 0;
   m_seu_hamming_per_LB = 0;
   m_seu_hamming_int_LB = 0;
   m_timeout_per_LB = 0;
   m_timeout_int_LB = 0;
   m_FEwarning_per_LB = 0;
   m_FEwarning_int_LB = 0;
   m_sync_mod_corr_LVL1ID_BCID2_per_LB = 0;
   m_sync_rod_corr_LVL1ID_BCID_per_LB = 0;
   m_num_spacepoints = 0;
   m_num_spacepoints_low = 0;
   m_spHit_x = 0;
   m_spHit_y = 0; 
   m_spHit_z = 0;
   m_spHit_r = 0;
   m_spHit_phi = 0;
   m_spHit_xy = 0;
   m_spHit_rz = 0;
   m_cluster_occupancy_LB = 0;
   m_errors_LB = 0;           
   m_status_LB = 0;           
   m_cluster_ToT_mod_LB = 0;
   m_cluster_num_mod_LB = 0;
   m_hit_num_mod_LB = 0;
   m_num_hits_LB = 0;
   m_num_hits_low_LB = 0;
   m_cluster_ToT_LB = 0;      
   m_num_clusters_LB = 0;                                
   m_num_clusters_low_LB = 0;  
   m_hitmap_mon = 0;

   m_pixelid =0;
   pixelmgr =0;
   m_event =0;
   m_event2 =0;
   m_startTime =0; 
   m_majorityDisabled =0;
   m_lumiBlockNum =0;
   m_currentTime =0;
   m_LBstartTime =0;
   m_LBendTime =0;
   m_runNum =0;
   m_idHelper =0;
   m_Pixel_clcontainer =0;
   m_Pixel_spcontainer =0;
   m_tracks =0;

   m_nGood_ECA=0;
   m_nGood_ECC=0;
   m_nGood_IBL2D=0;
   m_nGood_IBL3D=0;
   m_nGood_B0=0;
   m_nGood_B1=0;
   m_nGood_B2=0;

   for( int i=0; i<PixLayer::COUNT; i++){
      m_nlargeevt_per_lumi_mod[i] = 0;
      m_ToT_etaphi_mod[i] = 0;
      m_occupancy_summary_mod[i] = 0;
      m_occupancy_summary_low_mod[i] = 0;
      m_nhits_mod[i] = 0;
      m_diff_ROD_vs_Module_BCID_mod[i] = 0;
      m_Lvl1ID_diff_mod_ATLAS_mod[i] = 0;
      //m_Lvl1A_mod[i] = 0;
      m_Lvl1A_10min_mod[i] = 0;
      m_hit_ToT_tmp_mod[i] = 0;
      m_hit_ToT_Mon_mod[i] = 0;
      m_avgocc_per_lumi_mod[i] = 0;
      m_maxocc_per_lumi_mod[i] = 0;
      m_hits_per_lumi_mod[i] = 0;
      m_clusters_per_lumi_mod[i] = 0;
      m_totalclusters_per_lumi_mod[i] = 0;
      m_cluster_col_width_mod[i] = 0;
      m_cluster_row_width_mod[i] = 0;
      m_cluster_groupsize_mod[i] = 0;
      m_clusQ_vs_eta_mod[i] = 0;
      m_clussize_vs_eta_mod[i] = 0;
      m_bad_mod_errors_mod[i] = 0;
      m_errors_etaid_mod[i] = 0;
      m_hit_ToT_LB_mod[i] = 0;
      m_clusToT_vs_eta_mod[i] = 0;
      m_ToT_vs_clussize_mod[i] = 0;
      m_Q_vs_clussize_mod[i] = 0;
      m_nlowToT_vs_clussize_mod[i] = 0;
      m_1cluster_ToT_mod[i] = 0;
      m_2cluster_ToT_mod[i] = 0;
      m_3cluster_ToT_mod[i] = 0;
      m_bigcluster_ToT_mod[i] = 0;
      m_1cluster_Q_mod[i] = 0;
      m_2cluster_Q_mod[i] = 0;
      m_3cluster_Q_mod[i] = 0;
      m_bigcluster_Q_mod[i] = 0;
      m_cluster_LVL1A1d_mod[i] = 0;
   }
   for( int j=0; j<16; j++){
      m_errors_int_LB[j] = 0;
   }
   for( int i=0; i<PixLayerIBL2D3D::COUNT; i++){
      m_errors_per_lumi_mod[i] = 0;
      m_SyncErrors_per_lumi_mod[i] = 0;
      m_OpticalErrors_per_lumi_mod[i] = 0;
      m_SEU_Errors_per_lumi_mod[i] = 0;
      m_TruncationErrors_per_lumi_mod[i] = 0;
      m_TimeoutErrors_per_lumi_mod[i] = 0;
      m_ErrorBit_per_lumi_mod[i] = 0;
      m_Error_per_lumi_mod[i] = 0;
      m_cluster_ToT1d_mod[i] = 0;
      m_cluster_Q_mod[i] = 0;
      m_badModules_per_lumi_mod[i] = 0;
      m_disabledModules_per_lumi_mod[i] = 0;
      m_baddisabledModules_per_lumi_mod[i] = 0;
      for( int j=0; j<ErrorCategory::COUNT; j++){
         m_ErrorFraction_per_evt[j][i] = 0;
         m_ErrorCategoryMap[j] = 0;
      }
   }
   for( int i=0; i<PixLayerIBL2D3DDBM::COUNT; i++){
      m_hit_ToT[i] = 0;
   }
   for( int i=0; i<PixLayerDBM::COUNT; i++){
      m_Lvl1A_mod[i] = 0;
   }
   for( int i=0; i<ErrorCategoryMODROD::COUNT; i++){
      m_ErrorTypeMap[i] = 0;
   }
   for( int i=0; i<16; i++){
      m_ErrorStateMap[i] = 0;
      m_ErrorStateMap_per_LB[i] = 0;
   }

  // DCS Monitorning
  for( int ii = 0; ii < IBLStave::COUNT; ii++){
    // TEMPERATURE
    m_hist_moduleTemperature2Dscatter[ii] = 0;
    m_hist_moduleTemperatureLB[ii] = 0;
    m_hist_LB_moduleGroup_moduleTemperature[ii] = 0;
    // HV
    m_hist_HVoltage2Dscatter[ii] = 0;
    m_hist_HVoltageLB[ii] = 0;
    m_hist_LB_moduleGroup_HVoltage[ii] = 0;
    // FSM state
    m_hist_FSMstate2Dscatter[ii] = 0;
    m_hist_FSMstateLB[ii] = 0;
    m_hist_LB_moduleGroup_FSMstate[ii] = 0;
    // FSM status
    m_hist_FSMstatus2Dscatter[ii] = 0;
    m_hist_FSMstatusLB[ii] = 0;
    m_hist_LB_moduleGroup_FSMstatus[ii] = 0;
    // PIPES Inlet
    m_hist_Pipes_inletLB[ii] = 0;
    //m_hist_LB_moduleGroup_coolingPipeInlet[ii] = 0;
    // PIPES Outlet
    m_hist_Pipes_outletLB[ii] = 0;
    //m_hist_LB_moduleGroup_coolingPipeOutlet[ii] = 0;
    // LV
    m_hist_LVoltage2Dscatter[ii] = 0;
    m_hist_LB_moduleGroup_LVoltage[ii] = 0;
    m_hist_LB_moduleGroup_LVoltage[ii] = 0;
    // LV current
    m_hist_LVcurrent2Dscatter[ii] = 0;
    m_hist_LVcurrentLB[ii] = 0;
    m_hist_LB_moduleGroup_LVcurrent[ii] = 0;
    // HV current
    m_hist_HVcurrent2Dscatter[ii] = 0;
    m_hist_HVcurrentLB[ii] = 0;
    m_hist_LB_moduleGroup_HVcurrent[ii] = 0;
    // dT
    m_hist_LB_moduleGroup_dT[ii] = 0; // dT := module temp - cooling pipe outlet
    // LV P C
    m_hist_LB_moduleGroup_LVPowerConsumption[ii] = 0;
    // HV P C
    m_hist_LB_moduleGroup_HVPowerConsumption[ii] = 0;
    // LV+HV P C
    m_hist_LB_moduleGroup_LVHVPowerConsumption[ii] = 0;
    // effective FLEX temp
    m_hist_LB_moduleGroup_effFLEXtemp[ii] = 0;
    // thermal figure of merit
    m_hist_LB_moduleGroup_thermalFigureMerit[ii] = 0;
  } // end for loop over IBLStave
  // TEMPERATURE
  m_hist_moduleTemperatureEtaPhi = 0;
  m_hist_LB_staveID_moduleTemperature = 0;
  // HV
  m_hist_HVoltageEtaPhi = 0;
  m_hist_LB_staveID_HVoltage = 0;
  // FSM state
  m_hist_FSMstateEtaPhi = 0;
  m_hist_LB_staveID_FSMstate = 0;
  // FSM status
  m_hist_FSMstatusEtaPhi = 0;
  m_hist_LB_staveID_FSMstatus = 0;
  // PIPES Inlet
  //m_hist_Pipes_inletEtaPhi = 0;
  m_hist_Pipes_inlet2Dscatter = 0;
  m_hist_LB_staveID_coolingPipeInlet = 0;
  // PIPES Outlet
  //m_hist_Pipes_outletEtaPhi = 0;
  m_hist_Pipes_outlet2Dscatter = 0;
  m_hist_LB_staveID_coolingPipeOutlet = 0;
  // LV
  m_hist_LVoltageEtaPhi = 0;
  m_hist_LB_staveID_LVoltage = 0;
  // LV current
  m_hist_LVcurrentEtaPhi = 0;
  m_hist_LB_staveID_LVcurrent = 0;
  // HV current
  m_hist_HVcurrentEtaPhi = 0;
  m_hist_LB_staveID_HVcurrent= 0;
  // dT
  m_hist_dTEtaPhi = 0;
  m_hist_LB_staveID_dT = 0;
  // LV P C
  m_hist_LVPowerConsumptionEtaPhi = 0;
  m_hist_LB_staveID_LVPowerConsumption = 0;
  // HV P C
  m_hist_HVPowerConsumptionEtaPhi = 0;
  m_hist_LB_staveID_HVPowerConsumption = 0;
  // LV HV P C
  m_hist_LVHVPowerConsumptionEtaPhi = 0;
  m_hist_LB_staveID_LVHVPowerConsumption = 0;
  // eff flex temp
  m_hist_effFLEXtempEtaPhi = 0;
  m_hist_LB_staveID_effFLEXtemp = 0;
  // tfm
  m_hist_thermalFigureMeritEtaPhi = 0;
  m_hist_LB_staveID_thermalFigureMerit = 0;
}
   
PixelMainMon::~PixelMainMon()
{
    delete m_moduleTemperature;
    delete m_coolingPipeTemperatureInlet;
    delete m_coolingPipeTemperatureOutlet;
    delete m_HV;
    delete m_HV_current;
    delete m_LV_voltage;
    delete m_LV_current;
    delete m_FSM_state;
    delete m_FSM_status;
    delete m_moduleDCSDataHolder;

}


StatusCode PixelMainMon::initialize()
{
   sc = ManagedMonitorToolBase::initialize();
   if(!sc.isSuccess()) return sc;
   time ( &m_startTime );  //mark time for start of run
   m_idHelper = new AtlasDetectorID;

   // Retrieve tools
   if (detStore()->retrieve(m_pixelid, "PixelID").isFailure()) {
      msg(MSG::FATAL) << "Could not get Pixel ID helper" << endreq; 
      return StatusCode::FAILURE;
   }

   // Get the dictionary manager from the detector store

   sc = detStore()->retrieve(m_idHelper, "AtlasID");
   if (sc.isFailure()) {
     msg(MSG::ERROR) << "Could not get ID helper !" << endreq;
     return sc;
   }

   if ( m_pixelCondSummarySvc.retrieve().isFailure() ) {
      if(msgLvl(MSG::FATAL)) msg(MSG::FATAL)  << "Failed to retrieve tool " << m_pixelCondSummarySvc << endreq;
      return StatusCode::FAILURE;
   } else {
      if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Retrieved tool " << m_pixelCondSummarySvc << endreq;
   }

   if ( m_pixelCableSvc.retrieve().isFailure() ) {
      if(msgLvl(MSG::FATAL)) msg(MSG::FATAL)  << "Failed to retrieve tool " << m_pixelCableSvc << endreq;
      return StatusCode::FAILURE;
   } else {
      if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Retrieved tool " << m_pixelCableSvc << endreq;
   }

   if ( m_ErrorSvc.retrieve().isFailure() ) {
      if(msgLvl(MSG::FATAL)) msg(MSG::FATAL)  << "Failed to retrieve tool " << m_ErrorSvc << endreq;
      return StatusCode::FAILURE;
   } else {
      if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Retrieved tool " << m_ErrorSvc << endreq;
   }

   if (m_IBLParameterSvc.retrieve().isFailure()) {
      if(msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Could not retrieve IBLParameterSvc" << endreq;
      return StatusCode::FAILURE;
   } else {
      if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Retrieved tool " << m_IBLParameterSvc << endreq;
   }

   m_doIBL = m_IBLParameterSvc->containsIBL();
   msg(MSG::INFO)<<"doIBL set to "<<m_doIBL<<endreq;

   if(m_doIBL && !m_IBLParameterSvc->contains3D()){
      msg(MSG::WARNING)<<"Assuming hybrid 2D/3D IBL module composition, but geometry is all-planar"<<endreq;
   }

   if(m_doHoleSearch){
      if ( m_holeSearchTool.retrieve().isFailure() ) {
         msg(MSG::FATAL) << "Failed to retrieve tool " << m_holeSearchTool << endreq;
         return StatusCode::FAILURE;
      }else{
         msg(MSG::INFO) << "Retrieved tool " << m_holeSearchTool << endreq;
      }
   }

   if ( m_lumiTool.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_lumiTool << endreq;
      return StatusCode::FAILURE;
   } else {
      msg(MSG::INFO) << "Retrieved tool " << m_lumiTool << endreq;
   }

   //if ( m_trkSummaryTool.retrieve().isFailure() ) {
   //  if ( msgLvl(MSG::ERROR) ) msg(MSG::ERROR) << "Failed to retrieve tool " << m_trkSummaryTool << endreq;
   //} else {
   //  if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Retrieved tool " << m_trkSummaryTool << endreq;
   //}

  //m_elementsMap[std::string("/PIXEL/DCS/FSMSTATE")].push_back(std::string("FSM_state"));                    // type: String4k
  //m_elementsMap[std::string("/PIXEL/DCS/FSMSTATUS")].push_back(std::string("FSM_status"));                  // type: String4k
  //m_elementsMap[std::string("/PIXEL/DCS/HV")].push_back(std::string("HV"));                                 // type: Float
  //m_elementsMap[std::string("/PIXEL/DCS/HVCURRENT")].push_back(std::string("hv_current"));                  // type: Float
  //m_elementsMap[std::string("/PIXEL/DCS/LV")].push_back(std::string("lv_voltage"));                         // type: Float
  //m_elementsMap[std::string("/PIXEL/DCS/LV")].push_back(std::string("lv_current"));                         // type: Float
  //m_elementsMap[std::string("/PIXEL/DCS/PIPES")].push_back(std::string("temp_inlet"));                      // type: Float
  //m_elementsMap[std::string("/PIXEL/DCS/PIPES")].push_back(std::string("temp_outlet"));                     // type: Float
  //m_elementsMap[std::string("/PIXEL/DCS/PLANTS")].push_back(std::string("modbus_ack_user_setpoint_temp"));  // type: Float
  //m_elementsMap[std::string("/PIXEL/DCS/PLANTS")].push_back(std::string("modbus_cooling_ready"));           // type: Bool
  //m_elementsMap[std::string("/PIXEL/DCS/TEMPERATURE")].push_back("temperature");                            // type: Float

  if (!m_doDCS) return StatusCode::SUCCESS;

  m_atrcollist.push_back(std::string("/PIXEL/DCS/TEMPERATURE")); // module
  m_atrcollist.push_back(std::string("/PIXEL/DCS/HV"));          // readout unit
  m_atrcollist.push_back(std::string("/PIXEL/DCS/FSMSTATE"));    // readout unit
  m_atrcollist.push_back(std::string("/PIXEL/DCS/FSMSTATUS"));   // readout unit
  m_atrcollist.push_back(std::string("/PIXEL/DCS/PIPES"));       // IBL stave
  m_atrcollist.push_back(std::string("/PIXEL/DCS/HVCURRENT"));   // IBL readout unit
  //m_atrcollist.push_back(std::string("/PIXEL/DCS/PLANTS"));      // 1
  m_atrcollist.push_back(std::string("/PIXEL/DCS/LV"));          // IBL readout unit
  m_currentLumiBlockNumber = 0;

  //std::string testarea = std::getenv("TestArea");
  //ifstream moduleMapfile((testarea + "/InstallArea/share/wincc2cool.csv").c_str());
  //ifstream coolingPipeMapfile((testarea + "/InstallArea/share/coolingPipeMap.csv").c_str());
  //ifstream lvMapfile((testarea + "/InstallArea/share/lvMap.csv").c_str());

  //std::string cmtpath = std::getenv("CMTPATH");
  std::string cmtpath = std::getenv("DATAPATH");
  std::vector<std::string> paths = splitter(cmtpath, ':');
  for (const auto& x : paths){
    if(is_file_exist((x + "/wincc2cool.csv").c_str())){
      ATH_MSG_INFO("initialising DCS channel maps using files in " << x);
      std::ifstream moduleMapfile((x + "/wincc2cool.csv").c_str());
      std::ifstream coolingPipeMapfile((x + "/coolingPipeMap.csv").c_str());
      std::ifstream lvMapfile((x + "/lvMap.csv").c_str());
      if( moduleMapfile.fail() || coolingPipeMapfile.fail() || lvMapfile.fail() ) {
        ATH_MSG_WARNING("initialize(): Map File do not exist. m_doDCS has been changed to False.");
        m_doDCS = false;
      }
      std::string line;
      // make a dictionary to convert module name to channel number
      int channel; std::string moduleName; std::string rest;
      std::string inletName; std::string outletName;
      std::string lvVoltageName; std::string lvCurrentName;
      //while(getline(moduleMapfile, line))
      while(moduleMapfile >> channel >> moduleName >> rest) {
        // get channel number from wincc2cool.csv
        //int channel; std::string moduleName; std::string rest;
        //channel = atoi( ( line.substr(0, line.find(",")) ).c_str() );
        //std::string tmp_line = line.substr(line.find(","), std::string::npos);
        //// get the module name from wincc2cool.csv
        //moduleName = tmp_line.substr(0, tmp_line.find(","));
        m_moduleTemperature->m_maps->insert(std::make_pair(moduleName, channel));
        m_HV->m_maps->insert(std::make_pair(moduleName, channel));
        m_moduleDCSDataHolder->m_moduleMap->insert(std::make_pair(moduleName, channel));
        ATH_MSG_DEBUG( "initialize(): channel " << channel << ", moduleName " << moduleName );
      }
      // for cooling pipe
      while(coolingPipeMapfile >> channel >> inletName >> outletName) {
        m_coolingPipeTemperatureInlet->m_maps->insert(std::make_pair(inletName, channel));
        m_coolingPipeTemperatureOutlet->m_maps->insert(std::make_pair(outletName, channel));
      }
      while(lvMapfile >> channel >> lvVoltageName >> lvCurrentName) {
        m_LV_voltage->m_maps->insert(std::make_pair(lvVoltageName, channel));
        m_LV_current->m_maps->insert(std::make_pair(lvCurrentName, channel));
      }
      break;
    }
  }

    m_name2etaIndex["A_M4_A8_2"] = 9;
    m_name2etaIndex["A_M4_A8_1"] = 8;
    m_name2etaIndex["A_M4_A7_2"] = 7;
    m_name2etaIndex["A_M4_A7_1"] = 6;
    m_name2etaIndex["A_M3_A6"] = 5;
    m_name2etaIndex["A_M3_A5"] = 4;
    m_name2etaIndex["A_M2_A4"] = 3;
    m_name2etaIndex["A_M2_A3"] = 2;
    m_name2etaIndex["A_M1_A2"] = 1;
    m_name2etaIndex["A_M1_A1"] = 0;
    m_name2etaIndex["C_M1_C1"] = -1;
    m_name2etaIndex["C_M1_C2"] = -2;
    m_name2etaIndex["C_M2_C3"] = -3;
    m_name2etaIndex["C_M2_C4"] = -4;
    m_name2etaIndex["C_M3_C5"] = -5;
    m_name2etaIndex["C_M3_C6"] = -6;
    m_name2etaIndex["C_M4_C7_1"] = -7;
    m_name2etaIndex["C_M4_C7_2"] = -8;
    m_name2etaIndex["C_M4_C8_1"] = -9;
    m_name2etaIndex["C_M4_C8_2"] = -10;

    m_name2moduleGroup["C_M4"] = 0;
    m_name2moduleGroup["C_M3"] = 1;
    m_name2moduleGroup["C_M2"] = 2;
    m_name2moduleGroup["C_M1"] = 3;
    m_name2moduleGroup["A_M1"] = 4;
    m_name2moduleGroup["A_M2"] = 5;
    m_name2moduleGroup["A_M3"] = 6;
    m_name2moduleGroup["A_M4"] = 7;

    m_fsmState2enum["READY"] = 0.;
    m_fsmState2enum["ON"] = 1.;
    m_fsmState2enum["STANDBY"] = 2.;
    m_fsmState2enum["LV_ON"] = 3.;
    m_fsmState2enum["TRANSITION"] = 4.;
    m_fsmState2enum["UNDEFINED"] = 5.;
    m_fsmState2enum["LOCKED_OUT"] = 6.;
    m_fsmState2enum["DISABLED"] = 7.;
    m_fsmState2enum["OFF"] = 8.;
    m_fsmState2enum["DEAD"] = 9.;

    m_fsmStatus2enum["OK"] = 0.;
    m_fsmStatus2enum["WARNING"] = 1.;
    m_fsmStatus2enum["ERROR"] = 2.;
    m_fsmStatus2enum["UNINITIALIZED"] = 3.;
    m_fsmStatus2enum["DEAD"] = 4.;

   return StatusCode::SUCCESS;
}


StatusCode PixelMainMon::bookHistograms() 
{ 
  // m_isNewRun = newRun;
  // m_isNewLumiBlock = newLumiBlock;
  // m_newLowStatInterval = newLowStatInterval;
  //m_doOnline = true;

   const EventInfo* thisEventInfo;
   sc=evtStore()->retrieve(thisEventInfo);
   if(sc != StatusCode::SUCCESS) {
      if(msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "No EventInfo object found" << endreq;
   }else{
      m_lumiBlockNum = thisEventInfo->event_ID()->lumi_block();

      if(m_doOnline){
         m_runNum = thisEventInfo->event_ID()->run_number();
         std::stringstream m_runNumStr;
         m_runNumStr << m_runNum;
         m_histTitleExt = " (Run " + m_runNumStr.str() + ")";
      }else{
         m_histTitleExt = "";
      }
      if(newLumiBlock) {
         m_LBstartTime = thisEventInfo->event_ID()->time_stamp();
      }
      if( !isFirstBook ){
         m_firstBookTime = thisEventInfo->event_ID()->time_stamp();
         isFirstBook = true;
      }
   }

   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "bookHistograms()" << endreq; 

   if(m_environment==AthenaMonManager::tier0Raw&&!m_doOnline)
   {
     //m_doRDO        = false;
     //m_doRODError   = false;
     //m_doSpacePoint = false;
     //m_doCluster    = false;
     //m_doStatus     = false;
     //m_doTrack      = false;
   }
   if(m_environment==AthenaMonManager::tier0ESD&&!m_doOnline)
   {
     m_doRDO        = false;
     m_doRODError   = false;
     m_doSpacePoint = false;
     m_doCluster    = false;
     m_doStatus     = false;
     m_doDCS        = false;
     m_doTrack      = false;
   }

   //Book histograms

   //if(newLumiBlock) {}

   if(m_doLumiBlock) {
     
     if(m_doRDO) 
       {                                                    
	 sc=BookHitsLumiBlockMon();
	 if (sc.isFailure()) if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not book lowStat histograms" << endreq; 
	 if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Done booking RDO for lowStat" << endreq;  
       }
     if(m_doRODError) 
       {
         sc=BookRODErrorLumiBlockMon();
         if (sc.isFailure()) if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not book lowStat histograms" << endreq; 
         if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Done booking ROD Error for lowStat" << endreq;  
       }
     if(m_doCluster)
       {
         sc=BookClustersLumiBlockMon();
         if (sc.isFailure()) if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not book lowStat histograms" << endreq; 
         if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Done booking Cluster for lowStat" << endreq;  
       }
     if(m_doStatus)
       {
         sc=BookStatusLumiBlockMon();
         if (sc.isFailure()) if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not book lowStat histograms" << endreq; 
         if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Done booking Status for lowStat" << endreq;  
       }
     // if(m_doTrack){
     //   sc=BookTrackLumiBlockMon();
     //   if (sc.isFailure()) if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not book lowStat histograms" << endreq; 
     //   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Done booking Status for lowStat" << endreq;  
     // }
   }

   //   if(newRun) {
      std::string path = "Pixel/Errors";
      if(m_doOnTrack) path.replace(path.begin(), path.end(), "Pixel/ErrorsOnTrack");
      if(m_doOnPixelTrack) path.replace(path.begin(), path.end(), "Pixel/ErrorsOnPixelTrack");
      MonGroup errorHistos( this, path.c_str(), run, ATTRIB_MANAGED ); //declare a group of histograms
      sc = errorHistos.regHist(m_storegate_errors  = new TH2S("storegate_errors",  ("Storegate Errors" + m_histTitleExt + ";Container Name;Error Type").c_str(), 5,0.5,5.5,4,0.5,4.5));
      if (sc.isFailure()) if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not book histograms" << endreq; 
      
      //Set lables and style for error histogram
      m_storegate_errors->SetOption("colz");
      const char *xlabel[5] = { "RDOs", "SpacePoints", "Clusters", "Tracks", "RODErrors" };
      for (int i=0; i<5; i++) {
	m_storegate_errors->GetXaxis()->SetBinLabel( i+1, xlabel[i] );  // bin 0 is underflow
      }
      const char *ylabel[4] = { "Container not found", "Can't retrieve container", "Container empty", "Data is bad"};
      for (int i=0; i<4; i++) {
	m_storegate_errors->GetYaxis()->SetBinLabel( i+1, ylabel[i] );  // bin 0 is underflow
      }
      
      if(m_doTrack)
      {
         sc=BookTrackMon();
         if (sc.isFailure()) if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not book histograms" << endreq; 
         if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Done booking Track" << endreq;  
      }
      if(m_doRDO)
      {                                                    
         sc=BookHitsMon();
         if (sc.isFailure()) if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not book histograms" << endreq; 
         if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Done booking RDO" << endreq;  
      }
      if(m_doRODError) 
      {
         sc=BookRODErrorMon();
         if (sc.isFailure()) if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not book histograms" << endreq; 
         if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Done booking ROD Error" << endreq;  
      }
      if(m_doSpacePoint)
      {
         sc=BookSpacePointMon();
         if (sc.isFailure()) if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not book histograms" << endreq; 
         if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Done booking SP" << endreq;  
      }
      if(m_doCluster)
      {
         sc=BookClustersMon();
         if (sc.isFailure()) if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not book histograms" << endreq; 
         if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Done booking Cluster" << endreq;  
      }
      if(m_doStatus)
      {
         sc=BookStatusMon();
         if (sc.isFailure()) if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not book histograms" << endreq; 
         if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Done booking Status" << endreq;  
      }
      if(m_doDCS)
      {
         sc=BookPixelDCSMon();
         if (sc.isFailure()) if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not book histograms" << endreq; 
         if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Done booking DCS" << endreq;  
      }

      return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode PixelMainMon::fillHistograms() //get called twice per event 
{
   //Get required services, we want to repeat as little as possible in the function calls
   // Part 1: Get the messaging service, print where you are                

   m_event++;       
   m_majorityDisabled = false;

   const EventInfo* thisEventInfo;
   sc=evtStore()->retrieve(thisEventInfo);
   if(sc != StatusCode::SUCCESS) {
      if(msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "No EventInfo object found" << endreq;
   }else{
      m_currentTime = thisEventInfo->event_ID()->time_stamp(); 
      //msg(MSG::INFO) << "First booking time:" << m_firstBookTime << " Current time: " << m_currentTime << endreq;
      unsigned int currentdiff = (m_currentTime - m_firstBookTime)/100;
      if( currentdiff > m_nRefresh ){
         m_doRefresh = true;
         m_nRefresh = currentdiff;
      //   msg(MSG::INFO) << "m_doRefresh:" << m_doRefresh << " " << m_nRefresh << endreq;
      }else{
         m_doRefresh = false;
      }
   }

   PixelID::const_id_iterator idIt    = m_pixelid->wafer_begin();
   PixelID::const_id_iterator idItEnd = m_pixelid->wafer_end();

   m_nGood_ECA=0;
   m_nGood_ECC=0;
   m_nGood_IBL2D=0;
   m_nGood_IBL3D=0;
   m_nGood_B0=0;
   m_nGood_B1=0;
   m_nGood_B2=0;

   for (; idIt != idItEnd; ++idIt)
   {
      Identifier WaferID = *idIt;
      IdentifierHash id_hash = m_pixelid->wafer_hash(WaferID);

      int Index;
      if(m_pixelCondSummarySvc->isActive(id_hash) == true && m_pixelCondSummarySvc->isGood(id_hash) == true ) {Index=0;}
      else if (m_pixelCondSummarySvc->isActive(id_hash) == false) {Index=2;}
      else {Index=1;}
       
      if(Index==0){
	      int em  = m_pixelid->eta_module(WaferID);
	      if(m_pixelid->barrel_ec(WaferID)==2)  m_nGood_ECA++;
	      if(m_pixelid->barrel_ec(WaferID)==-2) m_nGood_ECC++;
	      if (m_pixelid->barrel_ec(WaferID)==0) {
	         if(m_pixelid->layer_disk(WaferID)==0 && m_doIBL){
	            if(em<6 && em>-7){
	               m_nGood_IBL2D++;
	            }else{
	               m_nGood_IBL3D++;
	            }
	         }
	         if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) m_nGood_B0++;
	         if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) m_nGood_B1++;
	         if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) m_nGood_B2++;
	      }
      }
   }
   ////////////////////////////////////////////////////////////////
   //fill methods go here
   if(m_doStatus) { //fill status histograms first, to check number of disabled modules
      sc=FillStatusMon();   
      if(sc.isFailure()) if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not fill histograms" << endreq; 
   }

   if(m_doTrack&&evtStore()->contains< TrackCollection >(m_TracksName))
   {
      sc=FillTrackMon();
      if (sc.isFailure()) if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not fill histograms" << endreq; 
   }else{
      m_storegate_errors->Fill(4.,1.);
   }

   if(m_doRDO&&evtStore()->contains<PixelRDO_Container>(m_Pixel_RDOName))
   {
      sc=FillHitsMon();
   }else{
      m_storegate_errors->Fill(1.,1.);  //first 1. is for RDO, second is for container not found
   }
   //if(m_doRODError&&evtStore()->contains<PixelRODErrorCollection>(m_detector_error_name))

   if(m_doRODError)
   {
      sc=FillRODErrorMon();
      if (sc.isFailure()) if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not fill histograms" << endreq; 
   }else{
      m_storegate_errors->Fill(5.,1.);
   }
   if(m_doSpacePoint&&evtStore()->contains<SpacePointContainer>(m_Pixel_SpacePointsName))
   {
      sc=FillSpacePointMon();
      if (sc.isFailure()) if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not fill histograms" << endreq; 
   }else{
      m_storegate_errors->Fill(2.,1.);
   }

   if(m_doCluster&&evtStore()->contains<InDet::PixelClusterContainer>(m_Pixel_SiClustersName))
   {
      sc=FillClustersMon();
      if (sc.isFailure()) if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not fill histograms" << endreq; 
   }else{
      m_storegate_errors->Fill(3.,1.);
   }

   if(m_doDCS)
   {
      sc=FillPixelDCSMon();
      if (sc.isFailure()) if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not fill histograms" << endreq;
   }else{
      m_storegate_errors->Fill(3.,1.);
   }
   return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode PixelMainMon::procHistograms()
{ 
   // Part 1: Get the messaging service, print where you are
   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "finalize()" << endreq;
   //if(endOfEventsBlock){}
   if(endOfLumiBlock)
   {
     m_LBendTime = m_currentTime;
     if (m_doTrack) { sc=ProcTrackMon(); }
     if (sc.isFailure()) if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not proc histograms" << endreq; 
   }
   
   if( m_doOnline ){
   //   if(m_doRDO){ sc=ProcHitsMon(); }
   //   if (m_doCluster) { sc=ProcClustersMon(); }
   }

   if(!m_doOnline && endOfRun)
   {
      if (m_doRDO)     { sc=ProcHitsMon(); }
      if (m_doCluster) { sc=ProcClustersMon(); }
      if (m_doStatus)  { sc=ProcStatusMon(); }
      if (m_doDCS) { sc=ProcPixelDCSMon(); }
      if (sc.isFailure()) if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not proc histograms" << endreq; 
   }
  
   return StatusCode::SUCCESS;
}


