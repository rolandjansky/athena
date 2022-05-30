/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef HIMONITORINGPHOTONSTOOL_H
#define HIMONITORINGPHOTONSTOOL_H


#include "AthenaMonitoring/ManagedMonitorToolBase.h"

class TH1D;
class TH2D_LW;
class AsgPhotonIsEMSelector;

class HIMonitoringPhotonsTool: public ManagedMonitorToolBase
{
public:
  HIMonitoringPhotonsTool(const std::string& type, const std::string& name,
                          const IInterface* parent);

  virtual ~HIMonitoringPhotonsTool();

  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

  void book_hist();
private:
  AsgPhotonIsEMSelector* m_photonTightIsEMSelector;
  AsgPhotonIsEMSelector* m_photonLooseIsEMSelector;

  double m_FCalEt;

  int m_FCalEt_nbins;
  float m_FCalEt_low;
  float m_FCalEt_high;
  //Cherry-picking commit 345b289e on 21.0
  float m_FCalEt_peripheral;
  float m_FCalEt_central;
  //////////
  int m_PhotonPt_nbins;
  float m_PhotonPt_low;
  float m_PhotonPt_high;

  int m_PhotonEta_nbins;
  float m_PhotonEta_low;
  float m_PhotonEta_high;

  int m_PhotonPhi_nbins;
  float m_PhotonPhi_low;
  float m_PhotonPhi_high;

  int m_reta_nbins;
  float m_reta_low;
  float m_reta_high;

  int m_rphi_nbins;
  float m_rphi_low;
  float m_rphi_high;

  int m_weta2_nbins;
  float m_weta2_low;
  float m_weta2_high;

  int m_rhad_nbins;
  float m_rhad_low;
  float m_rhad_high;

  int m_wtots1_nbins;
  float m_wtots1_low;
  float m_wtots1_high;

  int m_fracs1_nbins;
  float m_fracs1_low;
  float m_fracs1_high;

  int m_deltae_nbins;
  float m_deltae_low;
  float m_deltae_high;

  int m_eratio_nbins;
  float m_eratio_low;
  float m_eratio_high;

  int m_f1_nbins;
  float m_f1_low;
  float m_f1_high;

  int m_IsoWide_nbins;
  float m_IsoWide_low;
  float m_IsoWide_high;

  int m_IsoNarrow_nbins;
  float m_IsoNarrow_low;
  float m_IsoNarrow_high;

  /// histograms

  // basics
  TH2D_LW* m_h_photon_pt_eta = nullptr;
  TH2D_LW* m_h_photon_pt_phi = nullptr;
  TH2D_LW* m_h_photon_pt_fcal = nullptr;
  TH2D_LW* m_h_photon_eta_phi_ptCut = nullptr;

  TH2D_LW* m_h_photon_pt_eta_tight = nullptr;
  TH2D_LW* m_h_photon_pt_phi_tight = nullptr;
  TH2D_LW* m_h_photon_pt_fcal_tight = nullptr;
  TH2D_LW* m_h_photon_eta_phi_ptCut_tight = nullptr;

  // shower shapes


  TH1D* m_h_photon_reta_ptCut_all = nullptr;
  TH1D* m_h_photon_rphi_ptCut_all = nullptr;
  TH1D* m_h_photon_weta2_ptCut_all = nullptr;

  TH1D* m_h_photon_rhad_ptCut_all = nullptr;
  TH1D* m_h_photon_wtots1_ptCut_all = nullptr;
  TH1D* m_h_photon_fracs1_ptCut_all = nullptr;

  TH1D* m_h_photon_deltae_ptCut_all = nullptr;
  TH1D* m_h_photon_eratio_ptCut_all = nullptr;
  TH1D* m_h_photon_f1_ptCut_all = nullptr;

  //
  TH1D* m_h_photon_reta_ptCut_loose = nullptr;
  TH1D* m_h_photon_rphi_ptCut_loose = nullptr;
  TH1D* m_h_photon_weta2_ptCut_loose = nullptr;

  TH1D* m_h_photon_rhad_ptCut_loose = nullptr;
  TH1D* m_h_photon_wtots1_ptCut_loose = nullptr;
  TH1D* m_h_photon_fracs1_ptCut_loose = nullptr;

  TH1D* m_h_photon_deltae_ptCut_loose = nullptr;
  TH1D* m_h_photon_eratio_ptCut_loose = nullptr;
  TH1D* m_h_photon_f1_ptCut_loose = nullptr;

  //
  TH1D* m_h_photon_reta_ptCut_tight = nullptr;
  TH1D* m_h_photon_rphi_ptCut_tight = nullptr;
  TH1D* m_h_photon_weta2_ptCut_tight = nullptr;

  TH1D* m_h_photon_rhad_ptCut_tight = nullptr;
  TH1D* m_h_photon_wtots1_ptCut_tight = nullptr;
  TH1D* m_h_photon_fracs1_ptCut_tight = nullptr;

  TH1D* m_h_photon_deltae_ptCut_tight = nullptr;
  TH1D* m_h_photon_eratio_ptCut_tight = nullptr;
  TH1D* m_h_photon_f1_ptCut_tight = nullptr;



  TH2D_LW* m_h_photon_reta_eta_ptCut = nullptr;
  TH2D_LW* m_h_photon_rphi_eta_ptCut = nullptr;
  TH2D_LW* m_h_photon_weta2_eta_ptCut = nullptr;

  TH2D_LW* m_h_photon_rhad_eta_ptCut = nullptr;
  TH2D_LW* m_h_photon_wtots1_eta_ptCut = nullptr;
  TH2D_LW* m_h_photon_fracs1_eta_ptCut = nullptr;

  TH2D_LW* m_h_photon_deltae_eta_ptCut = nullptr;
  TH2D_LW* m_h_photon_eratio_eta_ptCut = nullptr;
  TH2D_LW* m_h_photon_f1_eta_ptCut = nullptr;

  TH2D_LW* m_h_photon_reta_fcal_ptCut_eta0 = nullptr;
  TH2D_LW* m_h_photon_reta_fcal_ptCut_eta1 = nullptr;
  TH2D_LW* m_h_photon_rphi_fcal_ptCut_eta0 = nullptr;
  TH2D_LW* m_h_photon_rphi_fcal_ptCut_eta1 = nullptr;
  TH2D_LW* m_h_photon_weta2_fcal_ptCut_eta0 = nullptr;
  TH2D_LW* m_h_photon_weta2_fcal_ptCut_eta1 = nullptr;
  TH2D_LW* m_h_photon_rhad_fcal_ptCut_eta0 = nullptr;
  TH2D_LW* m_h_photon_rhad_fcal_ptCut_eta1 = nullptr;
  TH2D_LW* m_h_photon_wtots1_fcal_ptCut_eta0 = nullptr;
  TH2D_LW* m_h_photon_wtots1_fcal_ptCut_eta1 = nullptr;
  TH2D_LW* m_h_photon_fracs1_fcal_ptCut_eta0 = nullptr;
  TH2D_LW* m_h_photon_fracs1_fcal_ptCut_eta1 = nullptr;
  TH2D_LW* m_h_photon_deltae_fcal_ptCut_eta0 = nullptr;
  TH2D_LW* m_h_photon_deltae_fcal_ptCut_eta1 = nullptr;
  TH2D_LW* m_h_photon_eratio_fcal_ptCut_eta0 = nullptr;
  TH2D_LW* m_h_photon_eratio_fcal_ptCut_eta1 = nullptr;
  TH2D_LW* m_h_photon_f1_fcal_ptCut_eta0 = nullptr;
  TH2D_LW* m_h_photon_f1_fcal_ptCut_eta1 = nullptr;


  // isolation
  TH2D_LW* m_h_photon_fcal_etcone20_ptCut = nullptr;
  TH2D_LW* m_h_photon_fcal_etcone30_ptCut = nullptr;
  TH2D_LW* m_h_photon_fcal_etcone40_ptCut = nullptr;

  TH2D_LW* m_h_photon_pt_etcone40_fcal0 = nullptr;
  TH2D_LW* m_h_photon_pt_etcone40_fcal1 = nullptr;
  TH2D_LW* m_h_photon_pt_etcone40_fcal2 = nullptr;

  TH1D* m_h_photon_etcone20_ptCut_fcal0 = nullptr;
  TH1D* m_h_photon_etcone20_ptCut_fcal1 = nullptr;
  TH1D* m_h_photon_etcone20_ptCut_fcal2 = nullptr;

  TH1D* m_h_photon_etcone30_ptCut_fcal0 = nullptr;
  TH1D* m_h_photon_etcone30_ptCut_fcal1 = nullptr;
  TH1D* m_h_photon_etcone30_ptCut_fcal2 = nullptr;

  TH1D* m_h_photon_etcone40_ptCut_fcal0 = nullptr;
  TH1D* m_h_photon_etcone40_ptCut_fcal1 = nullptr;
  TH1D* m_h_photon_etcone40_ptCut_fcal2 = nullptr;

  TH1D* m_h_photon_etcone30_ptCut_fcal0_all = nullptr;
  TH1D* m_h_photon_etcone30_ptCut_fcal0_loose = nullptr;
  TH1D* m_h_photon_etcone30_ptCut_fcal0_tight = nullptr;

  TH1D* m_h_photon_etcone30_ptCut_fcal1_all = nullptr;
  TH1D* m_h_photon_etcone30_ptCut_fcal1_loose = nullptr;
  TH1D* m_h_photon_etcone30_ptCut_fcal1_tight = nullptr;

  TH1D* m_h_photon_etcone30_ptCut_fcal2_all = nullptr;
  TH1D* m_h_photon_etcone30_ptCut_fcal2_loose = nullptr;
  TH1D* m_h_photon_etcone30_ptCut_fcal2_tight = nullptr;
};

#endif
