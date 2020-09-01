/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TH1D.h"
#include "TCanvas.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TPaveStats.h"

#include <vector>
#include <iostream>

void PrintInfo(TH1* hist,int ntoy)
{
  double integ,interr;
  integ=hist->IntegralAndError(1,hist->GetNbinsX(),interr);
  double relerr=interr/integ;
  double Neff=1/(relerr*relerr);
  std::cout<<hist->GetTitle()<<" Integral="<<integ<<" +- "<<interr<<" Neff="<<Neff<<" effective stats="<<Neff/ntoy*100<<"%"<<std::endl;
}

void CreateHistos(TString suffix,std::vector< TH1* >& hists,Int_t linecolor)
{
  hists.push_back(new TH1D(TString("x_")+suffix,"x Beamspot",240,-0.06,+0.06));
  hists.push_back(new TH1D(TString("y_")+suffix,"y Beamspot",240,-0.06,+0.06));
  hists.push_back(new TH1D(TString("z_")+suffix,"z Beamspot",250,-250,+250));
  for(auto h : hists) h->SetLineColor(linecolor);
}

void BeamSpotSize3DReweight()
{
  int ntoy=1000000;
  TVector3 borg(0.01,0.01,42);
  TVector3 bnew(0.008,0.008,35);
  std::vector< TString > bname={"x","y","z"};
  
  std::vector< TH1* > horg;
  std::vector< TH1* > hnew;
  std::vector< TH1* > horgtonew;
  std::vector< TH1* > horgtonewHaM;

  CreateHistos("Original",horg,2);
  CreateHistos("New",hnew,1);
  CreateHistos("OrgToNew",horgtonew,kGreen+2);
  CreateHistos("OrgToNewHitAndMiss",horgtonewHaM,4);

  TH1* hist_weight=new TH1D("hist_weight","Original->New weight",400,0,2);

  double maxweight=1;
  for(int i=0;i<3;++i) {
    double weight=TMath::Gaus(0,0,bnew[i],true)/TMath::Gaus(0,0,borg[i],true);
    maxweight*=weight;
    std::cout<<bname[i]<<": sigma original="<<borg[i]<<" new="<<bnew[i]<<" weight="<<weight<<" maxweight="<<maxweight<<std::endl;
  }

  for(int itoy=0;itoy<ntoy;++itoy) {
    TVector3 porg,pnew;
    double weight=1;
    for(int i=0;i<3;++i) {
      porg[i]=gRandom->Gaus(0,borg[i]);
      pnew[i]=gRandom->Gaus(0,bnew[i]);
      weight*=TMath::Gaus(porg[i],0,bnew[i],true)/TMath::Gaus(porg[i],0,borg[i],true);
      //std::cout<<bname[i]<<": weight="<<weight<<std::endl;
    }

    hist_weight->Fill(weight);
    for(int i=0;i<3;++i) {
      horg[i]->Fill(porg[i]);
      hnew[i]->Fill(pnew[i]);
      horgtonew[i]->Fill(porg[i],weight);
    }
    
    //Hit and miss unweighting
    if(gRandom->Rndm()>weight/maxweight) continue;

    for(int i=0;i<3;++i) {
      horgtonewHaM[i]->Fill(porg[i]);
    }
  }
  
  for(int i=0;i<3;++i) {
	  TCanvas* c=new TCanvas(bname[i]+"beamspot",bname[i]+" beamspot");

	  hnew[i]->Draw();
	  c->Modified(); 
	  c->Update();
	  TPaveStats *stats =(TPaveStats*)c->GetPrimitive("stats");
	  stats->SetName(bname[i]+"_stats_new");
	  stats->SetY1NDC(0.85);
	  stats->SetY2NDC(1.0);
	  stats->SetTextColor(1);

	  horg[i]->Draw("sames");
	  c->Update();
	  stats =(TPaveStats*)c->GetPrimitive("stats");
	  stats->SetName(bname[i]+"_stats_org");
	  stats->SetY1NDC(0.7);
	  stats->SetY2NDC(0.85);
	  stats->SetTextColor(2);

	  horgtonew[i]->Draw("sames");
	  c->Update();
	  stats =(TPaveStats*)c->GetPrimitive("stats");
	  stats->SetName(bname[i]+"_stats_orgtonew");
	  stats->SetY1NDC(0.55);
	  stats->SetY2NDC(0.7);
	  stats->SetTextColor(kGreen+2);

	  horgtonewHaM[i]->Sumw2();
	  horgtonewHaM[i]->Scale(ntoy/horgtonewHaM[i]->Integral());
	  horgtonewHaM[i]->Draw("sames");
	  c->Update();
	  stats =(TPaveStats*)c->GetPrimitive("stats");
	  stats->SetName(bname[i]+"_stats_orgtonewhitandmiss");
	  stats->SetY1NDC(0.4);
	  stats->SetY2NDC(0.55);
	  stats->SetTextColor(4);

	  c->SetLogy();
      c->SaveAs(".pdf");

	  PrintInfo(hnew[i],ntoy);
	  PrintInfo(horg[i],ntoy);
	  PrintInfo(horgtonew[i],ntoy);
	  PrintInfo(horgtonewHaM[i],ntoy);

      std::cout<<std::endl;
  }
  TCanvas* c=new TCanvas("hist_weight","weight");
  hist_weight->Draw();
  c->SaveAs(".pdf");
}
