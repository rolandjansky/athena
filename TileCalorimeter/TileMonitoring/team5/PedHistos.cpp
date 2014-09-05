/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define PedHistos_cpp
#include <TROOT.h>
#include <TFile.h>
#include <TH1F.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TString.h>
#include <iostream>
#include <TF1.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TProfile.h>

//#define psdirectory "/tmp/Koongah/Comm/Ped/61453/"

#define psdirectory "/afs/cern.ch/user/t/tilebeam/scratch0/Commissioning/results/team5/pedestal/"

void Level1PedHisto_Raw_Sum(TString runno,TString filename,TString runtype,TString module,TString BinErrType="",bool draw_plots=false,TString outdir=""){
  TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(filename);
  if (!f) {
  TString defdir;
  TString deffile;
  defdir = "/castor/cern.ch/user/t/tilebeam/commissioning/";
  deffile = "tilemon_"+runno+"_"+runtype+".0.root";
  if(filename == "")
    {
      filename = defdir+deffile;
    }
    f = TFile::Open("rfio:"+filename);
  }

  if (!f->IsZombie()){
    if (f->cd(filename+":/Tile/RawChannel/Summary")){
      TH1F * histoMeanlo;
      TH1F * histoMeanhi;
      TH1F * histoRMSlo;
      TH1F * histoRMShi;
      TH1F * histosigmalo;
      TH1F * histosigmahi;
      histoMeanlo =(TH1F*)gDirectory->Get(module+"_lo_amp");
      histoMeanhi =(TH1F*)gDirectory->Get(module+"_hi_amp");
      histosigmalo=(TH1F*)gDirectory->Get(module+"_lo_sigma");
      histosigmahi=(TH1F*)gDirectory->Get(module+"_hi_sigma");
      histoRMSlo  =(TH1F*)gDirectory->Get(module+"_lo_rms");
      histoRMShi  =(TH1F*)gDirectory->Get(module+"_hi_rms");
      if (histoMeanlo ||histoMeanhi|| histosigmalo||histosigmahi||histoRMSlo||histoRMShi){
        TCanvas *RawSumm = new TCanvas("SummaryRaw"+module+runno,
          runno+" Module "+module+", All the chns , high & low-gain: Raw Channel Summary Plots",1200,900);
        if (BinErrType=="RMS"){
          for (int i=0;i<48;i++){
            if (histoMeanhi){
              histoMeanhi->SetBinError(i+1,histoRMShi->GetBinContent(i+1));
            }
            if (histoMeanlo){
              histoMeanlo->SetBinError(i+1,histoRMSlo->GetBinContent(i+1));
            }
          }
        }
        RawSumm->Divide(2,2);
        RawSumm->cd(1);
        gStyle->SetOptStat(0);
        if (histoMeanlo){
          histoMeanlo->SetTitle(module+" low-gain Efit: Mean Value");
          histoMeanlo->SetXTitle("Channels");
          histoMeanlo->Draw();
        }
        RawSumm->cd(2);
        if (histoMeanhi){
          histoMeanhi->SetTitle(module+" high-gain Efit: Mean Value");
          histoMeanhi->SetXTitle("Channels");
          histoMeanhi->Draw();
        }
        RawSumm->cd(4);
        if (histoRMShi){
          histoRMShi->SetXTitle("Channels");
          histoRMShi->SetTitle(module+" high-gain Efit: RMS and #sigma");
          histoRMShi->Draw();
        }
        if ((histosigmahi)&&(histoRMShi)){
          histosigmahi->Draw("same");
        }
        if ((histosigmahi)&&(!histoRMShi)){
          histosigmahi->SetXTitle("Channels");
          histosigmahi->SetTitle(module+" high-gain Efit: RMS and #sigma");
          histosigmahi->Draw("");
        }
 
        if (histoRMShi||histosigmahi){
          TLegend *GraphLegendhi=new TLegend(0.89,0.89,0.99,0.99);
          if (histoRMShi){
            GraphLegendhi->AddEntry(histoRMShi,"RMS","p");
          }
          if (histosigmahi){
            GraphLegendhi->AddEntry(histosigmahi,"#sigma","p");
          }
          GraphLegendhi->Draw();
        }
        RawSumm->cd(3);
        if (histoRMSlo){
          histoRMSlo->SetXTitle("Channels");
          histoRMSlo->SetTitle(module+" low-gain Efit: RMS and #sigma");
          histoRMSlo->Draw();
        }
        if ((histosigmalo)&&(histoRMSlo)){
          histosigmalo->Draw("same");
        }
        if ((histosigmalo)&&(!histoRMSlo)){
          histosigmalo->SetXTitle("Channels");
          histosigmalo->SetTitle(module+" low-gain Efit: RMS and #sigma");
          histosigmalo->Draw("");
        }
        if (histoRMSlo||histosigmalo){
	  TLegend *GraphLegendlo=new TLegend(0.89,0.89,0.99,0.99);
	  if (histoRMSlo){
	    GraphLegendlo->AddEntry(histoRMSlo,"RMS","p");
	  }
	  if (histosigmalo){
	    GraphLegendlo->AddEntry(histosigmalo,"#sigma","p");
	  }
	  GraphLegendlo->Draw();
	  RawSumm->cd(0);

	  if (draw_plots) {
	    if (outdir=="") {
	      outdir= psdirectory;
	    }
	    TString w_sfilenameps =outdir+"/"+module+"/r"+runno+"_"+module+"Ped_RawCh_Summ.ps";
	    RawSumm->Print(w_sfilenameps);
	  }
        }
      }
    }
  }
}

void Level1PedHisto_Dig_Sum(TString runno,TString filename,TString runtype,TString module,bool draw_plots=false,TString outdir=""){
  TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(filename);
  if (!f) {
  TString defdir = "/castor/cern.ch/user/t/tilebeam/commissioning/";
  TString deffile= "tilemon_"+runno+"_"+runtype+".0.root";
  
  if(filename == "")
    {
      filename = defdir+deffile;
    }
    f = TFile::Open("rfio:"+filename);
  }

  if (!f->IsZombie()){
    if (f->cd(filename+":/Tile/Digits/Summary")){
      TH1F * histoMeanlo    = (TH1F*)gDirectory->Get(module+"_lo_ped");
      TH1F * histoRMSlolofr = (TH1F*)gDirectory->Get(module+"_lo_rms_lfr");
      TH1F * histoRMSlohifr = (TH1F*)gDirectory->Get(module+"_lo_rms_hfr");
      TH1F * histoMeanhi    = (TH1F*)gDirectory->Get(module+"_hi_ped");
      TH1F * histoRMShilofr = (TH1F*)gDirectory->Get(module+"_hi_rms_lfr");
      TH1F * histoRMShihifr = (TH1F*)gDirectory->Get(module+"_hi_rms_hfr");

      if (histoMeanlo || histoRMSlolofr || histoRMSlohifr ||
          histoMeanhi || histoRMShilofr || histoRMShihifr){
        TCanvas *DigSumm = new TCanvas("SummaryDig"+module+runno,
          runno+" Module "+module+", All the chns , high & low-gain: Digits Summary Plots",1200,900);
        DigSumm->Divide(2,3);
        DigSumm->cd(1);
        if (histoMeanlo){
          histoMeanlo->SetXTitle("Channels");
          histoMeanlo->Draw("p"); //Why a option??? histoMeanlo->Draw("ap");
        }
        DigSumm->cd(2);
        if (histoMeanhi){
          histoMeanhi->SetXTitle("Channels");
          histoMeanhi->Draw("p");
        } 
        DigSumm->cd(3);
        if (histoRMSlohifr){
          histoRMSlohifr->SetXTitle("Channels");
          histoRMSlohifr->Draw("p");
        }
        DigSumm->cd(4);
        if (histoRMShihifr){
          histoRMShihifr->SetXTitle("Channels");
          histoRMShihifr->Draw("p");
        }
        DigSumm->cd(5);
        if (histoRMSlolofr){
          histoRMSlolofr->SetXTitle("Channels");
          histoRMSlolofr->Draw("p");
        }
        DigSumm->cd(6);
        if (histoRMShilofr){
          histoRMShilofr->SetXTitle("Channels");
          histoRMShilofr->Draw("p");
        }
        DigSumm->cd(0);
	  if (draw_plots) {
	    if (outdir=="") {
	      outdir= psdirectory;
	    }
	    TString w_sfilenameps =outdir+"/"+module+"/r"+runno+"_"+module+"Ped_Digit_Summ.ps";
	    DigSumm->Print(w_sfilenameps);
	  }
      }
    }
  }
}

void Level2PedHisto_Efit(TString runno,TString filename,TString runtype,TString module,Int_t ch,bool draw_plots=false,TString outdir=""){
  TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(filename);
  if (!f) {
  TString defdir="/castor/cern.ch/user/t/tilebeam/commissioning/";
  TString deffile="tilemon_"+runno+"_"+runtype+".0.root";

  if(filename == "")
    {
      filename = defdir+deffile;
    }
    f = TFile::Open("rfio:"+filename);
  }
  if (!f->IsZombie()){
    if (f->cd(filename+":/Tile/RawChannel/"+module)){
      TH1S * histolow;
      TH1S * histohigh;
      TCanvas *efitCanvas=new TCanvas("Efit_"+module+(Long_t)ch+runno,runno+" Module "+module+", chn "+(Long_t)ch+", high & low-gain: Efit Distribution",900,450);
      efitCanvas->Divide(2);
      Long_t pmt=(Long_t)ch;
      if (pmt<10){
        histolow=(TH1S*)gDirectory->Get(module+"_pmt_0"+pmt+"_lo_amp");
        histohigh=(TH1S*)gDirectory->Get(module+"_pmt_0"+pmt+"_hi_amp");
      }
      else{  
        histolow=(TH1S*)gDirectory->Get(module+"_pmt_"+pmt+"_lo_amp");
        histohigh=(TH1S*)gDirectory->Get(module+"_pmt_"+pmt+"_hi_amp");
      } 
      efitCanvas->cd(1);
      Int_t infbin=1;
      Int_t supbin=histolow->GetNbinsX();
      while (!(Int_t)histolow->GetBinContent(infbin)){infbin++;}
      while (!(Int_t)histolow->GetBinContent(supbin)){supbin--;}
      histolow->GetXaxis()->SetRange(infbin-3,supbin+3);
      histolow->SetXTitle("Efit"+module+"lo["+(Long_t)ch+"]");
      gPad->SetLogy(1);
      histolow->Draw();
      histolow->Fit("gaus","Q");
      histolow->GetFunction("gaus")->SetLineColor(2);
      gStyle->SetOptFit(1111);
      gStyle->SetOptStat(111111);

      efitCanvas->cd(2);
      infbin=1;
      supbin=histohigh->GetNbinsX();
      while (!(Int_t)histohigh->GetBinContent(infbin)){infbin++;}
      while (!(Int_t)histohigh->GetBinContent(supbin)){supbin--;}
      histohigh->GetXaxis()->SetRange(infbin-3,supbin+3);
      histohigh->SetXTitle("Efit"+module+"hi["+(Long_t)ch+"]");
      gPad->SetLogy(1);
      histohigh->Draw();
      histohigh->Fit("gaus","Q");
      histohigh->GetFunction("gaus")->SetLineColor(2);
      gStyle->SetOptFit(1111);
      gStyle->SetOptStat(111111);
      efitCanvas->cd(0);

      if (draw_plots) {
	if (outdir=="") {
	  outdir= psdirectory;
	}
      TString w_sfilenameps =outdir+"/"+module+"/r"+runno+"_"+module+"Ped_Efit"+(Long_t)ch+".ps";
      efitCanvas->Print(w_sfilenameps);
      }
    }
  }
}

void Level2PedHisto_Efit_AllCh(TString runno, TString filename,TString runtype,TString module, bool draw_plots=false,TString outdir=""){
  TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(filename);
  if (!f) {
  TString defdir= "/castor/cern.ch/user/t/tilebeam/commissioning/";
  TString deffile= "tilemon_"+runno+"_"+runtype+".0.root";

  if(filename == "")
    {
      filename = defdir+deffile;
    }
    f = TFile::Open("rfio:"+filename);
  }
  if (!f->IsZombie()){
    if (f->cd(filename+":/Tile/RawChannel/"+module)){
      TH1S * histolow[48];
      TH1S * histohigh[48];
      Long_t pmt;
      for (Int_t ch=0; ch < 48; ch++){
        pmt=(Long_t)ch;
        if (pmt<10){
          histolow[ch]=(TH1S*)gDirectory->Get(module+"_pmt_0"+pmt+"_lo_amp");
          histohigh[ch]=(TH1S*)gDirectory->Get(module+"_pmt_0"+pmt+"_hi_amp");
        }
        else{  
          histolow[ch]=(TH1S*)gDirectory->Get(module+"_pmt_"+pmt+"_lo_amp");
          histohigh[ch]=(TH1S*)gDirectory->Get(module+"_pmt_"+pmt+"_hi_amp");
        }
      } 
      TCanvas *efitlow23Canvas=new TCanvas("1Efit_"+module+runno,runno+" Module "+module+", chns 0-23, low-gain: Efit Distribution",800,640);
      Int_t infbin;
      Int_t supbin;
      efitlow23Canvas->Divide(6,4);
      for (Int_t ch=0; ch < 24;ch++){
        efitlow23Canvas->cd(ch+1);
        gPad->SetLogy(1);
        infbin=1;
        supbin=histolow[ch]->GetNbinsX();
        while (!(Int_t)histolow[ch]->GetBinContent(infbin)){infbin++;}
        while (!(Int_t)histolow[ch]->GetBinContent(supbin)){supbin--;}
        histolow[ch]->GetXaxis()->SetRange(infbin-3,supbin+3);
        histolow[ch]->SetXTitle("Efit"+module+"lo["+(Long_t)ch+"]");
        histolow[ch]->Draw();
        histolow[ch]->Fit("gaus","Q");
        histolow[ch]->GetFunction("gaus")->SetLineColor(2);
        gStyle->SetOptFit(1111);
        gStyle->SetOptStat(111111);
      }
      TCanvas *efithigh23Canvas=new TCanvas("2Efit_"+module+runno,runno+" Module "+module+", chns 0-23, high-gain: Efit Distribution",800,640);
      efithigh23Canvas->Divide(6,4);
      for (Int_t ch=0; ch < 24;ch++){
        efithigh23Canvas->cd(ch+1);
        gPad->SetLogy(1);
        infbin=1;
        supbin=histohigh[ch]->GetNbinsX();
        while (!(Int_t)histohigh[ch]->GetBinContent(infbin)){infbin++;}
        while (!(Int_t)histohigh[ch]->GetBinContent(supbin)){supbin--;}
        histohigh[ch]->GetXaxis()->SetRange(infbin-3,supbin+3);
        histohigh[ch]->SetXTitle("Efit"+module+"hi["+(Long_t)ch+"]");
        histohigh[ch]->Draw();
        histohigh[ch]->Fit("gaus","Q");
        histohigh[ch]->GetFunction("gaus")->SetLineColor(2);
        gStyle->SetOptFit(1111);
        gStyle->SetOptStat(111111);

      }
      TCanvas *efitlow47Canvas=new TCanvas("3Efit_"+module+runno,runno+" Module "+module+", chns 24-47, low-gain: Efit Distribution",800,640);
      efitlow47Canvas->Divide(6,4);
      for (Int_t ch=24; ch < 48;ch++){
        efitlow47Canvas->cd(ch-23);
        gPad->SetLogy(1);
        infbin=1;
        supbin=histolow[ch]->GetNbinsX();
        while (!(Int_t)histolow[ch]->GetBinContent(infbin)){infbin++;}
        while (!(Int_t)histolow[ch]->GetBinContent(supbin)){supbin--;}
        histolow[ch]->GetXaxis()->SetRange(infbin-3,supbin+3);
        histolow[ch]->SetXTitle("Efit"+module+"lo["+(Long_t)ch+"]");
        histolow[ch]->Draw();
        histolow[ch]->Fit("gaus","Q");
        histolow[ch]->GetFunction("gaus")->SetLineColor(2);
        gStyle->SetOptFit(1111);
        gStyle->SetOptStat(111111);

      }
      TCanvas *efithigh47Canvas=new TCanvas("4Efit_"+module+runno,runno+" Module "+module+", chns 24-47, high-gain: Efit Distribution",800,640);
      efithigh47Canvas->Divide(6,4);
      for (Int_t ch=24; ch < 48;ch++){
        efithigh47Canvas->cd(ch-23);
        gPad->SetLogy(1);
        infbin=1;
        supbin=histohigh[ch]->GetNbinsX();
        while (!(Int_t)histohigh[ch]->GetBinContent(infbin)){infbin++;}
        while (!(Int_t)histohigh[ch]->GetBinContent(supbin)){supbin--;}
        histohigh[ch]->GetXaxis()->SetRange(infbin-3,supbin+3);
        histohigh[ch]->SetXTitle("Efit"+module+"hi["+(Long_t)ch+"]");
        histohigh[ch]->Draw();
        histohigh[ch]->Fit("gaus","Q");
        histohigh[ch]->GetFunction("gaus")->SetLineColor(2);
        gStyle->SetOptFit(1111);
        gStyle->SetOptStat(111111);

      }

      if (draw_plots) {
	if (outdir=="") {
	  outdir= psdirectory;
	}
      TString o_sfilenameps = outdir+"/"+module+"/r"+runno+"_"+module+"Ped_Efit_All.ps[";
      TString w_sfilenameps = outdir+"/"+module+"/r"+runno+"_"+module+"Ped_Efit_All.ps";
      TString c_sfilenameps = outdir+"/"+module+"/r"+runno+"_"+module+"Ped_Efit_All.ps]";
      efitlow23Canvas->Print(o_sfilenameps);
      efitlow23Canvas->Print(w_sfilenameps);
      efitlow47Canvas->Print(w_sfilenameps);
      efithigh23Canvas->Print(w_sfilenameps);
      efithigh47Canvas->Print(w_sfilenameps);
      efithigh47Canvas->Print(c_sfilenameps);
      }

    }
  }
}


void Level2PedHisto_SamplesProfile(TString runno,TString filename,TString runtype,TString module,Int_t ch,bool draw_plots=false,TString outdir=""){
  TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(filename);
  if (!f) {
  TString defdir="/castor/cern.ch/user/t/tilebeam/commissioning/";
  TString deffile= "tilemon_"+runno+"_"+runtype+".0.root";

  if(filename == "")
    {
      filename = defdir+deffile;
    }
  f = TFile::Open("rfio:"+filename);
  }
  if (!f->IsZombie()){
    if (f->cd(filename+":/Tile/Digits/"+module)){
      TProfile * pmtProfilelo;
      TProfile * pmtProfilehi;
      Long_t pmt=(Long_t)ch;
      if (pmt<10){
        pmtProfilelo=(TProfile *)gDirectory->Get(module+"_pmt_0"+pmt+"_lo_prof");
        pmtProfilehi=(TProfile *)gDirectory->Get(module+"_pmt_0"+pmt+"_hi_prof");
      }
      else{  
        pmtProfilelo=(TProfile *)gDirectory->Get(module+"_pmt_"+pmt+"_lo_prof");
        pmtProfilehi=(TProfile *)gDirectory->Get(module+"_pmt_"+pmt+"_hi_prof");
      }
      if (pmtProfilelo || pmtProfilehi){
        TCanvas *Prof=new TCanvas("Prof_"+module+(Long_t)ch+runno,runno+" Module "+module+", chn "+(Long_t)ch+", high & low-gain: Samples Profile",900,450);
        Prof->Divide(2);
        Prof->cd(1);
        if (pmtProfilelo){
          pmtProfilelo->SetXTitle("Sample");
          pmtProfilelo->Draw();
        }
        Prof->cd(2);
        if (pmtProfilehi){
          pmtProfilehi->SetXTitle("Sample");
          pmtProfilehi->Draw();
        }
        Prof->cd(0);

	if (draw_plots) {
	  if (outdir=="") {
	  outdir= psdirectory;
	  }

	  TString w_sfilenameps =outdir+"/"+module+"/r"+runno+"_"+module+"Ped_Samples_Prof"+(Long_t)ch+".ps";
	  Prof->Print(w_sfilenameps);
	}
      }
    }
  }
}
