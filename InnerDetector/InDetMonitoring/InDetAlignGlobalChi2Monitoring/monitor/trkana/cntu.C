/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TSystem.h"

TFile *f0;
TFile *check;
TTree *RecoTrk;
TTree *RHawNtu;
TTree *RHawMod;
TChain *GlblNtu;
TChain *GlblSummary;
TChain *TrkTrackNtu;
TChain *SiClusterNtu;
TChain *TrkMatchingNtu;
TTree *CBNTupl;
TString *InputMonitorFile;
TString *DriverMacro;

char path[500];
int firstFile,firstFile2;

void cntu(char ipath[500]=".",int nCPUs=1,int IterNumber=0) {

  char filename[500];

  bool doTrkTrack = true;
  bool doSiCluster = true;
  bool doTrkMatching = true;
  /*
   A TChain is a collection of TFile objects.
    the first parameter "name" is the name of the TTree object
    in the files added with Add.
   Use TChain::Add to add a new element to this chain.

   In case the Tree is in a subdirectory, do, eg:
     TChain ch("subdir/treename");

    Example:
  Suppose we have 3 files f1.root, f2.root and f3.root. Each file
  contains a TTree object named "T".
     TChain ch("T");  creates a chain to process a Tree called "T"
     ch.Add("f1.root");
     ch.Add("f2.root");
     ch.Add("f3.root");
     ch.Draw("x");
       The Draw function above will process the variable "x" in Tree "T"
       reading sequentially the 3 files in the chain ch.
  */

  // set the f0 the first file
  void *dirp = gSystem->OpenDirectory(ipath);
  Char_t *afile;
  Char_t path2[500];
  Int_t status;
  Long_t id, size, flag, modtime;
  firstFile = 999;
  while ((afile = const_cast<Char_t *>(gSystem->GetDirEntry(dirp)))) {
    //cout << "Filename " << afile << endl;
    if (strcmp(afile,".")==0) continue;
    if (strcmp(afile,"..")==0) continue;
    if (strcmp(afile,"logs")==0) continue;
    sprintf(filename,"%s/%s",ipath,afile);
    status = gSystem->GetPathInfo(filename,&id,&size,&flag,&modtime);

    if((flag=!3)) continue;
    firstFile2 = 0;
    sprintf(path2,"00");
    //cout << "aflie: " << afile << " " << path2 <<  endl;
    while ((strcmp(afile,path2)!=0) && (firstFile2<1000) ){
      firstFile2++;
      sprintf(path2,"%02d",firstFile2);
      
    }
    if (firstFile2<firstFile) firstFile = firstFile2;
  }
  
  sprintf(filename,"%s/%02d/Iter%d_monitor.root",ipath,firstFile,IterNumber);
  cout << "filename: " << filename << endl;
  f0 = new TFile(filename);
  
  if (f0!=NULL) {
     TChain *tree1 = (TChain*)f0->Get("TrkTrack");
     if (tree1==NULL) doTrkTrack = false;

     TChain *tree2 = (TChain*)f0->Get("SiCluster");
     if (tree2==NULL) doSiCluster = false;
     
     TChain *tree3 = (TChain*)f0->Get("TrkTrack_Matching");
     if (tree3==NULL) doTrkMatching = false;
     
    GlblNtu = new TChain("GlobalChi2/General");
    GlblSummary = new TChain("GlobalChi2/Summary");
    if (doTrkTrack) TrkTrackNtu = new TChain("TrkTrack");
    if (doSiCluster) SiClusterNtu = new TChain("SiCluster");
    if (doTrkMatching) TrkMatchingNtu = new TChain("TrkTrack_Matching");
    
    cout << "\nNumber of CPUs used to process the job: " << nCPUs << endl;
    for (int i=firstFile; i<nCPUs+firstFile; i++) { 
    
      sprintf(filename,"%s/%02d/Iter%d_monitor.root",ipath,i,IterNumber);
      // Check existence of the file
      check = new TFile(filename);
      if (check->IsZombie()){
	cout << " WARNING: " << filename << " not exists or is corrupt" << endl;
      }
      else {
 	cout << "Filename:" << filename << endl;
	GlblNtu->Add(filename);
	GlblSummary->Add(filename);
	if (doTrkTrack) TrkTrackNtu->Add(filename);
	if (doSiCluster) SiClusterNtu->Add(filename);
	if (doTrkMatching) TrkMatchingNtu->Add(filename);
      }
    }
  }
  else 
    cout << "ERROR:: no GlobalX2 ntuple !!!"<< endl;
  cout << " --> GlobalX2 ntuple merged\n" << endl;

  // patch
  InputMonitorFile = new TString(ipath);  // this is necessary to avoid a crash in trkana

  // patch 
  if (DriverMacro==NULL) {
    DriverMacro = new TString("monitor");
  }

  // cout << "TrkTrackNtu: " << doTrkTrack << endl;
  // cout << "SiClusterNtu: " << doSiCluster << endl;

  cout << "let's call trkana.C..." << endl;
  cout << flush;

}
