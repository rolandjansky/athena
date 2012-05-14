// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file SUSYTools/SUSYTools/SUSYObjDef.h
 * @author Renaud Bruneliere <Renaud.Bruneliere@cern.ch>
 * @date Feb, 2011
 * @brief Baseline SUSY object definitions.
 */

#ifndef SUSYTOOLS_SUSYOBJDEF_H
#define SUSYTOOLS_SUSYOBJDEF_H

#include <vector>
#include <string>
#include "TVector2.h"
#include "TLorentzVector.h"
#include "egammaAnalysisUtils/checkOQ.h"
#include "egammaAnalysisUtils/EnergyRescaler.h"
#include "egammaAnalysisUtils/egammaSFclass.h"
//#include "MuonMomentumCorrections/SmearingClass.h"
//#include "MuonEfficiencyCorrections/StacoCBScaleEffFactors.h"
//#include "MuonEfficiencyCorrections/StacoLooseScaleEffFactors.h"
#include "JetSelectorTools/TJetCleaningTool.h"
#include "SUSYTools/JetID.hpp"

class Container {
public:
  double pt, phi, eta;
  int index;

  inline bool operator > (const Container & other) const
  {
    if (pt>other.pt) return true;
    return false;
  }
  inline bool operator < (const Container & other) const
  {
    if (pt<other.pt) return true;
    return false;
  }
};

class SUSYObjDef : public TNamed
{
public:
  SUSYObjDef();
  ~SUSYObjDef();

  /// You should always call this function
  /// mm_corr_dir = path to muon momentum corrections
  ///               default path is $ROOTCOREDIR/../MuonMomentumCorrections/share/
  /// jetSelectorTools_file = JetSelectorTools python steering file
  ///                         default file is $ROOTCOREDIR/../JetSelectorTools/python/ConfiguredTJetCleaningTools.py
  /// jetSelectorTools_cfg = python method use to configure jet cleaning cuts, default are Loose cleaning cuts
  void initialize(std::string mm_corr_dir = "",
                  std::string jetSelectorTools_file = "",
                  std::string jetSelectorTools_cfg = "ConfiguredTJetCleaningTool_Loose()");

  /// You should always call this function
  void finalize();

  /// Clear internal variables (this method should be called once per event)
  void Reset();

  /// Baseline SUSY Electron object definition
  /// Variable used for electron smearing systematic: int value 0:nominal, 1: smearcorr_down, 2: smearcorr_up
  bool IsElectron(int iEl, int RunNumber,
                  float el_cl_eta, float el_cl_phi, float el_cl_E, float el_tracketa, float el_trackphi,
                  int el_author, unsigned int el_isEM, unsigned int el_OQ, int el_nPixHits, int el_nSCTHits,
                  bool isData, float etcut = 20000., float etacut = 2.47, bool ischeckOQ = true, int value = 0);
  /// Additionnal SUSY Electron criteria for signal selection: relative isolation cut on ptcone20/pt
  /// In order to be on plateau of e20_medium, offline cut is set to 25 GeV
  bool IsSignalElectron(int iEl, unsigned int el_isEM, float el_ptcone20, 
                        float etcut = 25000., float isocut = 0.1);
  /// Electron crack veto
  bool IsInCrack(float el_cl_eta) { return (fabs(el_cl_eta) > 1.37 && fabs(el_cl_eta) < 1.52); }
  /// Electron helper functions
  void SetElecTLV(int iEl,
                  float el_cl_eta, float el_cl_phi, float el_cl_E, float el_tracketa, float el_trackphi,
                  int el_nPixHits, int el_nSCTHits, bool isData, int value);
  TLorentzVector  GetElecTLV()        { return m_el[m_iEl]; }
  TLorentzVector& GetElecTLV(int iEl) { return m_el[iEl]; }

  /// Baseline SUSY Muon object definition
  /// Variable used for muon smearing systematic: THESTRING {"", "MSLOW", "MSUP", "IDLOW", "IDUP"}
  bool IsMuon(int iMu,
              float mu_staco_pt, float mu_staco_eta, float mu_staco_phi, float mu_staco_E,
              float mu_staco_me_qoverp_exPV, float mu_staco_id_qoverp_exPV,
              float mu_staco_me_theta_exPV, float mu_staco_id_theta_exPV, float mu_staco_id_theta,
              int mu_staco_isCombinedMuon, int mu_staco_isLowPtReconstructedMuon,
              int mu_staco_loose, int mu_staco_expectBLayerHit, int mu_staco_nBLHits, int mu_staco_nPixHits,
              int mu_staco_nPixelDeadSensors, int mu_staco_nPixHoles, int mu_staco_nSCTHits,
              int mu_staco_nSCTDeadSensors, int mu_staco_nSCTHoles, int mu_staco_nTRTHits, int mu_staco_nTRTOutliers,
              bool isData, float ptcut = 10000., float etacut = 2.4, std::string THESTRING = "");
  /// Additionnal SUSY Signal Muon criteria: absolute isolation cut on ptcone20
  /// In order to be on plateau of mu18, offline cut is set to 20 GeV
  bool IsSignalMuon(int iMu, float mu_staco_ptcone20, float ptcut = 20000., float isocut = 1800.);
  /// Cosmic rejection cuts
  /// WARNING: as method is named, it returns TRUE if |z0PV|>1mm OR |d0PV|>0.2mm, i.e. if a cosmic candidate is found
  bool IsCosmicMuon(float mu_staco_z0_exPV, float mu_staco_d0_exPV, float z0cut = 1., float d0cut = 0.2);
  /// Muon quality rejection cuts at the moment based on sigma(q/p)/|q/p|
  /// This cut should be checked on every muon passing IsMuon and before overlap removal
  /// If a muon is bad (method returns TRUE), then you should reject the event (Etmiss computation cannot be trusted)
  bool IsBadMuon(float mu_staco_qoverp_exPV, float mu_staco_cov_qoverp_exPV, float qopcut = 0.2);
  /// Muon helper functions
  void SetMuonTLV(int iMu,
                  float mu_staco_pt, float mu_staco_eta, float mu_staco_phi, float mu_staco_E,
                  float mu_staco_me_qoverp_exPV, float mu_staco_id_qoverp_exPV,
                  float mu_staco_me_theta_exPV, float mu_staco_id_theta_exPV, 
                  int mu_staco_isCombinedMuon, bool isData, std::string THESTRING);
  TLorentzVector  GetMuonTLV()        { return m_mu[m_iMu]; }
  TLorentzVector& GetMuonTLV(int iMu) { return m_mu[iMu]; }

  /// Check if an offline muon is matched to the muon used by trigger
  bool MuonHasTriggerMatch( unsigned int mItr, int mu_staco_n, int trig_EF_trigmuonef_n, vector<int> *trig_EF_trigmuonef_track_n, vector<float> *mu_staco_pt, vector<float> *mu_staco_eta, vector<float> *mu_staco_phi, vector<vector<float> > *trig_EF_trigmuonef_track_CB_pt, vector<vector<float> > *trig_EF_trigmuonef_track_CB_eta, vector<vector<float> > *trig_EF_trigmuonef_track_CB_phi, vector<vector<int> > *trig_EF_trigmuonef_track_CB_hasCB);//, float &onlpt, float &onleta, float &onlphi );

  /// SUSY Jet object definition
  bool IsJet(int iJet, float jet_AntiKt4TopoNewEM_pt, float jet_AntiKt4TopoNewEM_eta, float jet_AntiKt4TopoNewEM_phi,
             float jet_AntiKt4TopoNewEM_E, float jet_AntiKt4TopoNewEM_emscale_eta, 
             float jet_AntiKt4TopoNewEM_emfrac, float jet_AntiKt4TopoNewEM_hecf,
             float jet_AntiKt4TopoNewEM_LArQuality, float jet_AntiKt4TopoNewEM_HECQuality, 
             float jet_AntiKt4TopoNewEM_Timing, float jet_AntiKt4TopoNewEM_sumPtTrk, 
             float jet_AntiKt4TopoNewEM_fracSamplingMax,
             float jet_AntiKt4TopoNewEM_NegativeE, bool isData, float ptcut = 20000., float etacut = 2.8,
             JetID::BadJetCategory criteria = JetID::LooseBad);
  /// Jet helper functions
  void SetJetTLV(int iJet, float jet_AntiKt4TopoNewEM_pt, float jet_AntiKt4TopoNewEM_eta, float jet_AntiKt4TopoNewEM_phi,
                 float jet_AntiKt4TopoNewEM_E);
  TLorentzVector  GetJetTLV()         { return m_jet[m_iJet]; }
  TLorentzVector& GetJetTLV(int iJet) { return m_jet[iJet]; }

  /// SUSY B-Jet object definition
  /// jet_AntiKt4TopoNewEM_flavor_weight_xxx should be replaced by [SV0, JetFitterCOMBNN, Comb, JetProb]
  /// corresponding to jetTagger name.
  /// op is operating point for a given tagger. You can find the different thresholds on:
  /// https://twiki.cern.ch/twiki/bin/view/AtlasProtected/Analysis16
  /// By default (when op=999), official recommendation is used
  bool IsBJet(float jet_AntiKt4TopoNewEM_flavor_weight_xxx, std::string jetTagger = "SV0", float op = 999.);

  /// Basic LAr hole veto
  bool IsLArHoleVeto(float jet_AntiKt4TopoNewEM_pt, float jet_AntiKt4TopoNewEM_eta, float jet_AntiKt4TopoNewEM_phi,
                     float jet_AntiKt4TopoNewEM_BCH_CORR_JET, float jet_AntiKt4TopoNewEM_BCH_CORR_CELL, 
                     bool kIsData, float ptcut = 40000., int RunNumber = 180614);

  /// Is an object pointing in the LAr hole
  bool IsLArHole(float eta, float phi, 
                 float etamin = -0.1, float etamax = 1.5, 
                 float phimin = -0.9, float phimax = -0.5) {
    if (eta < etamin || eta > etamax) return false;
    if (phi < phimin || phi > phimax) return false;
    return true;
  }

  /// Etmiss computation: requires a list on muon indices to include in Etmiss. 
  /// In version SUSYD3PDMaker-00-08-07, tag p428, RefJet term need to be recomputed because jets with
  /// negative energy are included by default => redoRefJet = true
  /// In version SUSYD3PDMaker-00-09-03 or SUSYD3PDMaker-00-09-04, RefJet term is computed properly => redoRefJet = false
  /// Warning: it requires:
  ///  - m_mu array to be filled (muon selection applied) before calling this function
  ///  - m_el array to be filled (baseline electron selection should be applied with a 10 GeV pt veto) before calling 
  ///    this function to perform overlap removal between jet and electrons
  TVector2 GetMET(float MET_Simplified20_RefFinal_etx, float MET_Simplified20_RefFinal_ety,
                  float MET_Simplified20_RefJet_etx, float MET_Simplified20_RefJet_ety,
                  float MET_Simplified20_Muon_Total_Staco_etx, float MET_Simplified20_Muon_Total_Staco_ety,
                  std::vector<int> vmuidx, std::vector<int> velidx, bool redoRefJet = false);
  TVector2 GetFixedMet(float MET_Simplified20_RefFinal_calc_etx, float MET_Simplified20_RefFinal_calc_ety,
                       float MET_Simplified20_RefJet_etx, float MET_Simplified20_RefJet_ety,
                       float MET_Simplified20_RefEle_etx, float MET_Simplified20_RefEle_ety,
                       std::vector< TLorentzVector >* jet_AntiKt4TopoNewEM = NULL /* all units are MeV */);

  /// Primary vertex cut
  bool IsGoodVertex(std::vector<int>* vx_nTracks);

  /// Method used as python wrapper to get tight electron reco efficiency (and uncertainty)
  /// mode = 0 both reconstruction and identification scale factors are applied (default)
  ///      = 1 only reconstruction scale factor is applied
  ///      = 2 only identification scale factor is applied
  /// Tight (set=2) in release 15 (rel=0) / release 16 (rel=1)  / 
  /// release 16.6 estimated from 2010 data (rel=2)  / 
  /// release 16.6 estimated from 2011 data "EPS recommendations" (rel=3) /
  /// release 16.6 estimated from 2011 data "EPS recommendations" including Jpsi measurements (rel=4)
  /// measured with probes in the 20-50 GeV range (range=0) or 30-50 GeV (range=1) and correcting (etcorrection=1)
  /// or not (etcorrection=0) for the ET-dependence
  /// el_cl_et := cluster_E/cosh(track_eta)
  float GetSignalElecSF(float el_cl_eta, float el_cl_et, int rel = 4, int mode = 0) { 
    float sf = 1.;
    if (mode == 0 || mode == 1) sf *= m_egammaSFclass.scaleFactorRecoTrkQual(el_cl_eta,el_cl_et).first;
    if (mode == 0 || mode == 2) sf *= m_egammaSFclass.scaleFactorTightETcorrected(el_cl_eta,el_cl_et,rel).first;
    return sf; 
  }
  float GetSignalElecSFUnc(float el_cl_eta, float el_cl_et, int rel = 4, int mode = 0) { 
    float sfUnc = 0.;
    if (mode == 0 || mode == 1) sfUnc = m_egammaSFclass.scaleFactorRecoTrkQual(el_cl_eta,el_cl_et).second;
    if (mode == 0 || mode == 2) sfUnc = sqrt(sfUnc*sfUnc + m_egammaSFclass.scaleFactorTightETcorrected(el_cl_eta,el_cl_et,rel).second*m_egammaSFclass.scaleFactorTightETcorrected(el_cl_eta,el_cl_et,rel).second);
    return sfUnc;
  }
  float GetTrigElecSF(float el_cl_eta) { 
    return m_egammaSFclass.scaleFactorTrigger(el_cl_eta).first; 
  }
  float GetTrigElecSFUnc(float el_cl_eta) { 
    return m_egammaSFclass.scaleFactorTrigger(el_cl_eta).second; 
  }

  /// Method used as python wrapper to get muon reco efficiency (and uncertainty)
  /// We assume that isolation does not introduce an additionnal bias
  float GetSignalMuonSF(int iMu, bool isCBOnly = false);
  float GetSignalMuonSFUnc(int iMu, bool isCBOnly = false, int systType = 0);

  // Methods to retrieve the different tools
  egammaOQ&               GetegammaOQ()       { return m_egammaOQ; }
  eg2011::EnergyRescaler& GetEnergyRescaler() { return m_eRescale; }
  egammaSFclass&          GetegammaSFclass()  { return m_egammaSFclass; }
  SmearingClass*          GetSmearingClass()  { return m_mcp_smear; }

private:
  /// Array of TLorentzVector which can be used later in analysis (like for Etmiss computation)
  TLorentzVector m_el[100];
  TLorentzVector m_mu[100];
  TLorentzVector m_jet[300];
  /// Index of last object in array
  int m_iEl;
  int m_iMu;
  int m_iJet;

  /// Class used to detect dead material in LAr
  egammaOQ m_egammaOQ;
  /// Class used to rescale electron energy in data and smear electron energy in MC
  eg2011::EnergyRescaler m_eRescale;
  /// Class used to smear muon energy in MC
  SmearingClass* m_mcp_smear; 
  /// Class used to correct MC electron reco efficiencies
  egammaSFclass  m_egammaSFclass;
  /// Class used to rescale MC muon reco efficiencies
  Analysis::StacoCBScaleEffFactors    m_StacoCBSCF;
  Analysis::StacoLooseScaleEffFactors m_StacoLooseSCF;
  /// Class used to apply jet cleaning
  Root::TJetCleaningTool* m_JetCleanTool;

  #ifndef SUSY_ATHENA
  ClassDef(SUSYObjDef,1)
  #endif  
};

#endif // not SUSYTOOLS_SUSYOBJDEF_H
