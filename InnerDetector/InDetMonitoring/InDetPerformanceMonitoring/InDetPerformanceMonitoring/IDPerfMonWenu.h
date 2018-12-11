/*
 * Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#ifndef IDPerfMonWenu_H
#define IDPerfMonWenu_H

// **********************************************************************
// IDPerfMonWenu.cxx
// AUTHORS: Beate Heinemann, Tobias Golling, Sara Strandberg
// **********************************************************************

#include <vector>
#include <memory>
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"

class TH1;

class TH1F;
class TH2;
class TH2F;
class TProfile;

class IDPerfMonWenu : public ManagedMonitorToolBase
{

public:

  IDPerfMonWenu(const std::string & type, const std::string & name, const IInterface* parent);

  virtual ~IDPerfMonWenu();

  virtual StatusCode initialize();
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

  void RegisterHisto(MonGroup& mon, TH1* histo, bool doSumw2 = false);
  void RegisterHisto(MonGroup& mon, TH2* histo, bool d0Sumw2 = false);
  void RegisterHisto(MonGroup& mon, TProfile* histo);

  const xAOD::CaloCluster* getLeadingEMcluster(const xAOD::CaloClusterContainer* clusters, const xAOD::CaloCluster* omitCluster = 0) const;
  const xAOD::CaloCluster* getLeadingEMcluster(const xAOD::PhotonContainer* photons, const xAOD::ElectronContainer* electrons, const xAOD::CaloCluster* omitCluster = 0) const;

  const xAOD::TrackParticle* electronTrackMatch(const xAOD::TrackParticleContainer* tracks, const xAOD::CaloCluster* cluster,
					  double dEta = 0.05, double dPhi = 0.1) const;
  double electronTrackMatchEta(const xAOD::TrackParticleContainer* tracks, const xAOD::CaloCluster* cluster,
					  double dEta = 0.05) const;
  double electronTrackMatchPhi(const xAOD::TrackParticleContainer* tracks, const xAOD::CaloCluster* cluster,
					  double dPhi = 0.1) const;
  double InvMass(const xAOD::CaloCluster* EM1, const xAOD::CaloCluster* EM2) const;
  double InvMass(const xAOD::TrackParticle* trk1, const xAOD::TrackParticle* trk2) const;
  double TransMass(const xAOD::CaloCluster* EM, const xAOD::MissingET* met) const;
  double TransMass(const xAOD::TrackParticle* track, const xAOD::MissingET* met) const;
  double deltaR(const xAOD::CaloCluster* cluster, const xAOD::TrackParticle* track) const;
  double signedDeltaPhi(double phi1, double phi2) const;

  int isWenu(const xAOD::CaloCluster* em, const xAOD::CaloCluster* em2, double met) const;

  void makeEffHisto(TH1F* h_num, TH1F* h_denom, TH1F* h_eff);

  int etaRegion(double eta);

  void FillHistosPerCluster(const xAOD::CaloCluster* cluster, const xAOD::TrackParticle* track, int region, float dEta, float dPhi);

 protected:

  int m_histosBooked;

  enum m_eta_region { incl, barrel, eca, ecc, Nregions };
  std::vector<std::string> region_strings;

  // event histos

  TH1F* m_Nevents;

  TH1F* m_Wenu_met_sel;
  TH1F* m_Wenu_transmass_sel;
  TH1F* m_Wenu_trk_transmass_sel;
  TH1F* m_Wenu_trk_transmass_sel_scaled;
  TH1F* m_Wenu_clus_pt;

  // cluster histos

  std::vector<TH1F*> m_Wenu_eta;
  std::vector<TH1F*> m_Wenu_phi;
  std::vector<TH1F*> m_Wenu_deta;
  std::vector<TProfile*> m_Wenu_deta_vs_eta;
  std::vector<TProfile*> m_Wenu_deta_vs_phi;
  std::vector<TProfile*> m_Wenu_absdeta_vs_eta;
  std::vector<TProfile*> m_Wenu_absdeta_vs_phi;
  std::vector<TH1F*> m_Wenu_dphi;
  std::vector<TProfile*> m_Wenu_dphi_vs_eta;
  std::vector<TProfile*> m_Wenu_dphi_vs_phi;
  std::vector<TProfile*> m_Wenu_absdphi_vs_eta;
  std::vector<TProfile*> m_Wenu_absdphi_vs_phi;

  std::vector<TH1F*> m_Wenu_trackmatched_eta;
  std::vector<TH1F*> m_Wenu_trackmatched_phi;
  std::vector<TH1F*> m_Wenu_trackmatched_Eopmatched_eta;
  std::vector<TH1F*> m_Wenu_trackmatched_Eopmatched_phi;
  std::vector<TH1F*> m_Wenu_trackmatched_tightEopmatched_eta;
  std::vector<TH1F*> m_Wenu_trackmatched_tightEopmatched_phi;
  std::vector<TH1F*> m_Wenu_trackmatch_eff_vs_eta;
  std::vector<TH1F*> m_Wenu_trackmatch_eff_vs_phi;
  std::vector<TH1F*> m_Wenu_Eopmatch_eff_vs_eta;
  std::vector<TH1F*> m_Wenu_Eopmatch_eff_vs_phi;
  std::vector<TH1F*> m_Wenu_tightEopmatch_eff_vs_eta;
  std::vector<TH1F*> m_Wenu_tightEopmatch_eff_vs_phi;

  std::vector<TH1F*> m_Wenu_Eop;
  std::vector<TH1F*> m_Wenu_Eop_plus;
  std::vector<TH1F*> m_Wenu_Eop_minus;
  std::vector<TProfile*> m_Wenu_meanEop_vs_p;
  std::vector<TProfile*> m_Wenu_meanEop_vs_invp;
  std::vector<TProfile*> m_Wenu_meanEop_vs_E;
  std::vector<TProfile*> m_Wenu_meanEop_vs_chargedp;
  std::vector<TProfile*> m_Wenu_meanEop_vs_chargedE;
  std::vector<TProfile*> m_Wenu_meanEop_vs_phi;
  std::vector<TProfile*> m_Wenu_meanEop_vs_eta;
  std::vector<TProfile*> m_Wenu_meanEop_vs_p_plus;
  std::vector<TProfile*> m_Wenu_meanEop_vs_invp_plus;
  std::vector<TProfile*> m_Wenu_meanEop_vs_E_plus;
  std::vector<TProfile*> m_Wenu_meanEop_vs_phi_plus;
  std::vector<TProfile*> m_Wenu_meanEop_vs_eta_plus;
  std::vector<TProfile*> m_Wenu_meanEop_vs_p_minus;
  std::vector<TProfile*> m_Wenu_meanEop_vs_invp_minus;
  std::vector<TProfile*> m_Wenu_meanEop_vs_E_minus;
  std::vector<TProfile*> m_Wenu_meanEop_vs_phi_minus;
  std::vector<TProfile*> m_Wenu_meanEop_vs_eta_minus;
  std::vector<TH1F*> m_Wenu_Eop_lt1_gt1;
  std::vector<TH1F*> m_Wenu_Eopdiff;
  std::vector<TProfile*> m_Wenu_Eopdiff_vs_p;
  std::vector<TProfile*> m_Wenu_Eopdiff_vs_invp;
  std::vector<TProfile*> m_Wenu_Eopdiff_vs_E;
  std::vector<TProfile*> m_Wenu_Eopdiff_vs_phi;
  std::vector<TProfile*> m_Wenu_Eopdiff_vs_eta;
  std::vector<TH1F*> m_Wenu_Eop_lt1_vs_eta;
  std::vector<TH1F*> m_Wenu_Eop_lt1_vs_phi;
  std::vector<TH1F*> m_Wenu_Eop_gt1_vs_eta;
  std::vector<TH1F*> m_Wenu_Eop_gt1_vs_phi;
  std::vector<TH1F*> m_Wenu_frac_Eop_lt1_vs_eta;
  std::vector<TH1F*> m_Wenu_frac_Eop_lt1_vs_phi;

  std::vector<TH1F*> m_Wenu_Eop_05_25;
  std::vector<TH1F*> m_Wenu_Eop_15_25;
  std::vector<TH1F*> m_Wenu_frac_Eop_05_25_15_25;

 private:

  //StoreGateSvc* m_storeGate;
  std::string m_stream;
  std::string m_tracksName;
  std::string m_electronsName;
  std::string m_photonsName;
  std::string m_VxPrimContainerName;
  std::string m_emclustersName;
  std::string m_metName;
  std::string m_triggerChainName;
  std::string m_metRefFinalName;
  double m_eoverp_tight_min;
  double m_eoverp_tight_max;
  double m_eoverp_standard_min;
  double m_eoverp_standard_max;
  int m_checkrate;
  bool m_rejectSecondCluster;
  std::string m_electronIDLevel;
  bool m_doIDCuts;
  xAOD::Iso::IsolationType m_isolationCone;
  //Likelihood tool:
  std::unique_ptr<AsgElectronLikelihoodTool> m_LHTool2015; //!

};

#endif
