/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IDAlignMonResiduals_H
#define IDAlignMonResiduals_H

// **********************************************************************
// IDAlignMonResiduals.cxx
// AUTHORS: Beate Heinemann, Tobias Golling, Ben Cooper, John Alison, Pierfrancesco Butti
// **********************************************************************

#include <vector>

#include "GaudiKernel/StatusCode.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "EventPrimitives/EventPrimitives.h"
#include "TrkParameters/TrackParameters.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "CommissionEvent/ComTime.h"
#include "TFile.h"



class TH1F;
class TH1F_LW;
class TH2F;
class TH3F;
class TProfile;
class TProfile2D;
class TGraphErrors;
class ITRT_CalDbSvc;

class AtlasDetectorID;
class PixelID;
class SCT_ID;
class TRT_ID;

namespace InDetAlignMon{
  class TrackSelectionTool;
}

namespace InDetDD{
  class PixelDetectorManager;
  class SCT_DetectorManager;
}

namespace Trk {
  class IUpdator;
  class IPropagator;
  class RIO_OnTrack;
  class Track;
  class TrackStateOnSurface;
  class IResidualPullCalculator;
}

class IInDetAlignHitQualSelTool;
//namespace InDet {
//  class IInDetTrackSelectionTool;
//}


class IDAlignMonResiduals : public ManagedMonitorToolBase
{

 public:

  IDAlignMonResiduals( const std::string & type, const std::string & name, const IInterface* parent );

  virtual ~IDAlignMonResiduals();

  virtual StatusCode initialize();
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

  void MakePIXBarrelHistograms (MonGroup& al_mon);
  void MakePIXEndCapsHistograms (MonGroup& al_mon);
  void MakeSCTEndcapsHistograms (MonGroup& al_mon);
  void MakeSCTBarrelHistograms (MonGroup& al_mon);

  void MakeStaveShapeFit(float& mag, float& mag_er,float& base,float& base_er,TH1F* projection);
  TGraphErrors* ConvertHistoInGraph(TH1F*);

  void MakeTRTHistograms(MonGroup& al_mon);
  void MakeTRTBarrelHistograms(MonGroup& al_mon);
  void MakeTRTEndcapHistograms(MonGroup& al_mon);
  void MakeSiliconHistograms(MonGroup&);
  void fillTRTHistograms(int m_barrel_ec, int m_layer_or_wheel, int m_phi_module, int m_straw_layer,float perdictR, float hitR, float hitZ, float hitGlobalR, float residualR, float pullR, float LE, float EP, float t0, bool isTubeHit ,float trketa, float trkpt, double hweight);
  void fillTRTBarrelHistograms(int m_barrel_ec, int m_layer_or_wheel, int m_phi_module, int m_straw_layer,float perdictR, float hitR, float hitZ, float residualR, float pullR, bool LRcorrect, float LE, float EP, float t0, bool isTubeHit ,float trketa, float trkpt, double hweight);
  void fillTRTEndcapHistograms(int m_barrel_ec, int m_layer_or_wheel, int m_phi_module, int m_straw_layer,float perdictR, float hitR, float hitGlobalR, float residualR, float pullR, bool LRcorrect, float LE, float EP, float t0, bool isTubeHit ,float trketa, float trkpt, double hweight);
  unsigned int getRing(unsigned int wheel,unsigned int strawlayer);

  /** Convert from an int to a string */
  std::string intToString(int input);

  /** Make Histograms */
  TH1F_LW* MakeHist(const std::string& name, const std::string& title, int nBins, float xLow, float xUp, const std::string& xAxisTitle, const std::string& yAxisTitle);
  TH2F* MakeHist( const std::string& name, const std::string& title, int nXBins, float xLow, float xUp, int nYBins, float yLow, float yUp, const std::string& xAxisTitle, const std::string& yAxisTitle);


  /** Make Profiles */
  TProfile* MakeProfile(const std::string& name, const std::string& title, int nBins, float xLow, float xUp, float yLow, float yUp, const std::string& xAxisTitle, const std::string& yAxisTitle, bool setMinZero=true);

  void RegisterHisto(MonGroup& mon, TH1* histo);
  void RegisterHisto(MonGroup& mon, TH1F_LW* histo);
  void RegisterHisto(MonGroup& mon, TH2* histo);
  void RegisterHisto(MonGroup& mon, TProfile* histo);
  void RegisterHisto(MonGroup& mon, TProfile2D* histo);
  void RegisterHisto(MonGroup& mon, TH3* histo);

  void InitializeHistograms();


 protected:
  const ComTime *theComTime;
  std::string m_comTimeObjectName;
  ServiceHandle<ITRT_CalDbSvc> m_trtcaldbSvc ;
 private:


  StatusCode setupTools();

  bool isEdge(const Trk::RIO_OnTrack*);

  std::pair<const Trk::TrackStateOnSurface*, const Trk::TrackStateOnSurface*> findOverlapHit(const Trk::Track*, const Trk::RIO_OnTrack*);
  StatusCode getSiResiduals(const Trk::Track*, const Trk::TrackStateOnSurface*, bool, double*);
  const Trk::TrackParameters* getUnbiasedTrackParameters(const Trk::Track*, const Trk::TrackStateOnSurface*);
  void meanRMSProjections(TH2F*, TH1F*, int);
  void meanRMSProjection2D(TH3F*, TH2F*, int, bool);
  void fillRMSFromProfile(TProfile*, TProfile*);
  void fillGaussianMeanOrWidth(TH2F*, TH1F*, float, float, int);
  bool trackRequiresRefit(const Trk::Track*);

  //================================================================
  // Establishes a minimim window for the histograms
  //================================================================
  void SetMinWindow(TProfile* hProf, float min, float max);
  void SetMinWindow(TH1* h1, float min, float max);

  std::string m_stream;
  std::string m_tracksName;
  std::string m_triggerChainName;
  std::string m_Pixel_Manager;
  std::string m_SCT_Manager;
  std::string m_TRT_Manager;

  //Layer/Disk/Ring Gap for modified module histograms
  int m_gap_pix;
  int m_gap_sct;
  int m_NLumiBlocksMon;
  int m_maxModulesInASCTEndCapRing;
  int m_maxRingsInASCTEndCapDisk;

  float m_minTRTResWindow;
  float m_maxTRTResWindow;
  float m_minSiResMeanWindow;
  float m_maxSiResMeanWindow;
  float m_minSiResWidthWindow;
  float m_maxSiResWidthWindow;
  float m_minSiPullWidthWindow;
  float m_maxSiPullWidthWindow;
  float m_minSiResFillRange;
  float m_maxSiResFillRange;
  float m_maxSCTResFillRange;
  float m_minSCTResFillRange;
  float m_maxPIXResXFillRange;
  float m_minPIXResXFillRange;
  float m_maxPIXResYFillRange;
  float m_minPIXResYFillRange;
  float m_RangeOfPullHistos;
  float m_PtRange;
  float m_ClusterSizeRange;
  float PixelBarrelXSize;
  float PixelBarrelYSize;
  float SCTBarrelXSize;
  float SCTBarrelYSize;
  float m_IncidentThetaRange;
  float m_IncidentPhiRange;
  int m_nBinsPtRange;
  int m_histosBooked;
  int m_checkrate;
  int m_events;
  int m_mapSplit;
  int m_NPixLayers;
  bool m_doPulls;
  bool m_doHitQuality;
  bool m_do3DOverlapHistos;
  bool m_doClusterSizeHistos;
  bool m_extendedPlots;
  bool m_changedlumiblock;
  int  m_oldlumiblock;
  int m_FinerBinningFactor;
  float m_LBGranularity;
  std::vector<TString> m_siliconBarrelLayersLabels;
  std::vector<TString> m_siliconEndcapLayersLabels;
  bool m_useGausFit;
  float m_maxPtEC; // threshold for low-pt EC distributions

  //tools
  const AtlasDetectorID*                m_idHelper;
  const InDetDD::PixelDetectorManager*  m_PIX_Mgr;
  const InDetDD::SCT_DetectorManager*   m_SCT_Mgr;
  const PixelID*                        m_pixelID;
  const SCT_ID*                         m_sctID;
  const TRT_ID*                         m_trtID;
  ToolHandle<Trk::IUpdator>             m_iUpdator;
  ToolHandle<Trk::IPropagator>          m_propagator;

  ToolHandle<InDetAlignMon::TrackSelectionTool>  m_trackSelection;
  ToolHandle<Trk::IResidualPullCalculator>    m_residualPullCalculator;   //!< The residual and pull calculator tool handle
  ToolHandle<IInDetAlignHitQualSelTool>  m_hitQualityTool;
  //ToolHandle<InDet::IInDetTrackSelectionTool>   m_idtrackSelection;

  //histograms

  TH1F* m_totalEvents;
  TH1F* m_sirescalcfailure;
  std::vector<TH3F*> m_sct_b_Oxresxvsmodetaphi_3ds;
  std::vector<TH3F*> m_sct_b_Oyresxvsmodetaphi_3ds;
  std::vector<TH3F*> m_sct_b_xresvsmodetaphi_3ds;
  std::vector<TH3F*> m_sct_b_s0_xresvsmodetaphi_3ds;
  std::vector<TH3F*> m_sct_b_s1_xresvsmodetaphi_3ds;
  std::vector<TH3F*> m_sct_eca_s0_xresvsmodetaphi_3ds;
  std::vector<TH3F*> m_sct_eca_s1_xresvsmodetaphi_3ds;
  std::vector<TH3F*> m_sct_ecc_s0_xresvsmodetaphi_3ds;
  std::vector<TH3F*> m_sct_ecc_s1_xresvsmodetaphi_3ds;
  std::vector<TH1F*> m_sct_b_residualsx;
  //TH1F* m_sct_b0_residualy;
  std::vector<TH1F*> m_sct_b_pullsx;
  std::vector<TH1F*> m_sct_b_xoverlapresidualsx;
  std::vector<TH1F*> m_sct_b_yoverlapresidualsx;
  std::vector<TProfile*> m_sct_eca_xresvsmodphi_disks;
  std::vector<TProfile*> m_sct_ecc_xresvsmodphi_disks;

  std::vector<TProfile*> m_sct_eca_biased_xresvsmodphi_disks;
  std::vector<TProfile*> m_sct_ecc_biased_xresvsmodphi_disks;
  std::vector<TH3F*> m_sct_b_s0_biased_xresvsmodetaphi_3ds;
  std::vector<TH3F*> m_sct_b_s1_biased_xresvsmodetaphi_3ds;

  std::vector<TH3F*> m_sct_eca_s0_biased_xresvsmodetaphi_3ds;
  std::vector<TH3F*> m_sct_ecc_s0_biased_xresvsmodetaphi_3ds;
  std::vector<TH3F*> m_sct_eca_s1_biased_xresvsmodetaphi_3ds;
  std::vector<TH3F*> m_sct_ecc_s1_biased_xresvsmodetaphi_3ds;

  std::vector<TH3F*> m_sct_b_clustersizePhivsmodetaphi_3ds_s0;
  std::vector<TH3F*> m_sct_b_clustersizePhivsmodetaphi_3ds_s1;

  std::vector<TH3F*> m_sct_eca_xresvsmodetaphi_3ds;
  std::vector<TH3F*> m_sct_ecc_xresvsmodetaphi_3ds;

  std::vector<TH1F*> m_sct_b_biased_residualsx;
  std::vector<TH2F*> m_sct_b_biased_residualsx_pt;
  std::vector<TH2F*> m_sct_b_residualsx_pt;
  std::vector<TH2F*> m_sct_b_biased_residualsx_qoverp2;
  std::vector<TH2F*> m_sct_b_residualsx_qoverp2;
  std::vector<TH1F*> m_sct_b_biased_pullsx;
  std::vector<TH1F*> m_sct_eca_biased_residualsx;
  std::vector<TH1F*> m_sct_ecc_biased_residualsx;
  std::vector<TH2F*> m_sct_eca_biased_residualsx_pt;
  std::vector<TH2F*> m_sct_ecc_biased_residualsx_pt;
  std::vector<TH2F*> m_pix_b_pullsx_pt;
  std::vector<TH2F*> m_pix_b_pullsy_pt;
  std::vector<TH2F*> m_pix_b_residualsx_incitheta;
  std::vector<TH2F*> m_pix_b_residualsx_inciphi;
  std::vector<TH2F*> m_pix_b_residualsy_incitheta;
  std::vector<TH2F*> m_pix_b_residualsy_inciphi;

  //Pix Eca unbiased residuals maps

  TH3F* m_pix_eca_xresvsmodphidisk_3d;
  TH3F* m_pix_ecc_xresvsmodphidisk_3d;
  TH3F* m_pix_eca_yresvsmodphidisk_3d;
  TH3F* m_pix_ecc_yresvsmodphidisk_3d;


  //PixCluster Size
  std::vector<TH1F*> m_pix_b_clustersize;
  std::vector<TH1F*> m_pix_eca_clustersize;
  std::vector<TH1F*> m_pix_ecc_clustersize;

  std::vector<TH1F*> m_pix_b_clustersizePhi;
  std::vector<TH1F*> m_pix_b_clustersizeZ;
  std::vector<TH1F*> m_pix_eca_clustersizePhi;
  std::vector<TH1F*> m_pix_eca_clustersizeZ;
  std::vector<TH1F*> m_pix_ecc_clustersizePhi;
  std::vector<TH1F*> m_pix_ecc_clustersizeZ;

  std::vector<TH2F*> m_pix_b_residualsx_clustersize;
  std::vector<TH2F*> m_pix_b_residualsy_clustersize;
  std::vector<TH2F*> m_pix_b_residualsx_clustersizePhi;
  std::vector<TH2F*> m_pix_b_residualsy_clustersizePhi;
  std::vector<TH2F*> m_pix_b_residualsx_clustersizeZ;
  std::vector<TH2F*> m_pix_b_residualsy_clustersizeZ;

  std::vector<TProfile*> m_pix_b_residualsx_clustersizeP;
  std::vector<TProfile*> m_pix_b_residualsy_clustersizeP;
  std::vector<TProfile*> m_pix_b_residualsx_clustersizePhiP;
  std::vector<TProfile*> m_pix_b_residualsy_clustersizePhiP;
  std::vector<TProfile*> m_pix_b_residualsx_clustersizeZP;
  std::vector<TProfile*> m_pix_b_residualsy_clustersizeZP;

  std::vector<TH2F*> m_pix_eca_residualsx_clustersize;
  std::vector<TH2F*> m_pix_eca_residualsy_clustersize;
  std::vector<TH2F*> m_pix_eca_residualsx_clustersizePhi;
  std::vector<TH2F*> m_pix_eca_residualsy_clustersizePhi;
  std::vector<TH2F*> m_pix_eca_residualsx_clustersizeZ;
  std::vector<TH2F*> m_pix_eca_residualsy_clustersizeZ;

  std::vector<TH2F*> m_pix_ecc_residualsx_clustersize;
  std::vector<TH2F*> m_pix_ecc_residualsy_clustersize;
  std::vector<TH2F*> m_pix_ecc_residualsx_clustersizePhi;
  std::vector<TH2F*> m_pix_ecc_residualsy_clustersizePhi;
  std::vector<TH2F*> m_pix_ecc_residualsx_clustersizeZ;
  std::vector<TH2F*> m_pix_ecc_residualsy_clustersizeZ;

  std::vector<TProfile*> m_pix_eca_residualsx_clustersizeP;
  std::vector<TProfile*> m_pix_eca_residualsy_clustersizeP;
  std::vector<TProfile*> m_pix_eca_residualsx_clustersizePhiP;
  std::vector<TProfile*> m_pix_eca_residualsy_clustersizePhiP;
  std::vector<TProfile*> m_pix_eca_residualsx_clustersizeZP;
  std::vector<TProfile*> m_pix_eca_residualsy_clustersizeZP;

  std::vector<TProfile*> m_pix_ecc_residualsx_clustersizeP;
  std::vector<TProfile*> m_pix_ecc_residualsy_clustersizeP;
  std::vector<TProfile*> m_pix_ecc_residualsx_clustersizePhiP;
  std::vector<TProfile*> m_pix_ecc_residualsy_clustersizePhiP;
  std::vector<TProfile*> m_pix_ecc_residualsx_clustersizeZP;
  std::vector<TProfile*> m_pix_ecc_residualsy_clustersizeZP;


  //DBM plots

  std::vector<TH1F*> m_dbm_residualsx;
  std::vector<TH1F*> m_dbm_pullsx;
  std::vector<TH2F*> m_dbm_residualsx_incitheta;
  std::vector<TH2F*> m_dbm_residualsx_inciphi;
  std::vector<TH2F*> m_dbm_residualsx_pt;
  std::vector<TH2F*> m_dbm_residualsx_qoverp2;
  std::vector<TH1F*> m_dbm_biased_residualsx;
  std::vector<TH2F*> m_dbm_biased_residualsx_pt;
  std::vector<TH2F*> m_dbm_biased_residualsx_qoverp2;
  std::vector<TH2F*> m_dbm_pullsx_pt;
  std::vector<TH1F*> m_dbm_xoverlapresidualsx;

  TH2F* m_dbm_xresvsmodphi_2d;

  //SctCluster Size

  std::vector<TH1F*> m_sct_b_clustersizePhi;
  std::vector<TH1F*> m_sct_eca_clustersizePhi;
  std::vector<TH1F*> m_sct_ecc_clustersizePhi;

  std::vector<TH2F*> m_sct_b_residualsx_clustersizePhi;
  std::vector<TH2F*> m_sct_eca_residualsx_clustersizePhi;
  std::vector<TH2F*> m_sct_ecc_residualsx_clustersizePhi;



  std::vector<TProfile*> m_sct_b_residualsx_clustersizePhiP;
  std::vector<TProfile*> m_sct_eca_residualsx_clustersizePhiP;
  std::vector<TProfile*> m_sct_ecc_residualsx_clustersizePhiP;






  //Cluster Size vs Incident Angle

  std::vector<TProfile*> m_pix_b_clustersizePhi_incidentAngle;
  std::vector<TProfile*> m_pix_b_clustersizeZ_incidentAngle;
  std::vector<TProfile*> m_pix_b_clustersize_incidentAngle;

  std::vector<TProfile*> m_pix_eca_clustersizePhi_incidentAngle;
  std::vector<TProfile*> m_pix_eca_clustersizeZ_incidentAngle;
  std::vector<TProfile*> m_pix_eca_clustersize_incidentAngle;

  std::vector<TProfile*> m_pix_ecc_clustersizePhi_incidentAngle;
  std::vector<TProfile*> m_pix_ecc_clustersizeZ_incidentAngle;
  std::vector<TProfile*> m_pix_ecc_clustersize_incidentAngle;

  std::vector<TProfile*> m_sct_b_clustersizePhi_incidentAngle;
  std::vector<TProfile*> m_sct_eca_clustersizePhi_incidentAngle;
  std::vector<TProfile*> m_sct_ecc_clustersizePhi_incidentAngle;

  std::vector<TH2F*> m_sct_b_residualsx_incidentAnglePhi_s0;
  std::vector<TH2F*> m_sct_eca_residualsx_incidentAnglePhi_s0;
  std::vector<TH2F*> m_sct_ecc_residualsx_incidentAnglePhi_s0;

  std::vector<TH2F*> m_sct_b_residualsx_incidentAnglePhi_s1;
  std::vector<TH2F*> m_sct_eca_residualsx_incidentAnglePhi_s1;
  std::vector<TH2F*> m_sct_ecc_residualsx_incidentAnglePhi_s1;

  std::vector<TH2F*> m_sct_b_residualsx_incidentAngle_s0;
  std::vector<TH2F*> m_sct_eca_residualsx_incidentAngle_s0;
  std::vector<TH2F*> m_sct_ecc_residualsx_incidentAngle_s0;

  std::vector<TH2F*> m_sct_b_residualsx_incidentAngle_s1;
  std::vector<TH2F*> m_sct_eca_residualsx_incidentAngle_s1;
  std::vector<TH2F*> m_sct_ecc_residualsx_incidentAngle_s1;

  std::vector<TProfile*> m_pix_b_clustersizePhi_incidentAnglePhi;
  std::vector<TProfile*> m_pix_b_clustersizeZ_incidentAnglePhi;
  std::vector<TProfile*> m_pix_b_clustersize_incidentAnglePhi;

  std::vector<TProfile*> m_pix_eca_clustersizePhi_incidentAnglePhi;
  std::vector<TProfile*> m_pix_eca_clustersizeZ_incidentAnglePhi;
  std::vector<TProfile*> m_pix_eca_clustersize_incidentAnglePhi;

  std::vector<TProfile*> m_pix_ecc_clustersizePhi_incidentAnglePhi;
  std::vector<TProfile*> m_pix_ecc_clustersizeZ_incidentAnglePhi;
  std::vector<TProfile*> m_pix_ecc_clustersize_incidentAnglePhi;

  std::vector<TProfile*> m_sct_b_clustersizePhi_incidentAnglePhi;
  std::vector<TProfile*> m_sct_eca_clustersizePhi_incidentAnglePhi;
  std::vector<TProfile*> m_sct_ecc_clustersizePhi_incidentAnglePhi;




  TH1F* m_mu_perEvent;
  float  m_mu;
  bool m_hasBeenCalledThisEvent;
  int m_nBinsMuRange;
  float m_muRangeMin;
  float m_muRangeMax;
  int m_nBinsLB;
  float m_LBRangeMin;
  float m_LBRangeMax;
  int nIBLHitsPerLB;


  int lumiblock;

  TProfile2D*   m_pix_b0_resXvsetaLumiBlock;
  TProfile2D*   m_pix_b0_resXvsetaLumiBlock_planars;

  TH3F*   m_pix_b0_resXvsetaLumiBlock_3d;
  TH3F*   m_pix_b0_resXvsetaLumiBlock_planars_3d;
  TH3F*   m_pix_b0_resXvsetaLumiBlock_stave0;
  TH3F*   m_pix_b0_resXvsetaLumiBlock_stave1;
  TH3F*   m_pix_b0_resXvsetaLumiBlock_stave2;
  TH3F*   m_pix_b0_resXvsetaLumiBlock_stave3;
  TH3F*   m_pix_b0_resXvsetaLumiBlock_stave4;
  TH3F*   m_pix_b0_resXvsetaLumiBlock_stave5;
  TH3F*   m_pix_b0_resXvsetaLumiBlock_stave6;
  TH3F*   m_pix_b0_resXvsetaLumiBlock_stave7;
  TH3F*   m_pix_b0_resXvsetaLumiBlock_stave8;
  TH3F*   m_pix_b0_resXvsetaLumiBlock_stave9;
  TH3F*   m_pix_b0_resXvsetaLumiBlock_stave10;
  TH3F*   m_pix_b0_resXvsetaLumiBlock_stave11;
  TH3F*   m_pix_b0_resXvsetaLumiBlock_stave12;
  TH3F*   m_pix_b0_resXvsetaLumiBlock_stave13;

  //IBL fit magnitude and baseline as a function of LumiBlock

  TH1F* m_mag_vs_LB;
  TH1F* m_base_vs_LB;

  TH1F* m_mag_vs_LB_planars;
  TH1F* m_base_vs_LB_planars;




  // SCT
  TH2F* m_sct_b_pullx_pt;
  std::vector<TH2F*> m_sct_b_pullsx_pt;

  TH2F* m_sct_eca_pullx_pt;
  TH2F* m_sct_ecc_pullx_pt;


  std::vector<TH2F*> m_sct_eca_pullsx_pt;
  std::vector<TH2F*> m_sct_ecc_pullsx_pt;


  std::vector<TH2F*> m_sct_eca_residualsx_pt;
  std::vector<TH2F*> m_sct_ecc_residualsx_pt;

  std::vector<TH2F*> m_sct_eca_biased_residualsx_qoverp2;
  std::vector<TH2F*> m_sct_ecc_biased_residualsx_qoverp2;

  std::vector<TH2F*> m_sct_eca_residualsx_qoverp2;
  std::vector<TH2F*> m_sct_ecc_residualsx_qoverp2;

  TH3F* m_sct_b0_xresvsmodetaphi_3d;
  TH3F* m_sct_b1_xresvsmodetaphi_3d;
  TH3F* m_sct_b2_xresvsmodetaphi_3d;
  TH3F* m_sct_b3_xresvsmodetaphi_3d;

  TH1F* m_si_residualx;
  TH1F* m_si_b_residualx;

  TH1F* m_si_barrel_resX_mean;
  TH1F* m_si_eca_resX_mean;
  TH1F* m_si_ecc_resX_mean;
  TH1F* m_si_barrel_resY_mean;
  TH1F* m_si_eca_resY_mean;
  TH1F* m_si_ecc_resY_mean;

  TProfile* m_si_barrel_resX_mean_profile;
  TProfile* m_si_barrel_resY_mean_profile;


  TH1F* m_si_barrel_resX_rms;
  TH1F* m_si_eca_resX_rms;
  TH1F* m_si_ecc_resX_rms;
  TH1F* m_si_barrel_resY_rms;
  TH1F* m_si_eca_resY_rms;
  TH1F* m_si_ecc_resY_rms;

  TH2F* m_si_barrel_pullX;
  TH2F* m_si_eca_pullX;
  TH2F* m_si_ecc_pullX;
  TH2F* m_si_barrel_pullY;
  TH2F* m_si_eca_pullY;
  TH2F* m_si_ecc_pullY;

  TH2F* m_si_barrel_resX;
  TH2F* m_si_eca_resX;
  TH2F* m_si_ecc_resX;
  TH2F* m_si_barrel_resY;
  TH2F* m_si_eca_resY;
  TH2F* m_si_ecc_resY;

  TH1F* m_si_barrel_pullX_width;
  TH1F* m_si_eca_pullX_width;
  TH1F* m_si_ecc_pullX_width;
  TH1F* m_si_barrel_pullY_width;
  TH1F* m_si_eca_pullY_width;
  TH1F* m_si_ecc_pullY_width;

  TH1F* m_si_barrel_pullX_mean;
  TH1F* m_si_eca_pullX_mean;
  TH1F* m_si_ecc_pullX_mean;
  TH1F* m_si_barrel_pullY_mean;
  TH1F* m_si_eca_pullY_mean;
  TH1F* m_si_ecc_pullY_mean;


  //Pixel Histograms
  std::vector<TH1F*> m_pix_b_residualsx;
  std::vector<TH1F*> m_pix_b_residualsy;
  std::vector<TH1F*> m_pix_b_pullsx;
  std::vector<TH1F*> m_pix_b_pullsy;
  std::vector<TH3F*> m_pix_b_xresvsmodetaphi_3ds;
  std::vector<TH3F*> m_pix_b_yresvsmodetaphi_3ds;
  // -replaced by detailed std::vector<TH3F*> m_pix_b_biased_xresvsmodetaphi_3ds;
  // std::vector<TH3F*> m_pix_b_biased_yresvsmodetaphi_3ds;
  std::vector<TH3F*> m_pix_b_detailed_xresvsmodetaphi_3ds;
  std::vector<TH3F*> m_pix_b_detailed_yresvsmodetaphi_3ds;
  std::vector<TH1F*> m_pix_b_top_residualsx;
  std::vector<TH1F*> m_pix_b_top_residualsy;
  std::vector<TH1F*> m_pix_b_btm_residualsx;
  std::vector<TH1F*> m_pix_b_btm_residualsy;

  std::vector<TH3F*> m_pix_b_Oxresxvsmodetaphi_3ds;
  std::vector<TH3F*> m_pix_b_Oxresyvsmodetaphi_3ds;
  std::vector<TH3F*> m_pix_b_Oyresxvsmodetaphi_3ds;
  std::vector<TH3F*> m_pix_b_Oyresyvsmodetaphi_3ds;

  std::vector<TH1F*> m_pix_b_xoverlapresidualsx;
  std::vector<TH1F*> m_pix_b_xoverlapresidualsy;
  std::vector<TH1F*> m_pix_b_yoverlapresidualsx;
  std::vector<TH1F*> m_pix_b_yoverlapresidualsy;
  std::vector<TH1F*> m_pix_b_biased_residualsx;
  std::vector<TH1F*> m_pix_b_biased_residualsy;
  // Pixel biased residuals vs pt
  std::vector<TH2F*> m_pix_b_biased_residualsx_pt;
  std::vector<TH2F*> m_pix_b_biased_residualsy_pt;
  std::vector< TH2F*> m_pix_b_residualsx_pt;
  std::vector<TH2F*> m_pix_b_residualsy_pt;
  std::vector<TH2F*> m_pix_b_residualsx_qoverp2;
  std::vector<TH2F*> m_pix_b_residualsy_qoverp2;
  std::vector<TH2F*> m_pix_b_biased_residualsx_qoverp2;
  std::vector<TH2F*> m_pix_b_biased_residualsy_qoverp2;
  // Pixel halfshell residuals
  std::vector<TH1F*> m_pix_b_top_biased_residualsx;
  std::vector<TH1F*> m_pix_b_top_biased_residualsy;
  std::vector<TH1F*> m_pix_b_btm_biased_residualsx;
  std::vector<TH1F*> m_pix_b_btm_biased_residualsy;




  //Pixel Histograms
  TH1F* m_pix_b_residualx;
  TH1F* m_pix_b_residualy;
  TH1F* m_pix_b_residualx_fine;
  TH1F* m_pix_b_residualy_fine;
  TH1F* m_pix_b_biased_residualx;
  TH1F* m_pix_b_biased_residualy;
  TH1F* m_pix_eca_residualx;
  TH1F* m_pix_eca_residualy;
  TH1F* m_pix_ecc_residualx;
  TH1F* m_pix_ecc_residualy;
  TH1F* m_pix_eca_residualx_fine;
  TH1F* m_pix_eca_residualy_fine;
  TH1F* m_pix_ecc_residualx_fine;
  TH1F* m_pix_ecc_residualy_fine;

  TH1F* m_pix_eca_pullx;
  TH1F* m_pix_eca_pully;
  TH1F* m_pix_ecc_pullx;
  TH1F* m_pix_ecc_pully;






  TProfile* m_pix_bec_Oxresx_mean;
  TProfile* m_pix_bec_Oyresx_mean;
  TProfile* m_pix_bec_Oxresy_mean;
  TProfile* m_pix_bec_Oyresy_mean;
  TProfile* m_pix_bec_Oxresx_rms;
  TProfile* m_pix_bec_Oyresx_rms;
  TProfile* m_pix_bec_Oxresy_rms;
  TProfile* m_pix_bec_Oyresy_rms;

  TH1F* m_pix_b_xresvsmodeta;
  TH1F* m_pix_b_xresvsmodphi;
  TH1F* m_pix_b_yresvsmodeta;
  TH1F* m_pix_b_yresvsmodphi;
  TH1F* m_pix_eca_xresvsmodphi;
  TH1F* m_pix_ecc_xresvsmodphi;
  TH1F* m_pix_eca_yresvsmodphi;
  TH1F* m_pix_ecc_yresvsmodphi;





  TH2F* m_pix_b_biased_residualx_pt;
  TH2F* m_pix_b_biased_residualy_pt ;
  TH1F* m_pix_eca_biased_residualx ;
  TH1F* m_pix_eca_biased_residualy ;
  TH2F* m_pix_eca_biased_residualx_pt;
  TH2F* m_pix_eca_biased_residualy_pt;
  TH1F* m_pix_ecc_biased_residualx ;
  TH1F* m_pix_ecc_biased_residualy ;
  TH2F* m_pix_ecc_biased_residualx_pt;
  TH2F* m_pix_ecc_biased_residualy_pt;















  TH2F* m_pix_eca_xresvsmodphi_2d;
  TH2F* m_pix_ecc_xresvsmodphi_2d;
  TH2F* m_pix_eca_yresvsmodphi_2d;
  TH2F* m_pix_ecc_yresvsmodphi_2d;

  TProfile* m_pix_b_Oxresxvsmodeta;
  TProfile* m_pix_b_Oxresxvsmodphi;
  TProfile* m_pix_b_Oyresyvsmodeta;
  TProfile* m_pix_b_Oyresyvsmodphi;
  TProfile* m_pix_eca_Oxresxvsmodphi;
  TProfile* m_pix_ecc_Oxresxvsmodphi;
  TProfile* m_pix_eca_Oyresyvsmodphi;
  TProfile* m_pix_ecc_Oyresyvsmodphi;

  std::vector<TProfile*> m_pix_eca_unbiased_xresvsmodphi_disks;
  std::vector<TProfile*> m_pix_eca_unbiased_yresvsmodphi_disks;
  std::vector<TProfile*> m_pix_ecc_unbiased_xresvsmodphi_disks;
  std::vector<TProfile*> m_pix_ecc_unbiased_yresvsmodphi_disks;
  TProfile* m_pix_eca_unbiased_xresvsmodphi;
  TProfile* m_pix_eca_unbiased_yresvsmodphi;
  TProfile* m_pix_ecc_unbiased_xresvsmodphi;
  TProfile* m_pix_ecc_unbiased_yresvsmodphi;

  std::vector<TH1F*> m_pix_b_biasedresidualsx;
  std::vector<TH1F*> m_pix_b_biasedresidualsy;
  std::vector<TH1F*> m_pix_eca_biased_residualsx;
  std::vector<TH1F*> m_pix_eca_biased_residualsy;
  std::vector<TH1F*> m_pix_ecc_biased_residualsx;
  std::vector<TH1F*> m_pix_ecc_biased_residualsy;



  std::vector<TH2F*> m_pix_eca_residualsx_pt;
  std::vector<TH2F*> m_pix_eca_residualsy_pt;
  std::vector<TH2F*> m_pix_ecc_residualsx_pt;
  std::vector<TH2F*> m_pix_ecc_residualsy_pt;
  std::vector<TH2F*> m_pix_eca_biased_residualsx_pt;
  std::vector<TH2F*> m_pix_eca_biased_residualsy_pt;
  std::vector<TH2F*> m_pix_ecc_biased_residualsx_pt;
  std::vector<TH2F*> m_pix_ecc_biased_residualsy_pt;
  std::vector<TH2F*> m_pix_eca_pullsx_pt;
  std::vector<TH2F*> m_pix_eca_pullsy_pt;
  std::vector<TH2F*> m_pix_ecc_pullsx_pt;
  std::vector<TH2F*> m_pix_ecc_pullsy_pt;

  std::vector<TH2F*> m_pix_eca_residualsx_qoverp2;
  std::vector<TH2F*> m_pix_eca_residualsy_qoverp2;
  std::vector<TH2F*> m_pix_eca_biased_residualsx_qoverp2;
  std::vector<TH2F*> m_pix_eca_biased_residualsy_qoverp2;
  std::vector<TH2F*> m_pix_ecc_residualsx_qoverp2;
  std::vector<TH2F*> m_pix_ecc_residualsy_qoverp2;
  std::vector<TH2F*> m_pix_ecc_biased_residualsx_qoverp2;
  std::vector<TH2F*> m_pix_ecc_biased_residualsy_qoverp2;
  std::vector<TH2F*> m_pix_eca_biased_xresvsmodphi_2ds;
  std::vector<TH2F*> m_pix_eca_biased_yresvsmodphi_2ds;
  std::vector<TH2F*> m_pix_ecc_biased_xresvsmodphi_2ds;
  std::vector<TH2F*> m_pix_ecc_biased_yresvsmodphi_2ds;

  //SCT histograms
  TH1F* m_sct_b_residualx;
  TH1F* m_sct_b_residualx_fine;
  TH1F* m_sct_b_biasedresidualx;
  TH1F* m_sct_eca_residualx;
  TH1F* m_sct_ecc_residualx;
  TH1F* m_sct_eca_residualx_fine;
  TH1F* m_sct_ecc_residualx_fine;



  TH1F* m_sct_eca_pullx;
  TH1F* m_sct_ecc_pullx;


  TProfile* m_sct_bec_Oxresx_mean;
  TProfile* m_sct_bec_Oyresx_mean;
  TProfile* m_sct_bec_Oxresx_rms;
  TProfile* m_sct_bec_Oyresx_rms;

  TH2F* m_sct_eca_xresvsmodphi_2d;
  TH2F* m_sct_ecc_xresvsmodphi_2d;

  TH1F* m_sct_b_xresvsmodeta;
  TH1F* m_sct_b_xresvsmodphi;
  TH1F* m_sct_eca_xresvsmodphi;
  TH1F* m_sct_ecc_xresvsmodphi;

  TProfile* m_sct_b_Oxresxvsmodeta;
  TProfile* m_sct_b_Oxresxvsmodphi;
  TProfile* m_sct_b_Oyresxvsmodeta;
  TProfile* m_sct_b_Oyresxvsmodphi;
  TProfile* m_sct_eca_Oxresxvsmodphi;
  TProfile* m_sct_ecc_Oxresxvsmodphi;



  // Extended histograms



  // Pixel unbiased residuals vs pt
  TH2F* m_pix_b_residualx_pt;
  TH2F* m_pix_b_residualy_pt;
  TH2F* m_pix_eca_residualx_pt;
  TH2F* m_pix_eca_residualy_pt;
  TH2F* m_pix_ecc_residualx_pt;
  TH2F* m_pix_ecc_residualy_pt;




  TH1F* m_sct_b_biased_residualx;

  TH2F* m_sct_b_biased_residualx_pt;


  TH2F* m_sct_b_residualx_pt;


  TH2F* m_sct_b_biased_residualx_qoverp2;


  TH2F* m_sct_b_unbiased_residualx_qoverp2;



  TH1F* m_sct_eca_biased_residualx;
  TH1F* m_sct_eca_biased_residualy;
  TH1F* m_sct_ecc_biased_residualx;
  TH1F* m_sct_ecc_biased_residualy;

  TH2F* m_sct_eca_biased_residualx_pt;
  TH2F* m_sct_ecc_biased_residualx_pt;

  TH2F* m_sct_eca_residualx_pt;
  TH2F* m_sct_ecc_residualx_pt;

  TH2F* m_sct_eca_biased_residualx_qoverp2;
  TH2F* m_sct_ecc_biased_residualx_qoverp2;

  TH2F* m_sct_eca_unbiased_residualx_qoverp2;
  TH2F* m_sct_ecc_unbiased_residualx_qoverp2;





  // Local positions (extrapolated)
  TH1F* m_pix_b_extrapolated_localx;
  TH1F* m_pix_b_extrapolated_localy;
  TH1F* m_sct_b_extrapolated_st_localx;
  TH1F* m_sct_b_extrapolated_st_localy;
  TH1F* m_sct_b_extrapolated_nst_localx;
  TH1F* m_sct_b_extrapolated_nst_localy;

  // Local positions (mesasured)
  TH1F* m_pix_b_measured_localx;
  TH1F* m_pix_b_measured_localy;
  TH1F* m_sct_b_measured_st_localx;
  TH1F* m_sct_b_measured_st_localy;
  TH1F* m_sct_b_measured_nst_localx;
  TH1F* m_sct_b_measured_nst_localy;


  // Hit errors

  TH1F* m_hiterror_sct_b;
  TH1F* m_hiterror_sct_ec;
  TH1F* m_hiterror_sct_b_WideRange;
  TH1F* m_hiterror_sct_ec_WideRange;

  TH1F* m_hiterror_x_pix_b;
  TH1F* m_hiterror_x_pix_ec;
  TH1F* m_hiterror_y_pix_b;
  TH1F* m_hiterror_y_pix_ec;

  TH1F* m_hiterror_x_pix_b_WideRange;
  TH1F* m_hiterror_x_pix_ec_WideRange;
  TH1F* m_hiterror_y_pix_b_WideRange;
  TH1F* m_hiterror_y_pix_ec_WideRange;

  TH1F* m_hiterror_x_ibl_b;
  TH1F* m_hiterror_x_ibl_ec;
  TH1F* m_hiterror_y_ibl_b;
  TH1F* m_hiterror_y_ibl_ec;

  TH1F* m_hiterror_x_ibl_b_WideRange;
  TH1F* m_hiterror_x_ibl_ec_WideRange;
  TH1F* m_hiterror_y_ibl_b_WideRange;
  TH1F* m_hiterror_y_ibl_ec_WideRange;

  // Pulls vs pt
  // Pixel

  TH2F* m_pix_b_pullx_pt;
  TH2F* m_pix_b_pully_pt;
  TH2F* m_pix_eca_pullx_pt;
  TH2F* m_pix_eca_pully_pt;
  TH2F* m_pix_ecc_pullx_pt;
  TH2F* m_pix_ecc_pully_pt;



  /**====================================
   //TRT histograms
   //====================================*/

  /* Barrel
  //======================= */
  struct TRTBarrelHistograms;
  TRTBarrelHistograms* m_trt_b_hist;

  /** TRT EndCap
   //======================= */
  struct TRTEndcapHistograms;
  TRTEndcapHistograms* m_trt_ec_hist;


  // Weight for histograms
  bool m_applyHistWeight;
  std::string m_hWeightInFileName;
  TFile* m_hWeightInFile;
  TH2F*  m_hInputTrackWeight;
  std::string m_hWeightHistName;
  int    m_userInputWeigthMethod;

  // enumerate for trk weight methods
  enum {TRKETA_TRKPT, EVENTMU_TRKPT, EVENTMU_TRKETA};


  //I don't like an itialisation here.
  float z_axis[20] = {-322.8975, -301.7925, -280.6875,-259.5825,-228.2775,-186.7725,-145.2675,-103.7625,-62.2575,-20.7525,20.7525,62.2575,103.7625,145.2675,186.7725,228.2775,259.5825,280.6875,301.7925,322.8975};
  float m_z_fix;
  int m_minIBLhits;
  bool m_doIBLLBPlots;
};

#endif
