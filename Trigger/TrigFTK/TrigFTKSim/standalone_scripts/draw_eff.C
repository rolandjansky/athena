/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TROOT.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TGraphAsymmErrors.h"
#include "TCanvas.h"

#include <iostream>
using namespace std;

/* declare pointer to base histograms */
TH1 *histo_phi_distr;
TH1 *histo_curv_distr;
TH1 *histo_d0_distr;
TH1 *histo_eta_distr;
TH1 *histo_z0_distr;
TH1 *histo_pdg_distr;
TH2 *histo_etaz0_distr;
TH2 *histo_pdgcurv_distr;
TH2 *histo_pdgd0_distr;

EColor colorlist[] = {kBlack,kRed,kBlue,kOrange,kViolet,kGreen};
Int_t ncolors = 6;

TString osname;

TGraph* doEffPlot(TH1 *histoall, TH1 *histopassed,
		  EColor curcolor, const char *title)
{
  TGraphAsymmErrors *gr_eff = new TGraphAsymmErrors(histoall->GetNbinsX());
  gr_eff->BayesDivide(histopassed,histoall);
  gr_eff->SetMinimum(0);
  gr_eff->SetLineColor(curcolor);
  gr_eff->SetMarkerColor(curcolor);
  gr_eff->GetXaxis()->SetTitle(title);
  gr_eff->GetYaxis()->SetTitle("eff.");

  return gr_eff;
}

void doPlots(const char *suffix, int ifile, int pdgdet) {
  // get histograms
  TH1 *histo_curv = (TH1*) gDirectory->Get(Form("group_%s/histo_curv",suffix));
  TH1 *histo_phi = (TH1*) gDirectory->Get(Form("group_%s/histo_phi",suffix));
  TH1 *histo_d0 = (TH1*) gDirectory->Get(Form("group_%s/histo_d0",suffix));
  TH1 *histo_eta = (TH1*) gDirectory->Get(Form("group_%s/histo_eta",suffix));
  TH1 *histo_z0 = (TH1*) gDirectory->Get(Form("group_%s/histo_z0",suffix));
  TH1 *histo_pdg = (TH1*) gDirectory->Get(Form("group_%s/histo_pdg",suffix));
  TH2 *histo_etaz0 = (TH2*) gDirectory->Get(Form("group_%s/histo_etaz0",suffix));
  TH2 *histo_pdgcurv = (TH2*) gDirectory->Get(Form("group_%s/histo_pdgcurv",suffix));
  TH2 *histo_pdgd0 = (TH2*) gDirectory->Get(Form("group_%s/histo_pdgd0",suffix));

  cout << "Elaborate file: " << gDirectory->GetName() << endl;

  cout << "Efficiency study for group: " << suffix << endl;
  if (!histo_phi) {
    cout << "**** histograms for this group don't exist" << endl;
    return;
  }
  
  TString gopt="";
  EColor current_color = colorlist[ifile%ncolors];

  cout << "Elaborate file: " << gDirectory->GetName() << endl;  
  cout << "Use color: " << current_color << endl;

  /* study for efficiency as function of curvature */
  TString ccurv_name = Form("ceff_curv_%s",suffix);
  TCanvas *canvas_curv = (TCanvas*) gROOT->GetListOfCanvases()->FindObject(ccurv_name.Data());
  if (!canvas_curv) {
    canvas_curv = new TCanvas(ccurv_name.Data(),Form("Curv efficiency in %s",suffix),1);
    gopt = "ap";
   }
   else {
     canvas_curv->cd();
     gopt = "p";
   }
  TGraph *gr_eff_curv = doEffPlot(histo_curv_distr,histo_curv,current_color,"1/p_{T} [1/MeV]");
  gr_eff_curv->Draw(gopt);

  /* study for efficiency as function of phi */
  TString cphi_name = Form("ceff_phi_%s",suffix);
  TCanvas *canvas_phi = (TCanvas*) gROOT->GetListOfCanvases()->FindObject(cphi_name.Data());
  if (!canvas_phi) {
    canvas_phi = new TCanvas(cphi_name.Data(),Form("Phi efficiency in %s",suffix),1);
    gopt = "ap";
   }
   else {
     canvas_phi->cd();
     gopt = "p";
   }
  TGraphAsymmErrors *gr_eff_phi = doEffPlot(histo_phi_distr,histo_phi,current_color,"#phi [rad]");
  gr_eff_phi->Draw(gopt);

  /* study for efficiency as function of d0 */
  TString cd0_name = Form("ceff_d0_%s",suffix);
  TCanvas *canvas_d0 = (TCanvas*) gROOT->GetListOfCanvases()->FindObject(cd0_name.Data());
  if (!canvas_d0) {
    canvas_d0 = new TCanvas(cd0_name.Data(),Form("d0 efficiency in %s",suffix),1);
    gopt = "ap";
   }
   else {
     canvas_d0->cd();
     gopt = "p";
   }
  TGraphAsymmErrors *gr_eff_d0 = doEffPlot(histo_d0_distr,histo_d0,current_color,"d_{0} [mm]");
  gr_eff_d0->Draw(gopt);
  if (pdgdet) {
    TH1 *histo_pdgd0_pim = histo_pdgd0->ProjectionY("histo_pdgd0_pimP",3,3);
    TH1 *histo_pdgd0_pimall = histo_pdgd0_distr->ProjectionY("histo_pdgd0_pimA",3,3);
    doEffPlot(histo_pdgd0_pimall,histo_pdgd0_pim,kGreen,"dummy")->Draw("p");
    TH1 *histo_pdgd0_ep = histo_pdgd0->ProjectionY("histo_pdgd0_epP",4,4);
    TH1 *histo_pdgd0_epall = histo_pdgd0_distr->ProjectionY("histo_pdgd0_epA",4,4);
    doEffPlot(histo_pdgd0_epall,histo_pdgd0_ep,kRed,"dummy")->Draw("p");
    TH1 *histo_pdgd0_mup = histo_pdgd0->ProjectionY("histo_pdgd0_mupP",5,5);
    TH1 *histo_pdgd0_mupall = histo_pdgd0_distr->ProjectionY("histo_pdgd0_mupA",5,5);
    doEffPlot(histo_pdgd0_mupall,histo_pdgd0_mup,kBlue,"dummy")->Draw("p");
  }
  
  /* study of efficiency as function of eta */
  TString ceta_name = Form("ceff_eta_%s",suffix);
  TCanvas *canvas_eta = (TCanvas*) gROOT->GetListOfCanvases()->FindObject(ceta_name.Data());
  if (!canvas_eta) {
    canvas_eta = new TCanvas(ceta_name.Data(),Form("Eta efficiency in %s",suffix),1);
    gopt = "ap";
  }
  else {
    canvas_eta->cd();
    gopt = "p";
  }
  TGraphAsymmErrors *gr_eff_eta = doEffPlot(histo_eta_distr,histo_eta,current_color,"#eta");
  gr_eff_eta->Draw(gopt);
  
  /* study of efficiency as function of z0 */
  TString cz0_name = Form("ceff_z0_%s",suffix);
  TCanvas *canvas_z0 = (TCanvas*) gROOT->GetListOfCanvases()->FindObject(cz0_name.Data());
  if (!canvas_z0) {
    canvas_z0 = new TCanvas(cz0_name.Data(),Form("Z0 efficiency in %s",suffix),1);
    gopt = "ap";
  }
  else {
    canvas_z0->cd();
    gopt = "p";
  }
  TGraphAsymmErrors *gr_eff_z0 = doEffPlot(histo_z0_distr,histo_z0,current_color,"z_{0}");
  gr_eff_z0->Draw(gopt);
  
  /* study of officiency as function of truth pdg code */
  TString cpdg_name = Form("ceff_pdg_%s",suffix);
  TCanvas *canvas_pdg = (TCanvas*) gROOT->GetListOfCanvases()->FindObject(cpdg_name.Data());
  if (!canvas_pdg) {
    canvas_pdg = new TCanvas(cpdg_name.Data(),Form("PDG Efficiency in %s",suffix),1);
    gopt = "ap";
  }
  else {
    canvas_pdg->cd();
    gopt = "p";
  }
  Int_t nbinspdg = histo_pdg_distr->GetNbinsX();
  TGraphAsymmErrors *gr_eff_pdg = doEffPlot(histo_pdg_distr,histo_pdg,current_color,"PDG code");
  gr_eff_pdg->SetMinimum(0);
  gr_eff_pdg->Draw(gopt);
  Int_t grofL = gr_eff_pdg->GetXaxis()->GetFirst();
  Int_t grofR = gr_eff_pdg->GetXaxis()->GetLast();
  for (int ibin=1;ibin<=nbinspdg;++ibin) {
    Int_t grbin = gr_eff_pdg->GetXaxis()->FindBin(histo_pdg_distr->GetBinCenter(ibin));

    if (grbin<grofL||grbin>grofR) continue;
    gr_eff_pdg->GetXaxis()->SetBinLabel(grbin,histo_pdg_distr->GetXaxis()->GetBinLabel(ibin));
  }


  if (ifile==0 && false) {
    /* efficiency as function of z0 and eta, can show holes */
    TString cetaz0_name = Form("ceff_etaz0_%s",suffix);
    TCanvas *canvas_etaz0 = (TCanvas*) gROOT->GetListOfCanvases()->FindObject(cetaz0_name.Data());
    if (!canvas_etaz0) canvas_etaz0 = new TCanvas(cetaz0_name.Data(),Form("Eff. z0 vs eta in %s",suffix),1);
    histo_etaz0->Divide(histo_etaz0_distr);
    histo_etaz0->Draw("colz");

    /* efficiency as function of z0 and eta, can show holes */
    TString cpdgcurv_name = Form("ceff_pdgcurv_%s",suffix);
    TCanvas *canvas_pdgcurv = (TCanvas*) gROOT->GetListOfCanvases()->FindObject(cpdgcurv_name.Data());
    if (!canvas_pdgcurv) canvas_pdgcurv = new TCanvas(cpdgcurv_name.Data(),Form("Eff. curv vs pdg in %s",suffix),1);
    histo_pdgcurv->Divide(histo_pdgcurv_distr);
    histo_pdgcurv->Draw("box");
  }
}

void draw_eff(const char *opname="", int pdgdet=0,
	      int nregions=8, int subregions=16) {
  if (gDirectory==gROOT) {
    cerr << "It seems you dont't have opened any ROOT file" << endl;
    return;
  }

  osname = opname;

  // retrieve the files' list
  TSeqCollection *file_list = gROOT->GetListOfFiles();
  int nfiles = file_list->GetEntries();

  for (int ifile=0;ifile!=nfiles;++ifile) { // file loop
    TFile *current = (TFile*) file_list->At(ifile);
    current->cd();
        
    // take the basename for the eps and png plots from the file name
    TString basename = current->GetName();
    // strip the ROOT extension
    basename.Remove(basename.Length()-5);
    
    /** reference distributions */
    histo_curv_distr = (TH1*) gDirectory->Get("group_truth_fid/histo_curv");
    histo_phi_distr = (TH1*) gDirectory->Get("group_truth_fid/histo_phi");
    histo_d0_distr = (TH1*) gDirectory->Get("group_truth_fid/histo_d0");
    histo_eta_distr = (TH1*) gDirectory->Get("group_truth_fid/histo_eta");
    histo_z0_distr = (TH1*) gDirectory->Get("group_truth_fid/histo_z0");
    histo_pdg_distr = (TH1*) gDirectory->Get("group_truth_fid/histo_pdg");
    histo_etaz0_distr = (TH2*) gDirectory->Get("group_truth_fid/histo_etaz0");
    histo_pdgcurv_distr = (TH2*) gDirectory->Get("group_truth_fid/histo_pdgcurv");
    histo_pdgd0_distr = (TH2*) gDirectory->Get("group_truth_fid/histo_pdgd0");

    // define integral limits
    Double_t Barrel_min = -1;
    Double_t Barrel_max = 1;
    Int_t Barrel_minB = histo_phi_distr->FindBin(Barrel_min);
    Int_t Barrel_maxB = histo_phi_distr->FindBin(Barrel_max);

    Double_t nEndcap_min = -2.5;
    Double_t nEndcap_max = -1.1;
    Int_t nEndcap_minB = histo_phi_distr->FindBin(nEndcap_min);
    Int_t nEndcap_maxB = histo_phi_distr->FindBin(nEndcap_max);

    Double_t pEndcap_max = 2.5;
    Double_t pEndcap_min = 1.1;
    Int_t pEndcap_minB = histo_phi_distr->FindBin(pEndcap_min);
    Int_t pEndcap_maxB = histo_phi_distr->FindBin(pEndcap_max);
    

    // do plots for various vategories
    doPlots("ftk_effM",ifile,pdgdet);
    doPlots("ftk_effA",ifile,pdgdet);
    doPlots("road_effA",ifile,pdgdet);
    doPlots("ftk_trackaeffM",ifile,pdgdet);
  }  // end file loop

  if (osname.Length()>0) {
    gSystem->mkdir(osname.Data());
    gSystem->cd(osname.Data());
    TPDF report("eff_report.pdf");
    TList *iclist = gROOT->GetListOfCanvases();
    for (int icanvas=0;icanvas!=iclist->GetEntries();++icanvas) {
      if (icanvas!=0) report.NewPage();
      ((TCanvas*) iclist->At(icanvas))->Draw();
    }
    report.Close();
    gROOT->GetListOfCanvases()->Print(".eps");
    gROOT->GetListOfCanvases()->Print(".png");
    cout << endl;
  }
}
