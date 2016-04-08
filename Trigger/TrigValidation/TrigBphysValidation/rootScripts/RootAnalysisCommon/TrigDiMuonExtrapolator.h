/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///===================================================
/// TrigDiMuonExtrapolator class
///---------------------------------------------------
/// Author: Daniel Scheirich <scheiric@cern.ch>
/// Part of TrigDiMuonCalib
///===================================================

#ifndef TrigDiMuonExtrapolator_H
#define TrigDiMuonExtrapolator_H

#include <TProfile.h>
#include <TGraphErrors.h>
#include <TVector2.h>
#include "Log.h"

const double PI_OVER_4 = TMath::Pi()/4.;
const double PI_OVER_8 = TMath::Pi()/8.;

class TrigDiMuonExtrapolator {
  public:

    TrigDiMuonExtrapolator(OutputLevel olIn, std::string settings, bool createHist = true);

    // inner station specific methods
    void     getBinIE(double eta,
                      int& bin);                      // returns bin in Inner Endcap
    void     fillInner(Calib* hits);                  // fill inner station histograms
    void     drawInner(std::string settings);         // draw inner histograms
    TVector2 propagationInnerBarrel(double ptId,
                                    double etaId,
                                    double phiId,
                                    double charge);   // propagate to inner barrel
    TVector2 propagationInnerBarrel(Calib* t);        // propagate to inner barrel
    TVector2 propagationInnerEndCap(double ptId,
                                    double etaId,
                                    double phiId,
                                    double charge);   // propagate to inner endcap
    TVector2 propagationInnerEndCap(Calib* t);        // propagate to inner endcap

    // middle station specific methods
    void     getBinME(double ptId,
                      double etaId,
                      double phiId,
                      double charge,
                      int& binEta,
                      int& binPhi);                   // return bins in Middle Endcap
    void     fillMiddle(Calib* hits);                 // fill middle station histograms
    void     drawMiddle(std::string settings);        // draw inner histograms
    TVector2 propagationMiddleBarrel(double ptId,
                                     double etaId,
                                     double phiId,
                                     double charge);  // propagate to inner barrel
    TVector2 propagationMiddleBarrel(Calib* t);       // propagate to inner barrel
    TVector2 propagationMiddleEndCap(double ptId,
                                     double etaId,
                                     double phiId,
                                     double charge);  // propagate to inner endcap
    TVector2 propagationMiddleEndCap(Calib* t);       // propagate to inner endcap

    // hit matching
    bool     matchMdtInner(double r, double z,
                           double ptId,
                           double etaId,
                           double phiId,
                           double charge);            // check MDT inner hit matching

    bool     matchMdtMiddle(double r, double z,
                            double ptId,
                            double etaId,
                            double phiId,
                            double charge);           // check MDT inner hit matching
    bool     matchRpcEtaPad(double r, double z,
                            double ptId,
                            double etaId,
                            double phiId,
                            double charge);           // check RPC middle eta hit matching
    bool     matchRpcPhiPad(double x, double y,
                            double ptId,
                            double etaId,
                            double phiId,
                            double charge);           // check RPC middle phi hit matching
    bool     matchInnerTgcPhiHit(double phi,
                                 double ptId,
                                 double etaId,
                                 double phiId,
                                 double charge);      // check TGC inner phi hit matching
    bool     matchInnerTgcRhoHit(double r, double z,
                                 double phi_hit,
                                 double ptId,
                                 double etaId,
                                 double phiId,
                                 double charge);      // check TGC inner rho hit matching
    bool     matchMiddleTgcPhiHit(double phi,
                                 double ptId,
                                 double etaId,
                                 double phiId,
                                 double charge);      // check TGC middle phi hit matching
    bool     matchMiddleTgcRhoHit(double r, double z,
                                  double phi_hit,
                                  double ptId,
                                  double etaId,
                                  double phiId,
                                  double charge);     // check TGC middle rho hit matching

    // used by both
    void     read(std::string file);                  // read propagator constants from file
    void     readCutCone(std::string file);           // read matching cut-cone constants
    void     save(std::string file);                  // fave propagator constants to file

    // inline methods
    OutputLevel ol() { return m_ol; }

  private:

    OutputLevel m_ol;             // output level

    // profile histograms
    TProfile* h_ib_dPhi;          // inner barrel
    TProfile* h_ie_dPhi[3];       // inner endacp

    TProfile* h_mb_dEta;          // middle barrel
    TProfile* h_mb_dPhi;          // middle barrel
    TProfile* h_me_dEta[5][3];    // middle endcap
    TProfile* h_me_dPhi[2];       // middle endcap

    // 2D histograms
    TH2F* h2_ib_dPhi;          // inner barrel
    TH2F* h2_ie_dPhi[3];       // inner endacp

    TH2F* h2_mb_dEta;          // middle barrel
    TH2F* h2_mb_dPhi;          // middle barrel
    TH2F* h2_me_dEta[5][3];    // middle endcap
    TH2F* h2_me_dPhi[2];       // middle endcap

    TH2F* h_midEndcap_dEtaVsEta[4];    // dEta vs eta
    TH2F* h_midEndcap_dEtaVsPhi[4];    // dEta vs phi
    TH2F* h_innEndcap_dPhiVsEta[4];    // dEta vs phi

    // fitted functions
    TF1* fun_ib;
    TF1* fun_ie[3];

    TF1* fun_mb;
    TF1* fun_me[5][3];
    TF1* fun_me_dPhi;

    // propagator constants
    double m_ib_c0;
    double m_ib_c1;

    double m_ie_c0[3];
    double m_ie_c1[3];

    double m_mb_c0;
    double m_mb_c1;
    double m_mb_c2;

    double m_me_c0[5][3];
    double m_me_c1[5][3];
    double m_me_c2[5][3];

    double m_me_dPhi_c0;
    double m_me_dPhi_c1;

    // hit matching
    double m_RpcEtaFactor;
    double m_RpcPhiFactor;
    double m_TgcEtaFactor;
    double m_TgcPhiFactor;
    double m_MdtEtaFactor;

    // cut-cone constants
    double m_cone_Endcap_mdt_inner_dEta_c0;
    double m_cone_Endcap_mdt_inner_dEta_c1;

    double m_cone_Endcap_mdt_middle_dEta_c0;
    double m_cone_Endcap_mdt_middle_dEta_c1;

    double m_cone_Barrel_mdt_inner_dEta_c0;
    double m_cone_Barrel_mdt_inner_dEta_c1;

    double m_cone_Barrel_mdt_middle_dEta_c0;
    double m_cone_Barrel_mdt_middle_dEta_c1;

    double m_cone_rpc_middle_dEta_c0;
    double m_cone_rpc_middle_dEta_c1;

    double m_cone_rpc_middle_dPhi_c0;
    double m_cone_rpc_middle_dPhi_c1;

    double m_cone_tgc_wire_inner_dEta_c0;
    double m_cone_tgc_wire_inner_dEta_c1;

    double m_cone_tgc_wire_middle_dEta_c0;
    double m_cone_tgc_wire_middle_dEta_c1;

    double m_cone_tgc_strip_inner_dPhi_c0;
    double m_cone_tgc_strip_inner_dPhi_c1;

    double m_cone_tgc_strip_middle_dPhi_c0;
    double m_cone_tgc_strip_middle_dPhi_c1;
    double m_cone_tgc_strip_middle2_dPhi_c0;
    double m_cone_tgc_strip_middle2_dPhi_c1;


    // flag
    bool m_oneOverPt;
    int m_dEtaVsPhiBin;    // for which endcap eta bin plot dEta_vs_phi should be drawn
};

///===================================================
/// Begin of definitions
///===================================================
TrigDiMuonExtrapolator::TrigDiMuonExtrapolator(OutputLevel olIn, std::string settings, bool createHist) : m_ol(olIn)
{
  Log log("TrigDiMuonExtrapolator::TrigDiMuonExtrapolator", ol());

  m_oneOverPt = false;
  if( settings.find("OneOverPt") != std::string::npos) m_oneOverPt = true;

  double rangeMin = 2500;
  double rangeMax = 20000;
  double rangeMinF = rangeMin;
  if(m_oneOverPt) {
    rangeMin = 0;
    rangeMinF = 5e-5;
    rangeMax = 0.0004;
  }

  int nBins = 10;

  if(createHist) {
    // inner station
    h_ib_dPhi  = new TProfile("h_ib","Inner Barrel;1/p_{T} (MeV);#Delta#phi",nBins,rangeMin,rangeMax);
    h2_ib_dPhi = new TH2F("h2_ib","Inner Barrel;1/p_{T} (MeV);#Delta#phi",nBins,rangeMin,rangeMax,100,-1., 1.);

    // middle station
    h_mb_dEta = new TProfile("h_mb","Middle Barrel;1/p_{T} (MeV);#Delta#eta",nBins,rangeMin,rangeMax);
    h2_mb_dEta = new TH2F("h2_mb","Middle Barrel;1/p_{T} (MeV);#Delta#eta",nBins,rangeMin,rangeMax,100,-1.,1.);
    h_mb_dPhi = new TProfile("h_mb_dPhi","Middle Barrel;1/p_{T} (MeV);#Delta#phi",nBins,rangeMin,rangeMax);
    h2_mb_dPhi = new TH2F("h2_mb_dPhi","Middle Barrel;1/p_{T} (MeV);#Delta#phi",nBins,rangeMin,rangeMax,100,-1.,1.);

    for(int i=0; i<5; ++i) {
      for(int j=0; j<3; ++j) {
        h_me_dEta[i][j] = new TProfile(("h_me_"+iToStr(i)+"_"+iToStr(j)).c_str(),("Middle Endcap - bin "+iToStr(i)+" x "+iToStr(j)+";1/p_{T} (MeV);#Delta#eta").c_str(),nBins,rangeMin,rangeMax);
        h2_me_dEta[i][j] = new TH2F(("h2_me_"+iToStr(i)+"_"+iToStr(j)).c_str(),("Middle Endcap - bin "+iToStr(i)+" x "+iToStr(j)+";1/p_{T} (MeV);#Delta#eta").c_str(),nBins,rangeMin,rangeMax, 100, -1., 1.);
      }
    }

    for(int i=0; i<2; ++i) {
      h_me_dPhi[i] = new TProfile(("h_me_dPhi_"+iToStr(i)).c_str(),("Middle Endcap - bin "+iToStr(i)+";1/p_{T} (MeV);#Delta#phi").c_str(),nBins,rangeMin,rangeMax);
      h2_me_dPhi[i] = new TH2F(("h2_me_dPhi_"+iToStr(i)).c_str(),("Middle Endcap - bin "+iToStr(i)+";1/p_{T} (MeV);#Delta#phi").c_str(),nBins,rangeMin,rangeMax, 100, -1., 1.);
    }

    // dEta vs eta
    h_midEndcap_dEtaVsEta[0] = new TH2F("h_midEndcap_dEtaVsEta0","Middle Endcap p_{T} < 2GeV;#Delta#eta;#eta",100,-1.5,1,100,0.9,2.6);
    h_midEndcap_dEtaVsEta[1] = new TH2F("h_midEndcap_dEtaVsEta1","Middle Endcap 2GeV < p_{T} < 4GeV;#Delta#eta;#eta",100,-1.5,1,100,0.9,2.6);
    h_midEndcap_dEtaVsEta[2] = new TH2F("h_midEndcap_dEtaVsEta2","Middle Endcap 4GeV < p_{T} < 6GeV;#Delta#eta;#eta",100,-1.5,1,100,0.9,2.6);
    h_midEndcap_dEtaVsEta[3] = new TH2F("h_midEndcap_dEtaVsEta3","Middle Endcap 6GeV < p_{T};#Delta#eta;#eta",100,-1.5,1,100,0.9,2.6);

    h_midEndcap_dEtaVsPhi[0] = new TH2F("h_midEndcap_dEtaVsPhi0","Middle Endcap p_{T} < 2GeV;#Delta#eta;#phi",100,-1.5,0.5,100,0,PI_OVER_8);
    h_midEndcap_dEtaVsPhi[1] = new TH2F("h_midEndcap_dEtaVsPhi1","Middle Endcap 2GeV < p_{T} < 4GeV;#Delta#eta;#phi",100,-1.5,0.5,100,0,PI_OVER_8);
    h_midEndcap_dEtaVsPhi[2] = new TH2F("h_midEndcap_dEtaVsPhi2","Middle Endcap 4GeV < p_{T} < 6GeV;#Delta#eta;#phi",100,-1.5,0.5,100,0,PI_OVER_8);
    h_midEndcap_dEtaVsPhi[3] = new TH2F("h_midEndcap_dEtaVsPhi3","Middle Endcap 6GeV < p_{T};#Delta#eta;#phi",100,-1.5,0.5,100,0,PI_OVER_8);

    h_innEndcap_dPhiVsEta[0] = new TH2F("h_innEndcap_dPhiVsEta0","Inner Endcap p_{T} < 2GeV         ;#Delta#Phi;#eta",100,-0.5,1.,100,0.9,2.6);
    h_innEndcap_dPhiVsEta[1] = new TH2F("h_innEndcap_dPhiVsEta1","Inner Endcap 2GeV < p_{T} < 4GeV  ;#Delta#Phi;#eta",100,-0.5,1.,100,0.9,2.6);
    h_innEndcap_dPhiVsEta[2] = new TH2F("h_innEndcap_dPhiVsEta2","Inner Endcap 4GeV < p_{T} < 6GeV  ;#Delta#Phi;#eta",100,-0.5,1.,100,0.9,2.6);
    h_innEndcap_dPhiVsEta[3] = new TH2F("h_innEndcap_dPhiVsEta3","Inner Endcap 6GeV < p_{T}         ;#Delta#Phi;#eta",100,-0.5,1.,100,0.9,2.6);
  }

  /// fit functions
  // inner station

  if(m_oneOverPt)
    fun_ib = new TF1("fun_ib","[0] + [1]*x",rangeMinF,rangeMax);
  else
    fun_ib = new TF1("fun_ib","[0] + [1]*(1/x)",rangeMinF,rangeMax);

  //   fun_ib->SetParLimits(0,0,1e30);
  //   fun_ib->SetParLimits(1,0,1e30);

  for(int i=0; i<3; ++i) {
    h_ie_dPhi[i] = new TProfile(("h_ie_"+iToStr(i)).c_str(),("Inner Endcap - bin "+iToStr(i)+";1/p_{T} (MeV);#Delta#phi").c_str(),nBins,rangeMin,rangeMax);
    h2_ie_dPhi[i] = new TH2F(("h2_ie_"+iToStr(i)).c_str(),("Inner Endcap - bin "+iToStr(i)+";1/p_{T} (MeV);#Delta#phi").c_str(),nBins,rangeMin,rangeMax, 100, -1, 1.);

    if(m_oneOverPt)
      fun_ie[i] = new TF1(("fun_ie_"+iToStr(i)).c_str(),"[0] + [1]*x",rangeMinF,rangeMax);
    else
      fun_ie[i] = new TF1(("fun_ie_"+iToStr(i)).c_str(),"[0] + [1]*(1/x)",rangeMinF,rangeMax);

    //     fun_ie[i]->SetParLimits(0,0,1e30);
    //     fun_ie[i]->SetParLimits(1,0,3e10);
  }

  // middle station

  if(m_oneOverPt)
    fun_mb = new TF1("fun_mb","[0] + [1]*x + [2]*x*x",rangeMinF,rangeMax);
  else
    fun_mb = new TF1("fun_mb","[0] + [1]*(1/x) + [2]*1/(x*x)",rangeMinF,rangeMax);
    //   fun_mb->SetParLimits(0,0,1e10);

  for(int i=0; i<5; ++i) {
    for(int j=0; j<3; ++j) {
      if(m_oneOverPt)
        fun_me[i][j] = new TF1(("fun_me_"+iToStr(i)+"_"+iToStr(j)).c_str(),"[0] + [1]*x + [2]*x*x",rangeMinF,rangeMax);
      else
        fun_me[i][j] = new TF1(("fun_me_"+iToStr(i)+"_"+iToStr(j)).c_str(),"[0] + [1]*(1/x) + [2]*1/(x*x)",rangeMinF,rangeMax);
         // fun_me[i][j]->SetParLimits(0,0,1e10);
    }
  }

  if(m_oneOverPt)
    fun_me_dPhi = new TF1("fun_me_dPhi","[0] + [1]*x",rangeMinF,rangeMax);
  else
    fun_me_dPhi = new TF1("fun_me_dPhi","[0] + [1]*(1/x)",rangeMinF,rangeMax);


  // hit matching cuts
  m_RpcEtaFactor  = 2.;
  m_RpcPhiFactor  = 2.;
  m_TgcEtaFactor  = 2.;
  m_TgcPhiFactor  = 2.;
  m_MdtEtaFactor  = 2.;

  m_dEtaVsPhiBin = 1;
  getValue(settings, "DEtaVsPhiBin=", m_dEtaVsPhiBin);


}
///===================================================

void TrigDiMuonExtrapolator::getBinIE(double eta, int& bin) {
  Log log("TrigDiMuonExtrapolator::getBinIE", ol());

  if (fabs(eta)>1.6) {
    bin = 0;
  }else if (fabs(eta)>1.3 && fabs(eta)<=1.6){
    bin = 1;
  }else if (fabs(eta)<=1.3) {
    bin = 2;
  }

}
///===================================================
void TrigDiMuonExtrapolator::fillInner(Calib* hits) {
  Log log("TrigDiMuonExtrapolator::fillInner", ol());

  // calculate fill value
  double x;
  if(m_oneOverPt)
    x = 1/hits->Pt;
  else
    x = hits->Pt;

  // barrel
  if(fabs(hits->Eta) < 1.1) {
    //MDT
    std::vector<double>::iterator phiItr = hits->InnerMdt_phi->begin();
    for(; phiItr!=hits->InnerMdt_phi->end(); ++phiItr) {
      h_ib_dPhi->Fill(x, hits->Charge*dAngle(hits->Phi,(*phiItr)));
      h2_ib_dPhi->Fill(x, hits->Charge*dAngle(hits->Phi,(*phiItr)));
    }
  }

  // endcap
  if(fabs(hits->Eta) > 0.9) {
    int bin = 0;
    getBinIE(hits->Eta, bin);

    std::vector<double>::iterator phiItr;
    std::vector<int>::iterator typeItr;
//     //MDT
//     phiItr = hits->InnerMdt_phi->begin();
//     for(; phiItr!=hits->InnerMdt_phi->end(); ++phiItr) {
//       h_ie_dPhi[bin]->Fill(x, hits->Charge*dAngle(hits->Phi,(*phiItr)));
//       h2_ie_dPhi[bin]->Fill(x, hits->Charge*dAngle(hits->Phi,(*phiItr)));
//     }

    //CSC
    phiItr = hits->Csc_phi->begin();
    typeItr = hits->Csc_type->begin();
    for(; phiItr!=hits->Csc_phi->end(); ++phiItr, ++typeItr) {
      if((*typeItr) == 1) {
        h_ie_dPhi[bin]->Fill(x, hits->Charge*dAngle(hits->Phi,(*phiItr)));
        h2_ie_dPhi[bin]->Fill(x, hits->Charge*dAngle(hits->Phi,(*phiItr)));
      }

      if(fabs(hits->Pt) < 2000)
        h_innEndcap_dPhiVsEta[0]->Fill(hits->Charge*dAngle(hits->Phi,(*phiItr)), fabs(hits->Eta));
      else if (fabs(hits->Pt) < 4000)
        h_innEndcap_dPhiVsEta[1]->Fill(hits->Charge*dAngle(hits->Phi,(*phiItr)), fabs(hits->Eta));
      else if (fabs(hits->Pt) < 6000)
        h_innEndcap_dPhiVsEta[2]->Fill(hits->Charge*dAngle(hits->Phi,(*phiItr)), fabs(hits->Eta));
      else
        h_innEndcap_dPhiVsEta[3]->Fill(hits->Charge*dAngle(hits->Phi,(*phiItr)), fabs(hits->Eta));

    }

    //TGC
    phiItr = hits->InnerTgc_phi->begin();
    typeItr = hits->InnerTgc_type->begin();
    for(; phiItr!=hits->InnerTgc_phi->end(); ++phiItr, ++typeItr) {
      if((*typeItr) == 1) {
        h_ie_dPhi[bin]->Fill(x, hits->Charge*dAngle(hits->Phi,(*phiItr)));
        h2_ie_dPhi[bin]->Fill(x, hits->Charge*dAngle(hits->Phi,(*phiItr)));

        if(fabs(hits->Pt) < 2000)
          h_innEndcap_dPhiVsEta[0]->Fill(hits->Charge*dAngle(hits->Phi,(*phiItr)), fabs(hits->Eta));
        else if (fabs(hits->Pt) < 4000)
          h_innEndcap_dPhiVsEta[1]->Fill(hits->Charge*dAngle(hits->Phi,(*phiItr)), fabs(hits->Eta));
        else if (fabs(hits->Pt) < 6000)
          h_innEndcap_dPhiVsEta[2]->Fill(hits->Charge*dAngle(hits->Phi,(*phiItr)), fabs(hits->Eta));
        else
          h_innEndcap_dPhiVsEta[3]->Fill(hits->Charge*dAngle(hits->Phi,(*phiItr)), fabs(hits->Eta));

      }
    }
  }

}
///===================================================
void TrigDiMuonExtrapolator::drawInner(std::string settings) {
  Log log("TrigDiMuonExtrapolator::drawInner", ol());

  TCanvas* c;

  c = new TCanvas("c_ib_dPhi","c_ib_dPhi");
  c->Print("plots.ps[");
  c->Divide(2,1);

  c->cd(1);
  h2_ib_dPhi->SetMarkerStyle(1);
  h2_ib_dPhi->Draw();

  c->cd(2);
  h_ib_dPhi->SetStats(0);
  h_ib_dPhi->Draw();

  // fitting
  if( settings.find("fitInner") != std::string::npos) {
    h_ib_dPhi->Fit(fun_ib, "R 0");
  }

  fun_ib->SetLineColor(kRed);
  fun_ib->Draw("same");
  c->Print("plots.ps");
  c->SaveAs("./fig/calibration/innerBarrel_dPhi.C");

  for(int i=0; i<3; ++i) {
    c = new TCanvas(("c_ib_dPhi_"+iToStr(i)).c_str(),"c_ib_dPhi");
    c->Divide(2,1);

    c->cd(1);
    h2_ie_dPhi[i]->SetMarkerStyle(1);
    h2_ie_dPhi[i]->Draw();

    c->cd(2);
    h_ie_dPhi[i]->SetStats(0);
    h_ie_dPhi[i]->Draw();

    // fitting
    if( settings.find("fitInner") != std::string::npos) {
      h_ie_dPhi[i]->Fit(fun_ie[i], "R 0");
    }

    fun_ie[i]->SetLineColor(kRed);
    fun_ie[i]->Draw("same");
    c->Print("plots.ps");
    c->SaveAs(("./fig/calibration/innerEndcap_dPhi_bin_"+iToStr(i)+".C").c_str());

  }

  for(int i=0; i<4;i++) {
    c = new TCanvas(("c_endcap_dPhi"+iToStr(i)).c_str(),("c_endcap_dPhi"+iToStr(i)).c_str());
    h_innEndcap_dPhiVsEta[i]->SetMarkerStyle(1);
    h_innEndcap_dPhiVsEta[i]->Draw("box");

    TLine* l;

    l = new TLine(-.5, 1.3, 1., 1.3);
    l->SetLineColor(kRed);
    l->Draw();

    l = new TLine(-.5, 1.6, 1., 1.6);
    l->SetLineColor(kRed);
    l->Draw();

    c->Print("plots.ps");
    switch(i) {
      case 0: c->SaveAs("./fig/calibration/innerEndcap_dPhi_vs_eta_pT_0-2GeV.C"); break;
      case 1: c->SaveAs("./fig/calibration/innerEndcap_dPhi_vs_eta_pT_2-4GeV.C"); break;
      case 2: c->SaveAs("./fig/calibration/innerEndcap_dPhi_vs_eta_pT_4-6GeV.C"); break;
      case 3: c->SaveAs("./fig/calibration/innerEndcap_dPhi_vs_eta_pT_6-infGeV.C"); break;
    }
  }


}
///===================================================
TVector2 TrigDiMuonExtrapolator::propagationInnerBarrel(Calib* t) {
  Log log("TrigDiMuonExtrapolator::propagationInnerBarrel", ol());

  return propagationInnerBarrel(t->Pt, t->Eta, t->Phi, t->Charge);
}
///===================================================
TVector2 TrigDiMuonExtrapolator::propagationInnerBarrel(double ptId, double etaId, double phiId, double charge) {
  Log log("TrigDiMuonExtrapolator::propagationInnerBarrel", ol());

  double eta_ext_inner = etaId;
  double phi_ext_inner = phiId - charge*(m_ib_c0 + (m_ib_c1/ptId));

  log.debug(" Eta extrapolated Inner Barrel = " + fToStr(eta_ext_inner));
  log.debug(" Phi extrapolated Inner Barrel = " + fToStr(phi_ext_inner));

  return TVector2(eta_ext_inner, phi_ext_inner);
}
///===================================================
TVector2 TrigDiMuonExtrapolator::propagationInnerEndCap(Calib* t) {
  Log log("TrigDiMuonExtrapolator::propagationInnerEndCap", ol());

  return propagationInnerEndCap(t->Pt, t->Eta, t->Phi, t->Charge);

}
///===================================================
TVector2 TrigDiMuonExtrapolator::propagationInnerEndCap(double ptId, double etaId, double phiId, double charge) {
  Log log("TrigDiMuonExtrapolator::propagationInnerEndCap", ol());

  double eta_ext_inner = etaId;
  double phi_ext_inner = phiId;

  int bin;
  getBinIE(etaId, bin);

  phi_ext_inner = phiId - charge*(m_ie_c0[bin] + (m_ie_c1[bin] /ptId));

  log.debug(" Eta extrapolated Inner Endcap = " + fToStr(eta_ext_inner));
  log.debug(" Phi extrapolated Inner Endcap = " + fToStr(phi_ext_inner));

  return TVector2(eta_ext_inner, phi_ext_inner);

}
///===================================================
void TrigDiMuonExtrapolator::getBinME(double ptId, double etaId, double phiId, double charge, int& binEta, int& binPhi) {
  Log log("TrigDiMuonExtrapolator::getBinME", ol());

  double phi_ext_inner = propagationInnerEndCap(ptId, etaId, phiId, charge).Y(); // propagated phi coordinate

  // get phi octant
  int Octant = static_cast<int> (phi_ext_inner/PI_OVER_4);
  double PhiInOctant = fabs(phi_ext_inner-Octant*PI_OVER_4);
  if (PhiInOctant > PI_OVER_8) PhiInOctant = PI_OVER_4-PhiInOctant;
  int phiBin = static_cast<int>(PhiInOctant*30.557749073643904467625682567523);

  // get phi bins
  if (phiBin<=4)            binPhi = 0;
  if (phiBin>4 && phiBin<8) binPhi = 1;
  if (phiBin>=8)            binPhi = 2;

  // get eta and phi bins
  if (fabs(etaId)>2.)                        binEta = 0;
  if (fabs(etaId)>1.65 && fabs(etaId)<=2.)   binEta = 1;
  if (fabs(etaId)>1.35 && fabs(etaId)<=1.65) binEta = 2;
  if (fabs(etaId)>1.2 && fabs(etaId)<=1.35)  binEta = 3;
  if (fabs(etaId)<=1.2)                      binEta = 4;

}
///===================================================
void TrigDiMuonExtrapolator::fillMiddle(Calib* hits) {
  Log log("TrigDiMuonExtrapolator::fillMiddle", ol());

  // calculate fill value
  double x;
  if(m_oneOverPt)
    x = 1./hits->Pt;
  else
    x = hits->Pt;

  double ext_inner_phi = propagationInnerBarrel(hits).Y();

  // barrel
  if(fabs(hits->Eta) < 1.1) {
    // MDT
    std::vector<double>::iterator phiItr;
    std::vector<int>::iterator typeItr;

    std::vector<double>::iterator etaItr = hits->MiddleMdt_eta->begin();
    for(; etaItr!=hits->MiddleMdt_eta->end(); ++etaItr) {
      h_mb_dEta->Fill(x, hits->Charge*(hits->Eta - (*etaItr)));
      h2_mb_dEta->Fill(x, hits->Charge*(hits->Eta - (*etaItr)));
    }

//     phiItr = hits->MiddleMdt_phi->begin();
//     for(; phiItr!=hits->MiddleMdt_phi->end(); ++phiItr) {
//       h_mb_dPhi->Fill(x, hits->Charge*dAngle(ext_inner_phi, (*phiItr)));
//       h2_mb_dPhi->Fill(x, hits->Charge*dAngle(ext_inner_phi, (*phiItr)));
//     }

    // RPC
    etaItr = hits->MiddleRpc_eta->begin();
    typeItr = hits->MiddleRpc_type->begin();
    for(; etaItr!=hits->MiddleRpc_eta->end(); ++etaItr, ++typeItr) {
      if(*typeItr == 0) {
        h_mb_dEta->Fill(x, hits->Charge*(hits->Eta - (*etaItr)));
        h2_mb_dEta->Fill(x, hits->Charge*(hits->Eta - (*etaItr)));
      }
    }

    phiItr = hits->MiddleRpc_phi->begin();
    typeItr = hits->MiddleRpc_type->begin();
    for(; phiItr!=hits->MiddleRpc_phi->end(); ++phiItr, ++typeItr) {
      if(*typeItr == 1) {
        h_mb_dPhi->Fill(x, hits->Charge*dAngle(ext_inner_phi, (*phiItr)));
        h2_mb_dPhi->Fill(x, hits->Charge*dAngle(ext_inner_phi, (*phiItr)));
      }
    }
  }

  // endcap
  if(fabs(hits->Eta) > 0.9) {
    int binEta, binPhi;
    getBinME(hits->Pt, hits->Eta, hits->Phi, hits->Charge, binEta, binPhi);

    std::vector<int>::iterator typeItr;
    std::vector<double>::iterator phiItr;
    std::vector<double>::iterator etaItr;

    //MDT
    etaItr = hits->MiddleMdt_eta->begin();
    phiItr = hits->MiddleTgc_phi->begin();
    for(; etaItr!=hits->MiddleMdt_eta->end(); ++etaItr, ++phiItr) {
      h_me_dEta[binEta][binPhi]->Fill(x, hits->Charge*(hits->Eta - (*etaItr)));
      h2_me_dEta[binEta][binPhi]->Fill(x, hits->Charge*(hits->Eta - (*etaItr)));

      // dEta vs eta 2D histogram

      if(fabs(hits->Pt) < 2000)
        h_midEndcap_dEtaVsEta[0]->Fill(hits->Charge*(hits->Eta - (*etaItr)), fabs(hits->Eta));
      else if (fabs(hits->Pt) < 4000)
        h_midEndcap_dEtaVsEta[1]->Fill(hits->Charge*(hits->Eta - (*etaItr)), fabs(hits->Eta));
      else if (fabs(hits->Pt) < 6000)
        h_midEndcap_dEtaVsEta[2]->Fill(hits->Charge*(hits->Eta - (*etaItr)), fabs(hits->Eta));
      else
        h_midEndcap_dEtaVsEta[3]->Fill(hits->Charge*(hits->Eta - (*etaItr)), fabs(hits->Eta));

      // dEta vs phi 2D histogram
      if(binEta == m_dEtaVsPhiBin) {
          // get phi octant
        int Octant = static_cast<int> ( propagationInnerEndCap(hits).Y() / PI_OVER_4 );
        double PhiInOctant = fabs(propagationInnerEndCap(hits).Y() - Octant*PI_OVER_4);
        if (PhiInOctant > PI_OVER_8) PhiInOctant = PI_OVER_4-PhiInOctant;

        if(fabs(hits->Pt) < 2000)
          h_midEndcap_dEtaVsPhi[0]->Fill(hits->Charge*(hits->Eta - (*etaItr)), PhiInOctant);
        else if (fabs(hits->Pt) < 4000)
          h_midEndcap_dEtaVsPhi[1]->Fill(hits->Charge*(hits->Eta - (*etaItr)), PhiInOctant);
        else if (fabs(hits->Pt) < 6000)
          h_midEndcap_dEtaVsPhi[2]->Fill(hits->Charge*(hits->Eta - (*etaItr)), PhiInOctant);
        else
          h_midEndcap_dEtaVsPhi[3]->Fill(hits->Charge*(hits->Eta - (*etaItr)), PhiInOctant);
      }

    }

    //TGC
    etaItr = hits->MiddleTgc_eta->begin();
    phiItr = hits->MiddleTgc_phi->begin();
    typeItr = hits->MiddleTgc_type->begin();
    for(; etaItr!=hits->MiddleTgc_eta->end(); ++etaItr, ++typeItr, ++phiItr) {
      if(*typeItr == 0) {
        h_me_dEta[binEta][binPhi]->Fill(x, hits->Charge*(hits->Eta - (*etaItr)));
        h2_me_dEta[binEta][binPhi]->Fill(x, hits->Charge*(hits->Eta - (*etaItr)));

        // dEta vs eta 2D histogram
        if(fabs(hits->Pt) < 2000)
          h_midEndcap_dEtaVsEta[0]->Fill(hits->Charge*(hits->Eta - (*etaItr)), fabs(hits->Eta));
        else if (fabs(hits->Pt) < 4000)
          h_midEndcap_dEtaVsEta[1]->Fill(hits->Charge*(hits->Eta - (*etaItr)), fabs(hits->Eta));
        else if (fabs(hits->Pt) < 6000)
          h_midEndcap_dEtaVsEta[2]->Fill(hits->Charge*(hits->Eta - (*etaItr)), fabs(hits->Eta));
        else
          h_midEndcap_dEtaVsEta[3]->Fill(hits->Charge*(hits->Eta - (*etaItr)), fabs(hits->Eta));

        // dEta vs phi 2D histogram
        if(binEta == m_dEtaVsPhiBin) {
          // get phi octant
          int Octant = static_cast<int> ( propagationInnerEndCap(hits).Y() / PI_OVER_4 );
          double PhiInOctant = fabs(propagationInnerEndCap(hits).Y() - Octant*PI_OVER_4);
          if (PhiInOctant > PI_OVER_8) PhiInOctant = PI_OVER_4-PhiInOctant;

          if(fabs(hits->Pt) < 2000)
            h_midEndcap_dEtaVsPhi[0]->Fill(hits->Charge*(hits->Eta - (*etaItr)), PhiInOctant);
          else if (fabs(hits->Pt) < 4000)
            h_midEndcap_dEtaVsPhi[1]->Fill(hits->Charge*(hits->Eta - (*etaItr)), PhiInOctant);
          else if (fabs(hits->Pt) < 6000)
            h_midEndcap_dEtaVsPhi[2]->Fill(hits->Charge*(hits->Eta - (*etaItr)), PhiInOctant);
          else
            h_midEndcap_dEtaVsPhi[3]->Fill(hits->Charge*(hits->Eta - (*etaItr)), PhiInOctant);
        }
      }

    }

    // phi extrapolation
    binPhi = 0;
    if(fabs(propagationMiddleEndCap(hits).X()) > 2.) binPhi = 1;
    //MDT
//     phiItr = hits->MiddleMdt_phi->begin();
//     for(; phiItr!=hits->MiddleMdt_phi->end(); ++phiItr) {
//       //h_me_dPhi[binPhi]->Fill(x, hits->Charge*dAngle(ext_inner_phi, (*phiItr)));
//       //h2_me_dPhi[binPhi]->Fill(x, hits->Charge*dAngle(ext_inner_phi, (*phiItr)));
//       h_me_dPhi[binPhi]->Fill(x, hits->Charge*dAngle(hits->Phi, (*phiItr)));
//       h2_me_dPhi[binPhi]->Fill(x, hits->Charge*dAngle(hits->Phi, (*phiItr)));
//
//     }

    //TGC
    phiItr = hits->MiddleTgc_phi->begin();
    typeItr = hits->MiddleTgc_type->begin();
    for(; phiItr!=hits->MiddleTgc_phi->end(); ++phiItr, ++typeItr) {
      if(*typeItr == 1) {
        h_me_dPhi[binPhi]->Fill(x, hits->Charge*dAngle(hits->Phi, (*phiItr)));
        h2_me_dPhi[binPhi]->Fill(x, hits->Charge*dAngle(hits->Phi, (*phiItr)));
      }
    }

  }

}
///===================================================
void TrigDiMuonExtrapolator::drawMiddle(std::string settings) {
  Log log("TrigDiMuonExtrapolator::drawMiddle", ol());

  TCanvas* c;

  c = new TCanvas("c_mb_dEta","c_mb_dEta");
  c->Divide(2,1);

  c->cd(1);
  h2_mb_dEta->SetMarkerStyle(1);
  h2_mb_dEta->Draw();

  c->cd(2);
  h_mb_dEta->SetStats(0);
  h_mb_dEta->Draw();

  // fitting
  if( settings.find("fitMiddle") != std::string::npos) {
    h_mb_dEta->Fit(fun_mb, "R 0");
  }

  fun_mb->SetLineColor(kRed);
  fun_mb->Draw("same");

  c->Print("plots.ps");
  c->SaveAs("./fig/calibration/middleBarrel_dEta.C");

  for(int i=0; i<5; ++i) {
    for(int j=0; j<3; ++j) {
      c = new TCanvas(("c_me_dEta_"+iToStr(i)+"_"+iToStr(j)).c_str(),"c_me_dEta");
      c->Divide(2,1);

      c->cd(1);
      h2_me_dEta[i][j]->SetMarkerStyle(1);
      h2_me_dEta[i][j]->Draw();

      c->cd(2);
      h_me_dEta[i][j]->SetStats(0);
      h_me_dEta[i][j]->Draw();

      // fitting
      if( settings.find("fitMiddle") != std::string::npos) {
        h_me_dEta[i][j]->Fit(fun_me[i][j], "R 0");
      }

      fun_me[i][j]->SetLineColor(kRed);
      fun_me[i][j]->Draw("same");

      c->Print("plots.ps");
      c->SaveAs(("./fig/calibration/middleEndcap_dEta_bin_"+iToStr(i)+"x"+iToStr(j)+".C").c_str());
    }
  }

  c = new TCanvas("c_mb_dPhi","c_mb_dPhi");
  c->Divide(2,1);

  c->cd(1);
  h2_mb_dPhi->SetMarkerStyle(1);
  h2_mb_dPhi->Draw();

  c->cd(2);
  h_mb_dPhi->Draw();

  c->Print("plots.ps");
  c->SaveAs("./fig/calibration/middleBarrel_dPhi.C");

  for(int i=0; i<2; ++i) {
    c = new TCanvas(("c_me_dPhi_"+iToStr(i)).c_str(),"c_me_dPhi");
    c->Divide(2,1);

    c->cd(1);
    h2_me_dPhi[i]->SetMarkerStyle(1);
    h2_me_dPhi[i]->Draw();

    c->cd(2);
    h_me_dPhi[i]->Draw();

    if(i==1) {
      // fitting
      if( settings.find("fitMiddle") != std::string::npos) {
        h_me_dPhi[i]->Fit(fun_me_dPhi, "R 0");
      }
      fun_me_dPhi->SetLineColor(kRed);
      fun_me_dPhi->Draw("same");
    }
    c->Print("plots.ps");
    c->SaveAs(("./fig/calibration/middleEndcap_dPhi_bin_"+iToStr(i)+".C").c_str());

  }

  for(int i=0; i<4;i++) {
    c = new TCanvas(("c_endcap_dEta"+iToStr(i)).c_str(),("c_endcap_dEta"+iToStr(i)).c_str());
    h_midEndcap_dEtaVsEta[i]->SetMarkerStyle(1);
    h_midEndcap_dEtaVsEta[i]->Draw("box");

    TLine* l;

    l = new TLine(-1.5, 1.2, 1., 1.2);
    l->SetLineColor(kRed);
    l->Draw();

    l = new TLine(-1.5, 1.35, 1., 1.35);
    l->SetLineColor(kRed);
    l->Draw();

    l = new TLine(-1.5, 1.65, 1., 1.65);
    l->SetLineColor(kRed);
    l->Draw();

    l = new TLine(-1.5, 2., 1., 2.);
    l->SetLineColor(kRed);
    l->Draw();
    c->Print("plots.ps");
    switch(i) {
      case 0: c->SaveAs("./fig/calibration/middleEndcap_dEta_vs_eta_pT_0-2GeV.C"); break;
      case 1: c->SaveAs("./fig/calibration/middleEndcap_dEta_vs_eta_pT_2-4GeV.C"); break;
      case 2: c->SaveAs("./fig/calibration/middleEndcap_dEta_vs_eta_pT_4-6GeV.C"); break;
      case 3: c->SaveAs("./fig/calibration/middleEndcap_dEta_vs_eta_pT_6-infGeV.C"); break;
    }
  }

  for(int i=0; i<4;i++) {
    c = new TCanvas(("c_endcap_dEtaVsPhi"+iToStr(i)).c_str(),("c_endcap_dEtaVsPhi"+iToStr(i)).c_str());
    h_midEndcap_dEtaVsPhi[i]->SetMarkerStyle(1);
    h_midEndcap_dEtaVsPhi[i]->Draw("box");

    TLine* l;

    l = new TLine(-1.5, 5./30.557749073643904467625682567523, 0.5, 5./30.557749073643904467625682567523);
    l->SetLineColor(kRed);
    l->Draw();

    l = new TLine(-1.5, 8./30.557749073643904467625682567523, 0.5, 8./30.557749073643904467625682567523);
    l->SetLineColor(kRed);
    l->Draw();

    c->Print("plots.ps");
    switch(i) {
      case 0: c->SaveAs("./fig/calibration/middleEndcap_dEta_vs_phi_pT_0-2GeV.C"); break;
      case 1: c->SaveAs("./fig/calibration/middleEndcap_dEta_vs_phi_pT_2-4GeV.C"); break;
      case 2: c->SaveAs("./fig/calibration/middleEndcap_dEta_vs_phi_pT_4-6GeV.C"); break;
      case 3: c->SaveAs("./fig/calibration/middleEndcap_dEta_vs_phi_pT_6-infGeV.C"); break;
    }
  }

  c->Print("plots.ps]");

}
///===================================================
TVector2 TrigDiMuonExtrapolator::propagationMiddleBarrel(Calib* t) {
  Log log("TrigDiMuonExtrapolator::propagationMiddleBarrel", ol());

  return propagationMiddleBarrel(t->Pt, t->Eta, t->Phi, t->Charge);

}
///===================================================
TVector2 TrigDiMuonExtrapolator::propagationMiddleBarrel(double ptId, double etaId, double phiId, double charge) {
  Log log("TrigDiMuonExtrapolator::propagationMiddleBarrel", ol());

  double ptId2  = ptId * ptId;

  double eta_ext_middle = etaId - charge*(m_mb_c0 + ( m_mb_c1 /ptId)+( m_mb_c2 /ptId2));
  double phi_ext_middle = propagationInnerBarrel(ptId, etaId, phiId, charge).Y();

  log.debug(" Eta extrapolated Middle Barrel = " + fToStr(eta_ext_middle));
  log.debug(" Phi extrapolated Middle Barrel = " + fToStr(phi_ext_middle));

  return TVector2(eta_ext_middle, phi_ext_middle);

}
///===================================================
TVector2 TrigDiMuonExtrapolator::propagationMiddleEndCap(Calib* t) {
  Log log("TrigDiMuonExtrapolator::propagationMiddleEndCap", ol());

  return propagationMiddleEndCap(t->Pt, t->Eta, t->Phi, t->Charge);

}
///===================================================
TVector2 TrigDiMuonExtrapolator::propagationMiddleEndCap(double ptId, double etaId, double phiId, double charge) {
  Log log("TrigDiMuonExtrapolator::propagationMiddleEndCap", ol());

  double ptId2  = ptId * ptId;

  double eta_ext_middle = etaId;
  double phi_ext_middle = phiId;
  double phi_ext_inner  = propagationInnerEndCap(ptId, etaId, phiId, charge).Y();

  int binEta;
  int binPhi;
  getBinME(ptId, etaId, phiId, charge, binEta, binPhi);

  //propagation to Middle stations
  eta_ext_middle = etaId - charge*(m_me_c0[binEta][binPhi] + (m_me_c1[binEta][binPhi] / ptId)+( m_me_c2[binEta][binPhi] /ptId2));
  // propagation in phi

  phi_ext_middle = phi_ext_inner;

  if (fabs(eta_ext_middle)>=2.) {
    phi_ext_middle = phiId - charge*(m_me_dPhi_c0 + ( m_me_dPhi_c1 /ptId));
  }

  log.debug(" Eta extrapolated Middle Endcap = " + fToStr(eta_ext_middle));
  log.debug(" Phi extrapolated Middle Endcap = " + fToStr(phi_ext_middle));

  return TVector2(eta_ext_middle, phi_ext_middle);


}
///===================================================
bool TrigDiMuonExtrapolator::matchMdtInner(double r, double z,
                                           double ptId,
                                           double etaId,
                                           double phiId,
                                           double charge)
{
  double eta = propagationInnerEndCap(ptId, etaId, phiId, charge).X();
  double window_mdt_eta = m_MdtEtaFactor*m_cone_Endcap_mdt_inner_dEta_c0/pow(ptId,m_cone_Endcap_mdt_inner_dEta_c1);
  if(fabs(etaId) < 1.) {
    eta = propagationInnerBarrel(ptId, etaId, phiId, charge).X();
    window_mdt_eta = m_MdtEtaFactor*m_cone_Barrel_mdt_inner_dEta_c0/pow(ptId,m_cone_Barrel_mdt_inner_dEta_c1);
  }

  double eta_hit = getEta(r,z);

  return fabs(eta_hit - eta) < window_mdt_eta;

}
///===================================================
bool TrigDiMuonExtrapolator::matchMdtMiddle(double r, double z,
                                            double ptId,
                                            double etaId,
                                            double phiId,
                                            double charge)
{
  double eta = propagationMiddleEndCap(ptId, etaId, phiId, charge).X();
  double window_mdt_eta_m = m_MdtEtaFactor*m_cone_Endcap_mdt_middle_dEta_c0/pow(ptId,m_cone_Endcap_mdt_middle_dEta_c1);
  if(fabs(etaId) < 1.) {
    eta = propagationMiddleBarrel(ptId, etaId, phiId, charge).X();
    window_mdt_eta_m = m_MdtEtaFactor*m_cone_Barrel_mdt_middle_dEta_c0/pow(ptId,m_cone_Barrel_mdt_middle_dEta_c1);
  }

  double eta_hit = getEta(r,z);

  return fabs(eta_hit - eta) < window_mdt_eta_m;
}
///===================================================
bool TrigDiMuonExtrapolator::matchRpcEtaPad(double r, double z,
                                            double ptId,
                                            double etaId,
                                            double phiId,
                                            double charge)
{
  //Log log("TrigDiMuonExtrapolator::matchRpcEtaPad", ol());
  double eta = propagationMiddleBarrel(ptId, etaId, phiId, charge).X();
  double window_rpc_eta = m_RpcEtaFactor*m_cone_rpc_middle_dEta_c0/pow(ptId,m_cone_rpc_middle_dEta_c1);

  //log.info("window_rpc_eta="+fToStr(window_rpc_eta));

  double eta_hit = getEta(r,z);

  //log.info("r="+fToStr(r)+", z="+fToStr(z)+", eta_hit="+fToStr(eta_hit)+", etaId="+fToStr(etaId)+", eta="+fToStr(eta)+", dEta="+fToStr(fabs(eta_hit - eta)));

  return fabs(eta_hit - eta) < window_rpc_eta;

}
///===================================================
bool TrigDiMuonExtrapolator::matchRpcPhiPad(double x, double y,
                                            double ptId,
                                            double etaId,
                                            double phiId,
                                            double charge)
{
  double phi = propagationMiddleBarrel(ptId, etaId, phiId, charge).Y();
  double window_rpc_phi = m_RpcPhiFactor*m_cone_rpc_middle_dPhi_c0/pow(ptId,m_cone_rpc_middle_dPhi_c1);

  double phi_hit = atan2(y,x);

  return fabs(dAngle(phi_hit, phi)) < window_rpc_phi;
}
///===================================================
bool TrigDiMuonExtrapolator::matchInnerTgcPhiHit(double phi_hit,
                                                 double ptId,
                                                 double etaId,
                                                 double phiId,
                                                 double charge)
{
  double phi = propagationInnerEndCap(ptId, etaId, phiId, charge).Y();

  double window_phi_tgc_inner = m_TgcPhiFactor*0.005;
  if (ptId<20000.) window_phi_tgc_inner = m_TgcPhiFactor*m_cone_tgc_strip_inner_dPhi_c0/pow(ptId,m_cone_tgc_strip_inner_dPhi_c1);

  return fabs(dAngle(phi_hit, phi)) < window_phi_tgc_inner;

}
///===================================================
bool TrigDiMuonExtrapolator::matchInnerTgcRhoHit(double r, double z,
                                                 double phi_hit,
                                                 double ptId,
                                                 double etaId,
                                                 double phiId,
                                                 double charge)
{
  double eta = propagationInnerEndCap(ptId, etaId, phiId, charge).X();
  double phi = propagationInnerEndCap(ptId, etaId, phiId, charge).Y();

  double window_eta_tgc_inner = m_TgcEtaFactor*0.01;
  if (ptId<10000.) window_eta_tgc_inner = m_TgcEtaFactor*m_cone_tgc_wire_inner_dEta_c0/pow(ptId,m_cone_tgc_wire_inner_dEta_c1);

  double eta_hit = getEta(r,z);

  return fabs(eta_hit - eta) < window_eta_tgc_inner && fabs(dAngle(phi_hit, phi)) < 0.2;

}
///===================================================
bool TrigDiMuonExtrapolator::matchMiddleTgcPhiHit(double phi_hit,
                                                  double ptId,
                                                  double etaId,
                                                  double phiId,
                                                  double charge)
{
  double phi = propagationMiddleEndCap(ptId, etaId, phiId, charge).Y();
  double eta = propagationMiddleEndCap(ptId, etaId, phiId, charge).X();

  double window_phi_tgc_middle = m_TgcPhiFactor*m_cone_tgc_strip_middle_dPhi_c0/pow(ptId,m_cone_tgc_strip_middle_dPhi_c1);
  if (fabs(eta)>1.7) window_phi_tgc_middle = m_TgcPhiFactor*m_cone_tgc_strip_middle2_dPhi_c0/pow(ptId,m_cone_tgc_strip_middle2_dPhi_c1);

  return fabs(dAngle(phi_hit, phi)) < window_phi_tgc_middle;

}
///===================================================
bool TrigDiMuonExtrapolator::matchMiddleTgcRhoHit(double r, double z,
                                                  double phi_hit,
                                                  double ptId,
                                                  double etaId,
                                                  double phiId,
                                                  double charge)
{
  double eta = propagationMiddleEndCap(ptId, etaId, phiId, charge).X();
  double phi = propagationMiddleEndCap(ptId, etaId, phiId, charge).Y();

  double window_eta_tgc_middle = m_TgcEtaFactor*m_cone_tgc_wire_middle_dEta_c0/pow(ptId,m_cone_tgc_wire_middle_dEta_c1);

  double eta_hit = getEta(r,z);

  return fabs(eta_hit - eta) < window_eta_tgc_middle && fabs(dAngle(phi_hit, phi)) < 0.2;
}
///===================================================
void TrigDiMuonExtrapolator::read(std::string file) {
  Log log("TrigDiMuonExtrapolator::read", ol());

  log.info("Reading constants file: "+file);

  ifstream fin(file.c_str());

  const int maxLength = 1000;
  char buffer[maxLength];

  while( fin.getline(buffer, maxLength) ) {
    string line(buffer);

    // resolve comments: everything after # will be removed
    line = line.substr(0,line.find_first_of("#"));

    log.debug(line);

    /// Inner station ****
    //Barrel
    getValue(line,"InnerBarrel_c0=", m_ib_c0);
    getValue(line,"InnerBarrel_c1=", m_ib_c1);

    //Endcap
    //bin 1 : fabs(etaId)>1.6
    getValue(line, "InnerEndcap_etaBin1_c0=", m_ie_c0[0]);
    getValue(line, "InnerEndcap_etaBin1_c1=", m_ie_c1[0]);

    //bin 2 : fabs(line, etaId)>1.3 && fabs(line, etaId)<=1.6
    getValue(line, "InnerEndcap_etaBin2_c0=", m_ie_c0[1]);
    getValue(line, "InnerEndcap_etaBin2_c1=", m_ie_c1[1]);

    //bin 3 : fabs(line, etaId)<=1.3
    getValue(line, "InnerEndcap_etaBin3_c0=", m_ie_c0[2]);
    getValue(line, "InnerEndcap_etaBin3_c1=", m_ie_c1[2]);

    /// Middle station ****
    //Barrel
    getValue(line, "MiddleBarrel_c0=", m_mb_c0 );
    getValue(line, "MiddleBarrel_c1=", m_mb_c1 );
    getValue(line, "MiddleBarrel_c2=", m_mb_c2 );

    //Endcap
    //etaBin1 : fabs(etaId)>2.
    getValue(line, "MiddleEndcap_etaBin1_phiBin1_c0=", m_me_c0[0][0] );
    getValue(line, "MiddleEndcap_etaBin1_phiBin1_c1=", m_me_c1[0][0] );
    getValue(line, "MiddleEndcap_etaBin1_phiBin1_c2=", m_me_c2[0][0] );

    getValue(line, "MiddleEndcap_etaBin1_phiBin2_c0=", m_me_c0[0][1] );
    getValue(line, "MiddleEndcap_etaBin1_phiBin2_c1=", m_me_c1[0][1] );
    getValue(line, "MiddleEndcap_etaBin1_phiBin2_c2=", m_me_c2[0][1] );

    getValue(line, "MiddleEndcap_etaBin1_phiBin3_c0=", m_me_c0[0][2] );
    getValue(line, "MiddleEndcap_etaBin1_phiBin3_c1=", m_me_c1[0][2] );
    getValue(line, "MiddleEndcap_etaBin1_phiBin3_c2=", m_me_c2[0][2] );

    //etaBin2 : fabs(etaId)>1.65 && fabs(etaId)<=2.
    getValue(line, "MiddleEndcap_etaBin2_phiBin1_c0=", m_me_c0[1][0] );
    getValue(line, "MiddleEndcap_etaBin2_phiBin1_c1=", m_me_c1[1][0] );
    getValue(line, "MiddleEndcap_etaBin2_phiBin1_c2=", m_me_c2[1][0] );

    getValue(line, "MiddleEndcap_etaBin2_phiBin2_c0=", m_me_c0[1][1] );
    getValue(line, "MiddleEndcap_etaBin2_phiBin2_c1=", m_me_c1[1][1] );
    getValue(line, "MiddleEndcap_etaBin2_phiBin2_c2=", m_me_c2[1][1] );

    getValue(line, "MiddleEndcap_etaBin2_phiBin3_c0=", m_me_c0[1][2] );
    getValue(line, "MiddleEndcap_etaBin2_phiBin3_c1=", m_me_c1[1][2] );
    getValue(line, "MiddleEndcap_etaBin2_phiBin3_c2=", m_me_c2[1][2] );

    //etaBin3 : fabs(etaId)>1.35 && fabs(etaId)<=1.65
    getValue(line, "MiddleEndcap_etaBin3_phiBin1_c0=", m_me_c0[2][0] );
    getValue(line, "MiddleEndcap_etaBin3_phiBin1_c1=", m_me_c1[2][0] );
    getValue(line, "MiddleEndcap_etaBin3_phiBin1_c2=", m_me_c2[2][0] );

    getValue(line, "MiddleEndcap_etaBin3_phiBin2_c0=", m_me_c0[2][1] );
    getValue(line, "MiddleEndcap_etaBin3_phiBin2_c1=", m_me_c1[2][1] );
    getValue(line, "MiddleEndcap_etaBin3_phiBin2_c2=", m_me_c2[2][1] );

    getValue(line, "MiddleEndcap_etaBin3_phiBin3_c0=", m_me_c0[2][2] );
    getValue(line, "MiddleEndcap_etaBin3_phiBin3_c1=", m_me_c1[2][2] );
    getValue(line, "MiddleEndcap_etaBin3_phiBin3_c2=", m_me_c2[2][2] );

    //etaBin4 : fabs(etaId)>1.2 && fabs(etaId)<=1.35
    getValue(line, "MiddleEndcap_etaBin4_phiBin1_c0=", m_me_c0[3][0] );
    getValue(line, "MiddleEndcap_etaBin4_phiBin1_c1=", m_me_c1[3][0] );
    getValue(line, "MiddleEndcap_etaBin4_phiBin1_c2=", m_me_c2[3][0] );

    getValue(line, "MiddleEndcap_etaBin4_phiBin2_c0=", m_me_c0[3][1] );
    getValue(line, "MiddleEndcap_etaBin4_phiBin2_c1=", m_me_c1[3][1] );
    getValue(line, "MiddleEndcap_etaBin4_phiBin2_c2=", m_me_c2[3][1] );

    getValue(line, "MiddleEndcap_etaBin4_phiBin3_c0=", m_me_c0[3][2] );
    getValue(line, "MiddleEndcap_etaBin4_phiBin3_c1=", m_me_c1[3][2] );
    getValue(line, "MiddleEndcap_etaBin4_phiBin3_c2=", m_me_c2[3][2] );

    //etaBin5 : fabs(etaId)<=1.2
    getValue(line, "MiddleEndcap_etaBin5_phiBin1_c0=", m_me_c0[4][0] );
    getValue(line, "MiddleEndcap_etaBin5_phiBin1_c1=", m_me_c1[4][0] );
    getValue(line, "MiddleEndcap_etaBin5_phiBin1_c2=", m_me_c2[4][0] );

    getValue(line, "MiddleEndcap_etaBin5_phiBin2_c0=", m_me_c0[4][1] );
    getValue(line, "MiddleEndcap_etaBin5_phiBin2_c1=", m_me_c1[4][1] );
    getValue(line, "MiddleEndcap_etaBin5_phiBin2_c2=", m_me_c2[4][1] );

    getValue(line, "MiddleEndcap_etaBin5_phiBin3_c0=", m_me_c0[4][2] );
    getValue(line, "MiddleEndcap_etaBin5_phiBin3_c1=", m_me_c1[4][2] );
    getValue(line, "MiddleEndcap_etaBin5_phiBin3_c2=", m_me_c2[4][2] );

    getValue(line, "MiddleEndcap_c0=", m_me_dPhi_c0 );
    getValue(line, "MiddleEndcap_c1=", m_me_dPhi_c1 );

  }

  // set parameters to the functions
  // inner station
  fun_ib->SetParameters(m_ib_c0, m_ib_c1);

  for(int i=0; i<3; ++i) {
    fun_ie[i]->SetParameters(m_ie_c0[i], m_ie_c1[i]);
  }

  // middle station
  fun_mb->SetParameters(m_mb_c0, m_mb_c1, m_mb_c2);

  for(int i=0; i<5; ++i) {
    for(int j=0; j<3; ++j) {
      fun_me[i][j]->SetParameters(m_me_c0[i][j], m_me_c1[i][j], m_me_c2[i][j]);
    }
  }

  fun_me_dPhi->SetParameters(m_me_dPhi_c0, m_me_dPhi_c1);
}
///===================================================
void TrigDiMuonExtrapolator::readCutCone(std::string file) {
  Log log("TrigDiMuonExtrapolator::readCutCone", ol());

  log.info("Reading cut-cone file: "+file);

  ifstream fin(file.c_str());

  const int maxLength = 1000;
  char buffer[maxLength];

  while( fin.getline(buffer, maxLength) ) {
    string line(buffer);

    // resolve comments: everything after # will be removed
    line = line.substr(0,line.find_first_of("#"));

    log.debug(line);

    getValue(line,"Cone_Endcap_mdt_inner_dEta_c0=",   m_cone_Endcap_mdt_inner_dEta_c0);
    getValue(line,"Cone_Endcap_mdt_inner_dEta_c1=",   m_cone_Endcap_mdt_inner_dEta_c1);

    getValue(line,"Cone_Endcap_mdt_middle_dEta_c0=",  m_cone_Endcap_mdt_middle_dEta_c0);
    getValue(line,"Cone_Endcap_mdt_middle_dEta_c1=",  m_cone_Endcap_mdt_middle_dEta_c1);

    getValue(line,"Cone_Barrel_mdt_inner_dEta_c0=",   m_cone_Barrel_mdt_inner_dEta_c0);
    getValue(line,"Cone_Barrel_mdt_inner_dEta_c1=",   m_cone_Barrel_mdt_inner_dEta_c1);

    getValue(line,"Cone_Barrel_mdt_middle_dEta_c0=",  m_cone_Barrel_mdt_middle_dEta_c0);
    getValue(line,"Cone_Barrel_mdt_middle_dEta_c1=",  m_cone_Barrel_mdt_middle_dEta_c1);

    getValue(line,"Cone_rpc_middle_dEta_c0=",         m_cone_rpc_middle_dEta_c0);
    getValue(line,"Cone_rpc_middle_dEta_c1=",         m_cone_rpc_middle_dEta_c1);

    getValue(line,"Cone_rpc_middle_dPhi_c0=",         m_cone_rpc_middle_dPhi_c0);
    getValue(line,"Cone_rpc_middle_dPhi_c1=",         m_cone_rpc_middle_dPhi_c1);

    getValue(line,"Cone_tgc_wire_inner_dEta_c0=",     m_cone_tgc_wire_inner_dEta_c0);
    getValue(line,"Cone_tgc_wire_inner_dEta_c1=",     m_cone_tgc_wire_inner_dEta_c1);

    getValue(line,"Cone_tgc_wire_middle_dEta_c0=",    m_cone_tgc_wire_middle_dEta_c0);
    getValue(line,"Cone_tgc_wire_middle_dEta_c1=",    m_cone_tgc_wire_middle_dEta_c1);

    getValue(line,"Cone_tgc_strip_inner_dPhi_c0=",    m_cone_tgc_strip_inner_dPhi_c0);
    getValue(line,"Cone_tgc_strip_inner_dPhi_c1=",    m_cone_tgc_strip_inner_dPhi_c1);

    getValue(line,"Cone_tgc_strip_middle_dPhi_c0=",   m_cone_tgc_strip_middle_dPhi_c0);
    getValue(line,"Cone_tgc_strip_middle_dPhi_c1=",   m_cone_tgc_strip_middle_dPhi_c1);
    getValue(line,"Cone_tgc_strip_middle2_dPhi_c0=",   m_cone_tgc_strip_middle2_dPhi_c0);
    getValue(line,"Cone_tgc_strip_middle2_dPhi_c1=",   m_cone_tgc_strip_middle2_dPhi_c1);

  }

  log.info("Cone_Endcap_mdt_inner_dEta_c0=" + fToStr(   m_cone_Endcap_mdt_inner_dEta_c0));
  log.info("Cone_Endcap_mdt_inner_dEta_c1=" + fToStr(   m_cone_Endcap_mdt_inner_dEta_c1));

  log.info("Cone_Endcap_mdt_middle_dEta_c0=" + fToStr(  m_cone_Endcap_mdt_middle_dEta_c0));
  log.info("Cone_Endcap_mdt_middle_dEta_c1=" + fToStr(  m_cone_Endcap_mdt_middle_dEta_c1));

  log.info("Cone_Barrel_mdt_inner_dEta_c0=" + fToStr(   m_cone_Barrel_mdt_inner_dEta_c0));
  log.info("Cone_Barrel_mdt_inner_dEta_c1=" + fToStr(   m_cone_Barrel_mdt_inner_dEta_c1));

  log.info("Cone_Barrel_mdt_middle_dEta_c0=" + fToStr(  m_cone_Barrel_mdt_middle_dEta_c0));
  log.info("Cone_Barrel_mdt_middle_dEta_c1=" + fToStr(  m_cone_Barrel_mdt_middle_dEta_c1));

  log.info("Cone_rpc_middle_dEta_c0=" + fToStr(         m_cone_rpc_middle_dEta_c0));
  log.info("Cone_rpc_middle_dEta_c1=" + fToStr(         m_cone_rpc_middle_dEta_c1));

  log.info("Cone_rpc_middle_dPhi_c0=" + fToStr(         m_cone_rpc_middle_dPhi_c0));
  log.info("Cone_rpc_middle_dPhi_c1=" + fToStr(         m_cone_rpc_middle_dPhi_c1));

  log.info("Cone_tgc_wire_inner_dEta_c0=" + fToStr(     m_cone_tgc_wire_inner_dEta_c0));
  log.info("Cone_tgc_wire_inner_dEta_c1=" + fToStr(     m_cone_tgc_wire_inner_dEta_c1));

  log.info("Cone_tgc_wire_middle_dEta_c0=" + fToStr(    m_cone_tgc_wire_middle_dEta_c0));
  log.info("Cone_tgc_wire_middle_dEta_c1=" + fToStr(    m_cone_tgc_wire_middle_dEta_c1));

  log.info("Cone_tgc_strip_inner_dPhi_c0=" + fToStr(    m_cone_tgc_strip_inner_dPhi_c0));
  log.info("Cone_tgc_strip_inner_dPhi_c1=" + fToStr(    m_cone_tgc_strip_inner_dPhi_c1));

  log.info("Cone_tgc_strip_middle_dPhi_c0=" + fToStr(   m_cone_tgc_strip_middle_dPhi_c0));
  log.info("Cone_tgc_strip_middle_dPhi_c1=" + fToStr(   m_cone_tgc_strip_middle_dPhi_c1));
  log.info("Cone_tgc_strip_middle2_dPhi_c0=" + fToStr(   m_cone_tgc_strip_middle2_dPhi_c0));
  log.info("Cone_tgc_strip_middle2_dPhi_c1=" + fToStr(   m_cone_tgc_strip_middle2_dPhi_c1));

}
///===================================================
void TrigDiMuonExtrapolator::save(std::string file) {
  Log log("TrigDiMuonExtrapolator::saveInner", ol());

  // retrieve parameters
  // inner station
  m_ib_c0 = fun_ib->GetParameter(0);
  m_ib_c1 = fun_ib->GetParameter(1);

  for(int i=0; i<3; ++i) {
    m_ie_c0[i] = fun_ie[i]->GetParameter(0);
    m_ie_c1[i] = fun_ie[i]->GetParameter(1);
  }

  // middle station
  m_mb_c0 = fun_mb->GetParameter(0);
  m_mb_c1 = fun_mb->GetParameter(1);
  m_mb_c2 = fun_mb->GetParameter(2);

  for(int i=0; i<5; ++i) {
    for(int j=0; j<3; ++j) {
      m_me_c0[i][j] = fun_me[i][j]->GetParameter(0);
      m_me_c1[i][j] = fun_me[i][j]->GetParameter(1);
      m_me_c2[i][j] = fun_me[i][j]->GetParameter(2);
    }
  }

  m_me_dPhi_c0 = fun_me_dPhi->GetParameter(0);
  m_me_dPhi_c1 = fun_me_dPhi->GetParameter(1);

  ofstream fout(file.c_str(), ios::out);

  fout << "## Generated by TrigDiMuonRootCalib algorithm " << std::endl;
  fout << std::endl;

  // Inner station

  fout << "## Inner station ##########" << std::endl;
  fout << "## Barrel" << std::endl;
  fout << "InnerBarrel_c0=" << m_ib_c0 << std::endl;
  fout << "InnerBarrel_c1=" << m_ib_c1 << std::endl;
  fout << std::endl;
  fout << "## Endcap" << std::endl;
  fout << "## bin 1 : fabs(etaId)>1.6" << std::endl;
  fout <<  "InnerEndcap_etaBin1_c0=" << m_ie_c0[0] << std::endl;
  fout <<  "InnerEndcap_etaBin1_c1=" << m_ie_c1[0] << std::endl;
  fout << std::endl;
  fout << "## bin 2 : fabs(line << etaId)>1.3 && fabs(line << etaId)<=1.6" << std::endl;
  fout <<  "InnerEndcap_etaBin2_c0=" << m_ie_c0[1] << std::endl;
  fout <<  "InnerEndcap_etaBin2_c1=" << m_ie_c1[1] << std::endl;
  fout << std::endl;
  fout << "## bin 3 : fabs(line << etaId)<=1.3" << std::endl;
  fout <<  "InnerEndcap_etaBin3_c0=" << m_ie_c0[2] << std::endl;
  fout <<  "InnerEndcap_etaBin3_c1=" << m_ie_c1[2] << std::endl;

  // Middle station

  fout << std::endl;
  fout << "## Middle station ##########" << std::endl;
  fout << "## Barrel" << std::endl;
  fout << "MiddleBarrel_c0=" << m_mb_c0 << std::endl;
  fout << "MiddleBarrel_c1=" << m_mb_c1 << std::endl;
  fout << "MiddleBarrel_c2=" << m_mb_c2 << std::endl;
  fout << std::endl;
  fout << "## Endcap"  << std::endl;
  fout << "## etaBin1 : fabs(etaId)>2."  << std::endl;
  fout << "MiddleEndcap_etaBin1_phiBin1_c0=" << m_me_c0[0][0]  << std::endl;
  fout << "MiddleEndcap_etaBin1_phiBin1_c1=" << m_me_c1[0][0]  << std::endl;
  fout << "MiddleEndcap_etaBin1_phiBin1_c2=" << m_me_c2[0][0]  << std::endl;
  fout << std::endl;
  fout << "MiddleEndcap_etaBin1_phiBin2_c0=" << m_me_c0[0][1]  << std::endl;
  fout << "MiddleEndcap_etaBin1_phiBin2_c1=" << m_me_c1[0][1]  << std::endl;
  fout << "MiddleEndcap_etaBin1_phiBin2_c2=" << m_me_c2[0][1]  << std::endl;
  fout << std::endl;
  fout << "MiddleEndcap_etaBin1_phiBin3_c0=" << m_me_c0[0][2]  << std::endl;
  fout << "MiddleEndcap_etaBin1_phiBin3_c1=" << m_me_c1[0][2]  << std::endl;
  fout << "MiddleEndcap_etaBin1_phiBin3_c2=" << m_me_c2[0][2]  << std::endl;
  fout << std::endl;
  fout << "## etaBin2 : fabs(etaId)>1.65 && fabs(etaId)<=2."  << std::endl;
  fout << "MiddleEndcap_etaBin2_phiBin1_c0=" << m_me_c0[1][0]  << std::endl;
  fout << "MiddleEndcap_etaBin2_phiBin1_c1=" << m_me_c1[1][0]  << std::endl;
  fout << "MiddleEndcap_etaBin2_phiBin1_c2=" << m_me_c2[1][0]  << std::endl;
  fout << std::endl;
  fout << "MiddleEndcap_etaBin2_phiBin2_c0=" << m_me_c0[1][1]  << std::endl;
  fout << "MiddleEndcap_etaBin2_phiBin2_c1=" << m_me_c1[1][1]  << std::endl;
  fout << "MiddleEndcap_etaBin2_phiBin2_c2=" << m_me_c2[1][1]  << std::endl;
  fout << std::endl;
  fout << "MiddleEndcap_etaBin2_phiBin3_c0=" << m_me_c0[1][2]  << std::endl;
  fout << "MiddleEndcap_etaBin2_phiBin3_c1=" << m_me_c1[1][2]  << std::endl;
  fout << "MiddleEndcap_etaBin2_phiBin3_c2=" << m_me_c2[1][2]  << std::endl;
  fout << std::endl;
  fout << "## etaBin3 : fabs(etaId)>1.35 && fabs(etaId)<=1.65"  << std::endl;
  fout << "MiddleEndcap_etaBin3_phiBin1_c0=" << m_me_c0[2][0]  << std::endl;
  fout << "MiddleEndcap_etaBin3_phiBin1_c1=" << m_me_c1[2][0]  << std::endl;
  fout << "MiddleEndcap_etaBin3_phiBin1_c2=" << m_me_c2[2][0]  << std::endl;
  fout << std::endl;
  fout << "MiddleEndcap_etaBin3_phiBin2_c0=" << m_me_c0[2][1]  << std::endl;
  fout << "MiddleEndcap_etaBin3_phiBin2_c1=" << m_me_c1[2][1]  << std::endl;
  fout << "MiddleEndcap_etaBin3_phiBin2_c2=" << m_me_c2[2][1]  << std::endl;
  fout << std::endl;
  fout << "MiddleEndcap_etaBin3_phiBin3_c0=" << m_me_c0[2][2]  << std::endl;
  fout << "MiddleEndcap_etaBin3_phiBin3_c1=" << m_me_c1[2][2]  << std::endl;
  fout << "MiddleEndcap_etaBin3_phiBin3_c2=" << m_me_c2[2][2]  << std::endl;
  fout << std::endl;
  fout << "## etaBin4 : fabs(etaId)>1.2 && fabs(etaId)<=1.35"  << std::endl;
  fout << "MiddleEndcap_etaBin4_phiBin1_c0=" << m_me_c0[3][0]  << std::endl;
  fout << "MiddleEndcap_etaBin4_phiBin1_c1=" << m_me_c1[3][0]  << std::endl;
  fout << "MiddleEndcap_etaBin4_phiBin1_c2=" << m_me_c2[3][0]  << std::endl;
  fout << std::endl;
  fout << "MiddleEndcap_etaBin4_phiBin2_c0=" << m_me_c0[3][1]  << std::endl;
  fout << "MiddleEndcap_etaBin4_phiBin2_c1=" << m_me_c1[3][1]  << std::endl;
  fout << "MiddleEndcap_etaBin4_phiBin2_c2=" << m_me_c2[3][1]  << std::endl;
  fout << std::endl;
  fout << "MiddleEndcap_etaBin4_phiBin3_c0=" << m_me_c0[3][2]  << std::endl;
  fout << "MiddleEndcap_etaBin4_phiBin3_c1=" << m_me_c1[3][2]  << std::endl;
  fout << "MiddleEndcap_etaBin4_phiBin3_c2=" << m_me_c2[3][2]  << std::endl;
  fout << std::endl;
  fout << "## etaBin5 : fabs(etaId)<=1.2"  << std::endl;
  fout << "MiddleEndcap_etaBin5_phiBin1_c0=" << m_me_c0[4][0]  << std::endl;
  fout << "MiddleEndcap_etaBin5_phiBin1_c1=" << m_me_c1[4][0]  << std::endl;
  fout << "MiddleEndcap_etaBin5_phiBin1_c2=" << m_me_c2[4][0]  << std::endl;
  fout << std::endl;
  fout << "MiddleEndcap_etaBin5_phiBin2_c0=" << m_me_c0[4][1]  << std::endl;
  fout << "MiddleEndcap_etaBin5_phiBin2_c1=" << m_me_c1[4][1]  << std::endl;
  fout << "MiddleEndcap_etaBin5_phiBin2_c2=" << m_me_c2[4][1]  << std::endl;
  fout << std::endl;
  fout << "MiddleEndcap_etaBin5_phiBin3_c0=" << m_me_c0[4][2]  << std::endl;
  fout << "MiddleEndcap_etaBin5_phiBin3_c1=" << m_me_c1[4][2]  << std::endl;
  fout << "MiddleEndcap_etaBin5_phiBin3_c2=" << m_me_c2[4][2]  << std::endl;
  fout << std::endl;
  fout << "MiddleEndcap_c0=" << m_me_dPhi_c0  << std::endl;
  fout << "MiddleEndcap_c1=" << m_me_dPhi_c1  << std::endl;

  fout.close();

}
///===================================================

#endif

///===================================================
///      . .
///====o==V==o========================================
