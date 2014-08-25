/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//apply the blame to Jet Goodson <jgoodson@cern.ch>  Oct, 2009
 //I'm ripping hadd off as much as possible.
 // in an interactive ROOT session, edit the file names in sample1 and sample2 and targetFile as well as other customization variables.
// The macro will scale the each pair of compared histograms to each other, but lists the original number of events. The histos are scaled to the histo with the fewer events. So whichever number is smaller is the number of scaled events for both.
//Note that the chi2 is setup to use the sum in quadrature of both errors instead of just one error.
   // root > .L metCompare.C
   // root > metCompare()

#include <string.h>
#include <vector>
#include "TChain.h"
#include "TCanvas.h"
#include "TColor.h"
#include "TPave.h"
#include "TBox.h"
#include "THStack.h"
#include "TStyle.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TKey.h"
#include "Riostream.h"

TList *fileList1;
TList *fileList2;
TFile *Target; //pronounced Tar-jay
const string targetFile = "/tmp/jgoodson/comparison.root";
const string sample1 = "/tmp/jgoodson/user09.JetGoodson.105015.J6_pythia_jj.D3PDplusMETPerf1550.e344_s566_s584_r794_v10.root"; //first file
const string sample2 = "/tmp/jgoodson/user09.JetGoodson.105015.J6_pythia_jj.D3PDplusMETPerf1550_FTFP_Bert.e344_s586_s582_r797_v10.root"; //second file
const string sampleName1 = "sample1"; //name of file1 for legend
const string sampleName2 = "sample2"; //name of file2 for legend
const int color1 = kAzure+6; //colors for histograms.
const int color2 = kRed;       //The first one will be solid, the second one will be a line.
const int fill1 = 1001;                //fill styles. Default matches line above.
const int fill2 = 0;
const int width1 = 3;            //bolder lines for better visibility
const int width2 = 3;
const int style1 = 1;             //simple style.
const int style2 = 1;
 

void compareRootAANTs(TDirectory *target, TList *sourceList1, TList *sourceList2);
TLegend *legendaryTH1(TH1 *h1, TH1 *h2);
TLegend *legendaryTH2(TH2 *h1, TH2 *h2);


void metCompare(){
  Target = TFile::Open(targetFile.c_str(),  "RECREATE");

  fileList1 = new TList ();
  fileList1->Add( TFile::Open(sample1.c_str()) );
  fileList2 = new TList();	
  fileList2->Add(TFile::Open(sample2.c_str()) );

  compareRootAANTs(Target, fileList1, fileList2);
  }//end of metCompare()

void compareRootAANTs(TDirectory *target, TList *sourceList1, TList *sourceList2)
{
    
  TString path( (char*)strstr( target->GetPath(), ":" ) );
  path.Remove( 0, 2 );
 
  TFile *first_source = (TFile*)sourceList1->First();
  first_source->cd( path );
  TDirectory *current_sourcedir = gDirectory;
  Bool_t status = TH1::AddDirectoryStatus();
  TH1::AddDirectory(kFALSE);
  TCanvas *canvas = new TCanvas("canvas", "canvas", 0, 0, 1000,400);
  canvas->SetFillColor(0);
  canvas->GetFrame()->SetFillColor(21);
 canvas->GetFrame()->SetBorderMode(-1);
  Int_t entries_1 = 1;
 Int_t entries_2 = 1;
 Double_t scale_1 = 1.;
 Double_t scale_2 = 1.;
  //loop over each and every key
  TChain *globChain = 0;
  TIter nextkey( current_sourcedir->GetListOfKeys() );
  TKey *key, *oldkey=0;
  while( (key = (TKey*) nextkey()))
    {
      //keep only the highest of cycle numbers for each key. Good idea.
      if(oldkey && !strcmp(oldkey->GetName(),key->GetName())) continue;

      //read objects from first source file.
      first_source->cd( path );
      TObject *obj = key->ReadObj();
      if( obj->IsA()->InheritsFrom( "TH1" )  &&  !obj->IsA()->InheritsFrom( "TH2" ) ) { //a-ha!
	//if it's a descendent of TH1 we are going to grab and compare that sucka!
	TH1 *h1 = (TH1*)obj;
	h1->SetLineColor(color1);
	h1->SetFillStyle(fill1);
	h1->SetFillColor(color1);
	h1->SetLineStyle(style1);
	h1->SetLineWidth(width1);
	entries_1 = h1->GetEntries();
	//now loop over all the others.
	TFile *nextsource = (TFile*)sourceList2->First();
	while( nextsource) {
	  nextsource->cd( path );
	  TKey *key2 = (TKey*)gDirectory->GetListOfKeys()->FindObject(h1->GetName());
	  if(key2){
	    TH1 *h2 = (TH1*)key2->ReadObj();
	    cout << key->GetName() << endl;
	    h2->SetFillStyle(fill2);
	    h2->SetFillColor(color2);
	    h2->SetLineColor(color2);
	    h2->SetLineStyle(style2);
	    h2->SetLineWidth(width2);
	    entries_2 = h2->GetEntries();
	    scale_1 = 1.0;
	    scale_2 = 1.0;
	    if(entries_1 < entries_2 && entries_2 != 0) scale_2 = static_cast<double>(entries_1)/static_cast<double>(entries_2);
	    else if(entries_1 > entries_2 && entries_1 != 0)  scale_1 = static_cast<double>(entries_2)/static_cast<double>(entries_1);
	    h1->Scale(scale_1);
	    h2->Scale(scale_2);
	    target->cd();
	    canvas->SetName(key->GetName() );
	    canvas->SetTitle(key->GetName() );
	    canvas->cd();
	    if(h2->GetMaximum() > h1->GetMaximum()) h1->SetMaximum(1.1*h2->GetMaximum());
	    h1->Draw();
	    h2->Draw("SAME");
	    TLegend *legend1 = legendaryTH1(h1, h2);
	    legend1->Draw("SAME");
	    canvas->Write(key->GetName() );
	    canvas->Clear();
	  }//end key2 if-existence
	  nextsource = (TFile*)sourceList2->After( nextsource );
	}//end other source loop
      }//end of if-TH1
      else if (obj->IsA()->InheritsFrom("TH2" ) )
      {
	  TH2 *h1_2 = (TH2*)obj;
 	entries_1 = h1_2->GetEntries();
 	//now loop over all the others.
 	TFile *nextsource = (TFile*)sourceList2->First();
	while( nextsource) {
	  nextsource->cd( path );
 	  TKey *key2 = (TKey*)gDirectory->GetListOfKeys()->FindObject(h1_2->GetName());
 	  if(key2){
	    TH2 *h2_2 = (TH2*)key2->ReadObj();
 	    cout << "TH2 " << key->GetName() << endl;
 	    entries_2 = h2_2->GetEntries();
 	    scale_1 = 1.0;
 	    scale_2 = 1.0;
 	    if(entries_1 < entries_2 && entries_2 != 0) scale_2 = static_cast<double>(entries_1)/static_cast<double>(entries_2);
 	    else if(entries_1 > entries_2 && entries_1 != 0)  scale_1 = static_cast<double>(entries_2)/static_cast<double>(entries_1);
 	    h1_2->Scale(scale_1);
 	    h2_2->Scale(scale_2);
 	    target->cd();
	    //canvas->SetWindowSize(800, 400);
 	    canvas->SetName(key->GetName() );
 	    canvas->SetTitle(key->GetName() );
 	    canvas->cd();
	    canvas->Divide(2,1);
	    canvas->cd(1);
	    h1_2->Draw("colz");
	    canvas->cd(2);
	    h2_2->Draw("colz");
	    canvas->cd(1);
	    TLegend *legend2 = legendaryTH2(h1_2, h2_2);
	    legend2->Draw("SAME");
	    canvas->Write();
 	    canvas->Clear();
 	  }//end of key2
	  nextsource = (TFile*)sourceList2->After( nextsource );
 	}//end of nextsource
      }//end of if TH2
      else if ( obj->IsA()->InheritsFrom("TDirectory" ) )
	{
	  //it's a subdirectory.
	  cout << "Comparing subdirectory" << obj->GetName() << endl;
	  //create a new subdir of same name and title in the target file
	  target->cd();
	  TDirectory *newdir = target->mkdir( obj->GetName(), obj->GetTitle() );
	  //begin new round of comparing
	  compareRootAANTs(newdir, sourceList1, sourceList2);
	}//end of if-TDirectory
      else{
	//object not something we're interested in.
	cout << "Skipping non-involved object" << endl;
      }//end of unknown else
    }//end of while key = nextkey.
  target->SaveSelf(kTRUE);
  TH1::AddDirectory(status);
}//end of compareRootAANTs


TLegend *legendaryTH1(TH1 *h1, TH1 *h2){
  TLegend *legend = new TLegend(.68,.68, .99,.995);
  char c_words[100] = "blargh";
  char c_variable[100] = "#splitline{";
  Double_t chi2 = 0;
  for(Int_t i = 0; i < h1->GetNbinsX(); ++i)
    {
      if(h1->GetBinContent(i) == h2->GetBinContent(i)) chi2 += 1/h1->GetNbinsX();
      else if(h1->GetBinContent(i) && h2->GetBinContent(i)) chi2 += pow(h1->GetBinContent(i) - h2->GetBinContent(i),2)/(h1->GetNbinsX()*pow(sqrt(h1->GetBinContent(i)) + sqrt(h2->GetBinContent(i)),2));
    }//end of loop over bins.

  strcat(c_variable, sampleName1.c_str());
  strcat(c_variable, "}");
  sprintf(c_words, "{%i pre-scaled events}", h1->GetEntries());
  strcat(c_variable, c_words);
  legend->AddEntry(h1, c_variable, "l");

  strcpy(c_variable, "#splitline{");
 strcat(c_variable, sampleName2.c_str());
  strcat(c_variable, "}");
  sprintf(c_words, "{%i pre-scaled events}", h2->GetEntries());
  strcat(c_variable, c_words);
  legend->AddEntry(h2, c_variable, "l");

  sprintf(c_words, "#chi^{2}/DOF = %03f", chi2);
  legend->SetHeader(c_words);
legend->SetBorderSize(2);
  if(chi2 <= 2.0) legend->SetTextColor(kGreen);
   else if (chi2 <= 4.0 && chi2 > 2.0) legend->SetTextColor(kYellow+1);
  else legend->SetTextColor(kRed);

  return legend;
}//end of legendaryTH1

TLegend *legendaryTH2(TH2 *h1, TH2 *h2){
  TLegend *legend = new TLegend(.01, .75, .45, .995);
  char c_words[100] = "blargh";
  char c_variable[100] = "#splitline{";
  Double_t chi2 = 0;
  for(Int_t i = 0; i < h1->GetNbinsX(); ++i){
  for(Int_t j = 0; j < h1->GetNbinsY(); ++j)
  {
    if(h1->GetBinContent(i,j) == h2->GetBinContent(i,j)) chi2 += 1/(h1->GetNbinsY()*h1->GetNbinsX());
    else if(h1->GetBinContent(i,j) && h2->GetBinContent(i,j)) chi2 += pow(h1->GetBinContent(i,j) - h2->GetBinContent(i,j),2)/(h1->GetNbinsX()*h1->GetNbinsY()*pow(sqrt(h1->GetBinContent(i,j)) + sqrt(h2->GetBinContent(i,j)),2));
  }
  }//end of loop over bins.

  strcat(c_variable, sampleName1.c_str());
  strcat(c_variable, "}");
  sprintf(c_words, "{%i pre-scaled entries}", h1->GetEntries());
  strcat(c_variable, c_words);
  legend->AddEntry(h1, c_variable, "l");

  strcpy(c_variable, "#splitline{");
 strcat(c_variable, sampleName2.c_str());
  strcat(c_variable, "}");
  sprintf(c_words, "{%i pre-scaled entries}", h2->GetEntries());
  strcat(c_variable, c_words);
  legend->AddEntry(h2, c_variable, "l");

  sprintf(c_words, "#chi^{2}/DOF = %03f", chi2);
  legend->SetHeader(c_words);
 
  if(chi2 <= 2.0) legend->SetTextColor(kGreen);
  else if (chi2 <= 4.0 && chi2 > 2.0) legend->SetTextColor(kYellow+1);
  else legend->SetTextColor(kRed);

  return legend;
}//end of legendaryTH2


