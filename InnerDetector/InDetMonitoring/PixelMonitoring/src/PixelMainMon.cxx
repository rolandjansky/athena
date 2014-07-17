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
#include "PixelMonitoring/PixelMon2DMaps.h"
#include "PixelMonitoring/PixelMonProfiles.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH1I_LW.h"

#include <stdint.h>
#include <vector>
#include <sstream>

#include "GaudiKernel/StatusCode.h"       
//#include "Identifier/Identifier.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetIdentifier/PixelID.h"
#include "PixelCabling/IPixelCablingSvc.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"

//////////////////////////////////////////////////////////////////////////////

PixelMainMon::PixelMainMon(const std::string & type, 
      const std::string & name,
      const IInterface* parent) :
   ManagedMonitorToolBase(type, name, parent),
   m_pixelCondSummarySvc("PixelConditionsSummarySvc",name),
   m_ErrorSvc("PixelByteStreamErrorsSvc",name),
   m_pixelCableSvc("PixelCablingSvc",name),
   m_IBLParameterSvc("IBLParameterSvc",name),
   m_holeSearchTool("InDet::InDetTrackHoleSearchTool/InDetHoleSearchTool")
   //m_trkSummaryTool("Trk::TrackSummaryTool/InDetTrackSummaryTool")
{                                                                        //all job options flags go here
   declareProperty("PixelConditionsSummarySvc", m_pixelCondSummarySvc);
   declareProperty("PixelByteStreamErrorsSvc",  m_ErrorSvc);
   declareProperty("PixelCablingSvc",           m_pixelCableSvc);
   declareProperty("HoleSearchTool",  m_holeSearchTool);
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
   declareProperty("doStatus",        m_doStatus     = false);

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

   //initalize all the histograms to 0 to start
   m_hits_per_lumi = 0;
   m_hits_per_lumi_ECA = 0;
   m_hits_per_lumi_ECC = 0;
   m_hits_per_lumi_IBL = 0;
   m_hits_per_lumi_B0 = 0;
   m_hits_per_lumi_B1 = 0;
   m_hits_per_lumi_B2 = 0;         
   m_hit_ToT_ECA = 0;
   m_hit_ToT_ECC = 0;
   m_hit_ToT_IBL2D = 0;
   m_hit_ToT_IBL3D = 0;
   m_hit_ToT_B0 = 0;
   m_hit_ToT_B1 = 0;
   m_hit_ToT_B2 = 0;
   m_ToT_vs_eta_IBL = 0;
   m_ToT_vs_eta_B0 = 0;
   m_ToT_vs_eta_B1 = 0;
   m_ToT_vs_eta_B2 = 0;
   m_nlowToT_vs_clussize_IBL = 0;
   m_nlowToT_vs_clussize_B0 = 0;
   m_nlowToT_vs_clussize_B1 = 0;
   m_nlowToT_vs_clussize_B2 = 0;
   m_nlowToT_vs_clussize_ECC = 0;
   m_nlowToT_vs_clussize_ECA = 0;
   m_Lvl1A = 0;
   m_Lvl1ID = 0;                
   m_Lvl1ID_diff_mod_ATLAS_per_LB = 0;
   m_Lvl1ID_diff_mod_ATLAS = 0;
   m_BCID = 0;                  
   m_Atlas_BCID = 0;            
   m_Atlas_BCID_hits = 0;            
   m_BCID_Profile = 0;          
   m_diff_ROD_BCID = 0;         
   m_diff_ROD_vs_Module_BCID = 0;
   m_occupancy = 0;
   m_average_occupancy = 0;
   m_FE_chip_hit_summary = 0;
   m_ecA_occupancy_summary_low = 0;    
   m_ecC_occupancy_summary_low = 0;    
   m_bar_layI_occupancy_summary_low = 0;
   m_bar_lay0_occupancy_summary_low = 0;
   m_bar_lay1_occupancy_summary_low = 0;
   m_bar_lay2_occupancy_summary_low = 0;
   m_ecA_occupancy_summary = 0;    
   m_ecC_occupancy_summary = 0;
   m_bar_layI_occupancy_summary = 0;
   m_bar_lay0_occupancy_summary = 0;
   m_bar_lay1_occupancy_summary = 0;
   m_bar_lay2_occupancy_summary = 0;
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
   m_track_res_phi = 0; 
   m_track_pull_phi = 0;
   m_track_res_eta = 0; 
   m_track_pull_eta = 0;
   m_track_chi2 = 0;
   m_track_chi2_bcl1 = 0;
   m_track_chi2_bcl0 = 0;
   m_track_chi2_bclgt1 = 0;
   m_track_chi2_bcl1_highpt = 0;
   m_track_chi2_bcl0_highpt = 0;
   m_track_chi2_bclgt1_highpt = 0;
   m_track_clusToT = 0;
   m_track_qOverP = 0;  
   m_track_qOverP_inv = 0;  
   m_track_pt = 0;  
   m_track_d0 = 0;      
   m_track_z0 = 0;     
   m_track_phi0 = 0;    
   m_track_theta = 0;   
   m_track_eta = 0;    
   m_tracks_per_lumi = 0;
   m_trackRate_per_lumi = 0;
   m_tracksPerEvt_per_lumi = 0;
   m_track_dedx = 0;
   m_track_mass_dedx = 0;
   m_clustot_vs_pt = 0;
   m_clustot_lowpt = 0;
   m_1hitclustot_lowpt = 0;
   m_2hitclustot_lowpt = 0;
   m_clustot_highpt = 0;
   m_1hitclustot_highpt = 0;
   m_2hitclustot_highpt = 0;
   m_tsos_hitmap = 0;
   m_tsos_holemap = 0;
   m_tsos_outliermap = 0;
   m_clusters_per_lumi = 0;
   m_clusters_per_lumi_ECA = 0;
   m_clusters_per_lumi_ECC = 0;
   m_clusters_per_lumi_IBL = 0;
   m_clusters_per_lumi_B0 = 0;
   m_clusters_per_lumi_B1 = 0;
   m_clusters_per_lumi_B2 = 0;
   m_largeclusters_per_lumi = 0;
   m_verylargeclusters_per_lumi = 0;
   m_totalclusters_per_lumi = 0;
   m_totalclusters_per_lumi_ECA = 0;
   m_totalclusters_per_lumi_ECC = 0;
   m_totalclusters_per_lumi_IBL = 0;
   m_totalclusters_per_lumi_B0 = 0;
   m_totalclusters_per_lumi_B1 = 0;
   m_totalclusters_per_lumi_B2 = 0;
   m_highNclusters_per_lumi = 0;
   m_cluster_ToT = 0;
   m_cluster_ToT_ECA = 0;
   m_cluster_ToT_IBL3D = 0;
   m_cluster_ToT_IBL2D = 0;
   m_cluster_ToT_B0 = 0;
   m_cluster_ToT_B1 = 0;
   m_cluster_ToT_B2 = 0;
   m_cluster_ToT_ECC = 0;
   m_bigcluster_ToT_ECA = 0;
   m_bigcluster_ToT_IBL = 0;
   m_bigcluster_ToT_B0 = 0;
   m_bigcluster_ToT_B1 = 0;
   m_bigcluster_ToT_B2 = 0;
   m_bigcluster_ToT_ECC = 0;
   m_1cluster_ToT_ECA = 0;
   m_1cluster_ToT_IBL = 0;
   m_1cluster_ToT_B0 = 0;
   m_1cluster_ToT_B1 = 0;
   m_1cluster_ToT_B2 = 0;
   m_1cluster_ToT_ECC = 0;
   m_2cluster_ToT_ECA = 0;
   m_2cluster_ToT_IBL = 0;
   m_2cluster_ToT_B0 = 0;
   m_2cluster_ToT_B1 = 0;
   m_2cluster_ToT_B2 = 0;
   m_2cluster_ToT_ECC = 0;
   m_3cluster_ToT_ECA = 0;
   m_3cluster_ToT_IBL = 0;
   m_3cluster_ToT_B0 = 0;
   m_3cluster_ToT_B1 = 0;
   m_3cluster_ToT_B2 = 0;
   m_3cluster_ToT_ECC = 0;
   m_bigcluster_Q_ECA = 0;
   m_bigcluster_Q_IBL = 0;
   m_bigcluster_Q_B0 = 0;
   m_bigcluster_Q_B1 = 0;
   m_bigcluster_Q_B2 = 0;
   m_bigcluster_Q_ECC = 0;
   m_1cluster_Q_ECA = 0;
   m_1cluster_Q_IBL = 0;
   m_1cluster_Q_B0 = 0;
   m_1cluster_Q_B1 = 0;
   m_1cluster_Q_B2 = 0;
   m_1cluster_Q_ECC = 0;
   m_2cluster_Q_ECA = 0;
   m_2cluster_Q_IBL = 0;
   m_2cluster_Q_B0 = 0;
   m_2cluster_Q_B1 = 0;
   m_2cluster_Q_B2 = 0;
   m_2cluster_Q_ECC = 0;
   m_3cluster_Q_ECA = 0;
   m_3cluster_Q_IBL = 0;
   m_3cluster_Q_B0 = 0;
   m_3cluster_Q_B1 = 0;
   m_3cluster_Q_B2 = 0;
   m_3cluster_Q_ECC = 0;
   m_clussize_map = 0;
   m_cluscharge_map = 0;
   m_clusToT_map = 0;
   m_cluster_groupsize = 0;
   m_cluster_col_width = 0;
   m_cluster_row_width = 0;
   m_cluster_groupsize_IBL = 0;
   m_cluster_col_width_IBL = 0;
   m_cluster_row_width_IBL = 0;
   m_cluster_groupsize_B0 = 0;
   m_cluster_col_width_B0 = 0;
   m_cluster_row_width_B0 = 0;
   m_cluster_groupsize_B1 = 0;
   m_cluster_col_width_B1 = 0;
   m_cluster_row_width_B1 = 0;
   m_cluster_groupsize_B2 = 0;
   m_cluster_col_width_B2 = 0;
   m_cluster_row_width_B2 = 0;
   m_cluster_groupsize_ECC = 0;
   m_cluster_col_width_ECC = 0;
   m_cluster_row_width_ECC = 0;
   m_cluster_groupsize_ECA = 0;
   m_cluster_col_width_ECA = 0;
   m_cluster_row_width_ECA = 0;
   m_cluster_LVL1A = 0;
   m_cluster_LVL1A_highToT = 0;
   m_cluster_LVL1A_ECA = 0;
   m_cluster_LVL1A_ECC = 0;
   m_cluster_LVL1A_IBL = 0;
   m_cluster_LVL1A_B0 = 0;
   m_cluster_LVL1A_B1 = 0;
   m_cluster_LVL1A_B2 = 0;
   m_cluster_charge_ECA = 0;
   m_cluster_charge_ECC = 0;
   m_cluster_charge_IBL = 0;
   m_cluster_charge_B0 = 0;
   m_cluster_charge_B1 = 0;
   m_cluster_charge_B2 = 0;
   m_clusterSize_eta = 0;
   m_clusToT_vs_eta_IBL = 0;
   m_clusToT_vs_eta_B0 = 0;
   m_clusToT_vs_eta_B1 = 0;
   m_clusToT_vs_eta_B2 = 0;
   m_clusQ_vs_eta_IBL = 0;
   m_clusQ_vs_eta_B0 = 0;
   m_clusQ_vs_eta_B1 = 0;
   m_clusQ_vs_eta_B2 = 0;
   m_clusQ_vs_eta_ECC = 0;
   m_clusQ_vs_eta_ECA = 0;
   m_ToT_vs_clussize_IBL = 0;
   m_ToT_vs_clussize_B0 = 0;
   m_ToT_vs_clussize_B1 = 0;
   m_ToT_vs_clussize_B2 = 0;
   m_Q_vs_clussize_IBL = 0;
   m_Q_vs_clussize_B0 = 0;
   m_Q_vs_clussize_B1 = 0;
   m_Q_vs_clussize_B2 = 0;
   m_Q_vs_clussize_ECC = 0;
   m_Q_vs_clussize_ECA = 0;
   m_clussize_vs_eta_IBL = 0;
   m_clussize_vs_eta_B0 = 0;
   m_clussize_vs_eta_B1 = 0;
   m_clussize_vs_eta_B2 = 0;
   m_clussize_vs_eta_ECC = 0;
   m_clussize_vs_eta_ECA = 0;
   m_cluster_occupancy = 0;
   m_average_cluster_occupancy = 0;
   m_cluster_ToT_mod = 0;
   m_cluster_size_mod = 0;
   m_cluster_num_mod = 0;
   m_cluster_occupancy_time1 = 0;
   m_cluster_occupancy_time2 = 0;
   m_cluster_occupancy_time3 = 0;
   m_num_clusters = 0;
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
   m_badModules_per_lumi = 0;
   m_badModules_per_lumi_ECA = 0;
   m_badModules_per_lumi_ECC = 0;
   m_badModules_per_lumi_IBL = 0;
   m_badModules_per_lumi_B0 = 0;
   m_badModules_per_lumi_B1 = 0;
   m_badModules_per_lumi_B2 = 0;
   m_disabledModules_per_lumi = 0;
   m_disabledModules_per_lumi_ECA = 0;
   m_disabledModules_per_lumi_ECC = 0;
   m_disabledModules_per_lumi_IBL = 0;
   m_disabledModules_per_lumi_B0 = 0;
   m_disabledModules_per_lumi_B1 = 0;
   m_disabledModules_per_lumi_B2 = 0;
   m_error1 = 0;            
   m_errorTypes = 0;        
   m_errors_per_lumi = 0;
   m_errors_per_lumi_ECA = 0;
   m_errors_per_lumi_ECC = 0;
   m_errors_per_lumi_IBL = 0;
   m_errors_per_lumi_B0 = 0;
   m_errors_per_lumi_B1 = 0;
   m_errors_per_lumi_B2 = 0;   
   m_SyncErrors_per_lumi = 0; 
   m_SyncErrors_per_lumi_ECA = 0; 
   m_SyncErrors_per_lumi_ECC = 0; 
   m_SyncErrors_per_lumi_IBL = 0;
   m_SyncErrors_per_lumi_B0 = 0; 
   m_SyncErrors_per_lumi_B1 = 0; 
   m_SyncErrors_per_lumi_B2 = 0; 
   m_OpticalErrors_per_lumi = 0; 
   m_OpticalErrors_per_lumi_ECA = 0; 
   m_OpticalErrors_per_lumi_ECC = 0; 
   m_OpticalErrors_per_lumi_IBL = 0;
   m_OpticalErrors_per_lumi_B0 = 0; 
   m_OpticalErrors_per_lumi_B1 = 0; 
   m_OpticalErrors_per_lumi_B2 = 0; 
   m_SEU_Errors_per_lumi = 0; 
   m_SEU_Errors_per_lumi_ECA = 0; 
   m_SEU_Errors_per_lumi_ECC = 0; 
   m_SEU_Errors_per_lumi_IBL = 0;
   m_SEU_Errors_per_lumi_B0 = 0; 
   m_SEU_Errors_per_lumi_B1 = 0; 
   m_SEU_Errors_per_lumi_B2 = 0; 
   m_TruncationErrors_per_lumi = 0; 
   m_TruncationErrors_per_lumi_ECA = 0; 
   m_TruncationErrors_per_lumi_ECC = 0; 
   m_TruncationErrors_per_lumi_IBL = 0;
   m_TruncationErrors_per_lumi_B0 = 0; 
   m_TruncationErrors_per_lumi_B1 = 0; 
   m_TruncationErrors_per_lumi_B2 = 0; 
   m_TimeoutErrors_per_lumi = 0; 
   m_TimeoutErrors_per_lumi_ECA = 0; 
   m_TimeoutErrors_per_lumi_ECC = 0; 
   m_TimeoutErrors_per_lumi_IBL = 0;
   m_TimeoutErrors_per_lumi_B0 = 0; 
   m_TimeoutErrors_per_lumi_B1 = 0; 
   m_TimeoutErrors_per_lumi_B2 = 0; 
   m_error_time1 = 0;       
   m_error_time2 = 0;       
   m_error_time3 = 0;       
   m_errors = 0;
   m_OpticalErrors = 0;
   m_SEU_Errors = 0;
   m_TimeoutErrors = 0;
   m_SyncErrors = 0; 
   m_TruncationErrors = 0;
   m_SyncErrors_mod = 0;
   m_SyncErrors_ROD = 0;
   m_TruncErrors_mod = 0;
   m_TruncErrors_ROD = 0;
   m_sync_mod_BCID1 = 0;   
   m_sync_mod_BCID2 = 0;   
   m_sync_mod_LVL1ID = 0;
   m_sync_rod_BCID = 0;   
   m_sync_rod_LVL1ID = 0;   
   m_trunc_mod_EOC = 0;
   m_trunc_mod_hitOF = 0;
   m_trunc_mod_EoEOF = 0;
   m_trunc_rod_HTlim = 0;
   m_trunc_rod_FIFOOF = 0;
   m_optical_error = 0;
   m_seu_hit_parity = 0;
   m_seu_reg_parity = 0;
   m_seu_hamming = 0;
   m_timeout = 0;
   m_FEwarning = 0;
   m_sync_mod_BCID1_per_lumi = 0;
   m_sync_mod_BCID1_per_lumi_ECA = 0;
   m_sync_mod_BCID1_per_lumi_ECC = 0;
   m_sync_mod_BCID1_per_lumi_IBL = 0;
   m_sync_mod_BCID1_per_lumi_B0 = 0;
   m_sync_mod_BCID1_per_lumi_B1 = 0;
   m_sync_mod_BCID1_per_lumi_B2 = 0;
   m_sync_mod_BCID2_per_lumi = 0;
   m_sync_mod_BCID2_per_lumi_ECA = 0;
   m_sync_mod_BCID2_per_lumi_ECC = 0;
   m_sync_mod_BCID2_per_lumi_IBL = 0;
   m_sync_mod_BCID2_per_lumi_B0 = 0;
   m_sync_mod_BCID2_per_lumi_B1 = 0;
   m_sync_mod_BCID2_per_lumi_B2 = 0;
   m_sync_mod_LVL1ID_per_lumi = 0;
   m_sync_mod_LVL1ID_per_lumi_ECA = 0;
   m_sync_mod_LVL1ID_per_lumi_ECC = 0;
   m_sync_mod_LVL1ID_per_lumi_IBL = 0;
   m_sync_mod_LVL1ID_per_lumi_B0 = 0;
   m_sync_mod_LVL1ID_per_lumi_B1 = 0;
   m_sync_mod_LVL1ID_per_lumi_B2 = 0;
   m_sync_rod_BCID_per_lumi = 0;
   m_sync_rod_BCID_per_lumi_ECA = 0;
   m_sync_rod_BCID_per_lumi_ECC = 0;
   m_sync_rod_BCID_per_lumi_IBL = 0;
   m_sync_rod_BCID_per_lumi_B0 = 0;
   m_sync_rod_BCID_per_lumi_B1 = 0;
   m_sync_rod_BCID_per_lumi_B2 = 0;
   m_sync_rod_LVL1ID_per_lumi = 0;
   m_sync_rod_LVL1ID_per_lumi_ECA = 0;
   m_sync_rod_LVL1ID_per_lumi_ECC = 0;
   m_sync_rod_LVL1ID_per_lumi_IBL = 0;
   m_sync_rod_LVL1ID_per_lumi_B0 = 0;
   m_sync_rod_LVL1ID_per_lumi_B1 = 0;
   m_sync_rod_LVL1ID_per_lumi_B2 = 0;
   m_trunc_mod_EOC_per_lumi = 0;
   m_trunc_mod_EOC_per_lumi_ECA = 0;
   m_trunc_mod_EOC_per_lumi_ECC = 0;
   m_trunc_mod_EOC_per_lumi_IBL = 0;
   m_trunc_mod_EOC_per_lumi_B0 = 0;
   m_trunc_mod_EOC_per_lumi_B1 = 0;
   m_trunc_mod_EOC_per_lumi_B2 = 0;
   m_trunc_mod_hitOF_per_lumi = 0;
   m_trunc_mod_hitOF_per_lumi_ECA = 0;
   m_trunc_mod_hitOF_per_lumi_ECC = 0;
   m_trunc_mod_hitOF_per_lumi_IBL = 0;
   m_trunc_mod_hitOF_per_lumi_B0 = 0;
   m_trunc_mod_hitOF_per_lumi_B1 = 0;
   m_trunc_mod_hitOF_per_lumi_B2 = 0;
   m_trunc_mod_EoEOF_per_lumi = 0;
   m_trunc_mod_EoEOF_per_lumi_ECA = 0;
   m_trunc_mod_EoEOF_per_lumi_ECC = 0;
   m_trunc_mod_EoEOF_per_lumi_IBL = 0;
   m_trunc_mod_EoEOF_per_lumi_B0 = 0;
   m_trunc_mod_EoEOF_per_lumi_B1 = 0;
   m_trunc_mod_EoEOF_per_lumi_B2 = 0;
   m_trunc_rod_HTlim_per_lumi = 0;
   m_trunc_rod_HTlim_per_lumi_ECA = 0;
   m_trunc_rod_HTlim_per_lumi_ECC = 0;
   m_trunc_rod_HTlim_per_lumi_IBL = 0;
   m_trunc_rod_HTlim_per_lumi_B0 = 0;
   m_trunc_rod_HTlim_per_lumi_B1 = 0;
   m_trunc_rod_HTlim_per_lumi_B2 = 0;
   m_trunc_rod_FIFOOF_per_lumi = 0;
   m_trunc_rod_FIFOOF_per_lumi_ECA = 0;
   m_trunc_rod_FIFOOF_per_lumi_ECC = 0;
   m_trunc_rod_FIFOOF_per_lumi_IBL = 0;
   m_trunc_rod_FIFOOF_per_lumi_B0 = 0;
   m_trunc_rod_FIFOOF_per_lumi_B1 = 0;
   m_trunc_rod_FIFOOF_per_lumi_B2 = 0;
   m_optical_error_per_lumi = 0;
   m_optical_error_per_lumi_ECA = 0;
   m_optical_error_per_lumi_ECC = 0;
   m_optical_error_per_lumi_IBL = 0;
   m_optical_error_per_lumi_B0 = 0;
   m_optical_error_per_lumi_B1 = 0;
   m_optical_error_per_lumi_B2 = 0;
   m_seu_hit_parity_per_lumi = 0;
   m_seu_hit_parity_per_lumi_ECA = 0;
   m_seu_hit_parity_per_lumi_ECC = 0;
   m_seu_hit_parity_per_lumi_IBL = 0;
   m_seu_hit_parity_per_lumi_B0 = 0;
   m_seu_hit_parity_per_lumi_B1 = 0;
   m_seu_hit_parity_per_lumi_B2 = 0;
   m_seu_reg_parity_per_lumi = 0;
   m_seu_reg_parity_per_lumi_ECA = 0;
   m_seu_reg_parity_per_lumi_ECC = 0;
   m_seu_reg_parity_per_lumi_IBL = 0;
   m_seu_reg_parity_per_lumi_B0 = 0;
   m_seu_reg_parity_per_lumi_B1 = 0;
   m_seu_reg_parity_per_lumi_B2 = 0;
   m_seu_hamming_per_lumi = 0;
   m_seu_hamming_per_lumi_ECA = 0;
   m_seu_hamming_per_lumi_ECC = 0;
   m_seu_hamming_per_lumi_IBL = 0;
   m_seu_hamming_per_lumi_B0 = 0;
   m_seu_hamming_per_lumi_B1 = 0;
   m_seu_hamming_per_lumi_B2 = 0;
   m_timeout_per_lumi = 0;
   m_timeout_per_lumi_ECA = 0;
   m_timeout_per_lumi_ECC = 0;
   m_timeout_per_lumi_IBL = 0;
   m_timeout_per_lumi_B0 = 0;
   m_timeout_per_lumi_B1 = 0;
   m_timeout_per_lumi_B2 = 0;
   m_FEwarning_per_lumi = 0;
   m_FEwarning_per_lumi_ECA = 0;
   m_FEwarning_per_lumi_ECC = 0;
   m_FEwarning_per_lumi_IBL = 0;
   m_FEwarning_per_lumi_B0 = 0;
   m_FEwarning_per_lumi_B1 = 0;
   m_FEwarning_per_lumi_B2 = 0;
   m_bad_mod_errors = 0;
   m_bad_mod_errors_ECA = 0;
   m_bad_mod_errors_ECC = 0;
   m_bad_mod_errors_IBL = 0;
   m_bad_mod_errors_B0 = 0;
   m_bad_mod_errors_B1 = 0;
   m_bad_mod_errors_B2 = 0;
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
}

PixelMainMon::~PixelMainMon()
{
}


StatusCode PixelMainMon::initialize()
{
   sc = ManagedMonitorToolBase::initialize();
   if(!sc.isSuccess()) return sc;

   m_event = 0;
   m_lumiBlockNum = 0;
   time ( &m_startTime );  //mark time for start of run
   m_LBstartTime = 0;
   m_LBendTime = 0;
   m_currentTime = 0;
   m_runNum = 0;
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

   if ( m_holeSearchTool.retrieve().isFailure() ) {
     msg(MSG::FATAL) << "Failed to retrieve tool " << m_holeSearchTool << endreq;
     return StatusCode::FAILURE;
   } else {
     msg(MSG::INFO) << "Retrieved tool " << m_holeSearchTool << endreq;
   }

   //if ( m_trkSummaryTool.retrieve().isFailure() ) {
   //  if ( msgLvl(MSG::ERROR) ) msg(MSG::ERROR) << "Failed to retrieve tool " << m_trkSummaryTool << endreq;
   //} else {
   //  if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Retrieved tool " << m_trkSummaryTool << endreq;
   //}
   
   return StatusCode::SUCCESS;
}


StatusCode PixelMainMon::bookHistograms() 
{ 
  // m_isNewRun = newRun;
  // m_isNewLumiBlock = newLumiBlock;
  // m_newLowStatInterval = newLowStatInterval;

   const EventInfo* thisEventInfo;
   sc=evtStore()->retrieve(thisEventInfo);
   if (sc != StatusCode::SUCCESS) {
     if(msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "No EventInfo object found" << endreq;
   }
   else {
     m_lumiBlockNum = thisEventInfo->event_ID()->lumi_block();
     if(m_doOnline){
       m_runNum = thisEventInfo->event_ID()->run_number();
       std::stringstream m_runNumStr;
       m_runNumStr << m_runNum;
       m_histTitleExt = " (Run " + m_runNumStr.str() + ")";
     }
     else m_histTitleExt = "";
     if(newLumiBlock) {
       m_LBstartTime = thisEventInfo->event_ID()->time_stamp();
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
      //   }

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
   } 
   else {
     m_currentTime = thisEventInfo->event_ID()->time_stamp(); 

   }

   ////////////////////////////////////////////////////////////////
   //fill methods go here

   if(m_doStatus) { //fill status histograms first, to check number of disabled modules
     sc=FillStatusMon();   
     if (sc.isFailure()) if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not fill histograms" << endreq; 
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

   if(endOfRun)
   {
      if (m_doRDO)     { sc=ProcHitsMon(); }
      if (m_doCluster) { sc=ProcClustersMon(); }
      if (m_doStatus)  { sc=ProcStatusMon(); }
      if (sc.isFailure()) if(msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not proc histograms" << endreq; 
   }
   return StatusCode::SUCCESS;
}



