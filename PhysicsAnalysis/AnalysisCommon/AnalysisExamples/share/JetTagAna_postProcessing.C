#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>

#include "TROOT.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLegend.h"

void plotPerf(std::string fileIn)
{
  
  TFile* file = new TFile(fileIn.c_str(),"read");

  std::string tagger[10];
  tagger[0] = "IP2D";
  tagger[1] = "IP3D";
  tagger[2] = "SV1";
  tagger[3] = "SV2";
  tagger[4] = "IP3DSV1";
  tagger[5] = "Lifetime2D";
  tagger[6] = "SecVtxTagBU";
  tagger[7] = "lhSig";
  tagger[8] = "SoftMuonTag";
  tagger[9] = "SoftElectronTag";
  for(int i=0;i<10;i++)
    {
      std::cout << "Starting with tagger " << tagger[i] << std::endl;

      TString tit("perf_"); tit+=tagger[i]; tit.ToLower();

      std::cout << "Histo " << tit << std::endl;

      // b-jets:
      TH1F* bjet =  (TH1F*)file->Get(tit+"_b");
      if(bjet==0) continue;
      double nbjets = bjet->GetEntries();

      // light jets:
      TH1F* ujet =  (TH1F*)file->Get(tit+"_u");
      if(ujet==0)continue;
      double nujets = ujet->GetEntries();

      // purified light jets:
      TH1F* upurjet =  (TH1F*)file->Get(tit+"_upur");
      if(upurjet==0)continue;
      double nupurjets = upurjet->GetEntries();

      std::cout << "Histograms " << bjet << " " << ujet << " " << upurjet << std::endl;

      int nbins = ujet->GetNbinsX();
      const int nmax = 200;
      if(nbins>nmax)std::cout << "JetTagAna_postProcessing ERROR: nbins too large" << std::endl;
      double beff[nmax] = {0};
      double ueff[nmax] = {0};
      double upureff[nmax] = {0};
      int ngdbins(nbins);
      int ngdbinspur(nbins);
      for(int k=0;k<nbins;k++)
	{
	  if( ngdbins   ==nbins && (bjet->Integral(k+1,nbins) == 0 || ujet   ->Integral(k+1,nbins) == 0) ) ngdbins = k;
	  if( ngdbinspur==nbins && (bjet->Integral(k+1,nbins) == 0 || upurjet->Integral(k+1,nbins) == 0) ) ngdbinspur = k;
	  if(nbjets)beff[k]    = bjet   ->Integral(k+1,nbins)/nbjets;
	  if(nujets)ueff[k]    = ujet   ->Integral(k+1,nbins)/nujets;
	  if(nupurjets)upureff[k] = upurjet->Integral(k+1,nbins)/nupurjets;
	}

      std::cout << "Done computing efficiencies..." << std::endl;

      if(ngdbins==0||ngdbinspur==0)continue;

      double urej   [nmax] = {0};
      double upurrej[nmax] = {0};
      for(int k=0;k<nbins;k++)
	{
	  if(ueff[k])urej[k] = 1./ueff[k];
	  if(upureff[k])upurrej[k] = 1./upureff[k];
	}  

      std::cout << "Done computing rejections..." << std::endl;

      TString cn = tit+"_canvas";
      TCanvas* c = new TCanvas(cn,cn,800,600);
      c->Draw();
      c->cd();

      TGraph* bVSupur(0);      
      if(ngdbinspur)
	{
	  bVSupur = new TGraph(ngdbinspur,beff,upurrej);
	  std::cout << "TGraph " << bVSupur << std::endl;      
	  bVSupur->SetName(tit+"_graph_pur");
	  bVSupur->SetTitle(tagger[i].c_str());
	  bVSupur->SetLineColor(2);
	  bVSupur->SetMarkerSize(1.3);
	  bVSupur->SetLineWidth(4);
	  bVSupur->SetMarkerColor(2);
	  bVSupur->SetMarkerStyle(21);
	  bVSupur->GetXaxis()->SetTitle("#epsilon_{B}");
	  bVSupur->GetYaxis()->SetTitle("Rejection");
	  bVSupur->Draw("ACP");
	}

      TGraph* bVSu(0);      
      if(ngdbins)
	{
	  bVSu = new TGraph(ngdbins,beff,urej);
	  std::cout << "TGraph " << bVSu << std::endl;      
	  bVSu->SetName(tit+"_graph");
	  bVSu->SetTitle(tagger[i].c_str());
	  bVSu->SetLineColor(4);
	  bVSu->SetMarkerSize(1.3);
	  bVSu->SetLineWidth(4);
	  bVSu->SetMarkerColor(4);
	  bVSu->SetMarkerStyle(20);
	  bVSu->GetXaxis()->SetTitle("#epsilon_{B}");
	  bVSu->GetYaxis()->SetTitle("Rejection");
	  if(ngdbinspur)bVSu->Draw("CPsame");
	  else bVSu->Draw("ACP");
	}

      TLegend* leg1    = new TLegend(0.55,0.7,0.9,.87);
      leg1->SetName(tit+"_leg");
      leg1->AddEntry(bVSu->GetName(),"All light jets","pl");
      leg1->AddEntry(bVSupur->GetName(),"Purified light jets","pl");
      leg1->SetFillColor(0);
      leg1->Draw();

      c->Print(tit+".eps");
      c->Print(tit+".gif");

      std::cout << "Done with tagger " << tagger[i] << std::endl;
    }
  
  file->Close();
  file->Delete();

}

