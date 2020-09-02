/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TFile.h"
#include "TH1F.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TGraphErrors.h"
#include "TText.h"
#include "TLegend.h"
#include "TF1.h"
#include "TChain.h"
#include "TH2F.h"
#include "TProfile.h"

#include <vector>
#include <iostream>
#include <dirent.h>
#include <sstream>
#include <stdlib.h>
#include <cmath>

void getCorrections();

double** corrX;
double** corrY;

int main()
{

  gStyle->SetPalette(1,0);

  getCorrections();

  TChain* tree=new TChain("unbiasedMuonResiduals");
    
  std::string dir="/tmp/roberth/user10.RobertHarrington.data10_7TeV.allRuns.physics_MuonswBeam.merge.DESD_SGLMU.r1299_p165.UnbiasedRes.iPat.V22";
  
  tree->Add((dir+"/*root").c_str());
  
  int nPixHits;
  int nSCTHits;
  int nTRTHits;
  int nCSCHits;
  int nMDTHits;
  
  int author;

  int hIndex;
  int phiSector;
  int isEndcap;
  int identifier;
  int isCSide;
  
  int detType;

  int stName;
  int stPhi;
  int stEta;
  
  double momentum;
  double charge;
  double d0;
  double z0;
  double phi0;
  double theta;
  double eta;
  double calEnergyLoss;
  double calEnergyLossErr;
  double materialOnTrack;

  double dR;
  double dZlocal;
  double dXlocal;
  double dYlocal;
  double dXlocalAngle;
  double dYlocalAngle;

  double dXglobal;
  double dYglobal;
  double dZglobal;
  double dXglobalAngle;
  double dYglobalAngle;
  double dZglobalAngle;

  tree->SetBranchAddress("nPixHits",     &nPixHits);
  tree->SetBranchAddress("nSCTHits",     &nSCTHits);
  tree->SetBranchAddress("nTRTHits",     &nTRTHits);
  tree->SetBranchAddress("nCSCHits",     &nCSCHits);
  tree->SetBranchAddress("nMDTHits",     &nMDTHits);
  
  tree->SetBranchAddress("author",       &author);

  tree->SetBranchAddress("hIndex",       &hIndex);
  tree->SetBranchAddress("phiSector",    &phiSector);
  tree->SetBranchAddress("isEndcap",     &isEndcap);
  tree->SetBranchAddress("identifier",   &identifier);
  tree->SetBranchAddress("isCSide",      &isCSide);
  tree->SetBranchAddress("detType",      &detType);

  tree->SetBranchAddress("stName",       &stName);
  tree->SetBranchAddress("stPhi",        &stPhi);
  tree->SetBranchAddress("stEta",        &stEta);
  
  tree->SetBranchAddress("momentum",     &momentum);
  tree->SetBranchAddress("charge",       &charge);
  tree->SetBranchAddress("d0",           &d0);
  tree->SetBranchAddress("z0",           &z0);
  tree->SetBranchAddress("phi0",         &phi0);
  tree->SetBranchAddress("theta",        &theta);
  tree->SetBranchAddress("eta",          &eta);

  tree->SetBranchAddress("calEnergyLoss",    &calEnergyLoss);
  tree->SetBranchAddress("calEnergyLossErr", &calEnergyLossErr);
  tree->SetBranchAddress("materialOnTrack",  &materialOnTrack);

  tree->SetBranchAddress("dR",           &dR);
  tree->SetBranchAddress("dZlocal",      &dZlocal);     
  tree->SetBranchAddress("dXlocal",      &dXlocal);    
  tree->SetBranchAddress("dYlocal",      &dYlocal);    
  tree->SetBranchAddress("dXlocalAngle", &dXlocalAngle);
  tree->SetBranchAddress("dYlocalAngle", &dYlocalAngle);
  tree->SetBranchAddress("dXglobal",     &dXglobal);
  tree->SetBranchAddress("dYglobal",     &dYglobal);   
  tree->SetBranchAddress("dZglobal",     &dZglobal);   

  int itrk=0;
  int icharge=0;
  int isideAC=0;

  int NVAR=4;
  std::string histnames[NVAR]={"Matl",
			  "CaloEnergy",
			  "CaloEnergyErr"
			  "Momentum"};
  std::string histtitles[NVAR]={"Material On Track",
			   "Calorimeter Energy Loss",
			   "Calorimeter Energy Loss Error",
			   "Momentum"};
  int histnbins    [NVAR]   ={         30,           30,          30,     35};
  double histlimits[NVAR][2]={{140.,260.},{3200.,4200.},{400.,2000.},{0.,70.}};

  std::string varnames[NVAR]={"materialOnTrack"};
  const int NBEC=2;
  const int NPHI=4;
  TH2F**** yVVar = new TH2F***[NVAR];
  for (int ivar=0;ivar<NVAR;ivar++) {
    yVVar[ivar]=new TH2F***[NBEC];
    for (int ibec=0;ibec<NBEC;ibec++) {
      yVVar[ivar][ibec]=new TH2F**[NPHI];
      for (int iphi=0;iphi<NPHI;iphi++) {
        std::stringstream phiss; phiss<<iphi+1;
        std::string histname=histnames[ivar]+becStr[ibec]+phiss.str();
        std::string histtitle=histtitles[ivar]+"_"
        yVVAr[ivar][ibec][iphi]=new TH2F(histname.c_str(),histtitles.c_str(),histnbins[ivar],histlimits[ivar][0],histlimits[ivar][1]);
      }
    }
  }

  TH2F* yVMatlA=new TH2F("yVMatlA","yVMatlA",30,140.,260.,100,-50.,50.);
  TH2F* yVCaloEnergyA    =new TH2F("yVCaloEnergyA",   "yVCaloEnergyA",   
				  30,3200.,4200.,100,-50.,50.);
  TH2F* yVCaloEnergyErrA =new TH2F("yVCaloEnergyErrA","yVCaloEnergyErrA",
				  30, 400.,2000.,100,-50.,50.);
  TH2F* yVMomentumA =new TH2F("yVMomentumA","yVMomentumA",
				  35, 0.,70.,100,-50.,50.);
  TH1F* hMomPosA = new TH1F("hMomPosA", "MomentumPosChargeA",35,0.,70.);
  TH1F* hMomNegA = new TH1F("hMomNegA", "MomentumNegChargeA",35,0.,70.);

  TH2F* yVMatlC=new TH2F("yVMatlC","yVMatlC",100,140.,260.,100,-50.,50.);
  TH2F* yVCaloEnergyC    =new TH2F("yVCaloEnergyC",   "yVCaloEnergyC",   
				  30,3200.,4200.,100,-50.,50.);
  TH2F* yVCaloEnergyErrC =new TH2F("yVCaloEnergyErrC","yVCaloEnergyErrC",
				  30, 400.,2000.,100,-50.,50.);
  TH2F* yVMomentumC =new TH2F("yVMomentumC","yVMomentumC",
				  30, 0.,70.,100,-50.,50.);
  TH1F* hMomPosC = new TH1F("hMomPosC", "MomentumPosChargeC",35,0.,70.);
  TH1F* hMomNegC = new TH1F("hMomNegC", "MomentumNegChargeC",35,0.,70.);

  int nentries=tree->GetEntries();
  for (int ientry=0;ientry<nentries;ientry++) {
    tree->GetEntry(ientry);
    if (nSCTHits<6||nCSCHits<3) continue;
    if (detType!=1) continue;
    if (hIndex!=0) continue;
    if (isCSide==0) {
      double corr=(stPhi%2)?6.271:-0.238;
      corr += corrY[0][stPhi-1];
      double dY=dYlocal+corr;
      yVMatlA->Fill(materialOnTrack,dY);
      yVCaloEnergyA->Fill(calEnergyLoss,dY);
      yVCaloEnergyErrA->Fill(calEnergyLossErr,dY);
      yVMomentumA->Fill(momentum,dY);
      if (charge>0.) hMomPosA->Fill(momentum);
      else           hMomNegA->Fill(momentum);
    }
    else {
      double corr=(stPhi%2)?6.271:-0.238;
      corr += corrY[1][stPhi-1];
      double dY=dYlocal+corr;
      yVMatlC->Fill(materialOnTrack,dY);
      yVCaloEnergyC->Fill(calEnergyLoss,dY);
      yVCaloEnergyErrC->Fill(calEnergyLossErr,dY);
      yVMomentumC->Fill(momentum,dY);
      if (charge>0.) hMomPosC->Fill(momentum);
      else           hMomNegC->Fill(momentum);

    }
  }

  TCanvas* canv=new TCanvas("canv","canv");
  canv->SetFillColor(10);

  // A-side
  yVMatlA->GetXaxis()->SetTitle("material on track (X0)");
  yVMatlA->GetYaxis()->SetTitle("#Delta y_{local}");
  yVMatlA->Draw("COLZ");
  
  canv->Print("yVMatl_A.eps");

  canv->Clear(); canv->SetFillColor(10);
  yVMatlA->ProfileX()->Draw();
  canv->Print("yVMatlProfX_A.eps");
  
  canv->Clear(); canv->SetFillColor(10);
  yVCaloEnergyA->GetXaxis()->SetTitle("energy loss in calorimeter (MeV)");
  yVCaloEnergyA->GetYaxis()->SetTitle("#Delta y_{local}");
  yVCaloEnergyA->Draw("COLZ");
  
  canv->Print("yVCaloEnergy_A.eps");

  canv->Clear(); canv->SetFillColor(10);
  yVCaloEnergyA->ProfileX()->Draw();
  canv->Print("yVCaloEnergyProfX_A.eps");
  
  canv->Clear(); canv->SetFillColor(10);
  yVCaloEnergyErrA->GetXaxis()->SetTitle("error on energy loss in calorimeter (MeV)");
  yVCaloEnergyErrA->GetYaxis()->SetTitle("#Delta y_{local}");
  yVCaloEnergyErrA->Draw("COLZ");
  
  canv->Clear(); canv->SetFillColor(10);
  canv->Print("yVCaloEnergyErr_A.eps");

  canv->Clear(); canv->SetFillColor(10);
  yVCaloEnergyErrA->ProfileX()->Draw();

  canv->Print("yVCaloEnergyErrProfX_A.eps");
  
  canv->Clear(); canv->SetFillColor(10);
  yVMomentumA->GetXaxis()->SetTitle("momentum (GeV)");
  yVMomentumA->GetYaxis()->SetTitle("#Delta y_{local}");
  yVMomentumA->Draw("COLZ");
  
  canv->Print("yVMomentum_A.eps");

  canv->Clear(); canv->SetFillColor(10);
  yVMomentumA->ProfileX()->Draw();
  canv->Print("yVMomentumProfX_A.eps");
  
  

  // C-side
  canv->Clear(); canv->SetFillColor(10);
  yVMatlC->GetXaxis()->SetTitle("material on track (X0)");
  yVMatlC->GetYaxis()->SetTitle("#Delta y_{local}");
  yVMatlC->Draw("COLZ");
  
  canv->Print("yVMatl_C.eps");

  canv->Clear(); canv->SetFillColor(10);
  yVMatlC->ProfileX()->Draw();
  canv->Print("yVMatlProfX_C.eps");
  
  canv->Clear(); canv->SetFillColor(10);
  yVCaloEnergyC->GetXaxis()->SetTitle("energy loss in calorimeter (MeV)");
  yVCaloEnergyC->GetYaxis()->SetTitle("#Delta y_{local}");
  yVCaloEnergyC->Draw("COLZ");
  
  canv->Print("yVCaloEnergy_C.eps");

  canv->Clear(); canv->SetFillColor(10);
  yVCaloEnergyC->ProfileX()->Draw();
  canv->Print("yVCaloEnergyProfX_C.eps");
  
  canv->Clear(); canv->SetFillColor(10);
  yVCaloEnergyErrC->GetXaxis()->SetTitle("error on energy loss in calorimeter (MeV)");
  yVCaloEnergyErrC->GetYaxis()->SetTitle("#Delta y_{local}");
  yVCaloEnergyErrC->Draw("COLZ");
  
  canv->Print("yVCaloEnergyErr_AC.eps");

  canv->Clear(); canv->SetFillColor(10);
  yVCaloEnergyErrC->ProfileX()->Draw();
  canv->Print("yVCaloEnergyErrProfX_C.eps");
  
  canv->Clear(); canv->SetFillColor(10);
  yVMomentumC->GetXaxis()->SetTitle("momentum (GeV)");
  yVMomentumC->GetYaxis()->SetTitle("#Delta y_{local}");
  yVMomentumC->Draw("COLZ");
  
  canv->Print("yVMomentum_C.eps");

  canv->Clear(); canv->SetFillColor(10);
  yVMomentumC->ProfileX()->Draw();
  canv->Print("yVMomentumProfX_C.eps");

  // momentum plots
  canv->Clear(); canv->SetFillColor(10);
  gStyle->SetOptStat(0);
  hMomPosA->SetLineColor(kBlue);
  hMomNegA->SetLineColor(kRed);
  double max=0.;
  if (hMomPosA->GetMaximum()>max) max=hMomPosA->GetMaximum();
  if (hMomNegA->GetMaximum()>max) max=hMomNegA->GetMaximum();
  hMomPosA->SetMaximum(max);  
  hMomPosA->SetTitle("Momentum for Positive Muons, A-Side");
  hMomNegA->SetTitle("Momentum for Negative Muons, A-Side");
  hMomPosA->GetXaxis()->SetTitle("momentum (GeV)");
  hMomNegA->GetXaxis()->SetTitle("momentum (GeV)");
  hMomPosA->Draw();
  hMomNegA->Draw("same");

  TLegend* legA=new TLegend(.75,.6,.99,.95);
  legA->AddEntry(hMomPosC,"positive charge","L");
  legA->AddEntry(hMomNegC,"negative charge","L");
  legA->Draw();  
  canv->Print("momentum_A.eps");

  canv->Clear(); canv->SetFillColor(10);
  hMomPosC->SetLineColor(kBlue);
  hMomNegC->SetLineColor(kRed);
  max=0.;
  if (hMomPosC->GetMaximum()>max) max=hMomPosC->GetMaximum();
  if (hMomNegC->GetMaximum()>max) max=hMomNegC->GetMaximum();
  hMomPosC->SetMaximum(max);  
  hMomPosC->SetTitle("Momentum for Positive Muons, C-Side");
  hMomNegC->SetTitle("Momentum for Negative Muons, C-Side");
  hMomPosC->GetXaxis()->SetTitle("muon momentum (GeV)");
  hMomNegC->GetXaxis()->SetTitle("muon momentum (GeV)");
  hMomPosC->Draw();
  hMomNegC->Draw("same");

  TLegend* leg=new TLegend(.75,.6,.99,.95);
  leg->AddEntry(hMomPosC,"positive charge","L");
  leg->AddEntry(hMomNegC,"negative charge","L");
  leg->Draw();
  canv->Print("momentum_C.eps");

  delete canv;
}

//________________________________________________________________________
void getCorrections()
{
  corrX=new double*[2];
  corrY=new double*[2];
  for (int isideAC=0;isideAC<2;isideAC++) {
    corrX[isideAC]=new double[16];
    corrY[isideAC]=new double[16];
  }
  
  corrX[1][0] =-3.461; corrY[1][0] =-4.018;
  corrX[1][1] =-6.423; corrY[1][1] = 0.175;
  corrX[1][2] =-1.489; corrY[1][2] =-4.203;
  corrX[1][3] = 5.017; corrY[1][3] = 1.556;
  corrX[1][4] = 0.399; corrY[1][4] =-5.492;
  corrX[1][5] = 2.055; corrY[1][5] = 2.617;
  corrX[1][6] =-2.488; corrY[1][6] =-8.192;
  corrX[1][7] = 0.371; corrY[1][7] = 1.229;
  corrX[1][8] = 9.458; corrY[1][8] =-0.195;
  corrX[1][9] =-1.313; corrY[1][9] = 8.063;
  corrX[1][10]= 2.331; corrY[1][10]= 2.441;
  corrX[1][11]= 5.643; corrY[1][11]= 3.621;
  corrX[1][12]= 3.188; corrY[1][12]=-0.424;
  corrX[1][13]=-1.345; corrY[1][13]= 1.085;
  corrX[1][14]= 0.514; corrY[1][14]=-1.253;
  corrX[1][15]=-5.703; corrY[1][15]=-1.890;
  
  corrX[0][0] = 8.539; corrY[0][0] =-0.816;
  corrX[0][1] = 9.112; corrY[0][1] =-3.194;
  corrX[0][2] = 1.130; corrY[0][2] =-5.806;
  corrX[0][3] = 0.036; corrY[0][3] =-7.465;
  corrX[0][4] =-3.183; corrY[0][4] =-5.682;
  corrX[0][5] = 1.104; corrY[0][5] =-3.657;
  corrX[0][6] =-6.310; corrY[0][6] =-4.543;
  corrX[0][7] = 1.029; corrY[0][7] =-0.199;
  corrX[0][8] =-6.905; corrY[0][8] = 0.151;
  corrX[0][9] =-0.549; corrY[0][9] = 0.457;
  corrX[0][10]=-7.236; corrY[0][10]= 0.722;
  corrX[0][11]= 0.905; corrY[0][11]= 2.924;
  corrX[0][12]=-1.077; corrY[0][12]= 2.528;
  corrX[0][13]= 7.075; corrY[0][13]= 3.093;
  corrX[0][14]= 6.653; corrY[0][14]= 1.052;
  corrX[0][15]= 6.904; corrY[0][15]= 0.666;

  return;
}
