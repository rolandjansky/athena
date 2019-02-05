/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IDStandardPerformance_H
#define IDStandardPerformance_H

// **********************************************************************
// IDStandardPerformance.cxx
// AUTHORS: Beate Heinemann, Jean-Francois Arguin
// **********************************************************************

#include <vector>
#include <map>
#include <memory>

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"

#include "TrkTrack/TrackCollection.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "GeneratorObjects/HepMcParticleLink.h"

#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"

// // forward declarations
namespace AlignMon {
  class TrackSelectionTool;
}

namespace InDetDD {
  class SCT_DetectorManager;
}

namespace Trk {
  class ITruthToTrack;
  class ITrackSummaryTool;
  class IUpdator;
  class ITrackHoleSearchTool;
  class ITrackSelectorTool;
}

namespace HepPDT {
  class ParticleDataTable;
}

//class TrackCollection;
class TrackTruthCollection;
class McEventCollection;
class HepMcParticleLink;
class JetCollection;

//namespace InDet {
class IDStandardPerformance : public ManagedMonitorToolBase
{

  public:

    IDStandardPerformance( const std::string & type, const std::string & name, const IInterface* parent );

    virtual ~IDStandardPerformance();

    virtual StatusCode initialize();
    virtual StatusCode bookHistograms();
    virtual StatusCode fillHistograms();
    virtual StatusCode procHistograms();

    void MakeTrackPlots(const DataVector<Trk::Track>* trks,const TrackCollection * RecCollection,
	const TrackTruthCollection* TruthMap,const McEventCollection* SimTracks,
	const JetCollection* jetColl);
    void MakeHitPlots(const DataVector<Trk::Track>* trks);

    void MakeDataPlots(const DataVector<Trk::Track>* trks); //CB

    void HitBasedMatching(const DataVector<Trk::Track>* trks,const DataVector<Trk::Track>* pixel_trks,const DataVector<Trk::Track>* sct_trks,const DataVector<Trk::Track>* trt_trks);//CB

    void RegisterHisto(MonGroup &mon, TH1* histo);
    void RegisterHisto(MonGroup &mon, TH2* histo);
    void RegisterHisto(MonGroup &mon, TProfile* histo);

  protected:

    TH1F* m_ngenevent;
    TH1F* m_ntrack;
    TH1F* m_mu;
    TH1F* m_nparticle;
    TH1F* m_nparticleprimary;
    TH1F* m_ntracksel;
    TH1F* m_nparticlesel;
    TH1F* m_nbarcode0;
    TH1F* m_nbarcode0match;
    TH1F* m_nbarcode100k;
    TH1F* m_nbarcodegood;
    TH1F* m_nbadmatch;
    TH1F* m_nlowprob;

    // some truth track parameters
    TH1F* m_truthpT;
    TH1F* m_truthpTlow;
    TH1F* m_truthphi;
    TH1F* m_trutheta;
    TH1F* m_truthd0;
    TH1F* m_truthz0;
    TH1F* m_truthd0c;
    TH1F* m_truthcharge;
    TH1F* m_truthvtx_R;
    TH1F* m_truthvtx_z;

    // Nuclear int.
    TH1F* m_nuclearint_in_E;
    TH1F* m_nuclearint_in_pt;
    TH1F* m_nuclearint_in_phi;
    TH1F* m_nuclearint_in_eta;
    TH1F* m_nuclearint_in_pdgcode;
    TH1F* m_nuclearint_in_endvtx_r;
    TH1F* m_nuclearint_in_endvtx_z;
    TH1F* m_nuclearint_n_out;
    TH1F* m_nuclearint_totalEout;
    TH1F* m_nuclearint_out_E;
    TH1F* m_nuclearint_out_pt;
    TH1F* m_nuclearint_out_dphi;
    TH1F* m_nuclearint_out_deta;
    TH1F* m_nuclearint_out_pdgcode;

    // some rec track parameters
    TH1F* m_recpT;
    TH1F* m_recpTlow;
    TH1F* m_recphi;
    TH1F* m_receta;
    TH1F* m_recd0;
    TH1F* m_recz0;
    TH1F* m_recd0c;
    TH1F* m_recchi2;
    TH1F* m_recdeltaz0;
    TH1F* m_reccharge;

    // BH not yet defined
    TH1F* m_fakepT;
    TH1F* m_fakepTlow;
    TH1F* m_fakephi;
    TH1F* m_fakeeta;
    TH1F* m_faked0;
    TH1F* m_fakez0;
    TH1F* m_faked0c;
    TH1F* m_fakedeltaz0;


    TH1F* m_dr;
    TH1F* m_dtrkphi;
    TH1F* m_dtrketa;

    // 1D histograms used to determine the track parameter resolutions and possbile biases
    std::vector<TH1F*> m_hz0;
    std::vector<TH1F*> m_hd0;
    std::vector<TH1F*> m_hphi;
    std::vector<TH1F*> m_htheta;
    std::vector<TH1F*> m_hz0st;
    std::vector<TH1F*> m_hqopt;

    std::vector<TH1F*> m_pullz0;
    std::vector<TH1F*> m_pulld0;
    std::vector<TH1F*> m_pullphi;
    std::vector<TH1F*> m_pulltheta;
    std::vector<TH1F*> m_pullz0st;
    std::vector<TH1F*> m_pullqopt;

    std::vector<TH1F*> m_hptz0;
    std::vector<TH1F*> m_hptd0;
    std::vector<TH1F*> m_hptphi;
    std::vector<TH1F*> m_hpttheta;
    std::vector<TH1F*> m_hptz0st;
    std::vector<TH1F*> m_hptqopt;


    TH1F* m_pullz0all;
    TH1F* m_pulld0all;
    TH1F* m_pullphiall;
    TH1F* m_pullthetaall;
    TH1F* m_pullqoptall;
    TH1F* m_pullz0all_widerange;
    TH1F* m_pulld0all_widerange;
    TH1F* m_pullphiall_widerange;
    TH1F* m_pullthetaall_widerange;
    TH1F* m_pullqoptall_widerange;


    // summary plots of track parameter resolutions and means versus eta
    TH1F* m_d0_res_vs_eta;
    TH1F* m_phi_res_vs_eta;
    TH1F* m_qopt_res_vs_eta;
    TH1F* m_z0_res_vs_eta;
    TH1F* m_theta_res_vs_eta;
    TH1F* m_z0st_res_vs_eta;

    TH1F* m_d0_mean_vs_eta;
    TH1F* m_phi_mean_vs_eta;
    TH1F* m_qopt_mean_vs_eta;
    TH1F* m_z0_mean_vs_eta;
    TH1F* m_theta_mean_vs_eta;
    TH1F* m_z0st_mean_vs_eta;

    TH1F* m_d0_pullres_vs_eta;
    TH1F* m_phi_pullres_vs_eta;
    TH1F* m_qopt_pullres_vs_eta;
    TH1F* m_z0_pullres_vs_eta;
    TH1F* m_theta_pullres_vs_eta;
    TH1F* m_z0st_pullres_vs_eta;

    TH1F* m_d0_pullmean_vs_eta;
    TH1F* m_phi_pullmean_vs_eta;
    TH1F* m_qopt_pullmean_vs_eta;
    TH1F* m_z0_pullmean_vs_eta;
    TH1F* m_theta_pullmean_vs_eta;
    TH1F* m_z0st_pullmean_vs_eta;

    // summary plots of track parameter resolutions vs pt
    TH1F* m_d0_res_vs_pt;
    TH1F* m_phi_res_vs_pt;
    TH1F* m_qopt_res_vs_pt;
    TH1F* m_z0_res_vs_pt;
    TH1F* m_theta_res_vs_pt;
    TH1F* m_z0st_res_vs_pt;


    TProfile* m_trackeff_vs_eta;
    TProfile* m_trackeff_vs_phi;
    TProfile* m_trackeff_vs_pt;
    TProfile* m_trackeff_vs_pt_lowpt;
    TProfile* m_trackeff_vs_pt_loweta;
    TProfile* m_trackeff_vs_pt_medeta;
    TProfile* m_trackeff_vs_pt_higheta;
    TProfile* m_trackeff_vs_logpt;
    TProfile* m_trackeff_vs_logpt_loweta;
    TProfile* m_trackeff_vs_logpt_medeta;
    TProfile* m_trackeff_vs_logpt_higheta;
    TProfile* m_trackeff_vs_eta_vlowpt;
    TProfile* m_trackeff_vs_eta_lowpt;

    TProfile* m_trackinjeteff_vs_eta;
    TProfile* m_trackinjeteff_vs_phi;
    TProfile* m_trackinjeteff_vs_pt;
    TProfile* m_trackinjeteff_vs_dr;
    TProfile* m_trackinjeteff_vs_dr_lt_j50;
    TProfile* m_trackinjeteff_vs_dr_gr_j100;
    TProfile* m_trackinjeteff_vs_jetet;

    TProfile* m_trackeff_primary_vs_eta;
    TProfile* m_trackeff_primary_vs_phi;
    TProfile* m_trackeff_primary_vs_pt;
    TProfile* m_trackeff_primary_vs_pt_lowpt;
    TProfile* m_trackeff_primary_vs_pt_loweta;
    TProfile* m_trackeff_primary_vs_pt_medeta;
    TProfile* m_trackeff_primary_vs_pt_higheta;
    TProfile* m_trackeff_primary_vs_logpt;
    TProfile* m_trackeff_primary_vs_logpt_loweta;
    TProfile* m_trackeff_primary_vs_logpt_medeta;
    TProfile* m_trackeff_primary_vs_logpt_higheta;
    TProfile* m_trackeff_primary_vs_eta_vlowpt;
    TProfile* m_trackeff_primary_vs_eta_lowpt;

    TProfile* m_trackeff_secondary_vs_eta;
    TProfile* m_trackeff_secondary_vs_phi;
    TProfile* m_trackeff_secondary_vs_pt;
    TProfile* m_trackeff_secondary_vs_eta_vlowpt;
    TProfile* m_trackeff_secondary_vs_eta_lowpt;
    TProfile* m_trackeff_secondary_vs_radius;
    
    //CB for HitBasedMatching
    TH1F * m_nTrks, * m_nPixelTrks, * m_nSCTTrks,* m_nTRTTrks;
    TH1F * m_patternRecoInfo;
    TH1F * m_patternRecoInfo_npat;
    TH2F * m_patternRecoInfo_p1p2;

    TProfile * m_Pix_Track_Pteff;
    TProfile * m_Pix_Track_Ptlow_eff;
    TProfile * m_Pix_Track_Etaeff;
    TProfile * m_Pix_Track_Phieff;

    TProfile * m_SCT_Track_Pteff;
    TProfile * m_SCT_Track_Ptlow_eff;
    TProfile * m_SCT_Track_Etaeff;
    TProfile * m_SCT_Track_Phieff;

    TProfile * m_TRT_Track_Pteff;
    TProfile * m_TRT_Track_Ptlow_eff;
    TProfile * m_TRT_Track_Etaeff;
    TProfile * m_TRT_Track_Phieff;


    // hit efficiencies
    TProfile* m_eff_hit_vs_eta_ibl_barrel;
    TProfile* m_eff_hit_vs_eta_blay_barrel;
    TProfile* m_eff_hit_vs_eta_pix_barrel;
    TProfile* m_eff_hit_vs_eta_sct_barrel;
    TProfile* m_eff_hit_vs_eta_pix_endcap;
    TProfile* m_eff_hit_vs_eta_sct_endcap;

    TProfile* m_frac_hole_vs_eta_ibl_barrel;
    TProfile* m_frac_hole_vs_eta_blay_barrel;
    TProfile* m_frac_hole_vs_eta_pix_barrel;
    TProfile* m_frac_hole_vs_eta_sct_barrel;
    TProfile* m_frac_hole_vs_eta_pix_endcap;
    TProfile* m_frac_hole_vs_eta_sct_endcap;

    TProfile* m_frac_outlier_vs_eta_ibl_barrel;
    TProfile* m_frac_outlier_vs_eta_blay_barrel;
    TProfile* m_frac_outlier_vs_eta_pix_barrel;
    TProfile* m_frac_outlier_vs_eta_sct_barrel;
    TProfile* m_frac_outlier_vs_eta_pix_endcap;
    TProfile* m_frac_outlier_vs_eta_sct_endcap;

    TProfile* m_eff_hit_vs_eta_trt_barrel;
    TProfile* m_frac_hole_vs_eta_trt_barrel;
    TProfile* m_frac_outlier_vs_eta_trt_barrel;

    TProfile* m_eff_hit_vs_eta_trt_endcap;
    TProfile* m_frac_hole_vs_eta_trt_endcap;
    TProfile* m_frac_outlier_vs_eta_trt_endcap;

    TProfile* m_eff_hit_vs_layer_pix_barrel;
    TProfile* m_eff_hit_vs_layer_sct_barrel;

    TProfile* m_chargemisid;

    TProfile* m_track_fakerate_vs_eta;
    TProfile* m_track_fakerate_vs_phi;
    TProfile* m_track_fakerate_vs_d0;
    TProfile* m_track_fakerate_vs_z0;
    TProfile* m_track_fakerate_vs_pt;
    TProfile* m_track_fakerate_vs_deltaz0;
    TProfile* m_track_notruthrate_vs_eta;
    TProfile* m_track_notruthrate_vs_phi;
    TProfile* m_track_notruthrate_vs_d0;
    TProfile* m_track_notruthrate_vs_z0;
    TProfile* m_track_notruthrate_vs_pt;
    TProfile* m_track_badmatchrate_vs_eta;
    TProfile* m_track_badmatchrate_vs_phi;
    TProfile* m_track_badmatchrate_vs_d0;
    TProfile* m_track_badmatchrate_vs_z0;
    TProfile* m_track_badmatchrate_vs_pt;
    TProfile* m_track_badmatchrate_vs_deltaz0;
    // BH not yet
    TProfile* m_track_bcode0rate_vs_eta;
    TProfile* m_track_bcode0rate_matchbc0_vs_eta;
    TProfile* m_track_bcode0rate_matchprim_vs_eta;
    TProfile* m_track_bcode0rate_matchsec_vs_eta;
    TProfile* m_track_bcode0rate_vs_phi;
    TProfile* m_track_bcode0rate_vs_d0;
    TProfile* m_track_bcode0rate_vs_z0;
    TProfile* m_track_bcode0rate_vs_pt;
    TProfile* m_track_bcode0rate_vs_deltaz0;


    TProfile* m_trackinjet_fakerate_vs_eta;
    TProfile* m_trackinjet_fakerate_vs_phi;
    TProfile* m_trackinjet_fakerate_vs_pt;
    TProfile* m_trackinjet_fakerate_vs_dr;
    TProfile* m_trackinjet_fakerate_vs_dr_lt_j50;
    TProfile* m_trackinjet_fakerate_vs_dr_gr_j100;
    TProfile* m_trackinjet_fakerate_vs_jetet;
    TProfile* m_trackinjet_badmatchrate_vs_eta;
    TProfile* m_trackinjet_badmatchrate_vs_phi;
    TProfile* m_trackinjet_badmatchrate_vs_pt;
    TProfile* m_trackinjet_badmatchrate_vs_dr;
    TProfile* m_trackinjet_badmatchrate_vs_dr_lt_j50;
    TProfile* m_trackinjet_badmatchrate_vs_dr_gr_j100;
    TProfile* m_trackinjet_badmatchrate_vs_jetet;
    TProfile* m_trackinjet_bcode0rate_vs_eta;
    TProfile* m_trackinjet_bcode0rate_vs_phi;
    TProfile* m_trackinjet_bcode0rate_vs_pt;
    TProfile* m_trackinjet_bcode0rate_vs_dr;
    TProfile* m_trackinjet_bcode0rate_vs_dr_lt_j50;
    TProfile* m_trackinjet_bcode0rate_vs_dr_gr_j100;
    TProfile* m_trackinjet_bcode0rate_vs_jetet;

    TH1F* m_prob;
    TH1F* m_prob_injet;
    TH1F* m_prob_barcode0;
    TH1F* m_prob_barcode_primary;
    TH1F* m_prob_barcodeMAXBARCODE;
    TH1F* m_nTrkMatchesSameHmpl;
    TH1F* m_nTrkMatchesSameHmpl_barcode0;
    TH1F* m_nTrkMatchesSameHmpl_barcode_primary;
    TH1F* m_nTrkMatchesSameHmpl_barcodeMAXBARCODE;

    // Hit map plots
    TH2F*  m_nHits_pixel_2d;
    TH2F*  m_nHits_blay_2d;
    TH2F*  m_nExpectedHits_blay_2d;
    TH2F*  m_nHits_SCT_2d;
    TH2F*  m_nHits_TRT_2d;
    TH2F*  m_nHits_ALL_RZ;
    TH2F* m_prodvtx_secondaries_RZ;
    TH2F* m_prodvtx_primaries_RZ;

    TH1F* m_num_eventindex_match;
    TH1F* m_Good_num_eventindex_match;
    TH1F* m_Fake_num_eventindex_match;
    TH1F* m_bc0_num_eventindex_match;
    TH1F* m_num_truthmatch_match;
    TH1F* m_Good_num_truthmatch_match;
    TH1F* m_Fake_num_truthmatch_match;
    TH1F* m_bc0_num_truthmatch_match;

    // cluster information
    TH1F * m_cluster_width_phi_pixel;
    TH1F * m_cluster_width_eta_pixel;
    TH1F * m_cluster_width_phi_sct;
    TH2F * m_cluster_z_phi_pixel;
    TH2F * m_cluster_z_phi_sct;
    TH2F * m_cluster_width_phi_pixel_eta;
    TH2F * m_cluster_width_eta_pixel_eta;
    TH2F * m_cluster_width_phi_sct_eta;

    enum detAreaTypes {pix_b, pix_e, sct_b, sct_e, n_detAreaTypes};
    // Track incidence angle cluster information
    TH1F * m_cluster_angle_phi[n_detAreaTypes][4];
    TH1F * m_cluster_angle_eta[n_detAreaTypes][4];

    // residuals
    TH1F* m_residualx_pixel_barrel;
    TH1F* m_residualx_pixel_endcap;
    TH1F* m_residualy_pixel_barrel;
    TH1F* m_residualy_pixel_endcap;
    TH1F* m_residualx_sct_barrel;
    TH1F* m_residualx_sct_endcap;

    std::vector<TH1F*> m_residualx_pixel_barrel_l;
    std::vector<TH1F*> m_residualx_pixel_endcap_d;
    std::vector<TH1F*> m_residualy_pixel_barrel_l;
    std::vector<TH1F*> m_residualy_pixel_endcap_d;

    std::vector<TH1F*> m_residualx_sct_barrel_l;
    std::vector<TH1F*> m_residualx_sct_endcap_d;


    TH1F* m_residualx_trt_barrel;
    TH1F* m_residualx_trt_endcap;

    // residuals by cluster
    TH1F* m_residualx_pixel_barrel_1hit;
    TH1F* m_residualx_pixel_endcap_1hit;
    TH1F* m_residualy_pixel_barrel_1hit;
    TH1F* m_residualy_pixel_endcap_1hit;
    TH1F* m_residualx_sct_barrel_1hit;
    TH1F* m_residualx_sct_endcap_1hit;
    TH1F* m_residualx_pixel_barrel_2ormorehits;
    TH1F* m_residualx_pixel_endcap_2ormorehits;
    TH1F* m_residualy_pixel_barrel_2ormorehits;
    TH1F* m_residualy_pixel_endcap_2ormorehits;
    TH1F* m_residualx_sct_barrel_2ormorehits;
    TH1F* m_residualx_sct_endcap_2ormorehits;

    // residual pulls
    TH1F* m_residualpullx_pixel_barrel;
    TH1F* m_residualpullx_pixel_endcap;
    TH1F* m_residualpully_pixel_barrel;
    TH1F* m_residualpully_pixel_endcap;
    TH1F* m_residualpullx_sct_barrel;
    TH1F* m_residualpullx_sct_endcap;

    std::vector<TH1F*> m_residualpullx_pixel_barrel_l;
    std::vector<TH1F*> m_residualpullx_pixel_endcap_d;
    std::vector<TH1F*> m_residualpully_pixel_barrel_l;
    std::vector<TH1F*> m_residualpully_pixel_endcap_d;

    std::vector<TH1F*> m_residualpullx_sct_barrel_l;
    std::vector<TH1F*> m_residualpullx_sct_endcap_d;

    TH1F* m_residualpullx_trt_barrel;
    TH1F* m_residualpullx_trt_endcap;

    // ****************** residual and pull vs. incident, eta and phi *********************
    TH2F* m_pullPhi_vs_phi_pixelBarrel;
    TH2F* m_pullPhi_vs_phi_pixelEndcap;
    std::vector<TH2F*> m_pullZ_vs_incident_pixel_barrel_l;
    std::vector<TH2F*> m_pullZ_vs_incident_pixel_endcap_d;
    std::vector<TH2F*> m_residualZ_vs_incident_pixel_barrel_l;
    std::vector<TH2F*> m_residualZ_vs_incident_pixel_endcap_d;

    std::vector<TH2F*> m_pullPhi_vs_incident_pixel_barrel_l;
    std::vector<TH2F*> m_pullPhi_vs_incident_pixel_endcap_d;
    std::vector<TH2F*> m_residualPhi_vs_incident_pixel_barrel_l;
    std::vector<TH2F*> m_residualPhi_vs_incident_pixel_endcap_d;

    std::vector<TH2F*> m_pullZ_vs_eta_pixel_barrel_l;
    std::vector<TH2F*> m_pullZ_vs_eta_pixel_endcap_d;
    std::vector<TH2F*> m_residualZ_vs_eta_pixel_barrel_l;
    std::vector<TH2F*> m_residualZ_vs_eta_pixel_endcap_d;

    std::vector<TH2F*> m_pullPhi_vs_eta_pixel_barrel_l;
    std::vector<TH2F*> m_pullPhi_vs_eta_pixel_endcap_d;
    std::vector<TH2F*> m_residualPhi_vs_eta_pixel_barrel_l;
    std::vector<TH2F*> m_residualPhi_vs_eta_pixel_endcap_d;

    std::vector<TH2F*> m_pullZ_vs_phi_pixel_barrel_l;
    std::vector<TH2F*> m_pullZ_vs_phi_pixel_endcap_d;
    std::vector<TH2F*> m_residualZ_vs_phi_pixel_barrel_l;
    std::vector<TH2F*> m_residualZ_vs_phi_pixel_endcap_d;

    std::vector<TH2F*> m_pullPhi_vs_phi_pixel_barrel_l;
    std::vector<TH2F*> m_pullPhi_vs_phi_pixel_endcap_d;
    std::vector<TH2F*> m_residualPhi_vs_phi_pixel_barrel_l;
    std::vector<TH2F*> m_residualPhi_vs_phi_pixel_endcap_d;

    // ****************** residual and pull vs. incident, eta and phi *********************

    // Plots of the residual and pulls in bins of eta
    std::vector< std::vector<TH2F*> > m_residualPhi_vs_phi_pixel_barrel_etaBins;
    std::vector< std::vector<TH2F*> > m_pullPhi_vs_phi_pixel_barrel_etaBins;

    TH2F* m_residualPhi_vs_incident_pixel;
    TH2F* m_pullPhi_vs_incident_pixel;

    TH1F* m_pullMeanPhi_vs_phi_pixelEndcap;
    TH1F* m_pullWidthPhi_vs_phi_pixelEndcap;
    TH1F* m_pullMeanPhi_vs_phi_pixelBarrel;
    TH1F* m_pullWidthPhi_vs_phi_pixelBarrel;

    std::vector<TH1*> m_pullMeanPhi_vs_incident_pixel_barrel_layer;
    std::vector<TH1*> m_pullWidthPhi_vs_incident_pixel_barrel_layer;
    std::vector<TH1*> m_pullMeanPhi_vs_incident_pixel_endcap_disk;
    std::vector<TH1*> m_pullWidthPhi_vs_incident_pixel_endcap_disk;
    std::vector<TH1*> m_residualMeanPhi_vs_incident_pixel_barrel_layer;
    std::vector<TH1*> m_residualWidthPhi_vs_incident_pixel_barrel_layer;
    std::vector<TH1*> m_residualMeanPhi_vs_incident_pixel_endcap_disk;
    std::vector<TH1*> m_residualWidthPhi_vs_incident_pixel_endcap_disk;

    std::vector<TH1*> m_pullMeanPhi_vs_eta_pixel_barrel_layer;
    std::vector<TH1*> m_pullWidthPhi_vs_eta_pixel_barrel_layer;
    std::vector<TH1*> m_pullMeanPhi_vs_eta_pixel_endcap_disk;
    std::vector<TH1*> m_pullWidthPhi_vs_eta_pixel_endcap_disk;
    std::vector<TH1*> m_residualMeanPhi_vs_eta_pixel_barrel_layer;
    std::vector<TH1*> m_residualWidthPhi_vs_eta_pixel_barrel_layer;
    std::vector<TH1*> m_residualMeanPhi_vs_eta_pixel_endcap_disk;
    std::vector<TH1*> m_residualWidthPhi_vs_eta_pixel_endcap_disk;

    std::vector<TH1*> m_pullMeanPhi_vs_phi_pixel_barrel_layer;
    std::vector<TH1*> m_pullWidthPhi_vs_phi_pixel_barrel_layer;
    std::vector<TH1*> m_pullMeanPhi_vs_phi_pixel_endcap_disk;
    std::vector<TH1*> m_pullWidthPhi_vs_phi_pixel_endcap_disk;
    std::vector<TH1*> m_residualMeanPhi_vs_phi_pixel_barrel_layer;
    std::vector<TH1*> m_residualWidthPhi_vs_phi_pixel_barrel_layer;
    std::vector<TH1*> m_residualMeanPhi_vs_phi_pixel_endcap_disk;
    std::vector<TH1*> m_residualWidthPhi_vs_phi_pixel_endcap_disk;

    std::vector<TH1*> m_pullMeanZ_vs_incident_pixel_barrel_layer;
    std::vector<TH1*> m_pullWidthZ_vs_incident_pixel_barrel_layer;
    std::vector<TH1*> m_pullMeanZ_vs_incident_pixel_endcap_disk;
    std::vector<TH1*> m_pullWidthZ_vs_incident_pixel_endcap_disk;
    std::vector<TH1*> m_residualMeanZ_vs_incident_pixel_barrel_layer;
    std::vector<TH1*> m_residualWidthZ_vs_incident_pixel_barrel_layer;
    std::vector<TH1*> m_residualMeanZ_vs_incident_pixel_endcap_disk;
    std::vector<TH1*> m_residualWidthZ_vs_incident_pixel_endcap_disk;

    std::vector<TH1*> m_pullMeanZ_vs_eta_pixel_barrel_layer;
    std::vector<TH1*> m_pullWidthZ_vs_eta_pixel_barrel_layer;
    std::vector<TH1*> m_pullMeanZ_vs_eta_pixel_endcap_disk;
    std::vector<TH1*> m_pullWidthZ_vs_eta_pixel_endcap_disk;
    std::vector<TH1*> m_residualMeanZ_vs_eta_pixel_barrel_layer;
    std::vector<TH1*> m_residualWidthZ_vs_eta_pixel_barrel_layer;
    std::vector<TH1*> m_residualMeanZ_vs_eta_pixel_endcap_disk;
    std::vector<TH1*> m_residualWidthZ_vs_eta_pixel_endcap_disk;

    std::vector<TH1*> m_pullMeanZ_vs_phi_pixel_barrel_layer;
    std::vector<TH1*> m_pullWidthZ_vs_phi_pixel_barrel_layer;
    std::vector<TH1*> m_pullMeanZ_vs_phi_pixel_endcap_disk;
    std::vector<TH1*> m_pullWidthZ_vs_phi_pixel_endcap_disk;
    std::vector<TH1*> m_residualMeanZ_vs_phi_pixel_barrel_layer;
    std::vector<TH1*> m_residualWidthZ_vs_phi_pixel_barrel_layer;

    std::vector<TH1*> m_residualMeanZ_vs_phi_pixel_endcap_disk;
    std::vector<TH1*> m_residualWidthZ_vs_phi_pixel_endcap_disk;

    std::vector< std::vector<TH1F*> > m_residualMeanPhi_vs_phi_pixel_barrel_etaBins;
    std::vector< std::vector<TH1F*> > m_residualWidthPhi_vs_phi_pixel_barrel_etaBins;
    std::vector< std::vector<TH1F*> > m_pullMeanPhi_vs_phi_pixel_barrel_etaBins;
    std::vector< std::vector<TH1F*> > m_pullWidthPhi_vs_phi_pixel_barrel_etaBins;

    TH1F* m_residualMeanPhi_vs_incident_pixel;
    TH1F* m_pullMeanPhi_vs_incident_pixel;
    TH1F* m_residualWidthPhi_vs_incident_pixel;
    TH1F* m_pullWidthPhi_vs_incident_pixel;
    // hit content
    TH1F* m_HitContent_NiblHits;
    TH1F* m_HitContent_NiblOutliers;
    TH1F* m_HitContent_NiblSharedHits;
    TH1F* m_HitContent_NBlayerHits;
    TH1F* m_HitContent_NBlayerOutliers;
    TH1F* m_HitContent_NBlayerSharedHits;
    TH1F* m_HitContent_NPixelHits;
    TH1F* m_HitContent_NPixelHoles;
    TH1F* m_HitContent_NPixelOutliers;
    TH1F* m_HitContent_NPixelContribLayers;
    TH1F* m_HitContent_NPixelSharedHits;
    TH1F* m_HitContent_NPixelGangedHits;
    TH1F* m_HitContent_NPixelGangedHitsFlaggedFakes;
    TH1F* m_HitContent_NSCTHits;
    TH1F* m_HitContent_NSCTHoles;
    TH1F* m_HitContent_NSCTOutliers;
    TH1F* m_HitContent_NSCTDoubleHoles;
    TH1F* m_HitContent_NSCTSharedHits;
    TH1F* m_HitContent_NTRTHits;
    TH1F* m_HitContent_NTRTOutliers;
    TH1F* m_HitContent_NTRTHighThresholdHits;
    TH1F* m_HitContent_NTRTHighThresholdOutliers;

    TH1F* m_Fake_HitContent_NiblHits;
    TH1F* m_Fake_HitContent_NiblOutliers;
    TH1F* m_Fake_HitContent_NiblSharedHits;
    TH1F* m_Fake_HitContent_NBlayerHits;
    TH1F* m_Fake_HitContent_NBlayerOutliers;
    TH1F* m_Fake_HitContent_NBlayerSharedHits;
    TH1F* m_Fake_HitContent_NPixelHits;
    TH1F* m_Fake_HitContent_NPixelHoles;
    TH1F* m_Fake_HitContent_NPixelOutliers;
    TH1F* m_Fake_HitContent_NPixelContribLayers;
    TH1F* m_Fake_HitContent_NPixelSharedHits;
    TH1F* m_Fake_HitContent_NSCTHits;
    TH1F* m_Fake_HitContent_NSCTHoles;
    TH1F* m_Fake_HitContent_NSCTOutliers;
    TH1F* m_Fake_HitContent_NSCTDoubleHoles;
    TH1F* m_Fake_HitContent_NSCTSharedHits;
    TH1F* m_Fake_NSiHit;
    TH2F* m_Fake_NPixvNSct;
    TH1F* m_Fake_Chi2;

    TH1F* m_Good_HitContent_NiblHits;
    TH1F* m_Good_HitContent_NiblOutliers;
    TH1F* m_Good_HitContent_NiblSharedHits;
    TH1F* m_Good_HitContent_NBlayerHits;
    TH1F* m_Good_HitContent_NBlayerOutliers;
    TH1F* m_Good_HitContent_NBlayerSharedHits;
    TH1F* m_Good_HitContent_NPixelHits;
    TH1F* m_Good_HitContent_NPixelHoles;
    TH1F* m_Good_HitContent_NPixelOutliers;
    TH1F* m_Good_HitContent_NPixelContribLayers;
    TH1F* m_Good_HitContent_NPixelSharedHits;
    TH1F* m_Good_HitContent_NSCTHits;
    TH1F* m_Good_HitContent_NSCTHoles;
    TH1F* m_Good_HitContent_NSCTOutliers;
    TH1F* m_Good_HitContent_NSCTDoubleHoles;
    TH1F* m_Good_HitContent_NSCTSharedHits;
    TH1F* m_Good_NSiHit;
    TH2F* m_Good_NPixvNSct;
    TH1F* m_Good_Chi2;

    TProfile* m_HitContent_vs_eta_NiblHits;
    TProfile* m_HitContent_vs_eta_NiblOutliers;
    TProfile* m_HitContent_vs_eta_NiblSharedHits;
    TProfile* m_HitContent_vs_eta_NBlayerHits;
    TProfile* m_HitContent_vs_eta_NBlayerOutliers;
    TProfile* m_HitContent_vs_eta_NBlayerSharedHits;
    TProfile* m_HitContent_vs_eta_NPixelHits;
    TProfile* m_HitContent_vs_eta_NPixelHoles;
    TProfile* m_HitContent_vs_phi_NPixelHoles;
    TProfile* m_HitContent_vs_eta_NPixelOutliers;
    TProfile* m_HitContent_vs_eta_NPixelContribLayers;
    TProfile* m_HitContent_vs_eta_NPixelSharedHits;
    TProfile* m_HitContent_vs_eta_NPixelGangedHits;
    TProfile* m_HitContent_vs_eta_NPixelGangedHitsFlaggedFakes;
    TProfile* m_HitContent_vs_eta_NSCTHits;
    TProfile* m_HitContent_vs_eta_NSCTHoles;
    TProfile* m_HitContent_vs_eta_NSCTOutliers;
    TProfile* m_HitContent_vs_eta_NSCTDoubleHoles;
    TProfile* m_HitContent_vs_eta_NSCTSharedHits;
    TProfile* m_HitContent_vs_eta_NTRTHits;
    TProfile* m_HitContent_vs_eta_NTRTOutliers;
    TProfile* m_HitContent_vs_eta_NTRTHighThresholdHits;
    TProfile* m_HitContent_vs_eta_NTRTHighThresholdOutliers;

    TH1F* m_nTrackpT1_d0siggt3;
    TH1F* m_nTrackpT2_d0siggt3;
    TH1F* m_nTrackpT1_d0siggt5;
    TH1F* m_nTrackpT2_d0siggt5;
    TH1F* m_d0sig_trackpT1;
    TH1F* m_d0sig_trackpT2;
    TH1F* m_d0sig_trackpT5;
    TH1F* m_d0sig_trackpT10;

    TH1F* m_d0sig_pt1_sharedBL;
    TH1F* m_d0sig_pt1_sharedPIX;
    TH1F* m_d0sig_pt1_sharedSCT;

    TH1F* m_nprimperevent;
    TH1F* m_nsecperevent;
    TH1F* m_nprimperevent05;
    TH1F* m_nsecperevent05;



  private:
    TH1F* create_registeredTH1F(MonGroup &mon
	, const char* name
	, const char* title
	, int nBins
	, double min
	, double max);
    TH2F* create_registeredTH2F(MonGroup &mon
	, const char* name
	, const char* title
	, int nBinsX
	, double minX
	, double maxX
	, int nBinsY
	, double minY
	, double maxY);
    TProfile* create_registeredTProfile(MonGroup &mon
	, const char* name
	, const char* title
	, int nBins
	, double min
	, double max
	, double ylow
	, double yup);
    TProfile* create_registeredTProfile(MonGroup &mon
	, const char* name
	, const char* title
	, int nBins
	, double min
	, double max);

    void FillTrackIncidencePlots(detAreaTypes detArea, int phiWidth, int zWidth, float PixTrkAngle, float PixTrkThetaI);

    const Trk::TrackParameters*  getUnbiasedTrackParameters(const Trk::TrackParameters* trkParameters, const Trk::MeasurementBase* measurement );
    void SetSafeMinimumMaximum(TH1* h1, float min, float max);

    float m_Pi;
    const AtlasDetectorID*               m_idHelper;          //!< Used to find out the sub-det from PRD->identify().
    //StoreGateSvc*                      m_storeGate;
    //StoreGateSvc*                      m_detStore;

    const PixelID*                        m_pixelID;
    const SCT_ID*                         m_sctID;
    const TRT_ID*                         m_trtID;

    const InDetDD::PixelDetectorManager*  m_PIX_Mgr;
    const InDetDD::SCT_DetectorManager*   m_SCT_Mgr;

    const HepPDT::ParticleDataTable*      m_particleDataTable; //!< Atlas particle ID table
    ToolHandle<Trk::ITruthToTrack>        m_truthToTrack; //!< tool to create track parameters from a gen particle
    ToolHandle<Trk::ITrackSummaryTool>    m_trkSummaryTool;
    std::string m_stream;
    std::string m_tracksName;
    std::string m_tracksTruthName;
    std::string m_tracksDetailedTruthName;

    ToolHandle<Trk::IResidualPullCalculator>  m_residualPullCalculator;   //!< The residual and pull calculator tool handle
    ToolHandle<Trk::IUpdator>                 m_updatorHandle;            //!< Tool handle of updator for unbiased states
    ToolHandle<Trk::ITrackHoleSearchTool>     m_holeSearchTool;
    ToolHandle<Trk::ITrackSelectorTool >      m_trackSelectorTool;        //!<  Pointer to Trk::ITrackSelectorTool

    bool m_useTrackSelection;
    std::string m_histDirectoryName;
    int m_trackPtBins;
    float m_minTrackPtEff;
    float m_maxTrackPtEff;
    float m_minTrackPtRes;
    int m_trackEtaBins;
    float m_maxTrackEta;
    float m_minTrackEta;
    float m_maxTrackPhi;
    float m_minTrackPhi;
    float m_minProbEff;
    float m_minProbEffLow;
    std::string m_truthParticleName;
    std::string m_truthJetCollName;
    bool m_doTrackInJet;
    bool m_doUpgrade;
    bool m_plotsVsAbsEta;
    //int  m_checkrate;
    bool m_selHardScatter;
    float m_z0cutwrtPrimary;
    bool m_makeHitPlots;
    bool m_doTruth; //CB
    bool m_doHitBasedMatching;
    
    std::string m_PixeltracksName;
    SG::ReadHandleKey<TrackCollection> m_SCTtracksName;
    SG::ReadHandleKey<TrackCollection> m_TRTtracksKey{ this, "TRTtracksName", "StandaloneTRTTracks", "Container name for the Standalone TRT Tracks" };
    SG::ReadHandleKey<xAOD::EventInfo> m_evt{ this, "EvtInfo", "EventInfo", "EventInfo name"};

    typedef std::multimap<HepMcParticleLink,float> recoToTruthMap;//!< map containing reco track and matched truth track barcode
    recoToTruthMap m_rttMap;

    void fillPixelTrackPullHistos(const Identifier& elementID
	, const Trk::TrackStateOnSurface* trackState
	, const std::auto_ptr<const Trk::ResidualPull>& trackPull);


    mutable int       m_isUnbiased;  //!< if can get unbiased residuals
    mutable bool      m_UpdatorWarning; //!< warn only once, if unbiased track states can not be calculated

};
//} // close of namespace
#endif
