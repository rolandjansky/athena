#include <vector>
#include <iostream>
#include <fstream>
#include "TTree.h"
#include "TFile.h"
#include "TChain.h"
#include "DataNew.C"
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

void ANN( )
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

    TH1D * h_muon = new TH1D("h_muon", " N muons", 100, 0., 100000.);
    TH1D * h_muonEta = new TH1D("h_muonEta", "N muons", 60, -3, 3);
    TH1D * h_muonPhi = new TH1D("h_muonPhi", "N muons", 60, -TMath::Pi(), TMath::Pi());
    TH1D * h_muon_NN = new TH1D("h_muon_NN", " N muons with NN", 100, 0., 100000.);
    TH1D * h_muonNNEta = new TH1D("h_muonNNEta", " NN selection ", 60, -3, 3);
    TH1D * h_muonNNPhi = new TH1D("h_muonNNPhi", " NN selection ", 60, -TMath::Pi(), TMath::Pi());

    TH1D * h_muon_NN_eff = new TH1D("h_muon_NN_eff", " N muons with NN ", 100, 0., 100000.);
    TH1D * h_muon_NN_eff_eta = new TH1D("h_muon_NN_eff_eta", " N muons with NN ", 60, -3, 3);
    TH1D * h_muon_NN_eff_phi = new TH1D("h_muon_NN_eff_phi", " N muons with NN ", 60, -TMath::Pi(), TMath::Pi());

    TH1D * h_fake = new TH1D("h_fake", " N fakes", 100, 0., 100000.);
    TH1D * h_fakeEta = new TH1D("h_fakeEta", " N fakes", 60, -3, 3);
    TH1D * h_fakePhi = new TH1D("h_fakePhi", "N fakes", 60, -TMath::Pi(), TMath::Pi());
    TH1D * h_fake_NN = new TH1D("h_fake_NN", " N fakes with NN", 100, 0., 100000.);
    TH1D * h_fakeNNEta = new TH1D("h_fakeNNEta", " NN selection ", 60, -3, 3);
    TH1D * h_fakeNNPhi = new TH1D("h_fakeNNPhi", " NN selection ", 60, -TMath::Pi(), TMath::Pi());

    TH1D * h_fake_NN_eff = new TH1D("h_fake_NN_eff", " N muons with NN ", 100, 0., 100000.);
    TH1D * h_fake_NN_eff_eta = new TH1D("h_fake_NN_eff_eta", " N fakes with NN ", 60, -3, 3);
    TH1D * h_fake_NN_eff_phi = new TH1D("h_fake_NN_eff_phi", " N fakes with NN ", 60, -TMath::Pi(), TMath::Pi());

    //TVector3 v1;
    //TVector3 v2;

    TTree *Tree1; 
    TChain * chain = new TChain("MUGIRL","");

    chain->Add("singleMuonsPt100.root/MUGIRL");
    Tree1 = chain;
    DataNew * t = new DataNew(Tree1);
    if (t->fChain == 0) return;
    Int_t nentries = Int_t(t->fChain->GetEntries());

    //if (t->chain == 0) return;
    //Int_t nentries = Int_t(t->chain->GetEntries());
    
    cout<<" Natasha's test: xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"<<endl;
    cout<<" Reading data ..."<<nentries<<endl;
    Int_t nTen = nentries/10;
    Int_t nbytes = 0, nb = 0;
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

                        }
                    }
                }
                if (fabs(t->TruthPdgId) != 13)
                {
                    
                    h_fake->Fill(t->IDTrkPt);
                    h_fakeEta->Fill(t->IDTrkEta);
                    h_fakePhi->Fill(t->IDTrkPhi);
                    
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
                    if(nHitsMDT>2 && (t->InnerAngle <999. && t->MiddleAngle<999.))
                    {
                        if (t->passNN >0)
                        {
                            h_fake_NN->Fill(t->IDTrkPt);
                            h_fakeNNEta->Fill(t->IDTrkEta);
                            h_fakeNNPhi->Fill(t->IDTrkPhi);
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

        h_fake_NN_eff->Divide(h_fake_NN,h_fake,1,1);
        h_fake_NN_eff->Scale(1.0);
        h_fake_NN_eff->SetMinimum(0.0);
        h_fake_NN_eff->SetMaximum(1.02);
        
        h_fake_NN_eff->Draw();
        h_fake_NN_eff->GetYaxis()->SetTitle("Fake");
        h_fake_NN_eff->GetXaxis()->SetTitle("p_{T}(MeV)");

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

	h_muon_NN_eff->Divide(h_muon_NN,h_muon,1,1);
	h_muon_NN_eff->Scale(1.0);
	h_muon_NN_eff->SetMinimum(0.0);
	h_muon_NN_eff->SetMaximum(1.02);
		
	h_muon_NN_eff->Draw();
	h_muon_NN_eff->GetYaxis()->SetTitle("Efficiency");
	h_muon_NN_eff->GetXaxis()->SetTitle("p_{T}(MeV)");

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
	h_muon_NN_eff_eta->Scale(1.0);
	h_muon_NN_eff_eta->SetMinimum(0.0);
	h_muon_NN_eff_eta->SetMaximum(1.02);
	h_muon_NN_eff_eta->Draw();


	c2->SaveAs("EffEta.gif");
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

	h_muon_NN_eff_phi->Divide(h_muonNNPhi,h_muonPhi,1,1);
	h_fake_NN_eff_phi->Divide(h_fakeNNPhi,h_fakePhi,1,1);
	h_muon_NN_eff_phi->Scale(1.0);
	h_muon_NN_eff_phi->SetMinimum(0.0);
	h_muon_NN_eff_phi->SetMaximum(1.02);
	h_muon_NN_eff_phi->Draw();

	c3->SaveAs("EffPhi.gif");


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
    }

    string name="NewMuGirl_NewFitter_SingleMuons_Pt100.ps";
    const char* PsName=name.c_str();
    TPostScript *psFile = new TPostScript(PsName,112);

   TH1D * h_IDQOverPt = new TH1D("h_IDQOverPt", " ID QOverPt ", 50, 0.4, 1.6);
   TH1D * h_RefittedQOverPt = new TH1D("h_RefittedQOverPt", " Refitted QOverPt ", 50, 0.4, 1.6);
   TH1D * h_IDPullQOverPt = new TH1D("h_IDPullQOverPt", " ID Pull QOverPt ", 50, -20, 20);
   TH1D * h_RefittedPullQOverPt = new TH1D("h_RefittedPullQOverPt", " Refitted Pull QOverPt ", 50, -20, 20);
   TH1D * h_IDPhi = new TH1D("h_IDPhi", " IDPhi - truthPhi ", 50, -0.001, 0.001);  
   TH1D * h_RefittedPhi = new TH1D("h_RefittedPhi", " RefittedPhi - truthPhi ", 50, -0.001, 0.001); 
   TH1D * h_IDPullPhi = new TH1D("h_IDPullPhi", " ID Pull Phi ", 50, -10, 10);  
   TH1D * h_RefittedPullPhi = new TH1D("h_RefittedPullPhi", " Refitted Pull Phi ", 50, -10,10); 
   TH1D * h_IDTheta = new TH1D("h_IDTheta", " IDcotTheta - truthcotTheta ", 50, -0.004, 0.004);  
   TH1D * h_RefittedTheta = new TH1D("h_RefittedTheta", " RefittedcotTheta - truthcotTheta ", 50, -0.004, 0.004); 
   TH1D * h_IDPullTheta = new TH1D("h_IDPullTheta", " ID Pull cotTheta ", 50, -20, 20);  
   TH1D * h_RefittedPullTheta = new TH1D("h_RefittedPullTheta", " Refitted Pull cotTheta ", 50, -20,20); 

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




   gStyle->SetOptStat(111111);
   gStyle->SetOptFit(111);

    //QOverP

    psFile->NewPage();
    TCanvas  *d1=new TCanvas("QOverP","QoverP",600,450);

    d1->Divide(2,2);
    d1->cd(1);
    chain->Draw("(IDTrkCharge/IDTrkPt)/(TruthCharge/TruthPt)>>h_IDQOverPt","abs(TruthPdgId)==13");
    d1->cd(2);
    chain->Draw("(RefittedTrkCharge/RefittedTrkPt)/(TruthCharge/TruthPt)>>h_RefittedQOverPt","abs(TruthPdgId)==13&&RefittedTrkPt>0");
    h_IDQOverPt->GetXaxis()->SetTitle("ID QOverPt/Truth QOverPt");
    h_RefittedQOverPt->GetXaxis()->SetTitle("Refitted QOverPt/Truth QOverPt");
    h_IDQOverPt->Fit("gaus");
    //gaus->SetLineColor(2);
    h_RefittedQOverPt->Fit("gaus");
//Pull QOverP

   
    d1->cd(3);
    chain->Draw("((IDTrkCharge/IDTrkPt)-(TruthCharge/TruthPt))/sqrt(IDTrkCov55)>>h_IDPullQOverPt","abs(TruthPdgId)==13");
    d1->cd(4);
    chain->Draw("((RefittedTrkCharge/RefittedTrkPt)-(TruthCharge/TruthPt))/sqrt(RefittedTrkCov55)>>h_RefittedPullQOverPt","abs(TruthPdgId)==13&&RefittedTrkPt>0");
    h_IDPullQOverPt->GetXaxis()->SetTitle("(ID QoverPt-Truth QOverPt)/sqrt(IDTrkCov55)");
    h_RefittedPullQOverPt->GetXaxis()->SetTitle("(Refitted QoverPt-Truth QOverPt)/sqrt(RefittedTrkCov55)");
    h_IDPullQOverPt->Fit("gaus");
   // gaus->SetLineColor(2);
    h_RefittedPullQOverPt->Fit("gaus");
    d1->Update();
    psFile->NewPage();
    //Phi
    TCanvas  *d2=new TCanvas("Phi","Phi",600,450);
    d2->Divide(2,2);
    d2->cd(1);
    chain->Draw("IDTrkPhi-TruthPhi>>h_IDPhi","abs(TruthPdgId)==13");
    d2->cd(2);
    chain->Draw("RefittedTrkPhi-TruthPhi>>h_RefittedPhi","abs(TruthPdgId)==13&&RefittedTrkPt>0");
    h_IDPhi->GetXaxis()->SetTitle("IDPhi-TruthPhi");
    h_IDPhi->GetXaxis()->SetNdivisions(5);
    h_RefittedPhi->GetXaxis()->SetTitle("RefittedPhi-TruthPhi");
    h_RefittedPhi->GetXaxis()->SetNdivisions(5);
    h_IDPhi->Fit("gaus");
    h_RefittedPhi->Fit("gaus");

    d2->cd(3);
    chain->Draw("(IDTrkPhi-TruthPhi)/sqrt(IDTrkCov33)>>h_IDPullPhi","abs(TruthPdgId)==13");
    d2->cd(4);
    chain->Draw("(RefittedTrkPhi-TruthPhi)/sqrt(RefittedTrkCov33)>>h_RefittedPullPhi","abs(TruthPdgId)==13&&RefittedTrkPt>0");
    h_IDPullPhi->GetXaxis()->SetTitle("(ID Phi - Truth Phi)/sqrt(IDTrkCov33)");
    h_RefittedPullPhi->GetXaxis()->SetTitle("(Refitted Phi - Truth Phi)/sqrt(RefittedTrkCov33)");
    h_IDPullPhi->Fit("gaus");
   // gaus->SetLineColor(2);
    h_RefittedPullPhi->Fit("gaus");
    d2->Update();
    psFile->NewPage();


    //Theta
    TCanvas  *d3=new TCanvas("cotTheta","cotTheta",600,450);
    d3->Divide(2,2);
    d3->cd(1);
    chain->Draw("IDTrkTheta-TruthTheta>>h_IDTheta","abs(TruthPdgId)==13");
    d3->cd(2);
    chain->Draw("RefittedTrkTheta-TruthTheta>>h_RefittedTheta","abs(TruthPdgId)==13&&RefittedTrkPt>0");
    h_IDTheta->GetXaxis()->SetTitle("IDcotTheta-TruthcotTheta");
    h_RefittedTheta->GetXaxis()->SetTitle("RefittedcotTheta-TruthcotTheta");
    h_IDTheta->GetXaxis()->SetNdivisions(5);
    h_RefittedTheta->GetXaxis()->SetNdivisions(5);
    h_IDTheta->Fit("gaus");
    h_RefittedTheta->Fit("gaus");


  
    
    d3->cd(3);
    chain->Draw("(IDTrkTheta-TruthTheta)/sqrt(IDTrkCov44)>>h_IDPullTheta","abs(TruthPdgId)==13");
    d3->cd(4);
    chain->Draw("(RefittedTrkTheta-TruthTheta)/sqrt(RefittedTrkCov44)>>h_RefittedPullTheta","abs(TruthPdgId)==13&&RefittedTrkPt>0");
    h_IDPullTheta->GetXaxis()->SetTitle("(ID cotTheta - Truth cotTheta)/sqrt(IDTrkCov44)");
    h_RefittedPullTheta->GetXaxis()->SetTitle("(Refitted cotTheta - Truth cotTheta)/sqrt(RefittedTrkCov44)");
    h_IDPullTheta->Fit("gaus");
    //gaus->SetLineColor(2);
    h_RefittedPullTheta->Fit("gaus");
    d3->Update();
    psFile->NewPage();

    TCanvas  *d4=new TCanvas("d0","d0",600,450);
    d4->Divide(2,2);
    d4->cd(1);
    chain->Draw("IDTrka0-Trutha0>>h_IDd0","abs(TruthPdgId)==13");
    d4->cd(2);
    chain->Draw("RefittedTrka0-Trutha0>>h_Refittedd0","abs(TruthPdgId)==13&&RefittedTrkPt>0");
    h_IDd0->GetXaxis()->SetTitle("IDd0-Truthd0");
    h_Refittedd0->GetXaxis()->SetTitle("Refittedd0-Truthd0");
    h_IDd0->Fit("gaus");
    h_Refittedd0->Fit("gaus");

  
    d4->cd(3);
    chain->Draw("(IDTrka0-Trutha0)/sqrt(IDTrkCov11)>>h_IDpulld0","abs(TruthPdgId)==13");
    d4->cd(4);
    chain->Draw("(RefittedTrka0-Trutha0)/sqrt(RefittedTrkCov11)>>h_Refittedpulld0","abs(TruthPdgId)==13&&RefittedTrkPt>0");
    h_IDpulld0->GetXaxis()->SetTitle("(IDd0-Truthd0)/sqrt(IDTrkCov11)");
    h_Refittedpulld0->GetXaxis()->SetTitle("(Refittedd0-Truthd0)/sqrt(RefittedTrkCov11)");
    h_IDpulld0->Fit("gaus");
    h_Refittedpulld0->Fit("gaus");
    d4->Update();
    psFile->NewPage();

     TCanvas  *d5=new TCanvas("z0","z0",600,450);
    d5->Divide(2,2);
    d5->cd(1);
    chain->Draw("IDTrkz0-Truthz0>>h_IDz0","abs(TruthPdgId)==13");
    d5->cd(2);
    chain->Draw("RefittedTrkz0-Truthz0>>h_Refittedz0","abs(TruthPdgId)==13&&RefittedTrkPt>0");
    h_IDz0->GetXaxis()->SetTitle("IDz0-Truthz0");
    h_Refittedz0->GetXaxis()->SetTitle("Refittedz0-Truthz0");
    h_IDz0->Fit("gaus");
    h_Refittedz0->Fit("gaus");

  
    d5->cd(3);
    chain->Draw("(IDTrkz0-Truthz0)/sqrt(IDTrkCov22)>>h_IDpullz0","abs(TruthPdgId)==13");
    d5->cd(4);
    chain->Draw("(RefittedTrkz0-Truthz0)/sqrt(RefittedTrkCov22)>>h_Refittedpullz0","abs(TruthPdgId)==13&&RefittedTrkPt>0");
    h_IDpullz0->GetXaxis()->SetTitle("(IDz0-Truthz0)/sqrt(IDTrkCov22)");
    h_Refittedpullz0->GetXaxis()->SetTitle("(Refittedz0-Truthz0)/sqrt(RefittedTrkCov22)");
    h_IDpullz0->Fit("gaus");
    h_Refittedpullz0->Fit("gaus");
    d5->Update();

    psFile->NewPage();
    TCanvas  *d6=new TCanvas("Chi2pr","Chi2Pr",600,450);
    d6->Divide(2,1);
    d6->cd(1);
    chain->Draw("IDTrkChi2pr>>h_IDChi2pr","abs(TruthPdgId)==13");
    d6->cd(2);
    chain->Draw("RefittedTrkChi2pr>>h_RefittedChi2pr","abs(TruthPdgId)==13&&RefittedTrkPt>0");
    h_IDChi2pr->GetXaxis()->SetTitle("ID Chi2 Probability");
    h_RefittedChi2pr->GetXaxis()->SetTitle("Refitted Chi2 Probability");
    d6->Update();

    psFile->Close();
   
}
