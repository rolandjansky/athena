/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IDAlignMonEfficiencies_H
#define IDAlignMonEfficiencies_H

// **********************************************************************
// IDAlignMonEfficiencies.cxx
// AUTHORS: Beate Heinemann, Tobias Golling, John Alison
// **********************************************************************

#include <vector>

#include "GaudiKernel/StatusCode.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "EventPrimitives/EventPrimitives.h"

//forward declaration
class TH1F;
class TH2F;
class TH3F;
class TProfile;
class TProfile2D;
class AtlasDetectorID;
class PixelID;
class SCT_ID;
class TRT_ID;
class TH1F_LW;
class TH2F_LW;

namespace InDetDD{
  class PixelDetectorManager;
  class SCT_DetectorManager;
}

namespace Trk {
  class TrackStateOnSurface;
  class MeasurementBase;
  class Track;
  class ITrackHoleSearchTool;
}
//class Identifier;
class IInDetAlignHitQualSelTool; 

namespace InDetAlignMon{
  class TrackSelectionTool;
}

class IDAlignMonEfficiencies : public ManagedMonitorToolBase
{

 public:

  IDAlignMonEfficiencies( const std::string & type, const std::string & name, const IInterface* parent ); 

  virtual ~IDAlignMonEfficiencies();

  virtual StatusCode initialize();
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

  /** Convert from an int to a string */
  std::string intToString(int input);

  /** Make Profiles */
  TProfile* MakeProfile(const std::string & name, const std::string & title, int nBins, float xLow, float xUp, float yLow, float yUp, const std::string & xAxisTitle, const std::string & yAxisTitle, bool setMinZero=true);

  /** Make Histograms */
  TH1F_LW* MakeHist(const std::string & name, const std::string & title, int nBins, float xLow, float xUp, const std::string & xAxisTitle, const std::string & yAxisTitle);
	
  void RegisterHisto(MonGroup& mon, TH1* histo);
  void RegisterHisto(MonGroup& mon, TH1F_LW* histo);
  void RegisterHisto(MonGroup& mon, TH2* histo);
  void RegisterHisto(MonGroup& mon, TProfile* histo, const std::string & yAxisName="");
  void RegisterHisto(MonGroup& mon, TProfile2D* histo);

  void makeEffHisto(TH1F* h_num, TH1F* h_denom, TProfile* h_eff);
  void makeEffHisto(TH1F_LW* h_num, TH1F_LW* h_denom, TProfile* h_eff);
  void makeOverlapFracHisto(TH1F* h_num, TH1F* h_denom, TProfile* h_eff);
  void makeEffHisto(TH2F* h_num, TH2F* h_denom, TProfile2D* h_eff);
  void makeEffHisto(TH2F* h_num, TH2F* h_denom, TH2F* h_eff);
  void makeEffHistoWithCut(TH2F* h_num, TH2F* h_denom, TProfile2D* h_eff);

  void makeTRTHistograms(MonGroup& al_monitoring,MonGroup& al_monitoring_ls);
  void makeTRTBarrelHistograms(MonGroup& al_mon, MonGroup& al_mon_ls);
  void makeTRTEndcapHistograms(MonGroup& al_mon, MonGroup& al_mon_ls);
  void makePIXBarrelHistograms(MonGroup& al_mon);
  void makePIXEndCapsHistograms(MonGroup& al_mon);
  void makeSCTBarrelHistograms(MonGroup& al_mon);
  void makeSCTEndCapsHistograms(MonGroup& al_mon);
  void fillTRTTotalMeasurements(int m_barrel_ec,int m_layer_or_wheel,int m_phi_module,int m_straw_layer);
  void fillTRTBarrelTotalMeasurements(int m_layer_or_wheel,int m_phi_module,int m_straw_layer);
  void fillTRTEndcapTotalMeasurements(int m_barrel_ec, int m_layer_or_wheel,int m_phi_module,int m_straw_layer);

  void fillTRTHits(int m_barrel_ec,int m_layer_or_wheel,int m_phi_module,int m_straw_layer, bool isTubeHit);
  void fillTRTBarrelHits(int m_layer_or_wheel,int m_phi_module,int m_straw_layer, bool isTubeHit);
  void fillTRTEndcapHits(int m_barrel_ec,int m_layer_or_wheel,int m_phi_module,int m_straw_layer, bool isTubeHit);

  void fillTRTOutliers(int m_barrel_ec,int m_layer_or_wheel,int m_phi_module,int m_straw_layer);
  void fillTRTBarrelOutliers(int m_layer_or_wheel,int m_phi_module,int m_straw_layer);
  void fillTRTEndcapOutliers(int m_barrel_ec, int m_layer_or_wheel,int m_straw_layer);
  
  void InitializeHistograms();

  unsigned int getRing(unsigned int wheel,unsigned int strawlayer);
 
  std::pair<const Trk::TrackStateOnSurface*, const Trk::TrackStateOnSurface*> findOverlapHit(const Trk::Track*, const Trk::MeasurementBase*);

 protected:

  // histograms go here


  TH1F* m_hits_vs_layer_barrel;
  TH1F* m_hits_vs_layer_eca;
  TH1F* m_hits_vs_layer_ecc;

  TH1F* m_measurements_vs_layer_barrel;
  TH1F* m_measurements_vs_layer_eca;
  TH1F* m_measurements_vs_layer_ecc;

  TH1F* m_outliers_vs_layer_barrel;
  TH1F* m_outliers_vs_layer_eca;
  TH1F* m_outliers_vs_layer_ecc;
	
  TH1F* m_holes_vs_layer_barrel;
  TH1F* m_holes_vs_layer_eca;
  TH1F* m_holes_vs_layer_ecc;
	
  TH1F* m_noholes_vs_layer_barrel;
  TH1F* m_noholes_vs_layer_eca;
  TH1F* m_noholes_vs_layer_ecc;

  TH1F* m_overlapX_vs_layer_barrel;
  TH1F* m_overlapX_vs_layer_eca;
  TH1F* m_overlapX_vs_layer_ecc;
  TH1F* m_overlapY_vs_layer_barrel;
  TH1F* m_overlapY_vs_layer_eca;
  TH1F* m_overlapY_vs_layer_ecc;

  TProfile* m_measurements_eff_vs_layer_barrel;
  TProfile* m_measurements_eff_vs_layer_eca;
  TProfile* m_measurements_eff_vs_layer_ecc;

  TProfile* m_outliers_eff_vs_layer_barrel;
  TProfile* m_outliers_eff_vs_layer_eca;
  TProfile* m_outliers_eff_vs_layer_ecc;
	
  TProfile* m_holes_eff_vs_layer_barrel;
  TProfile* m_holes_eff_vs_layer_eca;
  TProfile* m_holes_eff_vs_layer_ecc;
	
  TProfile* m_noholes_eff_vs_layer_barrel;
  TProfile* m_noholes_eff_vs_layer_eca;
  TProfile* m_noholes_eff_vs_layer_ecc;

  TProfile* m_overlapX_eff_vs_layer_barrel;
  TProfile* m_overlapX_eff_vs_layer_eca;
  TProfile* m_overlapX_eff_vs_layer_ecc;
  TProfile* m_overlapY_eff_vs_layer_barrel;
  TProfile* m_overlapY_eff_vs_layer_eca;
  TProfile* m_overlapY_eff_vs_layer_ecc;

  TProfile2D* m_measurements_eff_vs_Eta_Phi_pix_eca;
  TProfile2D* m_measurements_eff_vs_Eta_Phi_pix_ecc;
  TH2F* m_measurements_vs_Eta_Phi_pix_eca;
  TH2F* m_measurements_vs_Eta_Phi_pix_ecc;
  TH2F* m_hits_vs_Eta_Phi_pix_eca;
  TH2F* m_hits_vs_Eta_Phi_pix_ecc;
  TH2F* m_outliers_vs_Eta_Phi_pix_eca;
  TH2F* m_outliers_vs_Eta_Phi_pix_ecc;
  TH2F* m_holes_vs_Eta_Phi_pix_eca;
  TH2F* m_holes_vs_Eta_Phi_pix_ecc;

  TProfile2D* m_measurements_eff_vs_Eta_Phi_sct_eca;
  TProfile2D* m_measurements_eff_vs_Eta_Phi_sct_ecc;
  TH2F* m_measurements_vs_Eta_Phi_sct_eca;
  TH2F* m_measurements_vs_Eta_Phi_sct_ecc;
  TH3F* m_measurements_vs_Eta_Phi_sct_eca_3d_s0;
  TH3F* m_measurements_vs_Eta_Phi_sct_eca_3d_s1;
  TH3F* m_measurements_vs_Eta_Phi_sct_ecc_3d_s0;
  TH3F* m_measurements_vs_Eta_Phi_sct_ecc_3d_s1;
  TH2F* m_hits_vs_Eta_Phi_sct_eca;
  TH2F* m_hits_vs_Eta_Phi_sct_ecc;
  TH2F* m_outliers_vs_Eta_Phi_sct_eca;
  TH2F* m_outliers_vs_Eta_Phi_sct_ecc;
  TH2F* m_holes_vs_Eta_Phi_sct_eca;
  TH2F* m_holes_vs_Eta_Phi_sct_ecc;

  std::vector<TH1F*> m_hits_vs_Phi_pix_eca;
  std::vector<TH1F*> m_hits_vs_Phi_sct_eca;

  std::vector<TH1F*> m_hits_vs_Phi_pix_ecc;
  std::vector<TH1F*> m_hits_vs_Phi_sct_ecc;
 

  std::vector<TH1F*> m_measurements_vs_Phi_pix_eca;
  std::vector<TH1F*> m_measurements_vs_Phi_sct_eca;
 
  std::vector<TH1F*> m_measurements_vs_Phi_pix_ecc;
  std::vector<TH1F*> m_measurements_vs_Phi_sct_ecc;
 
  std::vector<TProfile*> m_measurements_eff_vs_Phi_pix_eca;
  std::vector<TProfile*> m_measurements_eff_vs_Phi_sct_eca;
 

  std::vector<TProfile*> m_measurements_eff_vs_Phi_pix_ecc;
  std::vector<TProfile*> m_measurements_eff_vs_Phi_sct_ecc;
 

  std::vector<TH1F*> m_overlapX_vs_Phi_pix_eca;
  std::vector<TH1F*> m_overlapX_vs_Phi_sct_eca;
 

  std::vector<TH1F*> m_overlapX_vs_Phi_pix_ecc;
  std::vector<TH1F*> m_overlapX_vs_Phi_sct_ecc;
 
  std::vector<TProfile*> m_overlapX_eff_vs_Phi_pix_eca;
  std::vector<TProfile*> m_overlapX_eff_vs_Phi_sct_eca;
  

  std::vector<TProfile*> m_overlapX_eff_vs_Phi_pix_ecc;
  std::vector<TProfile*> m_overlapX_eff_vs_Phi_sct_ecc;
 

  std::vector<TH1F*> m_overlapY_vs_Phi_pix_eca;
  std::vector<TH1F*> m_overlapY_vs_Phi_sct_eca;
 

  std::vector<TH1F*> m_overlapY_vs_Phi_pix_ecc;
  std::vector<TH1F*> m_overlapY_vs_Phi_sct_ecc;
 

  std::vector<TProfile*> m_overlapY_eff_vs_Phi_pix_eca;
  std::vector<TProfile*> m_overlapY_eff_vs_Phi_sct_eca;
  
  std::vector<TProfile*> m_overlapY_eff_vs_Phi_pix_ecc;
  std::vector<TProfile*> m_overlapY_eff_vs_Phi_sct_ecc;
 

  std::vector<TProfile2D*> m_measurements_eff_vs_Eta_Phi_pix_b;
  std::vector<TProfile2D*> m_measurements_eff_vs_Eta_Phi_sct_b;
  
  std::vector<TH2F*> m_hits_vs_Eta_Phi_pix_b;
  std::vector<TH2F*> m_hits_vs_Eta_Phi_sct_b;
  std::vector<TH2F*> m_hits_vs_Eta_Phi_sct_s0_b;
  std::vector<TH2F*> m_hits_vs_Eta_Phi_sct_s1_b;
  
  std::vector<TH2F*> m_measurements_vs_Eta_Phi_pix_b;
  std::vector<TH2F*> m_measurements_vs_Eta_Phi_sct_b;
  std::vector<TH2F*> m_measurements_vs_Eta_Phi_sct_s0_b;  
  std::vector<TH2F*> m_measurements_vs_Eta_Phi_sct_s1_b;  

  std::vector<TProfile2D*> m_measurements_eff_vs_Eta_Phi_sct_s0_b;  
  std::vector<TProfile2D*> m_measurements_eff_vs_Eta_Phi_sct_s1_b;  

  std::vector<TH2F*> m_outliers_vs_Eta_Phi_pix_b;
  std::vector<TH2F*> m_outliers_vs_Eta_Phi_sct_b;
  
  std::vector<TH2F*> m_holes_vs_Eta_Phi_pix_b;
  std::vector<TH2F*> m_holes_vs_Eta_Phi_sct_b;
  

  std::vector<TH1F*> m_hits_vs_Eta_pix_b;
  std::vector<TH1F*> m_hits_vs_Eta_sct_b;
  

  std::vector<TH1F*> m_measurements_vs_Eta_pix_b;
  std::vector<TH1F*> m_measurements_vs_Eta_sct_b;
  
  std::vector<TH1F*> m_overlapY_vs_Eta_pix_b;
  std::vector<TH1F*> m_overlapY_vs_Eta_sct_b;
  
  std::vector<TH1F*> m_overlapX_vs_Eta_pix_b;
  std::vector<TH1F*> m_overlapX_vs_Eta_sct_b;
  
  std::vector<TH1F*> m_outliers_vs_Eta_pix_b;
  std::vector<TH1F*> m_outliers_vs_Eta_sct_b;
  
  std::vector<TH1F*> m_holes_vs_Eta_pix_b;
  std::vector<TH1F*> m_holes_vs_Eta_sct_b;
  
  std::vector<TProfile*> m_measurements_eff_vs_Eta_pix_b;
  std::vector<TProfile*> m_measurements_eff_vs_Eta_sct_b;
  
  std::vector<TProfile*> m_overlapX_eff_vs_Eta_pix_b;
  std::vector<TProfile*> m_overlapX_eff_vs_Eta_sct_b;
  
  std::vector<TProfile*> m_overlapY_eff_vs_Eta_pix_b;
  std::vector<TProfile*> m_overlapY_eff_vs_Eta_sct_b;
  
  std::vector<TProfile*> m_outliers_eff_vs_Eta_pix_b;
  std::vector<TProfile*> m_outliers_eff_vs_Eta_sct_b;
  
  std::vector<TProfile*> m_holes_eff_vs_Eta_pix_b;
  std::vector<TProfile*> m_holes_eff_vs_Eta_sct_b;
  std::vector<TH1F*> m_hits_vs_Phi_pix_b;
  std::vector<TH1F*> m_hits_vs_Phi_sct_b;
  
  std::vector<TH1F*> m_measurements_vs_Phi_pix_b;
  std::vector<TH1F*> m_measurements_vs_Phi_sct_b;
  
  std::vector<TH1F*> m_overlapX_vs_Phi_pix_b;
  std::vector<TH1F*> m_overlapX_vs_Phi_sct_b;
  
  std::vector<TH1F*> m_overlapY_vs_Phi_pix_b;
  std::vector<TH1F*> m_overlapY_vs_Phi_sct_b;
  
  std::vector<TH1F*> m_outliers_vs_Phi_pix_b;
  std::vector<TH1F*> m_outliers_vs_Phi_sct_b;
  
  std::vector<TH1F*> m_holes_vs_Phi_pix_b;
  std::vector<TH1F*> m_holes_vs_Phi_sct_b;
  
  std::vector<TProfile*> m_measurements_eff_vs_Phi_pix_b;
  std::vector<TProfile*> m_measurements_eff_vs_Phi_sct_b;
  
  std::vector<TProfile*> m_measurements_eff_vs_LB_pix_b;
  std::vector<TProfile*> m_holes_eff_vs_LB_pix_b;
  std::vector<TProfile*> m_outliers_eff_vs_LB_pix_b;
  std::vector<TH1F*> m_measurements_vs_LB_pix_b;
  std::vector<TH1F*> m_holes_vs_LB_pix_b;
  std::vector<TH1F*> m_outliers_vs_LB_pix_b;
  std::vector<TH1F*> m_hits_vs_LB_pix_b;

  std::vector<TProfile*> m_measurements_eff_vs_LB_sct_b;
  std::vector<TH1F*> m_measurements_vs_LB_sct_b;
  std::vector<TH1F*> m_hits_vs_LB_sct_b;

  std::vector<TProfile*> m_measurements_eff_vs_LB_pix_eca;
  std::vector<TProfile*> m_measurements_eff_vs_LB_pix_ecc;
   std::vector<TProfile*> m_holes_eff_vs_LB_pix_eca;
  std::vector<TProfile*> m_holes_eff_vs_LB_pix_ecc;
   std::vector<TProfile*> m_outliers_eff_vs_LB_pix_eca;
  std::vector<TProfile*> m_outliers_eff_vs_LB_pix_ecc;
  
  std::vector<TH1F*>     m_hits_vs_LB_pix_eca;
  std::vector<TH1F*>     m_outliers_vs_LB_pix_eca;
  std::vector<TH1F*>     m_holes_vs_LB_pix_eca;
  std::vector<TH1F*>     m_measurements_vs_LB_pix_eca;
  std::vector<TH1F*>     m_hits_vs_LB_pix_ecc;
  std::vector<TH1F*>     m_outliers_vs_LB_pix_ecc;
  std::vector<TH1F*>     m_holes_vs_LB_pix_ecc;
  std::vector<TH1F*>     m_measurements_vs_LB_pix_ecc;
  
  
  TProfile* m_measurements_eff_vs_LB_sct_eca{};
  TH1F*     m_hits_vs_LB_sct_eca{};
  TH1F*     m_measurements_vs_LB_sct_eca{};
  
  
  
  TProfile* m_measurements_eff_vs_LB_sct_ecc{};
  TH1F*     m_hits_vs_LB_sct_ecc{};
  TH1F*     m_measurements_vs_LB_sct_ecc{};

  
  std::vector<TProfile*> m_overlapX_eff_vs_Phi_pix_b;
  std::vector<TProfile*> m_overlapX_eff_vs_Phi_sct_b;
  
  std::vector<TProfile*> m_overlapY_eff_vs_Phi_pix_b;
  std::vector<TProfile*> m_overlapY_eff_vs_Phi_sct_b;
  
  std::vector<TProfile*> m_outliers_eff_vs_Phi_pix_b;
  std::vector<TProfile*> m_outliers_eff_vs_Phi_sct_b;
  
  std::vector<TProfile*> m_holes_eff_vs_Phi_pix_b;
  std::vector<TProfile*> m_holes_eff_vs_Phi_sct_b;
  
  std::vector<TH1F*> m_hits_vs_pT_pix_b;
  std::vector<TH1F*> m_hits_vs_pT_sct_b;
  
  std::vector<TH1F*> m_measurements_vs_pT_pix_b;
  std::vector<TH1F*> m_measurements_vs_pT_sct_b;
  
  
  std::vector<TH1F*> m_outliers_vs_pT_pix_b;
  std::vector<TH1F*> m_outliers_vs_pT_sct_b;
  
  std::vector<TH1F*> m_holes_vs_pT_pix_b;
  std::vector<TH1F*> m_holes_vs_pT_sct_b;
    
  std::vector<TProfile*> m_measurements_eff_vs_pT_pix_b;
  std::vector<TProfile*> m_measurements_eff_vs_pT_sct_b;
    
  std::vector<TProfile*> m_outliers_eff_vs_pT_pix_b;
  std::vector<TProfile*> m_outliers_eff_vs_pT_sct_b;
  
  std::vector<TProfile*> m_holes_eff_vs_pT_pix_b;
  std::vector<TProfile*> m_holes_eff_vs_pT_sct_b;




  /** 
  //  TRT plots
  */
  /* Barrel
  //======================= */
  struct TRTBarrelHistograms;
  TRTBarrelHistograms* m_trt_b_hist;

  /** TRT EndCap 
   //======================= */
  struct TRTEndcapHistograms;
  TRTEndcapHistograms* m_trt_ec_hist;
  
 private:

  //================================================================
  // Establishes a minimim window for the TProfile 
  //================================================================
  void SetMinWindow(TProfile* hProf, float min, float max);
  void SetMinWindow(TProfile2D* hProf, float min, float max);
	
  const AtlasDetectorID*                m_idHelper;
  const PixelID*                        m_pixelID;
  const SCT_ID*                         m_sctID; 
  const TRT_ID*                         m_trtID; 
  const InDetDD::PixelDetectorManager*  m_PIX_Mgr; 
  const InDetDD::SCT_DetectorManager*   m_SCT_Mgr;
	
  //const InDetDD::TRT_DetectorManager    *m_TRT_Mgr;

  std::string m_stream;
  std::string m_tracksName;
  ToolHandle<Trk::ITrackSummaryTool> m_trackSumTool;
  ToolHandle<Trk::ITrackHoleSearchTool>   m_holeSearchTool; 
  ToolHandle<InDetAlignMon::TrackSelectionTool>   m_trackSelection; 
  ToolHandle<IInDetAlignHitQualSelTool>  m_hitQualityTool;
  float m_minSiliconEffWindow;
  float m_maxSiliconEffWindow;
  std::string m_triggerChainName;
  bool m_doHoleSearch;
  bool m_extendedPlots;
  std::string m_Pixel_Manager;
  std::string m_SCT_Manager;
  std::string m_TRT_Manager;
  int m_checkrate;
  int m_NPixLayers;
  int m_events;
  int m_mapSplit;
  int m_histosBooked;
  bool m_doHitQuality;
  float m_minLB;
  float m_maxLB;
  int m_nLB;

};

#endif
