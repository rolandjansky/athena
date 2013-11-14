/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <stdlib.h>
#include <iostream>
#include "TFile.h"
#include "AANTEvent.h"
#include "TSystem.h"
#include "TStyle.h"
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>

#include <assert.h>

#include "Analysis.h"
std::vector<Analysis*> VectorOfAnalyses;

#include "Fakes.h"
#include "Efficiency.h"
#include "Resolution.h"
#include "Staco.h"
#include "Residual.h"

void CreateAnalyses(char* analyses) {
  bool noAnal = true;
//
  if( strstr(analyses, "effic")  != NULL ) {
    noAnal = false;
    Fakes* pFakes = new Fakes();
    VectorOfAnalyses.push_back(pFakes);
// 
    Efficiency* pEffic = new Efficiency();
    VectorOfAnalyses.push_back(pEffic);
// 
    Resolution* pResol = new Resolution();
    VectorOfAnalyses.push_back(pResol);
  }
//
  if( strstr(analyses, "staco")  != NULL ) {
    noAnal = false;
    Staco* pStaco = new Staco();
    VectorOfAnalyses.push_back(pStaco);
  }
//
/*   if( strstr(analyses, "resid")  != NULL ) { */
/*     noAnal = false; */
/*     Residual* pResidu = new Residual(); */
/*     VectorOfAnalyses.push_back(pResidu); */
/*   } */
//
//
  if(noAnal) {
    cout << "no valid analysis selected : " << analyses << endl;
    exit(1);
  }   
}

void DisplayHelp() {
  cout << " liste des options " << endl;
  cout << "   -f NtupleName :  process the file NtupleName" << endl;
  cout << "   -c ChainDir   :  create a chain with all Ntuples in directory ChainDir" << endl;
  cout << "   default (i.e. when neither -f nor -c are given) = -f ntuple.root" << endl;
  cout << "   -o OutDir     :  put all output files in directory OutDir" << endl;
  cout << "   -a Analyses   :  list of analyses to be performed (default = \"effic\")" << endl;
  cout << "   -b Comment    : commentaire a ajouter au titre des histos et plots (i.e. pt100)" << endl;
  cout << "   -s NofSigma   : Assoc criteria in sigmas (default = 20)" << endl;
  cout << "   -t            : test mode, only 50 evts are processed" << endl;
  cout << "   -d            : debug mode" << endl;
}

void initializeAnalyses(char* options, int* nofsig) {
// global initialisation
// Define Style
  gROOT->SetStyle("Plain");
  gStyle->SetMarkerStyle(20);
  gStyle->SetMarkerColor(4);
  gStyle->SetHistLineColor(9);
  gStyle->SetHistFillColor(8);
  gStyle->SetOptFit(11);            // on imprime les parametres/erreurs du fit
  gStyle->SetOptStat(1110);
  gStyle->SetFuncColor(2);
  gStyle->SetFuncWidth(2);
  gROOT->ForceStyle();
//
  printf("Starting Analysis with options: %s\n", options);
  if( strstr(options, "test")  != NULL ) printf("Test Option read : Only few events will be processed \n");
  if( strstr(options, "debug") != NULL ) printf("Debug Option read : Debug switch on \n");
//
  int SizeOfVectorOfAnalyses = VectorOfAnalyses.size() ;
  for (int is=0; is < SizeOfVectorOfAnalyses ; is++){
    VectorOfAnalyses[is]->initialize(options);
    VectorOfAnalyses[is]->setAssoCuts( *nofsig, *nofsig);
  }
//
}
void executeAnalyses() {
//
  Analysis::doAssoc();
// 
  int SizeOfVectorOfAnalyses = VectorOfAnalyses.size() ;
  for (int is=0; is < SizeOfVectorOfAnalyses ; is++){
    VectorOfAnalyses[is]->execute();
  }
// 
}
void finalizeAnalyses() {
//
  Analysis::closeAssoc();
// 
  int SizeOfVectorOfAnalyses = VectorOfAnalyses.size() ;
  for (int is=0; is < SizeOfVectorOfAnalyses ; is++){
    VectorOfAnalyses[is]->finalize();
  }
// 
}


void Boucle(TTree * fChain, char* options, char* comment, char* analyses, int* nofsig) {
   if (fChain == 0) return;
//
   AANTEvent* pevt = new AANTEvent(fChain);
   HasEvt::setEvt(pevt);
   Analysis::setComment(comment);
   CreateAnalyses(analyses);
//
   initializeAnalyses(options, nofsig);
//
   Long64_t nentries = fChain->GetEntriesFast();
   if(strstr(options, "test") != NULL && nentries > 50)  nentries = 50;

   Int_t nb = 0;
   int nono = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = pevt->LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);
      nono = nono + 1;
      if( pevt->MboyTrkMS_NTrk != pevt->MboyESD_nmuon) {
        cout << " Warning : MboyTrkMS_NTrk = " << pevt->MboyTrkMS_NTrk << " while MboyESD_nmuon = " << pevt->MboyESD_nmuon << endl;
        cout << "           for run " << pevt->Run << " evt " << pevt->Event << " nono " << nono << endl;
        continue;
      }
      executeAnalyses();
   }
   finalizeAnalyses();
}


int main(int argc, char** argv) 
{
  char options[2000];
  char HistFile[500];
  char ChainDir[500];
  char CastorFile[500];
  char Debug[200];
  char Test[200];
  char OutDir[500];
  char comment[500];
  char analyses[500];

  int c;
  int nofsig=20;

  strcpy(HistFile, "ntuple.root");
  strcpy(Debug,    "");
  strcpy(Test,     "");
  strcpy(ChainDir, "");
  strcpy(OutDir,   "");
  strcpy(comment,  "");
  strcpy(analyses, "effic");

  while (--argc > 0 && (*++argv)[0] == '-' ) {
    
    c = *++argv[0];
    
    switch (c) {
    case 'f':
      strcpy(HistFile, *++argv);
      --argc;
      break;
    case 'c':
      strcpy(ChainDir, *++argv);
      --argc;
      break;
    case 'o':
      strcpy(OutDir, *++argv);
      --argc;
      break;
    case 'a':
      strcpy(analyses, *++argv);
      --argc;
      break;
    case 'b':
      strcpy(comment, *++argv);
      --argc;
      break;
    case 'd':
      strcpy(Debug, "debug");
      break;
    case 't':
      strcpy(Test, "test");
      break;
    case 's':
      nofsig = atoi( *++argv );
      --argc;
      break;
    case 'h':
      DisplayHelp();
      return 0;
    default:
      printf("find: illegal option  found %c\n",c);
      //            --argc;
      //            argc = 0;
      break;
    }
      
  }
  

  printf("HistFile |%s|\n",HistFile);
  printf("ChainDir |%s|\n",ChainDir);
  printf("OutDir |%s|\n",OutDir);
  printf("Debug %s\n",Debug);
  printf("Test %s\n",Test);
  printf("Comment %s\n",comment);
  printf("Analyses %s\n",analyses);
  printf("NofSigma %d\n",nofsig);

  sprintf(options, "%s %s",Debug,Test);

  TTree * tree;
  if (strcmp(ChainDir,"") == 0) {
    TFile f;
    if ( strstr(HistFile,"castor") != NULL ) {
      sprintf(CastorFile,"rfio:%s",HistFile);
      printf("CastorFile %s\n",CastorFile);
      TFile *f = TFile::Open(CastorFile);
    } else {
      f.Open(HistFile);
    }
    gDirectory->ls();
    tree = ((TTree*)gDirectory->Get("CollectionTree"));
  } else {
    TChain * chain = new TChain("CollectionTree","");
    TString reper = TString(ChainDir);
    TString separ = TString("/");
    TString aant  = TString("/CollectionTree");
    void *dir = gSystem->OpenDirectory(ChainDir);
    const char *file;
    TFile f;
    int totevt = 0, filevt=0;
    while ((file = gSystem->GetDirEntry(dir))) {
      if(strcmp(file, ".") != 0 && strcmp(file, "..") ) {
        void *tmp = gSystem->OpenDirectory(file);
        if(tmp != 0)  printf (" file %s\n",file);
        TString s = TString(file);
	    if(s.EndsWith(".root")) {
	      TString ntuple =  reper + separ + s + aant;
              cout << " ntuple " << ntuple << endl;
	      chain->Add(ntuple.Data());
        }
        gSystem->FreeDirectory(tmp);
      }
    }
    tree = chain;
  }
  assert(tree);
  Boucle(tree, options, comment, analyses, &nofsig);

  if(strcmp(OutDir,"") != 0) {
    char command[256];
    sprintf(command, "mkdir -p %s", OutDir); gSystem->Exec(command);
    void *dir = gSystem->OpenDirectory(".");
    const char *file;
    while ((file = gSystem->GetDirEntry(dir))) {
      TString s = TString(file);
      if(s.EndsWith(".ps") || s.EndsWith(".png") || s.BeginsWith("dump") || s.BeginsWith("resol")) {
        sprintf(command,"mv %s %s/%s", file, OutDir, file); gSystem->Exec(command);
      }
    }
    gSystem->FreeDirectory(dir);  
  }
}
