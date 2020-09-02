/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISEXAMPLES_JETTAGANA_H
#define ANALYSISEXAMPLES_JETTAGANA_H

/** @class JetTagAna is an example algorithm which shows how to access the
   b-tagging information from JetTag objects.
   @author Laurent Vacavant <vacavant@in2p3.fr>
   Created: 2005/10/11
 */

#include "AthenaBaseComps/AthAlgorithm.h"
#include "CLHEP/Vector/LorentzVector.h"
#include <string>
#include <map>
#include <vector>
#include <sys/types.h>
#include <TH1.h>
#include <TH2.h>
#include "GaudiKernel/ITHistSvc.h"

#include "JetEvent/JetTagInfoBase.h"
#include "ParticleTruth/TrackParticleTruth.h"
#include "ParticleTruth/TrackParticleTruthCollection.h"
#include "AtlasHepMC/GenParticle.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "HepPDT/ParticleDataTable.hh"
#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/ElectronAssociation.h"

//namespace Analysis { class JetTagInfoBase; }
namespace Rec { class TrackParticleContainer; }
namespace Analysis { class MuonContainer; }
class TruthParticleContainer;
class ITHistSvc;
class TH1F;
class Jet;

static const int MAX_numTaggers = 28;
static const int MAX_numHLTTaggers = 13;

class JetTagAna : public AthAlgorithm {

 public:

  JetTagAna(const std::string& name, ISvcLocator* pSvcLocator);
  ~JetTagAna();

  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();

  // helper methods:
  void bookHistograms();
  void computeRejections();
  /** @brief check soft electron **/
  void checkSoftElectron(const std::vector<const JetTagInfoBase*> infoVector, uint iInfo, int ntotal, CLHEP::HepLorentzVector p4, int iflav, const Analysis::ElectronAssociation *ea, int ntrk_j, int nmu_j, const ElectronContainer* electronTDS, const TrackParticleTruthCollection* tpTruthColl);
  /** @brief track quality for soft electrons **/
  StatusCode checkTrackqualforSET(Rec::TrackParticleContainer::const_iterator trackItr,double *pT);
  void getRej(TH1F* u, TH1F* b, 
              double& r8 , double& e8 , double& w8, 
              double& r50, double& e50, double& w50, 
	      double& r60, double& e60, double& w60
	      );
  bool isBHadron(int pdg);
  bool isDHadron(int pdg);
  bool isLightJetIsolated(const CLHEP::HepLorentzVector& jet);
  bool isJetFarFromHPTLepton(const Jet* jetItr); // to suppress influence of W/Z/H decay products

 private:

  std::string m_tagger[MAX_numTaggers]; // number of taggers. Many histograms (below) have same size
  std::string m_jetET[6];

  std::string m_particleJetContainerName;
  std::string m_truthParticleContainerName;
  std::string m_trackParticleContainerName;
  std::string m_primaryVertexContainerName;
  std::string m_mcEventContainerName;
  /// name of the AOD electron container to retrieve from StoreGate
  std::string m_electronContainerName;
 
  /// the AOD muon container to retrieve
  std::string m_muonContainerName;

  int m_selectBFromRun; // use only b-jets from this run number
  int m_selectUFromRun; // ditto for light jets
  double m_purificationDeltaR;       // light jets closer than this dR to b,c,tau are ignored
  double m_leptonPurificationDeltaR; // jets closer than this dR to "high-pt" leptons are ignored

  int m_irun;
  int m_ievt;

  const Rec::TrackParticleContainer*  m_trackTES;
  const TruthParticleContainer*  m_mcpartTES;
  std::vector<double> m_usedSignif;

  ITHistSvc* m_histos; /// histo service
  TH1F* m_h_global_counters;
  TH1F* m_h_global_nprimvtx;
  TH1F* m_h_global_xprimvtx;
  TH1F* m_h_global_yprimvtx;
  TH1F* m_h_global_zprimvtx;
  TH1F* m_h_global_primvtxresx;
  TH1F* m_h_global_primvtxresy;
  TH1F* m_h_global_primvtxresz;
  TH1F* m_h_global_nmcpart;
  TH1F* m_h_global_ntrkpart;
  TH1F* m_h_global_BLayerHits;
  TH1F* m_h_global_BLayerSharedHits;
  TH1F* m_h_global_PixelHits;
  TH1F* m_h_global_PixelLayers;
  TH1F* m_h_global_SiHits;
  TH1F* m_h_global_TRTHits;
  TH1F* m_h_global_nmuon;
  TH1F* m_h_global_nelectron;
  TH1F* m_h_global_njettag;
  TH1F* m_h_truth_bquark_nb;
  TH1F* m_h_truth_bquark_pt;
  TH1F* m_h_truth_bquark_eta;
  TH1F* m_h_truth_cquark_nb;
  TH1F* m_h_truth_cquark_pt;
  TH1F* m_h_truth_cquark_eta;
  TH1F* m_h_truth_bhadr_nb;
  TH1F* m_h_truth_bhadr_pt;
  TH1F* m_h_truth_bhadr_eta;
  TH1F* m_h_truth_chadr_nb;
  TH1F* m_h_truth_chadr_pt;
  TH1F* m_h_truth_chadr_eta;
  TH1F* m_h_jet_ntotal;
  TH1F* m_h_jet_label;
  TH1F* m_h_jet_nlabelb;
  TH1F* m_h_jet_nlabelc;
  TH1F* m_h_jet_nlabelt;
  TH1F* m_h_jet_ntag;
  TH1F* m_h_jet_eta;
  TH1F* m_h_jet_phi;
  TH1F* m_h_jet_et;
  TH1F* m_h_jet_ntracks;
  TH1F* m_h_jet_tracks_pt;
  TH1F* m_h_jet_tracks_BLayerHits;
  TH1F* m_h_jet_tracks_PixelHits;
  TH1F* m_h_jet_tracks_SCTHits; 
  TH1F* m_h_jet_nmuons;
  TH1F* m_h_jet_muons_pt;
  TH1F* m_h_jet_nelectrons;
  TH1F* m_h_jet_electrons_pt;
  TH1F* m_h_tag_jetprob_w;
  TH1F* m_h_tag_i2d_w;
  TH1F* m_h_tag_i2d_n;
  TH1F* m_h_tag_i2d_b;
  TH1F* m_h_tag_i2d_u;
  TH1F* m_h_tag_i2d_sig;
  TH1F* m_h_tag_i3d_w;
  TH1F* m_h_tag_i3d_n;
  TH1F* m_h_tag_i3d_b;
  TH1F* m_h_tag_i3d_u;
  TH1F* m_h_tag_i3d_sig;
  TH1F* m_h_tag_sv1_w;
  TH1F* m_h_tag_sv1_b;
  TH1F* m_h_tag_sv1_u;
  TH1F* m_h_tag_sv2_w;
  TH1F* m_h_tag_sv2_b;
  TH1F* m_h_tag_sv2_u;
  TH1F* m_h_tag_sv_n;
  TH1F* m_h_tag_sv_n2t;
  TH1F* m_h_tag_sv_frc;
  TH1F* m_h_tag_sv_m;
  TH1F* m_h_tag_cmb_w;
  TH1F* m_h_tag_lf2d_w;
  TH1F* m_h_tag_svbu_w;
  TH1F* m_h_tag_lhsig_w;
  TH1F* m_h_tag_softm_w;
  TH1F* m_h_tag_softe_w;
  TH1F* m_h_tag_jetfitter_w;
  TH1F* m_h_tag_jetfitcomb_w;
  TH1F* m_h_tag_jetfitternn_w;
  TH1F* m_h_tag_jetfitcombnn_w;
  //*
  TH1F* m_h_tag_i2d_l2_w;
  TH1F* m_h_tag_i3d_l2_w;
  TH1F* m_h_tag_i2d_ef_w;
  TH1F* m_h_tag_i3d_ef_w;
  TH1F* m_h_tag_i2d_of_w;
  TH1F* m_h_tag_i3d_of_w;

  TH1F* m_h_tag_IPinfo_ntrk[6];
  TH1F* m_h_tag_IPinfo_trkPt[6];
  TH1F* m_h_tag_IPinfo_d0val[6];
  TH1F* m_h_tag_IPinfo_z0val[6];
  TH1F* m_h_tag_IPinfo_d0sig[6];
  TH1F* m_h_tag_IPinfo_z0sig[6];
  TH1F* m_h_tag_IPinfo_weight2D[6];
  TH1F* m_h_tag_IPinfo_weight3D[6];
  TH1F* m_h_tag_IPinfo_piJP[6]; 
  TH1F* m_h_tag_IPinfo_fromV0[6];
  TH1F* m_h_tag_IPinfo_grade[6];

  TH1F* m_h_perf_b[MAX_numTaggers];
  TH1F* m_h_perf_u[MAX_numTaggers];
  TH1F* m_h_perf_upur[MAX_numTaggers];
  TH1F* m_h_perf_vxeff_den_b;
  TH1F* m_h_perf_vxeff_num_b;
  TH1F* m_h_perf_vxeff_den_u;
  TH1F* m_h_perf_vxeff_num_u;
  TH1F* m_h_perf_vxeff_den_upur;
  TH1F* m_h_perf_vxeff_num_upur;
  TH1F* m_h_perf_rej10;
  TH1F* m_h_perf_rej50;
  TH1F* m_h_perf_rej60;
  TH1F* m_h_perf_rejpur10;
  TH1F* m_h_perf_rejpur50;
  TH1F* m_h_perf_rejpur60;

  // histograms contain weights (for different taggers) for 50% and 60% eff. points
  TH1F* m_h_perfWt_10;
  TH1F* m_h_perfWt_50;
  TH1F* m_h_perfWt_60;
  TH1F* m_h_perfWt_pur10;
  TH1F* m_h_perfWt_pur50;
  TH1F* m_h_perfWt_pur60;

  // histograms used to calculate eff/rej as a function of ET/eta/phi 
  // for the various taggers. Same dimension as m_tagger[]
  //
  TH2F* m_h_perf_b_ET[MAX_numTaggers];
  TH2F* m_h_perf_b_eta[MAX_numTaggers];
  TH2F* m_h_perf_b_phi[MAX_numTaggers];

  TH2F* m_h_perf_u_ET[MAX_numTaggers];
  TH2F* m_h_perf_u_eta[MAX_numTaggers];
  TH2F* m_h_perf_u_phi[MAX_numTaggers];

  TH2F* m_h_perf_upur_ET[MAX_numTaggers];
  TH2F* m_h_perf_upur_eta[MAX_numTaggers];
  TH2F* m_h_perf_upur_phi[MAX_numTaggers];

  TH1F* m_h_eff_b_ET[MAX_numTaggers];
  TH1F* m_h_eff_b_eta[MAX_numTaggers];
  TH1F* m_h_eff_b_phi[MAX_numTaggers];
  TH1F* m_h_rej_u_ET[MAX_numTaggers];
  TH1F* m_h_rej_u_eta[MAX_numTaggers];
  TH1F* m_h_rej_u_phi[MAX_numTaggers];
  TH1F* m_h_rej_upur_ET[MAX_numTaggers];
  TH1F* m_h_rej_upur_eta[MAX_numTaggers];
  TH1F* m_h_rej_upur_phi[MAX_numTaggers];
 
  std::string m_TPTruthContainer;
  // tag ntuple variables:
  static const int m_maxjet = 80;
 
  //int m_eventNumber;
  //int m_runNumber;

  //
  int   m_njet;
  float m_jet_phi[m_maxjet];
  float m_jet_eta[m_maxjet];
  float m_jet_et[m_maxjet]; 
  int   m_jet_flav[m_maxjet];
  int   m_jet_isol[m_maxjet];

  int   m_jet_ne[m_maxjet];
  int   m_jet_eTrueEle[m_maxjet],m_jet_eAnyEle[m_maxjet];
  float m_jet_eptr[m_maxjet], m_jet_ept[m_maxjet], m_jet_ea0[m_maxjet];
  float m_jet_ew[m_maxjet], m_jet_elh[m_maxjet],m_jet_epb[m_maxjet], m_jet_epu[m_maxjet];
  
  int m_jet_tag_pid[m_maxjet];
  int m_jet_tag_mothpid[m_maxjet];
  int m_jet_NtrackEle[m_maxjet];
  float m_jet_trackElept[m_maxjet][10],m_jet_trackEle[m_maxjet][10];
  int   m_jet_Ntrack[m_maxjet];
  float m_jet_sumptTrack[m_maxjet];
  int m_nelej[m_maxjet];
  int m_ntrackj[m_maxjet];
  int m_nmuonj[m_maxjet];
  //
  TTree* p_nt1;
  bool m_UseTupleSET;

  //
  double getTotWeight(double,double);
  const HepMC::GenParticle* getTruth(const Rec::TrackParticle* myTrackParticle,const TrackParticleTruthCollection* mcpartTES,const Rec::TrackParticleContainer* trackTES);
  //const HepPDT::ParticleDataTable* m_particleTable;

};

#endif // ANALYSISEXAMPLES_JETTAGANA_H

