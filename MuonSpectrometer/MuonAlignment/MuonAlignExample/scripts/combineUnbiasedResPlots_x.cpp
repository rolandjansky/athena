/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TFile.h"
#include "TH1F.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TGraphErrors.h"
#include "TText.h"
#include "TLegend.h"
#include "TF1.h"
#include "TChain.h"
#include "TPaveStats.h"

#include <vector>
#include <iostream>
#include <dirent.h>
#include <sstream>
#include <stdlib.h>
#include <cmath>

using namespace std;

// Declaration of leaf types
Int_t           run;
Int_t           evt;

Int_t           nPixHits;
Int_t           nSCTHits;
Int_t           nTRTHits;
Int_t           nCSCHits;
Int_t           nMDTHits;
Int_t           nTGCHits;
Int_t           nRPCHits;
Int_t           nInnerMDTHits;
Int_t           nMiddleMDTHits;
Int_t           nOuterMDTHits;
Int_t           nTriggerPhiHits;
Double_t*       cscHitLocX=new Double_t[20];
Double_t*       cscHitLocY=new Double_t[20];
Double_t*       cscHitErrors=new Double_t[20];
//Double_t*       mdtRes =new Double_t[80];
//Double_t*       tgcRes =new Double_t[80];
//Double_t*       cscRes =new Double_t[20];
//Double_t*       rpcRes =new Double_t[80];
//Double_t*       mdtPull=new Double_t[80];
//Double_t*       tgcPull=new Double_t[80];
//Double_t*       cscPull=new Double_t[20];
//Double_t*       rpcPull=new Double_t[80];

Int_t           author;
Int_t           hIndex;
Int_t           phiSector;
Int_t           isEndcap;
Int_t           identifier;
Int_t           isCSide;
Int_t           detType;
Int_t           layer;
Int_t           stName;
Int_t           stPhi;
Int_t           stEta;
Double_t        momentum;
Double_t        charge;
Double_t        d0;
Double_t        z0;
Double_t        phi0;
Double_t        theta0;
Double_t        eta0;
Double_t        calEnergyLoss;
Double_t        calEnergyLossErr;
Double_t        materialOnTrack;
Double_t        dR;
Double_t        dZlocal;
Double_t        dXlocal;
Double_t        dYlocal;
Double_t        pullXlocal;
Double_t        pullYlocal;
Double_t        dXlocalAngle;
Double_t        dYlocalAngle;
Double_t        dXglobal;
Double_t        dYglobal;
Double_t        dZglobal;
Double_t        detTheta;
Double_t        detThetaErr;

// new variables to add to tree
int nTrack;
int nInnerChambers;
int nMiddleChambers;
int nOuterChambers;
int nOtherChambers;

int innerStEta [10];
int middleStEta[10];
int outerStEta [10];
int otherStEta[20];

int innerStPhi [10];
int middleStPhi[10];
int outerStPhi [10];
int otherStPhi[20];

int innerIsEC[20];
int middleIsEC[20];
int outerIsEC[20];
int otherIsEC[20];

void setBranchAddresses(TTree* tree, bool newBranches=false);

int main() 
{  
  vector<string> dirs;
  //dirs.push_back("/data/roberth/bfieldoff_V6_noTGC/user.roberth.160296-160303.bfieldoff.UnbiasedRes.V6_MSSAwIDTrackMatch_noTgcEtaHits.100917091852/");
  //dirs.push_back("/data/roberth/bfieldoff_V6_noTGC/user.roberth.160296-160303.bfieldoff.UnbiasedRes.V6_MSSAwIDTrackMatch_noTgcEtaHits.100917011842/");
  //dirs.push_back(".");
  dirs.push_back("/data/roberth/bfieldoff_V10/user.roberth.160296-160303.bfieldoff.UnbiasedRes.V10.101002133022/");

  TChain* tree=new TChain("unbiasedMuonResiduals");
  for (int i=0;i<(int)dirs.size();i++) {
    std::string rootname=dirs[i]+"/*UnbiasedMuonResiduals.root*";
    cout<<"adding "<<rootname<<endl;
    tree->Add(rootname.c_str());
  }
  std::cout<<"tree has "<<tree->GetEntries()<<" entries"<<std::endl;
  if (tree->GetEntries()<1) exit(3);
  setBranchAddresses(tree); 

  nInnerChambers=nMiddleChambers=nOuterChambers=nOtherChambers=0;
  for (int ich=0;ich<10;ich++) {
    innerStEta[ich]=middleStEta[ich]=outerStEta[ich]=otherStEta[ich]=0;  
    innerStPhi[ich]=middleStPhi[ich]=outerStPhi[ich]=otherStPhi[ich]=0;    
    innerIsEC[ich]=middleIsEC[ich]=outerIsEC[ich]=otherIsEC[ich]=-1;    
  }
  int fileno=0;
  TFile* file=new TFile("/tmp/roberth/UnbiasedMuonResiduals.root","RECREATE");
  TTree* newTree=tree->CloneTree(0);
  setBranchAddresses(newTree,true);

  tree->GetEntry(0);
  double lastmomentum = momentum;
  double lastd0       = d0;
  double lastphi0     = phi0;
  int    lastHIndex   = hIndex;

  nTrack=0;
  int nFirstTrackEntry=0;
  int nEntriesForTrack=0;

  int kentry(0);
  for (int ientry=0;ientry<(int)tree->GetEntries();ientry++,kentry++) {
    //for (int ientry=0;ientry<50000;ientry++) {
    if (ientry%100000==0) 
      //if (ientry%100) 
      std::cout<<"entry "<<ientry<<std::endl;
    tree->GetEntry(ientry);
    //cout<<"hIndex: "<<hIndex<<"/"<<lastHIndex<<endl;
    //cout<<"momentum/d0/phi0: "<<momentum<<"/"<<d0<<"/"<<phi0<<endl;

    if (hIndex!=lastHIndex || 
	std::fabs(momentum-lastmomentum)>.01 ||
	std::fabs(d0      -lastd0)      >.01 ||
	std::fabs(phi0    -lastphi0)    >.01) {

      // get list of chambers for this track
      //std::list<int> chamberIds;
      for (int jentry=0;jentry<nEntriesForTrack;jentry++) {
	//cout<<"jentry: "<<jentry<<endl;
	tree->GetEntry(nFirstTrackEntry+jentry);	
	newTree->Fill();
      }
      if (kentry>=3500000) {
	cout<<"creating new file"<<endl;
	kentry=0;
	newTree->Write();
	file->Close();
	
	//delete newTree;
	fileno++;
	//delete file;
	stringstream filename; filename<<"/tmp/roberth/UnbiasedMuonResiduals_"<<fileno<<".root";
	file=new TFile(filename.str().c_str(),"RECREATE");
	newTree=tree->CloneTree(0);
	setBranchAddresses(newTree,true);
      }

      tree->GetEntry(ientry);
      nTrack++;
      nInnerChambers=nMiddleChambers=nOuterChambers=nOtherChambers=0;
      for (int ich=0;ich<10;ich++) {
	innerStEta[ich]=middleStEta[ich]=outerStEta[ich]=otherStEta[ich]=0;  
	innerStPhi[ich]=middleStPhi[ich]=outerStPhi[ich]=otherStPhi[ich]=0;  
	innerIsEC[ich]=middleIsEC[ich]=outerIsEC[ich]=otherIsEC[ich]=-1;  
      }

      lastmomentum=momentum;
      lastd0      =d0;
      lastphi0    =phi0;
      lastHIndex  =hIndex;
      nFirstTrackEntry=ientry;
      nEntriesForTrack=0;
    }
    
    nEntriesForTrack++;

    //cout<<"layer: "<<layer<<", stEta: "<<stEta<<endl;

    if      (layer==0) {
      int ieta=stEta;
      if (detType==1) ieta=0;
      innerStEta[nInnerChambers]=ieta;
      innerStPhi[nInnerChambers]=stPhi;
      innerIsEC[nInnerChambers]=isEndcap;
      //cout<<"innerStEta["<<nInnerChambers<<"]="<<innerStEta[nInnerChambers]<<endl;
      nInnerChambers++;
      if (nInnerChambers>=10) cout<<"too many inner chambers!"<<endl;
    }
    else if (layer==1) {
      middleStEta[nMiddleChambers]=stEta;
      middleStPhi[nMiddleChambers]=stPhi;
      middleIsEC[nMiddleChambers]=isEndcap;
      //cout<<"middleStEta["<<nMiddleChambers<<"]="<<middleStEta[nMiddleChambers]<<endl;
      nMiddleChambers++;
      if (nMiddleChambers>=10) cout<<"too many middle chambers!"<<endl;
    }
    else if (layer==2) {
      outerStEta[nOuterChambers]=stEta;
      outerStPhi[nOuterChambers]=stPhi;
      outerIsEC[nOuterChambers]=isEndcap;
      //cout<<"outerStEta["<<nOuterChambers<<"]="<<outerStEta[nOuterChambers]<<endl;
      nOuterChambers++;
      if (nOuterChambers>=10) cout<<"too many outer chambers!"<<endl;
    }
    else if (layer==3) {
      otherStEta[nOtherChambers]=stEta;
      otherStPhi[nOtherChambers]=stPhi;
      otherIsEC[nOtherChambers]=isEndcap;
      nOtherChambers++;
      if (nOtherChambers>=10) cout<<"too many other chambers!"<<endl;
    }
  }
  cout<<"writing tree"<<endl;

  newTree->Write();
  file->Close();
}

//________________________________________________________________________
void setBranchAddresses(TTree* tree, bool newBranches)
{
  tree->SetBranchAddress("run", &run);
  tree->SetBranchAddress("evt", &evt);
  tree->SetBranchAddress("nPixHits", &nPixHits);
  tree->SetBranchAddress("nSCTHits", &nSCTHits);
  tree->SetBranchAddress("nTRTHits", &nTRTHits);
  tree->SetBranchAddress("nCSCHits", &nCSCHits);
  tree->SetBranchAddress("nMDTHits", &nMDTHits);
  tree->SetBranchAddress("nTGCHits", &nTGCHits); 
  tree->SetBranchAddress("nRPCHits", &nRPCHits); 

  tree->SetBranchAddress("nInnerMDTHits", &nInnerMDTHits);
  tree->SetBranchAddress("nMiddleMDTHits", &nMiddleMDTHits);
  tree->SetBranchAddress("nOuterMDTHits", &nOuterMDTHits);
  tree->SetBranchAddress("nTriggerPhiHits", &nTriggerPhiHits);
  tree->SetBranchAddress("cscHitLocX", cscHitLocX);
  tree->SetBranchAddress("cscHitLocY", cscHitLocY);
  tree->SetBranchAddress("cscHitErrors", cscHitErrors);
  
  /*
  tree->SetBranchAddress("mdtRes",        mdtRes);
  tree->SetBranchAddress("tgcRes",        tgcRes);
  tree->SetBranchAddress("cscRes",        cscRes);
  tree->SetBranchAddress("rpcRes",        rpcRes);
  
  tree->SetBranchAddress("mdtPull",       mdtPull);
  tree->SetBranchAddress("tgcPull",       tgcPull);
  tree->SetBranchAddress("cscPull",       cscPull);
  tree->SetBranchAddress("rpcPull",       rpcPull);
  */

  tree->SetBranchAddress("author", &author);
  tree->SetBranchAddress("hIndex", &hIndex);
  tree->SetBranchAddress("phiSector", &phiSector);
  tree->SetBranchAddress("isEndcap", &isEndcap);
  tree->SetBranchAddress("identifier", &identifier);
  tree->SetBranchAddress("isCSide", &isCSide);
  tree->SetBranchAddress("detType", &detType);
  tree->SetBranchAddress("layer", &layer);
  tree->SetBranchAddress("stName", &stName);
  tree->SetBranchAddress("stPhi", &stPhi);
  tree->SetBranchAddress("stEta", &stEta);
  tree->SetBranchAddress("momentum", &momentum);
  tree->SetBranchAddress("charge", &charge);
  tree->SetBranchAddress("d0", &d0);
  tree->SetBranchAddress("z0", &z0);
  tree->SetBranchAddress("phi0", &phi0);
  tree->SetBranchAddress("theta0", &theta0);
  tree->SetBranchAddress("eta0", &eta0);
  tree->SetBranchAddress("calEnergyLoss", &calEnergyLoss);
  tree->SetBranchAddress("calEnergyLossErr", &calEnergyLossErr);
  tree->SetBranchAddress("materialOnTrack", &materialOnTrack);
  tree->SetBranchAddress("dR", &dR);
  tree->SetBranchAddress("dZlocal", &dZlocal);
  tree->SetBranchAddress("dXlocal", &dXlocal);
  tree->SetBranchAddress("dYlocal", &dYlocal);
  tree->SetBranchAddress("pullXlocal", &pullXlocal);
  tree->SetBranchAddress("pullYlocal", &pullYlocal);
  tree->SetBranchAddress("dXlocalAngle", &dXlocalAngle);
  tree->SetBranchAddress("dYlocalAngle", &dYlocalAngle);
  tree->SetBranchAddress("dXglobal", &dXglobal);
  tree->SetBranchAddress("dYglobal", &dYglobal);
  tree->SetBranchAddress("dZglobal", &dZglobal);
  tree->SetBranchAddress("detTheta", &detTheta);
  tree->SetBranchAddress("detThetaErr", &detThetaErr);

  // new branches
  if (newBranches) {
    tree->Branch("nTrack",          &nTrack,          "nTrack/I");
    tree->Branch("nInnerChambers",  &nInnerChambers,  "nInnerChambers/I");
    tree->Branch("nMiddleChambers", &nMiddleChambers, "nMiddleChambers/I");
    tree->Branch("nOuterChambers",  &nOuterChambers,  "nOuterChambers/I");
    tree->Branch("nOtherChambers",  &nOtherChambers,  "nOtherChambers/I");
    tree->Branch("innerStEta",       innerStEta,      "innerStEta[nInnerChambers]/I");
    tree->Branch("middleStEta",      middleStEta,     "middleStEta[nMiddleChambers]/I");
    tree->Branch("outerStEta",       outerStEta,      "outerStEta[nOuterChambers]/I");
    tree->Branch("otherStEta",       otherStEta,      "otherStEta[nOuterChambers]/I");
    tree->Branch("innerStPhi",       innerStPhi,      "innerStPhi[nInnerChambers]/I");
    tree->Branch("middleStPhi",      middleStPhi,     "middleStPhi[nMiddleChambers]/I");
    tree->Branch("outerStPhi",       outerStPhi,      "outerStPhi[nOuterChambers]/I");
    tree->Branch("otherStPhi",       otherStPhi,      "otherStPhi[nOuterChambers]/I");
    tree->Branch("innerIsEC",        innerIsEC,       "innerIsEC[nInnerChambers]/I");
    tree->Branch("middleIsEC",       middleIsEC,      "middleIsEC[nMiddleChambers]/I");
    tree->Branch("outerIsEC",        outerIsEC,       "outerIsEC[nOuterChambers]/I");
    tree->Branch("otherIsEC",        otherIsEC,       "otherIsEC[nOuterChambers]/I");
  }

  return;
}
