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
#include <TString.h>
//
// prototipus
void Set_style();
void PrintLogo();
//*************************************************************
void CDFFit_GX2(char nname[80], int level=3, const char *chopt="all")
{
  Set_style();
  PrintLogo();
  
  //---------------------------------------------------- 
  // option handling
  //----------------------------------------------------
  TString *SolveOptions = new TString(chopt);
  SolveOptions->ToLower();
  bool ComputeAll = SolveOptions->Contains("all");
  bool ComputeXY = SolveOptions->Contains("xy");

  if (ComputeAll) cout << " --> Solving all parameters (X_v, Y_v, alpha, beta) " << endl;
  if (ComputeXY) cout << " --> Solving 2 parameters: X_v, Y_v " << endl;
  
  //---------------------------------------------------- 
  // carreguem el fitxer
  //----------------------------------------------------
  TFile *f0 = new TFile(nname);
  TProfile2D *HD;
  char nameh[80];
  char pngfile[20];

  //---------------------------------------------------- 
  // traguem primer la big matrix
  //----------------------------------------------------
  sprintf(nameh,"/trkana/CDFFit/hCDFBigMat_%d",level);
  TH2F *hBigMat;
  hBigMat = (TH2F*)f0->Get(nameh); 
  if (hBigMat==NULL) {
    cout << " desole " << nameh << " not found " << endl;
    return;
  }

  hBigMat->GetYaxis()->SetAxisColor(2);
  hBigMat->GetXaxis()->SetAxisColor(2);
  hBigMat->GetYaxis()->SetLabelSize(0.03);
  hBigMat->GetYaxis()->SetTitleSize(0.03);
  hBigMat->GetYaxis()->SetTitleOffset(1.5);
  hBigMat->GetXaxis()->SetLabelSize(0.03);
  hBigMat->GetZaxis()->SetLabelSize(0.02);

  TCanvas *CanBigMat = new TCanvas("CanBigMat","BigMatrix",10,20,500,500);
  CanBigMat->SetGridx();
  CanBigMat->SetGridy();

  hBigMat->Draw("colz");

  //----------------------------------------------------
  // ara creem la big matrix 
  //----------------------------------------------------
  int size = hBigMat->GetNbinsX();
  TMatrixD BigMatrix(size,size);
  for (int row=0; row<size; row++) {
    for (int col=0; col<size; col++) {
      double val = hBigMat->GetBinContent(row+1,col+1);
      BigMatrix(row,col) = val;
    }
  }
  if (1 == 0) {
    cout << " -- Big Matrix at input " << endl;
    BigMatrix.Print();
  }

  //---------------------------------------------------- 
  // traguem despres el big vector
  //----------------------------------------------------
  sprintf(nameh,"/trkana/CDFFit/hCDFBigVect_%d",level);
  TH1F *hBigVec;
  hBigVec = (TH1F*)f0->Get(nameh); 

  hBigVec->GetXaxis()->SetAxisColor(2);
  hBigVec->GetXaxis()->SetLabelSize(0.03);

  TCanvas *CanBigVec = new TCanvas("CanBigVec","BigVector",520,20,500,500);
  CanBigVec->SetGridx();
  CanBigVec->SetGridy();

  hBigVec->Draw();

  //----------------------------------------------------
  // i ara creem el big vector
  //----------------------------------------------------
  
  TMatrixD BigVector(size,1);
  for (int row=0; row<size; row++) {
    double val = hBigVec->GetBinContent(row+1);
    //  cout << row << " " << val << endl;
    BigVector(row,0) = val;
  }
  if (1 == 0) {
    cout << " -- Big Vector at input " << endl;
    BigVector.Print();  
  }
  
  
  //----------------------------------------------------
  // Preparem la sol.lucio...
  // si sols volem X i Y hem de canviar el tamany de les matrius 
  // amb alpha i beta del big vector nuls
  //----------------------------------------------------
  if (ComputeXY) {
    BigVector.ResizeTo(2,1);
    if (1==0) {
      cout << " -- new Big vector (after resize) " << endl;
      BigVector.Print();
    }

    BigMatrix.ResizeTo(2,2);
    if (1==0) {
      cout << " -- new big matrix (after resize) " << endl;
      BigMatrix.Print();
    }
    // change size ! very important
    size = 2;
  }


  //----------------------------------------------------
  // Ara ja podem fer les operacions
  //----------------------------------------------------
  // primer calculem la inversa de la big matrix
  TMatrixD InvBigMat = BigMatrix;
  InvBigMat.Invert();
  if (1==0){
    cout << " -- InvBigMatrix " << endl;
    InvBigMat.Print();
  }

  // omplim un histograma per a pintar-la
  TH2F *hInvBigMat = new TH2F("hInvBigMat","Inverse of Big Matrix",
			      size, -0.5, size-0.5,
			      size, -0.5, size-0.5);
  for (int row=0; row<size; row++) {
    for (int col=0; col<size; col++) {
      hInvBigMat->SetBinContent(row+1,col+1,InvBigMat(row,col));
      if (1 == 0) {
	cout << "  InvBigMat("<< row << "," <<col<< ") = " << InvBigMat(row,col);
	cout << "   histo " << hInvBigMat->GetBinContent(row+1,col+1) << endl;
      }
    }
  }
  TCanvas *CanInvBigMat = new TCanvas("CanInvBigMat","Inverse of BigMatrix",10,520,500,500);
  CanInvBigMat->SetGridx();
  CanInvBigMat->SetGridy();

  hInvBigMat->Draw("colz");
  
  //---------------------------------------------------- 
  // i ara calculem la sol.lucio
  //----------------------------------------------------
  TMatrixD B_L1(size,1);
  B_L1 = InvBigMat * BigVector *(-1);
  if (1==0) {
    cout << " --- sol.lucio --- " << endl;
    B_L1.Print();
  }

  //---------------------------------------------------- 
  // Imprimim els valors finals
  //----------------------------------------------------


  cout << endl;
  cout << " ------ Final fit -------- (signes correctes per a la DB)" << endl;
  cout << " X_v   = " 
       << setw(8) << setprecision(3.3) << -B_L1(0,0) << "  +-  " 
       << setw(6) << setprecision(1.1) << InvBigMat(0,0) << "  mm " << endl;
  cout << " Y_v   = " 
       << setw(8) << setprecision(3.3) << -B_L1(1,0) << "  +-  " 
       << setw(6) << setprecision(1.1) << InvBigMat(1,1) << "  mm " << endl;
  if (ComputeAll) {
    cout << " alpha = " 
	 << setw(8) << setprecision(3.3) << -B_L1(2,0)*1E3 << "  +-  " 
	 << setw(6) << setprecision(1.1) << InvBigMat(2,2)*1E3 << "  mrad " << endl;
    cout << " beta  = " 
	 << setw(8) << setprecision(3.3) << -B_L1(3,0)*1E3 << "  +-  " 
	 << setw(6) << setprecision(1.1) << InvBigMat(3,3)*1E3 << "  mrad " << endl;
  }
    cout << " ------ Game Over. Insert coin -------- " << endl;
  cout << endl;
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

//////////////////////////////////////////////////
void PrintLogo()
{
  cout << endl;
  cout << "  +----------------------------+ " << endl;
  cout << "  | CDF Fit using matrices     | " << endl;
  cout << "  +----------------------------+ " << endl;
  cout << endl;
}
