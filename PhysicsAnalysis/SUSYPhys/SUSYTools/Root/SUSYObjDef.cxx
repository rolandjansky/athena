/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file SUSYTools/src/SUSYObjDef.cxx
 * @author Renaud Bruneliere <Renaud.Bruneliere@cern.ch>
 * @date Feb, 2011
 * @brief Baseline SUSY object definitions.
 *
 * Example usage:
 * if (mySUSYObj.IsElectron(...)) myeltlv = GetElecTLV();
 * if (mySUSYObj.IsMuon(...)) mymutlv = GetMuonTLV();
 * 
 */

#include <cstdlib>
#include "TPython.h"

#include "SUSYTools/SUSYObjDef.h"
#include "SUSYTools/egammaPIDdefs.h"

SUSYObjDef::SUSYObjDef() : m_mcp_smear(0), m_JetCleanTool(0)
{
  Reset();
}

SUSYObjDef::~SUSYObjDef()
{
  if (m_mcp_smear) delete m_mcp_smear;
}

void SUSYObjDef::initialize(std::string mm_corr_dir, std::string jetSelectorTools_file,
                            std::string jetSelectorTools_cfg)
{
  // m_egammaOQ.initialize(OQpath); // Obsolete
  m_eRescale.useDefaultCalibConstants("2011");
  // do muon scale correction as well, values are based on MCP Twikipage
  if (!m_mcp_smear) {
    if (mm_corr_dir == "") { // Default path
      char *tmparea=getenv("ROOTCOREDIR");
      if (tmparea != NULL) mm_corr_dir = tmparea;
      mm_corr_dir = mm_corr_dir + "/../MuonMomentumCorrections/share/"; 
    }
    m_mcp_smear = new SmearingClass("staco","q_pT",mm_corr_dir);
    m_mcp_smear->UseScale(1);
  }
  // Initialize jet cleaning tool
  // Do not use at the moment because python access is causing a crash when athena is setup
//  if (!m_JetCleanTool) {
//    if (jetSelectorTools_file == "") { // Default path
//      char *tmparea=getenv("ROOTCOREDIR");
//      if (tmparea != NULL) jetSelectorTools_file = tmparea;
//      jetSelectorTools_file = jetSelectorTools_file + "/../JetSelectorTools/python/ConfiguredTJetCleaningTools.py"; 
//    }
//    TPython::LoadMacro(jetSelectorTools_file.c_str());
//    m_JetCleanTool = static_cast<Root::TJetCleaningTool*>((void*)TPython::Eval(jetSelectorTools_cfg.c_str()));
//    m_JetCleanTool->initialize();
//  }
  // Reset internal variables
  Reset();
}

void SUSYObjDef::finalize()
{
  if (m_JetCleanTool) m_JetCleanTool->finalize();
}

void SUSYObjDef::Reset()
{
  m_iEl = m_iMu = m_iJet = 0;
  for (unsigned i = 0; i < 100; i++) {
    m_el[i].SetPxPyPzE(0.,0.,0.,0.);
    m_mu[i].SetPxPyPzE(0.,0.,0.,0.);
  }
  for (unsigned i = 0; i < 300; i++)
    m_jet[i].SetPxPyPzE(0.,0.,0.,0.);
}

bool SUSYObjDef::IsElectron(int iEl,
                            int RunNumber,
                            float el_cl_eta,
                            float el_cl_phi,
                            float el_cl_E,
                            float el_tracketa,
                            float el_trackphi,
                            int el_author,
                            unsigned int el_isEM,
                            unsigned int el_OQ,
                            int el_nPixHits,
                            int el_nSCTHits,
                            bool isData,
                            float etcut,
                            float etacut,
                            bool ischeckOQ,
			    int value)
{
  SUSYObjDef::SetElecTLV(iEl,el_cl_eta,el_cl_phi,el_cl_E,el_tracketa,el_trackphi,
                         el_nPixHits,el_nSCTHits,isData,value);
  if (m_el[iEl].Pt() <= etcut) return false; // eT cut done after rescaling/smearing
  if (fabs(el_cl_eta) >= etacut) return false; // Fiducial cuts are done on cluster position
  if (!(el_author == 1 || el_author == 3)) return false; // Keep only cluster-seeded electrons
  //if (el_isEM & egammaPID::ElectronLoose) return false;
  if (el_isEM & egammaPID::ElectronMedium) return false; // ElectronMedium_WithTrackMatch == ElectronMedium in r16.6.X 
  if ((el_OQ & egammaPID::BADCLUSELECTRON) != 0) return false; // Check object quality flag
  if (ischeckOQ && m_egammaOQ.checkOQClusterElectron(RunNumber,el_cl_eta,el_cl_phi) == 3) return false; // Check if necessary  that we are not in a dead region
  return true;
}

bool SUSYObjDef::IsSignalElectron(int iEl,
                                  unsigned int el_isEM,
                                  float el_ptcone20,
                                  float etcut,
                                  float isocut)
{
  // Warning: make sure you called IsElectron before
  if (el_isEM & egammaPID::ElectronTight_WithTrackMatch) return false;
  if (m_el[iEl].Pt() <= etcut) return false; // eT cut (might be necessary for leading electron to pass trigger)
  if (isocut > 0. && m_el[iEl].Pt() > 0. && el_ptcone20/m_el[iEl].Pt() >= isocut) return false;
  return true;
}

void SUSYObjDef::SetElecTLV(int iEl,
                            float el_cl_eta,
                            float el_cl_phi,
                            float el_cl_E,
                            float el_tracketa,
                            float el_trackphi,
                            int el_nPixHits,
                            int el_nSCTHits,
                            bool isData,
			    int value)
{
  float el_eta = 0.;
  float el_phi = 0.;
  if (el_nPixHits+el_nSCTHits < 4) {
    el_eta = el_cl_eta;
    el_phi = el_cl_phi;
  } else {
    el_eta = el_tracketa;
    el_phi = el_trackphi;
  }
  std::string part_type="ELECTRON";
  float el_E = el_cl_E;
  if (!isData) { // Smear (keep r16 smearing correction at the moment)
    m_eRescale.SetRandomSeed(int(1.e+5*fabs(el_cl_phi))); 
    // int value   0:nominal, 1: smearcorr_down, 2: smearcorr_up
    // A new argument as a string version number has been added which could be either "2011" or "2010" 
    // depending on the data year.
    double smearcorr = m_eRescale.getSmearingCorrectionMeV(el_cl_eta,el_cl_E,value,true,"2011");
    el_E *= smearcorr;
  } else { // Residual energy scale corrections to be applied to data
    el_E = m_eRescale.applyEnergyCorrectionMeV(el_cl_eta,el_cl_phi,el_E,el_E/cosh(el_eta),value,"ELECTRON");
  }
  // Neglect electron mass
  m_el[iEl].SetPtEtaPhiE(el_E/cosh(el_eta),el_eta,el_phi,el_E);
  m_iEl = iEl;
}

bool SUSYObjDef::IsMuon(int iMu,
                        float mu_staco_pt,
                        float mu_staco_eta,
                        float mu_staco_phi,
                        float mu_staco_E,
                        float mu_staco_me_qoverp_exPV,
                        float mu_staco_id_qoverp_exPV,
                        float mu_staco_me_theta_exPV,
                        float mu_staco_id_theta_exPV,
                        float mu_staco_id_theta,
                        int mu_staco_isCombinedMuon,
                        int mu_staco_isLowPtReconstructedMuon,
                        int mu_staco_loose,
                        int mu_staco_expectBLayerHit, 
                        int mu_staco_nBLHits,
                        int mu_staco_nPixHits,
                        int mu_staco_nPixelDeadSensors,
                        int mu_staco_nPixHoles,
                        int mu_staco_nSCTHits,
                        int mu_staco_nSCTDeadSensors,
                        int mu_staco_nSCTHoles,
                        int mu_staco_nTRTHits,
                        int mu_staco_nTRTOutliers,
                        bool isData,
                        float ptcut,
                        float etacut,
			std::string THESTRING)
{
  SUSYObjDef::SetMuonTLV(iMu,mu_staco_pt,mu_staco_eta,mu_staco_phi,mu_staco_E,
                         mu_staco_me_qoverp_exPV,mu_staco_id_qoverp_exPV,
                         mu_staco_me_theta_exPV,mu_staco_id_theta_exPV,mu_staco_isCombinedMuon,isData,THESTRING);

  // Acceptance cuts
  if (m_mu[iMu].Pt() <= ptcut) return false;
  if (fabs(m_mu[iMu].Eta()) >= etacut) return false;

  // Muon quality cuts
  //if (!(mu_staco_isCombinedMuon)) return false;
  if (!(mu_staco_isCombinedMuon || mu_staco_isLowPtReconstructedMuon)) return false;
  if (!mu_staco_loose) return false;

  // Track quality requirements
  if (mu_staco_expectBLayerHit && mu_staco_nBLHits == 0) return false;
  if (mu_staco_nPixHits + mu_staco_nPixelDeadSensors <= 1) return false;
  if (mu_staco_nSCTHits + mu_staco_nSCTDeadSensors < 6) return false;
  if (mu_staco_nPixHoles + mu_staco_nSCTHoles >= 3) return false;
  int nTRTOutliers = mu_staco_nTRTOutliers;
  int nTRTTotal    = nTRTOutliers + mu_staco_nTRTHits;
  float trackEta = -log(tan(mu_staco_id_theta/2));
  if (fabs(trackEta) < 1.9 && nTRTTotal <= 5) return false;
  if (nTRTTotal > 5 && nTRTOutliers >= 0.9*nTRTTotal) return false;

  return true;
}

bool SUSYObjDef::IsSignalMuon(int iMu, float mu_staco_ptcone20, float ptcut, float isocut)
{
  if (m_mu[iMu].Pt() <= ptcut) return false;                    // pT cut (might be necessary for leading electron to pass trigger)
  if (isocut > 0. && mu_staco_ptcone20 >= isocut) return false; // Muon isolation cut
  return true;
}

bool SUSYObjDef::IsCosmicMuon(float mu_staco_z0_exPV, float mu_staco_d0_exPV, float z0cut, float d0cut)
{
  // Muon cosmic rejection cuts.
  // Method returns TRUE if there is a cosmic candidate !
  // See https://indico.cern.ch/getFile.py/access?contribId=6&resId=0&materialId=slides&confId=132166 
  if (fabs(mu_staco_z0_exPV) >= z0cut) return true;
  if (fabs(mu_staco_d0_exPV) >= d0cut) return true;

  return false;
}

bool SUSYObjDef::IsBadMuon(float mu_staco_qoverp_exPV, float mu_staco_cov_qoverp_exPV, float qopcut)
{
  if (qopcut > 0. && mu_staco_qoverp_exPV != 0 && mu_staco_qoverp_exPV > -99999.) {
    float qoperror = sqrt(mu_staco_cov_qoverp_exPV)/fabs(mu_staco_qoverp_exPV);
    if (qoperror >= qopcut) return true;
  }
  return false;
}

void SUSYObjDef::SetMuonTLV(int iMu,
                            float mu_staco_pt,
                            float mu_staco_eta,
                            float mu_staco_phi,
                            float /*mu_staco_E*/,
                            float mu_staco_me_qoverp_exPV,
                            float mu_staco_id_qoverp_exPV,
                            float mu_staco_me_theta_exPV,
                            float mu_staco_id_theta_exPV,
                            int mu_staco_isCombinedMuon,
                            bool isData,
			    std::string THESTRING)
{
  float mypt = mu_staco_pt;
  if (!isData && m_mcp_smear) {
    /** Use the MC event number to set seed so that the random numbers are reproducible by
        different analyzers **/
    float ptcb = mu_staco_pt;
    float ptid = (mu_staco_id_qoverp_exPV != 0.) ? fabs(sin(mu_staco_id_theta_exPV)/mu_staco_id_qoverp_exPV) : 0.;
    float ptms = (mu_staco_me_qoverp_exPV != 0.) ? fabs(sin(mu_staco_me_theta_exPV)/mu_staco_me_qoverp_exPV) : 0.;
    //m_mcp_smear.SetSeed(EventNumber+iMu*1000000);
    m_mcp_smear->SetSeed(int(1.e+5*fabs(mu_staco_phi)));
    m_mcp_smear->Event(ptms,ptid,ptcb,mu_staco_eta); 

    if (THESTRING == "") {
      if (mu_staco_isCombinedMuon)
	mypt = m_mcp_smear->pTCB();
      else
	mypt = m_mcp_smear->pTID();
    } else {
      double pTMS_smeared = 0.;
      double pTID_smeared = 0.;
      double pTCB_smeared = 0.;
      
      // Valid values for "THESTRING": {"MSLOW", "MSUP", "IDLOW", "IDUP"} 
      m_mcp_smear->PTVar(pTMS_smeared, pTID_smeared, pTCB_smeared, THESTRING);
      
      if (mu_staco_isCombinedMuon)
	mypt = pTCB_smeared;
      else
	mypt = pTID_smeared;
    }
  }
  // Keep eta,phi fixed when smearing muon pt
  m_mu[iMu].SetPtEtaPhiM(mypt,mu_staco_eta,mu_staco_phi,105.66);
  m_iMu = iMu;
}

bool SUSYObjDef::IsJet(int iJet, 
                       float jet_AntiKt4TopoNewEM_pt,
                       float jet_AntiKt4TopoNewEM_eta,
                       float jet_AntiKt4TopoNewEM_phi,
                       float jet_AntiKt4TopoNewEM_E,
                       float jet_AntiKt4TopoNewEM_emscale_eta,
                       float jet_AntiKt4TopoNewEM_emfrac,
                       float jet_AntiKt4TopoNewEM_hecf,
                       float jet_AntiKt4TopoNewEM_LArQuality,
                       float jet_AntiKt4TopoNewEM_HECQuality,
                       float jet_AntiKt4TopoNewEM_Timing,
                       float jet_AntiKt4TopoNewEM_sumPtTrk,
                       float jet_AntiKt4TopoNewEM_fracSamplingMax,
                       float jet_AntiKt4TopoNewEM_NegativeE,
                       bool isData,
                       float ptcut,
                       float etacut,
                       JetID::BadJetCategory criteria)
{
  SUSYObjDef::SetJetTLV(iJet,jet_AntiKt4TopoNewEM_pt,jet_AntiKt4TopoNewEM_eta,jet_AntiKt4TopoNewEM_phi,jet_AntiKt4TopoNewEM_E);
  if (m_jet[iJet].Pt() <= ptcut) return false;
  if (fabs(jet_AntiKt4TopoNewEM_eta) >= etacut) return false;
  bool isgoodjet = true;
  if (isData) {
//    double chf = (jet_AntiKt4TopoNewEM_pt != 0.) ? jet_AntiKt4TopoNewEM_sumPtTrk/jet_AntiKt4TopoNewEM_pt : 0.;
//    isgoodjet = bool(m_JetCleanTool->accept(jet_AntiKt4TopoNewEM_emscale_eta,jet_AntiKt4TopoNewEM_NegativeE,
//                                            jet_AntiKt4TopoNewEM_hecf,jet_AntiKt4TopoNewEM_HECQuality,
//                                            jet_AntiKt4TopoNewEM_emfrac,chf,
//                                            jet_AntiKt4TopoNewEM_LArQuality,jet_AntiKt4TopoNewEM_Timing,
//                                            jet_AntiKt4TopoNewEM_fracSamplingMax));
// Be careful, the logic is reversed between m_JetCleanTool->accept and JetID::isBadJet
    if (JetID::isBadJet(criteria,jet_AntiKt4TopoNewEM_emfrac,jet_AntiKt4TopoNewEM_hecf,
                        jet_AntiKt4TopoNewEM_LArQuality,jet_AntiKt4TopoNewEM_HECQuality,
                        jet_AntiKt4TopoNewEM_Timing,jet_AntiKt4TopoNewEM_sumPtTrk/1000.,
                        jet_AntiKt4TopoNewEM_emscale_eta,jet_AntiKt4TopoNewEM_pt/1000.,
                        jet_AntiKt4TopoNewEM_fracSamplingMax,jet_AntiKt4TopoNewEM_NegativeE))
      isgoodjet = false;
  }
  return isgoodjet;
}

void SUSYObjDef::SetJetTLV(int iJet,
                           float jet_AntiKt4TopoNewEM_pt,
                           float jet_AntiKt4TopoNewEM_eta,
                           float jet_AntiKt4TopoNewEM_phi,
                           float jet_AntiKt4TopoNewEM_E)
{
  m_jet[iJet].SetPtEtaPhiE(jet_AntiKt4TopoNewEM_pt,jet_AntiKt4TopoNewEM_eta,
                           jet_AntiKt4TopoNewEM_phi,jet_AntiKt4TopoNewEM_E);
  m_iJet = iJet;
}

bool SUSYObjDef::IsBJet(float jet_AntiKt4TopoNewEM_flavor_weight_xxx, std::string jetTagger, float op)
{ 
  // Default operating points are taken from 
  // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/Analysis16#Calibrations_to_be_used_for_EPS
  float opdef = 5.85;
  if (jetTagger == "SV0")
    opdef = 5.85; // 50% efficiency
  else if (jetTagger == "JetProb") // Might require a -log(w).... need to check
    opdef = 3.25; // 50% efficiency
  else if (jetTagger == "Comb") // IP3D+SV1 is corresponding to jet_AntiKt4TopoNewEM_flavor_weight_Comb
    opdef = 4.50; // 60% efficiency
  else if (jetTagger == "JetFitterCOMBNN")
    opdef = 2.00; // 60% efficiency

  float opcut = opdef;
  if (op != 999.) opcut = op;
  return (jet_AntiKt4TopoNewEM_flavor_weight_xxx > opcut);
}


bool SUSYObjDef::IsLArHoleVeto(float jet_AntiKt4TopoNewEM_pt, float jet_AntiKt4TopoNewEM_eta, 
                               float jet_AntiKt4TopoNewEM_phi, 
                               float jet_AntiKt4TopoNewEM_BCH_CORR_JET, float jet_AntiKt4TopoNewEM_BCH_CORR_CELL, 
                               bool kIsData, float ptcut, int RunNumber)
{
  if (!SUSYObjDef::IsLArHole(jet_AntiKt4TopoNewEM_eta,jet_AntiKt4TopoNewEM_phi)) return false;
  float pt = ptcut;
  // Correct threshold cut by missing energy in LAr hole in data. 
  // BCH_CORR_CELL is necessary to take into account dead tile module near LAr hole. 
  if (kIsData && RunNumber >= 180614) 
    pt = pt*(1. - jet_AntiKt4TopoNewEM_BCH_CORR_JET)/(1. - jet_AntiKt4TopoNewEM_BCH_CORR_CELL);
  if (jet_AntiKt4TopoNewEM_pt <= pt) return false;
  return true;
}

TVector2 SUSYObjDef::GetMET(float MET_Simplified20_RefFinal_etx, 
                            float MET_Simplified20_RefFinal_ety,
                            float MET_Simplified20_RefJet_etx,
                            float MET_Simplified20_RefJet_ety,
                            float MET_Simplified20_Muon_Total_Staco_etx, 
                            float MET_Simplified20_Muon_Total_Staco_ety,
                            std::vector<int> vmuidx,
                            std::vector<int> velidx,
                            bool redoRefJet)
{
  float etx_orig = MET_Simplified20_RefFinal_etx - MET_Simplified20_Muon_Total_Staco_etx;
  float ety_orig = MET_Simplified20_RefFinal_ety - MET_Simplified20_Muon_Total_Staco_ety;
  if (redoRefJet) {
    etx_orig -= MET_Simplified20_RefJet_etx;
    ety_orig -= MET_Simplified20_RefJet_ety;
  }
  TVector2 met(etx_orig,ety_orig);
  if (redoRefJet) {
    for (int iJet = 0; iJet <= m_iJet; iJet++) {
      if (m_jet[iJet].Pt() <= 20000.) continue;
      bool isoverlap = false;
      for (unsigned int iElIdx = 0; iElIdx < velidx.size(); iElIdx++) {
        if (m_el[velidx.at(iElIdx)].DeltaR(m_jet[iJet]) > 0.2) continue;
        isoverlap = true;
        break;
      }
      if (isoverlap) continue; // jet already included in RefEle term
      met -= m_jet[iJet].Vect().XYvector();
    }
  }
  for (unsigned int iMuIdx = 0; iMuIdx < vmuidx.size(); iMuIdx++)
    met -= m_mu[vmuidx.at(iMuIdx)].Vect().XYvector();
  return met;
}

// use MET_Simplified20_RefFinal after all other calculation, such as sum of region, adding muon term
// MET_Simplifeid20_RefJet, RefEle are the default value directly obtained from D3PD
// vector of jets are all jet container without any selection, such as eta cut nor overlap removal between electron
TVector2 SUSYObjDef::GetFixedMet(float MET_Simplified20_RefFinal_calc_etx, float MET_Simplified20_RefFinal_calc_ety,
                                 float MET_Simplified20_RefJet_etx, float MET_Simplified20_RefJet_ety,
                                 float MET_Simplified20_RefEle_etx, float MET_Simplified20_RefEle_ety,
                                 std::vector<TLorentzVector>* jet_AntiKt4TopoNewEM /* all units are MeV */)
{
  TVector2 met(MET_Simplified20_RefFinal_calc_etx, MET_Simplified20_RefFinal_calc_ety);
  // if MET_RefEle_et is smaller than 20GeV, do nothing
  float metRefEleEt = sqrt(pow(MET_Simplified20_RefEle_etx,2)+pow(MET_Simplified20_RefEle_ety,2));
  if(metRefEleEt<=20000.) return met;
  // calculate sum of jet momentum for pt>20GeV
  float jetSumPx20(0.), jetSumPy20(0.);
  if (jet_AntiKt4TopoNewEM != 0) {
    for(unsigned int i=0; i<jet_AntiKt4TopoNewEM->size(); i++) {
      if (jet_AntiKt4TopoNewEM->at(i).Pt() <= 20000.) continue;
      jetSumPx20 += jet_AntiKt4TopoNewEM->at(i).Px();
      jetSumPy20 += jet_AntiKt4TopoNewEM->at(i).Py();
    }
  } else { // Take jets stored already in SUSYObjDef (all jets in container if you called GetJet for every jet)
    for (int iJet = 0; iJet <= m_iJet; iJet++) {
      if (m_jet[iJet].Pt() <= 20000.) continue;
      jetSumPx20 += m_jet[iJet].Px();
      jetSumPy20 += m_jet[iJet].Py();
    }
  }
  // calculate overlapped MET_RefJet term
  float metRefJetDifEx = MET_Simplified20_RefJet_etx + jetSumPx20;
  float metRefJetDifEy = MET_Simplified20_RefJet_ety + jetSumPy20;
  float metRefJetDifEt = sqrt(pow(metRefJetDifEx,2)+pow(metRefJetDifEy,2));
  // if overlapped MET_RefJet term is smaller than MET_RefEle term, regarded as bug
  // if not, do nothing
  if( metRefJetDifEt/metRefEleEt > 1.0 ) return met;
  // get e/h calibration factor as a function of MET_RefEle et
  float eoverh(1.402);
  if(metRefEleEt>=0. && metRefEleEt<=20000.) eoverh = 1.929;
  else if(metRefEleEt>20000. && metRefEleEt<=25000.) eoverh = 1.778;
  else if(metRefEleEt>25000. && metRefEleEt<=30000.) eoverh = 1.702;
  else if(metRefEleEt>30000. && metRefEleEt<=35000.) eoverh = 1.652;
  else if(metRefEleEt>35000. && metRefEleEt<=40000.) eoverh = 1.617;
  else if(metRefEleEt>40000. && metRefEleEt<=50000.) eoverh = 1.581;
  else if(metRefEleEt>50000. && metRefEleEt<=60000.) eoverh = 1.536;
  else if(metRefEleEt>60000. && metRefEleEt<=70000.) eoverh = 1.504;
  else if(metRefEleEt>70000. && metRefEleEt<=80000.) eoverh = 1.477;
  else if(metRefEleEt>80000. && metRefEleEt<=90000.) eoverh = 1.457;
  else if(metRefEleEt>90000. && metRefEleEt<=100000.) eoverh = 1.438;
  else if(metRefEleEt>100000.) eoverh = 1.402;
  // calculate fixed MET_RefFinal
  float metRefFinalFixedEx = MET_Simplified20_RefFinal_calc_etx - MET_Simplified20_RefJet_etx - jetSumPx20 - eoverh*MET_Simplified20_RefEle_etx;
  float metRefFinalFixedEy = MET_Simplified20_RefFinal_calc_ety - MET_Simplified20_RefJet_ety - jetSumPy20 - eoverh*MET_Simplified20_RefEle_ety;
  met.Set(metRefFinalFixedEx, metRefFinalFixedEy);
  return met;
}

bool SUSYObjDef::IsGoodVertex(std::vector<int>* vx_nTracks)
{
  // Check if primary vertex is a good vertex
  bool isGoodPrimaryVertex = false;
  if (vx_nTracks->size() && vx_nTracks->at(0) > 4) isGoodPrimaryVertex = true; // Jet-MET wg requirement
  //for (unsigned int ivx = 0; ivx < vx_nTracks->size(); ivx++)
  //  if (vx_nTracks->at(ivx) > 4) {
  //    isGoodPrimaryVertex = true; // Jet-MET wg requirement
  //    break;
  //  }
  return isGoodPrimaryVertex;
}

float SUSYObjDef::GetSignalMuonSF(int iMu, bool isCBOnly)
{
  if (isCBOnly) return m_StacoCBSCF.scaleFactor(m_mu[iMu]);
  return m_StacoLooseSCF.scaleFactor(m_mu[iMu]);
}

float SUSYObjDef::GetSignalMuonSFUnc(int iMu, bool isCBOnly, int systType)
{
  float sfunc  = 0.;
  float sfsyst = 0.;
  if (isCBOnly) {
    sfunc  = m_StacoCBSCF.scaleFactorUncertainty(m_mu[iMu]);
    sfsyst = m_StacoCBSCF.scaleFactorSystematicUncertainty(m_mu[iMu]);
  } else {
    sfunc  = m_StacoLooseSCF.scaleFactorUncertainty(m_mu[iMu]);
    sfsyst = m_StacoLooseSCF.scaleFactorSystematicUncertainty(m_mu[iMu]);
  }
  if (systType == 1) return sfunc;
  if (systType == 2) return sfsyst;
  return (sfunc + sfsyst); // Add uncertainties linearly
}

bool SUSYObjDef::MuonHasTriggerMatch(unsigned int mItr, int mu_staco_n, int trig_EF_trigmuonef_n, vector<int> *trig_EF_trigmuonef_track_n, vector<float> *mu_staco_pt, vector<float> *mu_staco_eta, vector<float> *mu_staco_phi, vector<vector<float> > *trig_EF_trigmuonef_track_CB_pt, vector<vector<float> > *trig_EF_trigmuonef_track_CB_eta, vector<vector<float> > *trig_EF_trigmuonef_track_CB_phi, vector<vector<int> > *trig_EF_trigmuonef_track_CB_hasCB){


 float onlpt;
 float onleta;
 float onlphi;

  // found match for object?
  bool muonHasTriggerMatch = false;
  TLorentzVector onl;
  TLorentzVector ofl;

  // minimal DeltaR found
  float DeltaRMin = 0.15;

  // the trigger thresholds: needed to check if a TE has actually passed the trigger Hypothesis. 
  float trigPtCutsEF_mu18[] = { 17530 , 17390 , 17340 , 17280 };
  float trigEtaBinsEF_mu18[] = {0 , 1.05 , 1.5 , 2 , 9.9 };


  // offline variables used for matching
  float ofl_eta = mu_staco_eta -> at(mItr);
  float ofl_phi = mu_staco_phi -> at(mItr);
  float ofl_pt = mu_staco_pt -> at(mItr);
  ofl.SetPtEtaPhiE(ofl_pt,ofl_eta,ofl_phi,ofl_pt*cosh(ofl_eta));

  // loop over all trigger elements and find the best matching one
  for( int trigItr = 0; trigItr != trig_EF_trigmuonef_n; ++trigItr ) {

    // loop over all tracks associated to the trigger element;
    for( int trackItr = 0; trackItr != trig_EF_trigmuonef_track_n -> at(trigItr); ++trackItr ) {

      // do only use combined tracks
      if( !trig_EF_trigmuonef_track_CB_hasCB -> at(trigItr).at(trackItr) ) continue;

      float onl_eta = trig_EF_trigmuonef_track_CB_eta -> at(trigItr).at(trackItr);
      float onl_phi = trig_EF_trigmuonef_track_CB_phi -> at(trigItr).at(trackItr);
      float onl_pt = trig_EF_trigmuonef_track_CB_pt -> at(trigItr).at(trackItr);
      float th = 1.e12;
      onl.SetPtEtaPhiE(onl_pt,onl_eta,onl_phi,onl_pt*cosh(onl_eta));

      // set trigger threshold
      for( int i = 0; i < 4; ++i ) {
        if( fabs(onl_eta) >= trigEtaBinsEF_mu18[i] && fabs(onl_eta) < trigEtaBinsEF_mu18[i+1] ) th = trigPtCutsEF_mu18[i];
      }

      // check DeltaR
      if( ofl.DeltaR(onl) < DeltaRMin ) {

        DeltaRMin = ofl.DeltaR(onl);
        onlpt = onl_pt;
        onleta = onl_eta;
        onlphi = onl_phi;

        // check part of trigger hypothesis by hand
        if( onl_pt > th ) {
          muonHasTriggerMatch = true;
        }
      }
    }
  }

  return muonHasTriggerMatch;
}
