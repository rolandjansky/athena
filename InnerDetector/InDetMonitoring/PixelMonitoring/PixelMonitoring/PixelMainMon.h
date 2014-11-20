/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXMAINMON_H_
#define PIXMAINMON_H_

#include <string.h>
#include <time.h>
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "GaudiKernel/ServiceHandle.h" 
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetPrepRawData/PixelClusterContainer.h"         
#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "TrkTrack/TrackCollection.h"            
//#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "LumiBlockComps/ILuminosityTool.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "PixelGeoModel/IBLParameterSvc.h"

class PixelMonModules1D;
class PixelMonModulesProf;
class PixelMonModules2D;
class PixelMon2DMaps;
class PixelMon2DMapsLW;
class PixelMonProfiles;
class PixelMon2DLumiProfiles;
class PixelMon2DLumiMaps;
class StatusCode;
class PixelID;
class TH1I;
class TH1F;
class TH2I;
class TH2F;
class TH1I_LW;
class TH1F_LW;
class TH2I_LW;
class TH2F_LW;
class TH2S;
class TProfile;
class TProfile2D;
class TProfile_LW;
class TProfile2D_LW;
namespace InDetDD{ class PixelDetectorManager;}
namespace InDet{ class PixelCluster;}
namespace Trk{ 
   class SpacePoint;
   class ITrackHoleSearchTool;
   //class ITrackSummaryTool;
}
class IPixelCablingSvc;
//class ILuminosityTool;
class SpacePointContainer;
class IPixelByteStreamErrorsSvc;
class IInDetConditionsSvc;
//class StoreGateSvc;
class PixelRDORawData;

typedef InDet::PixelCluster PixelCluster;                     
typedef InDetRawDataCollection<PixelRDORawData> RDO_Collection;
typedef Trk::SpacePoint SpacePoint;


class PixelMainMon:public ManagedMonitorToolBase 
{
   public:

      PixelMainMon (const std::string & type,
            const std::string & name, 
            const IInterface* parent);
      virtual ~PixelMainMon();
      virtual StatusCode initialize(); //!< Runs once at the start of the code.  Sets up services etc.  
      virtual StatusCode bookHistograms();  //!< Runs at the start of every event.  Books some histograms if one of the function parameters is true
      virtual StatusCode fillHistograms(); //!< Runs at every event, bulk of the code is inside here.  Fills the histograms.
      virtual StatusCode procHistograms();

      /** Convert the module ID in the job options.
       * Input is a string in the form "L0_B11_S2_M5A"
       * Output uses an int with each bit corisponding to part of the offline identifier.
       */
      void FillTimeHisto(double, TProfile*, TProfile*, TProfile*, double, double, double);
      void FillSummaryHistos(PixelMon2DMaps* occupancy, TH1F_LW* A, TH1F_LW* C, TH1F_LW* IBL, TH1F_LW* B0, TH1F_LW* B1, TH1F_LW* B2);
      int ParseDetailsString(std::string & detailsMod);
      bool OnTrack(Identifier id, bool isCluster);
       
      StatusCode BookClustersMon(void);
      StatusCode BookClustersLumiBlockMon(void);
      StatusCode FillClustersMon(void);
      StatusCode ProcClustersMon(void);
      
      StatusCode BookHitsMon(void);
      StatusCode BookHitsLumiBlockMon(void);
      StatusCode FillHitsMon(void);
      StatusCode ProcHitsMon(void);
      
      StatusCode BookStatusMon(void);
      StatusCode BookStatusLumiBlockMon(void);
      StatusCode FillStatusMon(void);
      StatusCode ProcStatusMon(void);
      
      StatusCode BookTrackMon(void);
      StatusCode BookTrackLumiBlockMon(void);
      StatusCode FillTrackMon(void);
      StatusCode ProcTrackMon(void);

      StatusCode BookRODErrorMon(void);
      StatusCode BookRODErrorLumiBlockMon(void);
      StatusCode FillRODErrorMon(void);

      StatusCode BookSpacePointMon(void);
      StatusCode FillSpacePointMon(void);

   private:

      ServiceHandle <IInDetConditionsSvc> m_pixelCondSummarySvc;
      ServiceHandle <IPixelByteStreamErrorsSvc> m_ErrorSvc;
      ServiceHandle <IPixelCablingSvc> m_pixelCableSvc;
      ServiceHandle <IBLParameterSvc> m_IBLParameterSvc;
      ToolHandle<Trk::ITrackHoleSearchTool>     m_holeSearchTool;
      ToolHandle<ILuminosityTool> m_lumiTool;

      //IPixelByteStreamErrorsSvc* m_ErrorSvc;
      //ToolHandle<Trk::ITrackSummaryTool> m_trkSummaryTool;

      //ActiveStoreSvc* m_activeStore;
      //StoreGateSvc* m_StoreGate;
      //StoreGateSvc* m_detStore;
      const PixelID* m_pixelid; // Pixel ID helper
      const InDetDD::PixelDetectorManager* pixelmgr;                               
      StatusCode sc;    
      int m_event;      
      time_t m_startTime;
      bool m_majorityDisabled; //check for each event, true if >50% modules disabled 

      int m_lumiBlockNum;
      int m_currentTime;
      int m_LBstartTime;
      int m_LBendTime;
      int m_runNum;
      const AtlasDetectorID* m_idHelper;      

      std::vector<Identifier> m_RDOIDs;                  ///////Make class scope 
      std::vector<Identifier> m_ClusterIDs;              ///////Make class scope 

      const DataHandle<PixelRDO_Container> m_rdocontainer; //container for pixel RDO
      //      PixelRODErrorCollection* m_error_summary;              //container for pixel ROD errors
      const InDet::PixelClusterContainer* m_Pixel_clcontainer; //container for cluster
      const SpacePointContainer* m_Pixel_spcontainer; //container for spacepoints
      const TrackCollection *m_tracks;  //container for tracks

      std::string m_Pixel_RDOName;          //storegate container names
      std::string m_detector_error_name;
      std::string m_Pixel_SpacePointsName;
      std::string m_Pixel_SiClustersName;
      std::string m_TracksName;
      std::string m_error_summary;  //commissioning
      
      std::string m_histTitleExt;

      bool m_doRDO;              //storegate type flags from jobOptions
      bool m_doRODError;
      bool m_doCluster;    
      bool m_doSpacePoint; 
      bool m_doTrack; 
      bool m_doStatus; 

      bool m_doESD;
      bool m_do2DMaps;
      bool m_doModules;
      bool m_doFEChipSummary;
      bool m_doOffline;
      bool m_doOnline;
      bool m_doLowOccupancy;
      bool m_doHighOccupancy;
      bool m_doOnTrack;
      bool m_doOnPixelTrack;
      bool m_doPixelOccupancy; 
      bool m_doRodSim;
      bool m_doDetails;
      bool m_doSpectrum;
      bool m_doNoiseMap;
      bool m_doTiming;
      bool m_doLumiBlock;   //store module status, error etc for each lumiblock

      bool m_doHeavyIonMon;  //modifications for heavy ion monitoring

      std::string m_DetailsMod1;          //other inputs from job options
      std::string m_DetailsMod2;   
      std::string m_DetailsMod3;   
      std::string m_DetailsMod4;   

      bool m_doIBL;

      bool m_isNewRun;
      bool m_isNewLumiBlock;
      bool m_newLowStatInterval;

      double m_occupancy_cut;

      //////////////////////Histograms///////////////////////////
      //Reconstruction Error Monitoring (mostly storegate error monitoring)
      TH2S* m_storegate_errors;

      //Hitmap  histograms  
      TH1F_LW*              m_mu_vs_lumi;
      PixelMonModulesProf*  m_hiteff_mod;
      TProfile_LW*          m_hits_per_lumi;
      TProfile_LW*          m_hits_per_lumi_ECA;
      TProfile_LW*          m_hits_per_lumi_ECC;
      TProfile_LW*          m_hits_per_lumi_IBL;
      TProfile_LW*          m_hits_per_lumi_B0;
      TProfile_LW*          m_hits_per_lumi_B1;
      TProfile_LW*          m_hits_per_lumi_B2;         
      TH1F_LW*              m_hit_ToT_ECA;
      TH1F_LW*              m_hit_ToT_ECC;
      TH1F_LW*              m_hit_ToT_IBL2D;
      TH1F_LW*              m_hit_ToT_IBL3D;
      TH1F_LW*              m_hit_ToT_B0;
      TH1F_LW*              m_hit_ToT_B1;
      TH1F_LW*              m_hit_ToT_B2;
      TH2F_LW*              m_ToT_vs_eta_IBL;
      TH2F_LW*              m_ToT_vs_eta_B0;
      TH2F_LW*              m_ToT_vs_eta_B1;
      TH2F_LW*              m_ToT_vs_eta_B2;
      TH2F_LW*              m_nlowToT_vs_clussize_IBL;
      TH2F_LW*              m_nlowToT_vs_clussize_B0;
      TH2F_LW*              m_nlowToT_vs_clussize_B1;
      TH2F_LW*              m_nlowToT_vs_clussize_B2;
      TH2F_LW*              m_nlowToT_vs_clussize_ECC;
      TH2F_LW*              m_nlowToT_vs_clussize_ECA;
      TH1I_LW*              m_Lvl1A;
      TH1I_LW*              m_Lvl1A_ECA; 
      TH1I_LW*              m_Lvl1A_ECC; 
      TH1I_LW*              m_Lvl1A_B0; 
      TH1I_LW*              m_Lvl1A_B1; 
      TH1I_LW*              m_Lvl1A_B2; 
      TH1I_LW*              m_Lvl1A_IBL; 
      TH1I_LW*              m_Lvl1ID;                
      PixelMon2DLumiProfiles*    m_Lvl1ID_diff_mod_ATLAS_per_LB;
      TH1I_LW*              m_Lvl1ID_diff_mod_ATLAS;
      TH1I_LW*              m_BCID;                  
      TH1F_LW*              m_Atlas_BCID;            
      TH2F_LW*              m_Atlas_BCID_hits;            
      TProfile_LW*          m_BCID_Profile;          
      TH2I_LW*              m_diff_ROD_BCID;         
      TH1I_LW*              m_diff_ROD_vs_Module_BCID;
      PixelMon2DMaps*       m_occupancy;
      PixelMon2DMaps*       m_average_occupancy;
      PixelMonModules1D*    m_FE_chip_hit_summary;
      TH1F_LW*              m_ecA_occupancy_summary_low;    
      TH1F_LW*              m_ecC_occupancy_summary_low;    
      TH1F_LW*              m_bar_layI_occupancy_summary_low;
      TH1F_LW*              m_bar_lay0_occupancy_summary_low;
      TH1F_LW*              m_bar_lay1_occupancy_summary_low;
      TH1F_LW*              m_bar_lay2_occupancy_summary_low;
      TH1F_LW*              m_ecA_occupancy_summary;    
      TH1F_LW*              m_ecC_occupancy_summary;
      TH1F_LW*              m_bar_layI_occupancy_summary;
      TH1F_LW*              m_bar_lay0_occupancy_summary;
      TH1F_LW*              m_bar_lay1_occupancy_summary;
      TH1F_LW*              m_bar_lay2_occupancy_summary;
      TProfile*             m_occupancy_time1;
      TProfile*             m_occupancy_time2;
      TProfile*             m_occupancy_time3;
      TH1I_LW*              m_num_hits;
      TH1I_LW*              m_num_hits_low;
      TH1F_LW*              m_RodSim_BCID_minus_ToT;
      TH1F_LW*              m_RodSim_FrontEnd_minus_Lvl1ID;
      TH1F_LW*              m_Details_mod1_num_hits;
      TH1F_LW*              m_Details_mod2_num_hits;
      TH1F_LW*              m_Details_mod3_num_hits;
      TH1F_LW*              m_Details_mod4_num_hits;
      TH2F_LW*              m_Details_mod1_occupancy;
      TH2F_LW*              m_Details_mod2_occupancy;
      TH2F_LW*              m_Details_mod3_occupancy;
      TH2F_LW*              m_Details_mod4_occupancy;
      TH1F_LW*              m_Details_mod1_ToT;     
      TH1F_LW*              m_Details_mod2_ToT;     
      TH1F_LW*              m_Details_mod3_ToT;     
      TH1F_LW*              m_Details_mod4_ToT;  
      PixelMonModules2D*    m_pixel_occupancy;
      PixelMonModules2D*    m_pixel_noise_map;
      PixelMonModules1D*    m_pixel_occupancy_spectrum;
//      TH1F_LW*              m_trigger;
//      TH1F_LW*              m_trigger_ECA;
//      TH1F_LW*              m_trigger_ECC;
//      TH1F_LW*              m_trigger_B0;
//      TH1F_LW*              m_trigger_B1;
//      TH1F_LW*              m_trigger_B2;
      PixelMonModules1D*    m_hit_num_mod;
      int                   m_HitPerEventArray_disksA[48][3];
      int                   m_HitPerEventArray_disksC[48][3];
      int                   m_HitPerEventArray_lI[14][20];
      int                   m_HitPerEventArray_l0[22][13];
      int                   m_HitPerEventArray_l1[38][13];
      int                   m_HitPerEventArray_l2[52][13]; 

      //track histograms
      TH1F_LW*              m_track_res_phi; 
      TH1F_LW*              m_track_pull_phi;
      TH1F_LW*              m_track_res_eta; 
      TH1F_LW*              m_track_pull_eta;
      TH1F_LW*              m_track_chi2;
      TH1F_LW*              m_track_chi2_LB;
      TH1F_LW*              m_track_chi2_bcl1;
      TH1F_LW*              m_track_chi2_bcl0;
      TH1F_LW*              m_track_chi2_bclgt1;
      TH1F_LW*              m_track_chi2_bcl1_highpt;
      TH1F_LW*              m_track_chi2_bcl0_highpt;
      TH1F_LW*              m_track_chi2_bclgt1_highpt;
      TH1F_LW*              m_track_clusToT;
      TH1F_LW*              m_track_qOverP;  
      TH1F_LW*              m_track_qOverP_inv;  
      TH1F_LW*              m_track_pt;  
      TH1F_LW*              m_track_d0;      
      TH1F_LW*              m_track_z0;     
      TH1F_LW*              m_track_phi0;    
      TH1F_LW*              m_track_theta;   
      TH1F_LW*              m_track_eta;    
      TH1I_LW*              m_tracks_per_lumi;
      TH1F_LW*              m_trackRate_per_lumi;
      TProfile_LW*          m_tracksPerEvt_per_lumi;
      TH2F_LW*              m_track_dedx;
      TH1F_LW*              m_track_mass_dedx;
      TH2F_LW*              m_clustot_vs_pt;
      TH1F_LW*              m_clustot_lowpt;
      TH1F_LW*              m_1hitclustot_lowpt;
      TH1F_LW*              m_2hitclustot_lowpt;
      TH1F_LW*              m_clustot_highpt;
      TH1F_LW*              m_1hitclustot_highpt;
      TH1F_LW*              m_2hitclustot_highpt;

      PixelMonModulesProf*  m_tsos_hiteff_vs_lumi;
      PixelMon2DMaps*       m_tsos_hitmap;
      PixelMon2DMaps*       m_tsos_holemap;
      PixelMon2DMaps*   m_tsos_outliermap;

      //cluster histograms
      TProfile_LW*          m_clusters_per_lumi;
      TProfile_LW*          m_clusters_per_lumi_PIX;
      TProfile_LW*          m_clusters_per_lumi_ECA;
      TProfile_LW*          m_clusters_per_lumi_ECC;
      TProfile_LW*          m_clusters_per_lumi_IBL;
      TProfile_LW*          m_clusters_per_lumi_B0;
      TProfile_LW*          m_clusters_per_lumi_B1;
      TProfile_LW*          m_clusters_per_lumi_B2;
      TProfile_LW*          m_largeclusters_per_lumi;
      TProfile_LW*          m_verylargeclusters_per_lumi;
      TH1I_LW*              m_totalclusters_per_lumi;
      TH1I_LW*              m_totalclusters_per_lumi_ECA;
      TH1I_LW*              m_totalclusters_per_lumi_ECC;
      TH1I_LW*              m_totalclusters_per_lumi_IBL;
      TH1I_LW*              m_totalclusters_per_lumi_B0;
      TH1I_LW*              m_totalclusters_per_lumi_B1;
      TH1I_LW*              m_totalclusters_per_lumi_B2;
      TH1I_LW*              m_highNclusters_per_lumi;
      TH1F_LW*              m_cluster_ToT;
      TH1F_LW*              m_cluster_ToT_PIX;
      TH1F_LW*              m_cluster_ToT_ECA;
      TH1F_LW*              m_cluster_ToT_IBL;
      TH1F_LW*              m_cluster_ToT_IBL2D;
      TH1F_LW*              m_cluster_ToT_IBL3D;
      TH1F_LW*              m_cluster_ToT_B0;
      TH1F_LW*              m_cluster_ToT_B1;
      TH1F_LW*              m_cluster_ToT_B2;
      TH1F_LW*              m_cluster_ToT_ECC;
      TH1F_LW*              m_bigcluster_ToT_ECA;
      TH1F_LW*              m_bigcluster_ToT_IBL;
      TH1F_LW*              m_bigcluster_ToT_B0;
      TH1F_LW*              m_bigcluster_ToT_B1;
      TH1F_LW*              m_bigcluster_ToT_B2;
      TH1F_LW*              m_bigcluster_ToT_ECC;
      TH1F_LW*              m_1cluster_ToT_ECA;
      TH1F_LW*              m_1cluster_ToT_IBL;
      TH1F_LW*              m_1cluster_ToT_B0;
      TH1F_LW*              m_1cluster_ToT_B1;
      TH1F_LW*              m_1cluster_ToT_B2;
      TH1F_LW*              m_1cluster_ToT_ECC;
      TH1F_LW*              m_2cluster_ToT_ECA;
      TH1F_LW*              m_2cluster_ToT_IBL;
      TH1F_LW*              m_2cluster_ToT_B0;
      TH1F_LW*              m_2cluster_ToT_B1;
      TH1F_LW*              m_2cluster_ToT_B2;
      TH1F_LW*              m_2cluster_ToT_ECC;
      TH1F_LW*              m_3cluster_ToT_ECA;
      TH1F_LW*              m_3cluster_ToT_IBL;
      TH1F_LW*              m_3cluster_ToT_B0;
      TH1F_LW*              m_3cluster_ToT_B1;
      TH1F_LW*              m_3cluster_ToT_B2;
      TH1F_LW*              m_3cluster_ToT_ECC;

      ///////////////                                    
      TH1F_LW*              m_bigcluster_Q_ECA;
      TH1F_LW*              m_bigcluster_Q_IBL;
      TH1F_LW*              m_bigcluster_Q_B0;
      TH1F_LW*              m_bigcluster_Q_B1;
      TH1F_LW*              m_bigcluster_Q_B2;
      TH1F_LW*              m_bigcluster_Q_ECC;
      TH1F_LW*              m_1cluster_Q_ECA;
      TH1F_LW*              m_1cluster_Q_IBL;
      TH1F_LW*              m_1cluster_Q_B0;
      TH1F_LW*              m_1cluster_Q_B1;
      TH1F_LW*              m_1cluster_Q_B2;
      TH1F_LW*              m_1cluster_Q_ECC;
      TH1F_LW*              m_2cluster_Q_ECA;
      TH1F_LW*              m_2cluster_Q_IBL;
      TH1F_LW*              m_2cluster_Q_B0;
      TH1F_LW*              m_2cluster_Q_B1;
      TH1F_LW*              m_2cluster_Q_B2;
      TH1F_LW*              m_2cluster_Q_ECC;
      TH1F_LW*              m_3cluster_Q_ECA;
      TH1F_LW*              m_3cluster_Q_IBL;
      TH1F_LW*              m_3cluster_Q_B0;
      TH1F_LW*              m_3cluster_Q_B1;
      TH1F_LW*              m_3cluster_Q_B2;
      TH1F_LW*              m_3cluster_Q_ECC;

      PixelMonProfiles*       m_clussize_map;
      PixelMonProfiles*       m_cluscharge_map;
      PixelMonProfiles*       m_clusToT_map;
      TH1F_LW*              m_cluster_groupsize;
      TH1F_LW*              m_cluster_col_width;
      TH1F_LW*              m_cluster_row_width;
      TH1F_LW*              m_cluster_groupsize_IBL;
      TH1F_LW*              m_cluster_col_width_IBL;
      TH1F_LW*              m_cluster_row_width_IBL;
      TH1F_LW*              m_cluster_groupsize_B0;
      TH1F_LW*              m_cluster_col_width_B0;
      TH1F_LW*              m_cluster_row_width_B0;
      TH1F_LW*              m_cluster_groupsize_B1;
      TH1F_LW*              m_cluster_col_width_B1;
      TH1F_LW*              m_cluster_row_width_B1;
      TH1F_LW*              m_cluster_groupsize_B2;
      TH1F_LW*              m_cluster_col_width_B2;
      TH1F_LW*              m_cluster_row_width_B2;
      TH1F_LW*              m_cluster_groupsize_ECC;
      TH1F_LW*              m_cluster_col_width_ECC;
      TH1F_LW*              m_cluster_row_width_ECC;
      TH1F_LW*              m_cluster_groupsize_ECA;
      TH1F_LW*              m_cluster_col_width_ECA;
      TH1F_LW*              m_cluster_row_width_ECA;
      TH1F_LW*              m_cluster_LVL1A;
      TH1F_LW*              m_cluster_LVL1A_highToT;
      TH1F_LW*              m_cluster_LVL1A_PIX;
      TH1F_LW*              m_cluster_LVL1A_ECA;
      TH1F_LW*              m_cluster_LVL1A_ECC;
      TH1F_LW*              m_cluster_LVL1A_IBL;
      TH1F_LW*              m_cluster_LVL1A_B0;
      TH1F_LW*              m_cluster_LVL1A_B1;
      TH1F_LW*              m_cluster_LVL1A_B2;
      TH1F_LW*              m_cluster_charge_ECA;
      TH1F_LW*              m_cluster_charge_ECC;
      TH1F_LW*              m_cluster_charge_IBL;
      TH1F_LW*              m_cluster_charge_B0;
      TH1F_LW*              m_cluster_charge_B1;
      TH1F_LW*              m_cluster_charge_B2;
      TProfile_LW*          m_clusterSize_eta;
      TH2F_LW*              m_clusToT_vs_eta_IBL;
      TH2F_LW*              m_clusToT_vs_eta_B0;
      TH2F_LW*              m_clusToT_vs_eta_B1;
      TH2F_LW*              m_clusToT_vs_eta_B2;
      TH2F_LW*              m_clusQ_vs_eta_IBL;
      TH2F_LW*              m_clusQ_vs_eta_B0;
      TH2F_LW*              m_clusQ_vs_eta_B1;
      TH2F_LW*              m_clusQ_vs_eta_B2;
      TH2F_LW*              m_clusQ_vs_eta_ECC;
      TH2F_LW*              m_clusQ_vs_eta_ECA;
      TH2F_LW*              m_ToT_vs_clussize_IBL;
      TH2F_LW*              m_ToT_vs_clussize_B0;
      TH2F_LW*              m_ToT_vs_clussize_B1;
      TH2F_LW*              m_ToT_vs_clussize_B2;
      TH2F_LW*              m_Q_vs_clussize_IBL;
      TH2F_LW*              m_Q_vs_clussize_B0;
      TH2F_LW*              m_Q_vs_clussize_B1;
      TH2F_LW*              m_Q_vs_clussize_B2;
      TH2F_LW*              m_Q_vs_clussize_ECC;
      TH2F_LW*              m_Q_vs_clussize_ECA;

      TH2F_LW*              m_clussize_vs_eta_IBL;
      TH2F_LW*              m_clussize_vs_eta_B0;
      TH2F_LW*              m_clussize_vs_eta_B1;
      TH2F_LW*              m_clussize_vs_eta_B2;
      TH2F_LW*              m_clussize_vs_eta_ECC;
      TH2F_LW*              m_clussize_vs_eta_ECA;
      PixelMon2DMaps*       m_cluster_occupancy;
      PixelMon2DMaps*       m_clusocc_sizenot1; 
      PixelMon2DMaps*       m_average_cluster_occupancy;
      PixelMonModulesProf*  m_cluseff_mod;
      PixelMonModules1D*    m_cluster_ToT_mod;
      PixelMonModules1D*    m_cluster_size_mod;
      PixelMonModules1D*    m_cluster_num_mod;
      TProfile*             m_cluster_occupancy_time1;
      TProfile*             m_cluster_occupancy_time2;
      TProfile*             m_cluster_occupancy_time3;
      TH1I_LW*              m_num_clusters;
      TH1I_LW*              m_num_clusters_PIX;
      TH1I_LW*              m_num_clusters_low;
      TH1I_LW*              m_num_clusters_ECA;
      TH1I_LW*              m_num_clusters_ECC;
      TH1I_LW*              m_num_clusters_IBL;
      TH1I_LW*              m_num_clusters_B0;
      TH1I_LW*              m_num_clusters_B1;
      TH1I_LW*              m_num_clusters_B2;
      TH1F_LW*              m_ecA_cluster_occupancy_summary;
      TH1F_LW*              m_ecC_cluster_occupancy_summary;
      TH1F_LW*              m_bar_layI_cluster_occupancy_summary;
      TH1F_LW*              m_bar_lay0_cluster_occupancy_summary;
      TH1F_LW*              m_bar_lay1_cluster_occupancy_summary;
      TH1F_LW*              m_bar_lay2_cluster_occupancy_summary;
      TH1F_LW*              m_ecA_cluster_occupancy_summary_low;
      TH1F_LW*              m_ecC_cluster_occupancy_summary_low;
      TH1F_LW*              m_bar_layI_cluster_occupancy_summary_low;
      TH1F_LW*              m_bar_lay0_cluster_occupancy_summary_low;
      TH1F_LW*              m_bar_lay1_cluster_occupancy_summary_low;
      TH1F_LW*              m_bar_lay2_cluster_occupancy_summary_low;
      int                   m_ClusPerEventArray_disksA[48][3];
      int                   m_ClusPerEventArray_disksC[48][3];
      int                   m_ClusPerEventArray_lI[14][20];
      int                   m_ClusPerEventArray_l0[22][13];
      int                   m_ClusPerEventArray_l1[38][13];
      int                   m_ClusPerEventArray_l2[52][13];
      PixelMonProfiles*     m_cluster_LVL1A_mod;
      PixelMonProfiles*     m_clus_LVL1A_sizenot1;
      TProfile_LW*          m_clustersOnOffTrack_per_lumi;

      //Status histograms
      PixelMonModules1D*    m_Status_modules;
      PixelMonProfiles*     m_status;
      PixelMon2DMaps*       m_dqStatus;
      TProfile_LW*          m_badModules_per_lumi;
      TProfile_LW*          m_badModules_per_lumi_ECA;
      TProfile_LW*          m_badModules_per_lumi_ECC;
      TProfile_LW*          m_badModules_per_lumi_IBL;
      TProfile_LW*          m_badModules_per_lumi_B0;
      TProfile_LW*          m_badModules_per_lumi_B1;
      TProfile_LW*          m_badModules_per_lumi_B2;
      TProfile_LW*          m_disabledModules_per_lumi;
      TProfile_LW*          m_disabledModules_per_lumi_PIX;
      TProfile_LW*          m_disabledModules_per_lumi_ECA;
      TProfile_LW*          m_disabledModules_per_lumi_ECC;
      TProfile_LW*          m_disabledModules_per_lumi_IBL;
      TProfile_LW*          m_disabledModules_per_lumi_B0;
      TProfile_LW*          m_disabledModules_per_lumi_B1;
      TProfile_LW*          m_disabledModules_per_lumi_B2;

      //ROD Error histograms
      TH1F_LW*              m_error1;            
      TH1F_LW*              m_errorTypes;        
      TProfile_LW*          m_errors_per_lumi;
      TProfile_LW*          m_errors_per_lumi_ECA;
      TProfile_LW*          m_errors_per_lumi_ECC;
      TProfile_LW*          m_errors_per_lumi_IBL;
      TProfile_LW*          m_errors_per_lumi_B0;
      TProfile_LW*          m_errors_per_lumi_B1;
      TProfile_LW*          m_errors_per_lumi_B2;   
      TProfile_LW*          m_SyncErrors_per_lumi; 
      TProfile_LW*          m_SyncErrors_per_lumi_ECA; 
      TProfile_LW*          m_SyncErrors_per_lumi_ECC; 
      TProfile_LW*          m_SyncErrors_per_lumi_IBL;
      TProfile_LW*          m_SyncErrors_per_lumi_B0; 
      TProfile_LW*          m_SyncErrors_per_lumi_B1; 
      TProfile_LW*          m_SyncErrors_per_lumi_B2; 
      TProfile_LW*          m_OpticalErrors_per_lumi; 
      TProfile_LW*          m_OpticalErrors_per_lumi_ECA; 
      TProfile_LW*          m_OpticalErrors_per_lumi_ECC; 
      TProfile_LW*          m_OpticalErrors_per_lumi_IBL;
      TProfile_LW*          m_OpticalErrors_per_lumi_B0; 
      TProfile_LW*          m_OpticalErrors_per_lumi_B1; 
      TProfile_LW*          m_OpticalErrors_per_lumi_B2; 
      TProfile_LW*          m_SEU_Errors_per_lumi; 
      TProfile_LW*          m_SEU_Errors_per_lumi_ECA; 
      TProfile_LW*          m_SEU_Errors_per_lumi_ECC; 
      TProfile_LW*          m_SEU_Errors_per_lumi_IBL;
      TProfile_LW*          m_SEU_Errors_per_lumi_B0; 
      TProfile_LW*          m_SEU_Errors_per_lumi_B1; 
      TProfile_LW*          m_SEU_Errors_per_lumi_B2; 
      TProfile_LW*          m_TruncationErrors_per_lumi; 
      TProfile_LW*          m_TruncationErrors_per_lumi_ECA; 
      TProfile_LW*          m_TruncationErrors_per_lumi_ECC; 
      TProfile_LW*          m_TruncationErrors_per_lumi_IBL;
      TProfile_LW*          m_TruncationErrors_per_lumi_B0; 
      TProfile_LW*          m_TruncationErrors_per_lumi_B1; 
      TProfile_LW*          m_TruncationErrors_per_lumi_B2; 
      TProfile_LW*          m_TimeoutErrors_per_lumi; 
      TProfile_LW*          m_TimeoutErrors_per_lumi_ECA; 
      TProfile_LW*          m_TimeoutErrors_per_lumi_ECC; 
      TProfile_LW*          m_TimeoutErrors_per_lumi_IBL;
      TProfile_LW*          m_TimeoutErrors_per_lumi_B0; 
      TProfile_LW*          m_TimeoutErrors_per_lumi_B1; 
      TProfile_LW*          m_TimeoutErrors_per_lumi_B2; 
      TProfile*             m_error_time1;       
      TProfile*             m_error_time2;       
      TProfile*             m_error_time3;       
      PixelMonModules1D*    m_errors;
      PixelMon2DMapsLW*       m_OpticalErrors;
      PixelMon2DMapsLW*       m_SEU_Errors;
      PixelMon2DMapsLW*       m_TimeoutErrors;
      PixelMon2DMapsLW*       m_SyncErrors;  //to be removed ?
      PixelMon2DMapsLW*       m_TruncationErrors;  //to be removed ?
      PixelMon2DMapsLW*       m_SyncErrors_mod;
      PixelMon2DMapsLW*       m_SyncErrors_ROD;
      PixelMon2DMapsLW*       m_TruncErrors_mod;
      PixelMon2DMapsLW*       m_TruncErrors_ROD;
      PixelMon2DMapsLW*       m_sync_mod_BCID1;   
      PixelMon2DMapsLW*       m_sync_mod_BCID2;   
      PixelMon2DMapsLW*       m_sync_mod_LVL1ID;
      PixelMon2DMapsLW*       m_sync_rod_BCID;   
      PixelMon2DMapsLW*       m_sync_rod_LVL1ID;   
      PixelMon2DMapsLW*       m_trunc_mod_EOC;
      PixelMon2DMapsLW*       m_trunc_mod_hitOF;
      PixelMon2DMapsLW*       m_trunc_mod_EoEOF;
      PixelMon2DMapsLW*       m_trunc_rod_HTlim;
      PixelMon2DMapsLW*       m_trunc_rod_FIFOOF;
      PixelMon2DMapsLW*       m_optical_error;
      PixelMon2DMapsLW*       m_seu_hit_parity;
      PixelMon2DMapsLW*       m_seu_reg_parity;
      PixelMon2DMapsLW*       m_seu_hamming;
      PixelMon2DMapsLW*       m_timeout;
      PixelMon2DMapsLW*       m_FEwarning;
      TProfile_LW*          m_sync_mod_BCID1_per_lumi;
      TProfile_LW*          m_sync_mod_BCID1_per_lumi_ECA;
      TProfile_LW*          m_sync_mod_BCID1_per_lumi_ECC;
      TProfile_LW*          m_sync_mod_BCID1_per_lumi_IBL;
      TProfile_LW*          m_sync_mod_BCID1_per_lumi_B0;
      TProfile_LW*          m_sync_mod_BCID1_per_lumi_B1;
      TProfile_LW*          m_sync_mod_BCID1_per_lumi_B2;
      TProfile_LW*          m_sync_mod_BCID2_per_lumi;
      TProfile_LW*          m_sync_mod_BCID2_per_lumi_ECA;
      TProfile_LW*          m_sync_mod_BCID2_per_lumi_ECC;
      TProfile_LW*          m_sync_mod_BCID2_per_lumi_IBL;
      TProfile_LW*          m_sync_mod_BCID2_per_lumi_B0;
      TProfile_LW*          m_sync_mod_BCID2_per_lumi_B1;
      TProfile_LW*          m_sync_mod_BCID2_per_lumi_B2;
      TProfile_LW*          m_sync_mod_LVL1ID_per_lumi;
      TProfile_LW*          m_sync_mod_LVL1ID_per_lumi_ECA;
      TProfile_LW*          m_sync_mod_LVL1ID_per_lumi_ECC;
      TProfile_LW*          m_sync_mod_LVL1ID_per_lumi_IBL;
      TProfile_LW*          m_sync_mod_LVL1ID_per_lumi_B0;
      TProfile_LW*          m_sync_mod_LVL1ID_per_lumi_B1;
      TProfile_LW*          m_sync_mod_LVL1ID_per_lumi_B2;
      TProfile_LW*          m_sync_rod_BCID_per_lumi;
      TProfile_LW*          m_sync_rod_BCID_per_lumi_ECA;
      TProfile_LW*          m_sync_rod_BCID_per_lumi_ECC;
      TProfile_LW*          m_sync_rod_BCID_per_lumi_IBL;
      TProfile_LW*          m_sync_rod_BCID_per_lumi_B0;
      TProfile_LW*          m_sync_rod_BCID_per_lumi_B1;
      TProfile_LW*          m_sync_rod_BCID_per_lumi_B2;
      TProfile_LW*          m_sync_rod_LVL1ID_per_lumi;
      TProfile_LW*          m_sync_rod_LVL1ID_per_lumi_ECA;
      TProfile_LW*          m_sync_rod_LVL1ID_per_lumi_ECC;
      TProfile_LW*          m_sync_rod_LVL1ID_per_lumi_IBL;
      TProfile_LW*          m_sync_rod_LVL1ID_per_lumi_B0;
      TProfile_LW*          m_sync_rod_LVL1ID_per_lumi_B1;
      TProfile_LW*          m_sync_rod_LVL1ID_per_lumi_B2;
      TProfile_LW*          m_trunc_mod_EOC_per_lumi;
      TProfile_LW*          m_trunc_mod_EOC_per_lumi_ECA;
      TProfile_LW*          m_trunc_mod_EOC_per_lumi_ECC;
      TProfile_LW*          m_trunc_mod_EOC_per_lumi_IBL;
      TProfile_LW*          m_trunc_mod_EOC_per_lumi_B0;
      TProfile_LW*          m_trunc_mod_EOC_per_lumi_B1;
      TProfile_LW*          m_trunc_mod_EOC_per_lumi_B2;
      TProfile_LW*          m_trunc_mod_hitOF_per_lumi;
      TProfile_LW*          m_trunc_mod_hitOF_per_lumi_ECA;
      TProfile_LW*          m_trunc_mod_hitOF_per_lumi_ECC;
      TProfile_LW*          m_trunc_mod_hitOF_per_lumi_IBL;
      TProfile_LW*          m_trunc_mod_hitOF_per_lumi_B0;
      TProfile_LW*          m_trunc_mod_hitOF_per_lumi_B1;
      TProfile_LW*          m_trunc_mod_hitOF_per_lumi_B2;
      TProfile_LW*          m_trunc_mod_EoEOF_per_lumi;
      TProfile_LW*          m_trunc_mod_EoEOF_per_lumi_ECA;
      TProfile_LW*          m_trunc_mod_EoEOF_per_lumi_ECC;
      TProfile_LW*          m_trunc_mod_EoEOF_per_lumi_IBL;
      TProfile_LW*          m_trunc_mod_EoEOF_per_lumi_B0;
      TProfile_LW*          m_trunc_mod_EoEOF_per_lumi_B1;
      TProfile_LW*          m_trunc_mod_EoEOF_per_lumi_B2;
      TProfile_LW*          m_trunc_rod_HTlim_per_lumi;
      TProfile_LW*          m_trunc_rod_HTlim_per_lumi_ECA;
      TProfile_LW*          m_trunc_rod_HTlim_per_lumi_ECC;
      TProfile_LW*          m_trunc_rod_HTlim_per_lumi_IBL;
      TProfile_LW*          m_trunc_rod_HTlim_per_lumi_B0;
      TProfile_LW*          m_trunc_rod_HTlim_per_lumi_B1;
      TProfile_LW*          m_trunc_rod_HTlim_per_lumi_B2;
      TProfile_LW*          m_trunc_rod_FIFOOF_per_lumi;
      TProfile_LW*          m_trunc_rod_FIFOOF_per_lumi_ECA;
      TProfile_LW*          m_trunc_rod_FIFOOF_per_lumi_ECC;
      TProfile_LW*          m_trunc_rod_FIFOOF_per_lumi_IBL;
      TProfile_LW*          m_trunc_rod_FIFOOF_per_lumi_B0;
      TProfile_LW*          m_trunc_rod_FIFOOF_per_lumi_B1;
      TProfile_LW*          m_trunc_rod_FIFOOF_per_lumi_B2;
      TProfile_LW*          m_optical_error_per_lumi;
      TProfile_LW*          m_optical_error_per_lumi_ECA;
      TProfile_LW*          m_optical_error_per_lumi_ECC;
      TProfile_LW*          m_optical_error_per_lumi_IBL;
      TProfile_LW*          m_optical_error_per_lumi_B0;
      TProfile_LW*          m_optical_error_per_lumi_B1;
      TProfile_LW*          m_optical_error_per_lumi_B2;
      TProfile_LW*          m_seu_hit_parity_per_lumi;
      TProfile_LW*          m_seu_hit_parity_per_lumi_ECA;
      TProfile_LW*          m_seu_hit_parity_per_lumi_ECC;
      TProfile_LW*          m_seu_hit_parity_per_lumi_IBL;
      TProfile_LW*          m_seu_hit_parity_per_lumi_B0;
      TProfile_LW*          m_seu_hit_parity_per_lumi_B1;
      TProfile_LW*          m_seu_hit_parity_per_lumi_B2;
      TProfile_LW*          m_seu_reg_parity_per_lumi;
      TProfile_LW*          m_seu_reg_parity_per_lumi_ECA;
      TProfile_LW*          m_seu_reg_parity_per_lumi_ECC;
      TProfile_LW*          m_seu_reg_parity_per_lumi_IBL;
      TProfile_LW*          m_seu_reg_parity_per_lumi_B0;
      TProfile_LW*          m_seu_reg_parity_per_lumi_B1;
      TProfile_LW*          m_seu_reg_parity_per_lumi_B2;
      TProfile_LW*          m_seu_hamming_per_lumi;
      TProfile_LW*          m_seu_hamming_per_lumi_ECA;
      TProfile_LW*          m_seu_hamming_per_lumi_ECC;
      TProfile_LW*          m_seu_hamming_per_lumi_IBL;
      TProfile_LW*          m_seu_hamming_per_lumi_B0;
      TProfile_LW*          m_seu_hamming_per_lumi_B1;
      TProfile_LW*          m_seu_hamming_per_lumi_B2;
      TProfile_LW*          m_timeout_per_lumi;
      TProfile_LW*          m_timeout_per_lumi_ECA;
      TProfile_LW*          m_timeout_per_lumi_ECC;
      TProfile_LW*          m_timeout_per_lumi_IBL;
      TProfile_LW*          m_timeout_per_lumi_B0;
      TProfile_LW*          m_timeout_per_lumi_B1;
      TProfile_LW*          m_timeout_per_lumi_B2;
      TProfile_LW*          m_FEwarning_per_lumi;
      TProfile_LW*          m_FEwarning_per_lumi_ECA;
      TProfile_LW*          m_FEwarning_per_lumi_ECC;
      TProfile_LW*          m_FEwarning_per_lumi_IBL;
      TProfile_LW*          m_FEwarning_per_lumi_B0;
      TProfile_LW*          m_FEwarning_per_lumi_B1;
      TProfile_LW*          m_FEwarning_per_lumi_B2;
      TH1I_LW*              m_bad_mod_errors;
      TH1I_LW*              m_bad_mod_errors_ECA;
      TH1I_LW*              m_bad_mod_errors_ECC;
      TH1I_LW*              m_bad_mod_errors_IBL;
      TH1I_LW*              m_bad_mod_errors_B0;
      TH1I_LW*              m_bad_mod_errors_B1;
      TH1I_LW*              m_bad_mod_errors_B2;
      
      PixelMon2DLumiProfiles*    m_sync_mod_BCID1_per_LB;
      PixelMon2DLumiMaps*        m_sync_mod_BCID1_int_LB;
      PixelMon2DLumiProfiles*    m_sync_mod_BCID2_per_LB;
      PixelMon2DLumiMaps*        m_sync_mod_BCID2_int_LB;
      PixelMon2DLumiProfiles*    m_sync_mod_LVL1ID_per_LB;
      PixelMon2DLumiMaps*        m_sync_mod_LVL1ID_int_LB;
      PixelMon2DLumiProfiles*    m_sync_rod_BCID_per_LB;
      PixelMon2DLumiMaps*        m_sync_rod_BCID_int_LB;
      PixelMon2DLumiProfiles*    m_sync_rod_LVL1ID_per_LB;
      PixelMon2DLumiMaps*        m_sync_rod_LVL1ID_int_LB;
      PixelMon2DLumiProfiles*    m_trunc_mod_EOC_per_LB;
      PixelMon2DLumiMaps*        m_trunc_mod_EOC_int_LB;
      PixelMon2DLumiProfiles*    m_trunc_mod_hitOF_per_LB;
      PixelMon2DLumiMaps*        m_trunc_mod_hitOF_int_LB;
      PixelMon2DLumiProfiles*    m_trunc_mod_EoEOF_per_LB;
      PixelMon2DLumiMaps*        m_trunc_mod_EoEOF_int_LB;
      PixelMon2DLumiProfiles*    m_trunc_rod_HTlim_per_LB;
      PixelMon2DLumiMaps*        m_trunc_rod_HTlim_int_LB;
      PixelMon2DLumiProfiles*    m_trunc_rod_FIFOOF_per_LB;
      PixelMon2DLumiMaps*        m_trunc_rod_FIFOOF_int_LB;
      PixelMon2DLumiProfiles*    m_optical_error_per_LB;
      PixelMon2DLumiMaps*        m_optical_error_int_LB;
      PixelMon2DLumiProfiles*    m_seu_hit_parity_per_LB;
      PixelMon2DLumiMaps*        m_seu_hit_parity_int_LB;
      PixelMon2DLumiProfiles*    m_seu_reg_parity_per_LB;
      PixelMon2DLumiMaps*        m_seu_reg_parity_int_LB;
      PixelMon2DLumiProfiles*    m_seu_hamming_per_LB;
      PixelMon2DLumiMaps*        m_seu_hamming_int_LB;
      PixelMon2DLumiProfiles*    m_timeout_per_LB;
      PixelMon2DLumiMaps*        m_timeout_int_LB;
      PixelMon2DLumiProfiles*    m_FEwarning_per_LB;
      PixelMon2DLumiMaps*        m_FEwarning_int_LB;
      PixelMon2DLumiProfiles*    m_sync_mod_corr_LVL1ID_BCID2_per_LB;
      PixelMon2DLumiProfiles*    m_sync_rod_corr_LVL1ID_BCID_per_LB;

      //SP Histograms
      TH1I_LW*  m_num_spacepoints;
      TH1I_LW*  m_num_spacepoints_low;
      TH1F_LW*  m_spHit_x;
      TH1F_LW*  m_spHit_y; 
      TH1F_LW*  m_spHit_z;
      TH1F_LW*  m_spHit_r;
      TH1F_LW*  m_spHit_phi;
      TH2F_LW*  m_spHit_xy;
      TH2F_LW*  m_spHit_rz;

      //Histograms stored for certain number of LB at a time
      PixelMon2DMaps*    m_cluster_occupancy_LB;  //cluster occupancy (shows module status)
      PixelMon2DMapsLW*    m_errors_LB;             //errors
      PixelMonProfiles*  m_status_LB;             //status of modules (shows disabled modules)
      PixelMonModules1D* m_cluster_ToT_mod_LB;
      PixelMonModules1D* m_cluster_num_mod_LB;
      PixelMonModules1D* m_hit_num_mod_LB;

      TH1I_LW*           m_num_hits_LB;
      TH1I_LW*           m_num_hits_low_LB;
      TH1F_LW*           m_cluster_ToT_LB;      
      TH1I_LW*           m_num_clusters_LB;                                
      TH1I_LW*           m_num_clusters_low_LB;  

};

#endif 
