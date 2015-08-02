//-----------------------------------------------------------------------
//     
//    root macro for RTT tests - sampling fraction value
//    
//
//-----------------------------------------------------------------------

#include <stdlib.h>
#include <vector>

#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TF1.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TString.h"
#include "TCanvas.h"
#include "TVector3.h"
#include "TPaveStats.h"


void Make_plots(TString infile_hit, TString infile_truth, TString outfile) {

  Bool_t write = false;

  TTree *t = new TTree();
  t->AddFriend("h32",infile_hit);
  t->AddFriend("truth",infile_truth);
  //t->AddFriend("truth=tile_ntuple",infile_truth);

   //function - dependence of the total energy in scintillators on the impact point
  Float_t thefun_p0   = 580.;
  Float_t thefun_p1   = 10.;
  Float_t thefun_p2   = 18.;
  Float_t thefun_p3   = 10.;
    
  TF1 * thefun = new TF1("thefun","[0]+ [1]*sin(x*(2*TMath::Pi()/[2])+[3])");
   
    //************************************************************************
    // Histo definitions
    //************************************************************************
   
    TH1F* h_totalE = new TH1F("h_totalE","total hit energy in scintillators",500,0.,7000.);
    TH1F* h_ptgen = new TH1F("h_ptgen","Pt of the generated electron",100,0.,250000.);
    TProfile* hp_totalEvsZ = new TProfile("hp_totalEvsZ","total energy in scint. dependence on Z",40,-20.,20.,0.,100000.);   
    
    TObjArray HistArray(0);
 
    TCanvas *c_g4=new TCanvas("c_g4","Tile TB simulation plots");
    c_g4->Divide(2,2);

    c_g4->cd(1);
    t->Draw("truth.mcpart_pt>>h_ptgen","","histo");
    h_ptgen->SetXTitle("Energy (MeV)");
    HistArray.Add(h_ptgen);
    
    c_g4->cd(2);
    t->Draw("h32.totalE>>h_totalE","","histo");
    thefun_p0 = h_totalE->GetMean();
    thefun_p1 = h_totalE->GetRMS();
    
    //h_totalE->Fit("gaus","","");
    h_totalE->SetXTitle("Energy (MeV)");
    HistArray.Add(h_totalE);   
   
    c_g4->cd(3);
    t->Draw("(h32.totalE):(truth.mcvtx_z)>>hp_totalEvsZ","","prof");
    hp_totalEvsZ->SetXTitle("ZVGen (mm)");
    hp_totalEvsZ->SetYTitle("Energy (MeV)");
    //hp_totalEvsZ->GetYaxis()->SetRangeUser(520,640);
    hp_totalEvsZ->Draw("p");

    thefun->SetParameter(0,thefun_p0);
    thefun->SetParameter(1,thefun_p1);
    thefun->SetParameter(2,thefun_p2);
    thefun->SetParameter(3,thefun_p3);

    thefun->SetParLimits(0,0.,9000.);
    thefun->SetParLimits(1,0.,900.);
    thefun->SetParLimits(2,11.,25.);
    thefun->SetParLimits(3,0.,25.);

    
    hp_totalEvsZ->Fit("thefun","","",-15.,15.);

    cout << "file name " << infile_hit << endl;
    cout << "beamE/total_E(hit) - no z correction " <<   (h_ptgen->GetMean())/(h_totalE->GetMean()) << endl;
    cout << "total_E(hits) with z correction " << thefun->GetParameter(0) << "+/-" << thefun->GetParError(0) << " dErr % " << thefun->GetParError(0)/thefun->GetParameter(0)*100 << endl;
    cout << "1/SF value: " << (h_ptgen->GetMean())/(thefun->GetParameter(0)) << "+/-" << (h_ptgen->GetMean()/thefun->GetParameter(0))*(thefun->GetParError(0)/thefun->GetParameter(0)) << " dErr % " <<  (thefun->GetParError(0)/thefun->GetParameter(0))*100 << endl;   


    HistArray.Add(hp_totalEvsZ);   

    c_g4->Update();
    c_g4->cd();
    // c_g4->SaveAs(outfile+".png");

    if (write) {
      TFile fout(outfile,"recreate");
      HistArray.Write(0);
      fout.Close();
    }
    
    TFile f("EvtMax=6000.root","recreate");
    HistArray.Write();
    f.Close();

}

int sf_eta035() {

  gROOT->SetStyle("Plain");
  gStyle->SetPalette(51,0);    
  gStyle->SetPadBottomMargin(0.13);
  gStyle->SetPadTopMargin   (0.20);
  gStyle->SetStatFontSize  (0.05);
  gStyle->SetStatStyle     (0);
  gStyle->SetOptStat(111111);  
  gStyle->SetOptFit(1111);  
  gStyle->SetTitleH(0.1);
  gStyle->SetCanvasDefW(900);
  gStyle->SetCanvasDefH(600);

  TString directory = "./";
  TString file_hit = directory+"tile_ntup.root";
  TString file_truth = directory+"tile_d3pd.root";

  cout << file_hit << endl;
  cout << file_truth << endl;

  TString output_filename("Histo.root");
  cout << output_filename << endl;
  
  Make_plots(file_hit, file_truth, output_filename);

  return 1;  

}



