/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <stdio.h>
#include <stdlib.h>


#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TSystem.h>
#include <TKey.h>
#include <TPaveStats.h>
#include <TPad.h>

#include <TTree.h>
#include <TFile.h>
#include <TMath.h>
#include <TH1.h>
#include <iostream> 
#include <iomanip>
#include <Riostream.h> 
#include <string.h>

using namespace std;


struct sample_struct {
  TString fname;
  TString label;
  Int_t color;
  Int_t line;
  Int_t marker;
};

#define NPROCESS 2
sample_struct samples[NPROCESS]={
  {"expert-monitoring.Up.root",   "Merged", 1, 1, 22 },
  {"expert-monitoring.Down.root", "L2+EF ", 2, 1, 23 },
};

#define NSKIP 3
static string toskip_list[NSKIP]={"Time", "TrigSteer_L2/ChainAcceptance","TrigSteer"};


//global variables
bool plot  = 1;
bool debug = 0;//default is 0
bool skip_steering =1;

TCanvas     *c1 ;
TLegend     *legend;
int dir_level=0;
char global_path[200];


// error counters:
int counter   = 0;
int n_tot     = 0;
int n_skip    = 0;
int n_diff_n  = 0;
int n_diff_c  = 0;
int n_missing = 0;

#define NLIST 200
TString list_skip[NLIST];
TString list_missing[NLIST];
TString list_diff_n[NLIST];
TString list_diff_c[NLIST];

//ROS access
TString missAllROSRequest[NLIST];
TString missROSRequest[NLIST];
TString missAllROSPrefetch[NLIST];
TString missROSPrefetch[NLIST];
int n_missAllROSRequest  = 0;
int n_missROSRequest     = 0;
int n_missAllROSPrefetch = 0;
int n_missROSPrefetch    = 0;

TString psname="compare_diff.ps";
TString psROS="checkROS.pdf";
// aux function
void readdir(TDirectory *refdir, TDirectory *mergedir, char *refname);
// aux function
void configH(TH1* h, Int_t i);
// function to check the results of the TrigROBDataProviderSvc
void checkROSAccess(TDirectory *mergedir);
// function to plot histograms from ROBMonitor
void checkROBMonitor(TDirectory *mergedir);
// function to compare histograms from merged and not merged run (tested with Run1 menu)
void checkMerge(Int_t toler,char *refname);

/// MAIN FUNCTION
void  TrigTest_CheckMerge(Int_t toler,char *refname){
 TFile* f1 = TFile::Open(refname);
 if (!c1) c1 = new TCanvas("c1","canvas",100,10,640,720);
 c1->SaveAs((psROS+"[").Data());// just open the file
 checkROSAccess(f1);
 checkROBMonitor(f1);
 c1->SaveAs((psROS+"]").Data());// just close the file
 return;
}
////////////////



// function to compare histograms from merged and not merged run (tested with Run1 menu)
void checkMerge(Int_t toler,char *refname){

  char basename[] = "expert-monitoring.root";
  // open input files
  TFile* f0 = TFile::Open(refname);
  TFile* f1 = TFile::Open(basename);

  printf("checking histograms in files %s and %s with tolerance %d\n",basename, refname, toler);
  if(plot){
    printf("Now plot...\n");
    samples[0].label="Merged";//to reset here....
    samples[1].label="L2+EF ";
    //style
    gErrorIgnoreLevel=kError;// set message level
    gStyle->SetHistLineWidth(3.6);
    gStyle->SetLineWidth(3.2);
    gStyle->SetOptStat(111110);
    gROOT->ForceStyle();
    //canvas
    if (!c1) c1 = new TCanvas("c1","canvas",100,10,640,720);
    c1->Divide(1,2);
    c1->SaveAs((psname+"[").Data());// just open the file
    //legend
    if (!legend) legend = new TLegend(0.17,0.75,1.05,0.89);
    legend->SetBorderSize(0);
    legend->SetFillColor(0);
    legend->SetTextSize(.04);
    legend->SetFillStyle(0);//transparent
  }
  else {
    printf("Just write the differences, do not Draw...\n");
  }
 

  sprintf(global_path,"");
  // readdir(f0, f1, refname);
  c1->SaveAs((psname+"]").Data());// just close the file

  // print Summary
  printf("===========================\n");
  printf("End of test: details below:\n");
  printf("===========================\n");
  printf("Skipped histograms: %d\n", n_skip);
  for(int i=0; i<TMath::Min(n_skip, int(NLIST));i++)   printf("%d) %s \n", i,list_skip[i].Data());
  if(n_skip>=NLIST) printf(".....");
  printf("\n");
  printf("Missing histograms in the merged system: %d\n", n_missing);
  for(int i=0; i<TMath::Min(n_missing, int(NLIST));i++) printf("%d) %s \n", i,list_missing[i].Data());
  if(n_missing>=NLIST) printf(".....");
  printf("\n");
  printf("Different histograms per number of entries: %d\n", n_diff_n);
  for(int i=0; i<TMath::Min(n_diff_n, int(NLIST));i++) printf("%d) %s \n", i, list_diff_n[i].Data());
  if(n_diff_n>=NLIST) printf(".....");
  printf("\n");
  printf("Different histograms per bin values: %d\n", n_diff_c);
  for(int i=0; i<TMath::Min(n_diff_c, int(NLIST));i++) printf("%d) %s \n", i, list_diff_c[i].Data());
  if(n_diff_c>=NLIST) printf("....."); 
  printf("\n\n");

  printf("===========================\n");
  printf("TrigTest_CheckMerge Summary:\n");
  printf("===========================\n");
  printf("Total histograms compared: %d\n", n_tot);
  printf("Skipped histograms: %d\n", n_skip);
  printf("Missing histograms in the merged system: %d\n", n_missing);
  printf("Different histograms per number of entries: %d\n", n_diff_n);
  printf("Different histograms per bin values: %d\n", n_diff_c);
  if ((n_diff_n+n_diff_c)>toler) printf("Differences found are not within tolerance %d\n", toler);
  else printf("Differences found are within tolerance %d\n", toler);
  printf("===========================\n");

  return;
}


void readdir(TDirectory *refdir, TDirectory *mergedir, char *refname){
  TKey *key;
  TClass *cl;
  TH1* hmerge;
  char htitle[200];
  char legtitle[200];  
  char old_path[200];
 
  TDirectory *dirsav = gDirectory;
  TIter next(refdir->GetListOfKeys());
  int max_count=500000;
  while ((key = (TKey*)next()) && counter<max_count) {
    cl = gROOT->GetClass(key->GetClassName());  
    if (key->IsFolder()) {
      strcpy(old_path, global_path);
      strcat(global_path,key->GetName());
      strcat(global_path,"/");
      if (debug) {
	for(int d=0;d<dir_level;d++)  printf(" ");
	printf("Now in directory %s\n", global_path);
      }

      refdir->cd(key->GetName());
      dir_level++;
      TDirectory *subdir = gDirectory;
      readdir(subdir, mergedir, refname);
      strcpy(global_path,old_path);
      dirsav->cd();
      dir_level--;
      continue;
    }

    if (!cl->InheritsFrom("TH1")) continue;
    counter++;   
    
    TH1 *href = (TH1*)key->ReadObj();

    string hname_ref(string(global_path) + string(key->GetName()));
    //    string hname_ref(string(dirsav->GetName()) +"/" + string(key->GetName()));
    string hname = hname_ref;
    string other_steering_name("");
    if (debug) printf("  %s  %s ",  key->GetClassName(), hname.c_str() );
    string::size_type index;
    if ( hname.find("TrigSteer_")!= std::string::npos) {
      if(!skip_steering){
	other_steering_name=hname;
	if      ( hname.find("TrigSteer_L2") != std::string::npos ) { index = hname.find("TrigSteer_L2"); other_steering_name.replace(index, 12, "TrigSteer_EF");}
	else if ( hname.find("TrigSteer_EF") != std::string::npos ) { index = hname.find("TrigSteer_EF"); other_steering_name.replace(index, 12, "TrigSteer_L2");}
	other_steering_name=string(refname) + ":" + other_steering_name;
	hname.replace(index, 12, "TrigSteer_HLT");
	if (debug) printf("  ....replaced to %s (other is %s)\n", hname.c_str(),other_steering_name.c_str()); 
      }    
      else{
	if (debug) printf("\n");
      }
    }
    else {if (debug) printf("\n");}
    
    sprintf(htitle, "%s", hname.c_str());  

    int toskip=false;
    for (int i=0; i<NSKIP;i++){
      if ( hname_ref.find(toskip_list[i]) != std::string::npos ) {
	if(n_skip<NLIST) list_skip[n_skip]=hname_ref;
	toskip=true;
	n_skip++;
      }
    }
    if (toskip) continue;


    n_tot++;
    if(! mergedir->Get(htitle)){
      if (n_missing<NLIST) list_missing[n_missing]=hname_ref;
      n_missing++;
      if (debug) printf("**NOTFOUND %s from %s\n",htitle, hname_ref.c_str());
      continue;
    }

    hmerge = (TH1*) mergedir->Get(htitle);     

    // to add: check number of bins


    // check number of entries
    int diff_n = (href->GetEntries() - hmerge->GetEntries());

    //check Steering histograms:
    if ( hname.find("TrigSteer") != std::string::npos ){
      if(refdir->Get(other_steering_name.c_str())) {
	printf("add other steering with entries: %d\n", ((TH1*) refdir->Get(other_steering_name.c_str()))->GetEntries());
	diff_n = ( hmerge->GetEntries() < TMath::Max(href->GetEntries(), ((TH1*) refdir->Get(other_steering_name.c_str()))->GetEntries()) ) ;
      }
      else diff_n=0;
    }

   // check different bin contents
    TH1* hdiff = (TH1F*)hmerge->Clone("Difference");
    hdiff->Add(href,-1.);
    bool diff_bin = (hdiff->GetMaximum()!=0 || hdiff->GetMinimum()!=0);

    //count and print errors on debug:
    if( diff_bin || diff_n!=0){
      if(diff_n!=0){
	if(n_diff_n<NLIST)list_diff_n[n_diff_n]=hname_ref;
	n_diff_n++;
	if (debug) printf("**DIFFN:%s =%f (%.f,%.f)\n", htitle, diff_n, href->GetEntries(), hmerge->GetEntries());
      }
      else {
	if(n_diff_c<NLIST)list_diff_c[n_diff_c]=hname_ref;
	n_diff_c++;
	if(debug)  printf("**DIFFC: %s (%.f,%.f)\n", htitle, href->GetEntries(), hmerge->GetEntries());	
      }
    }

    if ( ((hdiff->GetMaximum()!=0 || hdiff->GetMinimum()!=0) || diff_n!=0)){
      if (plot){
	legend->Clear();      
	sprintf(legtitle, "%s/%s", dirsav->GetName(), key->GetName() );
	legend->SetHeader(legtitle);
	c1->cd(1);
	href->Draw(); hmerge->Draw();//just to get the Statbox
	//draw
	configH((TH1*)href, 0);
	href->Draw("PLS");  
	configH((TH1*)hmerge, 1);
	hmerge->DrawCopy("PLSAMES"); 
	//legend:
	sprintf(legtitle, "%s  : #mu = %2.1f #pm %2.1f    N=%1.f", samples[0].label.Data(), href->GetMean(), href->GetRMS()/sqrt(href->GetEntries()),href->GetEntries());
	legend->AddEntry(href,legtitle); 
	sprintf(legtitle, "%s  : #mu = %2.1f #pm %2.1f    N=%1.f", samples[1].label.Data(), hmerge->GetMean(), hmerge->GetRMS()/sqrt(hmerge->GetEntries()),hmerge->GetEntries());
	legend->AddEntry(hmerge,legtitle); 
	legend->Draw();
	//difference
	c1->cd(2);  
	hdiff->SetLineColor(4);
	hdiff->SetTitle("Difference");
	hdiff->Draw();	
	c1->SaveAs(psname.Data());
      }

    }    
  }
}


void checkROSAccess( TDirectory *mergedir){
  printf("Executing checkROSAccess\n");
  char ROSdir[]="ROBDataProviderSvc";

  
  if (!c1) c1 = new TCanvas("c1","canvas",10,10,1640,1120);
  c1->Clear();
  c1->Divide(1,1);
  c1->SetCanvasSize(1600,1100);
  c1->SetBottomMargin(0.2);
  c1->SetTopMargin(0.2);
  c1->SetLeftMargin(0.2);
  c1->SetRightMargin(0.2);

  gStyle->SetOptStat(0);
  //gROOT->ForceStyle();

  TLegend     *myleg = new TLegend(0.1,0.85,0.35,.96);
  myleg->SetBorderSize(0);
  myleg->SetFillColor(0);
  myleg->SetTextSize(.03);


  TH1F* h1, *h2, *h3, *h4, *h5;
  TH2F* h1_2D;

  //-----------------------------------
  // retrieve number of events
  mergedir->cd("TrigSteer_HLT");
  mergedir->cd("TIMERS");
  int nevents=TrigSteer_HLT_TotalTime->GetEntries();
  c1->SaveAs(psROS.Data());

  mergedir->cd();
  mergedir->cd(ROSdir);

  //-----------------------------------
  // Monitor ROS requests
  //  h1_2D=ROSRequests;
  ROSRequests->GetYaxis()->SetLabelSize(0.02);
  //  ROSRequests->GetXaxis()->SetLabelSize(0.02);
  ROSRequests->Draw("COLTEXT");
  c1->SaveAs(psROS.Data());

  printf("\n");

  int nROS=ROSRequests->GetNbinsY();
  int nalgo=ROSRequests->GetNbinsX();
  int ncall=0, ymax=0, maxncall=0;
  float avgmax=0.;
  printf("    %-65s      ROS WITH MAX REQUESTS       |  ROS REQUESTS  |  AVG ROS REQ/EVT  |\n","");
  printf("    %-65s-----------------------------------------------------------------------|\n","");
  const char* label;
  const char* label2;
 
  for (int a=1; a<=nalgo; a++){
    label = ROSRequests->GetXaxis()->GetBinLabel(a);
    maxncall=0;
    ymax=0;
    for (int r=1; r<=nROS; r++){
      ncall=ROSRequests->GetBinContent(a,r);
      if (ncall>maxncall){
	maxncall=ncall;
	ymax=r;
      }
    }
    for(int algo2=1;algo2<=CallerPerEvent->GetNbinsX(); algo2++){
      label2=CallerPerEvent->GetXaxis()->GetBinLabel(algo2);
      if ( string(label2).find(label) != std::string::npos){
	avgmax= maxncall/CallerPerEvent->GetBinContent(algo2);
	break;
      }
    }
    printf(" %-65s:    %-30s  |   %-12d |    %-12.1f   |\n", label,  ROSRequests->GetYaxis()->GetBinLabel(ymax), maxncall, avgmax);
    //    printf("Algo %s sends maximum calls to ROS %s: value=%d\n",ROSRequests->GetXaxis()->GetBinLabel(a), ROSRequests->GetYaxis()->GetBinLabel(ymax), maxncall);
  }
    printf("    %-65s-----------------------------------------------------------------------|\n","");


  //-----------------------------------
  c1->SetCanvasSize(1600,800);
  c1->SetLogy();
  // Monitor ROBs
  h1= RequestedROBsPerAlgo;
  h1->SetLineColor(1);
  h1->SetFillColor(0);
  h1->SetLineWidth(2.);
  //  h1->GetXaxis()->LabelsOption("v");
  h1->Draw();
  myleg->AddEntry(h1, "All requested ROBs","LEP");

  h2= MissingRequestedROBsPerAlgo; 
  h2->SetMarkerColor(2);
  h2->SetMarkerStyle(20);
  h2->SetMarkerSize(1.5);
  h2->Draw("SAMEP0");
  myleg->AddEntry(h2, "not predeclared ROBs","LEP");

  h4 = MissingRequestedROBsPerAlgoButCached;
  h4->SetMarkerColor(6);
  h4->SetMarkerStyle(22);
  h4->SetMarkerSize(1.5);
  h4->Draw("SAMEP0");
  myleg->AddEntry(h4, "not predecalred ROBs, but cached by different algorithm","LEP");
 
  myleg->Draw();
  c1->SaveAs(psROS.Data());

  //-----------------------------------
  myleg->Clear();
  h1= RequestedROBsPerAlgo;
  h1->SetLineColor(1);
  h1->SetFillColor(0);
  h1->SetLineWidth(2.);
  //  h1->GetXaxis()->LabelsOption("v");
  h1->Draw();
  myleg->AddEntry(h1, "All requested ROBs","LEP");

  h3 = MissingRequestedROBsPerAlgo_pref;
  h3->SetMarkerColor(4);
  h3->SetMarkerStyle(21);
  h3->SetMarkerSize(1.5);
  h3->Draw("SAMEP0");
  myleg->AddEntry(h3, "not prefetched ROBs","LEP");

  h5 = MissingRequestedROBsPerAlgoButCached_pref;
  h5->SetMarkerColor(8);
  h5->SetMarkerStyle(23);
  h5->SetMarkerSize(1.5);
  h5->Draw("SAMEP0");
  myleg->AddEntry(h5, "not prefecthed ROBs, but cached by different algorithm","LEP");
 

  myleg->Draw();
  //  c1->SaveAs("checkROSAccess_prefetch.gif");
  c1->SaveAs(psROS.Data());

  printf("\n *check ROBs Access for %d algorithms* \n", h1->GetNbinsX());
  
  printf("    %-65s   ROB   |            DURING EXECUTION       |            DURING PREFETCHING      |\n","");
  printf("    %-65s   REQ   |  MISS ALL | MISS SOME | MISS CACH |  MISS ALL |  MISS SOME | MISS CACH |\n","");

  printf("    %-65s----------------------------------------------------------------------------------|\n","");
  const char* label;
  for (int i=1; i<h1->GetNbinsX(); i++){
    label = h1->GetXaxis()->GetBinLabel(i);
    printf(" %-65s:    %-5d  |  ", label, h1->GetBinContent(i));
    if (h2->GetBinContent(i) != 0 ) {
      if  (h2->GetBinContent(i) ==  h1->GetBinContent(i)) {// all the ROBs are missed
	missAllROSRequest[n_missAllROSRequest]=label;
	printf("  %-5d  |           |", h2->GetBinContent(i) );
	n_missAllROSRequest++;
      }
      else {
	missROSRequest[n_missROSRequest]=label;
	printf("         |   %-5d   |", h2->GetBinContent(i) );
	n_missROSRequest++;
      }
    }
    else printf("         |           |");
    //cached
    if (h4->GetBinContent(i) != 0 ) 
      printf("   %-5d   |", h4->GetBinContent(i) );
    else 
      printf("           |");


    if (h3->GetBinContent(i) != 0 ) {
      if  (h3->GetBinContent(i) ==  h1->GetBinContent(i)) {// all the ROBs are missed
	missAllROSPrefetch[n_missAllROSPrefetch]=label;
	printf("   %-5d   |            |", h3->GetBinContent(i)  );
	n_missAllROSPrefetch++;
      }
      else {
	missROSPrefetch[n_missROSPrefetch]=label;
	printf("           |     %-5d  |", h3->GetBinContent(i) );
	n_missROSPrefetch++;
      }
    }
    else printf("           |            |");

    if (h5->GetBinContent(i) != 0 ) 
      printf("   %-5d   |", h5->GetBinContent(i) );
    else 
      printf("           |");


    printf("\n");
  }
  printf("    %-65s----------------------------------------------------------------------------------|\n","");


  printf("\n");
  printf("# algos with missed ROB fragments during the execution:  %d %s \n", n_missROSRequest,     (n_missROSRequest!=0? " --> WARNING":"") );    
  printf("# algos with missed ROS requests  during the execution:  %d %s \n", n_missAllROSRequest,  (n_missAllROSRequest!=0? " --> WARNING":""));    
  printf("# algos with missed ROB fragments during the prefetch:   %d %s \n", n_missROSPrefetch,    (n_missROSPrefetch!=0? " --> WARNING":""));    
  printf("# algos with missed ROS requests  during the prefetch:   %d %s \n", n_missAllROSPrefetch, (n_missAllROSPrefetch!=0? " --> WARNING":""));  
  printf("\n");

  if (n_missROSRequest || n_missAllROSRequest || n_missROSPrefetch || n_missAllROSPrefetc){
    printf("WARNING Found in TrigTest_CheckMerg.C\n");
  }
  c1->SetLogy(0);


}



void configH(TH1* h, Int_t i){
  h->SetFillColor(0);
  h->SetLineColor(samples[i].color);
  h->SetLineStyle(samples[i].line);
  h->SetMarkerColor(samples[i].color);
  h->SetMarkerStyle(samples[i].marker);
  h->SetMarkerSize(1.);
  gPad->Modified(); gPad->Update(); 
  TPaveStats * stat=(TPaveStats*) gPad->GetPrimitive("stats");   
  if(stat){
    stat->SetY1NDC(stat->GetY1NDC()-(i)*0.25);
    stat->SetY2NDC(stat->GetY2NDC()-(i)*0.25);
    stat->SetTextColor(h->GetMarkerColor());
    stat->SetLineColor(h->GetMarkerColor());
    stat->SetTextSize(.03);
    stat->SetOptStat(111110);
  }
  gPad->Modified(); gPad->Update(); 
}



void checkROBMonitor( TDirectory *mergedir){
  printf("Executing checkROBMonitor\n");
  char ROBdir[]="TrigSteer_HLT/TrigROBMoni";
  mergedir->cd(ROBdir);
  
  if (!c1) c1 = new TCanvas("c1","canvas",10,10,1640,720);
  c1->Clear();
  c1->Divide(1,1);
  c1->SetCanvasSize(1600,700);
  c1->SetBottomMargin(0.4);
  c1->SetTopMargin(0.2);
  c1->SetLogy(0);
  gStyle->SetOptStat(0);
 //  gROOT->ForceStyle();

  TLegend     *myleg = new TLegend(0.1,0.85,0.35,.96);
  myleg->SetBorderSize(0);
  myleg->SetFillColor(0);
  myleg->SetTextSize(.03);

  // c1->cd(1)->SetLogy();
  TH2I* h2;
  TProfile* TProf;

  //  c1->cd(1);
  TProf=ROBRequestTime_Algo;
  TProf->SetMarkerStyle(20);
  TProf->SetMarkerColor(1);
  TProf->SetMarkerSize(1.);
  //TProf->Draw();

  //c1->cd(2);
  TProf=ROBRequestSize_Algo;
  TProf->SetMarkerStyle(20);
  TProf->SetMarkerColor(1);
  TProf->SetMarkerSize(1.);
  //TProf->Draw();

  h2=ROBHistory_Algo;
  h2->SetMarkerStyle(20);
  h2->SetMarkerColor(1);
  h2->SetMarkerSize(1.);
  h2->Draw("COLZTEXT");
  //  c1->SaveAs("checkROBMonitor.gif");
  c1->SaveAs(psROS.Data());  

}
