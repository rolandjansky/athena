/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//apply the blame to Jet Goodson <jgoodson@cern.ch> Oct, 2009
//Goes through a root file and grabs the hists or canvases and prints them
//to a postscript file.
//run with:
   // root > .L psPrinter.C
   // root > psPrinter()

#include <string.h>
#include <vector>
#include "TChain.h"
#include "TCanvas.h"
#include "TColor.h"
#include "TPave.h"
#include "TBox.h"
#include "THStack.h"
#include "TDirectory.h"
#include "TStyle.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TKey.h"
#include "Riostream.h"

TFile *Target; //pronounced Tar-jay
const string targetFile = "input.root"; //root file you want a folder made into a postscript
const string targetFolder = "BasicPlots"; //target folder, including parent directories if subfolder
const string psFile = "comparison.ps"; //output postscript file.
const int color = kRed; //color used for TH1s
const int fill = 1001; //fill style used
const int width = 3; //line width
const int style = 1; //line style

void psMaker(TDirectory *folder);

void psPrinter()
{
Target = TFile::Open(targetFile.c_str());
//folderFinder(Target, targetFolder);
 string directoryName = targetFile + ":/" + targetFolder;
 TDirectory *targetDirectory = gDirectory->GetDirectory(directoryName.c_str());
 psMaker(targetDirectory);
}//end of psPrinter()

void psMaker(TDirectory *folder){
 gStyle->SetOptStat(0);
 TPostScript *ps = new TPostScript(psFile.c_str(),111);

  Double_t stops[5] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  Double_t red[5]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t green[5] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t blue[5]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
  TColor::CreateGradientColorTable(5, stops, red, green, blue, 255);
  gStyle->SetNumberContours(255);

  TCanvas *psCanvas = new TCanvas("psCanvas", "psCanvas", 0, 0, 1000, 1200);
  psCanvas->Divide(1,3);
  psCanvas->Clear("d");
  ps->NewPage();
  TString path( (char*)strstr( folder->GetPath(), ":" ) );
  path.Remove( 0, 2 );
 
  Bool_t status = TH1::AddDirectoryStatus();
  TH1::AddDirectory(kFALSE);
  TChain *globChain = 0;
  TIter nextkey( folder->GetListOfKeys() );
  TKey *key, *oldkey=0;
  Int_t i = 0;
  bool run = true;
  key = ((TKey*) nextkey());
  while(run)//(key = (TKey*) nextkey()))
    {
      //keep only the highest of cycle numbers for each key. Good idea.
      if(oldkey && !strcmp(oldkey->GetName(),key->GetName())) continue;
      TObject *obj = key->ReadObj();
      if(!(key = (TKey*) nextkey())) run = false;
      if( obj->IsA()->InheritsFrom( "TH1" ) && !obj->IsA()->InheritsFrom( "TH2" ))
	{
	  psCanvas->cd(1 + i%3);
	  TH1 *h1 = (TH1*)obj;
	  h1->SetFillColor(color);
	  h1->SetLineColor(color);
	  h1->SetFillStyle(fill);
	  h1->SetLineStyle(style);
	  h1->SetLineWidth(width);
	  h1->Draw();
	  if(i%3 == 2){
	    psCanvas->Update();
	    if(run){
	      ps->NewPage();
	      psCanvas->Clear("d");
	    }
	    }//end of pagetest
	}//end of is -TH1
      else if(obj->IsA()->InheritsFrom( "TH2" ))
	{
	  psCanvas->cd(1 + i%3);
	  TH2 *h2 = (TH2*)obj;
	  h2->Draw("colz");
	  if(i%3 == 2){
	    psCanvas->Update();
	    if(run){
	      ps->NewPage();
	      psCanvas->Clear("d");
	    }//ifrun
	    }//end of pagetest
	}//end of TH2
      else if( obj->IsA()->InheritsFrom( "TCanvas" ))
	{
	  psCanvas->cd(1 + i%3);
	  TCanvas *canvas1 = (TCanvas*)obj;
	  canvas1->DrawClonePad();
	  if(i%3 == 2){
	    psCanvas->Update();
	    if(run){
	      ps->NewPage();
	      psCanvas->Clear("d");
	  }
	  }//end of pagetest
	}
      ++i;
    }//end of while key=nextkey
  ps->Close();
}//end of psmaker
