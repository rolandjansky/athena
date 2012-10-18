/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TH1.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TLegend.h>
#include <string>

bool debug =0;

void drawEffFake(TCanvas *c1, int isec, int imeas);
void drawF001(TCanvas* c1, int isec, int imeas);
void drawQpeak(TCanvas* c1, int isec, int imeas);
void drawqpFk(TCanvas* c1, int isec, int imeas);
int getIdxToSector(int ichm); 
enum HistType {
  Eff,
  Fake,
  Noise,
  QpSignal,
  QpFake
};
void decorateHist(TH1*, int color, HistType);

TFile *f;
TH1F* hEff[4], *hFake[4], *hf001[4], *hqp[4], *hqpFk[4];
TLegend *layerLgnd, *layerLgndSmall;


void macroEffFake(string jobname) {
  //  debug = deb;
  TCanvas* c1 = new TCanvas("c1","",0,0,800,800);
  string rootFileName = jobname+".root";
  f = new TFile(rootFileName.c_str());
  c1->Divide(2,2);
  c1->cd(1)->Divide(1,2);

  for (int isec=0; isec<32; ++isec) {
    int imeasphi =0;
    if (debug && isec!=12) continue;
    //    if (isec!=23) continue;

    //    cout << "isec " << isec << " meas " << imeasphi << endl; 
    drawEffFake(c1,isec,imeasphi);
    drawQpeak(c1,isec,imeasphi);
    drawF001(c1,isec,imeasphi);
    drawqpFk(c1,isec,imeasphi);


    char filename[100];
    sprintf(filename,"figures/sector%d_measphi%d.gif",isec,imeasphi);
    
    string fname = filename;
    string fullFilename = jobname + "/" + fname;
    //    cout << fullFilename << endl;
    
    if (!debug)
      c1->SaveAs(fullFilename.c_str());
  }
  
  c1->cd();
  c1->Modified();
  c1->Update();
}



void drawQpeak(TCanvas *c1, int isec, int imeasphi) {
  c1->cd(2);
  char qpname[30];
  char layname[30];
    
  layerLgndSmall= new TLegend(.65,.35,.85,.55);
  layerLgndSmall->SetTextSize(0.06);
  layerLgndSmall->SetFillColor(0);
  layerLgndSmall->SetBorderSize(0);
    
  float maxY = 0;
  for (Int_t i=0; i<4; i++){
    sprintf(qpname, "qps%dl%dm0",isec,i);
    sprintf(layname, "lay %d", i+1);
    
    hqp[i]  = (TH1F*) f->Get(qpname);
    if (imeasphi) 
      hqp[i] ->SetTitle(Form("#phi Sector %d",getIdxToSector(isec)));
    else 
      hqp[i] ->SetTitle(Form("#eta Sector %d",getIdxToSector(isec)));

    decorateHist(hqp[i],i+1,QpSignal);
    layerLgndSmall->AddEntry(hqp[i],layname);      
    if (maxY<hqp[i]->GetMaximum()) maxY =hqp[i]->GetMaximum();
  }

  for (Int_t i=0; i<4; i++){
    hqp[i]->SetMaximum(maxY*1.15);
    if (debug)
      if (i==0) hqp[i]->DrawCopy("");
      else hqp[i]->DrawCopy("same");
    else
      if (i==0) hqp[i]->Draw("");
      else hqp[i]->Draw("same");
  }
  layerLgndSmall->Draw();

}

void drawF001(TCanvas *c1, int isec, int imeasphi) {
  c1->cd(3);
  //  c1->cd(3)->SetGridy();
  char F001name[30];

  float maxY=0;
  for (Int_t i=0; i<4; i++){
    sprintf(F001name, "f001_s%dl%dm0",isec,i);
    hf001[i]  = (TH1F*) f->Get(F001name);
    decorateHist(hf001[i],i+1,Noise);
    hf001[i]->SetMinimum(0.5);
    hf001[i]->GetXaxis()->SetNdivisions(202);

    if (maxY<hf001[i]->GetMaximum())
      maxY =hf001[i]->GetMaximum();

    if (imeasphi) 
      hf001[i] ->SetTitle(Form("#phi Sector %d",getIdxToSector(isec)));
    else 
      hf001[i] ->SetTitle(Form("#eta Sector %d",getIdxToSector(isec)));
    
    if (debug)
      if (i==0) hf001[i]->DrawCopy("");
      else hf001[i]->DrawCopy("same");
    else
      if (i==0) hf001[i]->Draw("");
      else hf001[i]->Draw("same");
  }
  layerLgndSmall->Draw();

  if (maxY>0)
    c1->cd(3)->SetLogy();
    
  
}

void drawqpFk(TCanvas *c1, int isec, int imeasphi) {
  c1->cd(4);
  //  c1->cd(4)->SetGrid();
  char qpFkname[30];
  char layname[30];

  float maxY = 0;
  for (Int_t i=0; i<4; i++){
    sprintf(qpFkname, "qpFks%dl%dm0",isec,i);
    sprintf(layname, "lay %d", i+1);
    
    hqpFk[i]  = (TH1F*) f->Get(qpFkname);
    if (imeasphi) 
      hqpFk[i] ->SetTitle(Form("#phi Sector %d",getIdxToSector(isec)));
    else 
      hqpFk[i] ->SetTitle(Form("#eta Sector %d",getIdxToSector(isec)));

    if (maxY<hqpFk[i]->GetMaximum()) maxY =hqpFk[i]->GetMaximum();
  }

  
  TLegend* myLg= new TLegend(.65,.35,.85,.55);
  myLg->SetTextSize(0.06);
  myLg->SetFillColor(0);
  myLg->SetBorderSize(0);
  
  for (Int_t i=0; i<4; i++){
    hqpFk[i]->SetMaximum(maxY*1.25);
    decorateHist(hqpFk[i],i+1,QpFake);
    hqpFk[i]  ->GetXaxis() ->SetRangeUser(0,250000);
    hqpFk[i]  ->SetLineColor(i+1);
    sprintf(layname, "lay %d", i+1);
    myLg->AddEntry(hqp[i],layname);      
  if (debug)
      if (i==0) hqpFk[i]->DrawCopy("");
      else hqpFk[i]->DrawCopy("same");
    else
      if (i==0) hqpFk[i]->Draw("");
      else hqpFk[i]->Draw("same");
  }

  if (maxY>0)
    c1->cd(4)->SetLogy();

  myLg->Draw();
}

void drawEffFake(TCanvas *c1, int isec, int imeasphi) {//{

  TPad* p1 = (TPad*)c1->FindObject("c1_1");  

  p1->SetGrid();

  TPad* p11 = (TPad*) p1->GetPad(1);
  TPad* p12 = (TPad*) p1->GetPad(2);
  p11->SetGrid(); //  c1->cd(1)->cd(1)->SetGrid();  
  p12->SetGrid(); //  c1->cd(1)->cd(2)->SetGrid();  
  
  float r = 0.55;
  p11->SetPad(0,  r, 1.,1.);
  p12->SetPad(0,  0, 1.,r);
  p12->SetBottomMargin(0.25);
  p11->SetBottomMargin(0);
  p12->SetTopMargin(0);


  char effHname[30];
  char fakeHname[30];
  char layname[30];
  char sname[30];
  
  TLegend *layerLgnd;
  
  layerLgnd= new TLegend(.65,.45,.83,.75);
  layerLgnd->SetTextSize(0.10);
  layerLgnd->SetFillColor(0);
  layerLgnd->SetBorderSize(0);

  float maxY =0;
  for (Int_t i=0; i<4; i++){
    sprintf(effHname, "layEffEtaf001_s%dl%d",isec,i+1);
    sprintf(fakeHname,"layFakeEtaf001_s%dl%d",isec,i+1);
    sprintf(layname, "lay %d", i+1);
    
    c1->cd(1)->cd(1);
    hEff[i]  = (TH1F*) f->Get(effHname);
    hEff[i] ->SetTitle(Form("Sector %d",getIdxToSector(isec)));
    decorateHist(hEff[i],i+1,Eff); // effi ->true
    
    layerLgnd->AddEntry(hEff[i],layname);
    //      hEff->SetLineColor(i+1);
    if (debug) 
      if (i==0) hEff[i]->DrawCopy("");
      else hEff[i]->DrawCopy("same");
    else 
      if (i==0) hEff[i]->Draw("");
      else hEff[i]->Draw("same");
    
    c1->cd(1)->cd(2);
    hFake[i] = (TH1F*) f->Get(fakeHname);
    decorateHist(hFake[i],i+1,Fake); //fake->false
    if (maxY<hFake[i]->GetMaximum())
      maxY =hFake[i]->GetMaximum();
    //      hFake ->SetMinimum(0.0001);
    //      hFake->SetLineColor(i+1);
    if (debug) 
      if (i==0) hFake[i]->DrawCopy("");
      else hFake[i]->DrawCopy("same");
    else
      if (i==0) hFake[i]->Draw("");
      else hFake[i]->Draw("same");
    
  }
  if (maxY>0) 
    p12->SetLogy(); //  c1->cd(1)->cd(2)->SetLogy();

  
  layerLgnd->Draw();
      
}


void decorateHist(TH1* sHist, int colorCode, HistType itype) {
  sHist->GetXaxis()->SetLabelSize(.06);
  sHist->SetLineColor(colorCode);
  sHist->SetMarkerColor(colorCode);
  //  sHist->SetFillColor(colorCode);
  //  sHist->SetMarkerStyle(markerStyle);
  if (itype == Fake) {
    sHist->SetMinimum(0.0001);
    sHist->SetTitle("");
    sHist->GetYaxis()->SetLabelSize(.08);
    sHist->GetXaxis()->SetLabelSize(.08);
    sHist->GetXaxis()->SetTitleSize(.08);
    sHist->GetXaxis()->SetTitleOffset(-5.);
  } else if (itype == Eff) {
    sHist->SetMinimum(0.95);
    sHist->GetYaxis()->SetNdivisions(505);
    sHist->GetYaxis()->SetLabelSize(.1);
    sHist->GetXaxis()->SetTitleSize(.1);
    sHist->GetXaxis()->SetTitleOffset(-5.);
    //  } else if (itype == Noise || itype == QpSignal) {
  } else {
    sHist->SetMinimum(0.95);
    sHist->GetYaxis()->SetNdivisions(505);
    sHist->GetYaxis()->SetLabelSize(.06);
    sHist->GetXaxis()->SetLabelSize(.05);
    sHist->GetXaxis()->SetTitleSize(.06);
    sHist->GetXaxis()->SetTitleOffset(-6.);
    if (itype ==QpSignal) {
      sHist->GetXaxis()->SetNdivisions(505);
      sHist->Rebin(4);
    }

    if (itype ==QpFake) {
      sHist->SetMinimum(0.1);
    }    
    //      sHist->
  }
  sHist->SetTitleSize(1,"");
  
  sHist->SetStats(0);
}

 int getIdxToSector(int ichm) {// A wheel : 0-15  C wheel 16-31
   if (ichm>=0 && ichm <16) return ichm+1;
   else if (ichm >= 16 && ichm <32) return -1*(ichm-15);
   else return -99;
 } 
