#include <vector>
#include <iostream>
#include <fstream>
#include "TTree.h"
#include "TChain.h"
#include "Data.C"
#include "TH1.h"

void HitsIntersection( int region=1, int distance=0 )
{
    TH1D * h_deta_id = new TH1D("ID", " ID intersection ", 80, -.07, .07);
    TH1D * h_deta_mdt = new TH1D("MDT", " MDT Intersection ", 80, -.07, .07);
    TH1D * h_deta_tgc = new TH1D("TGC", " TGC Intresection ", 80, -.07, .07);

    TTree *Tree1; 
    TChain * chain = new TChain("MUGIRL",""); 
	chain->Add("bphys_mu6mu3_1.root/MUGIRL");
//    chain->Add("ntuple_muplus_pt20_eta320.forward.root/MUGIRL");
    /*chain->Add("ntuple_mu_minus6GeV_1_1000.root/MUGIRL");
    chain->Add("ntuple_mu_minus6GeV_2_1000.root/MUGIRL");*/

    Tree1 = chain;
    Data * t = new Data(Tree1);
    if (t->fChain == 0) 
        return;
    Int_t nentries = Int_t(t->fChain->GetEntries());
    
    cout<<" Natasha's test: xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"<<endl;
    cout<<" Reading data ..."<<nentries<<endl;
    
    Int_t nTen = nentries/10;
    Int_t nbytes = 0, nb = 0;
    for (Int_t jentry=0; jentry<nentries;jentry++) 
    { 
	    Int_t ientry = t->LoadTree(jentry);
	    if (ientry < 0) break;
	    nb = t->fChain->GetEntry(jentry);   nbytes += nb;
	    
        if (jentry%nTen==0) cout<<10*(jentry/nTen)<<"%-"<<flush;
	    if (jentry==nentries-1) cout<<endl;
    	
        for (Int_t iCand = 0; iCand < t->NumCandidates; iCand++)
        {
            if (t->TruthPt[iCand] == -100)
                continue;
            for(Int_t iHit = 0; iHit < t->NumHits[iCand]; iHit++)
            {
                if (t->HitTechnology[iCand][iHit] == 0) // Only MDT
                {
                    if (t->HitRegion[iCand][iHit] == region)
                    {
                        if (t->HitDistance[iCand][iHit] == distance)
                        {
                            h_deta_id->Fill((t->IsectPosEta[iCand][t->HitFineIsect[iCand][iHit]]-t->HitEta[iCand][iHit]));
                            if (t->HitMdtIsect[iCand][iHit]>=0)
                                h_deta_mdt->Fill((t->IsectPosEta[iCand][t->HitMdtIsect[iCand][iHit]]-t->HitEta[iCand][iHit]));
                            if (t->HitTgcIsect[iCand][iHit]>=0)
                                h_deta_tgc->Fill((t->IsectPosEta[iCand][t->HitTgcIsect[iCand][iHit]]-t->HitEta[iCand][iHit]));
                        }
                    }
                }
            }
        }
    }

    TCanvas  *c1=new TCanvas("c1","window",750,550);
    gStyle->SetOptStat(1);
	gStyle->SetOptFit(0);
	c1->SetFillColor(0);
	c1->SetBorderMode(0);
	c1->SetBorderSize(1);
	c1->SetFrameFillColor(1);
	c1->SetFrameFillStyle(0);
    c1->Divide(1,1);
    c1->cd(1);
    //h_deta_id->Draw(/*"sames"*/);
    //h_deta_id->SetLineWidth(2);
    //h_deta_id->SetLineColor(1);
    //h_deta_id->GetXaxis()->SetTitle("#Delta#eta");
    h_deta_mdt->Draw(/*"sames"*/);
    h_deta_mdt->SetLineWidth(2);
    h_deta_mdt->SetLineColor(2);
    h_deta_id->Draw("sames");
    h_deta_id->SetLineWidth(2);
    h_deta_id->SetLineColor(1);
    h_deta_id->GetXaxis()->SetTitle("#Delta#eta");
    h_deta_tgc->Draw("sames");
    h_deta_tgc->SetLineWidth(2);
    h_deta_tgc->SetLineColor(4);
    h_deta_tgc->GetXaxis()->SetTitle("#Delta#eta");
    
}

