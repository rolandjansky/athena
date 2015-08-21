/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// *
// * TO RUN:
// *
// *
// * gInterpreter->AddIncludePath("$AtlasBaseDir/*/$AtlasVersion/Trigger/TrigFTK/TrigFTKSim");
// * .L plot_simple_dataflow.C
// * run("./tempout.root");
// *
// *  


#include "TH2.h"
#include "TH1F.h"
#include "TrigFTKSim/FTKRoadStream.h"
#include "TrigFTKSim/FTKTrackStream.h"
#include "TCanvas.h"
#include <iostream>
#include "TChain.h"
#include "TBranch.h"

using namespace std;

ofstream myfile;

void printinfo(float towers[64], TString text);

void plot_simple_dataflow(TString locationOfFiles = "./tempout.root", Int_t ntorun=-1) {

  // [tower]
  float nRoad[64], nFit[64], nFitI[64], nTrack[64], nTrackI[64];
  float nCluster[8][64];
  FTKRoadStream *stream[64];
  FTKTrackStream *trackstream[64];

  for (int i=0; i < 64; i++) {
    stream[i] = 0;
    trackstream[i] = 0;
    nRoad[i] = 0;
    nFit[i] = 0;
    nTrack[i] = 0;
    nFitI[i] = 0;
    nTrackI[i] = 0;
    for (int k=0; k<8; k++)  nCluster[k][i]=0;
  }


  myfile.open ("output.txt");

  TChain *ch = 0;
  ch = new TChain("ftkdata");
  ch->Add(locationOfFiles);
  cerr << "location = " << locationOfFiles << endl;
  Int_t nentries = ch->GetEntries();
  float fnentries = nentries;
  if (ntorun > 0) nentries = ntorun;
  
  for (int itower = 0; itower < 64; itower++) {
    ch->SetBranchAddress(Form("FTKMergedRoadsStream%d",itower),&stream[itower]);
    ch->SetBranchAddress(Form("FTKMergedTracksStream%d",itower),&trackstream[itower]);
  }
  
  for (int ientry = 0; ientry < nentries; ientry++) {
    ch->GetEntry(ientry);
    if (ientry % 100 == 0) cerr << "ientry = " << ientry << " out of " << nentries << endl;
    for (int itower=0; itower < 64; itower++) {
      nRoad[itower] += stream[itower]->naoGetNroadsAM()/fnentries;
      nFit[itower] += trackstream[itower]->getNFits()/fnentries;
      nFitI[itower] += trackstream[itower]->getNFitsI()/fnentries;
      nTrack[itower] += trackstream[itower]->getNTracks()/fnentries;
      nTrackI[itower] += trackstream[itower]->getNTracksI()/fnentries;
      for (int il = 0; il < 8; il++) {
         nCluster[il][itower] += stream[itower]->naoGetNclus(il)/fnentries;
      }
    }
  }

  myfile << "Type\t\tbarrelmean\t\tbarrelmax\t\tendcapmean\t\tendcapmax" << endl;
  myfile << "--------------" << endl;
  printinfo (nFitI, "NFitAux");
  printinfo (nFit, "NFitSSB");
  printinfo (nTrackI, "NTrackAux");
  printinfo (nTrack, "NTrackSSB");
  printinfo (nRoad, "NRoads");

  // kludge, can do this better but works for now
  float temp[64];
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 64; j++) {
      temp[j] = nCluster[i][j];
    }
    printinfo (temp, Form("NClusterL%d",i));
  }
  return;
}

void printinfo(float towers[64], TString text) {

  float barrelmean(0), endcapmean(0), barrelmax(0), endcapmax(0);
  for (int i = 0; i<64; i++) {
    if (i < 16 || i >= 48) {
      if (towers[i] > endcapmax) endcapmax = towers[i];
      endcapmean += towers[i]/32.;
    }
    else {
      if (towers[i] > barrelmax) barrelmax = towers[i];
      barrelmean += towers[i]/32.;
    }
  }
  myfile << text << "\t\t" << barrelmean << "\t\t" << barrelmax << "\t\t" << endcapmean << "\t\t" << endcapmax << endl;
}

