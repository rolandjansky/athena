/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define VtxNtuple_cxx
#include "VtxNtuple.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#include "VtxStruct.h"
#include <iostream>

void VtxNtuple::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L VtxNtuple.C
//      Root > VtxNtuple t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
  using namespace std;
  
  Event evt;
  MCVertices mcvx;
  Vertices v; // standard vertices
  Vertices vRes, vPix, vSCT;
  
  TH1D* h = new TH1D("hDxMC","hDxMC",100,-0.3,0.3);
  TH1D* hRes = new TH1D("hDxRes","hDxRes",100,-0.2,0.2);
  TH1D* hPixSCT = new TH1D("hPixSCT","hPixSCT",100,-5,5);


  if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
     evt.fill((SEvent*)&ei_RunNumber);
     mcvx.fill((SMCVertex*)&mcVx_n);
     v.fill((SVertex*)&vx_n);
     vRes.fill((SVertex*)&vxSi_n);
     vPix.fill((SVertex*)&vxPix_n);
     vSCT.fill((SVertex*)&vxSCT_n);
     
     std::map<const MyMCVertex*,const MyVertex*> matched = matchVertices( mcvx, v);
     // cout << "Nvtx: " << mcvx.nVx << "  " << v.nVx << "  " << matched.size() << endl;
     for (std::map<const MyMCVertex*,const MyVertex*>::const_iterator it =  matched.begin(); it != matched.end(); ++it) {
       //it->first->print();
       //it->second->print();
       h->Fill( it->second->pos.X() - it->first->pos.X());
     }
     
     
     std::map<const MyVertex*,const MyVertex*> matchedRes = matchVertices( v, vRes);
     for (std::map<const MyVertex*,const MyVertex*>::const_iterator it =  matchedRes.begin(); it != matchedRes.end(); ++it) {
       hRes->Fill( it->second->pos.X() - it->first->pos.X());
     }
     
     std::map<const MyVertex*,const MyVertex*> matchedPixSCT = matchVertices( vPix, vSCT);
     for (std::map<const MyVertex*,const MyVertex*>::const_iterator it =  matchedPixSCT.begin(); it != matchedPixSCT.end(); ++it) {
       hPixSCT->Fill( it->second->pos.X() - it->first->pos.X());
     }
     
     
     
   }// loop over events
}




