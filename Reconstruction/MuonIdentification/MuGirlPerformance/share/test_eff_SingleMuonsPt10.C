#include <vector>
#include <iostream>
#include <fstream>
#include "TTree.h"
#include "TFile.h"
#include "TChain.h"
#include "DataMuGirl.C"
#include "DataAod.C"
#include "TH1.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TPad.h"
#include "TLegend.h"
#include "TGraphErrors.h"
#include "TText.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TPostScript.h"
#include "TVirtualPS.h"

#include <sstream>
#include <string>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>


//////////////////////////////////////////////
/// new without angles
/////////////////////////////////////////////////



Int_t NMdt = 0,
      NCscEta = 0,
      NCscPhi = 0,
      NTgcPhi = 0,
      NTgcEta = 0,
      NRpcPhi = 0,
      NRpcEta = 0,
      NMdtHitsSeg = 0,
      NTgcHitsSeg = 0,
      NRpcHitsSeg = 0,
      NMdtSeg = 0,
      NTgcSeg = 0,
      NRpcSeg = 0,
      NCscSeg = 0,
      NTgcRpcSeg = 0,
      NTgcRpcPhi = 0,
      NTgcRpcEta = 0,
      NTgcRpcHitsSeg = 0;

Double_t pt = 0,
         eta = 0,
         phi = 0,
         deltaEtaDirTgc = 99,
         deltaEtaDirMdt = 99;

int TotalCandidates=0;
int CandidatesWithAngle=0;
int iSegTgc;
Int_t type = 0;

void test_eff_SingleMuonsPt10( )
{
    int typeOfRun=2;
    std::vector<int> muon_index;

    TH1D * h_Pt_Muons = new TH1D("Pt_Muons", " Pt Muons ", 100, 0, 50000);
    TH1D * h_n_mdt_hits = new TH1D("h_n_mdt_hits", " N MDT hits ", 100, 0., 50.);
    TH1D * h_n_tgcphi_hits = new TH1D("h_n_tgcphi_hits", " N TGC #phi hits ", 30, 0., 30.);
    TH1D * h_n_tgceta_hits = new TH1D("h_n_tgceta_hits", " N TGC #eta hits ", 30, 0., 30.);
    TH1D * h_n_rpcphi_hits = new TH1D("h_n_rpcphi_hits", " N RPC #phi hits ", 25, 0., 25.);
    TH1D * h_n_rpceta_hits = new TH1D("h_n_rpceta_hits", " N RPC #eta hits ", 25, 0., 25.);
    TH1D * h_n_mdt_seghits = new TH1D("h_n_mdt_seghits", " N MDT hits ", 40, 0., 40.);
    TH1D * h_n_tgc_seghits = new TH1D("h_n_tgc_seghits", " N TGC hits ", 50, 0., 50.);
    TH1D * h_n_mdt_seg = new TH1D("h_n_mdt_seg", " N MDT seg ", 10, 0., 10.);
    TH1D * h_n_tgc_seg = new TH1D("h_n_tgc_seg", " N TGC seg ", 10, 0., 10.);
    TH1D * h_n_rpc_seg = new TH1D("h_n_rpc_seg", " N RPC seg ", 10, 0., 10.);
    TH1D * h_fake_hit = new TH1D("h_fake_hit", " N fakes with N hits>4", 30, 0., 30000.);
    TH1D * h_fake_hit1 = new TH1D("h_fake_hit1", " N fakes with N hits>4", 30, 0., 30000.);
    TH1D * h_fake_seg1 = new TH1D("h_fake_seg1", " N fakes with N seg>0", 30, 0., 30000.);

    TH1D * h_muon_hit = new TH1D("h_muon_hit", " N muons", 50, 0., 100000.);
    TH1D * h_muon_hit1 = new TH1D("h_muon_hit1", " N muons", 50, 0., 100000.);
    TH1D * h_muon_seg1 = new TH1D("h_muon_seg1", " N muons with N seg>0", 50, 0., 100000.);
    TH1D * h_muonHitEta = new TH1D("h_muonHitEta", "N muons", 60, 0., 2.5);
    TH1D * h_muonHit1Eta = new TH1D("h_muonHit1Eta", " N muons ", 60, 0., 2.5);
    TH1D * h_muonSegEta = new TH1D("h_muonSegEta", " N muons ", 60, 0., 2.5);

    TH1D * h_muon_chi2 = new TH1D("h_muon_chi2", " Muons chi2/Dof", 100, 0., 10.);
    TH1D * h_fake_chi2 = new TH1D("h_fake_chi2", " fakes chi2/Dof", 100, 0., 10.);
    TH1D * h_muon = new TH1D("h_muon", " N muons", 50, 0., 100000.);
    TH1D * h_muon_pt = new TH1D("h_muon_pt", " N muons", 50, 0., 100000.);
    TH1D * h_muonEta = new TH1D("h_muonEta", "N muons", 60, -3, 3);
    TH1D * h_muonPhi = new TH1D("h_muonPhi", "N muons", 60, -TMath::Pi(), TMath::Pi());
    TH1D * h_muon_NN = new TH1D("h_muon_NN", " N muons ", 50, 0., 100000.);
    TH1D * h_muonNNEta = new TH1D("h_muonNNEta", " N muons ", 60, -3, 3);
    TH1D * h_muonNNPhi = new TH1D("h_muonNNPhi", " N muons ", 60, -TMath::Pi(), TMath::Pi());
    TH1D * h_muonEta_low = new TH1D("h_muonEta_low", " N muons ", 60, -3, 3);
    TH1D * h_muonPhi_low = new TH1D("h_muonPhi_low", " N muons ", 60, -TMath::Pi(), TMath::Pi());
    TH1D * h_muonEta_high = new TH1D("h_muonEta_high", " N muons ", 60, -3, 3);
    TH1D * h_muonPhi_high = new TH1D("h_muonPhi_high", " N muons ", 60, -TMath::Pi(), TMath::Pi());
    TH1D * h_muonNNEta_low = new TH1D("h_muonNNEta_low", " N muons ", 60, -3, 3);
    TH1D * h_muonNNPhi_low = new TH1D("h_muonNNPhi_low", " N muons ", 60, -TMath::Pi(), TMath::Pi());
    TH1D * h_muonNNEta_high = new TH1D("h_muonNNEta_high", " N muons ", 60, -3, 3);
    TH1D * h_muonNNPhi_high = new TH1D("h_muonNNPhi_high", " N muons ", 60, -TMath::Pi(), TMath::Pi());

    TH1D * h_muon_NN_eff = new TH1D("h_muon_NN_eff", " N muons ", 50, 0., 100000.);
    TH1D * h_muon_NN_eff_eta = new TH1D("h_muon_NN_eff_eta", " N muons ", 60, -3, 3);
    TH1D * h_muon_NN_eff_phi = new TH1D("h_muon_NN_eff_phi", " N muons ", 60, -TMath::Pi(), TMath::Pi());
    TH1D * h_muon_NN_eff_eta_low = new TH1D("h_muon_NN_eff_eta_low", " N muons pt<5 GeV", 60, -3, 3);
    TH1D * h_muon_NN_eff_phi_low = new TH1D("h_muon_NN_eff_phi_low", " N muons pt<5 GeV", 60, -TMath::Pi(), TMath::Pi());
    TH1D * h_muon_NN_eff_eta_high = new TH1D("h_muon_NN_eff_eta_high", " N muons pt>5 GeV", 60, -3, 3);
    TH1D * h_muon_NN_eff_phi_high = new TH1D("h_muon_NN_eff_phi_high", " N muons pt>5 GeV", 60, -TMath::Pi(), TMath::Pi());

    TH1D * h_fake = new TH1D("h_fake", " N fakes", 50, 0., 100000.);
    TH1D * h_fake_pt = new TH1D("h_fake_pt", " N fakes", 50, 0., 100000.);
    TH1D * h_fakeEta = new TH1D("h_fakeEta", " N fakes", 60, -3, 3);
    TH1D * h_fakePhi = new TH1D("h_fakePhi", "N fakes", 60, -TMath::Pi(), TMath::Pi());
    TH1D * h_fake_NN = new TH1D("h_fake_NN", " N fakes", 50, 0., 100000.);
    TH1D * h_fakeNNEta = new TH1D("h_fakeNNEta", " N fakes ", 60, -3, 3);
    TH1D * h_fakeNNPhi = new TH1D("h_fakeNNPhi", " N fakes ", 60, -TMath::Pi(), TMath::Pi());
    TH1D * h_fakeEta_low = new TH1D("h_fakeEta_low", " N fakes ", 60, -3, 3);
    TH1D * h_fakePhi_low = new TH1D("h_fakePhi_low", " N fakes ", 60, -TMath::Pi(), TMath::Pi());
    TH1D * h_fakeEta_high = new TH1D("h_fakeEta_high", " N fakes ", 60, -3, 3);
    TH1D * h_fakePhi_high = new TH1D("h_fakePhi_high", " N fakes ", 60, -TMath::Pi(), TMath::Pi());
    TH1D * h_fakeNNEta_low = new TH1D("h_fakeNNEta_low", " N fakes ", 60, -3, 3);
    TH1D * h_fakeNNPhi_low = new TH1D("h_fakeNNPhi_low", " N fakes ", 60, -TMath::Pi(), TMath::Pi());
    TH1D * h_fakeNNEta_high = new TH1D("h_fakeNNEta_high", " N fakes ", 60, -3, 3);
    TH1D * h_fakeNNPhi_high = new TH1D("h_fakeNNPhi_high", " N fakes ", 60, -TMath::Pi(), TMath::Pi());

    TH1D * h_fake_NN_eff = new TH1D("h_fake_NN_eff", " N fakes ", 50, 0., 100000.);
    TH1D * h_fake_NN_eff_eta = new TH1D("h_fake_NN_eff_eta", " N fakes ", 60, -3, 3);
    TH1D * h_fake_NN_eff_phi = new TH1D("h_fake_NN_eff_phi", " N fakes ", 60, -TMath::Pi(), TMath::Pi());
    TH1D * h_fake_NN_eff_eta_low = new TH1D("h_fake_NN_eff_eta_low", " N fakes pt<5 GeV", 60, -3, 3);
    TH1D * h_fake_NN_eff_phi_low = new TH1D("h_fake_NN_eff_phi_low", " N fakes pt<5 GeV", 60, -TMath::Pi(), TMath::Pi());
    TH1D * h_fake_NN_eff_eta_high = new TH1D("h_fake_NN_eff_eta_high", " N fakes pt>5 GeV", 60, -3, 3);
    TH1D * h_fake_NN_eff_phi_high = new TH1D("h_fake_NN_eff_phi_high", " N fakes pt>5 GeV", 60, -TMath::Pi(), TMath::Pi());

    TH1D * h_muon_aod = new TH1D("h_muon_aod", " N muons", 50, 0., 100000.);
    TH1D * h_muon_eta_aod = new TH1D("h_muon_eta_aod", "N muons", 60, -3, 3);
    TH1D * h_muon_phi_aod = new TH1D("h_muon_phi_aod", "N muons", 60, -TMath::Pi(), TMath::Pi());
    TH1D * h_muon_eta_aod_low = new TH1D("h_muon_eta_aod_low", "N muons", 60, -3, 3);
    TH1D * h_muon_phi_aod_low = new TH1D("h_muon_phi_aod_low", "N muons", 60, -TMath::Pi(), TMath::Pi());
    TH1D * h_muon_eta_aod_high = new TH1D("h_muon_eta_aod_high", "N muons", 60, -3, 3);
    TH1D * h_muon_phi_aod_high = new TH1D("h_muon_phi_aod_high", "N muons", 60, -TMath::Pi(), TMath::Pi());
    TH1D * h_fake_aod = new TH1D("h_fake_aod", " N fakes", 50, 0., 100000.);
    TH1D * h_fake_eta_aod = new TH1D("h_fake_eta_aod", "N fakes", 60, -3, 3);
    TH1D * h_fake_phi_aod = new TH1D("h_fake_phi_aod", "N fakes", 60, -TMath::Pi(), TMath::Pi());
    TH1D * h_fake_eta_aod_low = new TH1D("h_fake_eta_aod_low", "N fakes", 60, -3, 3);
    TH1D * h_fake_phi_aod_low = new TH1D("h_fake_phi_aod_low", "N fakes", 60, -TMath::Pi(), TMath::Pi());
    TH1D * h_fake_eta_aod_high = new TH1D("h_fake_eta_aod_high", "N fakes", 60, -3, 3);
    TH1D * h_fake_phi_aod_high = new TH1D("h_fake_phi_aod_high", "N fakes", 60, -TMath::Pi(), TMath::Pi());

    TH1D * h_muon_eff_new = new TH1D("h_muon_eff_new", " N muons ", 50, 0., 100000.);
    TH1D * h_muon_eff_eta_new = new TH1D("h_muon_eff_eta_new", " N muons  ", 60, -3, 3);
    TH1D * h_muon_eff_phi_new = new TH1D("h_muon_eff_phi_new", " N muons  ", 60, -TMath::Pi(), TMath::Pi());
    TH1D * h_muon_eff_eta_new_low = new TH1D("h_muon_eff_eta_new_low", " N muons pt<5 GeV ", 60, -3, 3);
    TH1D * h_muon_eff_phi_new_low = new TH1D("h_muon_eff_phi_new_low", " N muons pt<5 GeV ", 60, -TMath::Pi(), TMath::Pi());
    TH1D * h_muon_eff_eta_new_high = new TH1D("h_muon_eff_eta_new_high", " N muons pt>5 GeV ", 60, -3, 3);
    TH1D * h_muon_eff_phi_new_high = new TH1D("h_muon_eff_phi_new_high", " N muons pt>5 GeV ", 60, -TMath::Pi(), TMath::Pi());
    TH1D * h_fake_eff_new = new TH1D("h_fake_eff_new", " N fakes ", 50, 0., 100000.);
    TH1D * h_fake_eff_eta_new = new TH1D("h_fake_eff_eta_new", " N fakes ", 60, -3, 3);
    TH1D * h_fake_eff_phi_new = new TH1D("h_fake_eff_phi_new", " N fakes ", 60, -TMath::Pi(), TMath::Pi());
    TH1D * h_fake_eff_eta_new_low = new TH1D("h_fake_eff_eta_new_low", " N fakes pt<5 GeV", 60, -3, 3);
    TH1D * h_fake_eff_phi_new_low = new TH1D("h_fake_eff_phi_new_low", " N fakes pt<5 GeV", 60, -TMath::Pi(), TMath::Pi());
    TH1D * h_fake_eff_eta_new_high = new TH1D("h_fake_eff_eta_new_high", " N fakes pt>5 GeV", 60, -3, 3);
    TH1D * h_fake_eff_phi_new_high = new TH1D("h_fake_eff_phi_new_high", " N fakes pt>5 GeV", 60, -TMath::Pi(), TMath::Pi());

    TTree *Tree1; 
    TChain * chain = new TChain("MUGIRL","");

    chain->Add("singleMuonsPt10.root/MUGIRL");
    
   // chain->Add("pileup_2.mugirl.root/MUGIRL");
  //  chain->Add("pileup_4.mugirl.root/MUGIRL");

    Tree1 = chain;
    DataMuGirl* t = new DataMuGirl(Tree1);
    if (t->fChain == 0) return;
    Int_t nentries = Int_t(t->fChain->GetEntries());

    TTree *Tree2; 
    TChain * chain2 = new TChain("AOD","");


    chain2->Add("singleMuonsPt10.root/AOD");
   
 
    Tree2 = chain2;
    DataAod * t2 = new DataAod(Tree2);
    if (t2->fChain == 0) return;
    Int_t nentries2 = Int_t(t2->fChain->GetEntries());
 
    Int_t nTen = nentries/10;
    Int_t nbytes = 0, nb = 0;

    cout<<" AOD tree entries "<<nentries2<<endl;

//**************************************** AOD TREE

    for (Int_t entry=0; entry<nentries2;entry++) 
    {
   
	Int_t ientry2 = t2->LoadTree(entry);
	if (ientry2 < 0) break;
	int nb2 = t2->fChain->GetEntry(entry);   
	    
        if (abs(t2->PdgID) == 13 && t2->PassNNsel == 1 &&
            t2->NumMdtHits>2 && (t2->InnAngle<999. || t2->MidAngle<999.))
        {
           if (t2->Dof >0) 
              h_muon_chi2->Fill(t2->ChiSq/t2->Dof);
           else 
           {
               h_muon_chi2->Fill(0);
              h_muon_pt->Fill(t2->IDPt);
           }
           h_muon_aod->Fill(t2->IDPt);
           h_muon_eta_aod->Fill(t2->IDEta);
           h_muon_phi_aod->Fill(t2->IDPhi);
           if (t2->IDPt<5000)
           {
              h_muon_eta_aod_low->Fill(t2->IDEta);
              h_muon_phi_aod_low->Fill(t2->IDPhi);
           }
           else 
           { 
           if (t2->Dof >0) {
              h_muon_eta_aod_high->Fill(t2->IDEta);
              h_muon_phi_aod_high->Fill(t2->IDPhi);
           }
           } 
        }
        if (abs(t2->PdgID) != 13 && t2->PassNNsel == 1 &&
            t2->NumMdtHits>2 && (t2->InnAngle<999. || t2->MidAngle<999.))
        {
           if (t2->Dof >0) 
              h_fake_chi2->Fill(t2->ChiSq/t2->Dof);
           else
            { 
              h_fake_chi2->Fill(0);
              h_fake_pt->Fill(t2->IDPt);
           }
           h_fake_aod->Fill(t2->IDPt);
           h_fake_eta_aod->Fill(t2->IDEta);
           h_fake_phi_aod->Fill(t2->IDPhi);
           if (t2->IDPt<5000)
           {
              h_fake_eta_aod_low->Fill(t2->IDEta);
              h_fake_phi_aod_low->Fill(t2->IDPhi);
           }
           else 
           { 
           if (t2->Dof >0) {
              h_fake_eta_aod_high->Fill(t2->IDEta);
              h_fake_phi_aod_high->Fill(t2->IDPhi);
           }
           } 
        }

    }

//**************************** MUGIRL TREE **************

    for (Int_t jentry=0; jentry<nentries;jentry++) 
    { 
	Int_t ientry = t->LoadTree(jentry);
	if (ientry < 0) break;
	nb = t->fChain->GetEntry(jentry);   nbytes += nb;
	    
        if (nTen!=0) if (jentry%nTen==0) cout<<10*(jentry/nTen)<<"%-"<<flush;
	if (jentry==nentries-1) cout<<endl;
    	
			
        NMdt = 0,
        NCscEta = 0,
        NCscPhi = 0,
        NTgcPhi = 0,
        NTgcEta = 0,
        NRpcPhi = 0,
        NRpcEta = 0,
	NTgcRpcPhi = 0,
        NTgcRpcEta = 0,
        NMdtHitsSeg = 0,
        NTgcHitsSeg = 0,
        NRpcHitsSeg = 0,
	NTgcRpcHitsSeg = 0,
        NMdtSeg = 0,
        NTgcSeg = 0,
        NRpcSeg =0,
	NCscSeg =0,
	NTgcRpcSeg = 0,
        pt = 0,
        eta = 0,
        phi = 0,
        deltaEtaDirTgc = 999,
        deltaEtaDirMdt = 999;
int nHitsTGCEta = 0,
    nHitsTGCPhi = 0,
    nHitsRPCEta = 0,
    nHitsRPCPhi = 0,
    nHitsTGCRPCEta = 0,
    nHitsTGCRPCPhi = 0,
    nHitsMDT = 0,
    nSegTgc = 0,
    nSegMdt = 0,
    nSegRpc = 0,
    nSegCsc = 0,
    nSegTgcRpc = 0,
    nSegHitsTgc = 0,
    nSegHitsMdt = 0,
    nSegHitsRpc = 0,
    nSegHitsTgcRpc = 0;
    double nDeltaEtaDirMdt = 99;
    double nDeltaEtaDirTgc = 99;
    for (Int_t iHit = 0; iHit < t->NumHits; iHit++)
    {
        if (t->HitTechnology[iHit] == 2) // Only TGC
        {
            if(t->HitIsEta[iHit]>0)
	    {
               nHitsTGCEta++;
	       nHitsTGCRPCEta++;	
            }
	    else
	    {
               nHitsTGCPhi++;
               nHitsTGCRPCPhi++;
	    }
	}
        if (t->HitTechnology[iHit] == 1) // Only RPC
        {
                    
           if(t->HitIsEta[iHit]>0)
	   {
              nHitsRPCEta++;
	      if (nHitsTGCRPCEta == 0)
	      nHitsTGCRPCEta++;
           }
	   else
	   {
              nHitsRPCPhi++;
	      if (nHitsTGCRPCPhi == 0)
	      nHitsTGCRPCPhi++;
	   }
        }
        if (t->HitTechnology[iHit] == 0) // Only MDT
            nHitsMDT++;                 
        }
        for(Int_t iIsec = 0; iIsec < t->NumIsections; iIsec++)
        {
           if (t->IsectType[iIsec] == 2)
           {
              if (t->IsectOrigTech[iIsec] == 2) //TGC
              {
		 if (t->IsectTargDist[iIsec] != 0)
		 {
		    nSegTgc++;
		    nSegTgcRpc++;
		    iSegTgc = t->IsectSegment[iIsec];
		    nSegHitsTgc = nSegHitsTgc + t->SegHits[iSegTgc];
		    nSegHitsTgcRpc = nSegHitsTgcRpc + t->SegHits[iSegTgc];
	         }
	      }
              if (t->IsectOrigTech[iIsec] == 0) //MDT
              {   
                 nSegMdt++; 
                 int iSegMdt = t->IsectSegment[iIsec];
                 nSegHitsMdt = nSegHitsMdt + t->SegHits[iSegMdt];
              }
              if (t->IsectOrigTech[iIsec] == 1) //RPC
              {    
		  nSegRpc++;
	          if (nSegTgcRpc ==0) nSegTgcRpc++;
                  int iSegRpc = t->IsectSegment[iIsec];
                  nSegHitsRpc = nSegHitsRpc + t->SegHits[iSegRpc];
		  if (nSegHitsTgcRpc == 0)
		  nSegHitsTgcRpc = nSegHitsTgcRpc + t->SegHits[iSegTgc];
              }
	      if (t->IsectOrigTech[iIsec] == 3) //CSC
              {
                  nSegCsc++; 
              }
           }
       }
			
            
            if (typeOfRun == 2)
            {
                NMdt = nHitsMDT;
                NTgcPhi = nHitsTGCPhi;
                NTgcEta = nHitsTGCEta;
                NRpcPhi = nHitsRPCPhi;
                NRpcEta = nHitsRPCEta;
                NTgcRpcPhi = nHitsTGCRPCPhi;
                NTgcRpcEta = nHitsTGCRPCEta;
		NTgcSeg = nSegTgc;
                NMdtSeg = nSegMdt;
                NRpcSeg = nSegRpc;
		NCscSeg = nSegCsc;
		NTgcRpcSeg = nSegTgcRpc;
                NMdtHitsSeg = nSegHitsMdt;
                NTgcHitsSeg = nSegHitsTgc;
                NRpcHitsSeg = nSegHitsRpc;
		NTgcRpcHitsSeg = nSegHitsTgcRpc;
                pt = t->IDTrkPt;
                eta = t->IDTrkEta;
                phi = t->IDTrkPhi;
                deltaEtaDirMdt = nDeltaEtaDirMdt;
                deltaEtaDirTgc = nDeltaEtaDirTgc;

				
                if (fabs(t->TruthPdgId) == 13)
                {
                    h_muon->Fill(t->IDTrkPt);
                    h_muonEta->Fill(t->IDTrkEta);
		    h_muonPhi->Fill(t->IDTrkPhi);
                    if (t->IDTrkPt<5000)
                    {
                       h_muonEta_low->Fill(t->IDTrkEta);
                       h_muonPhi_low->Fill(t->IDTrkPhi);
                    }
                    else 
                    {  
                       h_muonEta_high->Fill(t->IDTrkEta);
                       h_muonPhi_high->Fill(t->IDTrkPhi);
                    } 
		    h_n_mdt_hits ->Fill(NMdt);
    		    h_n_tgcphi_hits ->Fill(NTgcPhi);
    		    h_n_tgceta_hits ->Fill(NTgcEta);
   		    h_n_rpcphi_hits ->Fill(NRpcPhi);
   		    h_n_rpceta_hits ->Fill(NRpcEta);
    		    h_n_mdt_seghits ->Fill(NMdtHitsSeg);
    		    h_n_tgc_seghits ->Fill(NTgcHitsSeg);
    		    h_n_mdt_seg ->Fill(NMdtSeg);
   		    h_n_tgc_seg ->Fill(NTgcSeg);
    		    h_n_rpc_seg ->Fill(NRpcSeg);
                    
                    if(((nHitsTGCEta>1 && nHitsTGCPhi>1) || 
                        (nHitsRPCEta>1 && nHitsRPCPhi>1)) && nHitsMDT>2)
                    {
                        h_muon_hit->Fill(t->IDTrkPt);
                        h_muonHitEta->Fill(fabs(t->IDTrkEta));
                        if(((nHitsTGCEta>2 && nHitsTGCPhi>2) || 
                            (nHitsRPCEta>1 && nHitsRPCPhi>1)) && nHitsMDT>2)
                        {
                            h_muon_hit1->Fill(t->IDTrkPt);
                            h_muonHit1Eta->Fill(fabs(t->IDTrkEta));
                            if(nSegMdt>0 )
                            {
                                h_muon_seg1->Fill(t->IDTrkPt);
                                h_muonSegEta->Fill(fabs(t->IDTrkEta));
                            }
                        }
                    }

                    if(nHitsMDT>2 && (t->InnerAngle<999. || t->MiddleAngle<999.))
                    {
                        if (t->passNN>0)
                        {
                            h_muon_NN->Fill(t->IDTrkPt);
                            h_muonNNEta->Fill(t->IDTrkEta);
			    h_muonNNPhi->Fill(t->IDTrkPhi);
			    h_Pt_Muons->Fill(t->IDTrkPt);
                            if (t->IDTrkPt<5000)
                            {
                                h_muonNNEta_low->Fill(t->IDTrkEta);
                                h_muonNNPhi_low->Fill(t->IDTrkPhi);
                            }
                            else 
                            {  
                                h_muonNNEta_high->Fill(t->IDTrkEta);
                                h_muonNNPhi_high->Fill(t->IDTrkPhi);
                            } 

                        }
                    }
                }
                if (fabs(t->TruthPdgId) != 13)
                {
                    
                    h_fake->Fill(t->IDTrkPt);
                    h_fakeEta->Fill(t->IDTrkEta);
                    h_fakePhi->Fill(t->IDTrkPhi);
                    if (t->IDTrkPt<5000)
                    {
                         h_fakeEta_low->Fill(t->IDTrkEta);
                         h_fakePhi_low->Fill(t->IDTrkPhi);
                     }
                     else 
                     {  
                         h_fakeEta_high->Fill(t->IDTrkEta);
                         h_fakePhi_high->Fill(t->IDTrkPhi);
                      } 
                    
                    if (((nHitsTGCEta>1 && nHitsTGCPhi>1) || (nHitsRPCEta>1 && nHitsRPCPhi>1)) && nHitsMDT>2)
                    {
                        h_fake_hit->Fill(t->IDTrkPt);
                        if (((nHitsTGCEta>2 && nHitsTGCPhi>2) || (nHitsRPCEta>1 && nHitsRPCPhi>1)) && nHitsMDT>2)
                        {
                            h_fake_hit1->Fill(t->IDTrkPt);
                            if (nSegMdt>0)
                            {
                                h_fake_seg1->Fill(t->IDTrkPt);
                            }
                        }
                    }
                    if(nHitsMDT>2 && (t->InnerAngle <999. || t->MiddleAngle<999.))
                    {
                        if (t->passNN >0)
                        {
                            
                            h_fake_NN->Fill(t->IDTrkPt);
                            h_fakeNNEta->Fill(t->IDTrkEta);
                            h_fakeNNPhi->Fill(t->IDTrkPhi);
                            if (t->IDTrkPt<5000)
                            {
                                h_fakeNNEta_low->Fill(t->IDTrkEta);
                                h_fakeNNPhi_low->Fill(t->IDTrkPhi);
                            }
                            else 
                            {  
                                h_fakeNNEta_high->Fill(t->IDTrkEta);
                                h_fakeNNPhi_high->Fill(t->IDTrkPhi);
                            } 
                        }
                    }
                }
			
            //}
        }
    }
    
    gStyle->SetOptStat(111111);
    gStyle->SetOptFit(0);	
    gStyle->SetOptFit(111);
    gStyle->SetFrameBorderMode(0);
    gStyle->SetFrameBorderSize(0);
    gStyle->SetFrameFillColor(0);
    gStyle->SetTitleBorderSize(1);
    gStyle->SetTitleFillColor(0);
    gStyle->SetPadBorderMode(0);
    gStyle->SetStatBorderSize(1);
    gStyle->SetStatColor(0);

    if (typeOfRun == 2)
    {


        double fake = h_fake->GetEntries();
	    cout<<"fakes: "<<fake<<endl;

        double muon = h_muon->GetEntries();
        cout<<"muons: "<<muon<<endl;



        TCanvas  *c1=new TCanvas("c1","window",600,450);
        c1->SetFillColor(0);
        c1->SetBorderMode(0);
        c1->SetBorderSize(1);
        c1->SetFrameFillColor(1);
        c1->SetFrameFillStyle(0);
        c1->SetFillColor(0);
        c1->SetBorderMode(0);
        c1->SetBorderSize(1);
        c1->SetFrameFillColor(1);
        c1->SetFrameFillStyle(0);
    

        TList *list = new TList;
        list->Add(h_muon);
        list->Add(h_fake);
        TH1F *h_MuonsAndFakes = (TH1F*)h_muon->Clone("h_MuonsAndFakes");
        h_MuonsAndFakes->Reset();
        h_MuonsAndFakes->Merge(list);
        
        //h_fake_NN_eff->Sumw2();
        //h_fake_eff_new->Sumw2();

        h_fake_NN_eff->Divide(h_fake_NN,h_fake,1,1);
        h_fake_eff_new->Divide(h_fake_aod,h_fake,1,1);
        h_fake_NN_eff->Scale(1.0);
        h_fake_eff_new->Scale(1.0);
        h_fake_eff_new->SetMinimum(0.0);
        h_fake_eff_new->SetMaximum(0.1);
        h_fake_NN_eff->SetMinimum(0.0);
        h_fake_NN_eff->SetMaximum(0.1);
        h_fake_NN_eff->Draw();
        h_fake_eff_new->SetLineColor(2);
        h_fake_eff_new->Draw("same");
        h_fake_NN_eff->GetYaxis()->SetTitle("Fake");
        h_fake_NN_eff->GetXaxis()->SetTitle("p_{T}(MeV)");
        TLegend *leg = new TLegend();
        leg->AddEntry(h_fake_NN_eff,"Fake rate");
        leg->AddEntry(h_fake_NN_eff,"Fake rate with overlap removal");
        leg->Draw("same");
        c1->SaveAs("FakePt_SingleMuonsPt10.gif");

 
        TCanvas  *c4=new TCanvas("c4","window",600,450);
        c4->SetFillColor(0);
        c4->SetBorderMode(0);
        c4->SetBorderSize(1);
        c4->SetFrameFillColor(1);
        c4->SetFrameFillStyle(0);
        c4->SetFillColor(0);
        c4->SetBorderMode(0);
        c4->SetBorderSize(1);
        c4->SetFrameFillColor(1);
        c4->SetFrameFillStyle(0);

        //h_muon_NN_eff->Sumw2();
        //h_muon_eff_new->Sumw2();

	h_muon_NN_eff->Divide(h_muon_NN,h_muon,1,1);
	h_muon_eff_new->Divide(h_muon_aod,h_muon,1,1);
	h_muon_NN_eff->Scale(1.0);
	h_muon_NN_eff->SetMinimum(0.0);
	h_muon_NN_eff->SetMaximum(1.02);
	h_muon_eff_new->Scale(1.0);
        h_muon_eff_new->SetMinimum(0.0);
        h_muon_eff_new->SetMaximum(1.02);
		
	h_muon_NN_eff->Draw();
	h_muon_eff_new->SetLineColor(2);
	h_muon_eff_new->Draw("same");
	h_muon_NN_eff->GetYaxis()->SetTitle("Efficiency");
	h_muon_NN_eff->GetXaxis()->SetTitle("p_{T}(MeV)");
        TLegend *leg1 = new TLegend();
        leg1->AddEntry(h_muon_NN_eff,"Efficiency");
        leg1->AddEntry(h_muon_NN_eff,"Efficiency with overlap removal");
        leg1->Draw("same");
	c4->SaveAs("EffPt_SingleMuonsPt10.gif");
	c4->SaveAs("EffPt_SingleMuonsPt10.eps");
	//c4->SaveAs("EffPt_zmumu.gif");
	//c4->SaveAs("EffPt_zmumu.eps");
	//c4->SaveAs("EffPt_t1.gif");
	//c4->SaveAs("EffPt_t1.eps");
	//c4->SaveAs("EffPt_pileup.gif");
	//c4->SaveAs("EffPt_pileup.eps");

///////////////////////// eta ////////////////////////////////////
		
	TCanvas  *c2=new TCanvas("c2","window",600,450);   
	c2->SetFillColor(0);
	c2->SetBorderMode(0);
	c2->SetBorderSize(1);
	c2->SetFrameFillColor(1);
	c2->SetFrameFillStyle(0);
	c2->SetBorderMode(0);
	c2->SetBorderSize(1);
	c2->SetFrameFillColor(1);
	c2->SetFrameFillStyle(0);
	c2->SetFillColor(0);
	c2->SetBorderMode(0);
	c2->SetBorderSize(1);
	c2->SetFrameFillColor(1);
	c2->SetFrameFillStyle(0);
	c2->SetTickx(1);
	c2->SetTicky(1);

	h_muon_NN_eff_eta->Divide(h_muonNNEta,h_muonEta,1,1);
	h_fake_NN_eff_eta->Divide(h_fakeNNEta,h_fakeEta,1,1);
	h_muon_NN_eff_eta_low->Divide(h_muonNNEta_low,h_muonEta_low,1,1);
	h_muon_NN_eff_eta_high->Divide(h_muonNNEta_high,h_muonEta_high,1,1);
	h_muon_eff_eta_new->Divide(h_muon_eta_aod,h_muonEta,1,1);
	h_fake_eff_eta_new->Divide(h_fake_eta_aod,h_fakeEta,1,1);
	h_muon_eff_eta_new_low->Divide(h_muon_eta_aod_low,h_muonEta_low,1,1);
	h_muon_eff_eta_new_high->Divide(h_muon_eta_aod_high,h_muonEta_high,1,1);
	h_muon_NN_eff_eta->Scale(1.0);
	h_muon_NN_eff_eta->SetMinimum(0.0);
	h_muon_NN_eff_eta->SetMaximum(1.02);
	h_muon_eff_eta_new->Scale(1.0);
        h_muon_eff_eta_new->SetMinimum(0.0);
        h_muon_eff_eta_new->SetMaximum(1.02);
	h_muon_NN_eff_eta_high->Scale(1.0);
	h_muon_NN_eff_eta_high->SetMinimum(0.0);
	h_muon_NN_eff_eta_high->SetMaximum(1.02);
	h_muon_eff_eta_new_high->Scale(1.0);
        h_muon_eff_eta_new_high->SetMinimum(0.0);
        h_muon_eff_eta_new_high->SetMaximum(1.02);
	h_muon_NN_eff_eta_low->Scale(1.0);
	h_muon_NN_eff_eta_low->SetMinimum(0.0);
	h_muon_NN_eff_eta_low->SetMaximum(1.02);
	h_muon_eff_eta_new_low->Scale(1.0);
        h_muon_eff_eta_new_low->SetMinimum(0.0);
        h_muon_eff_eta_new_low->SetMaximum(1.02);
	h_muon_NN_eff_eta->Draw();
	h_muon_eff_eta_new->SetLineColor(2);
	h_muon_eff_eta_new->Draw("same");
	h_muon_NN_eff_eta->GetYaxis()->SetTitle("Efficiency");
	h_muon_NN_eff_eta->GetXaxis()->SetTitle("#eta");
        leg1->Draw("same");


	c2->SaveAs("EffEta_SingleMuonsPt10.gif");
	c2->SaveAs("EffEta_SingleMuonsPt10.eps");

	TCanvas  *l2=new TCanvas("l2","window",600,450);   
	l2->SetFillColor(0);
	l2->SetBorderMode(0);
	l2->SetBorderSize(1);
	l2->SetFrameFillColor(1);
	l2->SetFrameFillStyle(0);
	l2->SetBorderMode(0);
	l2->SetBorderSize(1);
	l2->SetFrameFillColor(1);
	l2->SetFrameFillStyle(0);
	l2->SetFillColor(0);
	l2->SetBorderMode(0);
	l2->SetBorderSize(1);
	l2->SetFrameFillColor(1);
	l2->SetFrameFillStyle(0);
	l2->SetTickx(1);
	l2->SetTicky(1);
	h_muon_NN_eff_eta_low->Draw();
	h_muon_eff_eta_new_low->SetLineColor(2);
	h_muon_eff_eta_new_low->Draw("same");
	h_muon_NN_eff_eta_low->GetYaxis()->SetTitle("Efficiency");
	h_muon_NN_eff_eta_low->GetXaxis()->SetTitle("#eta");
	//l2->SaveAs("EffEta_lowpt_pileup.gif");
	//l2->SaveAs("EffEta_lowpt_pileup.eps");
	//l2->SaveAs("EffEta_lowpt.gif");
	//l2->SaveAs("EffEta_lowpt.eps");
	//l2->SaveAs("EffEta_lowpt_zmumu.gif");
	//l2->SaveAs("EffEta_lowpt_zmumu.eps");
	//l2->SaveAs("EffEta_lowpt_t1.gif");
	//l2->SaveAs("EffEta_lowpt_t1.eps");
	h_muon_NN_eff_phi->Scale(1.0);
	h_muon_NN_eff_phi->SetMinimum(0.0);
	h_muon_NN_eff_phi->SetMaximum(1.02);

	TCanvas  *h2=new TCanvas("h2","window",600,450);   
	h2->SetFillColor(0);
	h2->SetBorderMode(0);
	h2->SetBorderSize(1);
	h2->SetFrameFillColor(1);
	h2->SetFrameFillStyle(0);
	h2->SetBorderMode(0);
	h2->SetBorderSize(1);
	h2->SetFrameFillColor(1);
	h2->SetFrameFillStyle(0);
	h2->SetFillColor(0);
	h2->SetBorderMode(0);
	h2->SetBorderSize(1);
	h2->SetFrameFillColor(1);
	h2->SetFrameFillStyle(0);
	h2->SetTickx(1);
	h2->SetTicky(1);
	h_muon_NN_eff_eta_high->Draw();
	h_muon_eff_eta_new_high->SetLineColor(2);
	h_muon_eff_eta_new_high->Draw("same");
	h_muon_NN_eff_eta_high->GetYaxis()->SetTitle("Efficiency");
	h_muon_NN_eff_eta_high->GetXaxis()->SetTitle("#eta");
	//h2->SaveAs("EffEta_highpt_pileup.gif");
	//h2->SaveAs("EffEta_highpt_pileup.eps");
	//h2->SaveAs("EffEta_highpt.gif");
	//h2->SaveAs("EffEta_highpt.eps");
	//h2->SaveAs("EffEta_highpt_zmumu.gif");
	//h2->SaveAs("EffEta_highpt_zmumu.eps");
	//h2->SaveAs("EffEta_highpt_t1.gif");
	//h2->SaveAs("EffEta_highpt_t1.eps");
///////////////////////// phi ////////////////////////////////////

	TCanvas  *c3=new TCanvas("c3","window",600,450);   
	c3->SetFillColor(0);
	c3->SetBorderMode(0);
	c3->SetBorderSize(1);
	c3->SetFrameFillColor(1);
	c3->SetFrameFillStyle(0);
	c3->SetFillColor(0);
	c3->SetBorderMode(0);
	c3->SetBorderSize(1);
	c3->SetFrameFillColor(1);
	c3->SetFrameFillStyle(0);
	c3->SetFillColor(0);
	c3->SetBorderMode(0);
	c3->SetBorderSize(1);
	c3->SetFrameFillColor(1);
	c3->SetFrameFillStyle(0);
	c3->SetTickx(1);
	c3->SetTicky(1);

        //h_muon_NN_eff_phi->Sumw2();
       // h_muon_eff_phi_new->Sumw2();
       // h_fake_NN_eff_phi->Sumw2();
       // h_fake_eff_phi_new->Sumw2();

	h_muon_NN_eff_phi->Divide(h_muonNNPhi,h_muonPhi,1,1);
	h_muon_NN_eff_phi_low->Divide(h_muonNNPhi_low,h_muonPhi_low,1,1);
	h_muon_NN_eff_phi_high->Divide(h_muonNNPhi_high,h_muonPhi_high,1,1);
	h_fake_NN_eff_phi->Divide(h_fakeNNPhi,h_fakePhi,1,1);
	h_muon_eff_phi_new->Divide(h_muon_phi_aod,h_muonPhi,1,1);
	h_muon_eff_phi_new_low->Divide(h_muon_phi_aod_low,h_muonPhi_low,1,1);
	h_muon_eff_phi_new_high->Divide(h_muon_phi_aod_high,h_muonPhi_high,1,1);
	h_fake_eff_phi_new->Divide(h_fake_phi_aod,h_fakePhi,1,1);
	h_muon_NN_eff_phi->Scale(1.0);
	h_muon_NN_eff_phi->SetMinimum(0.0);
	h_muon_NN_eff_phi->SetMaximum(1.02);
	h_muon_NN_eff_phi_low->Scale(1.0);
	h_muon_NN_eff_phi_low->SetMinimum(0.0);
	h_muon_NN_eff_phi_low->SetMaximum(1.02);
	h_muon_NN_eff_phi_high->Scale(1.0);
	h_muon_NN_eff_phi_high->SetMinimum(0.0);
	h_muon_NN_eff_phi_high->SetMaximum(1.02);
	h_muon_eff_phi_new->Scale(1.0);
        h_muon_eff_phi_new->SetMinimum(0.0);
        h_muon_eff_phi_new->SetMaximum(1.02);
	h_muon_NN_eff_phi->Draw();
	h_muon_eff_phi_new->SetLineColor(2);
	h_muon_eff_phi_new->Draw("same");
	h_muon_NN_eff_phi->GetYaxis()->SetTitle("Efficiency");
	h_muon_NN_eff_phi->GetXaxis()->SetTitle("#phi");
        leg1->Draw("same");

	//c3->SaveAs("EffPhi_pileup.gif");
	//c3->SaveAs("EffPhi_pileup.eps");
	c3->SaveAs("EffPhi_SingleMuonsPt10.gif");
	c3->SaveAs("EffPhi_SingleMuonsPt10.eps");
	//c3->SaveAs("EffPhi_zmumu.gif");
	//c3->SaveAs("EffPhi_zmumu.eps");
	//c3->SaveAs("EffPhi_t1.gif");
	//c3->SaveAs("EffPhi_t1.eps");

	TCanvas  *l3=new TCanvas("l3","window",600,450);   
	l3->SetFillColor(0);
	l3->SetBorderMode(0);
	l3->SetBorderSize(1);
	l3->SetFrameFillColor(1);
	l3->SetFrameFillStyle(0);
	l3->SetBorderMode(0);
	l3->SetBorderSize(1);
	l3->SetFrameFillColor(1);
	l3->SetFrameFillStyle(0);
	l3->SetFillColor(0);
	l3->SetBorderMode(0);
	l3->SetBorderSize(1);
	l3->SetFrameFillColor(1);
	l3->SetFrameFillStyle(0);
	l3->SetTickx(1);
	l3->SetTicky(1);
	h_muon_NN_eff_phi_low->Draw();
	h_muon_eff_phi_new_low->SetLineColor(2);
	h_muon_eff_phi_new_low->Draw("same");
	h_muon_NN_eff_phi_low->GetYaxis()->SetTitle("Efficiency");
	h_muon_NN_eff_phi_low->GetXaxis()->SetTitle("#phi");
	//l3->SaveAs("EffPhi_lowpt_pileup.gif");
	//l3->SaveAs("EffPhi_lowpt_pileup.eps");
	//l3->SaveAs("EffPhi_lowpt.gif");
	//l3->SaveAs("EffPhi_lowpt.eps");
	//l3->SaveAs("EffPhi_lowpt_zmumu.gif");
	//l3->SaveAs("EffPhi_lowpt_zmumu.eps");
	//l3->SaveAs("EffPhi_lowpt_t1.gif");
	//l3->SaveAs("EffPhi_lowpt_t1.eps");

	TCanvas  *h3=new TCanvas("h3","window",600,450);   
	h3->SetFillColor(0);
	h3->SetBorderMode(0);
	h3->SetBorderSize(1);
	h3->SetFrameFillColor(1);
	h3->SetFrameFillStyle(0);
	h3->SetBorderMode(0);
	h3->SetBorderSize(1);
	h3->SetFrameFillColor(1);
	h3->SetFrameFillStyle(0);
	h3->SetFillColor(0);
	h3->SetBorderMode(0);
	h3->SetBorderSize(1);
	h3->SetFrameFillColor(1);
	h3->SetFrameFillStyle(0);
	h3->SetTickx(1);
	h3->SetTicky(1);
	h_muon_NN_eff_phi_high->Draw();
	h_muon_eff_phi_new_high->SetLineColor(2);
	h_muon_eff_phi_new_high->Draw("same");
	h_muon_NN_eff_phi_high->GetYaxis()->SetTitle("Efficiency");
	h_muon_NN_eff_phi_high->GetXaxis()->SetTitle("#phi");
	//h3->SaveAs("EffPhi_highpt_pileup.gif");
	//h3->saveAs("EffPhi_highpt_pileup.eps");
	//h3->SaveAs("EffPhi_highpt.gif");
	//h3->SaveAs("EffPhi_highpt.eps");
	//h3->SaveAs("EffPhi_highpt_zmumu.gif");
	//h3->SaveAs("EffPhi_highpt_zmumu.eps");
	//h3->SaveAs("EffPhi_highpt_t1.gif");
	//h3->SaveAs("EffPhi_highpt_t1.eps");

        TCanvas  *c5=new TCanvas("c5","window",800,800);
        c5->Divide (3,4);
        c5->cd(1);
        h_n_mdt_hits ->Draw();
        c5->cd(2);
        h_n_tgcphi_hits ->Draw();
        c5->cd(3);
        h_n_tgceta_hits ->Draw();
        c5->cd(4);
        h_n_rpcphi_hits ->Draw();;
        c5->cd(5);
        h_n_rpceta_hits ->Draw();
        c5->cd(6);
        h_n_mdt_seghits ->Draw();
        c5->cd(7);
        h_n_tgc_seghits ->Draw();
        c5->cd(8);
        h_n_mdt_seg ->Draw();
        c5->cd(9);
        h_n_tgc_seg->Draw();
        c5->cd(10);
        h_n_rpc_seg ->Draw();
        c5->SaveAs("NHits.gif");
        c5->SaveAs("NHits.eps");

    }
 
    string name="GlobalFitPerformance_SingleMuonsPt10.ps";
    const char* PsName=name.c_str();
    TPostScript *psFile = new TPostScript(PsName,112);

   TH1D * h_IDQOverPt = new TH1D("h_IDQOverPt", " ID QOverPt ", 50, 0.6, 1.4);
   TH1D * h_RefittedQOverPt = new TH1D("h_RefittedQOverPt", " Refitted QOverPt ", 50, 0.6, 1.4);
   TH1D * h_IDPullQOverPt = new TH1D("h_IDPullQOverPt", " ID Pull QOverPt ", 50, -10, 10);
   TH1D * h_RefittedPullQOverPt = new TH1D("h_RefittedPullQOverPt", " Refitted Pull QOverPt ", 50, -10, 10);
   TH1D * h_IDPhi = new TH1D("h_IDPhi", " IDPhi - truthPhi ", 50, -0.001, 0.001);  
   TH1D * h_RefittedPhi = new TH1D("h_RefittedPhi", " RefittedPhi - truthPhi ", 50, -0.001, 0.001); 
   TH1D * h_IDPullPhi = new TH1D("h_IDPullPhi", " ID Pull Phi ", 50, -10, 10);  
   TH1D * h_RefittedPullPhi = new TH1D("h_RefittedPullPhi", " Refitted Pull Phi ", 50, -10,10); 
   TH1D * h_IDTheta = new TH1D("h_IDTheta", " IDcotTheta - truthcotTheta ", 50, -0.004, 0.004);  
   TH1D * h_RefittedTheta = new TH1D("h_RefittedTheta", " RefittedcotTheta - truthcotTheta ", 50, -0.004, 0.004); 
   TH1D * h_IDPullTheta = new TH1D("h_IDPullTheta", " ID Pull cotTheta ", 50, -10, 10);  
   TH1D * h_RefittedPullTheta = new TH1D("h_RefittedPullTheta", " Refitted Pull cotTheta ", 50, -10,10); 

   TH1D * h_IDd0 = new TH1D("h_IDd0", " IDd0 - truthd0 ", 50, -0.2, 0.2);  
   TH1D * h_Refittedd0 = new TH1D("h_Refittedd0", " Refittedd0 - truthd0 ", 50, -0.2, 0.2);
   TH1D * h_IDpulld0 = new TH1D("h_IDpulld0", " (IDd0 - truthd0)/sqrt(IDTrkcov11) ", 50, -10, 10);  
   TH1D * h_Refittedpulld0 = new TH1D("h_Refittedpulld0", " (Refittedd0 - truthd0)/sqrt(RefittedTrkCov11) ", 50, -10, 10);

   TH1D * h_IDz0 = new TH1D("h_IDz0", " IDz0 - truthz0 ", 50, -0.4, 0.4);  
   TH1D * h_Refittedz0 = new TH1D("h_Refittedz0", " Refittedz0 - truthz0 ", 50, -0.4, 0.4);
   TH1D * h_IDpullz0 = new TH1D("h_IDpullz0", " (IDz0 - truthz0)/sqrt(IDTrkcov22) ", 50, -10, 10);  
   TH1D * h_Refittedpullz0 = new TH1D("h_Refittedpullz0", " (Refittedz0 - truthz0)/sqrt(RefittedTrkCov22) ", 50, -10, 10);

   TH1D * h_IDChi2pr = new TH1D("h_IDChi2pr", " ID Chi2 Probability ", 50, 0, 1.1);
   TH1D * h_RefittedChi2pr = new TH1D("h_RefittedChi2pr", " Refitted Chi2 Probability ", 50, 0, 1.1);
   TH1D * h_IDChi2overDoF = new TH1D("h_IDChi2overDoF", " ID Chi2/DoF ", 50, 0, 5);
   TH1D * h_RefittedChi2overDoF = new TH1D("h_RefittedChi2overDoF", " Refitted Chi2/DoF ", 50, 0, 5);

   TH1D * h_RefittedEtaLowP = new TH1D("h_RefittedEtaLowP", "N muons with Chi2 prob <0.01", 60, -3, 3);
   TH1D * h_RefittedEtaHighChi2 = new TH1D("h_RefittedEtaHighChi2", "N muons with Chi2/DoF>10", 60, -3, 3);
   TH1D * h_RefittedPhiLowP = new TH1D("h_RefittedPhiLowP", "N muons with Chi2 prob <0.01", 60, -TMath::Pi(), TMath::Pi());

   TH1D * h_MSRefittedQOverPt = new TH1D("h_MSRefittedQOverPt", " MSRefitted QOverPt ", 50, 0.6, 1.4);
   TH1D * h_MSRefittedPullQOverPt = new TH1D("h_MSRefittedPullQOverPt", " MSRefitted Pull QOverPt ", 50, -10, 10);
   TH1D * h_MSRefittedPhi = new TH1D("h_MSRefittedPhi", " MSRefittedPhi - truthPhi ", 50, -0.05, 0.05); 
   TH1D * h_MSRefittedPullPhi = new TH1D("h_MSRefittedPullPhi", " MSRefitted Pull Phi ", 50, -10,10); 
   TH1D * h_MSRefittedTheta = new TH1D("h_MSRefittedTheta", " MSRefittedcotTheta - truthcotTheta ", 50, -0.15, 0.15); 
   TH1D * h_MSRefittedPullTheta = new TH1D("h_MSRefittedPullTheta", " MSRefitted Pull cotTheta ", 50, -10,10); 

   TH1D * h_MSRefittedd0 = new TH1D("h_MSRefittedd0", " MSRefittedd0 - truthd0 ", 50, -150, 150);
   TH1D * h_MSRefittedpulld0 = new TH1D("h_MSRefittedpulld0", " (MSRefittedd0 - truthd0)/sqrt(MSRefittedTrkCov11) ", 50, -10, 10);

   TH1D * h_MSRefittedz0 = new TH1D("h_MSRefittedz0", " MSRefittedz0 - truthz0 ", 50, -275, 275);
   TH1D * h_MSRefittedpullz0 = new TH1D("h_MSRefittedpullz0", " (MSRefittedz0 - truthz0)/sqrt(MSRefittedTrkCov22) ", 50, -10, 10);

   TH1D * h_MSRefittedChi2pr = new TH1D("h_MSRefittedChi2pr", " MSRefitted Chi2 Probability ", 50, 0, 1.1);
   TH1D * h_MSRefittedChi2overDoF = new TH1D("h_MSRefittedChi2overDoF", " MSRefitted Chi2/DoF ", 50, 0, 5);

   TH1D * h_MSRefittedEtaLowP = new TH1D("h_MSRefittedEtaLowP", "N muons with Chi2 prob <0.01", 60, -3, 3);
   TH1D * h_MSRefittedEtaHighChi2 = new TH1D("h_MSRefittedEtaHighChi2", "N muons with Chi2/DoF>10", 60, -3, 3);
   TH1D * h_MSRefittedPhiLowP = new TH1D("h_MSRefittedPhiLowP", "N muons with Chi2 prob <0.01", 60, -TMath::Pi(), TMath::Pi());




   TH1D * h_NumMdtHits = new TH1D("h_NumMdtHits", " Number of MDT hits ", 100, 0, 50);
   TH1D * h_NumTgcEtaHits = new TH1D("h_NumTgcEtaHits", " Number of TGC Eta hits ", 30, 0, 30);
   TH1D * h_NumTgcPhiHits = new TH1D("h_NumTgcPhiHits", " Number of TGC Phi hits ", 30, 0, 30);
   TH1D * h_NumRpcEtaHits = new TH1D("h_NumRpcEtaHits", " Number of RPC Eta hits ", 25, 0, 25);
   TH1D * h_NumRpcPhiHits = new TH1D("h_NumRpcPhiHits", " Number of RPC Phi hits ", 25, 0, 25);


   TH1D * h_NumMdtHoles = new TH1D("h_NumMdtHoles", " Number of MDT holes ", 15, 0, 15);
   TH1D * h_NumTgcEtaHoles = new TH1D("h_NumTgcEtaHoles", " Number of TGC Eta holes ", 10, 0, 10);
   TH1D * h_NumTgcPhiHoles = new TH1D("h_NumTgcPhiHoles", " Number of TGC Phi holes ", 10, 0, 10);
   TH1D * h_NumRpcEtaHoles = new TH1D("h_NumRpcEtaHoles", " Number of RPC Eta holes ", 10, 0, 10);
   TH1D * h_NumRpcPhiHoles = new TH1D("h_NumRpcPhiHoles", " Number of RPC Phi holes ", 10, 0, 10);


   TH1D * h_IDpt_nofit = new TH1D("h_IDpt_nofit", " IDTrk pT for candidates without refit ", 100, 0,50000);
   TH1D * h_IDeta_nofit = new TH1D("h_IDeta_nofit", " IDTrk Eta for candidates without refit ", 60, -3.,3.);
   TH1D * h_IDpt_MSnofit = new TH1D("h_IDpt_MSnofit", " IDTrk pT for candidates without MS refit ", 100, 0,50000);
   TH1D * h_IDeta_MSnofit = new TH1D("h_IDeta_MSnofit", " IDTrk Eta for candidates without MS refit ", 60, -3.,3.);
   gStyle->SetOptStat(111111);
   gStyle->SetOptFit(111);

    //QOverP

    psFile->NewPage();
    TCanvas  *d1=new TCanvas("QOverP","QoverP",600,450);

    d1->Divide(3,2);
    d1->cd(1);
    chain->Draw("(IDTrkCharge/IDTrkPt)/(TruthCharge/TruthPt)>>h_IDQOverPt","abs(TruthPdgId)==13&&passNN>0");
    h_IDQOverPt->Fit("gaus","Q","",0.8,1.2);
    d1->cd(2);
    chain->Draw("(RefittedTrkCharge/RefittedTrkPt)/(TruthCharge/TruthPt)>>h_RefittedQOverPt","abs(TruthPdgId)==13&&RefittedTrkPt>0&&passNN>0");
    h_RefittedQOverPt->Fit("gaus","Q","",0.8,1.2);
    d1->cd(3);
    chain->Draw("(MSRefittedTrkCharge/MSRefittedTrkPt)/(TruthCharge/TruthPt)>>h_MSRefittedQOverPt","abs(TruthPdgId)==13&&MSRefittedTrkPt>0&&passNN>0");
    h_MSRefittedQOverPt->Fit("gaus","Q","",0.8,1.2);

    h_IDQOverPt->GetXaxis()->SetTitle("ID QOverPt/Truth QOverPt");
    h_RefittedQOverPt->GetXaxis()->SetTitle("Refitted QOverPt/Truth QOverPt");
    h_MSRefittedQOverPt->GetXaxis()->SetTitle("MSRefitted QOverPt/Truth QOverPt");

//Pull QOverP   
    d1->cd(4);
    chain->Draw("((IDTrkCharge/IDTrkPt)-(TruthCharge/TruthPt))/sqrt(IDTrkCov55)>>h_IDPullQOverPt","abs(TruthPdgId)==13&&passNN>0");
    h_IDPullQOverPt->Fit("gaus","Q","",-5,5);
    d1->cd(5);
    chain->Draw("((RefittedTrkCharge/RefittedTrkPt)-(TruthCharge/TruthPt))/sqrt(RefittedTrkCov55)>>h_RefittedPullQOverPt","abs(TruthPdgId)==13&&RefittedTrkPt>0&&passNN>0");
    h_RefittedPullQOverPt->Fit("gaus","Q","",-5,5);
    d1->cd(6);
    chain->Draw("((MSRefittedTrkCharge/MSRefittedTrkPt)-(TruthCharge/TruthPt))/sqrt(MSRefittedTrkCov55)>>h_MSRefittedPullQOverPt","abs(TruthPdgId)==13&&MSRefittedTrkPt>0&&passNN>0");
    h_MSRefittedPullQOverPt->Fit("gaus","Q","",-5,5);

    h_IDPullQOverPt->GetXaxis()->SetTitle("(ID QoverPt-Truth QOverPt)/sqrt(IDTrkCov55)");
    h_RefittedPullQOverPt->GetXaxis()->SetTitle("(Refitted QoverPt-Truth QOverPt)/sqrt(RefittedTrkCov55)");
    h_MSRefittedPullQOverPt->GetXaxis()->SetTitle("(MSRefitted QoverPt-Truth QOverPt)/sqrt(MSRefittedTrkCov55)");


    d1->Update();
    psFile->NewPage();

    //Phi
    TCanvas  *d2=new TCanvas("Phi","Phi",600,450);
    d2->Divide(3,2);
    d2->cd(1);
    chain->Draw("IDTrkPhi-TruthPhi>>h_IDPhi","abs(TruthPdgId)==13&&passNN>0");
    h_IDPhi->Fit("gaus","Q");
    d2->cd(2);
    chain->Draw("RefittedTrkPhi-TruthPhi>>h_RefittedPhi","abs(TruthPdgId)==13&&RefittedTrkPt>0&&passNN>0");
    h_RefittedPhi->Fit("gaus","Q");
    d2->cd(3);
    chain->Draw("MSRefittedTrkPhi-TruthPhi>>h_MSRefittedPhi","abs(TruthPdgId)==13&&MSRefittedTrkPt>0&&passNN>0");
    h_MSRefittedPhi->Fit("gaus","Q");

    h_IDPhi->GetXaxis()->SetTitle("IDPhi-TruthPhi");
    h_IDPhi->GetXaxis()->SetNdivisions(5);
    h_RefittedPhi->GetXaxis()->SetTitle("RefittedPhi-TruthPhi");
    h_RefittedPhi->GetXaxis()->SetNdivisions(5);
    h_MSRefittedPhi->GetXaxis()->SetTitle("MSRefittedPhi-TruthPhi");
    h_MSRefittedPhi->GetXaxis()->SetNdivisions(5);


    d2->cd(4);
    chain->Draw("(IDTrkPhi-TruthPhi)/sqrt(IDTrkCov33)>>h_IDPullPhi","abs(TruthPdgId)==13&&passNN>0");
    h_IDPullPhi->Fit("gaus","Q","",-5,5);
    d2->cd(5);
    chain->Draw("(RefittedTrkPhi-TruthPhi)/sqrt(RefittedTrkCov33)>>h_RefittedPullPhi","abs(TruthPdgId)==13&&RefittedTrkPt>0&&passNN>0");
    h_RefittedPullPhi->Fit("gaus","Q","",-5,5);
    d2->cd(6);
    chain->Draw("(MSRefittedTrkPhi-TruthPhi)/sqrt(MSRefittedTrkCov33)>>h_MSRefittedPullPhi","abs(TruthPdgId)==13&&MSRefittedTrkPt>0&&passNN>0");
    h_MSRefittedPullPhi->Fit("gaus","Q","",-5,5);

    h_IDPullPhi->GetXaxis()->SetTitle("(ID Phi - Truth Phi)/sqrt(IDTrkCov33)");
    h_RefittedPullPhi->GetXaxis()->SetTitle("(Refitted Phi - Truth Phi)/sqrt(RefittedTrkCov33)");
    h_MSRefittedPullPhi->GetXaxis()->SetTitle("(MSRefitted Phi - Truth Phi)/sqrt(MSRefittedTrkCov33)");

    d2->Update();
    psFile->NewPage();


    //Theta
    TCanvas  *d3=new TCanvas("cotTheta","cotTheta",600,450);
    d3->Divide(3,2);
    d3->cd(1);
    chain->Draw("IDTrkTheta-TruthTheta>>h_IDTheta","abs(TruthPdgId)==13&&passNN>0");
    h_IDTheta->Fit("gaus","Q");
    d3->cd(2);
    chain->Draw("RefittedTrkTheta-TruthTheta>>h_RefittedTheta","abs(TruthPdgId)==13&&RefittedTrkPt>0&&passNN>0");
    h_RefittedTheta->Fit("gaus","Q");
    d3->cd(3);
    chain->Draw("MSRefittedTrkTheta-TruthTheta>>h_MSRefittedTheta","abs(TruthPdgId)==13&&MSRefittedTrkPt>0&&passNN>0");
    h_MSRefittedTheta->Fit("gaus","Q");

    h_IDTheta->GetXaxis()->SetTitle("IDcotTheta-TruthcotTheta");
    h_RefittedTheta->GetXaxis()->SetTitle("RefittedcotTheta-TruthcotTheta");
    h_MSRefittedTheta->GetXaxis()->SetTitle("MSRefittedcotTheta-TruthcotTheta");
    h_IDTheta->GetXaxis()->SetNdivisions(5);
    h_RefittedTheta->GetXaxis()->SetNdivisions(5);
    h_MSRefittedTheta->GetXaxis()->SetNdivisions(5);

    
    d3->cd(4);
    chain->Draw("(IDTrkTheta-TruthTheta)/sqrt(IDTrkCov44)>>h_IDPullTheta","abs(TruthPdgId)==13&&passNN>0");
    h_IDPullTheta->Fit("gaus","Q","",-5,5);
    d3->cd(5);
    chain->Draw("(RefittedTrkTheta-TruthTheta)/sqrt(RefittedTrkCov44)>>h_RefittedPullTheta","abs(TruthPdgId)==13&&RefittedTrkPt>0&&passNN>0");
    h_RefittedPullTheta->Fit("gaus","Q","",-5,5);
    d3->cd(6);
    chain->Draw("(MSRefittedTrkTheta-TruthTheta)/sqrt(MSRefittedTrkCov44)>>h_MSRefittedPullTheta","abs(TruthPdgId)==13&&MSRefittedTrkPt>0&&passNN>0");
    h_MSRefittedPullTheta->Fit("gaus","Q","",-5,5);
    h_IDPullTheta->GetXaxis()->SetTitle("(ID cotTheta - Truth cotTheta)/sqrt(IDTrkCov44)");
    h_RefittedPullTheta->GetXaxis()->SetTitle("(Refitted cotTheta - Truth cotTheta)/sqrt(RefittedTrkCov44)");
    h_MSRefittedPullTheta->GetXaxis()->SetTitle("(MSRefitted cotTheta - Truth cotTheta)/sqrt(MSRefittedTrkCov44)");

    d3->Update();
    psFile->NewPage();

    TCanvas  *d4=new TCanvas("d0","d0",600,450);
    d4->Divide(3,2);
    d4->cd(1);
    chain->Draw("IDTrka0-Trutha0>>h_IDd0","abs(TruthPdgId)==13&&passNN>0");
    h_IDd0->Fit("gaus","Q");
    d4->cd(2);
    chain->Draw("RefittedTrka0-Trutha0>>h_Refittedd0","abs(TruthPdgId)==13&&RefittedTrkPt>0&&passNN>0");
    h_Refittedd0->Fit("gaus","Q");
    d4->cd(3);
    chain->Draw("MSRefittedTrka0-Trutha0>>h_MSRefittedd0","abs(TruthPdgId)==13&&MSRefittedTrkPt>0&&passNN>0");
    h_MSRefittedd0->Fit("gaus","Q");

    h_IDd0->GetXaxis()->SetTitle("IDd0-Truthd0");
    h_Refittedd0->GetXaxis()->SetTitle("Refittedd0-Truthd0");
    h_MSRefittedd0->GetXaxis()->SetTitle("MSRefittedd0-Truthd0");

  
    d4->cd(4);
    chain->Draw("(IDTrka0-Trutha0)/sqrt(IDTrkCov11)>>h_IDpulld0","abs(TruthPdgId)==13&&passNN>0");
    h_IDpulld0->Fit("gaus","Q","",-5,5);
    d4->cd(5);
    chain->Draw("(RefittedTrka0-Trutha0)/sqrt(RefittedTrkCov11)>>h_Refittedpulld0","abs(TruthPdgId)==13&&RefittedTrkPt>0&&passNN>0");
    h_Refittedpulld0->Fit("gaus","Q","",-5,5);
    d4->cd(6);
    chain->Draw("(MSRefittedTrka0-Trutha0)/sqrt(MSRefittedTrkCov11)>>h_MSRefittedpulld0","abs(TruthPdgId)==13&&MSRefittedTrkPt>0&&passNN>0");
    h_MSRefittedpulld0->Fit("gaus","Q","",-5,5);

    h_IDpulld0->GetXaxis()->SetTitle("(IDd0-Truthd0)/sqrt(IDTrkCov11)");
    h_Refittedpulld0->GetXaxis()->SetTitle("(Refittedd0-Truthd0)/sqrt(RefittedTrkCov11)");
    h_MSRefittedpulld0->GetXaxis()->SetTitle("(MSRefittedd0-Truthd0)/sqrt(MSRefittedTrkCov11)");
    d4->Update();
    psFile->NewPage();

    TCanvas  *d5=new TCanvas("z0","z0",600,450);
    d5->Divide(3,2);
    d5->cd(1);
    chain->Draw("IDTrkz0-Truthz0>>h_IDz0","abs(TruthPdgId)==13&&passNN>0");
    h_IDz0->Fit("gaus","Q");
    d5->cd(2);
    chain->Draw("RefittedTrkz0-Truthz0>>h_Refittedz0","abs(TruthPdgId)==13&&RefittedTrkPt>0&&passNN>0");
    h_Refittedz0->Fit("gaus","Q");
    d5->cd(3);
    chain->Draw("MSRefittedTrkz0-Truthz0>>h_MSRefittedz0","abs(TruthPdgId)==13&&MSRefittedTrkPt>0&&passNN>0");
    h_MSRefittedz0->Fit("gaus","Q");
    h_IDz0->GetXaxis()->SetTitle("IDz0-Truthz0");
    h_Refittedz0->GetXaxis()->SetTitle("Refittedz0-Truthz0");
    h_MSRefittedz0->GetXaxis()->SetTitle("MSRefittedz0-Truthz0");

  
    d5->cd(4);
    chain->Draw("(IDTrkz0-Truthz0)/sqrt(IDTrkCov22)>>h_IDpullz0","abs(TruthPdgId)==13&&passNN>0");
    h_IDpullz0->Fit("gaus","Q","",-5,5);
    d5->cd(5);
    chain->Draw("(RefittedTrkz0-Truthz0)/sqrt(RefittedTrkCov22)>>h_Refittedpullz0","abs(TruthPdgId)==13&&RefittedTrkPt>0&&passNN>0");
    h_Refittedpullz0->Fit("gaus","Q","",-5,5);
    d5->cd(6);
    chain->Draw("(MSRefittedTrkz0-Truthz0)/sqrt(MSRefittedTrkCov22)>>h_MSRefittedpullz0","abs(TruthPdgId)==13&&MSRefittedTrkPt>0&&passNN>0");
    h_MSRefittedpullz0->Fit("gaus","Q","",-5,5);
    d5->Update();
    h_IDpullz0->GetXaxis()->SetTitle("(IDz0-Truthz0)/sqrt(IDTrkCov22)");
    h_Refittedpullz0->GetXaxis()->SetTitle("(Refittedz0-Truthz0)/sqrt(RefittedTrkCov22)");
    h_Refittedpullz0->GetXaxis()->SetTitle("(MSRefittedz0-Truthz0)/sqrt(MSRefittedTrkCov22)");

    psFile->NewPage();
    TCanvas  *d6=new TCanvas("Chi2pr","Chi2Pr",600,450);
    d6->Divide(3,1);
    d6->cd(1);
    chain->Draw("IDTrkChi2pr>>h_IDChi2pr","abs(TruthPdgId)==13&&passNN>0");
    d6->cd(2);
    chain->Draw("RefittedTrkChi2pr>>h_RefittedChi2pr","abs(TruthPdgId)==13&&RefittedTrkPt>0&&passNN>0");
    d6->cd(3);
    chain->Draw("MSRefittedTrkChi2pr>>h_MSRefittedChi2pr","abs(TruthPdgId)==13&&MSRefittedTrkPt>0&&passNN>0");
    h_IDChi2pr->GetXaxis()->SetTitle("ID Chi2 Probability");
    h_RefittedChi2pr->GetXaxis()->SetTitle("Refitted Chi2 Probability");
    h_MSRefittedChi2pr->GetXaxis()->SetTitle("MSRefitted Chi2 Probability");
    d6->Update();

    psFile->NewPage();
    TCanvas  *d65=new TCanvas("Chi2overDoF","Chi2overDoF",600,450);
    d65->Divide(3,1);
    d65->cd(1);
    chain->Draw("IDTrkChi2/IDTrkNDofFr>>h_IDChi2overDoF","abs(TruthPdgId)==13&&passNN>0");
    d65->cd(2);
    chain->Draw("RefittedTrkChi2/RefittedTrkNDofFr>>h_RefittedChi2overDoF","abs(TruthPdgId)==13&&RefittedTrkPt>0&&passNN>0");
    d65->cd(3);
    chain->Draw("MSRefittedTrkChi2/MSRefittedTrkNDofFr>>h_MSRefittedChi2overDoF","abs(TruthPdgId)==13&&MSRefittedTrkPt>0&&passNN>0");
    h_IDChi2pr->GetXaxis()->SetTitle("ID Chi2/DoF");
    h_RefittedChi2pr->GetXaxis()->SetTitle("Refitted Chi2/DoF");
    h_MSRefittedChi2pr->GetXaxis()->SetTitle("MSRefitted Chi2/DoF");
    d65->Update();


    psFile->NewPage();
    TCanvas  *d66=new TCanvas("EtaDistrHighChi2","EtaDistrHighChi2",600,450);
    d66->Divide(2,1);
    d66->cd(1);
    chain->Draw("RefittedTrkEta>>h_RefittedEtaHighChi2","abs(TruthPdgId)==13&&RefittedTrkPt>0&&passNN>0&&RefittedTrkChi2/RefittedTrkNDofFr>10");   
    h_RefittedEtaHighChi2->GetXaxis()->SetTitle("Eta (RefittedTrkChi2/RefittedTrkNDofFr>10)");

    d66->cd(2);
    chain->Draw("MSRefittedTrkEta>>h_MSRefittedEtaHighChi2","abs(TruthPdgId)==13&&MSRefittedTrkPt>0&&passNN>0&&MSRefittedTrkChi2/MSRefittedTrkNDofFr>10");   
    h_MSRefittedEtaHighChi2->GetXaxis()->SetTitle("Eta (MSRefittedTrkChi2/RefittedTrkNDofFr>10)");
    d66->Update();
    


    psFile->NewPage();
    TCanvas  *d7=new TCanvas("EtaPhiDistr","EtaPhiDistr",600,450);
    d7->Divide(2,2);
    d7->cd(1);
    chain->Draw("RefittedTrkEta>>h_RefittedEtaLowP","abs(TruthPdgId)==13&&RefittedTrkPt>0&&passNN>0&&RefittedTrkChi2pr<0.01");
    d7->cd(2);
    chain->Draw("MSRefittedTrkEta>>h_MSRefittedEtaLowP","abs(TruthPdgId)==13&&MSRefittedTrkPt>0&&passNN>0&&MSRefittedTrkChi2pr<0.01");
    d7->cd(3);
    chain->Draw("RefittedTrkPhi>>h_RefittedPhiLowP","abs(TruthPdgId)==13&&RefittedTrkPt>0&&passNN>0&&RefittedTrkChi2pr<0.01");
    d7->cd(4);
    chain->Draw("MSRefittedTrkPhi>>h_MSRefittedPhiLowP","abs(TruthPdgId)==13&&MSRefittedTrkPt>0&&passNN>0&&MSRefittedTrkChi2pr<0.01");
    h_RefittedEtaLowP->GetXaxis()->SetTitle("Eta (RefittedTrkChi2pr<0.01)");
    h_MSRefittedEtaLowP->GetXaxis()->SetTitle("Eta (MSRefittedTrkChi2pr<0.01)");
    h_RefittedPhiLowP->GetXaxis()->SetTitle("Phi (RefittedTrkChi2pr<0.01)");
    h_MSRefittedPhiLowP->GetXaxis()->SetTitle("Phi (MSRefittedTrkChi2pr<0.01)");
    d7->Update();

    

    

    psFile->NewPage();
    TCanvas  *d8=new TCanvas("NumHits","Number of Hits",600,450);
    d8->Divide(2,3);
    d8->cd(1);
    chain->Draw("RefittedTrkSummaryNumMdtHits>>h_NumMdtHits","abs(TruthPdgId)==13&&RefittedTrkPt>0&&passNN>0");
    h_NumMdtHits->GetXaxis()->SetTitle("Number of MDT Hits");
    d8->cd(3);
    chain->Draw("RefittedTrkSummaryNumTgcEtaHits>>h_NumTgcEtaHits","abs(TruthPdgId)==13&&RefittedTrkPt>0&&passNN>0");
    h_NumTgcEtaHits->GetXaxis()->SetTitle("Number of TGC Eta Hits");
    
    d8->cd(4);
    chain->Draw("RefittedTrkSummaryNumTgcPhiHits>>h_NumTgcPhiHits","abs(TruthPdgId)==13&&RefittedTrkPt>0&&passNN>0");
    h_NumTgcPhiHits->GetXaxis()->SetTitle("Number of TGC Phi Hits");
    d8->cd(5);
    chain->Draw("RefittedTrkSummaryNumRpcEtaHits>>h_NumRpcEtaHits","abs(TruthPdgId)==13&&RefittedTrkPt>0&&passNN>0");
    h_NumRpcEtaHits->GetXaxis()->SetTitle("Number of Rpc Eta Hits");

    d8->cd(6);
    chain->Draw("RefittedTrkSummaryNumRpcPhiHits>>h_NumRpcPhiHits","abs(TruthPdgId)==13&&RefittedTrkPt>0&&passNN>0");
    h_NumRpcPhiHits->GetXaxis()->SetTitle("Number of Rpc Phi Hits");

    d8->Update();

    psFile->NewPage();
    TCanvas  *d9=new TCanvas("NumHoles","Number of Holes",600,450);
    d9->Divide(2,3);
    d9->cd(1);
    chain->Draw("RefittedTrkSummaryNumMdtHoles>>h_NumMdtHoles","abs(TruthPdgId)==13&&RefittedTrkPt>0&&passNN>0");
    h_NumMdtHoles->GetXaxis()->SetTitle("Number of MDT Holes");
    d9->cd(3);
    chain->Draw("RefittedTrkSummaryNumTgcEtaHoles>>h_NumTgcEtaHoles","abs(TruthPdgId)==13&&RefittedTrkPt>0&&passNN>0");
    h_NumTgcEtaHoles->GetXaxis()->SetTitle("Number of TGC Eta Holes");
    
    d9->cd(4);
    chain->Draw("RefittedTrkSummaryNumTgcPhiHoles>>h_NumTgcPhiHoles","abs(TruthPdgId)==13&&RefittedTrkPt>0&&passNN>0");
    h_NumTgcPhiHoles->GetXaxis()->SetTitle("Number of TGC Phi Holes");
    d9->cd(5);
    chain->Draw("RefittedTrkSummaryNumRpcEtaHoles>>h_NumRpcEtaHoles","abs(TruthPdgId)==13&&RefittedTrkPt>0&&passNN>0");
    h_NumRpcEtaHoles->GetXaxis()->SetTitle("Number of Rpc Eta Holes");

    d9->cd(6);
    chain->Draw("RefittedTrkSummaryNumRpcPhiHoles>>h_NumRpcPhiHoles","abs(TruthPdgId)==13&&RefittedTrkPt>0&&passNN>0");
    h_NumRpcPhiHoles->GetXaxis()->SetTitle("Number of Rpc Phi Holes");

    d9->Update();

    psFile->NewPage();
    TCanvas  *d10=new TCanvas("IDTrackPt","IDTrack pT for fit failure",600,450);
    d10->Divide(2,2);
    d10->cd(1);
    chain->Draw("IDTrkPt>>h_IDpt_nofit","abs(TruthPdgId)==13&&RefittedTrkPt==0&&passNN>0");
    h_IDpt_nofit->GetXaxis()->SetTitle("ID pT (GeV)");
    d10->cd(2);
    chain->Draw("IDTrkPt>>h_IDpt_MSnofit","abs(TruthPdgId)==13&&MSRefittedTrkPt==0&&passNN>0");
    h_IDpt_MSnofit->GetXaxis()->SetTitle("MS Fit Failure, ID pT (GeV)");
    d10->cd(3);
    chain->Draw("IDTrkEta>>h_IDeta_nofit","abs(TruthPdgId)==13&&RefittedTrkPt==0&&passNN>0");
    h_IDeta_nofit->GetXaxis()->SetTitle("ID #eta ");
    d10->cd(4);
    chain->Draw("IDTrkEta>>h_IDeta_MSnofit","abs(TruthPdgId)==13&&MSRefittedTrkPt==0&&passNN>0");
    h_IDeta_MSnofit->GetXaxis()->SetTitle("MS Fit Failure, ID #eta ");
    d10->Update();

    psFile->Close();

   

}
