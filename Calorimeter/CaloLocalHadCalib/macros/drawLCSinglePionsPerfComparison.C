/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Macro to draw performance plots for local hadronic calibration on single pions
//

#include <math.h>
#include <vector>
#include <iostream>
#include "TROOT.h"
#include "TFile.h"
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

const int m_netabin(25);
const float m_etamin(0.0);
const float m_etamax(5.0);
const float m_deta(0.2);
const float m_nlogenerbin(22);
const float m_logenermin(2.0);
const float m_logenermax(6.4);
const float m_dlogener(0.2);
const int m_nnormtype(3); // wrt pion energy, wrt calibration hits, wrt calibration hits with "ideal" previous step
const int m_ncluscoll(4); // em, w, ooc, dm
const int m_ntagcases(3); // taggedem, taggedhad, taggedunknown
const int m_nmoments(3);  // ENG_CALIB_TOT, ENG_CALIB_OUT_M, ENG_CALIB_DEAT_TOT
const int m_nmomsums(5);  // ratio of moment assigned to cls0, to cls1, to cls2, to sum of all cls, what remains unassigned

// selected bins to draw
int a_enerbin_sel[]={10, 15};
int a_etabin_sel[]={1, 7, 10, 18};

// output pictures
std::string sformat("eps");
bool doSavePlots = true;

int cc_xx = 1024;
int cc_yy = 768;
std::string sparticle;
int myMarkerSize(1.0);
double myHistResolutionMax(0.5);
std::string srootfname;

struct CompData {
  const char *ftitle;
  const char *fname;
  TFile *top;
  CompData(const char *ft, const char *fn, TFile *tt)
  {
    ftitle = ft; fname = fn; top = tt;
  }
};

std::vector<CompData > vCompData;

int EngRecOverTruth(std::string &sfname);
int Resolution(std::string &sfname);
TPaveText *make_header();


/* ****************************************************************************
main routine to produce validation plots for local hadron calibration on single pions
**************************************************************************** */
int drawLCSinglePionsComp()
{
  bool doEngRecOverTruth = true;
  bool doResolution = true;

  gROOT->SetBatch(true);

  gStyle->SetCanvasColor(10);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadColor(10);
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameFillColor(0);
  gStyle->SetFrameBorderMode(0);

//   // pi0
//   vCompData.push_back(CompData("PI0_CSC05_QGSPBERT", "/home/pospelov/data/mc08/LCSinglePionsPerf/LCSinglePionsPerf_pizero_tid023002_15.0.0_20042009_15.4.0_def_sum.root", 0));
//   vCompData.push_back(CompData("PI0_GEO08_QGSPBERT", "/home/pospelov/data/mc09/singlepart_singlepi_logE/LocalHadCalibCheck/LCSinglePionsPerf/LCSinglePionsPerf_pizero_r796_tid081903_15.5.1def.sum.root", 0));
//   vCompData.push_back(CompData("PI0_GEO08_FTFPBERT", "/home/pospelov/data/mc09/singlepart_singlepi_logE/LocalHadCalibCheck/LCSinglePionsPerf/LCSinglePionsPerf_pizero_r795_tid081900_15.5.1def.sum.root", 0));

//  // piplus
   vCompData.push_back(CompData("PI+_CSC05_QGSPBERT", "/home/pospelov/data/mc08/LCSinglePionsPerf/LCSinglePionsPerf_piplus_tid023006_15.0.0_20042009_15.4.0_def_sum.root", 0));
   vCompData.push_back(CompData("PI+_GEO08_QGSPBERT", "/home/pospelov/data/mc09/singlepart_singlepi_logE/LocalHadCalibCheck/LCSinglePionsPerf/LCSinglePionsPerf_piplus_r796_tid081901_15.5.1ndim.vv01.sum.root", 0));
   vCompData.push_back(CompData("PI+_GEO08_FTFPBERT", "/home/pospelov/data/mc09/singlepart_singlepi_logE/LocalHadCalibCheck/LCSinglePionsPerf/LCSinglePionsPerf_piplus_r795_tid081898_15.5.1ndim.vv01.sum.root", 0));

//  vCompData.push_back(CompData("PI+_GEO08_FTFPBERT_OLD", "/home/pospelov/data/mc09/singlepart_singlepi_logE/LocalHadCalibCheck/LCSinglePionsPerf/LCSinglePionsPerf_piplus_r795_tid081898_15.5.1def.sum.root", 0));
//  vCompData.push_back(CompData("PI+_GEO08_FTFPBERT", "/home/pospelov/data/mc09/singlepart_singlepi_logE/LocalHadCalibCheck/LCSinglePionsPerf/LCSinglePionsPerf_piplus_r795_tid081898_15.5.1ndim.vv01.sum.root", 0));

//   vCompData.push_back(CompData("PI+_GEO08_QGSPBERT_OLD", "/home/pospelov/data/mc09/singlepart_singlepi_logE/LocalHadCalibCheck/LCSinglePionsPerf/LCSinglePionsPerf_piplus_r796_tid081901_15.5.1def.sum.root", 0));
//   vCompData.push_back(CompData("PI+_GEO08_QGSPBERT", "/home/pospelov/data/mc09/singlepart_singlepi_logE/LocalHadCalibCheck/LCSinglePionsPerf/LCSinglePionsPerf_piplus_r796_tid081901_15.5.1ndim.vv01.sum.root", 0));
  
  
  srootfname = vCompData[0].fname;
  if( srootfname.find("pizero") != std::string::npos) {
    sparticle = "pizero";
    myHistResolutionMax = 0.5;
  }else if(srootfname.find("piplus") != std::string::npos) {
    sparticle = "piplus";
  }else if(srootfname.find("piminus") != std::string::npos) {
    sparticle = "piminus";
  }else{
    sparticle = "piunkn";
  }

  for(int i_file=0; i_file<int(vCompData.size()); i_file++){
    TFile *top = new TFile(vCompData[i_file].fname, "READ");
    if(  !top->IsOpen() ) {
      std::cout << "Can't open '" << vCompData[i_file].fname << "'." << std::endl;
      return 0;
    }
    vCompData[i_file].top = top;
    std::cout << "File " << vCompData[i_file].fname << " is opened " << top->IsOpen() << std::endl;
  }

  // preparing report file
  std::string sreport(gSystem->BaseName(vCompData[0].fname));
  sreport.erase(sreport.find_last_of("."));
  sreport = "./reports/report_comparison_" + sreport;
  sreport = sreport + std::string(".ps");
  TCanvas *ctmp = new TCanvas("ctmp","ctmp",cc_xx,cc_yy);
  ctmp->SetBatch(kTRUE);
  std::string sfname = sreport;
  sfname += "[";
  ctmp->Print(sfname.c_str()); 
  sfname = sreport;

  // reco energy after each step of calibration
  if(doEngRecOverTruth) {
    EngRecOverTruth(sfname);
  }

  // resolution plots
  if(doResolution) {
    Resolution(sfname);
  }

  sfname = sreport;
  sfname += "]";
  ctmp->Print(sfname.c_str());

  return 0;
}



/* ****************************************************************************
  histograms to check reconstructed energy after each step of local hadronic 
  calibration, reco energy is defined as a sum of clusters energies within certain 
  angle around particle direction
**************************************************************************** */
int EngRecOverTruth(std::string &sfname)
{
  int a_color[]={kRed, kGreen, kBlue, kBlack, kOrange};
  char str[256];
  char cname[256];
  TLatex tex;
  tex.SetNDC(1);
  tex.SetTextSize(0.05);
  const char *c_keys_cls[5]={"em","w","ooc","dm",""};
  const char *c_title_norm[3]={"wrt pion energy", "wrt calibration hits", "wrt calibration hits with ideal previous step"};

  char hname[256];
  std::vector<std::vector<std::vector<std::vector<TProfile *> > > > m_engRecOverTruth_vs_eta;   // [ncomp][ncluscoll][nnormtype][nenerbin]
  std::vector<std::vector<std::vector<std::vector<TProfile *> > > > m_engRecOverTruth_vs_ebeam; // [ncomp][ncluscoll][nnormtype][netabin]

  int nfiles = int(vCompData.size());

  // as a function of pion eta for several energy bins
  m_engRecOverTruth_vs_eta.resize(nfiles);
  for(int i_file=0; i_file<nfiles; i_file++){
    m_engRecOverTruth_vs_eta[i_file].resize(m_ncluscoll);
    for(int i_coll=0; i_coll<m_ncluscoll; i_coll++){
      m_engRecOverTruth_vs_eta[i_file][i_coll].resize(m_nnormtype);
      for(int i_norm=0; i_norm<m_nnormtype; i_norm++){
        m_engRecOverTruth_vs_eta[i_file][i_coll][i_norm].resize(m_nlogenerbin, 0);
        for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
          sprintf(hname, "engRecOverTruth/hp_engRecOverTruth_vs_eta_coll%d_norm%d_ener%d",i_coll, i_norm, i_ener);
          TProfile *hp = (TProfile *) vCompData[i_file].top->Get(hname);
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
          m_engRecOverTruth_vs_eta[i_file][i_coll][i_norm][i_ener] = hp;
        } // i_ener
      } // i_norm
    } // i_coll
  } // i_file

  // as a function of pion energy for several eta bins
  m_engRecOverTruth_vs_ebeam.resize(nfiles);
  for(int i_file=0; i_file<nfiles; i_file++){
    m_engRecOverTruth_vs_ebeam[i_file].resize(m_ncluscoll);
    for(int i_coll=0; i_coll<m_ncluscoll; i_coll++){
      m_engRecOverTruth_vs_ebeam[i_file][i_coll].resize(m_nnormtype);
      for(int i_norm=0; i_norm<m_nnormtype; i_norm++){
        m_engRecOverTruth_vs_ebeam[i_file][i_coll][i_norm].resize(m_netabin, 0);
        for(int i_eta=0; i_eta<m_netabin; i_eta++) {
          sprintf(hname, "engRecOverTruth/hp_m_engRecOverTruth_vs_ebeam_coll%d_norm%d_eta%d",i_coll, i_norm, i_eta);
          TProfile *hp = (TProfile *) vCompData[i_file].top->Get(hname);
          if(!hp) {
            std::cout << "Can't get " << hname << std::endl;
            return 0;
          }
          m_engRecOverTruth_vs_ebeam[i_file][i_coll][i_norm][i_eta] = hp;
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
  } // i_file

  TLegend *leg = new TLegend(0.4,0.2,0.85,0.5);
  leg->SetTextSize(0.04);
  leg->SetBorderSize(1);
  leg->SetFillStyle(0);
  /* ********************************************
  as a function of pion eta for two energy bins
  ********************************************* */
  for(int i_norm=0; i_norm<m_nnormtype; i_norm++){ // loop over normalisation types
    for(int i_coll=0; i_coll<m_ncluscoll; i_coll++){
      sprintf(cname,"LCSinglePionsComp_EngRecOverTruth_%s_norm%d_coll%d_vs_eta",sparticle.c_str(), i_norm, i_coll);
      TCanvas *c1 = new TCanvas(cname, cname, cc_xx, cc_yy);
      c1->cd();
      sprintf(cname,"LCSinglePionsComp_EngRecOverTruth_%s_norm%d_coll%d_vs_eta_pad1",sparticle.c_str(), i_norm, i_coll);
      TPad *pad1 = new TPad(cname, cname, 0.0, 0.9, 1.0, 1.0); pad1->Draw();
      sprintf(cname,"LCSinglePionsComp_EngRecOverTruth_%s_norm%d_coll%d_vs_eta_pad2",sparticle.c_str(), i_norm, i_coll);
      TPad *pad2 = new TPad(cname, cname, 0.0, 0.0, 1.0, 0.9); pad2->Draw();

      pad1->cd();
      TPaveText *tp1 = make_header();
      tp1->AddText("Reconstructed pion energy (sum over topoclusters within angle 1.5*mollier_factor)");
      tp1->AddText("As a function of pion coordinate for different energy bins");
      sprintf(str, "%s > %s ", c_keys_cls[i_coll], c_title_norm[i_norm]);
      tp1->AddText(str);
      tp1->Draw();

      pad2->Divide(2,2);
      for(int i_ener=0; i_ener<int(sizeof(a_enerbin_sel)/sizeof(int)); i_ener++) {
        int i_ener_sel = a_enerbin_sel[i_ener];
        pad2->cd(i_ener+1);
        for(int i_file=0; i_file<nfiles; i_file++){
          TProfile *hp = m_engRecOverTruth_vs_eta[i_file][i_coll][i_norm][i_ener_sel];
          hp->SetLineColor(a_color[i_file]);
          hp->SetStats(0);
          hp->SetMarkerColor(a_color[i_file]);
          hp->SetMarkerStyle(20);
          hp->SetMarkerSize(myMarkerSize);
          if(i_norm==0){
            hp->SetMaximum(1.2);
            hp->SetMinimum(0.2);
          }else{
            hp->SetMaximum(1.25);
            hp->SetMinimum(0.75);
          }
          if( i_file==0 ){
            hp->Draw();
          } else {
            hp->Draw("same");
          }
          if(i_ener == 0 && i_norm==0 && i_coll==0) {
            leg->AddEntry(hp, vCompData[i_file].ftitle,"pl");
          }
        } // i_file
        sprintf(str,"E = %3.1f-%3.1f GeV", pow(10,m_logenermin+m_dlogener*i_ener_sel)/1000.,
                    pow(10,m_logenermin+m_dlogener*(i_ener_sel+1))/1000.);
        tex.DrawLatex(0.2,0.85,str);
        leg->Draw();
        TLine line;
        line.SetLineStyle(3);
        line.DrawLine(-5.0, 0.98, 5.0, 0.98);
        line.DrawLine(-5.0, 1.02, 5.0, 1.02);
        if(doSavePlots) {
          sprintf(str, "./pictures/pict_%s_enerbin%d.%s",c1->GetName(),i_ener_sel,sformat.c_str());
          gPad->Print(str);
        }
      } // i_ener
      c1->Print(sfname.c_str());
    } // i_coll
  } // i_norm

  for(int i_norm=0; i_norm<m_nnormtype; i_norm++){ // loop over normalisation types
    for(int i_coll=0; i_coll<m_ncluscoll; i_coll++){
      /* ********************************************
      as a function of pion energy for several eta bins
      ********************************************* */
      sprintf(cname,"LCSinglePionsComp_EngRecOverTruth_%s_norm%d_coll%d_vs_ebeam",sparticle.c_str(), i_norm, i_coll);
      TCanvas *c2 = new TCanvas(cname, cname, cc_xx, cc_yy);
      c2->cd();
      sprintf(cname,"LCSinglePionsComp_EngRecOverTruth_%s_norm%d_coll%d_vs_ebeam_pad1",sparticle.c_str(), i_norm, i_coll);
      TPad *pad1 = new TPad(cname, cname, 0.0, 0.9, 1.0, 1.0); pad1->Draw();
      sprintf(cname,"LCSinglePionsComp_EngRecOverTruth_%s_norm%d_coll%d_vs_ebeam_pad2",sparticle.c_str(), i_norm, i_coll);
      TPad *pad2 = new TPad(cname, cname, 0.0, 0.0, 1.0, 0.9); pad2->Draw();

      pad1->cd();
      TPaveText *tp2 = make_header();
      tp2->AddText("Reconstructed pion energy (sum over topoclusters within angle 1.5*mollier_factor)");
      tp2->AddText("As a function of pion energy for different eta bins");
      sprintf(str, "%s > %s ", c_keys_cls[i_coll], c_title_norm[i_norm]);
      tp2->AddText(str);
      tp2->Draw();

      pad2->Divide(2,2);
      for(int i_eta=0; i_eta<int(sizeof(a_etabin_sel)/sizeof(int)); i_eta++){
        int i_eta_sel = a_etabin_sel[i_eta];
        pad2->cd(i_eta+1);
        gPad->SetGrid(1,0);
        gPad->SetLogx();

        for(int i_file=0; i_file<nfiles; i_file++){
          TProfile *hp = m_engRecOverTruth_vs_ebeam[i_file][i_coll][i_norm][i_eta_sel];
          hp->SetLineColor(a_color[i_file]);
          hp->SetStats(0);
          hp->SetMarkerColor(a_color[i_file]);
          hp->SetMarkerStyle(20);
          hp->SetMarkerSize(myMarkerSize);
          hp->GetXaxis()->SetRangeUser(1000.,pow(10,m_logenermax));
          if(i_norm==0){
            hp->SetMaximum(1.2);
            hp->SetMinimum(0.2);
          }else{
            hp->SetMaximum(1.25);
            hp->SetMinimum(0.75);
          }
          if( i_file==0 ){
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
          sprintf(str, "./pictures/pict_%s_etabin%d.%s",c2->GetName(),i_eta_sel,sformat.c_str());
          gPad->Print(str);
        }
      } // i_ener

      c2->Print(sfname.c_str());
      //c1->SetEditable(kFALSE);
    } // i_coll
  } // i_norm

  return 0;
}



/* ****************************************************************************
resolution
**************************************************************************** */
int Resolution(std::string &sfname)
{
  int a_color[]={kRed, kGreen, kBlue, kBlack, kOrange};
  char str[256];
  char cname[256];
  char hname[256];
  TLatex tex;
  tex.SetNDC(1);
  tex.SetTextSize(0.05);
  const char *c_keys_cls[5]={"em","w","ooc","dm",""};

  int nfiles = int(vCompData.size());

  std::vector<std::vector<std::vector<std::vector<TH1F *> > > > m_engRecSpect; // [nfiles][ncluscoll][nenerbin][netabin]

  m_engRecSpect.resize(nfiles);
  for(int i_file=0; i_file<nfiles; i_file++) {
    m_engRecSpect[i_file].resize(m_ncluscoll);
    for(int i_coll=0; i_coll<m_ncluscoll; i_coll++){
      m_engRecSpect[i_file][i_coll].resize(m_nlogenerbin);
      for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
        m_engRecSpect[i_file][i_coll][i_ener].resize(m_netabin, 0);
        for(int i_eta=0; i_eta<m_netabin; i_eta++){
          sprintf(hname, "engRecSpect/hp_engRecSpect_coll%d_ener%d_eta%d",i_coll, i_ener, i_eta);
          TH1F *h1 = (TH1F *) vCompData[i_file].top->Get(hname);
          if(!h1) {
            std::cout << "Can't get " << hname << std::endl;
            return 0;
          }
          h1->GetXaxis()->SetTitle("E_{reco}/E_{#pi}");
          m_engRecSpect[i_file][i_coll][i_ener][i_eta] = h1;
        } // i_eta
      } // i_ener
    } // i_coll
  } // i_file

  std::vector<std::vector<std::vector<TH1F *> > > m_engPionSpect;   // [i_file][nenerbin][netabin]
  m_engPionSpect.resize(nfiles);
  for(int i_file=0; i_file<nfiles; i_file++) {
    m_engPionSpect[i_file].resize(m_nlogenerbin);
    for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
      m_engPionSpect[i_file][i_ener].resize(m_netabin, 0);
      for(int i_eta=0; i_eta<m_netabin; i_eta++){
        sprintf(hname, "engRecSpect/hp_engPionSpect_ener%d_eta%d",i_ener, i_eta);
        TH1F *h1 = (TH1F *) vCompData[i_file].top->Get(hname);
        if(!h1) {
          std::cout << "Can't get " << hname << std::endl;
          return 0;
        }
        m_engPionSpect[i_file][i_ener][i_eta] = h1;
      } // i_eta
    } // i_ener
  } // i_file

  TLegend *leg = new TLegend(0.4,0.6,0.9,0.9);
  leg->SetTextSize(0.04);
  leg->SetBorderSize(1);
  leg->SetFillStyle(0);

  /* ********************************************
  resolution plots as a function of eta
  ********************************************* */
  for(int i_coll = 0; i_coll<m_ncluscoll; i_coll++){
    sprintf(cname,"LCSinglePionsComp_%s_Resolution_coll%d_vs_eta",sparticle.c_str(), i_coll);
    TCanvas *c1 = new TCanvas(cname, cname, cc_xx, cc_yy);
    c1->cd();
    sprintf(cname,"LCSinglePionsComp_%s_Resolution_coll%d_vs_eta_pad1",sparticle.c_str(), i_coll);
    TPad *c1_pad1 = new TPad(cname, cname, 0.0, 0.9, 1.0, 1.0); c1_pad1->Draw();
    sprintf(cname,"LCSinglePionsComp_%s_Resolution_coll%d_vs_eta_pad2",sparticle.c_str(), i_coll);
    TPad *c1_pad2 = new TPad(cname, cname, 0.0, 0.0, 1.0, 0.9); c1_pad2->Draw();
    c1_pad1->cd();
    TPaveText *tp1 = make_header();
    tp1->AddText("Resolution (rms/mean) plots as a function of eta");
    tp1->AddText("For different energy bins");
    sprintf(str, "%s >", c_keys_cls[i_coll]);
    tp1->AddText(str);
    tp1->Draw();
    TH1F *h1ref = new TH1F("h1ref","h1ref",100, 0.0, 5.0);
    h1ref->SetMaximum(myHistResolutionMax);
    h1ref->SetMinimum(0.0);
    h1ref->GetXaxis()->SetTitle("#eta");
    h1ref->GetYaxis()->SetTitle("#sigma_{E}/E");
    c1_pad2->Divide(2,2);
    for(int i_ener=0; i_ener<int(sizeof(a_enerbin_sel)/sizeof(int)); i_ener++){
      int i_ener_sel = a_enerbin_sel[i_ener];
      c1_pad2->cd(i_ener+1);
      gPad->SetGrid();
      h1ref->Draw();
      for(int i_file = 0; i_file<nfiles; i_file++){
        TGraphErrors *gr = new TGraphErrors(m_netabin);
        if(i_ener==0 && i_coll==0) leg->AddEntry(gr, vCompData[i_file].ftitle,"pl");
        for(int i_eta=0; i_eta<m_netabin; i_eta++){
          if(m_engPionSpect[i_file][i_ener_sel][i_eta]->GetMean() == 0.0) continue;
          TH1F *h1 = m_engRecSpect[i_file][i_coll][i_ener_sel][i_eta];
          gr->SetPoint(i_eta, (i_eta+0.5)*m_deta, h1->GetRMS()/h1->GetMean());
        }
        gr->SetLineColor(a_color[i_file]);
        gr->SetMarkerColor(a_color[i_file]);
        gr->SetMarkerSize(myMarkerSize);
        gr->Draw("pl");
      } // i_file
      leg->Draw();
      sprintf(str,"E = %3.1f-%3.1f GeV",
              pow(10,m_logenermin+m_dlogener*i_ener_sel)/1000., 
                  pow(10,m_logenermin+m_dlogener*(i_ener_sel+1))/1000.);
      tex.SetTextSize(0.05);
      tex.DrawLatex(0.2,0.85,str);
      if(doSavePlots) {
        sprintf(str, "./pictures/pict_%s_enerbin%d.%s",c1->GetName(), i_ener_sel, sformat.c_str());
        gPad->Print(str);
      }
    }// i_ener;
    c1->Print(sfname.c_str());
  } // i_coll


  /* ********************************************
  resolution plots as a function of energy
  ********************************************* */
  for(int i_coll = 0; i_coll<m_ncluscoll; i_coll++){
    sprintf(cname,"LCSinglePionsComp_%s_Resolution_coll%d_vs_ebeam",sparticle.c_str(), i_coll);
    TCanvas *c2 = new TCanvas(cname, cname, cc_xx, cc_yy);
    c2->cd();
    sprintf(cname,"LCSinglePionsComp_%s_Resolution_coll%d_vs_ebeam_pad1",sparticle.c_str(), i_coll);
    TPad *c2_pad1 = new TPad(cname, cname, 0.0, 0.9, 1.0, 1.0); c2_pad1->Draw();
    sprintf(cname,"LCSinglePionsComp_%s_Resolution_coll%d_vs_ebeam_pad2",sparticle.c_str(), i_coll);
    TPad *c2_pad2 = new TPad(cname, cname, 0.0, 0.0, 1.0, 0.9); c2_pad2->Draw();
    c2_pad1->cd();
    TPaveText *tp2 = make_header();
    tp2->AddText("Resolution plots as a function of energy");
    tp2->AddText("For different eta bins");
    sprintf(str, "%s >", c_keys_cls[i_coll]);
    tp2->AddText(str);
    tp2->Draw();
    TH1F *h2ref = new TH1F("h2ref","h2ref",100, 1000., pow(10,m_logenermax));
    h2ref->SetMaximum(myHistResolutionMax);
    h2ref->SetMinimum(0.0);
    h2ref->GetXaxis()->SetTitle("E_{#pi}");
    h2ref->GetYaxis()->SetTitle("#sigma_{E}/E");
    c2_pad2->Divide(2,2);
    for(int i_eta=0; i_eta<int(sizeof(a_etabin_sel)/sizeof(int)); i_eta++){
      c2_pad2->cd(i_eta+1);
      gPad->SetGrid();
      gPad->SetLogx();
      h2ref->Draw();
      int i_eta_sel = a_etabin_sel[i_eta];
      for(int i_file = 0; i_file<nfiles; i_file++){
        TGraphErrors *gr = new TGraphErrors(m_nlogenerbin);
        for(int i_ener=0; i_ener<m_nlogenerbin; i_ener++){
          if(m_engPionSpect[i_file][i_ener][i_eta_sel]->GetMean() == 0.0) continue;
          TH1F *h1 = m_engRecSpect[i_file][i_coll][i_ener][i_eta_sel];
          gr->SetPoint(i_ener, m_engPionSpect[i_file][i_ener][i_eta_sel]->GetMean(), h1->GetRMS()/h1->GetMean());
        }
        gr->SetLineColor(a_color[i_file]);
        gr->SetMarkerColor(a_color[i_file]);
        gr->SetMarkerSize(myMarkerSize);
        gr->Draw("pl");
      } // i_coll
      leg->Draw();
      sprintf(str,"#eta = %3.1f-%3.1f",m_etamin+m_deta*i_eta_sel, m_etamin+m_deta*(i_eta_sel+1));
      tex.SetTextSize(0.05);
      tex.DrawLatex(0.2,0.85,str);
      if(doSavePlots) {
        sprintf(str, "./pictures/pict_%s_etabin%d.%s",c2->GetName(), i_eta_sel, sformat.c_str());
        gPad->Print(str);
      }
    }// i_ener
    c2->Print(sfname.c_str());
  } // i_coll


  return 0;
}



TPaveText *make_header()
{
  TPaveText *tp1 = new TPaveText(0.08, 0.05, 0.97, 0.95, "NDC");
  tp1->SetFillStyle(0);
  tp1->SetBorderSize(1);
  tp1->SetTextSize(0.15);
  tp1->SetTextAlign(12);
  TText *tt = tp1->AddText(gSystem->BaseName(srootfname.c_str()));
  tt->SetTextSize(0.1);
  tt->SetTextAlign(32);
  return tp1;
}
