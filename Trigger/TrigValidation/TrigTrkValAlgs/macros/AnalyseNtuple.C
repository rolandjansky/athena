/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AnalyseNtuple.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TH1F.h"
#include "TCanvas.h"
#include <iostream>
#include <stdio.h>

AnalyseNtuple::AnalyseNtuple(std::string fileName_):
fileName(fileName_)
{
 // match tolerance parameters
 deltaEta = 0.01;
 deltaPhi = 0.05;

 // define cuts for offline tracks d0 and z0 are in mm
 z0OffCut = 500;
 d0OffCut = 400;
 nHitOffCut = 5; 
 
 // define cuts for EF tracks d0 and z0 are in mm
 z0EFCut = 500;
 d0EFCut = 400;
 nHitEFCut = 5; 
 
 offTreeName = "TrackParticleCandidate"; // name of offline collection
 efTreeName  = "InDetTrigParticleCreationIOTRT_CosmicsN_EFID"; // name of EF collection
}

void AnalyseNtuple::initialize()
{
 file = new TFile(fileName.c_str());
 if(file->IsZombie()) throw AnalyseNtupleError::LoadTFileError(fileName);
 else std::cout<<"File "<<fileName<<" succesfully loaded."<<std::endl;

 eventPropertyTree  = (TTree*) file->Get("Validation/EventToTrkParticleLink"); // tree with information about event 
 if(!eventPropertyTree ) throw AnalyseNtupleError::LoadTTreeError("EventToTrkParticleLink");

 offTrkParticleTree = (TTree*) file->Get(("Validation/"+offTreeName).c_str()); // tree with ofline tracks
 if(!offTrkParticleTree ) throw AnalyseNtupleError::LoadTTreeError(offTreeName);

 efTrkParticleTree  = (TTree*) file->Get(("Validation/"+efTreeName).c_str()); // tree with EF tracks
 if(!efTrkParticleTree ) throw AnalyseNtupleError::LoadTTreeError(efTreeName);

 setBrancheAddresses(); //set adresses of branches

 regHistos(); // register histograms
    
 numOfEvents = eventPropertyTree->GetEntries(); //retrieve number of events in the Ntuple
}

void AnalyseNtuple::execute()
{
  getchar();
 //loop over events
 for (int ev=0; ev < numOfEvents; ev++ )
   {
    // try to find EF track for given offline track (Efficiency)
    eventPropertyTree->GetEntry(ev);
    //printTracksInEvent();
    for( int offTrkID = 0; offTrkID < numOfOffTrkPerEvent; offTrkID++)
      {
        offTrkParticleTree->GetEntry(offTrkIndexBegin + offTrkID);
        if(!passedOfflineCuts()) continue;
	z0ffHist->Fill(z0Off);
        d0ffHist->Fill(d0Off);
        if(isMatchedToEF()) fillEfficiency();
      } // end of loop over offline tracks

     // try to find offline track for given EF track (if not found Fake rate)
    eventPropertyTree->GetEntry(ev);
    for( int efTrkID = 0; efTrkID < numOfEFTrkPerEvent; efTrkID++)
      {
        efTrkParticleTree->GetEntry(efTrkIndexBegin + efTrkID);
        if(!passedEFCuts()) continue;
	z0EFHist->Fill(z0EF);
        d0EFHist->Fill(d0EF);
        if(!isMatchedToOff()) fillFakeRate();
      } // end of loop over EF tracks

   } // end of loop over events
}

void AnalyseNtuple::finalize()
{
 finalizeHistos();
}

void AnalyseNtuple::regHistos()
{
 z0ResEfficiencyHist = new TH1F("Efficiency vs. z0","Efficiency vs. z0", 10 , -z0OffCut, z0OffCut );
 z0ResEfficiencyHist->Sumw2();
 z0ResEfficiencyHist->GetXaxis()->SetTitle("z0 [mm]");
 z0ResEfficiencyHist->GetYaxis()->SetTitle("Efficiency");
 z0ResEfficiencyHist->SetLineColor(600);
 z0ResEfficiencyHist->SetLineWidth(2);

 d0ResEfficiencyHist = new TH1F("Efficiency vs. d0","Efficiency vs. d0", 10 , -d0OffCut, d0OffCut );
 d0ResEfficiencyHist->Sumw2();
 d0ResEfficiencyHist->GetXaxis()->SetTitle("d0 [mm]");
 d0ResEfficiencyHist->GetYaxis()->SetTitle("Efficiency");
 d0ResEfficiencyHist->SetLineColor(600);
 d0ResEfficiencyHist->SetLineWidth(2);
 
 z0ResFakeRateHist = new TH1F("FakeRate vs. z0","FakeRate vs. z0", 10 , -z0OffCut, z0OffCut );
 z0ResFakeRateHist->Sumw2();
 z0ResFakeRateHist->GetXaxis()->SetTitle("z0 [mm]");
 z0ResFakeRateHist->GetYaxis()->SetTitle("Fake Rate");
 z0ResFakeRateHist->SetLineColor(600);
 z0ResFakeRateHist->SetLineWidth(2);

 d0ResFakeRateHist = new TH1F("FakeRate vs. d0","FakeRate vs. d0", 10 , -d0OffCut, d0OffCut );
 d0ResFakeRateHist->Sumw2();
 d0ResFakeRateHist->GetXaxis()->SetTitle("d0 [mm]");
 d0ResFakeRateHist->GetYaxis()->SetTitle("Fake Rate");
 d0ResFakeRateHist->SetLineColor(600);
 d0ResFakeRateHist->SetLineWidth(2);
 
 z0ffHist = new TH1F("z0 Offline Tracks","z0 Offline Tracks", 10, -z0OffCut, z0OffCut );
 z0ffHist->Sumw2();

 d0ffHist = new TH1F("d0 Offline Tracks","d0 Offline Tracks", 10, -d0OffCut, d0OffCut );
 d0ffHist->Sumw2();

 z0EFHist = new TH1F("z0 EFline Tracks","z0 EFline Tracks", 10, -z0EFCut, z0EFCut );
 z0EFHist->Sumw2();

 d0EFHist = new TH1F("d0 EFline Tracks","d0 EFline Tracks", 10, -d0EFCut, d0EFCut );
 d0EFHist->Sumw2();
}

void AnalyseNtuple::setBrancheAddresses()
{
  eventPropertyTree->SetBranchAddress(("n"+offTreeName+"PerEvent").c_str(), &numOfOffTrkPerEvent);
  eventPropertyTree->SetBranchAddress(("n"+efTreeName+"PerEvent").c_str(), &numOfEFTrkPerEvent);
  eventPropertyTree->SetBranchAddress((offTreeName+"IndexBegin").c_str(), &offTrkIndexBegin);
  eventPropertyTree->SetBranchAddress((efTreeName+"IndexBegin").c_str(), &efTrkIndexBegin);  

  offTrkParticleTree->SetBranchAddress("RecZ0", &z0Off );
  offTrkParticleTree->SetBranchAddress("RecD0", &d0Off);
  offTrkParticleTree->SetBranchAddress("RecEta", &etaOff);
  offTrkParticleTree->SetBranchAddress("RecPhi0", &phiOff);
  offTrkParticleTree->SetBranchAddress("nPixelHits", &numOfPixOff);
  offTrkParticleTree->SetBranchAddress("nSCTHits", &numOfSCTOff);
  offTrkParticleTree->SetBranchAddress("EventNumber", &eventNumberOff);
  
  efTrkParticleTree->SetBranchAddress("RecZ0", &z0EF);
  efTrkParticleTree->SetBranchAddress("RecD0", &d0EF);
  efTrkParticleTree->SetBranchAddress("RecEta", &etaEF);
  efTrkParticleTree->SetBranchAddress("RecPhi0", &phiEF);
  efTrkParticleTree->SetBranchAddress("nPixelHits", &numOfPixEF);
  efTrkParticleTree->SetBranchAddress("nSCTHits", &numOfSCTEF);
  efTrkParticleTree->SetBranchAddress("EventNumber", &eventNumberEF);
}


bool AnalyseNtuple::isMatchedToEF()
{
 for(int trkID =0; trkID < numOfEFTrkPerEvent; trkID ++)
   {
    efTrkParticleTree->GetEntry(efTrkIndexBegin + trkID);

    if( (TMath::Abs(etaOff - etaEF) < deltaEta) &&
        (TMath::Abs(phiOff - phiEF) < deltaPhi) ) return true;
                                                  
   }

 return false;
}

bool AnalyseNtuple::isMatchedToOff()
{
 for(int trkID =0; trkID < numOfOffTrkPerEvent; trkID++)
   {
    offTrkParticleTree->GetEntry(offTrkIndexBegin + trkID);

    if( (TMath::Abs(etaOff - etaEF) < deltaEta) &&
        (TMath::Abs(phiOff - phiEF) < deltaPhi) ) return true;
                                                  
   }
 
 //printTracksInEvent();
 return false;
}

void AnalyseNtuple::fillEfficiency()
{
  z0ResEfficiencyHist->Fill(z0Off);
  d0ResEfficiencyHist->Fill(d0Off);
}

void AnalyseNtuple::fillFakeRate()
{
  z0ResFakeRateHist->Fill(z0EF);
  d0ResFakeRateHist->Fill(d0EF);
}


void AnalyseNtuple::finalizeHistos()
{
 z0ResEfficiencyHist->Divide(z0ResEfficiencyHist, z0ffHist , 1.,1., "B");
 d0ResEfficiencyHist->Divide(d0ResEfficiencyHist, d0ffHist , 1.,1., "B");
 z0ResFakeRateHist->Divide(z0ResFakeRateHist, z0EFHist , 1.,1., "B");
 d0ResFakeRateHist->Divide(d0ResFakeRateHist, d0EFHist , 1.,1., "B");
 
 drawHistos();
}

void AnalyseNtuple::drawHistos() 
{
  TCanvas* canv = new TCanvas("Efficiency & Fake Rate", "Efficiency & Fake Rate");
  canv->Divide(2,2);
  canv->SetGrid();
  canv->SetFillColor(0);

  canv->cd(1);
  z0ResEfficiencyHist->Draw();

  canv->cd(2);
  d0ResEfficiencyHist->Draw();

  canv->cd(3);
  z0ResFakeRateHist->Draw();

  canv->cd(4);
  d0ResFakeRateHist->Draw();
}

inline bool AnalyseNtuple::passedOfflineCuts()
{
 if ( (TMath::Abs(d0Off) < d0OffCut) && 
      (TMath::Abs(z0Off) < z0OffCut) &&
      ((numOfPixOff +  numOfSCTOff/2.) > nHitOffCut)   ) return true;
 
 return false;
}

inline bool AnalyseNtuple::passedEFCuts()
{
 if ( (TMath::Abs(d0EF) < d0EFCut) && 
      (TMath::Abs(z0EF) < z0EFCut) &&
      ((numOfPixEF + numOfSCTEF)> nHitEFCut )  ) return true;
 
 return false;
}


void AnalyseNtuple::printTracksInEvent()
{
  
  std::cout <<"*********************************************************************************************"<<std::endl;
  std::cout <<"Number of offline tracks: "<< numOfOffTrkPerEvent<<std::endl;
  std::cout <<"------------------- Offline tracks in event--------------------------------------------------" <<std::endl;

   for( int offTrkID = 0; offTrkID < numOfOffTrkPerEvent; offTrkID++)
      {
       std::cout <<"Index "<<offTrkIndexBegin + offTrkID<<endl;
       offTrkParticleTree->GetEntry(offTrkIndexBegin + offTrkID);
       std::cout<<offTrkID<<", EventNumber: "<<eventNumberOff<<" eta:  "<<etaOff
		<<" phi:  "<<phiOff<<" #Pixel hits: "<<numOfPixOff<<" #SCT hits: "<<numOfSCTOff<<endl;
      } 
   std::cout <<"Number of EF tracks: "<< numOfEFTrkPerEvent<<std::endl;
   std::cout <<"------------------- Trigger tracks in event--------------------------------------------------" <<std::endl;
   for( int efTrkID = 0; efTrkID < numOfEFTrkPerEvent; efTrkID++)
      {
       efTrkParticleTree->GetEntry(efTrkIndexBegin + efTrkID);
       std::cout<<efTrkID<<", EventNumber: "<<eventNumberEF<<", eta:  "<<etaEF<<", phi:  "<<phiEF<<", #Pixel hits: "<<numOfPixEF<<", #SCT hits: "<<numOfSCTEF<<endl;
      } 
 std::cout <<"*********************************************************************************************"<<std::endl;
 getchar();
}
