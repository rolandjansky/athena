/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXMAINMON_H_
#define PIXMAINMON_H_

#include <stdint.h>
#include <string.h>
#include <time.h>
#include <map>
#include <vector>
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "GaudiKernel/ServiceHandle.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "TrkTrack/TrackCollection.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "LumiBlockComps/ILuminosityTool.h"
#include "PixelGeoModel/IBLParameterSvc.h"

class PixelMonModules1D;
class PixelMonModulesProf;
class PixelMonModules2D;
class PixelMon2DMapsLW;
class PixelMon2DProfilesLW;
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
namespace InDetDD { class PixelDetectorManager; }
namespace InDet {
class PixelCluster;
class IInDetTrackSelectionTool;
}  // namespace InDet
namespace Trk {
class SpacePoint;
class ITrackHoleSearchTool;
}  // namespace Trk
class IPixelCablingSvc;
class SpacePointContainer;
class IPixelByteStreamErrorsSvc;
class IInDetConditionsSvc;
class PixelRDORawData;

typedef InDet::PixelCluster PixelCluster;
typedef InDetRawDataCollection<PixelRDORawData> RDO_Collection;
typedef Trk::SpacePoint SpacePoint;

class ErrorCategory {
 public:
  enum ErrorCategoryID {kSync = 0, kTrunc, kOpt, kSeu, kTout, COUNT};
};

class ErrorCategoryMODROD {
 public:
  enum ErrorCategoryMODRODID {kSyncMod = 0, kSyncROD, kTruncMod, kTruncROD, kOpt, kSeu, kTout, COUNT};
};

class PixLayerIBL2D3D {
 public:
  enum PixLayerIBL2D3DID {kECA = 0, kECC, kB0, kB1, kB2, kIBL, kIBL2D, kIBL3D, COUNT};
};

class PixLayer {
 public:
  enum PixLayerID {kECA = 0, kECC, kB0, kB1, kB2, kIBL, COUNT};
};

class PixLayerDisk {
 public:
  enum PixLayerDiskID {kECA0 = 0, kECA1, kECA2, kECC0, kECC1, kECC2, kB0, kB1, kB2, kIBL, COUNT};
};

class IBLStave {
 public:
  enum IBLStaveID {kS01 = 0, kS02, kS03, kS04, kS05, kS06, kS07, kS08, kS09, kS10, kS11, kS12, kS13, kS14, COUNT};
};

class PixLayerDBM {
 public:
  enum PixLayerDBMID {kECA = 0, kECC, kB0, kB1, kB2, kDBMA, kDBMC, kIBL, COUNT};
};

class PixLayerIBL2D3DDBM {
 public:
  enum PixLayerIBL2D3DDBMID {kECA = 0, kECC, kB0, kB1, kB2, kDBMA, kDBMC, kIBL, kIBL2D, kIBL3D, COUNT};
};

class PixelMainMon : public ManagedMonitorToolBase {
 public:
  PixelMainMon(const std::string& type, const std::string& name, const IInterface* parent);
  PixelMainMon (const PixelMainMon&) = delete;
  PixelMainMon& operator = (const PixelMainMon&) = delete;
  virtual ~PixelMainMon();
  virtual StatusCode initialize();      //!< Runs once at the start of the code.  Sets up services etc.
  virtual StatusCode bookHistograms();  //!< Runs at the start of every event.  Books some histograms if one of the function parameters is true
  virtual StatusCode fillHistograms();  //!< Runs at every event, bulk of the code is inside here.  Fills the histograms.
  virtual StatusCode procHistograms();  //!

  int getPixLayerID(int ec, int ld, bool ibl);
  int getPixLayerIDWithDBM(int id);
  int getPixLayerIDIBL2D3D(int ec, int ld, int eta, bool ibl);
  int getPixLayerIDIBL2D3DDBM(int ec, int ld, int eta, bool ibl);
  int getPixLayerIDDBM(int ec, int ld, bool ibl);
  int getPixLayerDiskID(int ec, int ld, bool ibl);
  void th1FillMonitoring(TH1F_LW* mon, TH1F_LW* tmp);
  void fillSummaryHistos(PixelMon2DMapsLW* occupancy, TH1F_LW* A, TH1F_LW* C, TH1F_LW* IBL, TH1F_LW* B0, TH1F_LW* B1, TH1F_LW* B2);
  void fillPP0Histos(PixelMon2DMapsLW* occupancy, TProfile_LW* D_A, TProfile_LW* D_C, TProfile_LW* B0, TProfile_LW* B1, TProfile_LW* B2, TProfile_LW* IBL_A, TProfile_LW* IBL_C);
  void formatPP0Histos(TProfile_LW* D_A, TProfile_LW* D_C, TProfile_LW* B0, TProfile_LW* B1, TProfile_LW* B2, TProfile_LW* IBL_A, TProfile_LW* IBL_C);
  int parseDetailsString(std::string& detailsMod);
  bool isOnTrack(Identifier id, bool isCluster);
  bool isOnTrack(Identifier id, double& cosalpha);
  unsigned int getEventBitLength(const Identifier& WaferID, const unsigned int& num_femcc_errorwords);
  double getBitStreamFraction(const Identifier& WaferID, const unsigned int& bits);
  int getErrorState(int bit, bool isibl);
  std::string makeHistname(std::string set, bool ontrk);
  std::string makeHisttitle(std::string set, std::string axis, bool ontrk);
  bool getFEID(int pixlayer, int phiid, int etaid, int& oufephi, int& outfeeta);
  void divide_TH1F_LW(TH1F_LW* num, TH1F_LW* den); //!< returns num = num/den w/ error propagation

  StatusCode bookClustersMon(void);
  StatusCode bookClustersLumiBlockMon(void);
  StatusCode fillClustersMon(void);
  StatusCode procClustersMon(void);

  StatusCode bookHitsMon(void);
  StatusCode bookHitsLumiBlockMon(void);
  StatusCode fillHitsMon(void);
  StatusCode procHitsMon(void);

  StatusCode bookStatusMon(void);
  StatusCode bookStatusLumiBlockMon(void);
  StatusCode fillStatusMon(void);
  StatusCode procStatusMon(void);

  StatusCode bookTrackMon(void);
  StatusCode bookTrackLumiBlockMon(void);
  StatusCode fillTrackMon(void);
  StatusCode procTrackMon(void);

  StatusCode bookRODErrorMon(void);
  StatusCode bookRODErrorLumiBlockMon(void);
  StatusCode fillRODErrorMon(void);

  StatusCode bookSpacePointMon(void);
  StatusCode fillSpacePointMon(void);

  StatusCode bookPixelDCSMon(void);
  StatusCode fillPixelDCSMon(void);
  StatusCode procPixelDCSMon(void);

 private:
  ServiceHandle<IInDetConditionsSvc> m_pixelCondSummarySvc;
  ServiceHandle<IPixelByteStreamErrorsSvc> m_ErrorSvc;
  ServiceHandle<IPixelCablingSvc> m_pixelCableSvc;
  ServiceHandle<IBLParameterSvc> m_IBLParameterSvc;
  ToolHandle<Trk::ITrackHoleSearchTool> m_holeSearchTool;
  ToolHandle<InDet::IInDetTrackSelectionTool> m_trackSelTool;
  ToolHandle<ILuminosityTool> m_lumiTool;

  const PixelID* m_pixelid;
  uint64_t m_event;
  uint64_t m_event5min;

  time_t m_startTime;
  bool m_majorityDisabled;  // check for each event, true if >50% modules disabled

  int m_lbRange;
  int m_bcidRange;
  int m_lumiBlockNum;
  unsigned int m_firstBookTime;
  unsigned int m_currentTime;
  unsigned int m_currentBCID;
  int m_runNum;
  int m_ntracksPerEvent;
  int m_nGood_mod[PixLayerIBL2D3DDBM::COUNT];
  int m_nActive_mod[PixLayerIBL2D3DDBM::COUNT];

  int m_nRefresh;
  int m_nRefresh5min;

  const AtlasDetectorID* m_idHelper;

  std::vector<Identifier> m_RDOIDs;
  std::vector<std::pair<Identifier, double> > m_ClusterIDs;

  const DataHandle<PixelRDO_Container> m_rdocontainer;
  const InDet::PixelClusterContainer* m_Pixel_clcontainer;
  const SpacePointContainer* m_Pixel_spcontainer;
  const TrackCollection* m_tracks;

  std::string m_Pixel_RDOName;
  std::string m_detector_error_name;
  std::string m_Pixel_SpacePointsName;
  std::string m_Pixel_SiClustersName;
  std::string m_TracksName;

  std::string m_histTitleExt;

  std::string m_modLabel_PixLayerIBL2D3D[PixLayerIBL2D3D::COUNT] = {
    "ECA", "ECC", "B0", "B1", "B2",
    "IBL", "IBL2D", "IBL3D"
  };
  std::string m_modLabel_PixLayerDBM[PixLayerDBM::COUNT] = {
    "ECA", "ECC",
    "B0",   "B1",   "B2",
    "DBMA", "DBMC", "IBL"
  };
  std::string m_modLabel_PixLayerIBL2D3DDBM[PixLayerIBL2D3DDBM::COUNT] = {
    "ECA", "ECC",
    "B0",   "B1",   "B2",
    "DBMA", "DBMC", "IBL", "IBL2D", "IBL3D"
  };
  std::string m_modLabel_PixLayerDisk[PixLayerDisk::COUNT] = {
    "ECA0", "ECA1", "ECA2",
    "ECC0", "ECC1", "ECC2",
    "B0",   "B1",   "B2", "IBL"
  };

  bool m_doRDO;
  bool m_doRODError;
  bool m_doCluster;
  bool m_doSpacePoint;
  bool m_doTrack;
  bool m_doStatus;
  bool m_doDCS;

  bool m_doHoleSearch;
  bool m_do2DMaps;
  bool m_doModules;
  bool m_doOffline;
  bool m_doOnline;
  bool m_doLowOccupancy;
  bool m_doHighOccupancy;
  bool m_doOnTrack;
  bool m_doPixelOccupancy;
  bool m_doDetails;
  bool m_doTiming;
  bool m_doLumiBlock;
  bool m_doRefresh;
  bool m_doRefresh5min;
  bool m_isFirstBook;

  bool m_doHeavyIonMon;

  std::string m_DetailsMod1;
  std::string m_DetailsMod2;
  std::string m_DetailsMod3;
  std::string m_DetailsMod4;

  bool m_doIBL;

  bool m_isNewRun;
  bool m_newLowStatInterval;

  int m_ClusPerEventArray_disksA[48][3];
  int m_ClusPerEventArray_disksC[48][3];
  int m_ClusPerEventArray_lI[14][20];
  int m_ClusPerEventArray_l0[22][13];
  int m_ClusPerEventArray_l1[38][13];
  int m_ClusPerEventArray_l2[52][13];

  int m_HitPerEventArray_disksA[48][3] = {{0}};
  int m_HitPerEventArray_disksC[48][3] = {{0}};
  // Effective number of hit words based on phi pairing
  int m_HitPerEventArray_lI_phi_paired[14][20] = {{0}};
  int m_HitPerEventArray_lI[14][20] = {{0}};
  int m_HitPerEventArray_l0[22][13] = {{0}};
  int m_HitPerEventArray_l1[38][13] = {{0}};
  int m_HitPerEventArray_l2[52][13] = {{0}};

  //////////////////////Histograms///////////////////////////

  // event info
  TH1F_LW* m_events_per_lumi;
  TProfile_LW* m_mu_vs_bcid;
  TProfile_LW* m_mu_vs_lumi;
  TH2F_LW* m_storegate_errors;

  // number of hits
  TProfile_LW* m_hits_per_lumi;
  TH1I_LW* m_num_hits;
  std::unique_ptr<PixelMon2DMapsLW> m_hitmap_mon;
  std::unique_ptr<PixelMon2DMapsLW> m_hitmap_tmp;
  TH1F_LW* m_nhits_mod[PixLayer::COUNT];
  TProfile_LW* m_hits_per_lumi_mod[PixLayer::COUNT];
  TH1F_LW* m_totalhits_per_bcid_mod[PixLayerIBL2D3D::COUNT];

  // hit occupancy
  TProfile_LW* m_avgocc_per_lumi;
  TProfile_LW* m_avgocc_per_lumi_mod[PixLayerIBL2D3DDBM::COUNT];
  TH1F* m_avgocc_ratio_lastXlb_mod[PixLayer::COUNT];
  TProfile* m_avgocc_ratio_lastXlb_mod_prof[PixLayer::COUNT];
  TProfile_LW* m_avgocc_per_bcid_mod[PixLayerIBL2D3DDBM::COUNT];
  TProfile_LW* m_avgocc_active_per_lumi_mod[PixLayerIBL2D3D::COUNT];
  TH2F_LW* m_maxocc_per_lumi_mod[PixLayerIBL2D3D::COUNT];
  TH2F_LW* m_modocc_per_lumi[PixLayer::COUNT];
  TH1F_LW* m_maxocc_per_bcid_mod[PixLayerIBL2D3D::COUNT];
  std::unique_ptr<PixelMon2DMapsLW> m_occupancy;
  std::unique_ptr<PixelMon2DMapsLW> m_average_pixocc;
  std::unique_ptr<PixelMon2DProfilesLW> m_occupancy_pix_evt;
  std::unique_ptr<PixelMon2DMapsLW> m_occupancy_10min;
  TH1F_LW* m_occupancy_summary_mod[PixLayer::COUNT];
  TProfile_LW* m_occupancy_PP0_ECA;
  TProfile_LW* m_occupancy_PP0_ECC;
  TProfile_LW* m_occupancy_PP0_B0;
  TProfile_LW* m_occupancy_PP0_B1;
  TProfile_LW* m_occupancy_PP0_B2;
  TProfile_LW* m_occupancy_PP0_IBLA;
  TProfile_LW* m_occupancy_PP0_IBLC;
  TProfile_LW* m_cluster_occupancy_PP0_ECA;
  TProfile_LW* m_cluster_occupancy_PP0_ECC;
  TProfile_LW* m_cluster_occupancy_PP0_B0;
  TProfile_LW* m_cluster_occupancy_PP0_B1;
  TProfile_LW* m_cluster_occupancy_PP0_B2;
  TProfile_LW* m_cluster_occupancy_PP0_IBLA;
  TProfile_LW* m_cluster_occupancy_PP0_IBLC;
  TH3F* m_nFEswithHits_mod[PixLayer::COUNT];

  // hit tot
  TH1F_LW* m_hit_ToT[PixLayerIBL2D3DDBM::COUNT];
  TH1F_LW* m_hit_ToT_tmp_mod[PixLayer::COUNT];
  TH1F_LW* m_hit_ToT_Mon_mod[PixLayer::COUNT];
  TProfile_LW* m_hit_ToTMean_mod[PixLayer::COUNT];

  // timing
  TH1I_LW* m_Lvl1ID_PIX;
  TH1I_LW* m_Lvl1ID_IBL;
  TH1F_LW* m_Atlas_BCID;
  TH2F_LW* m_Atlas_BCID_hits;
  TH1I_LW* m_BCID;
  TProfile_LW* m_BCID_Profile;
  TH1F_LW* m_Lvl1A;
  TH1F_LW* m_Lvl1A_mod[PixLayerDBM::COUNT];
  TH1F_LW* m_Lvl1A_10min_mod[PixLayer::COUNT];
  TH1I_LW* m_Lvl1ID_diff_mod_ATLAS_mod[PixLayer::COUNT];
  TH1I_LW* m_diff_ROD_vs_Module_BCID_mod[PixLayer::COUNT];
  std::unique_ptr<PixelMon2DLumiProfiles> m_Lvl1ID_diff_mod_ATLAS_per_LB;
  std::unique_ptr<PixelMon2DLumiProfiles> m_Lvl1ID_absdiff_mod_ATLAS_per_LB;

  // module histograms
  std::unique_ptr<PixelMonModulesProf> m_hiteff_mod;
  std::unique_ptr<PixelMonModules1D> m_FE_chip_hit_summary;
  std::unique_ptr<PixelMonModules2D> m_pixel_occupancy;

  // detailed histograms
  TH1F_LW* m_Details_mod1_num_hits;
  TH1F_LW* m_Details_mod2_num_hits;
  TH1F_LW* m_Details_mod3_num_hits;
  TH1F_LW* m_Details_mod4_num_hits;
  TH2F_LW* m_Details_mod1_occupancy;
  TH2F_LW* m_Details_mod2_occupancy;
  TH2F_LW* m_Details_mod3_occupancy;
  TH2F_LW* m_Details_mod4_occupancy;
  TH1F_LW* m_Details_mod1_ToT;
  TH1F_LW* m_Details_mod2_ToT;
  TH1F_LW* m_Details_mod3_ToT;
  TH1F_LW* m_Details_mod4_ToT;

  // track quality
  TH1F_LW* m_track_res_phi;
  TH1F_LW* m_track_pull_phi;
  TH1F_LW* m_track_res_eta;
  TH1F_LW* m_track_pull_eta;
  TH1F_LW* m_track_chi2;
  TProfile_LW* m_tracksPerEvt_per_lumi;
  TProfile_LW* m_tracksPerEvtPerMu_per_lumi;

  // track state on surface
  std::unique_ptr<PixelMon2DMapsLW> m_tsos_hitmap;
  std::unique_ptr<PixelMon2DMapsLW> m_tsos_holemap;
  std::unique_ptr<PixelMon2DMapsLW> m_tsos_outliermap;
  std::unique_ptr<PixelMon2DProfilesLW> m_tsos_holeratio_tmp;
  std::unique_ptr<PixelMon2DProfilesLW> m_tsos_holeratio_mon;
  std::unique_ptr<PixelMon2DProfilesLW> m_misshits_ratio_tmp;
  std::unique_ptr<PixelMon2DProfilesLW> m_misshits_ratio_mon;

  // hit efficiency
  TProfile_LW* m_hiteff_incl_mod[PixLayer::COUNT];
  TProfile* m_hiteff_lastXlb_mod[PixLayer::COUNT];

  // npixhits/track/lumi
  TH2F_LW* m_npixhits_per_track_lumi;
  TH2F* m_npixhits_per_track_lastXlb;

  // zoomed clusterToTxcosAlpha peak region
  TH2F_LW* m_zoomed_clusterToTcosA_lumi_IBL;
  TH2F* m_zoomed_clusterToTcosA_lastXlb_IBL;
  TH2F_LW* m_zoomed_clusterToTcosA_lumi_B0;
  TH2F* m_zoomed_clusterToTcosA_lastXlb_B0;
  TH2F_LW* m_zoomed_clusterToTcosA_lumi_B1;
  TH2F* m_zoomed_clusterToTcosA_lastXlb_B1;
  TH2F_LW* m_zoomed_clusterToTcosA_lumi_B2;
  TH2F* m_zoomed_clusterToTcosA_lastXlb_B2;
  TH2F_LW* m_zoomed_clusterToTcosA_lumi_ECA;
  TH2F* m_zoomed_clusterToTcosA_lastXlb_ECA;
  TH2F_LW* m_zoomed_clusterToTcosA_lumi_ECC;
  TH2F* m_zoomed_clusterToTcosA_lastXlb_ECC;

  // cluster size
  TH1F_LW* m_clusize_ontrack_mod[PixLayerIBL2D3D::COUNT];
  TH1F_LW* m_clusize_offtrack_mod[PixLayerIBL2D3D::COUNT];

  // cluster histograms
  TProfile_LW* m_clusters_per_lumi;
  TProfile_LW* m_clusters_per_lumi_mod[PixLayer::COUNT];
  TProfile_LW* m_clusters_row_width_per_lumi_mod[PixLayerIBL2D3D::COUNT];
  TProfile_LW* m_clusters_col_width_per_lumi_mod[PixLayerIBL2D3D::COUNT];
  TProfile_LW* m_clusters_row_width_per_bcid_mod[PixLayerIBL2D3D::COUNT];
  TProfile_LW* m_clusters_col_width_per_bcid_mod[PixLayerIBL2D3D::COUNT];
  TH1I_LW* m_totalclusters_per_lumi;
  TH1I_LW* m_totalclusters_per_lumi_mod[PixLayerIBL2D3D::COUNT];
  TH1I_LW* m_totalclusters_per_bcid_mod[PixLayerIBL2D3D::COUNT];
  TH1F_LW* m_cluster_ToT1d_mod[PixLayerIBL2D3DDBM::COUNT];
  TH1F_LW* m_cluster_ToT1d_corr[PixLayerIBL2D3DDBM::COUNT];
  TH1F_LW* m_cluster_Q_mod[PixLayerIBL2D3DDBM::COUNT];
  TH1F_LW* m_cluster_Q_corr[PixLayerIBL2D3DDBM::COUNT];
  std::unique_ptr<PixelMon2DProfilesLW> m_clussize_map;
  std::unique_ptr<PixelMon2DProfilesLW> m_cluscharge_map;
  std::unique_ptr<PixelMon2DProfilesLW> m_clusToT_map;
  TH1F_LW* m_cluster_groupsize;
  TH1F_LW* m_cluster_col_width;
  TH1F_LW* m_cluster_row_width;
  TH1F_LW* m_cluster_col_width_mod[PixLayerIBL2D3D::COUNT];
  TH1F_LW* m_cluster_row_width_mod[PixLayerIBL2D3D::COUNT];
  TH1F_LW* m_cluster_groupsize_mod[PixLayerIBL2D3DDBM::COUNT];
  TH1F_LW* m_cluster_LVL1A;
  TH1F_LW* m_cluster_LVL1A1d_mod[PixLayer::COUNT];
  TProfile_LW* m_clusterSize_eta;
  TProfile_LW* m_clussize_vs_eta_mod[PixLayer::COUNT];

  std::unique_ptr<PixelMon2DMapsLW> m_clustermap_mon;
  std::unique_ptr<PixelMon2DMapsLW> m_clustermap_tmp;
  std::unique_ptr<PixelMon2DMapsLW> m_cluster_occupancy;

  std::unique_ptr<PixelMon2DMapsLW> m_clusocc_sizenot1;
  std::unique_ptr<PixelMonModulesProf> m_cluseff_mod;
  std::unique_ptr<PixelMonModules1D> m_cluster_ToT_mod;
  std::unique_ptr<PixelMonModules1D> m_cluster_size_mod;
  std::unique_ptr<PixelMonModules1D> m_cluster_num_mod;
  TH1I_LW* m_num_clusters;
  TProfile_LW* m_clusters_per_track_per_lumi_mod[PixLayer::COUNT];
  TH1I_LW* m_num_clusters_mod[PixLayer::COUNT];
  TH1F_LW* m_cluster_occupancy_summary_mod[PixLayer::COUNT];
  std::unique_ptr<PixelMon2DProfilesLW> m_cluster_LVL1A_mod;
  std::unique_ptr<PixelMon2DProfilesLW> m_clus_LVL1A_sizenot1;
  TProfile_LW* m_clustersOnOffTrack_per_lumi;

  // status histograms
  std::unique_ptr<PixelMon2DProfilesLW> m_status;
  std::unique_ptr<PixelMon2DProfilesLW> m_status_mon;
  std::unique_ptr<PixelMon2DProfilesLW> m_status_LB;
  std::unique_ptr<PixelMon2DMapsLW> m_dqStatus;
  TProfile_LW* m_disabledModules_per_lumi_PIX;
  TProfile_LW* m_badModules_per_lumi_mod[PixLayerIBL2D3D::COUNT];
  TProfile_LW* m_disabledModules_per_lumi_mod[PixLayerIBL2D3D::COUNT];
  TProfile_LW* m_baddisabledModules_per_lumi_mod[PixLayerIBL2D3D::COUNT];

  // ROD error histograms
  static const int kNumErrorBits{32};
  static const int kNumErrorBitsIBL{40};
  static const int kNumErrorStates{16};
  static const int kNumErrorStatesIBL{27};
  TProfile_LW* m_errhist_errcat_LB[PixLayerIBL2D3DDBM::COUNT][ErrorCategory::COUNT];
  TProfile_LW* m_errhist_errtype_LB[PixLayerIBL2D3DDBM::COUNT][ErrorCategoryMODROD::COUNT - 3];
  std::unique_ptr<PixelMon2DMapsLW> m_errhist_errcat_map[ErrorCategory::COUNT];
  std::unique_ptr<PixelMon2DMapsLW> m_errhist_errtype_map[ErrorCategoryMODROD::COUNT];
  TProfile_LW* m_errhist_errcat_avg[ErrorCategory::COUNT][PixLayerIBL2D3DDBM::COUNT];
  TProfile_LW* m_errhist_errtype_avg[ErrorCategoryMODROD::COUNT - 3][PixLayerIBL2D3DDBM::COUNT];
  TProfile_LW* m_errhist_tot_LB[PixLayerIBL2D3DDBM::COUNT];
  TProfile2D_LW* m_errhist_per_bit_LB[PixLayerIBL2D3DDBM::COUNT];
  TProfile2D_LW* m_errhist_per_type_LB[PixLayerIBL2D3DDBM::COUNT];
  TProfile_LW* m_errhist_syncerr_LB_pix;
  std::unique_ptr<PixelMonModules1D> m_errors;
  std::unique_ptr<PixelMon2DProfilesLW> m_errhist_femcc_errwords_map;
  std::unique_ptr<PixelMon2DProfilesLW> m_errhist_bitstr_occ_errors;
  std::unique_ptr<PixelMon2DProfilesLW> m_errhist_bitstr_occ_tot;
  TProfile_LW* m_errhist_bitstr_occ_errors_avg[PixLayerIBL2D3D::COUNT];
  TProfile_LW* m_errhist_bitstr_occ_tot_avg[PixLayerIBL2D3D::COUNT];

  // Histograms in 'ErrorsExpert' folder
  std::unique_ptr<PixelMon2DMapsLW> m_errhist_expert_maps[kNumErrorStates + kNumErrorStatesIBL];
  TProfile_LW* m_errhist_expert_LB[PixLayer::COUNT - 1][kNumErrorStates];
  TProfile_LW* m_errhist_expert_DBMIBL_LB[PixLayerDBM::COUNT - PixLayerDBM::kDBMA][kNumErrorStatesIBL];
  TH3F* m_errhist_expert_fe_trunc_err_3d[PixLayer::COUNT];
  TH1F_LW* m_errhist_expert_servrec_ibl_unweighted;
  TH1F_LW* m_errhist_expert_servrec_ibl_weighted;
  TH1F_LW* m_errhist_expert_servrec_ibl_count;

  // Histograms in low-stat folder
  std::unique_ptr<PixelMon2DMapsLW> m_errors_LB;
  std::unique_ptr<PixelMon2DMapsLW> m_errors_RODSync_mod;
  std::unique_ptr<PixelMon2DMapsLW> m_errors_ModSync_mod;

  // space-point histograms
  TH1I_LW* m_num_spacepoints;
  TH1I_LW* m_num_spacepoints_low;
  TH1F_LW* m_spHit_x;
  TH1F_LW* m_spHit_y;
  TH1F_LW* m_spHit_z;
  TH1F_LW* m_spHit_r;
  TH1F_LW* m_spHit_phi;
  TH2F_LW* m_spHit_xy;
  TH2F_LW* m_spHit_rz;

  std::unique_ptr<PixelMon2DMapsLW> m_cluster_occupancy_LB;  // cluster occupancy (shows module status)
  std::unique_ptr<PixelMonModules1D> m_cluster_ToT_mod_LB;
  std::unique_ptr<PixelMonModules1D> m_cluster_num_mod_LB;
  std::unique_ptr<PixelMonModules1D> m_hit_num_mod_LB;

  TH1I_LW* m_num_hits_LB;
  TH1F_LW* m_hit_ToT_LB_mod[PixLayer::COUNT];
  TH1F_LW* m_cluster_ToT_LB;
  TH1I_LW* m_num_clusters_LB;

  // DCS Monitoring
  TProfile2D_LW* m_hist_moduleTemperatureEtaPhi;
  TH2F_LW* m_hist_moduleTemperature2Dscatter[IBLStave::COUNT];
  TProfile_LW* m_hist_moduleTemperatureLB[IBLStave::COUNT];
  TProfile2D_LW* m_hist_LB_staveID_moduleTemperature;
  TProfile2D_LW* m_hist_LB_moduleGroup_moduleTemperature[IBLStave::COUNT];
  TProfile2D_LW* m_hist_HVoltageEtaPhi;
  TH2F_LW* m_hist_HVoltage2Dscatter[IBLStave::COUNT];
  TProfile_LW* m_hist_HVoltageLB[IBLStave::COUNT];
  TProfile2D_LW* m_hist_LB_staveID_HVoltage;
  TProfile2D_LW* m_hist_LB_moduleGroup_HVoltage[IBLStave::COUNT];
  TH2F_LW* m_hist_Pipes_inlet2Dscatter;
  TProfile_LW* m_hist_Pipes_inletLB[IBLStave::COUNT];
  TProfile2D_LW* m_hist_LB_staveID_coolingPipeInlet;
  TH2F_LW* m_hist_Pipes_outlet2Dscatter;
  TProfile_LW* m_hist_Pipes_outletLB[IBLStave::COUNT];
  TProfile2D_LW* m_hist_LB_staveID_coolingPipeOutlet;
  TProfile2D_LW* m_hist_LVoltageEtaPhi;
  TH2F_LW* m_hist_LVoltage2Dscatter[IBLStave::COUNT];
  TProfile_LW* m_hist_LVoltageLB[IBLStave::COUNT];
  TProfile2D_LW* m_hist_LB_staveID_LVoltage;
  TProfile2D_LW* m_hist_LB_moduleGroup_LVoltage[IBLStave::COUNT];
  TProfile2D_LW* m_hist_LVcurrentEtaPhi;
  TH2F_LW* m_hist_LVcurrent2Dscatter[IBLStave::COUNT];
  TProfile_LW* m_hist_LVcurrentLB[IBLStave::COUNT];
  TProfile2D_LW* m_hist_LB_staveID_LVcurrent;
  TProfile2D_LW* m_hist_LB_moduleGroup_LVcurrent[IBLStave::COUNT];
  TProfile2D_LW* m_hist_HVcurrentEtaPhi;
  TH2F_LW* m_hist_HVcurrent2Dscatter[IBLStave::COUNT];
  TProfile_LW* m_hist_HVcurrentLB[IBLStave::COUNT];
  TProfile2D_LW* m_hist_LB_staveID_HVcurrent;
  TProfile2D_LW* m_hist_LB_moduleGroup_HVcurrent[IBLStave::COUNT];
  TH2F* m_hist_FSMstateEtaPhi;
  TH2F_LW* m_hist_FSMstate2Dscatter[IBLStave::COUNT];
  TH2F_LW* m_hist_FSMstateLB[IBLStave::COUNT];
  TH2F* m_hist_LB_staveID_FSMstate;
  TH2F* m_hist_LB_moduleGroup_FSMstate[IBLStave::COUNT];
  TH2F* m_hist_FSMstatusEtaPhi;
  TH2F_LW* m_hist_FSMstatus2Dscatter[IBLStave::COUNT];
  TH2F_LW* m_hist_FSMstatusLB[IBLStave::COUNT];
  TH2F* m_hist_LB_staveID_FSMstatus;
  TH2F* m_hist_LB_moduleGroup_FSMstatus[IBLStave::COUNT];

  // Combination histograms
  TProfile2D_LW* m_hist_dTEtaPhi;
  TProfile2D_LW* m_hist_LB_moduleGroup_dT[IBLStave::COUNT];
  TProfile2D_LW* m_hist_LB_staveID_dT;
  TProfile2D_LW* m_hist_LVPowerConsumptionEtaPhi;
  TProfile2D_LW* m_hist_LB_moduleGroup_LVPowerConsumption[IBLStave::COUNT];
  TProfile2D_LW* m_hist_LB_staveID_LVPowerConsumption;
  TProfile2D_LW* m_hist_HVPowerConsumptionEtaPhi;
  TProfile2D_LW* m_hist_LB_moduleGroup_HVPowerConsumption[IBLStave::COUNT];
  TProfile2D_LW* m_hist_LB_staveID_HVPowerConsumption;
  TProfile2D_LW* m_hist_LVHVPowerConsumptionEtaPhi;
  TProfile2D_LW* m_hist_LB_moduleGroup_LVHVPowerConsumption[IBLStave::COUNT];
  TProfile2D_LW* m_hist_LB_staveID_LVHVPowerConsumption;
  TProfile2D_LW* m_hist_effFLEXtempEtaPhi;
  TProfile2D_LW* m_hist_LB_moduleGroup_effFLEXtemp[IBLStave::COUNT];
  TProfile2D_LW* m_hist_LB_staveID_effFLEXtemp;
  TProfile2D_LW* m_hist_thermalFigureMeritEtaPhi;
  TProfile2D_LW* m_hist_LB_moduleGroup_thermalFigureMerit[IBLStave::COUNT];
  TProfile2D_LW* m_hist_LB_staveID_thermalFigureMerit;

  std::vector<std::string> m_atrcollist;
  int m_currentLumiBlockNumber;

  class dcsDataHolder {
   public:
    dcsDataHolder() : m_values(new std::map<int, std::map<int, float>*>),
                      m_maps(new std::map<std::string, int>){};
    dcsDataHolder (const dcsDataHolder&) = delete;
    dcsDataHolder& operator = (const dcsDataHolder&) = delete;
    ~dcsDataHolder() {
      for (auto lbmap : *m_values) {
        delete lbmap.second;
      }
      delete m_values;
      delete m_maps;
    };
    std::map<int, std::map<int, float>*>* m_values;  // <channel number, <LB, value> >
    std::map<std::string, int>* m_maps;              // <module name, channel number>
  };
  dcsDataHolder* m_moduleTemperature;
  dcsDataHolder* m_coolingPipeTemperatureInlet;
  dcsDataHolder* m_coolingPipeTemperatureOutlet;
  dcsDataHolder* m_HV;
  dcsDataHolder* m_HV_current;
  dcsDataHolder* m_LV_voltage;
  dcsDataHolder* m_LV_current;
  dcsDataHolder* m_FSM_state;
  dcsDataHolder* m_FSM_status;

  class moduleDcsDataHolder {
   public:
    moduleDcsDataHolder() : m_tempModule(new std::map<int, std::map<int, float>*>),
                            m_tempInlet(new std::map<int, std::map<int, float>*>),
                            m_tempOutlet(new std::map<int, std::map<int, float>*>),
                            m_hv_voltage(new std::map<int, std::map<int, float>*>),
                            m_lv_voltage(new std::map<int, std::map<int, float>*>),
                            m_hv_current(new std::map<int, std::map<int, float>*>),
                            m_lv_current(new std::map<int, std::map<int, float>*>),
                            m_fsm_state(new std::map<int, std::map<int, float>*>),
                            m_fsm_status(new std::map<int, std::map<int, float>*>),
                            m_moduleMap(new std::map<std::string, int>){};
    moduleDcsDataHolder (const moduleDcsDataHolder&) = delete;
    moduleDcsDataHolder& operator = (const moduleDcsDataHolder&) = delete;
    ~moduleDcsDataHolder() {
      for (auto lbmap : *m_tempModule) {
        delete lbmap.second;
      }
      for (auto lbmap : *m_tempInlet) {
        delete lbmap.second;
      }
      for (auto lbmap : *m_tempOutlet) {
        delete lbmap.second;
      }
      for (auto lbmap : *m_hv_voltage) {
        delete lbmap.second;
      }
      for (auto lbmap : *m_lv_voltage) {
        delete lbmap.second;
      }
      for (auto lbmap : *m_hv_current) {
        delete lbmap.second;
      }
      for (auto lbmap : *m_lv_current) {
        delete lbmap.second;
      }
      for (auto lbmap : *m_fsm_state) {
        delete lbmap.second;
      }
      for (auto lbmap : *m_fsm_status) {
        delete lbmap.second;
      }
      delete m_tempModule;
      delete m_tempInlet;
      delete m_tempOutlet;
      delete m_hv_voltage;
      delete m_lv_voltage;
      delete m_hv_current;
      delete m_lv_current;
      delete m_fsm_state;
      delete m_fsm_status;
      delete m_moduleMap;
    };
    // <module number, <LB, value> >
    std::map<int, std::map<int, float>*>* m_tempModule;
    std::map<int, std::map<int, float>*>* m_tempInlet;
    std::map<int, std::map<int, float>*>* m_tempOutlet;
    std::map<int, std::map<int, float>*>* m_hv_voltage;
    std::map<int, std::map<int, float>*>* m_lv_voltage;
    std::map<int, std::map<int, float>*>* m_hv_current;
    std::map<int, std::map<int, float>*>* m_lv_current;
    std::map<int, std::map<int, float>*>* m_fsm_state;
    std::map<int, std::map<int, float>*>* m_fsm_status;
    // <module name, channel number>
    std::map<std::string, int>* m_moduleMap;
  };
  moduleDcsDataHolder* m_moduleDCSDataHolder;

 public:
  int getIBLetaIndexFromModuleName(std::string moduleName) {
    int module_eta = m_name2etaIndex[moduleName.substr(static_cast<int>(moduleName.find("S")) + 4)];
    return module_eta;
  }

  int getIBLmoduleGroupFromModuleName(std::string moduleName) {
    int moduleGroup = m_name2moduleGroup[moduleName.substr(static_cast<int>(moduleName.find("S")) + 4, 4)];  // LI_S01_A_M4_A8_2
    return moduleGroup;
  }

  int getIBLstaveIdFromModuleName(std::string moduleName) {
    std::string staveNumStr(moduleName, moduleName.find("S") + 1, 2);  // LI_S01_A_...
    int staveNum = std::stoi(staveNumStr);
    return staveNum;
  }

 private:
  std::map<std::string, int> m_name2etaIndex;
  std::map<std::string, int> m_name2moduleGroup;

  std::map<std::string, float> m_fsmState2enum;
  std::map<std::string, float> m_fsmStatus2enum;
};  // end class PixelMainMon : public ManagedMonitorToolBase

#endif
