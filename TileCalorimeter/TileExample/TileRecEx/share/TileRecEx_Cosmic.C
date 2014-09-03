//-----------------------------------------------------------------------
//
//     TileRecEx_Cosmic.C
//    Luca Fiorini Jan 2007
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
#include "TF1.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TString.h"
#include "TCanvas.h"
#include "TVector3.h"
#include "TPaveStats.h"


enum ExitResult {
  FAILLURE = -1, SUCCESS = 0
};

ExitResult TileRecEx_Cosmic() {
	//gROOT->Reset();
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

  TString inputFileName("tile_156463_RTT.aan.root");
  TString outputFileName("TileCosmicRttHistograms.root");
  TString treeName("caloD3PD");

  Float_t ecellcut = 300.;

  TFile* inputFile = new TFile(inputFileName);

  if (inputFile->IsZombie()) {
    std::cout << "ERROR: \t Input file [" << inputFileName << "]"
        << " is incorrect root file or does not exist!" << std::endl;
    return FAILLURE;
  }
  
  TTree* inputTree;
  inputFile->GetObject(treeName, inputTree);
  if (!inputTree) {
    std::cout << "ERROR: \t There are no tree [" << treeName.Data() << "]"
	      << " in file [" << inputFile->GetName() << "]" << std::endl;
    std::cout << "INFO: \t List of file [" << inputFile->GetName() << "]:" << std::endl;
    inputFile->ls();
    return FAILLURE;
  }





//************************************************************************
// Histo definitions
//************************************************************************

   TH1F* ecell_ped = new TH1F("ecell_ped","tile_E in the pedestal region",100,-300.,2000.);
   TH1F* ecell_cosmic = new TH1F("ecell_cosmic","tile_E in the cosmic region",100, 500.,10000.);
   TH1F* ecell_pathologic = new TH1F("ecell_pathologic","tile_E < -1000 Mev or tile_E > 50000 Mev",100,-10000.,100000.);

   TH1F* tcell = new TH1F("tcell","tile_Time (tile_E > 500 Mev)",80,-120.,120.);
   TProfile* tcell_eta = new TProfile("tcell_eta","tile_Time vs tile_eta (tile_E > 500 Mev)",21, -2.025, 2.025);
   TProfile* tcell_phi = new TProfile("tcell_phi","tile_Time vs tile_phi (tile_E > 500 Mev)",64, -3.15, 3.15);
   TProfile2D* tcell_etaphi = new TProfile2D("tcell_etaphi","tile_Time (tile_E > 500 Mev)",21, -2.025, 2.025, 64,-3.15,3.15);


   // TH1F* muid_ntag = new TH1F("muid_ntag","Number of MuId tags",10,0.,10.);
   // TH2F* muid_etaphi = new TH2F("muid_etaphi","Muid tags Eta vs Phi",21, -2.025, 2.025, 64,-3.15,3.15);
   // TH2F* muonfit_direction = new TH2F("muonfit_direction","MuonFit direction",20, -0.025, 3.225, 64,-3.15,3.15);
   // TH1F* muonfit_time = new TH1F("muonfit_time","MuonFit time wrt TTC",50,-50.,50.);

   TObjArray HistArray(0);

   TCanvas *c_cellgen=new TCanvas("c_cell","Tile Cell plots");
   c_cellgen->Divide(2,2);

   c_cellgen->cd(1);
 //  inputTree->Draw("ECellsTile>>ecell_ped","ECellsTile>-300.&&ECellsTile<2000.","histo");
   inputTree->Draw("tile_E>>ecell_ped","tile_E>-300.&&tile_E<2000.","histo");
   ecell_ped->Fit("gaus","","",-250.,250.);
   ecell_ped->SetXTitle("Energy (MeV)");
   HistArray.Add(ecell_ped);
   gPad->SetLogy();



   c_cellgen->cd(2);
 //  inputTree->Draw("ECellsTile>>ecell_cosmic","ECellsTile>500.&&ECellsTile<10000.","histo");
   inputTree->Draw("tile_E>>ecell_cosmic","tile_E>500.&&tile_E<10000.","histo");
   ecell_cosmic->SetXTitle("Energy (MeV)");
   HistArray.Add(ecell_cosmic);
   gPad->SetLogy();


   c_cellgen->cd(3);
//   inputTree->Draw("ECellsTile>>ecell_pathologic","ECellsTile>50000.&&ECellsTile<-1000.","histo");
   inputTree->Draw("tile_E>>ecell_pathologic","tile_E<-1000.||tile_E>50000.","histo");
   ecell_pathologic->SetXTitle("Energy (MeV)");
   HistArray.Add(ecell_pathologic);
   
   c_cellgen->Update();
   c_cellgen->cd();
   c_cellgen->SaveAs("TileRecEx_c_cellgen.png");


   //////////////////////
   //Cell time
   //////////////////////
   TCanvas *c_celltime=new TCanvas("c_celltime","Tile Cell Time plots");
   c_celltime->Divide(2,2);

   c_celltime->cd(1);
//  inputTree->Draw("TimeCellsTile>>tcell","ECellsTile>500.","histo");
   inputTree->Draw("tile_Time>>tcell","tile_E>500.","histo");
   tcell->SetXTitle("Time (ns)");
   HistArray.Add(tcell);
   gPad->SetLogy();


   c_celltime->cd(2);
//   inputTree->Draw("TimeCellsTile:EtaCellsTile>>tcell_eta","ECellsTile>500.","prof");
   inputTree->Draw("tile_Time:tile_eta>>tcell_eta", "tile_E>500.", "prof");
   tcell_eta->SetXTitle("#eta");
   tcell_eta->SetYTitle("Time (ns)");
   HistArray.Add(tcell_eta);

   c_celltime->cd(3);
//   inputTree->Draw("TimeCellsTile:PhiCellsTile>>tcell_phi","ECellsTile>500.","prof");
   inputTree->Draw("tile_Time:tile_phi>>tcell_phi","tile_E>500.","prof");
   tcell_phi->SetXTitle("#phi");
   tcell_phi->SetYTitle("Time (ns)");
   HistArray.Add(tcell_phi);

   c_celltime->cd(4);
//  inputTree->Draw("TimeCellsTile:PhiCellsTile:EtaCellsTile>>tcell_etaphi","ECellsTile>500.","prof,zcol");
   inputTree->Draw("tile_Time:tile_phi:tile_eta>>tcell_etaphi","tile_E>500.","prof,zcol");
   tcell_etaphi->SetXTitle("#eta");
   tcell_etaphi->SetYTitle("#phi");
   HistArray.Add(tcell_etaphi);
   c_celltime->Update();
   c_celltime->cd();
   c_celltime->SaveAs("TileRecEx_c_celltime.png");

   //////////////////////
   //Tile Muid
   //////////////////////
//    TCanvas *c_muid=new TCanvas("c_muid","Tile Muid plots");
//    c_muid->Divide(1,2);

//    c_muid->cd(1);
// //   inputTree->Draw("NTileMuonTag>>muid_ntag","","histo");
//    inputTree->Draw("TileMuId_NMuons>>muid_ntag","","histo");
//    HistArray.Add(muid_ntag);
//    gPad->SetLogy();

//    c_muid->cd(2);
//    inputTree->Draw("TileMuId_Phi:TileMuId_Eta>>muid_etaphi","","zcol");
//    muid_etaphi->SetXTitle("#eta");
//    muid_etaphi->SetYTitle("#phi");
//    HistArray.Add(muid_etaphi);
//    c_muid->Update();
//    c_muid->cd();
//    c_muid->SaveAs("TileRecEx_c_muid.png");

   //////////////////////
   //Tile MuonFit
   //////////////////////
//    TCanvas *c_muonfit=new TCanvas("c_muonfit","Tile Muon fit plots");
//    c_muonfit->Divide(1,2);

//    c_muonfit->cd(1);
// //   inputTree->Draw("TileMuonFitterDirection>>muonfit_direction","","histo");
//    inputTree->Draw("TileCosmicsHT_phi[TileCosmicsHT_trackOfMaxE]:TileCosmicsHT_theta[TileCosmicsHT_trackOfMaxE]>>muonfit_direction", "","histo,zcol");
//    muonfit_direction->SetXTitle("#theta");
//    muonfit_direction->SetYTitle("#phi");
//    HistArray.Add(muonfit_direction);
// //   gPad->SetLogy();
   


//    c_muonfit->cd(2);
// //   inputTree->Draw("TileMuonFitterTime-TileMuonFitterTTCTime>>muonfit_time","","histo");
//    inputTree->Draw("TileCosmicsHT_time[TileCosmicsHT_trackOfMaxE]>>muonfit_time","","histo");
//    muonfit_time->SetXTitle("Time (ns)");
//    HistArray.Add(muonfit_time);
//    c_muonfit->Update();
//    c_muonfit->cd();
//    c_muonfit->SaveAs("TileRecEx_c_muonfit.png");


   //gSystem->Exec(mdir);


   TFile fout(outputFileName,"recreate");
   HistArray.Write(0);
   fout.Close();
   
   return SUCCESS;

}



