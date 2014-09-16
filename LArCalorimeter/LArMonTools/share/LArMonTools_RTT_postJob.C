//#####################################################################
//## AUTHOR:      Tayfun Ince (Tayfun.Ince@cern.ch)
//## FILE:        LArMonTools_RTT_postJob.C
//## MODIFIED:    2006/04/04
//## DESCRIPTION: Runs once the RTT finishes the athena job on a 
//##              pedestal run 1268 for 1k events and dumps some of 
//##              the histos from the root file to a ps file.
//#####################################################################

#include <TROOT.h>
#include <TCanvas.h> 
#include <TString.h>
#include <TFile.h>
#include <TPostScript.h>
#include <iostream>
#include <iomanip>
using namespace std;

TFile *rootFile = 0;
TH1 *h1d = 0; 
TH2 *h2d = 0;
TString rootFileName = "RootHistos_1268.root";
TString psFileName = "LArMonTools_RTT_postJob.ps";
TString dirLArDigit0Noise = "/LArDigit0Noise/";
TString dirLArRawChannelNoise = "/LArRawChannelNoise/";
TString dirLArRawChannel = "/LArRawChannel/";
 
//#####################################################################
//#####################################################################
void LArMonTools_RTT_postJob()
{
  gROOT->SetStyle("Plain");
  gStyle->SetPaperSize(TStyle::kUSLetter);
  gStyle->SetOptStat("e");
  gStyle->SetStatW(0.12);
  gStyle->SetStatFontSize(0.05);
  gStyle->SetStatX(0.99);
  gStyle->SetStatY(0.99);

  //######################
  // open all files
  ofstream textFile("LArMonTools_RTT_postJob.txt");
  textFile << "Processing LArMonTools_RTT_postJob.C...\n";
  Long_t *id, *sz, *flgs, *mt;
  int fileExist = gSystem->GetPathInfo(rootFileName, id, sz, flgs, mt);
  if(fileExist != 0){
    textFile << "\nERROR ==>> ROOT FILE DOES NOT EXIST!\n";
    return;
  } else{
    rootFile = new TFile(rootFileName);
    textFile << "\nINFO ==>> FILE:" << rootFileName << " is opened.\n";
  }
  if(rootFile->IsZombie()){
    textFile << "\nERROR ==>> NOT A ROOT FILE OR CORRUPTED!\n";
    textFile.close();
    rootFile = 0;
    return;
  }
  TCanvas *canvas = new TCanvas("canvas");
  canvas->Print(psFileName+"[");

  //######################
  // Check existing directories
  textFile << "\nINFO ==>> Existing subdirectories in the root file:\n";
  rootFile->cd();
  TKey *key; TIter next(gDirectory->GetListOfKeys());
  while(key = (TKey*)next()){
    TString dirName = key->GetName();
    textFile << setw(12) << " " << dirName << "\n";
  }

  //######################
  // Save all summary plots from LArDigit0Noise and LArRawChannelNoise
  canvas->Divide(1,2);
  int padIndex = 1;
  TList *histoList = new TList();
  int nhisto = 0, counter = 0;

  textFile << "\nINFO ==>> List of histos saved in the ps file:\n";

  Int_t colors[] = {10, 3, 2}; gStyle->SetPalette(3,colors);
  TPaveText *pt= new TPaveText(0.832,0.830,0.99,0.99,"trNDC");
  TText *t1 = pt->AddText("White(0) Unconnected");
  TText *t2 = pt->AddText("Green(1) Good");
  TText *t3 = pt->AddText("Red(2)     Warning");
  t2->SetTextColor(3); t3->SetTextColor(2);
  pt->SetBorderSize(1); pt->SetTextAlign(12);

  TString dirName = dirLArDigit0Noise;
  if(gDirectory->Get(dirName.Remove(3,'/')) != 0){
    rootFile->cd(dirLArDigit0Noise);
    if(gDirectory->Get("summary") != 0){
      gDirectory->cd("summary");
      histoList = gDirectory->GetListOfKeys();
      nhisto = histoList->GetSize();
      TIter next(histoList);
      while(key = (TKey*)next()){
	TString histoName = key->GetName();
	textFile << setw(12) << " " << histoName << "\n";
	h2d = (TH2*)gDirectory->Get(histoName);
	canvas->cd(padIndex);
	h2d->SetStats(0); h2d->SetMinimum(0); h2d->SetMaximum(2);
	h2d->Draw("COL:TEXT"); pt->Draw();
	if(padIndex == 2){
	  canvas->Print(psFileName);
	  canvas->Clear("D");
	  padIndex = 1;
	  continue;
	}
	padIndex++;
      }
    }
  }
  rootFile->cd();
  nhisto = 0; counter = 0;
  histoList->Clear();
  dirName = dirLArRawChannelNoise;
  if(gDirectory->Get(dirName.Remove(3,'/')) != 0){
    rootFile->cd(dirLArRawChannelNoise);
    if(gDirectory->Get("summary") != 0){
      gDirectory->cd("summary");
      histoList = gDirectory->GetListOfKeys();
      nhisto = histoList->GetSize();
      TIter next(histoList);
      while(key = (TKey*)next()){
	TString histoName = key->GetName();
	textFile << setw(12) << " " << histoName << "\n";
	h2d = (TH2*)gDirectory->Get(histoName);
	canvas->cd(padIndex);
	h2d->SetStats(0); h2d->SetMinimum(0); h2d->SetMaximum(2);
	h2d->Draw("COL:TEXT"); pt->Draw(); counter++;
	if(padIndex == 2 || counter == nhisto){
	  canvas->Print(psFileName);
	  canvas->Clear("D");
	  padIndex = 1;
	  continue;
	}
	padIndex++;
      }
    }
  }

  //######################
  // Save all rms plots from LArDigit0Noise and LArRawChannelNoise
  canvas->Clear();
  canvas->Divide(2,2);
  padIndex = 1;

  rootFile->cd();
  nhisto = 0; counter = 0;
  histoList->Clear();
  dirName = dirLArDigit0Noise;
  if(gDirectory->Get(dirName.Remove(3,'/')) != 0){
    rootFile->cd(dirLArDigit0Noise);
    if(gDirectory->Get("perFebRms") != 0){
      gDirectory->cd("perFebRms");
      histoList = gDirectory->GetListOfKeys();
      nhisto = histoList->GetSize();
      TIter next(histoList);
      while(key = (TKey*)next()){
	TString histoName = key->GetName();
	textFile << setw(12) << " " << histoName << "\n";
	h1d = (TH1*)gDirectory->Get(histoName);
	canvas->cd(padIndex);
	h1d->Draw(); counter++;
	if(padIndex == 4 || counter == nhisto){
  	  canvas->Print(psFileName);  
	  canvas->Clear("D");
	  padIndex = 1;
	  continue;
	}
	padIndex++;
      }
    }
  }
  rootFile->cd();
  nhisto = 0; counter = 0;
  histoList->Clear();
  dirName = dirLArRawChannelNoise;
  if(gDirectory->Get(dirName.Remove(3,'/')) != 0){
    rootFile->cd(dirLArRawChannelNoise);
    if(gDirectory->Get("perFebRms") != 0){
      gDirectory->cd("perFebRms");
      histoList = gDirectory->GetListOfKeys();
      nhisto = histoList->GetSize();
      TIter next(histoList);
      while(key = (TKey*)next()){
	TString histoName = key->GetName();
	textFile << setw(12) << " " << histoName << "\n";
	h1d = (TH1*)gDirectory->Get(histoName);
	canvas->cd(padIndex);
	h1d->Draw(); counter++;
	if(padIndex == 4 || counter == nhisto){
  	  canvas->Print(psFileName);  
	  canvas->Clear("D");
	  padIndex = 1;
	  continue;
	}
	padIndex++;
      }
    }
  }

  //######################
  // Save all energy plots from LArRawChannel
  gStyle->SetOptStat("emr");
  canvas->Clear();
  canvas->Divide(2,2);
  padIndex = 1;

  rootFile->cd();
  nhisto = 0; counter = 0;
  histoList->Clear();
  dirName = dirLArRawChannel;
  if(gDirectory->Get(dirName.Remove(3,'/')) != 0){
    rootFile->cd(dirLArRawChannel);
    if(gDirectory->Get("febEnergy") != 0){
      gDirectory->cd("febEnergy");
      histoList = gDirectory->GetListOfKeys();
      nhisto = histoList->GetSize();
      TIter next(histoList);
      while(key = (TKey*)next()){
	TString histoName = key->GetName();
	textFile << setw(12) << " " << histoName << "\n";
	h1d = (TH1*)gDirectory->Get(histoName);
	canvas->cd(padIndex);
	h1d->Draw(); counter++;
	if(padIndex == 4 || counter == nhisto){
  	  canvas->Print(psFileName);  
	  canvas->Clear("D");
	  padIndex = 1;
	  continue;
	}
	padIndex++;
      }
    }
  }

  // Close all files
  textFile << "\nMacro is processed successfully.\n";
  textFile.close();
  canvas->Print(psFileName+"]");
  canvas->Close();
  rootFile->Close();
}
//#####################################################################
