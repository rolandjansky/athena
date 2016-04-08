/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
#include <iostream.h>
#include <iomanip.h>
//
#include <TH2F.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TF1.h>
#include <TF2.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TStyle.h>
//
// prototipus
double Vtx2D_fitfun(double *x, double *par);
double cdfsvt(double *x, double *par);
void Set_style();
//*************************************************************
void CDFFit(char nname[80], int level=3)
{
  Set_style();
  
  TFile *f0 = new TFile(nname);
  TProfile2D *HD;
  char nameh[80];
  char pngfile[20];

  // Els signes del valors que eixen ja estan canviats per a que siguen
  // els que hem de posar en la base de dades


  // 19/Setembre/2007 canviem un poc l'estrategia. Fins ara s'ajustava 
  // el histograma de 2D directament i els valors inicials eren tots 0 (cero)
  // La nova estrategia es ajustar primer el en 1D per a traure uns valors 
  // inicials de Xv i Yv i despres fer l'ajust del histograma en 2D on ja ajustem 
  // els quatre parametres: Xv, Yv, alpha i beta

  //---------------------------------------------------- 
  // ajust del histograma en 1D
  // traguem dos parametres: Xv i Yv
  // inicialitzem Xv i Yv amb 0 
  //----------------------------------------------------
  sprintf(nameh,"/trkana/Track_Parameters/hRecoD0_RecoPhi0_%d",level);
  TH2F *HDxy;
  HDxy = (TH2F*)f0->Get(nameh); 

  HDxy->GetYaxis()->SetAxisColor(2);
  HDxy->GetXaxis()->SetAxisColor(2);
  HDxy->GetYaxis()->SetLabelSize(0.03);
  HDxy->GetYaxis()->SetTitleSize(0.03);
  HDxy->GetYaxis()->SetTitleOffset(1.5);
  HDxy->GetXaxis()->SetLabelSize(0.03);
  HDxy->GetZaxis()->SetLabelSize(0.02);

  TCanvas *CanL1F = new TCanvas("CanL1F","D0 vs #phi0",10,20,500,500);
  CanL1F->SetGridx();
  CanL1F->SetGridy();

  HDxy->Draw("colz");

  // traguem el profile que es el que s'ajusta
  TProfile *HProfi = HDxy->ProfileX("",-1,-1,""); 
  HProfi->Draw("same");

  // definim la funcio a ajustar
  TF1 *Vtx2DFun = new TF1 ("Vtx2D", Vtx2D_fitfun, 0., 0., 2);
  Vtx2DFun->SetLineColor(1);
  Vtx2DFun->SetParameters(0.0, 0.0);  // ==> inicialitzem a 0
  int D0MaxBin = HProfi->GetMaximumBin();
  cout << "bin " << D0MaxBin << endl;

  Vtx2DFun->SetParName(0,"X_v");

  Vtx2DFun->SetParName(1,"Y_v");
  HProfi->Fit("Vtx2D","nW");
  Vtx2DFun->Draw("same");
  

  cout << " ------ First pass fit -------- " << endl;
  cout << " X_v = " << setw(8) <<   setprecision(3) << Vtx2DFun->GetParameter(0) << " mm " << endl;
  cout << " Y_v = " << setw(8) <<   setprecision(3) <<  Vtx2DFun->GetParameter(1) << " mm " << endl;
  cout << " ------ Going for second pass fit -------- " << endl;

  sprintf(pngfile,"Level1FitSel%d.png",level);
  CanL1F->Print("Level1Fit.eps");
  CanL1F->Print(pngfile);

  //---------------------------------------------------- 
  // ajust del histograma en 2D
  // traguem quatre parametres: Xv, Yv, alpha i beta
  // inicialitzem Xv i Yv amb els valors ajustats anteriorment
  //----------------------------------------------------
  sprintf(nameh,"trkana/Track_Parameters/hRecoD0_RecoPhi0_RecoZ0%d",level);
  HD = (TProfile2D*)f0->Get(nameh); 
  
  HD->GetYaxis()->SetAxisColor(2);
  HD->GetXaxis()->SetAxisColor(2);
  HD->GetZaxis()->SetAxisColor(2);
  HD->GetYaxis()->SetTitleSize(0.03);
  HD->GetYaxis()->SetTitleOffset(1.5);
  HD->GetXaxis()->SetLabelSize(0.03);
  HD->GetYaxis()->SetLabelSize(0.03);
  HD->GetZaxis()->SetLabelSize(0.02);
  HD->GetZaxis()->SetTitle("d0 (mm)");
  //HD->GetYaxis()->SetRangeUser(-150.,150.);

  // i mirem quantes entrades te cada canal
  for (int ix=1; ix<=HD->GetNbinsX(); ix++) {
    for (int iy=1; iy<=HD->GetNbinsY(); iy++) {
      //   cout << ix << " - " << iy << " = " << HD->GetBinContent(ix,iy) << " +- " << HD->GetBinError(ix,iy) << endl;
      if (HD->GetBinError(ix,iy)==0.) {
	//cout << "**" << ix << " - " << iy << " = " << HD->GetBinContent(ix,iy) << " +- " << HD->GetBinError(ix,iy) << endl;
	HD->SetBinContent(ix,iy,0.);
	HD->SetBinError(ix,iy,0.);
	//cout << "  " << ix << " - " << iy << " = " << HD->GetBinContent(ix,iy) << " +- " << HD->GetBinError(ix,iy) << endl;
      }
    }
  }


  // a pintar i ajustar
  TCanvas *CanCDF = new TCanvas("CanCDF","D0 vs #phi0-Z0",520,20,500,500);
  CanCDF->SetGridx();
  CanCDF->SetGridy();

  HD->Draw("colz");

  //TF2 *Ajust = new TF2("CDFSVT",cdfsvt, 0.,0.,-200,200,4);
  TF2 *Ajust = new TF2("CDFSVT",cdfsvt, 0.,0.,-100.,100.,4);
  double Xmin, Ymin, Xmax, Ymax;
  Ajust->GetRange(Xmin, Ymin, Xmax, Ymax);
  cout << " Xmin " << Xmin << "   Ymin " << Ymin << "   Xmax " << Xmax << "    Ymax " << Ymax << endl;

  // fiquem noms
  Ajust->SetParName(0,"X_v (mm)");
  Ajust->SetParName(1,"Y_v (mm)");
  Ajust->SetParName(2,"tan_a (mrad)");
  Ajust->SetParName(3,"tan_b (mrad)");

  // ==> inicialitzem Xv i Yv al resultat anterior, mentres que alpha i beta els tenim a 0
  Ajust->SetParameters(Vtx2DFun->GetParameter(0), Vtx2DFun->GetParameter(1), 0.0, 0.0);  

  //HD->Fit("CDFSVT","n");
  //HD->Fit("CDFSVT","nR");
  HD->Fit(Ajust,"nW");


  HD->Draw("colz");
  Ajust->SetLineColor(1);
  Ajust->SetLineWidth(1);
  Ajust->Draw("cont3 same");

  
  //HD->Draw("lego");
  Ajust->SetLineColor(1);
  Ajust->SetLineWidth(1);
  //Ajust->Draw("surf2 same");
  sprintf(pngfile,"CDFFitSel%d.png",level);
  CanCDF->Print("CDFFit.eps");
  CanCDF->Print(pngfile);

  //---------------------------------------------------- 
  // Ara anem a ajustar la Zv
  //----------------------------------------------------


  // primer pas
  // eta
  TFile *f0 = new TFile(nname);
  TH2F *HDeta, *HT;
  char nameh[80];
  sprintf(nameh,"trkana/Track_Parameters/hRecoZ0_RecoEta_%d",level);
  HDeta = (TH2F*)f0->Get(nameh); 
  
  HDeta->GetYaxis()->SetAxisColor(2);
  HDeta->GetXaxis()->SetAxisColor(2);
  HDeta->GetYaxis()->SetLabelSize(0.03);
  HDeta->GetYaxis()->SetTitleSize(0.03);
  HDeta->GetYaxis()->SetTitleOffset(1.5);
  HDeta->GetXaxis()->SetLabelSize(0.03);
  HDeta->GetZaxis()->SetLabelSize(0.02);

  TCanvas *CanZ = new TCanvas("Zv-eta","Z0 vs #eta",520,520,500,500);
  CanZ->SetGridx();
  CanZ->SetGridy();

  HDeta->Draw("colz");

  TProfile *HProfZ = HDeta->ProfileX("",-1,-1,""); 
  HProfZ->Draw("same");
  HProfZ->Fit("pol1","","",-0.9,0.9);
  //  HProfZ->Fit("pol0","","",-2.5,-1.7);
  //HProfZ->Fit("pol0","","",1.7,2.5);
  HDeta->Draw("col");
  HProfZ->Draw("same");
  HProfZ->GetFunction("pol1")->SetLineColor(4);
  sprintf(pngfile,"ZvFit_etaSel%d.png",level);
  CanZ->Print("ZvFit_eta.eps");
  CanZ->Print(pngfile);

  //---------------------------------------------------- 
  // Imprimim els valors finals
  //----------------------------------------------------


  cout << endl;
  cout << " ------ Final fit -------- (signes correctes per a la DB)" << endl;
  cout << " X_v   = " 
       << setw(8) << setprecision(3.3) << Ajust->GetParameter(0) << "  +-  " 
       << setw(6) << setprecision(1.1) << Ajust->GetParError(0) << "  mm " << endl;
  cout << " Y_v   = " 
       << setw(8) << setprecision(3.3) << Ajust->GetParameter(1) << "  +-  " 
       << setw(6) << setprecision(1.1) << Ajust->GetParError(1) << "  mm " << endl;
  cout << " alpha = " 
       << setw(8) << setprecision(3.3) << Ajust->GetParameter(2) << "  +-  " 
       << setw(6) << setprecision(1.1) << Ajust->GetParError(2) << "  mrad " << endl;
  cout << " beta  = " 
       << setw(8) << setprecision(3.3) << Ajust->GetParameter(3) << "  +-  " 
       << setw(6) << setprecision(1.1) << Ajust->GetParError(3) << "  mrad " << endl;
  cout << " Z_v   = " 
       << setw(8) << setprecision(3.3) << -HProfZ->GetFunction("pol1")->GetParameter(0) << "  +-  " 
       << setw(6) << setprecision(1.1) << HProfZ->GetFunction("pol1")->GetParError(0) << "  mm " << endl;
  cout << " ------ Game Over. Insert coin -------- " << endl;
  cout << endl;
}
//////////////////////////////////////////////////
double cdfsvt(double *x, double *par)
{
  double X_v = par[0];
  double Y_v = par[1];
  double tan_a = par[2]/1000;  // ho tenim en mrad
  double tan_b = par[3]/1000; 

  double d0;
  
  d0 = (X_v+tan_b*x[1])*sin(x[0]) - (Y_v - tan_a*x[1])*cos(x[0]);

  return d0;
}

//////////////////////////////////////////////////
double Vtx2D_fitfun(double *x, double *par)
{
  double Vtx2D;

  
  double X_V = -par[0];
  double Y_V = -par[1];
  double R_V = 0.;

  Vtx2D =  - X_V * sin(x[0]) + Y_V * cos(x[0]) + R_V;
  /*
  double R_v = par[0];
  double Phi00 = par[1];

  Vtx2D =  R_v * sin(x[0]+Phi00);
  */
  return Vtx2D;
}

//////////////////////////////////////////////////
void Set_style()
{
  // style
  gStyle->SetPalette(1);
  gStyle->SetCanvasColor(0);
  //gStyle->SetOptFit(10);
  gStyle->SetOptFit(0);
  gStyle->SetOptStat(10);
  //  gStyle->SetStatFontSize(0);
  gStyle->SetStatColor(0);
  gStyle->SetStatW(0.22);
  //gStyle->SetStatH(0.22);
}

