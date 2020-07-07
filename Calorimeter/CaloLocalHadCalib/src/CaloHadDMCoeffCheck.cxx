/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloHadDMCoeffCheck.cxx,v 1.1 2009/03/03 17:30:23 pospelov Exp $
//
// Description: see CaloHadDMCoeffCheck.h
// 
// Environment:
//      Software developed for the ATLAS Detector at CERN LHC
//
// Author List:
//      Gennady Pospelov
//
//-----------------------------------------------------------------------
#include "CaloLocalHadCalib/CaloHadDMCoeffCheck.h"
#include "CaloLocalHadCalib/CaloHadDMCoeffData.h"
#include "CaloLocalHadCalib/CaloLocalHadCoeffHelper.h"
#include "CaloLocalHadCalib/CaloHadDMCoeffFit.h"
#include "CaloLocalHadCalib/GetLCSinglePionsPerf.h"
#include "CaloConditions/CaloLocalHadCoeff.h"
#include "CaloConditions/CaloLocalHadDefs.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include <math.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

#include <CLHEP/Vector/LorentzVector.h>
#include <CLHEP/Units/SystemOfUnits.h>

#include "TROOT.h"
#include "TStyle.h"
#include "TError.h"
#include "TFile.h"
#include "TChain.h"
#include "TH1.h"
#include "TH1F.h"
#include "TF1.h"
#include "TProfile.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TText.h"
#include "TLatex.h"
#include "TGraphErrors.h"


using CLHEP::HepLorentzVector;
using CLHEP::MeV;


CaloHadDMCoeffCheck::CaloHadDMCoeffCheck() :
    m_isTestbeam(false),
    m_energyMin(200*MeV),
    m_netabin(25), m_etamin(0.0), m_etamax(5.0),
    m_deta(0),
    m_nphibin(1), m_phimin(-M_PI), m_phimax(M_PI),
    m_dphi(0),
    m_nlogenerbin(22), m_logenermin(2.0), m_logenermax(6.4),
    m_dlogener(0),
    m_nrecobin(3), m_npdg(2), m_doTailRejection(true)
{
  m_data = 0;
  m_HadDMCoeff = 0;
  m_HadDMHelper = new CaloLocalHadCoeffHelper();

}


CaloHadDMCoeffCheck::~CaloHadDMCoeffCheck()
{
  delete m_HadDMHelper;
}


int CaloHadDMCoeffCheck::process(CaloHadDMCoeffData *myData, CaloLocalHadCoeff *myHadDMCoeff, bool isSingleParticle, bool tbflag)
{
  m_isTestbeam = tbflag;

  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << "--- CaloHadDMCoeffCheck::process() --- " << std::endl;

  if(m_isTestbeam){
    m_etamin = 2.5;
    m_etamax = 5.0;
    m_netabin = 25;
    m_phimin = M_PI*3./4.-0.15;
    m_phimax = M_PI*3./4.+0.15;
    m_nphibin = 1;
    m_logenermin = 2.1;
    m_logenermax = 5.5;
    m_nlogenerbin = 17;
  }
  m_deta = (m_etamax - m_etamin)/m_netabin;
  m_dphi = (m_phimax - m_phimin)/m_nphibin;
  m_dlogener = (m_logenermax - m_logenermin)/m_nlogenerbin;

  m_data = myData;
  m_HadDMCoeff = myHadDMCoeff;

  // loading only necessary branches
  m_data->fChain->SetBranchStatus("*",0);
  m_data->fChain->SetBranchStatus("mc_ener",1);
  m_data->fChain->SetBranchStatus("mc_eta",1);
  m_data->fChain->SetBranchStatus("mc_phi",1);
  m_data->fChain->SetBranchStatus("mc_pdg",1);
  m_data->fChain->SetBranchStatus("ncls",1);
  m_data->fChain->SetBranchStatus("cls_eta",1);
  m_data->fChain->SetBranchStatus("cls_phi",1);
  m_data->fChain->SetBranchStatus("cls_lambda",1);
  m_data->fChain->SetBranchStatus("cls_calib_emfrac",1);
  m_data->fChain->SetBranchStatus("cls_engcalib",1);
  m_data->fChain->SetBranchStatus("engClusSumCalib",1);
  m_data->fChain->SetBranchStatus("cls_ener_unw",1);
  m_data->fChain->SetBranchStatus("narea",1);
  m_data->fChain->SetBranchStatus("cls_eprep",1);
  m_data->fChain->SetBranchStatus("cls_dmener",1);
  m_data->fChain->SetBranchStatus("cls_smpener_unw",1);
  m_data->fChain->SetBranchStatus("cls_oocener",1);
  m_data->fChain->SetBranchStatus("cls_engcalibpres",1);

  if( !m_data->GetEntries() ) {
    std::cout << "CaloHadDMCoeffCheck::process -> Error! No entries in DeadMaterialTree." << std::endl;
    return 0;
  }

  std::vector<std::vector<std::vector<CaloHadDMCoeffFit::PrepData *> > > ereco; // [sDM][m_netabin][m_nlogenerbin]
  std::vector<std::vector<std::vector<CaloHadDMCoeffFit::PrepData *> > > etrue; // [sDM][m_netabin][m_nlogenerbin]
  int nArea = m_HadDMCoeff->getSizeAreaSet()+1;
  ereco.resize(nArea);
  etrue.resize(nArea);
  for(int i_area=0; i_area<nArea; i_area++){
    ereco[i_area].resize(m_netabin);
    etrue[i_area].resize(m_netabin);
    for(int i_eta=0; i_eta<m_netabin; i_eta++){
      ereco[i_area][i_eta].resize(m_nlogenerbin, 0);
      etrue[i_area][i_eta].resize(m_nlogenerbin, 0);
      for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
        ereco[i_area][i_eta][i_ener] = new CaloHadDMCoeffFit::PrepData();
        etrue[i_area][i_eta][i_ener] = new CaloHadDMCoeffFit::PrepData();
      } // i_ener
    } // i_eta
  } // i_area

  /* ********************************************
  first loop to calculate histogram limits
  this code reproduces CaloUtils/src/CaloLCDeadMaterialTool.cxx
  ********************************************* */
  std::cout << "CaloHadDMCoeffCheck::process() -> Info. First loop to find histogram limits " << std::endl;
  for(int i_ev=0; m_data->GetEntry(i_ev)>0;i_ev++) {
    if(i_ev%10000==0) std::cout << "    i_ev: " << i_ev << " '" << ((TChain *)m_data->fChain)->GetFile()->GetName() << "'" << std::endl;
    if(m_data->m_mc_ener <= 0.0) {
      std::cout << "CaloHadDMCoeffCheck::process() -> Warning! Unknown particle energy " << m_data->m_mc_ener << std::endl;
      continue;
    }

    // checking event quality
    if(isSingleParticle) {
      bool GoodClusterFound(false);
      if( m_data->m_ncls )  {
        HepLorentzVector hlv_pion(1,0,0,1);
        hlv_pion.setREtaPhi(1./cosh(m_data->m_mc_eta), m_data->m_mc_eta, m_data->m_mc_phi);
        for(int i_cls=0; i_cls<m_data->m_ncls; i_cls++){ // loop over clusters
          HepLorentzVector hlv_cls(1,0,0,1);
          hlv_cls.setREtaPhi(1./cosh( (*m_data->m_cls_eta)[i_cls] ), (*m_data->m_cls_eta)[i_cls], (*m_data->m_cls_phi)[i_cls]);
          double r = hlv_pion.angle(hlv_cls.vect());
          if(r < 1.5*GetLCSinglePionsPerf::angle_mollier_factor(m_data->m_mc_eta) 
            && (*m_data->m_cls_engcalib)[i_cls] > 20.0*MeV 
            //&& (*m_data->m_cls_ener)[i_cls] > 0.01*m_data->m_mc_ener
            ) {
            GoodClusterFound = true;
            break;
          }
        } // i_cls
      }
      if(!GoodClusterFound) continue;
    }

    int mc_enerbin = int( (log10(m_data->m_mc_ener) - m_logenermin)/m_dlogener);
    int mc_etabin = int((fabs(m_data->m_mc_eta)-m_etamin)/m_deta);
    int mc_phibin = int((fabs(m_data->m_mc_phi)-m_phimin)/m_dphi);

    if(mc_etabin <0 || mc_etabin >= m_netabin || mc_enerbin <0 || mc_enerbin >= m_nlogenerbin || mc_phibin!=0) continue;
    if(m_data->m_engClusSumCalib <=0.0) continue;
 
    std::vector<std::vector<double > > engDmReco; // [ncls][narea]
    getDmReco(engDmReco);
    std::vector<double > engDmRecSumClus;
    std::vector<double > engDmTrueSumClus;
    engDmRecSumClus.resize(nArea, 0.0);
    engDmTrueSumClus.resize(nArea, 0.0);
    for(int i_cls=0; i_cls<m_data->m_ncls; i_cls++){ // loop over clusters
      for(int i_area=0; i_area<m_HadDMCoeff->getSizeAreaSet(); i_area++){
        engDmRecSumClus[i_area] += engDmReco[i_cls][i_area];
        engDmTrueSumClus[i_area] += (*m_data->m_cls_dmener)[i_cls][i_area];
        engDmRecSumClus[m_HadDMCoeff->getSizeAreaSet()] += engDmReco[i_cls][i_area];
        engDmTrueSumClus[m_HadDMCoeff->getSizeAreaSet()] += (*m_data->m_cls_dmener)[i_cls][i_area];
      }
    }
    for(int i_area=0; i_area<m_HadDMCoeff->getSizeAreaSet()+1; i_area++){
      ereco[i_area][mc_etabin][mc_enerbin]->add(engDmRecSumClus[i_area]);
      etrue[i_area][mc_etabin][mc_enerbin] -> add(engDmTrueSumClus[i_area]);
    }
  } // i_ev


  /* ********************************************
  histogram creation
  ******************************************** */
  char hname[256];
  m_h2_etrue_vs_ereco.resize(nArea);
  m_hp_etrue_vs_ereco.resize(nArea);
  for(int i_area=0; i_area<nArea; i_area++){
    m_h2_etrue_vs_ereco[i_area].resize(m_netabin);
    m_hp_etrue_vs_ereco[i_area].resize(m_netabin);
    for(int i_eta=0; i_eta<m_netabin; i_eta++){
      m_h2_etrue_vs_ereco[i_area][i_eta].resize(m_nlogenerbin, 0);
      m_hp_etrue_vs_ereco[i_area][i_eta].resize(m_nlogenerbin, 0);
      for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
        float elimreco = ereco[i_area][i_eta][i_ener]->m_aver + 5.*sqrt(ereco[i_area][i_eta][i_ener]->m_rms);
        float elimtrue = etrue[i_area][i_eta][i_ener]->m_aver + 5.*sqrt(etrue[i_area][i_eta][i_ener]->m_rms);
        if(elimreco <=0.0) elimreco = 1.0;
        if(elimtrue <=0.0) elimtrue = 1.0;

        sprintf(hname,"h2_etrue_vs_ereco_dm%d_eta%d_ener%d",i_area, i_eta, i_ener);
        m_h2_etrue_vs_ereco[i_area][i_eta][i_ener] = new TH2F(hname, hname,  30, 0.0, elimtrue, 30, 0.0, elimreco);
        sprintf(hname,"hp_etrue_vs_ereco_dm%d_eta%d_ener%d",i_area, i_eta, i_ener);
        m_hp_etrue_vs_ereco[i_area][i_eta][i_ener] = new TProfile(hname, hname,  30, 0.0, elimtrue);
        m_hp_etrue_vs_ereco[i_area][i_eta][i_ener] -> SetMinimum(0.0);
        m_hp_etrue_vs_ereco[i_area][i_eta][i_ener] -> SetMaximum(elimreco);
      } // i_ener
    } // i_eta
  } // i_area

  // reco over truth
  m_hp_engRecoOverTruth_vs_eta.resize(m_npdg);
  for(int i_pdg=0; i_pdg<m_npdg; i_pdg++){
    m_hp_engRecoOverTruth_vs_eta[i_pdg].resize(nArea);
    for(int i_area=0; i_area<nArea; i_area++){
      m_hp_engRecoOverTruth_vs_eta[i_pdg][i_area].resize(m_nrecobin);
      for(int i_reco=0; i_reco<m_nrecobin; i_reco++){
        m_hp_engRecoOverTruth_vs_eta[i_pdg][i_area][i_reco].resize(m_nlogenerbin);
        for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
          sprintf(hname,"m_hp_engRecoOverTruth_vs_eta_pdg%d_dm%d_reco%d_ener%d",i_pdg,i_area, i_reco, i_ener);
          m_hp_engRecoOverTruth_vs_eta[i_pdg][i_area][i_reco][i_ener] = new TProfile(hname, hname,  50, m_etamin, m_etamax);
        } // i_ener
      } // m_nrecobin
    } // i_area
  } // i_pdg

  // reco spect
  m_engRecSpect.resize(m_npdg);
  for(int i_pdg=0; i_pdg<m_npdg; i_pdg++){
    m_engRecSpect[i_pdg].resize(nArea);
    for(int i_area=0; i_area<nArea; i_area++){
      m_engRecSpect[i_pdg][i_area].resize(m_nrecobin);
      for(int i_reco=0; i_reco<m_nrecobin; i_reco++){
        m_engRecSpect[i_pdg][i_area][i_reco].resize(m_nlogenerbin);
        for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
          m_engRecSpect[i_pdg][i_area][i_reco][i_ener].resize(m_netabin, 0);
          for(int i_eta=0; i_eta<m_netabin; i_eta++){
            sprintf(hname,"m_engRecSpect_pdg%d_dm%d_reco%d_ener%d_eta%d",i_pdg,i_area, i_reco, i_ener, i_eta);
            TH1F *h1 = new TH1F(hname, hname, 110, -0.2, 2.0);
            m_engRecSpect[i_pdg][i_area][i_reco][i_ener][i_eta] = h1;
          } // i_eta
        } // i_ener
      } // i_reco
    } // i_area
  } // i_pdg

  /* ********************************************
  histogram  filling
  ******************************************** */
  std::cout << "CaloHadDMCoeffCheck::process() -> Info. Second loop to fill histogram " << std::endl;
  for(int i_ev=0; m_data->GetEntry(i_ev)>0;i_ev++) {
    if(i_ev%10000==0) std::cout << "    i_ev: " << i_ev << " '" << ((TChain *)m_data->fChain)->GetFile()->GetName() << "'" << std::endl;

    int mc_enerbin = int( (log10(m_data->m_mc_ener) - m_logenermin)/m_dlogener);
    int mc_etabin = int((fabs(m_data->m_mc_eta)-m_etamin)/m_deta);
    int mc_phibin = int((fabs(m_data->m_mc_phi)-m_phimin)/m_dphi);
    if(mc_etabin <0 || mc_etabin >= m_netabin || mc_enerbin <0 || mc_enerbin >= m_nlogenerbin || mc_phibin!=0) continue;
    if(m_data->m_engClusSumCalib <=0.0) continue;

    int i_pdg=0;
    if( abs(m_data->m_mc_pdg)==211) {
      i_pdg=0;
    }else{
      i_pdg=1;
    }

    std::vector<std::vector<double > > engDmReco; // [ncls][narea]
    getDmReco(engDmReco);
    std::vector<double > engDmRecSumClus;
    std::vector<double > engDmTrueSumClus;
    engDmRecSumClus.resize(nArea, 0.0);
    engDmTrueSumClus.resize(nArea, 0.0);
    double engClusSumCalib = 0.0;
    double engClusSumOOC = 0.0;
    double engClusSumDM = 0.0;
    double engClusSumCalibInPresampler = 0.0;

    for(int i_cls=0; i_cls<m_data->m_ncls; i_cls++){ // loop over clusters
      engClusSumCalib += (*m_data->m_cls_engcalib)[i_cls];
      engClusSumOOC += (*m_data->m_cls_oocener)[i_cls];
      engClusSumCalibInPresampler += (*m_data->m_cls_engcalibpres)[i_cls];
      for(int i_area=0; i_area<m_HadDMCoeff->getSizeAreaSet(); i_area++){
        engDmRecSumClus[i_area] += engDmReco[i_cls][i_area];
        engDmTrueSumClus[i_area] += (*m_data->m_cls_dmener)[i_cls][i_area];
        engDmRecSumClus[m_HadDMCoeff->getSizeAreaSet()] += engDmReco[i_cls][i_area];
        engDmTrueSumClus[m_HadDMCoeff->getSizeAreaSet()] += (*m_data->m_cls_dmener)[i_cls][i_area];
        engClusSumDM += (*m_data->m_cls_dmener)[i_cls][i_area];
      }
    }
    for(int i_area=0; i_area<m_HadDMCoeff->getSizeAreaSet()+1; i_area++){
      m_h2_etrue_vs_ereco[i_area][mc_etabin][mc_enerbin]->Fill(engDmTrueSumClus[i_area], engDmRecSumClus[i_area]);
      m_hp_etrue_vs_ereco[i_area][mc_etabin][mc_enerbin]->Fill(engDmTrueSumClus[i_area], engDmRecSumClus[i_area]);
    }

    // engRecoOverTruth
    double sum = engClusSumCalib + engClusSumOOC + engClusSumDM - engClusSumCalibInPresampler; // presampler is already included in the dead material
    for(int i_area=0; i_area<m_HadDMCoeff->getSizeAreaSet()+1; i_area++){
      for(int i_reco=0; i_reco<m_nrecobin; i_reco++){
        double engReco = 0.0;
        if(i_reco == kENG_RECO){ // red
          engReco = sum - engDmTrueSumClus[i_area] + engDmRecSumClus[i_area];
        }else if(i_reco == kENG_TRUTH){ // blue
          engReco = sum;
        }else if(i_reco == kENG_NORECO){ // green
          engReco = sum - engDmTrueSumClus[i_area];
        }else{
          std::cout << "panic" << std::endl;
        }
         m_hp_engRecoOverTruth_vs_eta[i_pdg][i_area][i_reco][mc_enerbin]->Fill(fabs(m_data->m_mc_eta), engReco/m_data->m_mc_ener);
         m_engRecSpect[i_pdg][i_area][i_reco][mc_enerbin][mc_etabin]->Fill(engReco/m_data->m_mc_ener);
      } // i_reco
    } // i_area

  } // i_ev

  /* ********************************************
  histogram  fitting
  ******************************************** */
  TF1 *fun_p1 = new TF1("fun_udeg3","[0]+[1]*x",1.,200000.);
  for(int i_area=0; i_area<nArea; i_area++){
    for(int i_eta=0; i_eta<m_netabin; i_eta++){
      for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
        TProfile *hp = m_hp_etrue_vs_ereco[i_area][i_eta][i_ener];
        if(hp->GetEntries() < 100) continue;
        float fitlim1 = hp->GetBinCenter(2);
        float fitlim2 = hp->GetBinCenter(30);
        hp->Fit(fun_p1,"0Q","",fitlim1,fitlim2); // silent fit, but now drawing option for fit results is switched off
        TF1 *f=hp->GetFunction(fun_p1->GetName());
        if(f) {
          f->ResetBit(TF1::kNotDraw);
          f->SetLineWidth(1);
          f->SetLineColor(4);
        }
      } // i_ener
    } // i_eta
  } // i_area



  return 0;
}



/* ****************************************************************************
Making nice postscript report
**************************************************************************** */
void CaloHadDMCoeffCheck::make_report(std::string &sreport)
{
  const char *a_title[sDM] = {
    "EMB0_EMB1",   // 0 before PreSamplerB, between PreSamplerB and EMB1
    "EMB3_TILE0",  // 1 between barrel and tile
    "SCN",         // 2 before TileGap3 (scintillator)
    "EME0_EME12",  // 3 before PreSamplerE, between PreSamplerE and EME1
    "EME3_HEC0",   // 4 between EME3 and HEC0
    "FCAL",        // 5 between HEC and FCAL, before FCAL
    "LEAKAGE",     // 6 leakage behind calorimeter
    "UNCLASS",     // 7 unclassified DM enegry
    "DMTOT"
  };

  int a_enerbin_sel[] = {12, 15};
  int a_etabin_sel[] = {2, 7, 10, 18};
  //int a_area_sel[] = {sDM_EME3_HEC0, sDM_FCAL, sDM_LEAKAGE, sDM_UNCLASS, sDM_DMTOT};
  int a_area_sel[] = {
    sDM_EMB0_EMB1,   // 0 before PreSamplerB, between PreSamplerB and EMB1
    sDM_EMB3_TILE0,  // 1 between barrel and tile
    sDM_SCN,         // 2 before TileGap3 (scintillator)
    sDM_EME0_EME12,  // 3 before PreSamplerE, between PreSamplerE and EME1
    sDM_EME3_HEC0,   // 4 between EME3 and HEC0
    sDM_FCAL,        // 5 between HEC and FCAL, before FCAL
    sDM_LEAKAGE,     // 6 leakage behind calorimeter
    sDM_UNCLASS,     // 7 unclassified DM enegry
    sDM_DMTOT       // 8 sum of DM energy over all zones
  };

  std::cout << "CaloHadDMCoeffCheck::make_report() -> Info. Making report..." << std::endl;
  gStyle->SetCanvasColor(10);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadColor(10);
  gStyle->SetPadBorderMode(0); 
  gStyle->SetPalette(1);
  gStyle->SetTitleBorderSize(1);
  gStyle->SetTitleFillColor(10);
  int cc_xx = 768, cc_yy = 1024;
  char str[1024];
  gROOT->SetBatch(kTRUE);
  gErrorIgnoreLevel=3; // root global variables to supress text output in ->Print() methods
  std::string sfname = sreport;
  TCanvas *ctmp = new TCanvas("ctmp","ctmp", cc_xx, cc_yy);
  sfname += "[";
  ctmp->Print(sfname.c_str());
  sfname = sreport;

  TLatex tex;
  tex.SetNDC(1);
  tex.SetTextSize(0.05);

  TCanvas *c1ps = new TCanvas("c1ps","c1ps", cc_xx, cc_yy);
  for(int i_ener=0; i_ener<int(sizeof(a_enerbin_sel)/sizeof(int)); i_ener++){
    int i_ener_sel = a_enerbin_sel[i_ener];
    for(int i_eta=0; i_eta<int(sizeof(a_etabin_sel)/sizeof(int)); i_eta++){
      int i_eta_sel = a_etabin_sel[i_eta];
      c1ps->Clear();
      c1ps->Divide(3,3);
      for(int i_area=0; i_area<int(sizeof(a_area_sel)/sizeof(int)); i_area++){
        int i_area_sel = a_area_sel[i_area];
        c1ps->cd(i_area+1); 
        gPad->SetLeftMargin(0.12);
        gPad->SetTopMargin(0.20);
        sprintf(str,"%s ener%d eta%d dm%d",a_title[i_area_sel], i_ener_sel, i_eta_sel, i_area);
        TH1 *h = m_h2_etrue_vs_ereco[i_area_sel][i_eta_sel][i_ener_sel];
        h->SetTitle(str);
        h->GetXaxis()->SetTitle("E_{DM} True");
        h->GetYaxis()->SetTitle("E_{DM} Reco");
        h->GetYaxis()->SetTitleOffset(1.6);
        h->GetYaxis()->SetNdivisions(508);
        h->GetXaxis()->SetNdivisions(508);
        h->Draw("box");
        h = m_hp_etrue_vs_ereco[i_area_sel][i_eta_sel][i_ener_sel];
        h->SetLineColor(kRed);
        h->SetMarkerColor(kRed);
        h->Draw("same");
        sprintf(str,"E = %3.1f-%3.1f GeV",
            pow(10,m_logenermin+m_dlogener*i_ener_sel)*1e-3, 
                pow(10,m_logenermin+m_dlogener*(i_ener_sel+1))*1e-3);
        tex.SetTextSize(0.05);
        tex.DrawLatex(0.2,0.85,str);
        sprintf(str,"#eta = %3.1f-%3.1f",m_etamin+m_deta*i_eta_sel, m_etamin+m_deta*(i_eta_sel+1));
        tex.DrawLatex(0.2,0.78,str);
      }  // i_area
      c1ps->Print(sfname.c_str());
    } // i_eta
  } // i_ener

  // engRecoOverTruth
  int a_color[] = {kRed, kBlue, kGreen};
  for(int i_pdg=0; i_pdg<m_npdg; i_pdg++){
    for(int i_ener=0; i_ener<int(sizeof(a_enerbin_sel)/sizeof(int)); i_ener++){
      int i_ener_sel = a_enerbin_sel[i_ener];

      char cname[1024];
      sprintf(cname,"c2ps_engRecoOverTruth_%d_%d",i_pdg, i_ener);
      TCanvas *c1 = new TCanvas(cname, cname, cc_xx, cc_yy);
      c1->cd();
      sprintf(cname,"c2ps_engRecoOverTruth_%d_%d_pad1",i_pdg, i_ener);
      TPad *c1_pad1 = new TPad(cname, cname, 0.0, 0.9, 1.0, 1.0); c1_pad1->Draw();
      sprintf(cname,"c2ps_engRecoOverTruth_%d_%d_pad2",i_pdg, i_ener);
      TPad *c1_pad2 = new TPad(cname, cname, 0.0, 0.0, 1.0, 0.9); c1_pad2->Draw();

      c1_pad1->cd();
      sprintf(str,"pdg:%d  E = %3.1f-%3.1f GeV",i_pdg,
          pow(10,m_logenermin+m_dlogener*i_ener_sel)*1e-3, 
           pow(10,m_logenermin+m_dlogener*(i_ener_sel+1))*1e-3);
      tex.SetTextSize(0.12);
      tex.DrawLatex(0.1, 0.5, str);
      c1_pad2->cd();
      c1_pad2->Divide(3,3);
      for(int i_area=0; i_area<int(sizeof(a_area_sel)/sizeof(int)); i_area++){
        int i_area_sel = a_area_sel[i_area];
        c1_pad2->cd(i_area+1); gPad->SetGrid();
        for(int i_reco=0; i_reco<m_nrecobin; i_reco++){
          TProfile *hp = m_hp_engRecoOverTruth_vs_eta[i_pdg][i_area_sel][i_reco][i_ener_sel];
          hp->SetLineColor(a_color[i_reco]);
          hp->SetMinimum(0.5);
          hp->SetMaximum(1.2);
          hp->GetXaxis()->SetTitle("mc_eta");
          hp->GetYaxis()->SetTitle("engReco / engTruth");
          hp->SetTitle(a_title[i_area_sel]);
          if(i_reco==0) {
            hp->Draw();
          }else{
            hp->Draw("same");
          }
        } // i_reco
      }
      std::cout << "adding " << i_pdg << " " << i_ener << std::endl;
      c1->Print(sfname.c_str());

      sprintf(cname,"c2ps_engResolution_%d_%d",i_pdg, i_ener);
      TCanvas *c2 = new TCanvas(cname, cname, cc_xx, cc_yy);
      c2->cd();
      sprintf(cname,"c2ps_engResolution_%d_%d_pad1",i_pdg, i_ener);
      TPad *c2_pad1 = new TPad(cname, cname, 0.0, 0.9, 1.0, 1.0); c2_pad1->Draw();
      sprintf(cname,"c2ps_engResolution_%d_%d_pad2",i_pdg, i_ener);
      TPad *c2_pad2 = new TPad(cname, cname, 0.0, 0.0, 1.0, 0.9); c2_pad2->Draw();

      c2_pad1->cd();
      sprintf(str,"pdg:%d  E = %3.1f-%3.1f GeV (resolution)",i_pdg,
          pow(10,m_logenermin+m_dlogener*i_ener_sel)*1e-3, 
           pow(10,m_logenermin+m_dlogener*(i_ener_sel+1))*1e-3);
      tex.SetTextSize(0.12);
      tex.DrawLatex(0.1, 0.5, str);
      c2_pad2->cd();
      c2_pad2->Divide(3,3);
      TH1F *h1ref = new TH1F("h1ref","h1ref",100, m_etamin, m_etamax);
      h1ref->SetMaximum(0.25);
      h1ref->SetMinimum(0.0);
      h1ref->GetXaxis()->SetTitle("#eta");
      h1ref->GetYaxis()->SetTitle("#sigma_{E}/E");
      h1ref->SetStats(0);
      for(int i_area=0; i_area<int(sizeof(a_area_sel)/sizeof(int)); i_area++){
        int i_area_sel = a_area_sel[i_area];
        c2_pad2->cd(i_area+1); gPad->SetGrid();
        h1ref->SetTitle(a_title[i_area_sel]);
        h1ref->DrawCopy();

        for(int i_reco=0; i_reco<m_nrecobin; i_reco++){
          TGraphErrors *gr = new TGraphErrors(m_netabin);
          for(int i_eta=0; i_eta<m_netabin; i_eta++){
            std::cout << " i_pdg:" << i_pdg
            << " i_area_sel:" << i_area_sel
            << " i_reco:" << i_reco
            << " i_ener_sel:" << i_ener_sel
            << " i_eta:" << i_eta
            << std::endl;
            TH1F *h1 = m_engRecSpect[i_pdg][i_area_sel][i_reco][i_ener_sel][i_eta];
            if(h1 == 0) {
              std::cout << " panic, h1==0" << std::endl;
              continue;
            }
            if(h1->GetMean() == 0.0) {
              std::cout << " Resolution() -> Warning! Skipping point, no energy i_ener:" << i_ener_sel 
              << " i_eta:" << i_eta << " " << (m_etamin + (i_eta+0.5)*m_deta)
              << " mean2:" << h1->GetMean()
              << " nent2:" << h1->GetEntries() << std::endl;
              continue;
            }

            if(m_doTailRejection) {
              float mean3(0), rms3(0);
              GetRmsWithoutTails(h1, mean3, rms3);
              gr->SetPoint(i_eta, m_etamin + (i_eta+0.5)*m_deta, rms3/mean3);
            }else{
              gr->SetPoint(i_eta, m_etamin + (i_eta+0.5)*m_deta, h1->GetRMS()/h1->GetMean());
            }
          } // i_eta
          gr->SetLineColor(a_color[i_reco]);
          gr->SetMarkerColor(a_color[i_reco]);
          gr->SetMarkerSize(1.0);
          gr->Draw("pl");
        } // i_reco
      } // i_area
      c2->Print(sfname.c_str());

    } // i_ener
  } // i_pdg

  sfname = sreport;
  sfname += "]";
  ctmp->Print(sfname.c_str());
}


/* ****************************************************************************
Toy analog of CaloLCDeadMaterialTool.cxx
**************************************************************************** */
void CaloHadDMCoeffCheck::getDmReco(std::vector<std::vector<double > > &engDmReco) // [ncls][narea]
{
  engDmReco.clear();
  engDmReco.resize(m_data->m_ncls);
  for(int i_cls=0; i_cls<m_data->m_ncls; i_cls++){
    engDmReco[i_cls].resize(m_HadDMCoeff->getSizeAreaSet(), 0.0);
  }
  for(int i_cls=0; i_cls<m_data->m_ncls; i_cls++){ // loop over clusters
    if( (*m_data->m_cls_ener_unw)[i_cls] < m_energyMin) continue;
    float clusEnerOrig = (*m_data->m_cls_ener_unw)[i_cls];
    for(int i_dma=0; i_dma<m_data->m_narea; i_dma++){ // loop over DM areas
      const CaloLocalHadCoeff::LocalHadArea *area = m_HadDMCoeff->getArea(i_dma);

      float clusEner = (*m_data->m_cls_ener_unw)[i_cls];
      float emax = pow(10,area->getDimension(CaloLocalHadCoeffHelper::DIM_ENER)->getXmax());
      if( clusEner > emax) clusEner = emax-0.001;
      (*m_data->m_cls_ener_unw)[i_cls] = clusEner;

      std::vector<float> vars;
      m_data->PackClusterVars(i_cls, vars);

      (*m_data->m_cls_ener_unw)[i_cls] = clusEnerOrig;

      const CaloLocalHadCoeff::LocalHadCoeff *pars = m_HadDMCoeff->getCoeff(i_dma, vars);
      if(pars==0) continue;
      double eprep = (*m_data->m_cls_eprep)[i_cls][i_dma];
      double engDmRec = 0.0;
      if(area->getType() == CaloLocalHadDefs::AREA_DMFIT && area->getNpars() == 3) {
        if(eprep > 0.0) engDmRec = (*pars)[0] + (*pars)[1]*pow(eprep, (*pars)[2]);
      }else if(area->getType() == CaloLocalHadDefs::AREA_DMLOOKUP && area->getNpars() == 3){
        if( (*pars)[1] > 40) {
          //double isol = (*m_data->m_cls_isol)[i_cls];
          double isol = 1.0;
          engDmRec = isol*(*m_data->m_cls_ener_unw)[i_cls]*((*pars)[0] - 1.0 );
        }
      }else if(area->getType() == CaloLocalHadDefs::AREA_DMSMPW && area->getNpars() == CaloSampling::Unknown+1){
        double ecalonew = 0.0;
        double ecaloold = 0.0;
        for(int i_smp=0; i_smp<CaloSampling::Unknown; i_smp++){
          float smpener = (*m_data->m_cls_smpener_unw)[i_cls][i_smp];
          ecaloold += smpener;
          ecalonew += smpener * (*pars)[i_smp];
        }
        ecalonew += (*pars)[CaloSampling::Unknown]; // const
        engDmRec = ecalonew - ecaloold;
      }else{
        std::cout << "Panic! Unknown DM area type" << std::endl;
      }
//       double edmWrong = 0.0;
//       if(i_dma == sDM_EMB0_EMB1) {
//         edmWrong = (*m_data->m_cls_smpener_unw)[i_cls][CaloSampling::PreSamplerB];
//       }else if(i_dma == sDM_SCN) {
//         edmWrong = (*m_data->m_cls_smpener_unw)[i_cls][CaloSampling::TileGap3];
//       }else if(i_dma == sDM_EME0_EME12) {
//         edmWrong = (*m_data->m_cls_smpener_unw)[i_cls][CaloSampling::PreSamplerE];
//       }else{
//         edmWrong = 0.0;
//       }
//       engDmRec -= edmWrong;
      if(engDmRec >0.0) engDmReco[i_cls][i_dma] = engDmRec;
    } // i_dma
  } // i_cls
}



/* **************************************************************************

************************************************************************** */
int CaloHadDMCoeffCheck::GetRmsWithoutTails(const TH1F *pH, float &mean, float &rms, float ths)
{
  mean = pH->GetMean();
  rms = pH->GetRMS();
  float lim1 = mean - ths*rms;
  float lim2 = mean + ths*rms;

  //double sum(0);
  double d_aver(0);
  double d_rms(0);
  double d_sw(0);
  for(int i=1; i<=(int)pH->GetNbinsX(); i++){
    if( pH->GetBinCenter(i)>=lim1 && pH->GetBinCenter(i)<= lim2 ){
      float xx = pH->GetBinCenter(i);
      float w = pH->GetBinContent(i);
      if(w == 0) continue;
      //sum += xx;
      d_rms = (d_sw/(d_sw+w))*(d_rms+(w/(d_sw+w))*(xx-d_aver)*(xx-d_aver));
      d_aver = d_aver+(xx-d_aver)*w/(d_sw+w);
      d_sw += w;
    }
  }
  if(d_rms==0) {
    d_rms = pH->GetRMS();
  }else{
    d_rms = sqrt(d_rms);
  }

  if(d_aver==0) {
    d_aver = pH->GetMean();
  }

  mean = d_aver;
  rms = d_rms;
  return 0;
}
