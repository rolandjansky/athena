/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXMAINMON_H_
#define PIXMAINMON_H_

#include <string.h>
#include <vector>
#include <map>
#include <stdint.h>
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
class DBMMon2DMaps;
class PixelMon2DMapsLW;
class PixelMon2DProfilesLW;
class PixelMonProfiles;
class PixelMon2DLumiProfiles;
class PixelMon2DLumiMaps;
class StatusCode;
class PixelID;
class TH1I;
class TH1F;
class TH2I;
class TH2F;
class TH3F;
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


class ErrorCategory{
   public:
   enum ErrorCategoryID {kSync = 0, kTrunc, kOpt, kSeu, kTout, COUNT};
};

class ErrorCategoryMODROD{
   public:
   enum ErrorCategoryMODRODID {kSyncMod = 0, kSyncROD, kTruncMod, kTruncROD, kOpt, kSeu, kTout, COUNT};
};

class PixLayerIBL2D3D{
   public:
   enum PixLayerIBL2D3DID {kECA = 0, kECC, kB0, kB1, kB2, kIBL, kIBL2D, kIBL3D, COUNT};
};

class PixLayer{
   public:
   enum PixLayerID {kECA = 0, kECC, kB0, kB1, kB2, kIBL, COUNT};
};

class PixLayerDisk{
   public:
   enum PixLayerDiskID {kECA0 = 0, kECA1, kECA2, kECC0, kECC1, kECC2, kB0, kB1, kB2, kIBL, COUNT};
};

class IBLStave{
   public:
   enum IBLStaveID {kS01 = 0, kS02, kS03, kS04, kS05, kS06, kS07, kS08, kS09, kS10, kS11, kS12, kS13, kS14, COUNT};
};

class PixLayerDBM{
   public:
   enum PixLayerDBMID {kECA0 = 0, kECA1, kECA2, kECC0, kECC1, kECC2, kB0, kB1, kB2, kDBMA, kDBMC, kIBL, COUNT};
};

class PixLayerIBL2D3DDBM{
   public:
   enum PixLayerIBL2D3DDBMID {kECA0 = 0, kECA1, kECA2, kECC0, kECC1, kECC2, kB0, kB1, kB2, kDBMA, kDBMC, kIBL, kIBL2D, kIBL3D, COUNT};
};

class PixelMainMon:public ManagedMonitorToolBase 
{
public:

   PixelMainMon (const std::string & type,
       const std::string & name, 
       const IInterface* parent);
   virtual ~PixelMainMon();
   virtual StatusCode initialize(); //!< Runs once at the start of the code.  Sets up services etc.  
   virtual StatusCode bookHistograms(); //!< Runs at the start of every event.  Books some histograms if one of the function parameters is true
   virtual StatusCode fillHistograms(); //!< Runs at every event, bulk of the code is inside here.  Fills the histograms.
   virtual StatusCode procHistograms(); //!

   /** Convert the module ID in the job options.
    * Input is a string in the form "L0_B11_S2_M5A"
    * Output uses an int with each bit corisponding to part of the offline identifier.
    */
   int GetPixLayerID(int ec, int ld, bool ibl);
   int GetPixLayerIDIBL2D3D(int ec, int ld, int eta, bool ibl);
   int GetPixLayerIDIBL2D3DDBM(int ec, int ld, int eta, bool ibl);
   int GetPixLayerIDDBM(int ec, int ld, bool ibl);
   int GetPixLayerDiskID(int ec, int ld, bool ibl);
   int GetPhiID(Identifier &id, const PixelID* pixID);
   int GetEtaID(Identifier &id, const PixelID* pixID, bool doIBL, bool doIBL2D3D);
   void TH1FFillMonitoring(TH1F_LW* mon, TH1F_LW* tmp);
   void TH2FSetBinScaled(TH2F_LW* mon, TH2F_LW* tmp, int nevent);
   void FillTimeHisto(double, TProfile*, TProfile*, TProfile*, double, double, double);
   void FillSummaryHistos(PixelMon2DMaps* occupancy, TH1F_LW* A, TH1F_LW* C, TH1F_LW* IBL, TH1F_LW* B0, TH1F_LW* B1, TH1F_LW* B2);
   int ParseDetailsString(std::string & detailsMod);
   bool OnTrack(Identifier id, bool isCluster);
   int getErrorCategory(int bit, bool isibl);
	std::string makeHistname(std::string set, bool ontrk);
	std::string makeHisttitle(std::string set, std::string axis, bool ontrk);
   bool GetFEID( int pixlayer, int phiid, int etaid, int &oufephi, int &outfeeta);

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

   StatusCode BookPixelDCSMon(void);
   StatusCode FillPixelDCSMon(void);
   StatusCode ProcPixelDCSMon(void);

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
   uint64_t m_event;      
   uint64_t m_event2;

   time_t m_startTime;
   bool m_majorityDisabled; //check for each event, true if >50% modules disabled 

   int m_lbRange;
   int m_bcidRange;
   int m_lumiBlockNum;
   unsigned int m_firstBookTime;
   unsigned int m_currentTime;
   unsigned int m_LBstartTime;
   unsigned int m_LBendTime;
   unsigned int m_currentBCID;
   int m_runNum;
   int m_ntracksPerEvent;
   int m_nGood_IBL2D;
   int m_nGood_IBL3D;
   int m_nGood_B0;
   int m_nGood_B1;
   int m_nGood_B2;
   int m_nGood_ECC;
   int m_nGood_ECA;
   int m_nActivAndSync_mod[PixLayerIBL2D3D::COUNT];
   unsigned int m_nRefresh;
   unsigned int m_nRefresh5min;

   const AtlasDetectorID* m_idHelper;      

   std::vector<Identifier> m_RDOIDs;                  ///////Make class scope 
   std::vector<Identifier> m_ClusterIDs;              ///////Make class scope 

   const DataHandle<PixelRDO_Container> m_rdocontainer; //container for pixel RDO
   //PixelRODErrorCollection* m_error_summary;//container for pixel ROD errors
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

   std::string m_modLabel_PixLayerIBL2D3D[PixLayerIBL2D3D::COUNT] = {
      "ECA", "ECC", "B0", "B1", "B2",
      "IBL", "IBL2D", "IBL3D"
   };
   std::string m_modLabel_PixLayerIBL2D3DDBM[PixLayerIBL2D3DDBM::COUNT] = {
      "ECA0", "ECA1", "ECA2",
      "ECC0", "ECC1", "ECC2",
      "B0",   "B1",   "B2",
      "DBMA", "DBMC", "IBL", "IBL2D", "IBL3D"
   };

   bool m_doRDO;              //storegate type flags from jobOptions
   bool m_doRODError;
   bool m_doCluster;    
   bool m_doSpacePoint; 
   bool m_doTrack; 
   bool m_doStatus; 
   bool m_doDCS;

   bool m_doHoleSearch; 
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
   bool m_doRefresh;
   bool m_doRefresh5min;
   bool isFirstBook;
   bool m_doDegFactorMap;

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

   std::vector<double> m_hitocc_stock[PixLayer::COUNT];


   //////////////////////Histograms///////////////////////////
   
   ///
   /// Event Info
   ///
   
   TH1F_LW*                m_events_per_lumi;
   TProfile_LW*            m_mu_vs_bcid;
   TProfile_LW*            m_mu_vs_lumi;
   TH2F_LW*                m_storegate_errors;//Reconstruction (storegate) Error Monitoring

   ///
   /// Hitmap  histograms  
   ///
   /// the number of hits
   TProfile_LW*            m_hits_per_lumi;
   TH1I_LW*                m_num_hits;
   TH1I_LW*                m_num_hits_low;
   TH1F_LW*                m_nhits_mod[PixLayer::COUNT];
   TProfile_LW*            m_hits_per_lumi_mod[PixLayer::COUNT];
   TH1F_LW*                m_nlargeevt_per_lumi_mod[PixLayerIBL2D3D::COUNT];
   TH1F_LW*                m_totalhits_per_bcid_mod[PixLayerIBL2D3D::COUNT];
   PixelMon2DMaps*         m_hitmap_mon;
   PixelMon2DMaps*         m_hitmap_tmp;
   /// hit occupancy
   TProfile_LW*            m_avgocc_per_lumi;
   TProfile_LW*            m_avgocc_ratioIBLB0_per_lumi;
   TProfile_LW*            m_avgocc_per_lumi_mod[PixLayerIBL2D3D::COUNT];
   TProfile_LW*            m_avgocc_per_bcid_mod[PixLayerIBL2D3D::COUNT];
   TProfile_LW*            m_avgocc_wSyncMod_per_lumi_mod[PixLayerIBL2D3D::COUNT];
   TH2F_LW*                m_maxocc_per_lumi_mod[PixLayerIBL2D3D::COUNT];
   TH1F_LW*                m_maxocc_per_bcid_mod[PixLayerIBL2D3D::COUNT];
   PixelMon2DMaps*         m_occupancy;
   PixelMon2DMapsLW*       m_average_pixocc;
   PixelMon2DMaps*         m_occupancy_10min;
   TProfile*               m_occupancy_time1;
   TProfile*               m_occupancy_time2;
   TProfile*               m_occupancy_time3;
   TH1F_LW*                m_occupancy_summary_low_mod[PixLayer::COUNT];
   TH1F_LW*                m_occupancy_summary_mod[PixLayer::COUNT];
   TH3F*                   m_nFEswithHits_mod[PixLayer::COUNT];
   /// hit tot
   TH1F_LW*                m_hit_ToT[PixLayerIBL2D3DDBM::COUNT];
   TH1F_LW*                m_hit_ToT_Mon_mod[PixLayer::COUNT];
   TH1F_LW*                m_hit_ToT_tmp_mod[PixLayer::COUNT];
   TProfile2D_LW*          m_ToT_etaphi_mod[PixLayer::COUNT];
   TProfile_LW*            m_hit_ToTMean_mod[PixLayer::COUNT];
   /// timing
   TH1I_LW*                m_Lvl1ID_PIX;
   TH1I_LW*                m_Lvl1ID_IBL;
   TH1F_LW*                m_Atlas_BCID;
   TH2F_LW*                m_Atlas_BCID_hits;
   TH1I_LW*                m_BCID;
   TProfile_LW*            m_BCID_Profile;
   TH1F_LW*                m_Lvl1A;
   TH1F_LW*                m_Lvl1A_mod[PixLayerDBM::COUNT];
   TH1F_LW*                m_Lvl1A_10min_mod[PixLayer::COUNT];
   TH1I_LW*                m_Lvl1ID_diff_mod_ATLAS_mod[PixLayer::COUNT];
   TH1I_LW*                m_diff_ROD_vs_Module_BCID_mod[PixLayer::COUNT];
   PixelMon2DLumiProfiles* m_Lvl1ID_diff_mod_ATLAS_per_LB;
   PixelMon2DLumiProfiles* m_Lvl1ID_absdiff_mod_ATLAS_per_LB;
   /// Quick status
   TH2F_LW*                m_nhits_L0_B11_S2_C6;
   TProfile2D_LW*          m_occupancy_L0_B11_S2_C6;

   /// module histo
   PixelMonModules1D*      m_hit_num_mod;
   PixelMonModulesProf*    m_hiteff_mod;
   PixelMonModules1D*      m_FE_chip_hit_summary;
   PixelMonModules2D*      m_pixel_occupancy;
   /// ROD Sim
   TH1F_LW*                m_RodSim_BCID_minus_ToT;
   TH1F_LW*                m_RodSim_FrontEnd_minus_Lvl1ID;
   /// details
   TH1F_LW*                m_Details_mod1_num_hits;
   TH1F_LW*                m_Details_mod2_num_hits;
   TH1F_LW*                m_Details_mod3_num_hits;
   TH1F_LW*                m_Details_mod4_num_hits;
   TH2F_LW*                m_Details_mod1_occupancy;
   TH2F_LW*                m_Details_mod2_occupancy;
   TH2F_LW*                m_Details_mod3_occupancy;
   TH2F_LW*                m_Details_mod4_occupancy;
   TH1F_LW*                m_Details_mod1_ToT;     
   TH1F_LW*                m_Details_mod2_ToT;     
   TH1F_LW*                m_Details_mod3_ToT;     
   TH1F_LW*                m_Details_mod4_ToT;  


   //PixelMonModules2D*    m_pixel_noise_map;
   //PixelMonModules1D*    m_pixel_occupancy_spectrum;

   //-----------------
   // track histograms
   //-----------------
   /// Track quality
   TH1F_LW*                m_track_res_phi; 
   TH1F_LW*                m_track_pull_phi;
   TH1F_LW*                m_track_res_eta; 
   TH1F_LW*                m_track_pull_eta;
   TH1F_LW*                m_track_chi2;
   /// The number of tracks
   TProfile_LW*            m_tracksPerEvt_per_lumi;
   TProfile_LW*            m_tracksPerEvtPerMu_per_lumi;
   /// track state on surface
   PixelMon2DMapsLW*       m_tsos_hitmap;
   PixelMon2DMapsLW*       m_tsos_holemap;
   PixelMon2DMapsLW*       m_tsos_outliermap;
   //PixelMon2DProfilesLW*   m_tsos_measratio;
   //PixelMon2DProfilesLW*   m_tsos_holeratio;
   PixelMon2DProfilesLW*   m_misshits_ratio;
   PixelMon2DProfilesLW*   m_tsos_holeratio_tmp;
   PixelMon2DProfilesLW*   m_tsos_holeratio_mon;
   PixelMon2DProfilesLW*   m_misshits_ratio_tmp;
   PixelMon2DProfilesLW*   m_misshits_ratio_mon;
   /// hit efficiency
   TProfile_LW*            m_hiteff_incl_mod[PixLayerDBM::COUNT];
   TProfile_LW*            m_hiteff_actv_mod[PixLayerDBM::COUNT];
   /// Lorentz Angle
   TProfile2D_LW*          m_LorentzAngle_IBL;
   TProfile2D_LW*          m_LorentzAngle_IBL2D;
   TProfile2D_LW*          m_LorentzAngle_IBL3D;
   TProfile2D_LW*          m_LorentzAngle_B0;
   TProfile2D_LW*          m_LorentzAngle_B1;
   TProfile2D_LW*          m_LorentzAngle_B2;
   /// degradation factor
   TProfile2D_LW*          m_degFactorMap;
   TProfile_LW*            m_degFactorMap_per_lumi;
   TProfile2D_LW*          m_degFactorMap_eta_per_lumi;
   //TProfile2D_LW*          m_degFactorMap_phi_per_lumi;

   /// cluster size
   TH1F_LW*                m_clusize_ontrack_mod[PixLayerIBL2D3D::COUNT];
   TH1F_LW*                m_clusize_offtrack_mod[PixLayerIBL2D3D::COUNT];
   /// module histo
   TH1F_LW*                m_track_chi2_bcl1;
   TH1F_LW*                m_track_chi2_bcl0;
   TH1F_LW*                m_track_chi2_bclgt1;
   TH1F_LW*                m_track_chi2_bcl1_highpt;
   TH1F_LW*                m_track_chi2_bcl0_highpt;
   TH1F_LW*                m_track_chi2_bclgt1_highpt;
   TH2F_LW*                m_clustot_vs_pt;
   TH1F_LW*                m_clustot_lowpt;
   TH1F_LW*                m_1hitclustot_lowpt;
   TH1F_LW*                m_2hitclustot_lowpt;
   TH1F_LW*                m_clustot_highpt;
   TH1F_LW*                m_1hitclustot_highpt;
   TH1F_LW*                m_2hitclustot_highpt;
   PixelMonModulesProf*    m_tsos_hiteff_vs_lumi;


   ///
   /// Cluster histograms
   /// 
   TProfile_LW*          m_clusters_per_lumi;
   TProfile_LW*          m_clusters_per_lumi_mod[PixLayer::COUNT];
   TProfile_LW*          m_clusters_row_width_per_lumi_mod[PixLayerIBL2D3D::COUNT];
   TProfile_LW*          m_clusters_col_width_per_lumi_mod[PixLayerIBL2D3D::COUNT];
   TProfile_LW*          m_clusters_row_width_per_bcid_mod[PixLayerIBL2D3D::COUNT];
   TProfile_LW*          m_clusters_col_width_per_bcid_mod[PixLayerIBL2D3D::COUNT];
   TProfile_LW*          m_largeclusters_per_lumi;
   TProfile_LW*          m_verylargeclusters_per_lumi;
   TH1I_LW*              m_totalclusters_per_lumi;
   TH1I_LW*              m_totalclusters_per_lumi_mod[PixLayerIBL2D3D::COUNT];
   TH1I_LW*              m_totalclusters_per_bcid_mod[PixLayerIBL2D3D::COUNT];
   TH1I_LW*              m_highNclusters_per_lumi;
   TH1F_LW*              m_cluster_ToT1d_mod[PixLayerIBL2D3DDBM::COUNT];
   TH1F_LW*              m_1cluster_ToT_mod[PixLayer::COUNT];
   TH1F_LW*              m_2cluster_ToT_mod[PixLayer::COUNT];
   TH1F_LW*              m_3cluster_ToT_mod[PixLayer::COUNT];
   TH1F_LW*              m_bigcluster_ToT_mod[PixLayer::COUNT];

   TH1F_LW*              m_cluster_Q_mod[PixLayerIBL2D3DDBM::COUNT];
   TH1F_LW*              m_1cluster_Q_mod[PixLayer::COUNT];
   TH1F_LW*              m_2cluster_Q_mod[PixLayer::COUNT];
   TH1F_LW*              m_3cluster_Q_mod[PixLayer::COUNT];
   TH1F_LW*              m_bigcluster_Q_mod[PixLayer::COUNT];

   PixelMonProfiles*     m_clussize_map;
   PixelMonProfiles*     m_cluscharge_map;
   PixelMonProfiles*     m_clusToT_map;
   TH1F_LW*              m_cluster_groupsize;
   TH1F_LW*              m_cluster_col_width;
   TH1F_LW*              m_cluster_row_width;
   //TH1F_LW*              m_cluster_row_width_IBL;
   TH1F_LW*              m_cluster_col_width_mod[PixLayerIBL2D3D::COUNT];
   TH1F_LW*              m_cluster_row_width_mod[PixLayerIBL2D3D::COUNT];
   TH1F_LW*              m_cluster_groupsize_mod[PixLayerIBL2D3D::COUNT];
   TH1F_LW*              m_cluster_LVL1A;
   //TH1F_LW*              m_cluster_LVL1A_highToT;
   TH1F_LW*              m_cluster_LVL1A1d_mod[PixLayer::COUNT];
   TProfile_LW*          m_clusterSize_eta;
   TH2F_LW*              m_clusToT_vs_eta_mod[PixLayer::COUNT];
   //TProfile_LW*          m_clusQ_vs_eta_mod[PixLayer::COUNT];
   //TH2F_LW*              m_clusQ_vs_eta_mod[PixLayer::COUNT];
   TH2F_LW*              m_ToT_vs_clussize_mod[PixLayer::COUNT];
   //TH2F_LW*              m_Q_vs_clussize_mod[PixLayer::COUNT];
   TProfile_LW*          m_clussize_vs_eta_mod[PixLayer::COUNT];

   PixelMon2DMaps*       m_clustermap_mon;
   PixelMon2DMaps*       m_clustermap_tmp;
   PixelMon2DMaps*       m_cluster_occupancy;
   TH2F_LW*              m_cluster_occupancy_FE_B0_mon;
   //TH2F_LW*              m_cluster_occupancy_FE_L0_B11_S2_C6;
   //TH2F_LW*              m_cluster_occupancy_isgood_FE_L0_B11_S2_C6;
   TProfile*             m_cluster_occupancy_time1;
   TProfile*             m_cluster_occupancy_time2;
   TProfile*             m_cluster_occupancy_time3;

   //TProfile2D_LW*        m_cluster_totmean_L0_B11_S2_C6;
   PixelMon2DMaps*       m_clusocc_sizenot1; 
   PixelMonModulesProf*  m_cluseff_mod;
   PixelMonModules1D*    m_cluster_ToT_mod;
   PixelMonModules1D*    m_cluster_size_mod;
   PixelMonModules1D*    m_cluster_num_mod;
   //PixelMonModules1D*    m_cluster_num_mod_ontrack;
   TH1I_LW*              m_num_clusters;
   TH1I_LW*              m_num_clusters_low;
   TProfile_LW*          m_clusters_per_track_per_lumi_mod[PixLayer::COUNT];
   TH1I_LW*              m_num_clusters_mod[PixLayer::COUNT];
   TH1F_LW*              m_cluster_occupancy_summary_mod[PixLayer::COUNT];
   TH1F_LW*              m_cluster_occupancy_summary_low_mod[PixLayer::COUNT];
   /// Quick status
   TH2F_LW*              m_clusters_onTrack_L0_B11_S2_C6;
   TH2F_LW*              m_clusters_offTrack_L0_B11_S2_C6;

   int                   m_ClusPerEventArray_disksA[48][3];
   int                   m_ClusPerEventArray_disksC[48][3];
   int                   m_ClusPerEventArray_lI[14][20];
   int                   m_ClusPerEventArray_l0[22][13];
   int                   m_ClusPerEventArray_l1[38][13];
   int                   m_ClusPerEventArray_l2[52][13];
   PixelMonProfiles*     m_cluster_LVL1A_mod;
   PixelMonProfiles*     m_clus_LVL1A_sizenot1;
   TProfile_LW*          m_clustersOnOffTrack_per_lumi;
   PixelMon2DProfilesLW* m_lowToTHitsFraction_7;
   PixelMon2DProfilesLW* m_lowToTHitsFraction_9;
   PixelMon2DProfilesLW* m_lowToTHitsFraction_11;
   PixelMon2DProfilesLW* m_lowToTHitsFraction_13;

   ///
   ///Status histograms
   ///
   PixelMonModules1D*    m_Status_modules;
   PixelMonProfiles*     m_status;
   PixelMonProfiles*     m_status_mon;
   PixelMonProfiles*     m_status_LB;
   PixelMonProfiles*     m_disabled;
   PixelMon2DMaps*       m_dqStatus;
   TProfile_LW*          m_disabledModules_per_lumi_PIX;
   TProfile_LW*          m_badModules_per_lumi_mod[PixLayerIBL2D3D::COUNT];
   TProfile_LW*          m_disabledModules_per_lumi_mod[PixLayerIBL2D3D::COUNT];
   TProfile_LW*          m_baddisabledModules_per_lumi_mod[PixLayerIBL2D3D::COUNT];

   ///
   /// ROD Error histograms
   ///
   TProfile_LW*          m_ErrorFraction_per_evt[ErrorCategory::COUNT][PixLayerIBL2D3D::COUNT];
   TProfile_LW*          m_errors_per_lumi_mod[PixLayerIBL2D3D::COUNT];
   TProfile_LW*          m_SyncErrors_per_lumi_PIX;
   TProfile_LW*          m_SyncErrors_per_lumi_mod[PixLayerIBL2D3D::COUNT];
   TProfile_LW*          m_OpticalErrors_per_lumi_mod[PixLayerIBL2D3D::COUNT];
   TProfile_LW*          m_SEU_Errors_per_lumi_mod[PixLayerIBL2D3D::COUNT];
   TProfile_LW*          m_TruncationErrors_per_lumi_mod[PixLayerIBL2D3D::COUNT];
   TProfile_LW*          m_TimeoutErrors_per_lumi_mod[PixLayerIBL2D3D::COUNT]; 
   TProfile*             m_error_time1;       
   TProfile*             m_error_time2;       
   TProfile*             m_error_time3;       
   PixelMonModules1D*    m_errors;
   PixelMon2DMapsLW*     m_ErrorTypeMap[ErrorCategoryMODROD::COUNT];
   PixelMon2DMapsLW*     m_ErrorCategoryMap[ErrorCategory::COUNT];
   PixelMon2DMapsLW*     m_ErrorStateMap[16];

   TProfile_LW*          m_errorStates_per_lumi_mod[PixLayer::COUNT-1][16];
   TProfile2D_LW*        m_ErrorBit_per_lumi_mod[PixLayerIBL2D3D::COUNT];
   TProfile2D_LW*        m_Error_per_lumi_mod[PixLayerIBL2D3D::COUNT];
   TH1I_LW*              m_bad_mod_errors_mod[PixLayer::COUNT];
   TH3F*                 m_nFEswithTruncErr_mod[PixLayer::COUNT];

   TH1F_LW*              m_errors_ServiceRecordUnweight_IBL;
   TH1F_LW*              m_errors_ServiceRecordWeight_IBL;
   
   PixelMon2DLumiMaps*   m_errors_int_LB[16];
   PixelMon2DMapsLW*     m_errors_LB;
   PixelMon2DMapsLW*     m_errors_RODSync_mod;
   PixelMon2DMapsLW*     m_errors_ModSync_mod;

   ///
   /// SP Histograms
   ///
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
   PixelMonModules1D* m_cluster_ToT_mod_LB;
   PixelMonModules1D* m_cluster_num_mod_LB;
   PixelMonModules1D* m_hit_num_mod_LB;

   TH1I_LW*           m_num_hits_LB;
   TH1F_LW*           m_hit_ToT_LB_mod[PixLayer::COUNT];
   TH1I_LW*           m_num_hits_low_LB;
   TH1F_LW*           m_cluster_ToT_LB;      
   TH1I_LW*           m_num_clusters_LB;                                
   TH1I_LW*           m_num_clusters_low_LB;  

   //---------------------
   // DCS Monitoring
   //---------------------

   //// Basic histograms ////
   // TEMPERATURE
   TProfile2D_LW* m_hist_moduleTemperatureEtaPhi;
   TH2F_LW* m_hist_moduleTemperature2Dscatter[IBLStave::COUNT];
   TProfile_LW* m_hist_moduleTemperatureLB[IBLStave::COUNT];
   TProfile2D_LW* m_hist_LB_staveID_moduleTemperature;
   TProfile2D_LW* m_hist_LB_moduleGroup_moduleTemperature[IBLStave::COUNT];
   // HV
   TProfile2D_LW* m_hist_HVoltageEtaPhi;
   TH2F_LW* m_hist_HVoltage2Dscatter[IBLStave::COUNT];
   TProfile_LW* m_hist_HVoltageLB[IBLStave::COUNT];
   TProfile2D_LW* m_hist_LB_staveID_HVoltage;
   TProfile2D_LW* m_hist_LB_moduleGroup_HVoltage[IBLStave::COUNT];
   // PIPES Inlet
   //TProfile2D_LW* m_hist_Pipes_inletEtaPhi;
   TH2F_LW* m_hist_Pipes_inlet2Dscatter;
   TProfile_LW* m_hist_Pipes_inletLB[IBLStave::COUNT];
   TProfile2D_LW* m_hist_LB_staveID_coolingPipeInlet;
   //TProfile2D_LW* m_hist_LB_moduleGroup_coolingPipeInlet[IBLStave::COUNT];
   // PIPES Outlet
   //TProfile2D_LW* m_hist_Pipes_outletEtaPhi;
   TH2F_LW* m_hist_Pipes_outlet2Dscatter;
   TProfile_LW* m_hist_Pipes_outletLB[IBLStave::COUNT];
   TProfile2D_LW* m_hist_LB_staveID_coolingPipeOutlet;
   //TProfile2D_LW* m_hist_LB_moduleGroup_coolingPipeOutlet[IBLStave::COUNT];
   // LV
   TProfile2D_LW* m_hist_LVoltageEtaPhi;
   TH2F_LW* m_hist_LVoltage2Dscatter[IBLStave::COUNT];
   TProfile_LW* m_hist_LVoltageLB[IBLStave::COUNT];
   TProfile2D_LW* m_hist_LB_staveID_LVoltage;
   TProfile2D_LW* m_hist_LB_moduleGroup_LVoltage[IBLStave::COUNT];
   // LV CURRENT
   TProfile2D_LW* m_hist_LVcurrentEtaPhi;
   TH2F_LW* m_hist_LVcurrent2Dscatter[IBLStave::COUNT];
   TProfile_LW* m_hist_LVcurrentLB[IBLStave::COUNT];
   TProfile2D_LW* m_hist_LB_staveID_LVcurrent;
   TProfile2D_LW* m_hist_LB_moduleGroup_LVcurrent[IBLStave::COUNT];
   // HVCURRENT
   TProfile2D_LW* m_hist_HVcurrentEtaPhi;
   TH2F_LW* m_hist_HVcurrent2Dscatter[IBLStave::COUNT];
   TProfile_LW* m_hist_HVcurrentLB[IBLStave::COUNT];
   TProfile2D_LW* m_hist_LB_staveID_HVcurrent;
   TProfile2D_LW* m_hist_LB_moduleGroup_HVcurrent[IBLStave::COUNT];
   // FSMSTATE
   //TProfile2D_LW* m_hist_FSMstateEtaPhi;
   //TH2F_LW* m_hist_FSMstateEtaPhi;
   TH2F* m_hist_FSMstateEtaPhi;
   TH2F_LW* m_hist_FSMstate2Dscatter[IBLStave::COUNT];
   //TProfile_LW* m_hist_FSMstateLB[IBLStave::COUNT];
   TH2F_LW* m_hist_FSMstateLB[IBLStave::COUNT];
   //TProfile2D_LW* m_hist_LB_staveID_FSMstate;
   //TH2F_LW* m_hist_LB_staveID_FSMstate;
   TH2F* m_hist_LB_staveID_FSMstate;
   //TProfile2D_LW* m_hist_LB_moduleGroup_FSMstate[IBLStave::COUNT];
   //TProfile2D* m_hist_LB_moduleGroup_FSMstate[IBLStave::COUNT];
   TH2F* m_hist_LB_moduleGroup_FSMstate[IBLStave::COUNT];
   // FSMSTATUS
   //TProfile2D_LW* m_hist_FSMstatusEtaPhi;
   //TH2F_LW* m_hist_FSMstatusEtaPhi;
   TH2F* m_hist_FSMstatusEtaPhi;
   TH2F_LW* m_hist_FSMstatus2Dscatter[IBLStave::COUNT];
   //TProfile_LW* m_hist_FSMstatusLB[IBLStave::COUNT];
   TH2F_LW* m_hist_FSMstatusLB[IBLStave::COUNT];
   //TProfile2D_LW* m_hist_LB_staveID_FSMstatus;
   //TH2F_LW* m_hist_LB_staveID_FSMstatus;
   TH2F* m_hist_LB_staveID_FSMstatus;
   //TProfile2D_LW* m_hist_LB_moduleGroup_FSMstatus[IBLStave::COUNT];
   TH2F* m_hist_LB_moduleGroup_FSMstatus[IBLStave::COUNT];
   // PLANTS

   //// Combination histograms ////
   // dT := module temperature - cooling pipe outlet
   TProfile2D_LW* m_hist_dTEtaPhi;
   TProfile2D_LW* m_hist_LB_moduleGroup_dT[IBLStave::COUNT];
   TProfile2D_LW* m_hist_LB_staveID_dT;
   // LV Power consumption
   TProfile2D_LW* m_hist_LVPowerConsumptionEtaPhi;
   TProfile2D_LW* m_hist_LB_moduleGroup_LVPowerConsumption[IBLStave::COUNT];
   TProfile2D_LW* m_hist_LB_staveID_LVPowerConsumption;
   // HV Power consumption
   TProfile2D_LW* m_hist_HVPowerConsumptionEtaPhi;
   TProfile2D_LW* m_hist_LB_moduleGroup_HVPowerConsumption[IBLStave::COUNT];
   TProfile2D_LW* m_hist_LB_staveID_HVPowerConsumption;
   // LV+HV Power consumption
   TProfile2D_LW* m_hist_LVHVPowerConsumptionEtaPhi;
   TProfile2D_LW* m_hist_LB_moduleGroup_LVHVPowerConsumption[IBLStave::COUNT];
   TProfile2D_LW* m_hist_LB_staveID_LVHVPowerConsumption;
   // effective FLEX temperature = dT * 0.6 + cooling pipe outlet
   TProfile2D_LW* m_hist_effFLEXtempEtaPhi;
   TProfile2D_LW* m_hist_LB_moduleGroup_effFLEXtemp[IBLStave::COUNT];
   TProfile2D_LW* m_hist_LB_staveID_effFLEXtemp;
   // thermal figure of merit (/size) = dT/Power consumption
   TProfile2D_LW* m_hist_thermalFigureMeritEtaPhi;
   TProfile2D_LW* m_hist_LB_moduleGroup_thermalFigureMerit[IBLStave::COUNT];
   TProfile2D_LW* m_hist_LB_staveID_thermalFigureMerit;

   std::vector<std::string> m_atrcollist;
   std::map<std::string,std::vector< std::string> > m_elementsMap;
   int m_currentLumiBlockNumber;
   //typedef struct dcsDataHolder
   class dcsDataHolder {
     public:
       dcsDataHolder() :
         m_values(new std::map<int, std::map<int, float>* >),
         m_maps(new std::map<std::string, int>)
       {};
       ~dcsDataHolder() {
         for (auto lbmap : *m_values) {
           delete lbmap.second;
         }
         delete m_values;
         delete m_maps;
       };
       std::map<int, std::map<int, float>* > *m_values;  // <channel number, <LB, value> >
       std::map<std::string, int> *m_maps;               // <module name, channel number>
   };
   dcsDataHolder *m_moduleTemperature;
   dcsDataHolder *m_coolingPipeTemperatureInlet;
   dcsDataHolder *m_coolingPipeTemperatureOutlet;
   dcsDataHolder *m_HV;
   dcsDataHolder *m_HV_current;
   dcsDataHolder *m_LV_voltage;
   dcsDataHolder *m_LV_current;
   dcsDataHolder *m_FSM_state;
   dcsDataHolder *m_FSM_status;

   class moduleDcsDataHolder {
     public:
       moduleDcsDataHolder() :
         m_tempModule(new std::map<int, std::map<int, float>* >),
         m_tempInlet (new std::map<int, std::map<int, float>* >),
         m_tempOutlet(new std::map<int, std::map<int, float>* >),
         m_hv_voltage(new std::map<int, std::map<int, float>* >),
         m_lv_voltage(new std::map<int, std::map<int, float>* >),
         m_hv_current(new std::map<int, std::map<int, float>* >),
         m_lv_current(new std::map<int, std::map<int, float>* >),
         m_fsm_state (new std::map<int, std::map<int, float>* >),
         m_fsm_status(new std::map<int, std::map<int, float>* >),
         m_moduleMap(new std::map<std::string, int>)
       {};
       ~moduleDcsDataHolder() {
         for (auto lbmap : *m_tempModule) delete lbmap.second;
         for (auto lbmap : *m_tempInlet ) delete lbmap.second;
         for (auto lbmap : *m_tempOutlet) delete lbmap.second;
         for (auto lbmap : *m_hv_voltage) delete lbmap.second;
         for (auto lbmap : *m_lv_voltage) delete lbmap.second;
         for (auto lbmap : *m_hv_current) delete lbmap.second;
         for (auto lbmap : *m_lv_current) delete lbmap.second;
         for (auto lbmap : *m_fsm_state ) delete lbmap.second;
         for (auto lbmap : *m_fsm_status) delete lbmap.second;
         delete m_tempModule;
         delete m_tempInlet;
         delete m_hv_voltage;
         delete m_lv_voltage;
         delete m_hv_current;
         delete m_lv_current;
         delete m_fsm_state;
         delete m_fsm_status;
         delete m_moduleMap;
       };
       // <module number, <LB, value> >
       std::map<int, std::map<int, float>* > *m_tempModule;
       std::map<int, std::map<int, float>* > *m_tempInlet;
       std::map<int, std::map<int, float>* > *m_tempOutlet;
       std::map<int, std::map<int, float>* > *m_hv_voltage;
       std::map<int, std::map<int, float>* > *m_lv_voltage;
       std::map<int, std::map<int, float>* > *m_hv_current;
       std::map<int, std::map<int, float>* > *m_lv_current;
       std::map<int, std::map<int, float>* > *m_fsm_state;
       std::map<int, std::map<int, float>* > *m_fsm_status;
       // <module name, channel number>
       std::map<std::string, int> *m_moduleMap;
   };
   moduleDcsDataHolder *m_moduleDCSDataHolder;

public:
   int getIBLetaIndexFromModuleName(std::string moduleName){
     int module_eta = m_name2etaIndex[moduleName.substr(static_cast<int>(moduleName.find("S")) + 4)];
     return module_eta;
   }

   int getIBLmoduleGroupFromModuleName(std::string moduleName){
     int moduleGroup = m_name2moduleGroup[moduleName.substr(static_cast<int>(moduleName.find("S")) + 4, 4)]; // LI_S01_A_M4_A8_2
     return moduleGroup;
   }

   int getIBLstaveIdFromModuleName(std::string moduleName){
     std::string staveNumStr(moduleName, moduleName.find("S") + 1, 2); // LI_S01_A_...
     int staveNum = std::stoi(staveNumStr);
     return staveNum;
   }

private:
   std::map<std::string, int> m_name2etaIndex;
   std::map<std::string, int> m_name2moduleGroup;

   std::map<std::string, float> m_fsmState2enum;
   std::map<std::string, float> m_fsmStatus2enum;
}; // end class PixelMainMon : public ManagedMonitorToolBase


#endif 
