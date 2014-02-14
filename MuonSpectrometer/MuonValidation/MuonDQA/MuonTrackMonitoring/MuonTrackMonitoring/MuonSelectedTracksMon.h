/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonSelectedTracksMon_H
#define MuonSelectedTracksMon_H

// **********************************************************************
// MuonSelectedTracksMon.cxx
// AUTHORS: N. Benekos, E. Christidi, A. Eppig, Tony Liss
// **********************************************************************

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "TrkToolInterfaces/ITrackSummaryTool.h"

#include "TrigConfL1Data/TriggerItem.h"
#include "TrigDecisionInterface/ITrigDecisionTool.h"
 
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"

#include <vector>

class IInterface;

namespace Trk {
  class Track;
  class ITrackSummaryTool;
  class TrackSummary;
  class MaterialEffectsOnTrack; 
}

namespace Muon {
  class MuonEDMHelperTool;
  class MuonIdHelperTool;
  class IMuonTofTool; 
  class IMuonHitSummaryTool;
}
 

class MuonSelectedTracksMon : public ManagedMonitorToolBase
{

 public:
  /** Constructor */
  MuonSelectedTracksMon( const std::string & type, const std::string & name, const IInterface* parent );
  /** Destructor */
  virtual ~MuonSelectedTracksMon();
  
  StatusCode initialize();

  /** Histogram booking method */
  virtual StatusCode bookHistograms();
  /** Histogram filling method */
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms(); 
  
   
  StatusCode finalize();
  float   RadsToDegrees(float Radians);
 protected:
  
 private:

 
  StatusCode bookSummaryHistograms();
  StatusCode bookTrkParametersHistograms();
  StatusCode bookTrkHolesHistograms();
  StatusCode bookHitsPerTrkHistograms();

  StatusCode FillDetailedTrkSummaryHistograms(const std::string CurrentTrackCollection, int h_ind);
  StatusCode FillTrkSummaryHistograms(const std::string CurrentTrackCollection, int h_ind);
  
  StatusCode setupTools();
  StoreGateSvc* m_storeGate;
  std::string m_stream;
 

  /** Cuts: Declared Properties */
  int   m_checkrate, m_nphi_bins;
  float v_maxeta, v_mineta;
  //float v_minpt, v_maxz0, v_minMDThits, v_minRPCphiHits;
  bool  v_applied_cut;
  
  float m_gtrk_minPt, m_gtrk_maxChi2oDoF;
  int   m_gtrk_minPhiHits, m_gtrk_minEtaHits, m_gtrk_minMdtCscHits, m_gtrk_minStations;
  bool  good, good_chi2oDoF, good_pT, good_eta, good_phi, good_nStation;

  /** ToolHandle idHelper*/
  ToolHandle<Muon::MuonIdHelperTool>   m_idHelperTool;
  ToolHandle<Muon::MuonEDMHelperTool> m_helperTool;
  ToolHandle<Muon::IMuonHitSummaryTool> m_muonHitSummaryTool;
  ToolHandle<Trk::ITrackSummaryHelperTool>  m_trackSummaryTool; //<! muon track summary helper 
  
  /** ToolHandle ToFTool*/                          
  ToolHandle<Muon::IMuonTofTool>       m_tofTool; //!<Time of flight tool (handle tof if not coming from IP)
  bool                                 m_DoToFCosmic;   

  /** Mdt Id Helper*/
  const MuonGM::MuonDetectorManager*  m_detMgr;
  const MdtIdHelper*                  m_mdtIdHelper;
  
  /** ToolHandle Trigger Decision Tool*/
  ToolHandle<ITrigDecisionTool> m_trigDecTool;
  
  /** Trigger items */
  std::vector<std::string> m_muon_triggers;
  /** User Flag to switch Trigger on or off */
  bool m_useTrigger;

  /** Trigger related variables */
  std::string m_MuonTriggerChainName;   
  
  /** Pointer to Trk::ITrackSummaryTool */
  ToolHandle< Trk::ITrackSummaryTool > m_trackSumTool;
  const DataVector<Trk::Track>* trks;
  //const TrackCollection* trks_summary;

  /** Track Collections */
  std::string  m_TrackCol;                             //!< holds the name of the track coll to be used
  std::vector<std::string> m_trackCollectionName;      //!< Reconstructed track collection name
  std::vector<int> m_trackCollectionFlag; 
 

  /** Debugging variables */
  mutable MsgStream      m_log;                  //!< Gaudi message stream - msgstream as private member (-> speed)
  bool                   m_debuglevel;           //!< private member to control debug messages
 
   
   
  /** Muon Monitoring Summary Histograms */
  std::vector<TH1F*> muon_numHits_per_Trk;
  std::vector<TH1F*> muon_numHits_per_Event;
  std::vector<TH1F*> muon_numEtaHits_per_Trk;
  std::vector<TH1F*> muon_numPhiHits_per_Trk;
  // std::vector<TH1F*> muon_numTrigHits_per_Trk;
  std::vector<TH1F*> muon_numScatterers_per_Trk;
  //std::vector<TH1F*> muon_numPseudos_per_Trk;
  std::vector<TH1F*> muon_numHoles_per_Trk;
  std::vector<TH2F*> muon_MDThits_per_Trk_vs_Eta;
  std::vector<TH2F*> muon_MDThits_per_Trk_vs_Phi;
  std::vector<TH1F*> muon_numTrks_flaggedSL;
  
  /** Muon Trigger Aware Monitoring Histogram*/
  //TH1F* muon_numEvents_passedTrigger;
  TH2F* muon_numEvents_passedTrigger;
   
  /** Muon Monitoring Histograms */
  std::vector<TProfile*> muon_trk_chi2oDoF;
  std::vector<TProfile*> muon_trk_chi2oDoF_pT;
  std::vector<TProfile*> muon_trk_chi2oDoF_MDThits;
  std::vector<TProfile*> muon_trk_chi2Prob;
  std::vector<TH2F*>     muon_trk_pT_MDThits;
  
  /** Muon X0 and ToF Histograms */     
  std::vector<TH1F*> muon_HitOnTrk_X0;  
  std::vector<TH1F*> muon_HitOnTrk_ToF;   

  /** MuonSpectrometer AllRegions */
  //std::vector<TH1F*> muon_nhits_per_event;
  std::vector<TH1F*> muon_nhits_per_track;

  std::vector<TH1F*> muon_nmdtholes_per_track;
  std::vector<TH1F*> muon_nrpcholes_per_track;
  std::vector<TH1F*> muon_ntgcholes_per_track;
  std::vector<TH1F*> muon_ncscholes_per_track;

  std::vector<TH1F*> muon_ntracks;
  
  std::vector<TH1F*> muon_ngtracks;
  std::vector<TH1F*> muon_gtrk_cutflow;
  std::vector<TH1F*> muon_gtrk_acceptance;
  std::vector<TH2F*> muon_gtrk_eta_phi;
  std::vector<TH1F*> muon_gtrk_eta;
  std::vector<TH1F*> muon_gtrk_phi;
  std::vector<TH1F*> muon_gtrk_d0;
  std::vector<TH1F*> muon_gtrk_z0;
  std::vector<TH1F*> muon_gtrk_numEtaHits;
  std::vector<TH1F*> muon_gtrk_numPhiHits;
  std::vector<TH1F*> muon_gtrk_pT;
  std::vector<TH1F*> muon_gtrk_pT_pos;
  std::vector<TH1F*> muon_gtrk_pT_neg;
  std::vector<TH1F*> muon_gtrk_ChargeRatio_pT;
  std::vector<TProfile*> muon_gtrk_chi2oDoF;

  std::vector<TH1F*> muon_nmdthits_per_track;
  std::vector<TH1F*> muon_nrpchits_per_track;
  std::vector<TH1F*> muon_ntgchits_per_track;
  std::vector<TH1F*> muon_ncschits_per_track;
  
  std::vector<TH2F*> muon_mdt_vs_rpc_hits_per_track;
  std::vector<TH2F*> muon_mdt_vs_tgc_hits_per_track;

  std::vector<TH1F*> muon_noutliers_per_track;

  std::vector<TH1F*> muon_nrpcetahits_per_track;
  std::vector<TH1F*> muon_nrpcphihits_per_track;
  std::vector<TH1F*> muon_ntgcetahits_per_track;
  std::vector<TH1F*> muon_ntgcphihits_per_track;
  std::vector<TH1F*> muon_ncscetahits_per_track;
  std::vector<TH1F*> muon_ncscphihits_per_track;

  /** MDT Hits per track, per phi chamber */
  std::vector<TH2F*> muon_nmdthits_per_track_vs_phi_per_chamber;

  /** MuonSpectrometer Barrel */
  std::vector<TH1F*>     muon_trk_d0_barrel ;
  std::vector<TH1F*>     muon_trk_d0c_barrel;
  std::vector<TH1F*>     muon_trk_z0_barrel;
  std::vector<TProfile*> muon_trk_d0_vs_phi_barrel;
  std::vector<TProfile*> muon_trk_d0_vs_z0_barrel;
  std::vector<TH1F*>     muon_trk_phi0_neg_barrel;
  std::vector<TH1F*>     muon_trk_phi0_pos_barrel;
  std::vector<TH1F*>     muon_trk_phi0_asym_barrel;
  
  /** Small Phi Sector */
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_2_barrel;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_4_barrel;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_6_barrel;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_8_barrel;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_10_barrel;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_12_barrel;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_14_barrel;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_16_barrel;
  /** Large Phi Sector */
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_1_barrel;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_3_barrel;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_5_barrel;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_7_barrel;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_9_barrel;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_11_barrel;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_13_barrel;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_15_barrel;
  
  
  /** MuonSpectrometer EndCap A */
  std::vector<TH1F*>     muon_trk_d0_eca;
  std::vector<TH1F*>     muon_trk_d0c_eca;
  std::vector<TH1F*>     muon_trk_z0_eca;
  std::vector<TProfile*> muon_trk_d0_vs_phi_eca;
  std::vector<TProfile*> muon_trk_d0_vs_z0_eca;
  std::vector<TH1F*>     muon_trk_phi0_neg_eca;
  std::vector<TH1F*>     muon_trk_phi0_pos_eca;
  std::vector<TH1F*>     muon_trk_phi0_asym_eca;
  
  /** Small Phi Sector */
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_2_eca;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_4_eca;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_6_eca;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_8_eca;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_10_eca;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_12_eca;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_14_eca;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_16_eca;
  /** Large Phi Sector */
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_1_eca;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_3_eca;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_5_eca;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_7_eca;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_9_eca;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_11_eca;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_13_eca;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_15_eca;

  /** MuonSpectrometer EndCap C */
  std::vector<TH1F*>     muon_trk_d0_ecc;
  std::vector<TH1F*>     muon_trk_d0c_ecc;
  std::vector<TH1F*>     muon_trk_z0_ecc;
  std::vector<TProfile*> muon_trk_d0_vs_phi_ecc;
  std::vector<TProfile*> muon_trk_d0_vs_z0_ecc;
  std::vector<TH1F*>     muon_trk_phi0_neg_ecc;
  std::vector<TH1F*>     muon_trk_phi0_pos_ecc;
  std::vector<TH1F*>     muon_trk_phi0_asym_ecc;
  
  /** Small Phi Sector */
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_2_ecc;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_4_ecc;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_6_ecc;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_8_ecc;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_10_ecc;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_12_ecc;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_14_ecc;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_16_ecc;
  /** Large Phi Sector */
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_1_ecc;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_3_ecc;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_5_ecc;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_7_ecc;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_9_ecc;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_11_ecc;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_13_ecc;
  std::vector<TProfile*> muon_nmdthits_per_track_vs_eta_PhiSector_15_ecc;

	
};

#endif


