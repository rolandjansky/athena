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


void Make_plots(TString infile, TString outfile) {

  cout << "Opening file " << infile << "..." << endl;
  TFile *inputfile = new TFile(infile);   
  TTree *t = (TTree*) inputfile->Get("CollectionTree");
   
    //************************************************************************
    // Histo definitions
    //************************************************************************
   
    TH1F* h_totalE = new TH1F("h_totalE","total hit energy in scintillators",100,200.,1000.);
    TH1F* h_ptgen = new TH1F("h_ptgen","Pt of the generated electron",100,0.,100000.);
    TProfile* hp_totalEvsZ = new TProfile("hp_totalEvsZ","total energy in scint. dependence on Z",40,-20.,20.,0.,2000.);
    
    TObjArray HistArray(0);
 
    TCanvas *c_g4=new TCanvas("c_g4","Tile TB simulation plots");
    c_g4->Divide(2,2);

    c_g4->cd(1);
    t->Draw("PtGen[0]>>h_ptgen","","histo");
    h_ptgen->SetXTitle("Energy (MeV)");
    HistArray.Add(h_ptgen);
    
    c_g4->cd(2);
    t->Draw("totalE>>h_totalE","","histo");
    //h_totalE->Fit("gaus");
    h_totalE->SetXTitle("Energy (MeV)");
    HistArray.Add(h_totalE);   
   
    cout << "file name " << infile << endl;
    cout << "1/SF value: " << (h_ptgen->GetMean())/(h_totalE->GetMean()) << endl;

    c_g4->cd(3);
    t->Draw("totalE:ZVGen[0]>>hp_totalEvsZ","","prof");
    hp_totalEvsZ->SetXTitle("ZVGen (mm)");
    hp_totalEvsZ->SetYTitle("Energy (MeV)");
    hp_totalEvsZ->GetYaxis()->SetRangeUser(520,640);
    hp_totalEvsZ->Draw("p");

    HistArray.Add(hp_totalEvsZ);   

    c_g4->Update();
    c_g4->cd();
    c_g4->SaveAs(outfile+".png");

    TFile fout(outfile+".root","recreate");
    HistArray.Write(0);
    fout.Close();

}

int TileSimEx_90deg_macro() {

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

  TString input_filename("tiletb_aa.root");
  TString output_filename("Histo");
  Make_plots(input_filename, output_filename);
  
}



