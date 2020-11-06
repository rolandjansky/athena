/*
  Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration
*/

#include <TROOT.h>

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std ;

/// quick gaussian fit
void fitParameter(TH1F * hist) {
  hist->Fit("gaus");
  double RMS = hist->GetRMS();
  TF1 *fit = hist->GetFunction("gaus");
  double mean = fit->GetParameter(1);
  double sigma = fit->GetParameter(2);
  TText *tRMS = new TText(.6,.95,("RMS : " + to_string(RMS) ).c_str());
  TText *tMean = new TText(.6,.90,("Fit mean : " + to_string(mean) ).c_str());
  TText *tSigma = new TText(.6,.85,("Fit sigma : " + to_string(sigma) ).c_str());
  tRMS->SetNDC();
  tMean->SetNDC();
  tSigma->SetNDC();
  tRMS->Draw();
  tMean->Draw();
  tSigma->Draw();
  return;
}

/// Draw the Geant Following plots. Compare for each step the position of the G4 step and the ATLAS and Acts extrapolation.
/// Also compare the G4 material with the one collected by the Acts

void Geant_Following(std::string File = "", std::string Detector = "", std::string name = "") {

  std::vector<int> detectors;
  if(Detector == ""){
    name = name + "/All/";
  }
  else{
    name = name + "/" + Detector + "/";
  }
  double range_G4 = 30;
  double range_Comp = 0.2;

  if(Detector == "ID"){
    detectors.push_back(12);
    detectors.push_back(13);
    detectors.push_back(14);
    detectors.push_back(18);
    detectors.push_back(21);
    detectors.push_back(24);
    detectors.push_back(19);
    detectors.push_back(22);
    detectors.push_back(25);
    range_G4 = 3;
    range_Comp = 0.02;
  }
  else if(Detector == "Pixel"){
    detectors.push_back(12);
    detectors.push_back(13);
    detectors.push_back(14);
    range_G4 = 0.3;
    range_Comp = 0.0001;
  }
  else if(Detector == "SCT"){
    detectors.push_back(18);
    detectors.push_back(21);
    detectors.push_back(24);
    range_G4 = 1.5;
    range_Comp = 0.002;
  }
  else if(Detector == "TRT"){
    detectors.push_back(19);
    detectors.push_back(22);
    detectors.push_back(25);
    range_G4 = 3;
    range_Comp = 0.01;
  }
  else if(Detector == "Calo"){
    detectors.push_back(30);
    range_G4 = 60;
    range_Comp = 0.1;
  }

  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);

  TFile *tfile = new TFile(File.c_str());  
  TTree *tree = (TTree*)tfile->Get("G4Follower_ToolSvc.ActsGeantFollowerHelper");

  TCut detSelection = ""; 

  for(auto const& id: detectors) {
    TCut c1 = detSelection;
    TCut c2 = ("ActsVolumeId == " + to_string(id)).c_str();
    detSelection = c1 || c2;
  }

  TH1F * G4_Acts_X = new TH1F("G4_Acts_X","G4_Acts_X",41,-range_G4,range_G4);
  TH1F * G4_Acts_Y = new TH1F("G4_Acts_Y","G4_Acts_Y",41,-range_G4,range_G4);
  TH1F * G4_Acts_Z = new TH1F("G4_Acts_Z","G4_Acts_Z",41,-range_G4,range_G4);

  TH1F * G4_Trk_X = new TH1F("G4_Trk_X","G4_Trk_X",41,-range_G4,range_G4);
  TH1F * G4_Trk_Y = new TH1F("G4_Trk_Y","G4_Trk_Y",41,-range_G4,range_G4);
  TH1F * G4_Trk_Z = new TH1F("G4_Trk_Z","G4_Trk_Z",41,-range_G4,range_G4);

  TH1F * Trk_Acts_X = new TH1F("Trk_Acts_X","Trk_Acts_X",41,-range_Comp,range_Comp);
  TH1F * Trk_Acts_Y = new TH1F("Trk_Acts_Y","Trk_Acts_Y",41,-range_Comp,range_Comp);
  TH1F * Trk_Acts_Z = new TH1F("Trk_Acts_Z","Trk_Acts_Z",41,-range_Comp,range_Comp);    

  TH1F * G4_Acts_TX0 = new TH1F("G4_Acts_TX0","G4_Acts_TX0",41,-1,1);
  TH1F * G4_Acts_ACCX0 = new TH1F("G4_Acts_ACCX0","G4_Acts_ACCX0",41,-range_G4,range_G4);

  TH2F * Wrong_Mat_Step = new TH2F("Wrong_Mat_Step","Wrong_Mat_Step",14000,-7000,7000,4000,0,4000);

  TCanvas *C_G4_Acts_X = new TCanvas("C_G4_Acts_X","Comparison between the G4 and Acts step position in X") ;
  tree->Draw("G4StepX-ActsStepX>>G4_Acts_X",detSelection,"");
  G4_Acts_X->GetXaxis()->SetTitle("G4StepX-ActsStepX [mm]");
  fitParameter(G4_Acts_X);
  C_G4_Acts_X->Print((name + "/G4_Acts_X.pdf").c_str());

  TCanvas *C_G4_Acts_Y = new TCanvas("C_G4_Acts_Y","Comparison between the G4 and Acts step position in Y") ;
  tree->Draw("G4StepY-ActsStepY>>G4_Acts_Y",detSelection,"");
  G4_Acts_Y->GetXaxis()->SetTitle("G4StepY-ActsStepY [mm]");
  fitParameter(G4_Acts_Y);
  C_G4_Acts_Y->Print( (name+"/G4_Acts_Y.pdf").c_str());

  TCanvas *C_G4_Acts_Z = new TCanvas("C_G4_Acts_Z","Comparison between the G4 and Acts step position in Z") ;
  tree->Draw("G4StepZ-ActsStepZ>>G4_Acts_Z",detSelection,"");
  G4_Acts_Z->GetXaxis()->SetTitle("G4StepZ-ActsStepZ [mm]");
  fitParameter(G4_Acts_Z);
  C_G4_Acts_Z->Print( (name+"/G4_Acts_Z.pdf").c_str());


  TCanvas *C_G4_Trk_X = new TCanvas("C_G4_Trk_X","Comparison between the G4 and Trk step position in X") ;
  tree->Draw("G4StepX-TrkStepX>>G4_Trk_X",detSelection,"");
  G4_Trk_X->GetXaxis()->SetTitle("G4StepX-TrkStepX [mm]");
  fitParameter(G4_Trk_X);
  C_G4_Trk_X->Print( (name+"/G4_Trk_X.pdf").c_str());

  TCanvas *C_G4_Trk_Y = new TCanvas("C_G4_Trk_Y","Comparison between the G4 and Trk step position in Y") ;
  tree->Draw("G4StepY-TrkStepY>>G4_Trk_Y",detSelection,"");
  G4_Trk_Y->GetXaxis()->SetTitle("G4StepY-TrkStepY [mm]");
  fitParameter(G4_Trk_Y);
  C_G4_Trk_Y->Print( (name+"/G4_Trk_Y.pdf").c_str());

  TCanvas *C_G4_Trk_Z = new TCanvas("C_G4_Trk_Z","Comparison between the G4 and Trk step position in Z") ;
  tree->Draw("G4StepZ-TrkStepZ>>G4_Trk_Z",detSelection,"");
  G4_Trk_Z->GetXaxis()->SetTitle("G4StepZ-TrkStepZ [mm]");
  fitParameter(G4_Trk_Z);
  C_G4_Trk_Z->Print( (name+"/G4_Trk_Z.pdf").c_str());


  TCanvas *C_Trk_Acts_X = new TCanvas("C_Trk_Acts_X","Comparison between the Trk and Acts step position in X") ;
  tree->Draw("TrkStepX-ActsStepX>>Trk_Acts_X",detSelection,"");
  Trk_Acts_X->GetXaxis()->SetTitle("TrkStepX-ActsStepX [mm]");
  fitParameter(Trk_Acts_X);
  C_Trk_Acts_X->Print( (name+"/Trk_Acts_X.pdf").c_str());

  TCanvas *C_Trk_Acts_Y = new TCanvas("C_Trk_Acts_Y","Comparison between the Trk and Acts step position in Y") ;
  tree->Draw("TrkStepY-ActsStepY>>Trk_Acts_Y",detSelection,"");
  Trk_Acts_Y->GetXaxis()->SetTitle("TrkStepY-ActsStepY [mm]");
  fitParameter(Trk_Acts_Y);
  C_Trk_Acts_Y->Print( (name+"/Trk_Acts_Y.pdf").c_str());

  TCanvas *C_Trk_Acts_Z = new TCanvas("CZTrk_Acts_Z","Comparison between the Trk and Acts step position in Z") ;
  tree->Draw("TrkStepZ-ActsStepZ>>Trk_Acts_Z",detSelection,"");
  Trk_Acts_Z->GetXaxis()->SetTitle("TrkStepZ-ActsStepZ [mm]");
  fitParameter(Trk_Acts_Z);
  C_Trk_Acts_Z->Print( (name+"/Trk_Acts_Z.pdf").c_str());  

  TCanvas *C_G4_Acts_TX0 = new TCanvas("C_G4_Acts_TX0","Comparison between the G4 and Acts step length/X0") ;
  tree->Draw("G4StepTX0-ActsStepTX0>>G4_Acts_TX0",detSelection,"");
  G4_Acts_TX0->GetXaxis()->SetTitle("G4StepTX0-ActsStepTX0");
  C_G4_Acts_TX0->Print( (name+"/G4_Acts_TX0.pdf").c_str());

  TCanvas *C_G4_Acts_ACCX0 = new TCanvas("C_G4_Acts_ACCX0","Comparison between the G4 and Acts accumulated length/X0") ;
  tree->Draw("G4AccumX0-ActsAccumX0>>G4_Acts_ACCX0",detSelection,"");
  G4_Acts_ACCX0->GetXaxis()->SetTitle("G4accumX0-ActsAccumX0");
  C_G4_Acts_ACCX0->Print( (name+"/G4_Acts_ACCX0.pdf").c_str());

  if(Detector == ""){
    TCanvas *C_Wrong_Mat_Step = new TCanvas("C_Wrong_Mat_Step","Step with material less than 10% away from G4") ;
    tree->Draw("(sqrt(ActsStepX*ActsStepX+ActsStepY*ActsStepY)):ActsStepZ>>Wrong_Mat_Step","(fabs(G4AccumX0-ActsAccumX0)<0.10*G4AccumX0) && ActsStepTX0>0.0001","");
    C_Wrong_Mat_Step->Print( (name+"/Wrong_Mat_Step.pdf").c_str());
    C_Wrong_Mat_Step->Print( (name+"/Wrong_Mat_Step.png").c_str());
  }
  return;
}
