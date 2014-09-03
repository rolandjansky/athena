/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Macro to draw performance plots for local hadronic calibration on single pions
//

#include <math.h>
#include <vector>
#include <iostream>
#include "TROOT.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TProfile.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TPad.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TLine.h"
#include "TPaveText.h"
#include "TText.h"
#include "TGraphErrors.h"

int m_netabin(25);
float m_etamin(0.0);
float m_etamax(5.0);
float m_deta(0);
int m_nphibin(1);
float m_phimin(-TMath::Pi());
float m_phimax(-TMath::Pi());
float m_dphi(0);
int m_nlogenerbin(22);
float m_logenermin(2.0);
float m_logenermax(6.4);
float m_dlogener(0);
int m_nnormtype(3); // wrt pion energy, wrt calibration hits, wrt calibration hits with "ideal" previous step
int m_ncluscoll(4); // em, w, ooc, dm
int m_ntagcases(3); // taggedem, taggedhad, taggedunknown
int m_nmoments(3);  // ENG_CALIB_TOT, ENG_CALIB_OUT_M, ENG_CALIB_DEAT_TOT
int m_nmomsums(5);  // ratio of moment assigned to cls0, to cls1, to cls2, to sum of all cls, what remains unassigned

float m_EnergyAxisMin(1e+03);
float m_EnergyAxisMax(2.5e+06);
float m_EtaAxisMin(-5.0);
float m_EtaAxisMax(5.0);

float m_NumberOfRMS = 3.0; // tails rejection for the resolution

// selected bins to draw
int a_enerbin_sel[]={10, 15};
int a_etabin_sel[]={1, 7, 10, 18};
// int a_enerbin_sel[]={12, 15};
// int a_etabin_sel[]={2, 7, 10, 18};

// output pictures
std::string sformat("eps");
bool doSavePlots = true;
std::string sPlotDirName;

TFile *top = 0;
int cc_xx = 1024;
int cc_yy = 768;
std::string sparticle;
double myMarkerSize(1.0);
double myHistResolutionMax(0.5);


int EngRecOverTruth(std::string &sfname);
int Classification(std::string &sfname);
int EngRecSpect(std::string &sfname);
int Resolution(std::string &sfname, bool doRMSCut=false, float nRMS=3.0);
int EngNoiseClus(std::string &sfname);
int ClusterMoments(std::string &sfname);
int RecoEfficiency(std::string &sfname);
int CalibHitsValidation(std::string &sfname);

TPaveText *make_header();
void ReadParameters();
int GetRmsWithoutTails(const TH1F *pH, float &mean, float &rms, float ths=3.0);

// stamp which will appear on each page of report file
std::string srootfname;

int m_npage; // number of page in the report file (will appear in the name of eps files with separate figures)


/* ****************************************************************************
main routine to produce validation plots for local hadron calibration on single pions
**************************************************************************** */
int drawLCSinglePionsPerf(char *inputfn=0, bool save_plots = true)
{
  m_npage = 1;
  doSavePlots = save_plots;

  m_deta = (m_etamax - m_etamin)/m_netabin;
  m_dlogener = (m_logenermax - m_logenermin)/m_nlogenerbin;

  bool doEngRecOverTruth = true;
  bool doEngTag = true;
  bool doEngRecSpect = true;
  bool doResolution = true;
  bool doEngNoiseClus = true;
  bool doClusMoments = true;
  bool doRecoEfficiency = true;
  bool doCalibHitsValidation = true;

  // for testbeam plots
//   m_EnergyAxisMin = 5e+03;
//   m_EnergyAxisMax = 200.e+03;
//   m_EtaAxisMin = 2.5;
//   m_EtaAxisMax = 4.0;

  gROOT->SetBatch(true);

  gStyle->SetCanvasColor(10);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadColor(10);
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameFillColor(0);
  gStyle->SetFrameBorderMode(0);

  if(inputfn == 0) {
    //srootfname = std::string("/home/pospelov/data/mc11/singlepart_singlepi_logE/LCSinglePionsPerf/mc10_7TeV.107410.singlepiplus.r2424.LCSinglePionsPerf.16652.v1.root"); 
    //srootfname = std::string("/home/pospelov/data/mc11/singlepart_singlepi_logE/LCSinglePionsPerf/mc10_7TeV.107411.singlepiminus.r2424.LCSinglePionsPerf.16652.v1.root"); 
    //srootfname = std::string("/home/pospelov/data/mc11/singlepart_singlepi_logE/LCSinglePionsPerf/mc10_7TeV.107412.singlepizero.r2424.LCSinglePionsPerf.16652.v2.root"); 
    //srootfname = std::string("/home/pospelov/data/mc11/singlepart_singlepi_logE/LCSinglePionsPerf/mc10_7TeV.107410.singlepiplus.r2424.LCSinglePionsPerf.16652.Coeff01.v1.root"); 
    //srootfname = std::string("/home/pospelov/data/mc11/singlepart_singlepi_logE/LCSinglePionsPerf/mc10_7TeV.107412.singlepizero.r2424.LCSinglePionsPerf.16652.Coeff01.v1.root"); 

//     srootfname = std::string("/home/pospelov/data/mc11/singlepart_singlepi_logE/LCSinglePionsPerf/mc10_7TeV.107410.singlepiplus.r2424.LCSinglePionsPerf.16652.Coeff01.HadAll.v2.root"); 
//    srootfname = std::string("/home/pospelov/data/mc11/singlepart_singlepi_logE/LCSinglePionsPerf/mc10_7TeV.107412.singlepizero.r2424.LCSinglePionsPerf.16652.Coeff01.EMAll.v2.root"); 

//    srootfname = std::string("/home/pospelov/data/mc11/singlepart_singlepi_logE/LCSinglePionsPerf/mc10_7TeV.107410.singlepiplus.r2424.LCSinglePionsPerf.16652.Coeff01.NColl8.v2.root"); 
//     srootfname = std::string("/home/pospelov/data/mc11/singlepart_singlepi_logE/LCSinglePionsPerf/mc10_7TeV.107412.singlepizero.r2424.LCSinglePionsPerf.16652.Coeff01.NColl8.v2.root"); 

//     srootfname = std::string("/home/pospelov/data/mc11/singlepart_singlepi_logE/LCSinglePionsPerf/mc10_7TeV.107410.singlepiplus.r2424.LCSinglePionsPerf.16652.Coeff02.v1.root"); 
     srootfname = std::string("/home/pospelov/data/mc11/singlepart_singlepi_logE/LCSinglePionsPerf/mc10_7TeV.107412.singlepizero.r2424.LCSinglePionsPerf.16652.Coeff02.v1.root"); 

  } else{
    srootfname = inputfn;
  }

  if( srootfname.find("pizero") != std::string::npos) {
    sparticle = std::string("pizero");
    myHistResolutionMax = 0.5;
  }else if(srootfname.find("piplus") != std::string::npos) {
    sparticle = std::string("piplus");
  }else if(srootfname.find("piminus") != std::string::npos) {
    sparticle = std::string("piminus");
  }else{
    sparticle = std::string("piunkn");
  }

  if(top) {
    top->Close();
    delete top;
  }
  top = new TFile(srootfname.c_str(),"READ");
  if(  !top->IsOpen() ) {
    std::cout << "Can't open '" << srootfname << "'." << std::endl;
    return 0;
  }

  // preparing report file
  gSystem->Exec("mkdir -p ./reports");
  std::string sreport(gSystem->BaseName(srootfname.c_str()));
  sreport.erase(sreport.find_last_of("."));
  sreport = "./reports/report_" + sreport;
  sreport = sreport + std::string(".ps");
  TCanvas *ctmp = new TCanvas("ctmp","ctmp",cc_xx,cc_yy);
  ctmp->SetBatch(kTRUE);
  std::string sfname = sreport;
  sfname += "[";
  ctmp->Print(sfname.c_str());
  sfname = sreport;

  // preparing output directories
  if(doSavePlots) {
    sPlotDirName = "./pictures/";
    sPlotDirName += std::string(gSystem->BaseName(srootfname.c_str()));
    sPlotDirName.erase(sPlotDirName.find_last_of("."));
    std::string scommand=std::string("mkdir -p ") + sPlotDirName;
    gSystem->Exec(scommand.c_str());
  }

  // reading binning information from the tree with parameters
  ReadParameters();

  // classification performance
  if(doEngTag) {
    Classification(sfname);
  }

  // reco energy after each step of calibration
  if(doEngRecOverTruth) {
    EngRecOverTruth(sfname);
  }

  // resolution plots
  if(doResolution) {
    Resolution(sfname);
    Resolution(sfname, true, 3.0); // with tail rejection
  }

  // quality of moments assignment procedure
  if(doClusMoments) {
    ClusterMoments(sfname);
  }

  // average energy of noise clusters
  if(doEngNoiseClus) {
    EngNoiseClus(sfname);
  }

  // energy spectras, resolution
  if(doEngRecSpect) {
    EngRecSpect(sfname);
  }

  // reconstruction efficiency
  if(doRecoEfficiency) {
    RecoEfficiency(sfname);
  }

  // calibration hits validation
  if(doCalibHitsValidation ) {
    CalibHitsValidation(sfname);
  }

  sfname = sreport;
  sfname += "]";
  ctmp->Print(sfname.c_str());

  std::cout << "---------------------------------------------------------------------" << std::endl;
  std::cout << "Report was saved into the file '" << sreport << "'" << std::endl;
  if(sPlotDirName.size()) std::cout << "Pictures were saved into directory '" << sPlotDirName << "'" << std::endl;
  std::cout << "---------------------------------------------------------------------" << std::endl;

  //top->Close();
  delete ctmp;
  return 0;
}



/* ****************************************************************************
classification performance
**************************************************************************** */
int Classification(std::string &sfname)
{
  std::vector<std::vector<TProfile *> > m_engTag_vs_eta;   // [ntagcases][nenerbin]
  std::vector<std::vector<TProfile *> > m_engTag_vs_ebeam; // [ntagcases][netabin]
  int a_color_tag[]={kRed, kBlue, kGreen};
  const char *c_keys_tagged[3]={"tagged EM","tagged HAD","unknown"};

  char str[256];
  char hname[256];
  char cname[256];
  TLatex tex;
  tex.SetNDC(1);
  tex.SetTextSize(0.05);

  // as a function of pion eta for several energy bins
  m_engTag_vs_eta.resize(m_ntagcases);
  for(int i_tag=0; i_tag<m_ntagcases; i_tag++) {
    m_engTag_vs_eta[i_tag].resize(m_nlogenerbin, 0);
    for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
      sprintf(hname, "engTag/hp_engTag_vs_eta_tag%d_ener%d",i_tag, i_ener);
      TProfile *hp = (TProfile *) top->Get(hname);
      if(!hp) {
        std::cout << "Can't get " << hname << std::endl;
        return 0;
      }
      hp->GetXaxis()->SetTitle("#eta");
      hp->GetXaxis()->SetTitleOffset(1.3);
      hp->GetYaxis()->SetTitle("E_{class}/E_{tot}");
      hp->GetYaxis()->SetTitleOffset(1.4);
      m_engTag_vs_eta[i_tag][i_ener] = hp;
    } // i_ener
  } // i_tag
  // as a function of pion energy for several eta bins
  m_engTag_vs_ebeam.resize(m_ntagcases);
  for(int i_tag=0; i_tag<m_ntagcases; i_tag++){
    m_engTag_vs_ebeam[i_tag].resize(m_netabin, 0);
    for(int i_eta=0; i_eta<m_netabin; i_eta++) {
      sprintf(hname, "engTag/hp_engTag_vs_ebeam_tag%d_eta%d",i_tag, i_eta);
      TProfile *hp = (TProfile *) top->Get(hname);
      if(!hp) {
        std::cout << "Can't get " << hname << std::endl;
        return 0;
      }
      hp->GetXaxis()->SetTitle("E_{#pi}, MeV");
      hp->GetXaxis()->SetTitleOffset(1.3);
      hp->GetYaxis()->SetTitle("E_{class}/E_{tot}");
      hp->GetYaxis()->SetTitleOffset(1.4);
      m_engTag_vs_ebeam[i_tag][i_eta] = hp;
    }
  }

  TLegend *leg1 = new TLegend(0.65,0.35,0.90,0.6);
  leg1->SetTextSize(0.04);
  leg1->SetBorderSize(1);
  leg1->SetFillStyle(0);

  sprintf(cname,"LCSinglePionsPerf_%s_classification_vs_eta",sparticle.c_str());
  TCanvas *c1 = new TCanvas(cname, cname, cc_xx, cc_yy);
  c1->cd();
  sprintf(cname,"LCSinglePionsPerf_%s_classification_vs_eta_pad1",sparticle.c_str());
  TPad *c1_pad1 = new TPad(cname, cname, 0.0, 0.9, 1.0, 1.0); c1_pad1->Draw();
  sprintf(cname,"LCSinglePionsPerf_%s_classification_vs_eta_pad2",sparticle.c_str());
  TPad *c1_pad2 = new TPad(cname, cname, 0.0, 0.0, 1.0, 0.9); c1_pad2->Draw();

  c1_pad1->cd();
  TPaveText *tp1 = make_header();
  tp1->AddText("Classification performance");
  tp1->AddText("TAGGEDEM, TAGGEDHAD and TAGGEDUNKNOWN energy fractions as a function of pion coordinate");
  tp1->AddText("for different energy bins");
  tp1->Draw();

//  c1_pad2->Divide(1,2);
  c1_pad2->Divide(2,2);
  for(int i_ener=0; i_ener<int(sizeof(a_enerbin_sel)/sizeof(int)); i_ener++){
    int i_ener_sel = a_enerbin_sel[i_ener];
    c1_pad2->cd(i_ener+1);
    gPad->SetGrid();
    for(int i_tag=0; i_tag<m_ntagcases; i_tag++){
      TProfile *hp = m_engTag_vs_eta[i_tag][i_ener_sel];
      hp->SetLineColor(a_color_tag[i_tag]);
      hp->SetStats(0);
      hp->SetMarkerColor(a_color_tag[i_tag]);
      hp->SetMarkerSize(myMarkerSize);
      hp->SetMarkerStyle(20);
      hp->GetXaxis()->SetRangeUser(m_EtaAxisMin, m_EtaAxisMax);
      if( i_tag==0 ){
        hp->SetMaximum(1.0);
        hp->SetMinimum(0.0);
        hp->Draw();
      } else {
        hp->Draw("same");
      }
    } // i_tag
    sprintf(str,"E = %3.1f-%3.1f GeV",
            pow(10,m_logenermin+m_dlogener*i_ener_sel)/1000., 
                pow(10,m_logenermin+m_dlogener*(i_ener_sel+1))/1000.);
    tex.SetTextSize(0.05);
    tex.DrawLatex(0.2,0.85,str);

    for(int i_tag=0; i_tag<m_ntagcases; i_tag++){
      if(i_ener==0) leg1->AddEntry(m_engTag_vs_eta[i_tag][i_ener_sel],c_keys_tagged[i_tag],"pl");
    }
    leg1->Draw();
    tex.SetTextSize(0.1);
    if(sparticle == "pizero"){
      tex.DrawLatex(0.25,0.5,"#pi^{0}");
    }else{
      tex.DrawLatex(0.25,0.5,"#pi^{+}");
    }
    if(doSavePlots) {
      sprintf(str, "%s/pict_page%02d_%s_enerbin%d.%s",sPlotDirName.c_str(), m_npage, c1->GetName(), i_ener_sel, sformat.c_str());
      gPad->Print(str);
    }
  } // i_ener
//   c1->SetEditable(kFALSE);
  c1->Print(sfname.c_str()); m_npage++;

  sprintf(cname,"LCSinglePionsPerf_%s_classification_vs_ebeam",sparticle.c_str());
  TCanvas *c2 = new TCanvas(cname, cname, cc_xx, cc_yy);
  c2->cd();
  sprintf(cname,"LCSinglePionsPerf_%s_classification_vs_ebeam_pad1",sparticle.c_str());
  TPad *c2_pad1 = new TPad(cname, cname, 0.0, 0.9, 1.0, 1.0); c2_pad1->Draw();
  sprintf(cname,"LCSinglePionsPerf_%s_classification_vs_ebeam_pad2",sparticle.c_str());
  TPad *c2_pad2 = new TPad(cname, cname, 0.0, 0.0, 1.0, 0.9); c2_pad2->Draw();

  c2_pad1->cd();
  TPaveText *tp2 = make_header();
  tp2->AddText("Classification performance");
  tp2->AddText("TAGGEDEM, TAGGEDHAD and TAGGEDUNKNOWN energy fractions as a function of pion energy");
  tp2->AddText("for different eta bins");
  tp2->Draw();

  c2_pad2->Divide(2,2);
  for(int i_eta=0; i_eta<int(sizeof(a_etabin_sel)/sizeof(int)); i_eta++) {
    int i_eta_sel = a_etabin_sel[i_eta];
    c2_pad2->cd(i_eta+1);
    gPad->SetGrid();
    gPad->SetLogx();
    for(int i_tag=0; i_tag<m_ntagcases; i_tag++){
      TProfile *hp = m_engTag_vs_ebeam[i_tag][i_eta_sel];
      hp->SetLineColor(a_color_tag[i_tag]);
      hp->SetMarkerColor(a_color_tag[i_tag]);
      hp->SetMarkerSize(myMarkerSize);
      hp->SetMarkerStyle(20);
      hp->SetStats(0);
      hp->GetXaxis()->SetRangeUser(m_EnergyAxisMin, m_EnergyAxisMax);
      if( i_tag==0 ){
        hp->GetXaxis()->SetRangeUser(m_EnergyAxisMin, m_EnergyAxisMax);
        hp->SetMaximum(1.0);
        hp->SetMinimum(0.0);
        hp->Draw();
      } else {
        hp->Draw("same");
      }
    }
    sprintf(str,"#eta = %3.1f-%3.1f",m_etamin+m_deta*i_eta_sel, m_etamin+m_deta*(i_eta_sel+1));
    tex.SetTextSize(0.05);
    tex.DrawLatex(0.2,0.85,str);
    leg1->Draw();
    tex.SetTextSize(0.1);
    if(sparticle == "pizero"){
      tex.DrawLatex(0.25,0.5,"#pi^{0}");
    }else{
      tex.DrawLatex(0.25,0.5,"#pi^{+}");
    }
    // saving separate pictures
    if(doSavePlots) {
      sprintf(str, "%s/pict_page%02d_%s_etabin%d.%s",sPlotDirName.c_str(), m_npage, c2->GetName(), i_eta_sel,sformat.c_str());
      gPad->Print(str);
    }
  } // i_eta
  c2->Print(sfname.c_str());  m_npage++;
  //c2->SetEditable(kFALSE);
  return 0;
}



/* ****************************************************************************
  histograms to check reconstructed energy after each step of local hadronic 
  calibration, reco energy is defined as a sum of clusters energies within certain 
  angle around particle direction
**************************************************************************** */
int EngRecOverTruth(std::string &sfname)
{
  int a_color[]={kRed, kGreen, kBlue, kBlack, kOrange, kMagenta};
  char str[256];
  char cname[256];
  TLatex tex;
  tex.SetNDC(1);
  tex.SetTextSize(0.05);
  const char *c_keys_cls[5]={"em","w","ooc","dm",""};
  const char *c_title_norm[3]={"wrt pion energy", "wrt calibration hits", "wrt calibration hits with ideal previous step"};

  char hname[256];
  std::vector<std::vector<std::vector<TProfile *> > > m_engRecOverTruth_vs_eta;   // [ncluscoll][nnormtype][nenerbin]
  std::vector<std::vector<std::vector<TProfile *> > > m_engRecOverTruth_vs_ebeam; // [ncluscoll][nnormtype][netabin]

  // as a function of pion eta for several energy bins
  m_engRecOverTruth_vs_eta.resize(m_ncluscoll);
  for(int i_coll=0; i_coll<m_ncluscoll; i_coll++){
    m_engRecOverTruth_vs_eta[i_coll].resize(m_nnormtype);
    for(int i_norm=0; i_norm<m_nnormtype; i_norm++){
      m_engRecOverTruth_vs_eta[i_coll][i_norm].resize(m_nlogenerbin, 0);
      for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
        sprintf(hname, "engRecOverTruth/hp_engRecOverTruth_vs_eta_coll%d_norm%d_ener%d",i_coll, i_norm, i_ener);
        TProfile *hp = (TProfile *) top->Get(hname);
        if(!hp) {
          std::cout << "Can't get " << hname << std::endl;
          return 0;
        }
        if(i_norm==0) {
          hp->GetYaxis()->SetTitle("E_{reco}/E_{#pi}");
        }else{
          hp->GetYaxis()->SetTitle("E_{reco}/E_{calib}");
        }
        hp->GetYaxis()->SetTitleOffset(1.4);
        m_engRecOverTruth_vs_eta[i_coll][i_norm][i_ener] = hp;
      } // i_ener
    } // i_norm
  } // i_coll
  // as a function of pion energy for several eta bins
  m_engRecOverTruth_vs_ebeam.resize(m_ncluscoll);
  for(int i_coll=0; i_coll<m_ncluscoll; i_coll++){
    m_engRecOverTruth_vs_ebeam[i_coll].resize(m_nnormtype);
    for(int i_norm=0; i_norm<m_nnormtype; i_norm++){
      m_engRecOverTruth_vs_ebeam[i_coll][i_norm].resize(m_netabin, 0);
      for(int i_eta=0; i_eta<m_netabin; i_eta++) {
        sprintf(hname, "engRecOverTruth/hp_m_engRecOverTruth_vs_ebeam_coll%d_norm%d_eta%d",i_coll, i_norm, i_eta);
        TProfile *hp = (TProfile *) top->Get(hname);
        if(!hp) {
          std::cout << "Can't get " << hname << std::endl;
          return 0;
        }
        m_engRecOverTruth_vs_ebeam[i_coll][i_norm][i_eta] = hp;
        hp->GetXaxis()->SetTitle("E_{#pi}, MeV");
        hp->GetXaxis()->SetTitleOffset(1.3);
        if(i_norm==0) {
          hp->GetYaxis()->SetTitle("E_{reco}/E_{#pi}");
        }else{
          hp->GetYaxis()->SetTitle("E_{reco}/E_{calib}");
        }
        hp->GetYaxis()->SetTitleOffset(1.4);
      } // i_eta
    } // i_norm
  } // i_coll

  TLegend *leg = new TLegend(0.7,0.2,0.85,0.5);
  leg->SetTextSize(0.04);
  leg->SetBorderSize(1);
  leg->SetFillStyle(0);
  /* ********************************************
  as a function of pion eta for two energy bins
  ********************************************* */
  for(int i_norm=0; i_norm<m_nnormtype; i_norm++){ // loop over normalisation types
    sprintf(cname,"LCSinglePionsPerf_%s_EngRecOverTruth_norm%d_vs_eta",sparticle.c_str(), i_norm);
    TCanvas *c1 = new TCanvas(cname, cname, cc_xx, cc_yy);
    c1->cd();
    sprintf(cname,"LCSinglePionsPerf_%s_EngRecOverTruth_norm%d_vs_eta_pad1",sparticle.c_str(), i_norm);
    TPad *pad1 = new TPad(cname, cname, 0.0, 0.9, 1.0, 1.0); pad1->Draw();
    sprintf(cname,"LCSinglePionsPerf_%s_EngRecOverTruth_norm%d_vs_eta_pad2",sparticle.c_str(), i_norm);
    TPad *pad2 = new TPad(cname, cname, 0.0, 0.0, 1.0, 0.9); pad2->Draw();

    pad1->cd();
    TPaveText *tp1 = make_header();
    tp1->AddText("Reconstructed pion energy (sum over topoclusters within angle 1.5*mollier_factor)");
    tp1->AddText("As a function of pion coordinate for different energy bins");
    tp1->AddText(c_title_norm[i_norm]);
    tp1->Draw();

    pad2->Divide(2,2);
    for(int i_ener=0; i_ener<int(sizeof(a_enerbin_sel)/sizeof(int)); i_ener++){
      int i_ener_sel = a_enerbin_sel[i_ener];
      pad2->cd(i_ener+1);
      for(int i_coll=0; i_coll<m_ncluscoll; i_coll++){
        TProfile *hp = m_engRecOverTruth_vs_eta[i_coll][i_norm][i_ener_sel];
        hp->SetLineColor(a_color[i_coll]);
        hp->SetStats(0);
        hp->SetMarkerColor(a_color[i_coll]);
        hp->SetMarkerStyle(20);
        hp->SetMarkerSize(myMarkerSize);
        hp->GetXaxis()->SetRangeUser(m_EtaAxisMin, m_EtaAxisMax);
        if(i_norm==0){
          hp->SetMaximum(1.2);
          hp->SetMinimum(0.2);
        }else{
          hp->SetMaximum(1.25);
          hp->SetMinimum(0.75);
        }
        if( i_coll==0 ){
          hp->Draw();
        } else {
          hp->Draw("same");
        }
      } // i_coll
      sprintf(str,"E = %3.1f-%3.1f GeV", pow(10,m_logenermin+m_dlogener*i_ener_sel)/1000.,
                  pow(10,m_logenermin+m_dlogener*(i_ener_sel+1))/1000.);
      tex.DrawLatex(0.2,0.85,str);
      if(i_ener == 0 && i_norm==0) {
        for(int i_coll=m_ncluscoll-1; i_coll>=0; i_coll--){
          leg->AddEntry(m_engRecOverTruth_vs_eta[i_coll][i_norm][i_ener_sel], c_keys_cls[i_coll],"pl");
        }
      }
      leg->Draw();
      TLine line;
      line.SetLineStyle(3);
      if(m_etamin == 0) {
        line.DrawLine(-m_etamax, 0.98, m_etamax, 0.98);
        line.DrawLine(-m_etamax, 1.02, m_etamax, 1.02);
      }else{
        line.DrawLine(m_etamin, 0.98, m_etamax, 0.98);
        line.DrawLine(m_etamin, 1.02, m_etamax, 1.02);
      }
      if(doSavePlots) {
        sprintf(str, "%s/pict_page%02d_%s_enerbin%d.%s",sPlotDirName.c_str(), m_npage, c1->GetName(), i_ener_sel,sformat.c_str());
        gPad->Print(str);
      }
    } // i_ener
    c1->Print(sfname.c_str());  m_npage++;
  }

  for(int i_norm=0; i_norm<m_nnormtype; i_norm++){ // loop over normalisation types
    /* ********************************************
    as a function of pion energy for several eta bins
    ********************************************* */
    sprintf(cname,"LCSinglePionsPerf_%s_EngRecOverTruth_norm%d_vs_ebeam",sparticle.c_str(), i_norm);
    TCanvas *c2 = new TCanvas(cname, cname, cc_xx, cc_yy);
    c2->cd();
    sprintf(cname,"LCSinglePionsPerf_%s_EngRecOverTruth_norm%d_vs_ebeam_pad1",sparticle.c_str(), i_norm);
    TPad *pad1 = new TPad(cname, cname, 0.0, 0.9, 1.0, 1.0); pad1->Draw();
    sprintf(cname,"LCSinglePionsPerf_%s_EngRecOverTruth_norm%d_vs_ebeam_pad2",sparticle.c_str(), i_norm);
    TPad *pad2 = new TPad(cname, cname, 0.0, 0.0, 1.0, 0.9); pad2->Draw();

    pad1->cd();
    TPaveText *tp2 = make_header();
    tp2->AddText("Reconstructed pion energy (sum over topoclusters within angle 1.5*mollier_factor)");
    tp2->AddText("As a function of pion energy for different eta bins");
    tp2->AddText(c_title_norm[i_norm]);
    tp2->Draw();

    pad2->Divide(2,2);
    for(int i_eta=0; i_eta<int(sizeof(a_etabin_sel)/sizeof(int)); i_eta++){
      int i_eta_sel = a_etabin_sel[i_eta];
      pad2->cd(i_eta+1);
      gPad->SetGrid(1,0);
      gPad->SetLogx();

      for(int i_coll=0; i_coll<m_ncluscoll; i_coll++){
        TProfile *hp = m_engRecOverTruth_vs_ebeam[i_coll][i_norm][i_eta_sel];
        hp->SetLineColor(a_color[i_coll]);
        hp->SetStats(0);
        hp->SetMarkerColor(a_color[i_coll]);
        hp->SetMarkerStyle(20);
        hp->SetMarkerSize(myMarkerSize);
        hp->GetXaxis()->SetRangeUser(m_EnergyAxisMin, m_EnergyAxisMax);
        if(i_norm==0){
          hp->SetMaximum(1.2);
          hp->SetMinimum(0.2);
        }else{
          hp->SetMaximum(1.25);
          hp->SetMinimum(0.75);
        }
        if( i_coll==0 ){
          hp->Draw();
        } else {
          hp->Draw("same");
        }
      } // i_coll
      sprintf(str,"#eta = %3.1f-%3.1f",m_etamin+m_deta*i_eta_sel, m_etamin+m_deta*(i_eta_sel+1));
      tex.DrawLatex(0.2,0.85,str);
      leg->Draw();
      TLine line;
      line.SetLineStyle(3);
      line.DrawLine(1000., 0.98, pow(10,m_logenermax), 0.98);
      line.DrawLine(1000., 1.02, pow(10,m_logenermax), 1.02);
      if(doSavePlots) {
        sprintf(str, "%s/pict_page%02d_%s_etabin%d.%s",sPlotDirName.c_str(), m_npage, c2->GetName(),  i_eta_sel,sformat.c_str());
        gPad->Print(str);
      }
    } // i_eta

    c2->Print(sfname.c_str());  m_npage++;
    //c1->SetEditable(kFALSE);
  } // i_norm

  return 0;
}



/* ****************************************************************************
reconstructed energy spectras
**************************************************************************** */
int EngRecSpect(std::string &sfname)
{
  int a_color[]={kRed, kGreen, kBlue, kBlack, kOrange};
  char str[256];
  char cname[256];
  char hname[256];
  TLatex tex;
  tex.SetNDC(1);
  tex.SetTextSize(0.05);
  const char *c_keys_cls[5]={"em","w","ooc","dm",""};

  std::vector<std::vector<std::vector<TH1F *> > > m_engRecSpect; // [ncluscoll][nenerbin][netabin]
  m_engRecSpect.resize(m_ncluscoll);
  for(int i_coll=0; i_coll<m_ncluscoll; i_coll++){
    m_engRecSpect[i_coll].resize(m_nlogenerbin);
    for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
      m_engRecSpect[i_coll][i_ener].resize(m_netabin, 0);
      for(int i_eta=0; i_eta<m_netabin; i_eta++){
        sprintf(hname, "engRecSpect/hp_engRecSpect_coll%d_ener%d_eta%d",i_coll, i_ener, i_eta);
        TH1F *h1 = (TH1F *) top->Get(hname);
        if(!h1) {
          std::cout << "Can't get " << hname << std::endl;
          return 0;
        }
        h1->GetXaxis()->SetTitle("E_{reco}/E_{#pi}");
        m_engRecSpect[i_coll][i_ener][i_eta] = h1;
      } // i_eta
    } // i_ener
  } // i_coll

  std::vector<std::vector<TH1F *> > m_engPionSpect;   // [nenerbin][netabin]
  m_engPionSpect.resize(m_nlogenerbin);
  for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
    m_engPionSpect[i_ener].resize(m_netabin, 0);
    for(int i_eta=0; i_eta<m_netabin; i_eta++){
      sprintf(hname, "engRecSpect/hp_engPionSpect_ener%d_eta%d",i_ener, i_eta);
      TH1F *h1 = (TH1F *) top->Get(hname);
      if(!h1) {
        std::cout << "Can't get " << hname << std::endl;
        return 0;
      }
      m_engPionSpect[i_ener][i_eta] = h1;
    } // i_eta
  } // i_ener

  TLegend *leg = new TLegend(0.75,0.6,0.9,0.9);
  leg->SetTextSize(0.04);
  leg->SetBorderSize(1);
  leg->SetFillStyle(0);
  /* ********************************************
  energy spectras
  ********************************************* */
  for(int i_ener=0; i_ener<int(sizeof(a_enerbin_sel)/sizeof(int)); i_ener++){
    int i_ener_sel = a_enerbin_sel[i_ener];
    sprintf(cname,"LCSinglePionsPerf_%s_engRecSpect_enerbin%d",sparticle.c_str(), i_ener_sel);
    TCanvas *c1 = new TCanvas(cname, cname, cc_xx, cc_yy);
    c1->cd();
    sprintf(cname,"LCSinglePionsPerf_%s_engRecSpect_enerbin%d_pad1",sparticle.c_str(), i_ener_sel);
    TPad *pad1 = new TPad(cname, cname, 0.0, 0.9, 1.0, 1.0); pad1->Draw();
    sprintf(cname,"LCSinglePionsPerf_%s_engRecSpect_enerbin%d_pad2",sparticle.c_str(), i_ener_sel);
    TPad *pad2 = new TPad(cname, cname, 0.0, 0.0, 1.0, 0.9); pad2->Draw();
    pad1->cd();
    TPaveText *tp1 = make_header();
    tp1->AddText("Reconstructed pion energy spectras");
    sprintf(str,"E = %3.1f-%3.1f GeV", pow(10,m_logenermin+m_dlogener*i_ener_sel)/1000.,pow(10,m_logenermin+m_dlogener*(i_ener_sel+1))/1000.);
    tp1->AddText("Different eta bins");
    tp1->AddText(str);
    tp1->Draw();

    pad2->Divide(2,2);
    for(int i_eta=0; i_eta<int(sizeof(a_etabin_sel)/sizeof(int)); i_eta++){
      int i_eta_sel = a_etabin_sel[i_eta];
      pad2->cd(i_eta+1);
      // getting maximum
      float hmax = -999.;
      for(int i_coll=0; i_coll<m_ncluscoll; i_coll++) {
        if(hmax < m_engRecSpect[i_coll][i_ener_sel][i_eta_sel]->GetMaximum()) hmax = m_engRecSpect[i_coll][i_ener_sel][i_eta_sel]->GetMaximum();
      }
      for(int i_coll=0; i_coll<m_ncluscoll; i_coll++){
        TH1F *h1 = m_engRecSpect[i_coll][i_ener_sel][i_eta_sel];
        h1->SetLineColor(a_color[i_coll]);
        h1->SetStats(0);
        h1->SetMaximum(hmax*1.2);
        h1->SetMinimum(0.0);
        if( i_coll==0 ){
          h1->Draw();
        } else {
          h1->Draw("same");
        }
      } // i_coll
      if(i_eta == 0 && i_ener==0) {
        for(int i_coll=m_ncluscoll-1; i_coll>=0; i_coll--){
          leg->AddEntry(m_engRecSpect[i_coll][i_ener_sel][i_eta_sel], c_keys_cls[i_coll],"l");
        }
      }
      sprintf(str,"#eta = %3.1f-%3.1f",m_etamin+m_deta*i_eta_sel, m_etamin+m_deta*(i_eta_sel+1));
      tex.DrawLatex(0.2,0.85,str);
      leg->Draw();
      if(doSavePlots) {
        sprintf(str, "%s/pict_page%02d_%s_enerbin%d_etabin%d.%s",sPlotDirName.c_str(), m_npage, c1->GetName(),i_ener_sel,i_eta_sel,sformat.c_str());
        gPad->Print(str);
      }
    } // i_eta
    c1->Print(sfname.c_str());  m_npage++;
  } // i_ener
  return 0;
}



/* ****************************************************************************
resolution
**************************************************************************** */
int Resolution(std::string &sfname, bool doRMSCut, float nRMSCut)
{
  int a_color[]={kRed, kGreen, kBlue, kBlack, kOrange};
  char str[256];
  char cname[256];
  char hname[256];
  TLatex tex;
  tex.SetNDC(1);
  tex.SetTextSize(0.05);
  const char *c_keys_cls[5]={"em","w","ooc","dm",""};

  std::vector<std::vector<std::vector<TH1F *> > > m_engRecSpect; // [ncluscoll][nenerbin][netabin]
  m_engRecSpect.resize(m_ncluscoll);
  for(int i_coll=0; i_coll<m_ncluscoll; i_coll++){
    m_engRecSpect[i_coll].resize(m_nlogenerbin);
    for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
      m_engRecSpect[i_coll][i_ener].resize(m_netabin, 0);
      for(int i_eta=0; i_eta<m_netabin; i_eta++){
        sprintf(hname, "engRecSpect/hp_engRecSpect_coll%d_ener%d_eta%d",i_coll, i_ener, i_eta);
        TH1F *h1 = (TH1F *) top->Get(hname);
        if(!h1) {
          std::cout << "Can't get " << hname << std::endl;
          return 0;
        }
        h1->GetXaxis()->SetTitle("E_{reco}/E_{#pi}");
        m_engRecSpect[i_coll][i_ener][i_eta] = h1;
      } // i_eta
    } // i_ener
  } // i_coll
  std::vector<std::vector<TH1F *> > m_engPionSpect;   // [nenerbin][netabin]
  m_engPionSpect.resize(m_nlogenerbin);
  for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
    m_engPionSpect[i_ener].resize(m_netabin, 0);
    for(int i_eta=0; i_eta<m_netabin; i_eta++){
      sprintf(hname, "engRecSpect/hp_engPionSpect_ener%d_eta%d",i_ener, i_eta);
      TH1F *h1 = (TH1F *) top->Get(hname);
      if(!h1) {
        std::cout << "Can't get " << hname << std::endl;
        return 0;
      }
      m_engPionSpect[i_ener][i_eta] = h1;
    } // i_eta
  } // i_ener

  TLegend *leg = new TLegend(0.75,0.6,0.9,0.9);
  leg->SetTextSize(0.04);
  leg->SetBorderSize(1);
  leg->SetFillStyle(0);
  /* ********************************************
  resolution plots as a function of eta
  ********************************************* */
  sprintf(cname,"LCSinglePionsPerf_%s_Resolution%d_vs_eta",sparticle.c_str(), int(doRMSCut));
  TCanvas *c1 = new TCanvas(cname, cname, cc_xx, cc_yy);
  c1->cd();
  sprintf(cname,"LCSinglePionsPerf_%s_Resolution%d_vs_eta_pad1",sparticle.c_str(), int(doRMSCut));
  TPad *c1_pad1 = new TPad(cname, cname, 0.0, 0.9, 1.0, 1.0); c1_pad1->Draw();
  sprintf(cname,"LCSinglePionsPerf_%s_Resolution%d_vs_eta_pad2",sparticle.c_str(), int(doRMSCut));
  TPad *c1_pad2 = new TPad(cname, cname, 0.0, 0.0, 1.0, 0.9); c1_pad2->Draw();
  c1_pad1->cd();
  TPaveText *tp1 = make_header();
  tp1->AddText("Resolution (rms/mean) plots as a function of eta");
  tp1->AddText("For different energy bins");
  if(doRMSCut) {
    tp1->AddText("doRMSCut=true");
  }else{
    tp1->AddText("doRMSCut=false");
  }
  tp1->Draw();

  TH1F *h1ref = new TH1F("h1ref","h1ref",100, m_etamin, m_etamax);
  h1ref->SetMaximum(myHistResolutionMax);
  h1ref->SetMinimum(0.0);
  h1ref->GetXaxis()->SetTitle("#eta");
  h1ref->GetYaxis()->SetTitle("#sigma_{E}/E");
  h1ref->SetStats(0);
  h1ref->GetXaxis()->SetRangeUser(m_EtaAxisMin, m_EtaAxisMax);

  c1_pad2->Divide(2,2);
  for(int i_ener=0; i_ener<int(sizeof(a_enerbin_sel)/sizeof(int)); i_ener++){
    int i_ener_sel = a_enerbin_sel[i_ener];
    c1_pad2->cd(i_ener+1);
    gPad->SetGrid();
    h1ref->Draw();
    for(int i_coll = 0; i_coll<m_ncluscoll; i_coll++){
      TGraphErrors *gr = new TGraphErrors(m_netabin);
      if(i_ener==0) leg->AddEntry(gr, c_keys_cls[i_coll],"pl");
      for(int i_eta=0; i_eta<m_netabin; i_eta++){
        TH1F *h1 = m_engRecSpect[i_coll][i_ener_sel][i_eta];
        if(m_engPionSpect[i_ener_sel][i_eta]->GetMean() == 0.0 || h1->GetMean() == 0.0) {
          std::cout << " Resolution() -> Warning! Skipping point, no energy i_ener:" << i_ener_sel << " i_eta:" << i_eta  << " " << m_engPionSpect[i_ener_sel][i_eta]->GetMean() << " " << h1->GetMean() << std::endl;
          continue;
        }
        float y = h1->GetRMS()/h1->GetMean();
        if(doRMSCut){
          float xmean(0), xres(0);
          GetRmsWithoutTails(h1, xmean, xres, nRMSCut);
//           std::cout << "nRMSCut:" << nRMSCut 
//           << " h1->GetMean():" << h1->GetMean()
//           << " h1->GetRMS():" << h1->GetRMS()
//           << " r:" << h1->GetRMS()/h1->GetMean()
//           << " xmean:" << xmean
//           << " xres:" << xres
//           << " y:" << xres/xmean
//           << std::endl;
          y = xres/xmean;
        }
        gr->SetPoint(i_eta, m_etamin + (i_eta+0.5)*m_deta, y);
      }
      gr->SetLineColor(a_color[i_coll]);
      gr->SetMarkerColor(a_color[i_coll]);
      gr->SetMarkerSize(myMarkerSize);
      gr->Draw("pl");
    } // i_coll
    leg->Draw();
    sprintf(str,"E = %3.1f-%3.1f GeV",
            pow(10,m_logenermin+m_dlogener*i_ener_sel)/1000., 
                pow(10,m_logenermin+m_dlogener*(i_ener_sel+1))/1000.);
    tex.SetTextSize(0.05);
    tex.DrawLatex(0.2,0.85,str);
    if(doSavePlots) {
      sprintf(str, "%s/pict_page%02d_%s_enerbin%d.%s",sPlotDirName.c_str(), m_npage, c1->GetName(), i_ener_sel, sformat.c_str());
      gPad->Print(str);
    }
  }// i_ener;
  c1->Print(sfname.c_str());  m_npage++;

  /* ********************************************
  resolution plots as a function of energy
  ********************************************* */
  sprintf(cname,"LCSinglePionsPerf_%s_Resolution%d_vs_ebeam",sparticle.c_str(), int(doRMSCut));
  TCanvas *c2 = new TCanvas(cname, cname, cc_xx, cc_yy);
  c2->cd();
  sprintf(cname,"LCSinglePionsPerf_%s_Resolution%d_vs_ebeam_pad1",sparticle.c_str(), int(doRMSCut));
  TPad *c2_pad1 = new TPad(cname, cname, 0.0, 0.9, 1.0, 1.0); c2_pad1->Draw();
  sprintf(cname,"LCSinglePionsPerf_%s_Resolution%d_vs_ebeam_pad2",sparticle.c_str(), int(doRMSCut));
  TPad *c2_pad2 = new TPad(cname, cname, 0.0, 0.0, 1.0, 0.9); c2_pad2->Draw();
  c2_pad1->cd();
  TPaveText *tp2 = make_header();
  tp2->AddText("Resolution plots as a function of energy");
  tp2->AddText("For different eta bins");
  if(doRMSCut) {
    tp2->AddText("doRMSCut=true");
  }else{
    tp2->AddText("doRMSCut=false");
  }
  tp2->Draw();
  TH1F *h2ref = new TH1F("h2ref","h2ref",100, 1000., pow(10,m_logenermax));
  h2ref->SetMaximum(myHistResolutionMax);
  h2ref->SetMinimum(0.0);
  h2ref->GetXaxis()->SetTitle("E_{#pi}, MeV");
  h2ref->GetYaxis()->SetTitle("#sigma_{E}/E");
  h2ref->SetStats(0);
  c2_pad2->Divide(2,2);
  for(int i_eta=0; i_eta<int(sizeof(a_etabin_sel)/sizeof(int)); i_eta++){
    c2_pad2->cd(i_eta+1);
    gPad->SetGrid();
    gPad->SetLogx();
    h2ref->Draw();
    int i_eta_sel = a_etabin_sel[i_eta];
    for(int i_coll = 0; i_coll<m_ncluscoll; i_coll++){
      TGraphErrors *gr = new TGraphErrors(m_nlogenerbin);
      for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
        TH1F *h1 = m_engRecSpect[i_coll][i_ener][i_eta_sel];
        if(m_engPionSpect[i_ener][i_eta_sel]->GetMean() == 0.0 || h1->GetMean() == 0.0) {
          std::cout << " Resolution() -> Warning! Skipping point, no energy i_ener:" << i_ener << " i_eta_sel:" << i_eta_sel  << " " << m_engPionSpect[i_ener][i_eta_sel]->GetMean() << " " << h1->GetMean() << std::endl;
          continue;
        }
        float y = h1->GetRMS()/h1->GetMean();
        if(doRMSCut){
          float xmean(0), xres(0);
          GetRmsWithoutTails(h1, xmean, xres, nRMSCut);
          y = xres/xmean;
        }
        gr->SetPoint(i_ener, m_engPionSpect[i_ener][i_eta_sel]->GetMean(), y);
      }
      gr->SetLineColor(a_color[i_coll]);
      gr->SetMarkerColor(a_color[i_coll]);
      gr->SetMarkerSize(myMarkerSize);
      gr->Draw("pl");
    } // i_coll
    leg->Draw();
    sprintf(str,"#eta = %3.1f-%3.1f",m_etamin+m_deta*i_eta_sel, m_etamin+m_deta*(i_eta_sel+1));
    tex.SetTextSize(0.05);
    tex.DrawLatex(0.2,0.85,str);
    if(doSavePlots) {
      sprintf(str, "%s/pict_page%02d_%s_etabin%d.%s",sPlotDirName.c_str(), m_npage, c2->GetName(), i_eta_sel, sformat.c_str());
      gPad->Print(str);
    }
  }// i_eta
  c2->Print(sfname.c_str());  m_npage++;

  /* ********************************************
  another linearity plot: reco energy taken from mean of energy spectras
  ******************************************** */
  bool doAnotherLinearity = true;
  if(doAnotherLinearity && !doRMSCut){
    /* ********************************************
    linearity plots as a function of eta
    ********************************************* */
    sprintf(cname,"LCSinglePionsPerf_%s_Linearity_vs_eta",sparticle.c_str());
    TCanvas *c1 = new TCanvas(cname, cname, cc_xx, cc_yy);
    c1->cd();
    sprintf(cname,"LCSinglePionsPerf_%s_Linearity_vs_eta_pad1",sparticle.c_str());
    TPad *c1_pad1 = new TPad(cname, cname, 0.0, 0.9, 1.0, 1.0); c1_pad1->Draw();
    sprintf(cname,"LCSinglePionsPerf_%s_Linearity_vs_eta_pad2",sparticle.c_str());
    TPad *c1_pad2 = new TPad(cname, cname, 0.0, 0.0, 1.0, 0.9); c1_pad2->Draw();
    c1_pad1->cd();
    TPaveText *tp1 = make_header();
    tp1->AddText("Linearity plots as a function of eta");
    tp1->AddText("For different energy bins");
    tp1->AddText("");
    tp1->Draw();
    TH1F *h1ref = new TH1F("h1ref_lin","h1ref_lin",100, m_etamin, m_etamax);
    h1ref->SetMaximum(1.2);
    h1ref->SetMinimum(0.2);
    h1ref->GetXaxis()->SetTitle("#eta");
    h1ref->GetYaxis()->SetTitle("E_{reco}/E_{pi}");
    h1ref->SetStats(0);
    c1_pad2->Divide(2,2);
    for(int i_ener=0; i_ener<int(sizeof(a_enerbin_sel)/sizeof(int)); i_ener++){
      int i_ener_sel = a_enerbin_sel[i_ener];
      c1_pad2->cd(i_ener+1);
      h1ref->Draw();
      for(int i_coll = 0; i_coll<m_ncluscoll; i_coll++){
        TGraphErrors *gr = new TGraphErrors(m_netabin);
        for(int i_eta=0; i_eta<m_netabin; i_eta++){
          if(m_engPionSpect[i_ener_sel][i_eta]->GetMean() == 0.0) continue;
          TH1F *h1 = m_engRecSpect[i_coll][i_ener_sel][i_eta];
          gr->SetPoint(i_eta, m_etamin + (i_eta+0.5)*m_deta, h1->GetMean());
        }
        gr->SetLineColor(a_color[i_coll]);
        gr->SetMarkerColor(a_color[i_coll]);
        gr->SetMarkerSize(myMarkerSize);
        gr->Draw("pl");
      } // i_coll
      leg->Draw();
      sprintf(str,"E = %3.1f-%3.1f GeV",
              pow(10,m_logenermin+m_dlogener*i_ener_sel)/1000., 
                  pow(10,m_logenermin+m_dlogener*(i_ener_sel+1))/1000.);
      tex.SetTextSize(0.05);
      tex.DrawLatex(0.2,0.85,str);
      TLine line;
      line.SetLineStyle(3);
      line.DrawLine(m_etamin, 0.98, m_etamax, 0.98);
      line.DrawLine(m_etamin, 1.02, m_etamax, 1.02);
      if(doSavePlots) {
        sprintf(str, "%s/pict_page%02d_%s_enerbin%d.%s",sPlotDirName.c_str(), m_npage, c1->GetName(), i_ener_sel, sformat.c_str());
        gPad->Print(str);
      }
    }// i_ener;
    c1->Print(sfname.c_str());  m_npage++;

    /* ********************************************
    linearity plots as a function of energy
    ********************************************* */
    sprintf(cname,"LCSinglePionsPerf_%s_Linearity_vs_ebeam",sparticle.c_str());
    TCanvas *c2 = new TCanvas(cname, cname, cc_xx, cc_yy);
    c2->cd();
    sprintf(cname,"LCSinglePionsPerf_%s_Linearity_vs_ebeam_pad1",sparticle.c_str());
    TPad *c2_pad1 = new TPad(cname, cname, 0.0, 0.9, 1.0, 1.0); c2_pad1->Draw();
    sprintf(cname,"LCSinglePionsPerf_%s_Linearity_vs_ebeam_pad2",sparticle.c_str());
    TPad *c2_pad2 = new TPad(cname, cname, 0.0, 0.0, 1.0, 0.9); c2_pad2->Draw();
    c2_pad1->cd();
    TPaveText *tp2 = make_header();
    tp2->AddText("Linearity plots as a function of energy");
    tp2->AddText("For different eta bins");
    tp2->AddText("");
    tp2->Draw();
    TH1F *h2ref = new TH1F("h2ref_lin","h2ref_lin",100, 1000., pow(10,m_logenermax));
    h2ref->SetMaximum(1.2);
    h2ref->SetMinimum(0.2);
    h2ref->GetXaxis()->SetTitle("E_{#pi}");
    h2ref->GetYaxis()->SetTitle("E_{reco}/E_{pi}");
    h2ref->SetStats(0);
    c2_pad2->Divide(2,2);
    for(int i_eta=0; i_eta<int(sizeof(a_etabin_sel)/sizeof(int)); i_eta++){
      c2_pad2->cd(i_eta+1);
      gPad->SetGrid(1,0);
      gPad->SetLogx();
      h2ref->Draw();
      int i_eta_sel = a_etabin_sel[i_eta];
      for(int i_coll = 0; i_coll<m_ncluscoll; i_coll++){
        TGraphErrors *gr = new TGraphErrors(m_nlogenerbin);
        for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
          if(m_engPionSpect[i_ener][i_eta_sel]->GetMean() == 0.0) continue;
          TH1F *h1 = m_engRecSpect[i_coll][i_ener][i_eta_sel];
          gr->SetPoint(i_ener, m_engPionSpect[i_ener][i_eta_sel]->GetMean(), h1->GetMean());
        }
        gr->SetLineColor(a_color[i_coll]);
        gr->SetMarkerColor(a_color[i_coll]);
        gr->SetMarkerSize(myMarkerSize);
        gr->Draw("pl");
      } // i_coll
      leg->Draw();
      sprintf(str,"#eta = %3.1f-%3.1f",m_etamin+m_deta*i_eta_sel, m_etamin+m_deta*(i_eta_sel+1));
      tex.SetTextSize(0.05);
      tex.DrawLatex(0.2,0.85,str);
      TLine line;
      line.SetLineStyle(3);
      line.DrawLine(1000., 0.98, pow(10,m_logenermax), 0.98);
      line.DrawLine(1000., 1.02, pow(10,m_logenermax), 1.02);
      if(doSavePlots) {
        sprintf(str, "%s/pict_page%02d_%s_etabin%d.%s",sPlotDirName.c_str(), m_npage, c2->GetName(), i_eta_sel, sformat.c_str());
        gPad->Print(str);
      }
    }// i_eta
    c2->Print(sfname.c_str());  m_npage++;
  }

  return 0;
}



/* ****************************************************************************

**************************************************************************** */
int EngNoiseClus(std::string &sfname)
{
  char hname[256];
  char cname[256];
  TLatex tex;
  tex.SetNDC(1);
  tex.SetTextSize(0.1);
  int a_color[]={kRed, kGreen, kBlue, kBlack, kOrange};
  const char *c_keys_cls[5]={"em","w","ooc","dm",""};
  std::vector<TProfile *> m_engNoiseClus_vs_eta; // [ncluscoll]
  m_engNoiseClus_vs_eta.resize(m_ncluscoll, 0);
  for(int i_coll=0; i_coll<m_ncluscoll; i_coll++){
    sprintf(hname, "engNoiseClus/hp_engNoiseClus_vs_eta_coll%d",i_coll);
    TProfile *hp = (TProfile *)top->Get(hname);
    if(!hp) {
      std::cout << "Can't get " << hname << std::endl;
      return 0;
    }
    hp->GetXaxis()->SetTitle("#eta");
    hp->GetYaxis()->SetTitle("E_{clus}, MeV");
    m_engNoiseClus_vs_eta[i_coll] = hp;
    //hp->SetErrorOption("s");
  }

  sprintf(cname,"LCSinglePionsPerf_%s_NoiseClus_vs_eta",sparticle.c_str());
  TCanvas *c1 = new TCanvas(cname, cname, cc_xx, cc_yy);
  c1->cd();
  sprintf(cname,"LCSinglePionsPerf_%s_NoiseClus_vs_eta_pad1",sparticle.c_str());
  TPad *pad1 = new TPad(cname, cname, 0.0, 0.9, 1.0, 1.0); pad1->Draw();
  sprintf(cname,"LCSinglePionsPerf_%s_NoiseClus_vs_eta_pad2",sparticle.c_str());
  TPad *pad2 = new TPad(cname, cname, 0.0, 0.0, 1.0, 0.9); pad2->Draw();

  pad1->cd();
  TPaveText *tp1 = make_header();
  tp1->AddText("Average energy of noise clusters .vs. cluster coordinate");
  tp1->AddText("After each step of calibration");
  tp1->AddText("");
  tp1->Draw();

  pad2->Divide(2,2);
  for(int i_coll=0; i_coll<m_ncluscoll; i_coll++){
    pad2->cd(i_coll+1); gPad->SetGrid();
    TProfile *hp = m_engNoiseClus_vs_eta[i_coll];
    hp->SetMinimum(-500.);
    hp->SetMaximum(500.);
    hp->SetMarkerColor(a_color[i_coll]);
    hp->SetMarkerStyle(20);
    hp->SetMarkerSize(myMarkerSize);
    hp->Draw();
//       sprintf(str,"#eta = %3.1f-%3.1f",m_etamin+m_deta*i_eta_sel, m_etamin+m_deta*(i_eta_sel+1));
    tex.DrawLatex(0.3,0.8,c_keys_cls[i_coll]);
  }
  c1->Print(sfname.c_str());  m_npage++;

  return 0;
}



/* ****************************************************************************

**************************************************************************** */
int ClusterMoments(std::string &sfname)
{
  int a_color[]={kRed, kGreen, kBlue, kBlack, kOrange, kMagenta};
  char str[256];
  char cname[256];
  TLatex tex;
  tex.SetNDC(1);
  tex.SetTextSize(0.05);
  //const char *c_title_momsums[m_nmomsums]={"cls0","cls1","cls2","clssum","mom_miss"};
  //const char *c_title_moments[m_nmoments]={"ENG_CALIB_TOT", "ENG_CALIB_OUT_L", "ENG_CALIB_DEAD_TOT"};
  const char *c_title_momsums[]={"cls0","cls1","cls2","clssum","mom_miss","mom_toeng"};
  const char *c_title_moments[]={
    "ENG_CALIB_TOT", "ENG_CALIB_OUT_L", "ENG_CALIB_DEAD_TOT",
    "ENG_CALIB_DEAD_EMB0", "ENG_CALIB_DEAD_TILE0", "ENG_CALIB_DEAD_TILEG3",
    "ENG_CALIB_DEAD_EME0", "ENG_CALIB_DEAD_HEC0", "ENG_CALIB_DEAD_FCAL", "ENG_CALIB_DEAD_LEAKAGE",
   "ENG_CALIB_DEAD_UNCLASS"
  };

  char hname[256];

  std::vector<std::vector<std::vector<TProfile *> > > m_clusMoment_vs_eta;   // [nmoments][nmomsums][nenerbin]
  std::vector<std::vector<std::vector<TProfile *> > > m_clusMoment_vs_ebeam; // [nmoments][nmomsums][netabin]

  m_clusMoment_vs_eta.resize(m_nmoments);
  for(int i_mom=0; i_mom<m_nmoments; i_mom++){
    m_clusMoment_vs_eta[i_mom].resize(m_nmomsums);
    for(int i_sum=0; i_sum<m_nmomsums; i_sum++){
      m_clusMoment_vs_eta[i_mom][i_sum].resize(m_nlogenerbin, 0);
      for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
        sprintf(hname, "clusMoment/hp_clusMoment_vs_eta_mom%d_sum%d_ener%d",i_mom, i_sum, i_ener);
        TProfile *hp = (TProfile *) top->Get(hname);
        if(!hp) {
          std::cout << "Can't get " << hname << std::endl;
          return 0;
        }
        hp->GetXaxis()->SetTitle("#eta");
        hp->GetYaxis()->SetTitle("Mom_{cls}/Mom_{sum}");
        m_clusMoment_vs_eta[i_mom][i_sum][i_ener] = hp;
      } // i_ener
    } // i_sum
  } // i_mom

  m_clusMoment_vs_ebeam.resize(m_nmoments);
  for(int i_mom=0; i_mom<m_nmoments; i_mom++){
    m_clusMoment_vs_ebeam[i_mom].resize(m_nmomsums);
    for(int i_sum=0; i_sum<m_nmomsums; i_sum++){
      m_clusMoment_vs_ebeam[i_mom][i_sum].resize(m_netabin, 0);
      for(int i_eta=0; i_eta<m_netabin; i_eta++){
        sprintf(hname, "clusMoment/hp_clusMoment_vs_ebeam_mom%d_sum%d_eta%d",i_mom, i_sum, i_eta);
        TProfile *hp = (TProfile *) top->Get(hname);
        if(!hp) {
          std::cout << "Can't get " << hname << std::endl;
          return 0;
        }
        hp->GetXaxis()->SetTitle("E_{#pi}");
        hp->GetYaxis()->SetTitle("Mom_{cls}/Mom_{sum}");
        m_clusMoment_vs_ebeam[i_mom][i_sum][i_eta] = hp;
      } // i_eta
    } // i_sum
  } // i_mom

  TLegend *leg = new TLegend(0.7,0.4,0.87,0.7);
  leg->SetTextSize(0.04);
  leg->SetBorderSize(1);
  leg->SetFillStyle(0);
  /* ********************************************
  as a function of pion eta for two energy bins
  ********************************************* */
  for(int i_mom=0; i_mom<m_nmoments; i_mom++){ // loop over normalisation types
    sprintf(cname,"LCSinglePionsPerf_%s_ClusMoments_mom%d_vs_eta",sparticle.c_str(), i_mom);
    TCanvas *c1 = new TCanvas(cname, cname, cc_xx, cc_yy);
    c1->cd();
    sprintf(cname,"LCSinglePionsPerf_%s_ClusMoments_mom%d_vs_eta_pad1",sparticle.c_str(), i_mom);
    TPad *pad1 = new TPad(cname, cname, 0.0, 0.9, 1.0, 1.0); pad1->Draw();
    sprintf(cname,"LCSinglePionsPerf_%s_ClusMoments_mom%d_vs_eta_pad2",sparticle.c_str(), i_mom);
    TPad *pad2 = new TPad(cname, cname, 0.0, 0.0, 1.0, 0.9); pad2->Draw();

    pad1->cd();
    TPaveText *tp1 = make_header();
    tp1->AddText("Cluster moments assignment .vs. pion coordinate");
    tp1->AddText(c_title_moments[i_mom]);
    tp1->AddText("");
    tp1->Draw();

    pad2->Divide(2,2);
    for(int i_ener=0; i_ener<int(sizeof(a_enerbin_sel)/sizeof(int)); i_ener++) {
      int i_ener_sel = a_enerbin_sel[i_ener];
      pad2->cd(i_ener+1);
      for(int i_momsum=0; i_momsum<m_nmomsums; i_momsum++){
        TProfile *hp = m_clusMoment_vs_eta[i_mom][i_momsum][i_ener_sel];
        hp->SetLineColor(a_color[i_momsum]);
        hp->SetStats(0);
        hp->SetMarkerColor(a_color[i_momsum]);
        hp->SetMarkerStyle(20);
        hp->SetMarkerSize(myMarkerSize);
        hp->SetMaximum(1.1);
        hp->SetMinimum(-0.1);
        if(i_ener == 0 && i_mom==0) leg->AddEntry(hp, c_title_momsums[i_momsum],"pl");
        if( i_momsum==0 ){
          hp->Draw();
        } else {
          hp->Draw("same");
        }
      } // i_coll
      sprintf(str,"E = %3.1f-%3.1f GeV", pow(10,m_logenermin+m_dlogener*i_ener_sel)/1000.,
                  pow(10,m_logenermin+m_dlogener*(i_ener_sel+1))/1000.);
      tex.DrawLatex(0.2,0.85,str);
      leg->Draw();
      TLine line;
      line.SetLineStyle(3);
      if(m_etamin == 0) {
        line.DrawLine(-m_etamax, 1.0, m_etamax, 1.0);
        line.DrawLine(-m_etamax, 0.0, m_etamax, 0.0);
      }else{
        line.DrawLine(m_etamin, 1.0, m_etamax, 1.0);
        line.DrawLine(m_etamin, 0.0, m_etamax, 0.0);
      }
      if(doSavePlots) {
        sprintf(str, "%s/pict_page%02d_%s_enerbin%d.%s",sPlotDirName.c_str(), m_npage, c1->GetName(), i_ener_sel, sformat.c_str());
        gPad->Print(str);
      }
    } // i_ener
    c1->Print(sfname.c_str());  m_npage++;
  }

  for(int i_mom=0; i_mom<m_nmoments; i_mom++){ // loop over moments
    /* ********************************************
    as a function of pion energy for several eta bins
    ********************************************* */
    sprintf(cname,"LCSinglePionsPerf_%s_ClusMoments_mom%d_vs_ebeam",sparticle.c_str(), i_mom);
    TCanvas *c2 = new TCanvas(cname, cname, cc_xx, cc_yy);
    c2->cd();
    sprintf(cname,"LCSinglePionsPerf_%s_ClusMoments_mom%d_vs_ebeam_pad1",sparticle.c_str(), i_mom);
    TPad *pad1 = new TPad(cname, cname, 0.0, 0.9, 1.0, 1.0); pad1->Draw();
    sprintf(cname,"LCSinglePionsPerf_%s_ClusMoments_mom%d_vs_ebeam_pad2",sparticle.c_str(), i_mom);
    TPad *pad2 = new TPad(cname, cname, 0.0, 0.0, 1.0, 0.9); pad2->Draw();

    pad1->cd();
    TPaveText *tp2 = make_header();
    tp2->AddText("Cluster moments assignment .vs. pion energy");
    tp2->AddText(c_title_moments[i_mom]);
    tp2->AddText("");
    tp2->Draw();

    pad2->Divide(2,2);
    for(int i_eta=0; i_eta<int(sizeof(a_etabin_sel)/sizeof(int)); i_eta++){
      int i_eta_sel = a_etabin_sel[i_eta];
      pad2->cd(i_eta+1);
      gPad->SetGrid();
      gPad->SetLogx();

      for(int i_momsum=0; i_momsum<m_nmomsums; i_momsum++){
        TProfile *hp = m_clusMoment_vs_ebeam[i_mom][i_momsum][i_eta_sel];
        hp->SetLineColor(a_color[i_momsum]);
        hp->SetStats(0);
        hp->SetMarkerColor(a_color[i_momsum]);
        hp->SetMarkerStyle(20);
        hp->SetMarkerSize(myMarkerSize);
        hp->GetXaxis()->SetRangeUser(m_EnergyAxisMin, m_EnergyAxisMax);
        hp->SetMaximum(1.1);
        hp->SetMinimum(-0.1);
        if( i_momsum==0 ){
          hp->Draw();
        } else {
          hp->Draw("same");
        }
      } // i_coll
      sprintf(str,"#eta = %3.1f-%3.1f",m_etamin+m_deta*i_eta_sel, m_etamin+m_deta*(i_eta_sel+1));
      tex.DrawLatex(0.2,0.85,str);
      leg->Draw();
      TLine line;
      line.SetLineStyle(3);
      line.DrawLine(1000., 1.0, pow(10,m_logenermax), 1.0);
      if(doSavePlots) {
        sprintf(str, "%s/pict_page%02d_%s_enerbin%d.%s",sPlotDirName.c_str(), m_npage, c2->GetName(), i_eta_sel, sformat.c_str());
        gPad->Print(str);
      }
    } // i_eta

    c2->Print(sfname.c_str());  m_npage++;
    //c1->SetEditable(kFALSE);
  } // i_norm
  return 0;
}



/* ****************************************************************************

**************************************************************************** */
int RecoEfficiency(std::string &sfname)
{
  char str[256];
  char hname[256];
  char cname[256];
  TLatex tex;
  tex.SetNDC(1);
  tex.SetTextSize(0.05);

  // reconstruction efficiency
  std::vector<TH1F *> m_RecoEfficiency_vs_eta[2]; // [nenerbin]
  std::vector<TH1F *> m_RecoEfficiency_vs_ebeam[2]; // [netabin]

  // as a function of eta for several energy bins
  for(int i_eff=0; i_eff<2; i_eff++){ // 0-reconstructed, 1-all events
    m_RecoEfficiency_vs_eta[i_eff].resize(m_nlogenerbin, 0);
    for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
      sprintf(hname, "RecoEff/hp_m_RecoEfficiency_eff%d_ener%d", i_eff, i_ener);
      TH1F *h1 = (TH1F *) top->Get(hname);
      if(!h1) {
        std::cout << "Can't get " << hname << std::endl;
        return 0;
      }
      m_RecoEfficiency_vs_eta[i_eff][i_ener] = h1;
    } // i_eta
  } // i_eff
  // as a function of energy for several eta bins
  for(int i_eff=0; i_eff<2; i_eff++){ // 0-reconstructed, 1-all events
    m_RecoEfficiency_vs_ebeam[i_eff].resize(m_netabin, 0);
    for(int i_eta=0; i_eta<m_netabin; i_eta++){
      sprintf(hname, "RecoEff/hp_m_RecoEfficiency_eff%d_eta%d", i_eff, i_eta);
      TH1F *h1 = (TH1F *) top->Get(hname);
      if(!h1) {
        std::cout << "Can't get " << hname << std::endl;
        return 0;
      }
      m_RecoEfficiency_vs_ebeam[i_eff][i_eta] = h1;
    } // i_eta
  } // i_eff

//   TLegend *leg1 = new TLegend(0.65,0.35,0.90,0.6);
//   leg1->SetTextSize(0.04);
//   leg1->SetBorderSize(1);
//   leg1->SetFillStyle(0);

  sprintf(cname,"LCSinglePionsPerf_%s_RecoEfficiency_vs_ebeam",sparticle.c_str());
  TCanvas *c2 = new TCanvas(cname, cname, cc_xx, cc_yy);
  c2->cd();
  sprintf(cname,"LCSinglePionsPerf_%s_RecoEfficiency_vs_ebeam_pad1",sparticle.c_str());
  TPad *c2_pad1 = new TPad(cname, cname, 0.0, 0.9, 1.0, 1.0); c2_pad1->Draw();
  sprintf(cname,"LCSinglePionsPerf_%s_RecoEfficiency_vs_ebeam_pad2",sparticle.c_str());
  TPad *c2_pad2 = new TPad(cname, cname, 0.0, 0.0, 1.0, 0.9); c2_pad2->Draw();

  c2_pad1->cd();
  TPaveText *tp2 = make_header();
  tp2->AddText("Reconstruction efficiency");
  tp2->AddText("for different eta bins");
  tp2->Draw();

  c2_pad2->Divide(2,2);
  for(int i_eta=0; i_eta<int(sizeof(a_etabin_sel)/sizeof(int)); i_eta++) {
    int i_eta_sel = a_etabin_sel[i_eta];
    c2_pad2->cd(i_eta+1);
    gPad->SetGrid();
    gPad->SetLogx();
    TH1F *hp = m_RecoEfficiency_vs_ebeam[0][i_eta_sel];
    hp->Divide(m_RecoEfficiency_vs_ebeam[1][i_eta_sel]);
//     hp->SetLineColor(a_color_tag[i_tag]);
//     hp->SetMarkerColor(a_color_tag[i_tag]);
//     hp->SetMarkerSize(myMarkerSize);
//     hp->SetMarkerStyle(20);
    hp->SetStats(0);
    hp->GetXaxis()->SetRangeUser(m_EnergyAxisMin, m_EnergyAxisMax);
    hp->GetXaxis()->SetRangeUser(m_EnergyAxisMin, m_EnergyAxisMax);
    hp->SetMaximum(1.0);
    hp->SetMinimum(0.0);
    hp->Draw();
    sprintf(str,"#eta = %3.1f-%3.1f",m_etamin+m_deta*i_eta_sel, m_etamin+m_deta*(i_eta_sel+1));
    tex.SetTextSize(0.05);
    tex.DrawLatex(0.2,0.85,str);
    //leg1->Draw();
    tex.SetTextSize(0.1);
    if(sparticle == "pizero"){
      tex.DrawLatex(0.25,0.5,"#pi^{0}");
    }else{
      tex.DrawLatex(0.25,0.5,"#pi^{+}");
    }
    // saving separate pictures
    if(doSavePlots) {
      sprintf(str, "%s/pict_page%02d_%s_etabin%d.%s",sPlotDirName.c_str(), m_npage, c2->GetName(), i_eta_sel,sformat.c_str());
      gPad->Print(str);
    }
  } // i_eta
  c2->Print(sfname.c_str());  m_npage++;
  return 0;
}




/* ****************************************************************************

**************************************************************************** */
int CalibHitsValidation(std::string &sfname)
{

  
  return 0;
}



/* ****************************************************************************

**************************************************************************** */
void ReadParameters()
{
  TTree *tree = (TTree *)top->Get("Parameters/ParamTree");
  if(!tree) {
    std::cout << "ReadParameters() -> Warning! No parameters in the root file, using default" << std::endl;
    return;
  }
  tree->SetBranchAddress("m_netabin",&m_netabin);
  tree->SetBranchAddress("m_etamin",&m_etamin);
  tree->SetBranchAddress("m_etamax",&m_etamax);
  tree->SetBranchAddress("m_nphibin",&m_nphibin);
  tree->SetBranchAddress("m_phimin",&m_phimin);
  tree->SetBranchAddress("m_phimax",&m_phimax);
  tree->SetBranchAddress("m_nlogenerbin",&m_nlogenerbin);
  tree->SetBranchAddress("m_logenermin",&m_logenermin);
  tree->SetBranchAddress("m_logenermax",&m_logenermax);
  tree->SetBranchAddress("m_nnormtype",&m_nnormtype);
  tree->SetBranchAddress("m_ncluscoll",&m_ncluscoll);
  tree->SetBranchAddress("m_ntagcases",&m_ntagcases);
  tree->SetBranchAddress("m_nmoments",&m_nmoments);
  tree->SetBranchAddress("m_nmomsums",&m_nmomsums);
  tree->GetEntry(0);
  m_deta = (m_etamax - m_etamin)/m_netabin;
  m_dphi = (m_phimax - m_phimin)/m_nphibin;
  m_dlogener = (m_logenermax - m_logenermin)/m_nlogenerbin;
}


/* ****************************************************************************

**************************************************************************** */
TPaveText *make_header()
{
  TPaveText *tp1 = new TPaveText(0.08, 0.05, 0.97, 0.95, "NDC");
  tp1->SetFillStyle(0);
  tp1->SetBorderSize(1);
  tp1->SetTextSize(0.15);
  tp1->SetTextAlign(12);
  TText *tt = tp1->AddText(gSystem->BaseName(srootfname.c_str()));
  tt->SetTextSize(0.15);
  tt->SetTextAlign(32);
  return tp1;
}



/* ****************************************************************************

**************************************************************************** */
int GetRmsWithoutTails(const TH1F *pH, float &mean, float &rms, float ths)
{
  mean = pH->GetMean();
  rms = pH->GetRMS();
  float lim1 = mean - ths*rms;
  float lim2 = mean + ths*rms;

  double m_sum(0);
  double m_aver(0);
  double m_rms(0);
  double m_sw(0);
  for(int i=1; i<=(int)pH->GetNbinsX(); i++){
    //std::cout << " lim1:" << lim1 << " lim2:" << lim2 << " " << pH->GetBinCenter(i) << std::endl;
    if( pH->GetBinCenter(i)>=lim1 && pH->GetBinCenter(i)<= lim2 ){
      float xx = pH->GetBinCenter(i);
      float w = pH->GetBinContent(i);
      if(w==0) continue;
      m_sum += xx;
      m_rms = (m_sw/(m_sw+w))*(m_rms+(w/(m_sw+w))*(xx-m_aver)*(xx-m_aver));
      m_aver = m_aver+(xx-m_aver)*w/(m_sw+w);
      m_sw += w;
      //std::cout << " i:" << i << " x:" << xx << " w:" << w << " aver:" << m_aver << std::endl;
    }
  }
  if(m_rms!=0) m_rms = sqrt(m_rms);

  mean = m_aver;
  rms = m_rms;
  return 0;
}


