/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IDAlignMonGenericTracks_H
#define IDAlignMonGenericTracks_H

// **********************************************************************
// IDAlignMonGenericTracks.cxx
// AUTHORS: Beate Heinemann, Tobias Golling
// **********************************************************************

#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include "GaudiKernel/StatusCode.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
#include "StoreGate/ReadHandleKey.h"
#include <vector>
#include <map>

class TH1F;
class TH2F;
class TH3F;
class TProfile;
class TFile;
class AtlasDetectorID;
class PixelID;
class SCT_ID;
class TRT_ID;

namespace Trk  { 
  class Track;
  class VxTrackAtVertex;
}

class IInDetAlignHitQualSelTool; 

namespace InDetAlignMon{
  class TrackSelectionTool;
}

class IDAlignMonGenericTracks : public ManagedMonitorToolBase
{

public:

  IDAlignMonGenericTracks( const std::string & type, const std::string & name, const IInterface* parent ); 

  virtual ~IDAlignMonGenericTracks();

  virtual StatusCode initialize();
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();
  
  void RegisterHisto(MonGroup& mon, TH1* histo);
  void RegisterHisto(MonGroup& mon, TH2* histo);
  void RegisterHisto(MonGroup& mon, TProfile* histo);

  void ProcessAsymHistograms(TH1F* m_neg, TH1F* m_pos, TH1F* m_asym); 
  void InitializeHistograms();

protected:

  bool m_hasBeamCondSvc{};
  TH1F* m_summary{};

  TProfile* m_trk_chi2oDoF{};
  TProfile* m_trk_chi2Prob{};



  //Histo for self beam spot calculation
  TH3F* m_trk_d0_vs_phi0_z0{};

  // barrel
  TH1F* m_trk_d0_barrel{};
  TH1F* m_trk_d0_barrel_zoomin{};
  TH1F* m_trk_d0c_barrel{};
  TH1F* m_trk_z0_barrel{};
  TH1F* m_trk_z0_barrel_zoomin{};
  TProfile* m_trk_qopT_vs_phi_barrel{};
  TProfile* m_trk_d0_vs_phi_barrel{};
  TProfile* m_trk_d0_vs_z0_barrel{};
  TH1F* m_trk_phi0_neg_barrel{};
  TH1F* m_trk_phi0_pos_barrel{};
  TH1F* m_trk_phi0_asym_barrel{};
  TH1F* m_trk_pT_neg_barrel{};
  TH1F* m_trk_pT_pos_barrel{};
  TH1F* m_trk_pT_asym_barrel{};
  TH1F* m_npixhits_per_track_barrel{};
  TH1F* m_nscthits_per_track_barrel{};
  TH1F* m_ntrthits_per_track_barrel{};
  TH1F* m_chi2oDoF_barrel{};
  TH1F* m_phi_barrel{};
  TH2F* m_hitMap_barrel{};
  TH2F* m_hitMap_endcapA{};
  TH2F* m_hitMap_endcapC{};

  TH3F* m_trk_d0_vs_phi_vs_eta_barrel{};
  TH2F* m_trk_pT_vs_eta_barrel{};
  TH3F* m_trk_d0_wrtPV_vs_phi_vs_eta_barrel{};
  TH3F* m_trk_z0_wrtPV_vs_phi_vs_eta_barrel{};

  // endcap A
  TH1F* m_trk_d0_eca{}; 
  TH1F* m_trk_d0_eca_zoomin{}; 
  TH1F* m_trk_d0c_eca{};
  TH1F* m_trk_z0_eca{};
  TH1F* m_trk_z0_eca_zoomin{};
  TProfile* m_trk_qopT_vs_phi_eca{};
  TProfile* m_trk_d0_vs_phi_eca{};
  TProfile* m_trk_d0_vs_z0_eca{};
  TH1F* m_trk_phi0_neg_eca{};
  TH1F* m_trk_phi0_pos_eca{};
  TH1F* m_trk_phi0_asym_eca{};
  TH1F* m_trk_pT_neg_eca{};
  TH1F* m_trk_pT_pos_eca{};
  TH1F* m_trk_pT_asym_eca{};
  TH1F* m_npixhits_per_track_eca{};
  TH1F* m_nscthits_per_track_eca{};
  TH1F* m_ntrthits_per_track_eca{};
  TH1F* m_chi2oDoF_eca{};
  TH1F* m_phi_eca{};

  TH3F* m_trk_d0_vs_phi_vs_eta_eca{};
  TH2F* m_trk_pT_vs_eta_eca{};
  TH3F* m_trk_d0_wrtPV_vs_phi_vs_eta_eca{};
  TH3F* m_trk_z0_wrtPV_vs_phi_vs_eta_eca{};


  // endcap C
  TH1F* m_trk_d0_ecc{};
  TH1F* m_trk_d0_ecc_zoomin{};
  TH1F* m_trk_d0c_ecc{};
  TH1F* m_trk_z0_ecc{};
  TH1F* m_trk_z0_ecc_zoomin{};
  TProfile* m_trk_qopT_vs_phi_ecc{};
  TProfile* m_trk_d0_vs_phi_ecc{};
  TProfile* m_trk_d0_vs_z0_ecc{};
  TH1F* m_trk_phi0_neg_ecc{};
  TH1F* m_trk_phi0_pos_ecc{};
  TH1F* m_trk_phi0_asym_ecc{};
  TH1F* m_trk_pT_neg_ecc{};
  TH1F* m_trk_pT_pos_ecc{};
  TH1F* m_trk_pT_asym_ecc{};
  TH1F* m_npixhits_per_track_ecc{};
  TH1F* m_nscthits_per_track_ecc{};
  TH1F* m_ntrthits_per_track_ecc{};
  TH1F* m_chi2oDoF_ecc{};
  TH1F* m_phi_ecc{};

  TH3F* m_trk_d0_vs_phi_vs_eta_ecc{};
  TH2F* m_trk_pT_vs_eta_ecc{};

  TH3F* m_trk_d0_wrtPV_vs_phi_vs_eta_ecc{};
  TH3F* m_trk_z0_wrtPV_vs_phi_vs_eta_ecc{};

  // Whole detector


  TH1F* m_nhits_per_event{};
  TH1F* m_nhits_per_track{};
  TH1F* m_ntrk{};
  TH1F* m_ngtrk{};
  TH1F* m_npixhits_per_track{};
  TH1F* m_nscthits_per_track{};
  TH1F* m_ntrthits_per_track{};
  TH1F* m_chi2oDoF{};
  TH1F* m_eta{};
  TH1F* m_phi{};
  TH1F* m_z0{};
  TH1F* m_z0sintheta{};
  TH1F* m_z0_pvcorr{};
  TH1F* m_z0sintheta_pvcorr{};
  TH1F* m_d0{};
  TH1F* m_d0_pvcorr{};
  TH1F* m_d0_bscorr{};
  TH1F* m_trk_d0_wrtPV{};
  TH1F* m_trk_z0_wrtPV{};
  TH1F* m_pT{};
  TH1F* m_pTRes{};
  TH1F* m_pTResOverP{};
  TH1F* m_P{};
  
  TH3F* m_trk_d0_vs_phi_vs_eta{};
  TH2F* m_trk_pT_vs_eta{};
  
  TH3F* m_trk_d0_wrtPV_vs_phi_vs_eta{};
  TH3F* m_trk_z0_wrtPV_vs_phi_vs_eta{};


  // extended plots
  TH2F* m_trk_PIXvSCTHits{};
  TH2F* m_trk_PIXHitsvEta{};
  TH2F* m_trk_SCTHitsvEta{};
  TH2F* m_trk_TRTHitsvEta{};
  TH2F* m_trk_chi2oDoF_Phi{};
  TH2F* m_trk_chi2oDoF_Pt{};
  TH2F* m_trk_chi2oDoF_P{};
  TH1F* m_trk_chi2ProbDist{};
  TH1F* m_errCotTheta{};
  TH2F* m_errCotThetaVsD0BS{};
  TH2F* m_errCotThetaVsPt{};
  TH2F* m_errCotThetaVsP{};
  TH2F* m_errCotThetaVsPhi{};
  TH2F* m_errCotThetaVsEta{};
  TH1F* m_errTheta{};
  TH2F* m_errThetaVsD0BS{};
  TH2F* m_errThetaVsPt{};
  TH2F* m_errThetaVsP{};
  TH2F* m_errThetaVsPhi{};
  TH2F* m_errThetaVsEta{};
  TH1F* m_errD0{};
  TH2F* m_errD0VsD0BS{};
  TH2F* m_errD0VsPt{};
  TH2F* m_errD0VsP{};
  TH2F* m_errD0VsPhi{};
  TH2F* m_errD0VsPhiBarrel{};
  TH2F* m_errD0VsPhiECA{};
  TH2F* m_errD0VsPhiECC{};
  TH2F* m_errD0VsEta{};
  TH1F* m_errPhi0{};
  TH2F* m_errPhi0VsD0BS{};
  TH2F* m_errPhi0VsPt{};
  TH2F* m_errPhi0VsP{};
  TH2F* m_errPhi0VsPhi0{};
  TH2F* m_errPhi0VsEta{};
  TH1F* m_errZ0{};
  TH2F* m_errZ0VsD0BS{};
  TH2F* m_errZ0VsPt{};
  TH2F* m_errZ0VsP{};
  TH2F* m_errZ0VsPhi0{};
  TH2F* m_errZ0VsEta{};
  TH1F* m_errPt{};
  TH2F* m_PtVsPhi0Pos{};
  TH2F* m_PtVsPhi0Neg{};
  TH2F* m_errPtVsD0BS{};
  TH2F* m_errPtVsPt{};
  TH2F* m_errPtVsP{};
  TH1F* m_errPt_Pt2{};
  TH2F* m_errPt_Pt2VsPt{};
  TH2F* m_errPt_Pt2VsPhi0{};
  TH2F* m_errPt_Pt2VsEta{};
  TH2F* m_errPtVsPhi0{};
  TH2F* m_errPtVsEta{};
  
  TH2F* m_D0VsPhi0{};
  TH2F* m_Z0VsEta{};
  TH2F* m_QoverPtVsPhi0{};
  TH2F* m_QoverPtVsEta{};
  TH2F* m_QPtVsPhi0{};
  TH2F* m_QPtVsEta{};
       
  //BeamSpot Plots

  TH2F* m_D0bsVsPhi0{};
  TH2F* m_D0bsVsPhi0ECC{};
  TH2F* m_D0bsVsPhi0ECA{};
  TH2F* m_D0bsVsPhi0Barrel{};
  TH2F* m_D0bsVsEta{};
  TH2F* m_D0bsVsPt{};
  TH2F* m_D0bsVsPtECC{};
  TH2F* m_D0bsVsPtECA{};
  TH2F* m_D0bsVsPtBarrel{};


  //BeamSpot Position Plots

  TH2F* m_YBs_vs_XBs{};
  TH2F* m_YBs_vs_ZBs{};
  TH2F* m_XBs_vs_ZBs{};

  TH1F* m_XBs{};
  TH1F* m_YBs{};
  TH1F* m_ZBs{};
  TH1F* m_TiltX_Bs{};
  TH1F* m_TiltY_Bs{};

  //versus lumiblock
  TProfile* m_XBs_vs_LumiBlock{};
  TProfile* m_YBs_vs_LumiBlock{};
  TProfile* m_ZBs_vs_LumiBlock{};
  TProfile* m_BeamSpotTiltX_vs_LumiBlock{};
  TProfile* m_BeamSpotTiltY_vs_LumiBlock{};
       

       


  // End of extended plots
  
  TH1F* m_phi_barrel_pos_2_5GeV{};
  TH1F* m_phi_barrel_pos_5_10GeV{};
  TH1F* m_phi_barrel_pos_10_20GeV{};
  TH1F* m_phi_barrel_pos_20plusGeV{};
  TH1F* m_phi_barrel_neg_2_5GeV{};
  TH1F* m_phi_barrel_neg_5_10GeV{};
  TH1F* m_phi_barrel_neg_10_20GeV{};
  TH1F* m_phi_barrel_neg_20plusGeV{};

  TH1F* m_phi_eca_pos_2_5GeV{};
  TH1F* m_phi_eca_pos_5_10GeV{};
  TH1F* m_phi_eca_pos_10_20GeV{};
  TH1F* m_phi_eca_pos_20plusGeV{};
  TH1F* m_phi_eca_neg_2_5GeV{};
  TH1F* m_phi_eca_neg_5_10GeV{};
  TH1F* m_phi_eca_neg_10_20GeV{};
  TH1F* m_phi_eca_neg_20plusGeV{};

  TH1F* m_phi_ecc_pos_2_5GeV{};
  TH1F* m_phi_ecc_pos_5_10GeV{};
  TH1F* m_phi_ecc_pos_10_20GeV{};
  TH1F* m_phi_ecc_pos_20plusGeV{};
  TH1F* m_phi_ecc_neg_2_5GeV{};
  TH1F* m_phi_ecc_neg_5_10GeV{};
  TH1F* m_phi_ecc_neg_10_20GeV{};
  TH1F* m_phi_ecc_neg_20plusGeV{};

  TH2F* m_eta_phi_pos_2_5GeV{};
  TH2F* m_eta_phi_pos_5_10GeV{};
  TH2F* m_eta_phi_pos_10_20GeV{};
  TH2F* m_eta_phi_pos_20plusGeV{};
  TH2F* m_eta_phi_neg_2_5GeV{};
  TH2F* m_eta_phi_neg_5_10GeV{};
  TH2F* m_eta_phi_neg_10_20GeV{};
  TH2F* m_eta_phi_neg_20plusGeV{};

  TH1F* m_Zmumu{};
  TH1F* m_Zmumu_barrel{};
  TH1F* m_Zmumu_eca{};
  TH1F* m_Zmumu_ecc{};
  TH1F* m_Zmumu_barrel_eca{};
  TH1F* m_Zmumu_barrel_ecc{};
  TH1F* m_ZpT_n{};
  TH1F* m_ZpT_p{};
  TH1F* m_ZpT_diff{};

  TH1F* m_pT_n{};
  TH1F* m_pT_p{};
  TH1F* m_pT_diff{};
  TH1F* m_trk_pT_asym{};

  TH1F* m_eta_neg{};
  TH1F* m_eta_pos{};
  TH1F* m_eta_asym{};

  TH1F* m_LumiBlock{};
  TH1F* m_Tracks_per_LumiBlock{};
  TH1F* m_NPIX_per_LumiBlock{};
  TH1F* m_NSCT_per_LumiBlock{};
  TH1F* m_NTRT_per_LumiBlock{};

  TH1F* m_trk_d0c_pos{};
  TH1F* m_trk_d0c_neg{};
  TH1F* m_trk_d0c_pos_barrel{};
  TH1F* m_trk_d0c_neg_barrel{};
  TH1F* m_trk_d0c_pos_eca{};
  TH1F* m_trk_d0c_neg_eca{};
  TH1F* m_trk_d0c_pos_ecc{};
  TH1F* m_trk_d0c_neg_ecc{};
  TH1F* m_trk_d0c_asym{};
  TH1F* m_trk_d0c_asym_barrel{};
  TH1F* m_trk_d0c_asym_eca{};
  TH1F* m_trk_d0c_asym_ecc{};

  TH1F* m_trk_z0c_pos{};
  TH1F* m_trk_z0c_neg{};
  TH1F* m_trk_z0c_pos_barrel{};
  TH1F* m_trk_z0c_neg_barrel{};
  TH1F* m_trk_z0c_pos_eca{};
  TH1F* m_trk_z0c_neg_eca{};
  TH1F* m_trk_z0c_pos_ecc{};
  TH1F* m_trk_z0c_neg_ecc{};
  TH1F* m_trk_z0c_asym{};
  TH1F* m_trk_z0c_asym_barrel{};
  TH1F* m_trk_z0c_asym_eca{};
  TH1F* m_trk_z0c_asym_ecc{};


 private:
 
  bool fillVertexInformation(std::map<const xAOD::TrackParticle*, const xAOD::Vertex*>& trackVertexMapTP) const;
  static const Trk::Track*        getTrkTrack(const Trk::VxTrackAtVertex*);
  const xAOD::Vertex* findAssociatedVertexTP(const std::map<const xAOD::TrackParticle*, const xAOD::Vertex*>& trackVertexMapTP, const xAOD::TrackParticle *) const;

  const AtlasDetectorID*                m_idHelper{};
  const PixelID*                        m_pixelID{};
  const SCT_ID*                         m_sctID{}; 
  const TRT_ID*                         m_trtID{}; 
  
  std::string m_stream;
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this, "EventInfoKey", "EventInfo", "SG Key of EventInfo object"};
  SG::ReadHandleKey<xAOD::VertexContainer> m_VxPrimContainerName{this, "VxPrimContainerName", ""};
  int m_checkrate{};
  int m_events{};
  int m_histosBooked{};
  SG::ReadHandleKey<TrackCollection> m_tracksName{this, "tracksName", "ExtendedTracks"};
  std::string m_triggerChainName;
  float m_barrelEta{};
  const xAOD::VertexContainer* m_vertices{};
  const xAOD::Vertex* m_pvtx{};
  bool m_doHitQuality{};
  bool m_extendedPlots{};
  float m_d0Range{};
  float m_d0BsRange{};
  int   m_d0BsNbins{};
  float m_pTRange{};
  float m_z0Range{};
  float m_etaRange{};
  int   m_NTracksRange{};
  int   m_rangePixHits{};
  int   m_rangeSCTHits{};
  int   m_rangeTRTHits{};

  bool m_doIP{};

  
  
  ToolHandle< InDetAlignMon::TrackSelectionTool > m_trackSelection; 
  ToolHandle<IInDetAlignHitQualSelTool>  m_hitQualityTool;
  SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
  
  PublicToolHandle< Trk::ITrackToVertexIPEstimator >  m_trackToVertexIPEstimator
     {this,"TrackToVertexIPEstimator","Trk::TrackToVertexIPEstimator",""};
  
  std::string m_Pixel_Manager;
  std::string m_SCT_Manager;
  std::string m_TRT_Manager;

  // Weight variables
  bool m_applyHistWeight{};
  std::string m_hWeightInFileName;
  TFile* m_hWeightInFile{};
  TH2F*  m_etapTWeight{};
  std::string m_hWeightHistName;
  
};

#endif
