#include "TCanvas.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"
#include <iostream>
#include <string>
void JetTagging_test()
{
//---------------------insert HIST filename here---------------------------
string HISTfile = "/tmp/mbarison/data11_7TeV.00186493.physics_JetTauEtmiss.merge.HIST.f393_m926/data11_7TeV.00186493.physics_JetTauEtmiss.merge.HIST.f393_m926._0001.1";
//---------------------------------------------------------------------------
  gStyle->SetPalette(1);
 using std;
 double P[16];
 double Summ[16];
 double Summ1[16];
 double Summ2[16];
 double Max[16];
 double Max1[16];
 double Max2[16];
 string MonFile = "root://castoratlas//castor/cern.ch/grid/atlas/caf/atlcal/perm/dataqual/han_references/Collisions/data11_7TeV.00183347.express_express.merge.HIST.f384_m875._0001.1.root";

   
 string run;
 TFile* BTagMonteCarlo= TFile::Open(MonFile.c_str());
 TFile* BTagExp= TFile::Open(HISTfile.c_str());

 size_t a;
 a=HISTfile.find(".00");
 run=HISTfile.substr(a+3,6);

 printf ("================== Analysing Run %s ==================\n", run.c_str());

 BTagMonteCarlo->cd();
 TH1F* m_jet_electrons_n = (TH1F*)BTagMonteCarlo->Get("run_183347/JetTagging/jet_electrons_n");
 TH1F* m_jet_muons_n = (TH1F*)BTagMonteCarlo->Get("run_183347/JetTagging/jet_muons_n");
  TH1F* m_jet_tracks_hits_BLayer = (TH1F*)BTagMonteCarlo->Get("run_183347/JetTagging/jet_tracks_hits_BLayer");
  TH1F* m_jet_tracks_hits_Pixel = (TH1F*)BTagMonteCarlo->Get("run_183347/JetTagging/jet_tracks_hits_Pixel");
  TH1F* m_jet_tracks_hits_SCT = (TH1F*)BTagMonteCarlo->Get("run_183347/JetTagging/jet_tracks_hits_SCT");
  TH1F* m_jet_tracks_n = (TH1F*)BTagMonteCarlo->Get("run_183347/JetTagging/jet_tracks_n");
  TH1F* m_jetProb = (TH1F*)BTagMonteCarlo->Get("run_183347/JetTagging/tag_JetProb_w");
  TH1F* m_sv0 = (TH1F*)BTagMonteCarlo->Get("run_183347/JetTagging/tag_SV0_w");
  TH1F* m_tag_COMB_w = (TH1F*)BTagMonteCarlo->Get("run_183347/JetTagging/tag_COMB_w");
  TH1F* m_tag_IP1D_w = (TH1F*)BTagMonteCarlo->Get("run_183347/JetTagging/tag_IP1D_w");
  TH1F* m_tag_IP2D_w = (TH1F*)BTagMonteCarlo->Get("run_183347/JetTagging/tag_IP2D_w");
  TH1F* m_tag_IP3D_w = (TH1F*)BTagMonteCarlo->Get("run_183347/JetTagging/tag_IP3D_w");
  TH1F* m_trkCnt = (TH1F*)BTagMonteCarlo->Get("run_183347/JetTagging/tag_TrkCnt_w");  
  TH1F* m_SOFTE = (TH1F*)BTagMonteCarlo->Get("run_183347/JetTagging/tag_SOFTE_w");
  TH1F* m_SOFTM = (TH1F*)BTagMonteCarlo->Get("run_183347/JetTagging/tag_SOFTM_w");
  TH2F* m_trackSelEff = (TH2F*)BTagMonteCarlo->Get("run_183347/JetTagging/track_selector_eff");
  TH2F* m_trackSelAll = (TH2F*)BTagMonteCarlo->Get("run_183347/JetTagging/track_selector_all");
 
  BTagExp->cd();
  TH1F* ex_jet_electrons_n = (TH1F*)BTagExp->Get(TString("run_")+run.c_str()+TString("/JetTagging/jet_electrons_n"));
  TH1F* ex_jet_muons_n = (TH1F*)BTagExp->Get(TString("run_")+run.c_str()+TString("/JetTagging/jet_muons_n"));
  TH1F* ex_jet_tracks_hits_BLayer = (TH1F*)BTagExp->Get(TString("run_")+run.c_str()+TString("/JetTagging/jet_tracks_hits_BLayer"));
  TH1F* ex_jet_tracks_hits_Pixel = (TH1F*)BTagExp->Get(TString("run_")+run.c_str()+TString("/JetTagging/jet_tracks_hits_Pixel"));
  TH1F* ex_jet_tracks_hits_SCT = (TH1F*)BTagExp->Get(TString("run_")+run.c_str()+TString("/JetTagging/jet_tracks_hits_SCT"));
  TH1F* ex_jet_tracks_n = (TH1F*)BTagExp->Get(TString("run_")+run.c_str()+TString("/JetTagging/jet_tracks_n"));
  TH1F* ex_jetProb = (TH1F*)BTagExp->Get(TString("run_")+run.c_str()+TString("/JetTagging/tag_JetProb_w"));
  TH1F* ex_sv0 = (TH1F*)BTagExp->Get(TString("run_")+run.c_str()+TString("/JetTagging/tag_SV0_w"));
  TH1F* ex_tag_COMB_w = (TH1F*)BTagExp->Get(TString("run_")+run.c_str()+TString("/JetTagging/tag_COMB_w"));
  TH1F* ex_tag_IP1D_w = (TH1F*)BTagExp->Get(TString("run_")+run.c_str()+TString("/JetTagging/tag_IP1D_w"));
  TH1F* ex_tag_IP2D_w = (TH1F*)BTagExp->Get(TString("run_")+run.c_str()+TString("/JetTagging/tag_IP2D_w"));
  TH1F* ex_tag_IP3D_w = (TH1F*)BTagExp->Get(TString("run_")+run.c_str()+TString("/JetTagging/tag_IP3D_w"));
  TH1F* ex_trkCnt = (TH1F*)BTagExp->Get(TString("run_")+run.c_str()+TString("/JetTagging/tag_TrkCnt_w"));  
  TH1F* ex_SOFTE = (TH1F*)BTagExp->Get(TString("run_")+run.c_str()+TString("/JetTagging/tag_SOFTE_w"));
  TH1F* ex_SOFTM = (TH1F*)BTagExp->Get(TString("run_")+run.c_str()+TString("/JetTagging/tag_SOFTM_w"));
  TH2F* ex_trackSelEff = (TH2F*)BTagExp->Get(TString("run_")+run.c_str()+TString("/JetTagging/track_selector_eff"));
  TH2F* ex_trackSelAll = (TH2F*)BTagExp->Get(TString("run_")+run.c_str()+TString("/JetTagging/track_selector_all"));
 
if((m_jet_electrons_n->Integral() != 0)&&(ex_jet_electrons_n->Integral()!=0))  
  {
    m_jet_electrons_n->Scale(1./m_jet_electrons_n->Integral());
    ex_jet_electrons_n->Scale(1./ex_jet_electrons_n->Integral());
  } 
  m_jet_electrons_n ->SetLineColor(kRed);
  ex_jet_electrons_n->SetLineColor(kBlue);

if((m_jet_muons_n->Integral() != 0)&&(ex_jet_muons_n->Integral()!=0))  
  {
  m_jet_muons_n->Scale(1./m_jet_muons_n->Integral());
  ex_jet_muons_n->Scale(1./ex_jet_muons_n->Integral());
  }
  m_jet_muons_n->SetLineColor(kRed);
  ex_jet_muons_n->SetLineColor(kBlue);

if((m_jet_tracks_hits_BLayer->Integral() != 0)&&(ex_jet_tracks_hits_BLayer->Integral()!=0))  
  {
  m_jet_tracks_hits_BLayer->Scale(1./m_jet_tracks_hits_BLayer->Integral());
  ex_jet_tracks_hits_BLayer->Scale(1./ex_jet_tracks_hits_BLayer->Integral());
  }
  m_jet_tracks_hits_BLayer->SetLineColor(kRed);
  ex_jet_tracks_hits_BLayer->SetLineColor(kBlue);

if((m_jet_tracks_hits_Pixel->Integral() != 0)&&(ex_jet_tracks_hits_Pixel->Integral()!=0))  
  {
  m_jet_tracks_hits_Pixel->Scale(1./m_jet_tracks_hits_Pixel->Integral());
  ex_jet_tracks_hits_Pixel->Scale(1./ex_jet_tracks_hits_Pixel->Integral());
  }
  m_jet_tracks_hits_Pixel->SetLineColor(kRed);
  ex_jet_tracks_hits_Pixel->SetLineColor(kBlue);

if((m_jet_tracks_hits_SCT->Integral() != 0)&&(ex_jet_tracks_hits_SCT->Integral()!=0))  
  {
  m_jet_tracks_hits_SCT->Scale(1./m_jet_tracks_hits_SCT->Integral());
  ex_jet_tracks_hits_SCT->Scale(1./ex_jet_tracks_hits_SCT->Integral());
  }
  m_jet_tracks_hits_SCT->SetLineColor(kRed);
  ex_jet_tracks_hits_SCT->SetLineColor(kBlue);

if((m_jet_tracks_n->Integral() != 0)&&(ex_jet_tracks_n->Integral()!=0))  
  {
  m_jet_tracks_n->Scale(1./m_jet_tracks_n->Integral());
  ex_jet_tracks_n->Scale(1./ex_jet_tracks_n->Integral());
  }
  m_jet_tracks_n->SetLineColor(kRed);
  ex_jet_tracks_n->SetLineColor(kBlue);

if((m_jetProb->Integral() != 0)&&(ex_jetProb->Integral()!=0))  
  { 
  m_jetProb->Scale(1./m_jetProb->Integral());
  ex_jetProb->Scale(1./ex_jetProb->Integral());
  }
  m_jetProb->SetLineColor(kRed);
  ex_jetProb->SetLineColor(kBlue);

if((m_sv0->Integral() != 0)&&(ex_sv0->Integral()!=0))  
  {
  m_sv0->Scale(1./m_sv0->Integral());
  ex_sv0->Scale(1./ex_sv0->Integral());
  }
  m_sv0->SetLineColor(kRed);
  ex_sv0->SetLineColor(kBlue);

if((m_tag_COMB_w->Integral() != 0)&&(ex_tag_COMB_w->Integral()!=0))
  {
  m_tag_COMB_w->Scale(1./m_tag_COMB_w->Integral());
  ex_tag_COMB_w->Scale(1./ex_tag_COMB_w->Integral());
  }
  m_tag_COMB_w->SetLineColor(kRed);
  ex_tag_COMB_w->SetLineColor(kBlue);

if((m_tag_IP1D_w->Integral() != 0)&&(ex_tag_IP1D_w->Integral()!=0))
  {
  m_tag_IP1D_w->Scale(1./m_tag_IP1D_w->Integral());
  ex_tag_IP1D_w->Scale(1./ex_tag_IP1D_w->Integral());
  }
  m_tag_IP1D_w->SetLineColor(kRed);
  ex_tag_IP1D_w->SetLineColor(kBlue);

if((m_tag_IP2D_w->Integral() != 0)&&(ex_tag_IP2D_w->Integral()!=0))
  {
  m_tag_IP2D_w->Scale(1./m_tag_IP2D_w->Integral());
  ex_tag_IP2D_w->Scale(1./ex_tag_IP2D_w->Integral());
  }
  m_tag_IP2D_w->SetLineColor(kRed);
  ex_tag_IP2D_w->SetLineColor(kBlue);

if((m_tag_IP3D_w->Integral() != 0)&&(ex_tag_IP3D_w->Integral()!=0))
  {
  m_tag_IP3D_w->Scale(1./m_tag_IP3D_w->Integral());
  ex_tag_IP3D_w->Scale(1./ex_tag_IP3D_w->Integral());
  }
  m_tag_IP3D_w->SetLineColor(kRed);
  ex_tag_IP3D_w->SetLineColor(kBlue);

if((m_trkCnt->Integral() != 0)&&(ex_trkCnt->Integral()!=0))  
  {
  m_trkCnt->Scale(1./m_trkCnt->Integral());
  ex_trkCnt->Scale(1./ex_trkCnt->Integral());
  }
  m_trkCnt->SetLineColor(kRed);
  ex_trkCnt->SetLineColor(kBlue);

if((m_SOFTE->Integral() != 0)&&(ex_SOFTE->Integral()!=0))  
  {
  m_SOFTE->Scale(1./m_SOFTE->Integral());
  ex_SOFTE->Scale(1./ex_SOFTE->Integral());
  }
  m_SOFTE->SetLineColor(kRed);
  ex_SOFTE->SetLineColor(kBlue);

if((m_SOFTM->Integral() != 0)&&(ex_SOFTM->Integral()!=0))  
  {
  m_SOFTM->Scale(1./m_SOFTM->Integral());
  ex_SOFTM->Scale(1./ex_SOFTM->Integral());
  }
  m_SOFTM->SetLineColor(kRed);
  ex_SOFTM->SetLineColor(kBlue);

  //if((m_trackSelEff->Integral() != 0)&&(ex_trackSelEff->Integral()!=0))
  // {
  ex_trackSelEff->Divide(ex_trackSelAll);
    //ex_trackSelEff->Scale(1./(ex_trackSelAll->Integral()));
    //  }
  //  m_trackSelEff->SetLineColor(kRed);
    //ex_trackSelEff->SetLineColor(kBlue);


  Max1[0]=m_jet_electrons_n->GetMaximum();
  Max2[0]=ex_jet_electrons_n->GetMaximum();
  Max1[1]=m_jet_muons_n->GetMaximum();
  Max2[1]=ex_jet_muons_n->GetMaximum();
  Max1[2]=m_jet_tracks_hits_BLayer->GetMaximum();
  Max2[2]=ex_jet_tracks_hits_BLayer->GetMaximum();
  Max1[3]=m_jet_tracks_hits_Pixel->GetMaximum();
  Max2[3]=ex_jet_tracks_hits_Pixel->GetMaximum(); 
  Max1[4]=m_jet_tracks_hits_SCT->GetMaximum();
  Max2[4]=ex_jet_tracks_hits_SCT->GetMaximum();
  Max1[5]=m_jet_tracks_n->GetMaximum();
  Max2[5]=ex_jet_tracks_n->GetMaximum();
  Max1[6]=m_jetProb->GetMaximum();
  Max2[6]=ex_jetProb->GetMaximum(); 
  Max1[7]=m_sv0->GetMaximum();
  Max2[7]=ex_sv0->GetMaximum(); 
  Max1[8]=m_trkCnt->GetMaximum();
  Max2[8]=ex_trkCnt->GetMaximum();
  Max1[9]=m_SOFTE->GetMaximum();
  Max2[9]=ex_SOFTE->GetMaximum();
  Max1[10]=m_SOFTM->GetMaximum();
  Max2[10]=ex_SOFTM->GetMaximum();
  Max1[11]=m_trackSelEff->GetMaximum();
  Max2[11]=ex_trackSelEff->GetMaximum();
  Max1[12]=m_tag_COMB_w->GetMaximum();
  Max2[12]=ex_tag_COMB_w->GetMaximum();
  Max1[13]=m_tag_IP1D_w->GetMaximum();
  Max2[13]=ex_tag_IP1D_w->GetMaximum();
  Max1[14]=m_tag_IP2D_w->GetMaximum();
  Max2[14]=ex_tag_IP2D_w->GetMaximum();
  Max1[15]=m_tag_IP3D_w->GetMaximum();
  Max2[15]=ex_tag_IP3D_w->GetMaximum();
  Summ1[0]= m_jet_electrons_n->GetEntries();
  Summ2[0]= ex_jet_electrons_n->GetEntries();
  for(int i=0;i<14;i++)
    {
      if (Max2[i]>Max1[i])
	{ 
	  Max[i]=Max2[i];
	  Summ[i]=Summ2[i];
	}
      else 
	{
	  Max[i]=Max1[i];
	  Summ[i]=Summ1[i];
	}
    }
  P[0]=m_jet_electrons_n->KolmogorovTest(ex_jet_electrons_n,"M");  
  P[1]=m_jet_muons_n->KolmogorovTest(ex_jet_muons_n,"M");
  P[2]=m_jet_tracks_hits_BLayer->KolmogorovTest(ex_jet_tracks_hits_BLayer,"M");
  P[3]=m_jet_tracks_hits_Pixel->KolmogorovTest(ex_jet_tracks_hits_Pixel,"M");
  P[4]=m_jet_tracks_hits_SCT->KolmogorovTest(ex_jet_tracks_hits_SCT,"M");
  P[5]=m_jet_tracks_n->KolmogorovTest(ex_jet_tracks_n,"M");
  P[6]=m_jetProb->KolmogorovTest(ex_jetProb,"M");
  P[7]=m_sv0->KolmogorovTest(ex_sv0,"M");
  P[8]=m_trkCnt->KolmogorovTest(ex_trkCnt,"M");
  P[9]=m_SOFTE->KolmogorovTest(ex_SOFTE,"M");
  P[10]=m_SOFTM->KolmogorovTest(ex_SOFTM,"M");
  P[11]=m_trackSelEff->KolmogorovTest(ex_trackSelEff,"M");
  P[12]=m_tag_COMB_w->KolmogorovTest(ex_tag_COMB_w,"M");
  P[13]=m_tag_IP1D_w->KolmogorovTest(ex_tag_IP1D_w,"M");
  P[14]=m_tag_IP2D_w->KolmogorovTest(ex_tag_IP2D_w,"M");
  P[15]=m_tag_IP3D_w->KolmogorovTest(ex_tag_IP3D_w,"M");
  

  TCanvas* c1 = new TCanvas("c1","cuts",0,0,1200,1000);
  c1->Divide(4,4);

  c1->cd(1);  
  m_jet_electrons_n->Draw();
  ex_jet_electrons_n->Draw("same");
  if(P[0]<0.15)
    c1_1->SetFillColor(kGreen);
  else if (P[0]<0.3)
    c1_1->SetFillColor(kYellow);
  else
    c1_1->SetFillColor(kRed);
  gPad->SetFrameFillColor(10);
  m_jet_electrons_n -> SetAxisRange(0.01, Max[0]*1.2, "Y");
  leg1 = new TLegend(0.9,0.6,0.55,0.8);
  leg1->AddEntry(m_jet_electrons_n,"Reference","l");
  leg1->AddEntry(ex_jet_electrons_n,"Data","l");
  leg1->Draw();

  c1->cd(2);  
  m_jet_muons_n->Draw();
  ex_jet_muons_n->Draw("same");
  if(P[1]<0.15)
    c1_2->SetFillColor(kGreen);
  else if (P[1]<0.3)
    c1_2->SetFillColor(kYellow);
  else
    c1_2->SetFillColor(kRed);
  gPad->SetFrameFillColor(10);
  gPad->SetLogy();
  m_jet_muons_n -> SetAxisRange(1e-5, Max[1]*2, "Y");
  
  c1->cd(3);  
  m_jet_tracks_hits_BLayer->Draw();
  ex_jet_tracks_hits_BLayer->Draw("same");
  if(P[2]<0.15)
    c1_3->SetFillColor(kGreen);
  else if (P[2]<0.3)
    c1_3->SetFillColor(kYellow);
  else
    c1_3->SetFillColor(kRed);
  gPad->SetFrameFillColor(10);
  m_jet_tracks_hits_BLayer -> SetAxisRange(0, Max[2]*1.2, "Y");
  
  c1->cd(4);  
  m_jet_tracks_hits_Pixel->Draw();
  ex_jet_tracks_hits_Pixel->Draw("same");
  if(P[3]<0.15)
    c1_4->SetFillColor(kGreen);
  else if (P[3]<0.3)
    c1_4->SetFillColor(kYellow);
  else
    c1_4->SetFillColor(kRed);
  gPad->SetFrameFillColor(10);
  m_jet_tracks_hits_Pixel -> SetAxisRange(0, Max[3]*1.2, "Y");
  
  c1->cd(5);  
  m_jet_tracks_hits_SCT->Draw();
  ex_jet_tracks_hits_SCT->Draw("same");
  if(P[4]<0.15)
    c1_5->SetFillColor(kGreen);
  else if (P[4]<0.3)
    c1_5->SetFillColor(kYellow);
  else
    c1_5->SetFillColor(kRed);
  gPad->SetFrameFillColor(10);
  m_jet_tracks_hits_SCT -> SetAxisRange(0, Max[4]*1.2, "Y");
  
  c1->cd(6);  
  m_jet_tracks_n->Draw();
  ex_jet_tracks_n->Draw("same");
  if(P[5]<0.15)
    c1_6->SetFillColor(kGreen);
  else if (P[5]<0.3)
    c1_6->SetFillColor(kYellow);
  else
    c1_6->SetFillColor(kRed);
  gPad->SetFrameFillColor(10);
  m_jet_tracks_n -> SetAxisRange(0, Max[5]*1.2, "Y");
  
  c1->cd(7);   
  m_jetProb->Draw();
  ex_jetProb->Draw("same");
  if(P[6]<0.15)
    c1_7->SetFillColor(kGreen);
  else if (P[6]<0.3)
    c1_7->SetFillColor(kYellow);
  else
    c1_7->SetFillColor(kRed);
  gPad->SetFrameFillColor(10);
  gPad->SetLogy();
  m_jetProb -> SetAxisRange(1e-3, Max[6]*2, "Y");
  
  c1->cd(8);  
  m_sv0->Draw();
  ex_sv0->Draw("same");
  if(P[7]<0.01)
    c1_8->SetFillColor(kGreen);
  else if (P[7]<0.03)
    c1_8->SetFillColor(kYellow);
  else
    c1_8->SetFillColor(kRed);
  gPad->SetFrameFillColor(10);
  gPad->SetLogy();
  m_sv0 -> SetAxisRange(1e-5, Max[7]*2, "Y");
  
  c1->cd(9);  
  m_trkCnt->Draw();
  ex_trkCnt->Draw("same");
  if(P[8]<0.15)
    c1_9->SetFillColor(kGreen);
  else if (P[8]<0.3)
    c1_9->SetFillColor(kYellow);
  else
    c1_9->SetFillColor(kRed);
  gPad->SetFrameFillColor(10);
  gPad->SetLogy();
  m_trkCnt -> SetAxisRange(1e-3, Max[8]*2, "Y");

  c1->cd(10);  
  m_SOFTE->Draw();
  ex_SOFTE->Draw("same");
  if(P[9]<0.15)
    c1_10->SetFillColor(kGreen);
  else if (P[9]<0.3)
    c1_10->SetFillColor(kYellow);
  else
    c1_10->SetFillColor(kRed);
  gPad->SetFrameFillColor(10);
  m_SOFTE -> SetAxisRange(0, Max[9]*1.2, "Y");

  c1->cd(11);  
  m_SOFTM->Draw();
  ex_SOFTM->Draw("same");
  if(P[10]<0.15)
    c1_11->SetFillColor(kGreen);
  else if (P[10]<0.3)
    c1_11->SetFillColor(kYellow);
  else
    c1_11->SetFillColor(kRed);
  gPad->SetFrameFillColor(10);
  m_SOFTM -> SetAxisRange(0, Max[10]*1.2, "Y");

  c1->cd(12);
 // m_trackSelEff->Draw("colz");
  ex_trackSelEff->Draw("colz");
  if(P[11]<0.15)
    c1_12->SetFillColor(kGreen);
  else if (P[11]<0.3)
    c1_12->SetFillColor(kYellow);
  else
    c1_12->SetFillColor(kRed);
  gPad->SetFrameFillColor(10);
  m_trackSelEff -> SetAxisRange(0, Max[11]*1.2, "Y");
  m_trackSelEff -> SetAxisRange(0, Max[11]*1.2, "X");
 

  c1->cd(13);
  m_tag_COMB_w->Draw();
  ex_tag_COMB_w->Draw("same");
  if(P[12]<0.15)
    c1_13->SetFillColor(kGreen);
  else if (P[11]<0.3)
    c1_13->SetFillColor(kYellow);
  else
    c1_13->SetFillColor(kRed);
  gPad->SetFrameFillColor(10);
  m_tag_COMB_w -> SetAxisRange(0, Max[12]*1.2, "Y");


  c1->cd(14);
  m_tag_IP1D_w->Draw();
  ex_tag_IP1D_w->Draw("same");
  if(P[13]<0.15)
    c1_14->SetFillColor(kGreen);
  else if (P[13]<0.3)
    c1_14->SetFillColor(kYellow);
  else
    c1_14->SetFillColor(kRed);
  gPad->SetFrameFillColor(10);
  m_tag_IP1D_w -> SetAxisRange(0, Max[13]*1.2, "Y");


  c1->cd(15);
  m_tag_IP2D_w->Draw();
  ex_tag_IP2D_w->Draw("same");
  if(P[14]<0.15)
    c1_15->SetFillColor(kGreen);
  else if (P[14]<0.3)
    c1_15->SetFillColor(kYellow);
  else
    c1_15->SetFillColor(kRed);
  gPad->SetFrameFillColor(10);
  m_tag_IP2D_w -> SetAxisRange(0, Max[14]*1.2, "Y");


  c1->cd(16);
  m_tag_IP3D_w->Draw();
  ex_tag_IP3D_w->Draw("same");
  if(P[15]<0.15)
    c1_16->SetFillColor(kGreen);
  else if (P[15]<0.3)
    c1_16->SetFillColor(kYellow);
  else
    c1_16->SetFillColor(kRed);
  gPad->SetFrameFillColor(10);
  m_tag_IP3D_w -> SetAxisRange(0, Max[15]*1.2, "Y");


  P[0]=m_jet_electrons_n->KolmogorovTest(ex_jet_electrons_n,"M");  
  P[1]=m_jet_muons_n->KolmogorovTest(ex_jet_muons_n,"M");
  P[2]=m_jet_tracks_hits_BLayer->KolmogorovTest(ex_jet_tracks_hits_BLayer,"M");
  P[3]=m_jet_tracks_hits_Pixel->KolmogorovTest(ex_jet_tracks_hits_Pixel,"M");
  P[4]=m_jet_tracks_hits_SCT->KolmogorovTest(ex_jet_tracks_hits_SCT,"M");
  P[5]=m_jet_tracks_n->KolmogorovTest(ex_jet_tracks_n,"M");
  P[6]=m_jetProb->KolmogorovTest(ex_jetProb,"M");
  P[7]=m_sv0->KolmogorovTest(ex_sv0,"M");
  P[8]=m_trkCnt->KolmogorovTest(ex_trkCnt,"M");
  P[9]=m_SOFTE->KolmogorovTest(ex_SOFTE,"M");
  P[10]=m_SOFTM->KolmogorovTest(ex_SOFTM,"M");
  P[11]=m_trackSelEff->KolmogorovTest(ex_trackSelEff,"M");
  P[12]=m_tag_COMB_w->KolmogorovTest(ex_tag_COMB_w,"M");
  P[13]=m_tag_IP1D_w->KolmogorovTest(ex_tag_IP1D_w,"M");
  P[14]=m_tag_IP2D_w->KolmogorovTest(ex_tag_IP2D_w,"M");
  P[15]=m_tag_IP3D_w->KolmogorovTest(ex_tag_IP3D_w,"M");
  cout<<"MaxDifference value of KolmogorovTest  for next variables:"<<endl;
  cout<<"jet_electrons_n: "<<P[0]<<endl;
  cout<<"jet_muons_n: "<<P[1]<<endl;
  cout<<"jet_tracks_hits_BLayer: "<<P[2]<<endl;
  cout<<"jet_tracks_hits_Pixel: "<<P[3]<<endl;
  cout<<"jet_tracks_hits_SCT: "<<P[4]<<endl;
  cout<<"jet_tracks_n: "<<P[5]<<endl;
  cout<<"jetProb: "<<P[6]<<endl;
  cout<<"sv0: "<<P[7]<<endl;
  cout<<"trkCnt: "<<P[8]<<endl;
  cout<<"SOFTE: "<<P[9]<<endl;
  cout<<"SOFTM: "<<P[10]<<endl;
  cout<<"track_selector_eff: "<<P[11]<<endl;
  cout<<"tag_COMB_w: "<<P[12]<<endl;
  cout<<"tag_IP1D_w: "<<P[13]<<endl;
  cout<<"tag_IP2D_w: "<<P[14]<<endl;
  cout<<"tag_IP3D_w: "<<P[15]<<endl;
  
  char name1[32];
  char name2[32];

  sprintf(name1,"dq_%s.pdf", run.c_str());
  sprintf(name2,"dq_%s.png", run.c_str());

  c1->Print(name1);
  c1->Print(name2);
  return ;
}

