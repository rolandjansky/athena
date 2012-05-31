/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "vector"
#include "stdio.h"
#include "TFile.h"
#include "TChain.h"
#include "TH2F"
#include "TH1F.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TLatex.h"


#include "TTree.h"
#include "TBrowser.h"
#include "TH2.h"
#include "TRandom.h"

#ifdef __MAKECINT__
#pragma link C++ class vector<float>+;
#endif

const Float_t MIN_MC_PROB = 0.5;

const Float_t PT_CUT = 500.;

// event selection
const Int_t APPLY_PV_CUT = 1;
const Int_t APPLY_TRIGGER_CUT  = 0;
const Int_t APPLY_ONEGOODTRACK = 1;
//const TRIGL1NAME = 'L1_MBTS_1';

const Int_t APPLY_PV_CORR  = 0;
const Int_t MAX_EVTS = -1;

   std::vector<float> *reco=0;
   std::vector<int> *recoh=0;
   TBranch *br;
   TBranch *brh;   
   Float_t aa;
   Float_t aaa;
   
   
bool trkCutsEvtSelection(TTree *L, Int_t evt, Int_t r, Float_t pt, Float_t eta){


    // pT > PT_CUT MeV
    if (pt < PT_CUT) return 0;

    // |eta| < 2.5
    if (fabs(eta) > 2.5 ) return 0;
//////////////////////////////////////////////
    brh = L->GetBranch("trk_nPixHits");
    brh->SetAddress(&recoh);
    brh->GetEntry(evt);

    // Pixel hits
    if ( recoh->at(r) < 1 ) return 0;
    // Pixel holes
    //if evt.trk_nPixHoles[r] > 0 : return False

    brh = L->GetBranch("trk_nSCTHits");
    brh->SetAddress(&recoh);
    brh->GetEntry(evt);

    // SCT hits
    if (recoh->at(r) < 6 ) return 0;



    br = L->GetBranch("trk_d0_wrtPV");
    br->SetAddress(&reco);
    br->GetEntry(evt);

    if (fabs( reco->at(r) ) > 1.5 ) return 0;

    br = L->GetBranch("trk_z0_wrtPV");
    br->SetAddress(&reco);
    br->GetEntry(evt);
	aa = reco->at(r);

    br = L->GetBranch("trk_theta");
    br->SetAddress(&reco);
    br->GetEntry(evt);
	aaa = reco->at(r);

    // z0 sin(theta) cut
   if ( fabs( aa * TMath::Sin( aaa) ) > 1.5 ) return 0;

    // all cuts good ==> good track    
    return 1;
}

   std::vector<float> *recot=0;
   std::vector<int> *recoht=0;
   TBranch *brt;
   TBranch *brht;
   
   Float_t aat;
   Float_t aaat;
   
bool trkCuts(TTree *L, Int_t evt, Int_t r, Float_t pt, Float_t eta){


    // pT > PT_CUT MeV
    if (pt < PT_CUT) return 0;

    // |eta| < 2.5
    if (fabs(eta) > 2.5 ) return 0;
//////////////////////////////////////////////
    brht = L->GetBranch("trk_nPixHits");
    brht->SetAddress(&recoht);
    brht->GetEntry(evt);

    // Pixel hits
    if ( recoht->at(r) < 1 ) return 0;
    // Pixel holes
    //if evt.trk_nPixHoles[r] > 0 : return False

    brht = L->GetBranch("trk_nSCTHits");
    brht->SetAddress(&recoht);
    brht->GetEntry(evt);

    // SCT hits
    if (recoht->at(r) < 6 ) return 0;

    brt = L->GetBranch("trk_d0_wrtPV");
    brt->SetAddress(&recot);
    brt->GetEntry(evt);

    if (fabs( recot->at(r) ) > 1.5 ) return 0;

    brt = L->GetBranch("trk_z0_wrtPV");
    brt->SetAddress(&recot);
    brt->GetEntry(evt);
	aat = recot->at(r);

    brt = L->GetBranch("trk_theta");
    brt->SetAddress(&recot);
    brt->GetEntry(evt);
    aaat = recot->at(r);

    // z0 sin(theta) cut
    if ( fabs( aat * TMath::Sin( aaat) ) > 1.5 ) return 0;

    // all cuts good ==> good track    
    return 1;
}

void HIInDetValidation_Vertexing(){

/////////////////ATLAS STYLE//////////////////////////////////////////
TStyle *atlasStyle = new TStyle("ATLAS","Atlas style");

  // use plain black on white colors
  Int_t icol=0; // WHITE
  atlasStyle->SetFrameBorderMode(icol);
  atlasStyle->SetFrameFillColor(icol);
  atlasStyle->SetCanvasBorderMode(icol);
  atlasStyle->SetCanvasColor(icol);
  atlasStyle->SetPadBorderMode(icol);
  atlasStyle->SetPadColor(icol);
  atlasStyle->SetStatColor(icol);
  //atlasStyle->SetFillColor(icol); // don't use: white fill color for *all* objects

  // set the paper & margin sizes
  atlasStyle->SetPaperSize(20,26);

  // set margin sizes
  atlasStyle->SetPadTopMargin(0.10);
  atlasStyle->SetPadRightMargin(0.05);
  atlasStyle->SetPadBottomMargin(0.10);
  atlasStyle->SetPadLeftMargin(0.10);

  // set title offsets (for axis label)
  atlasStyle->SetTitleXOffset(1.2);
  atlasStyle->SetTitleYOffset(1.1);

  // use large fonts
  //Int_t font=72; // Helvetica italics
  Int_t font=42; // Helvetica
  Double_t tsize=0.04;
  atlasStyle->SetTextFont(font);

  atlasStyle->SetTextSize(tsize);
  atlasStyle->SetLabelFont(font,"x");
  atlasStyle->SetTitleFont(font,"x");
  atlasStyle->SetLabelFont(font,"y");
  atlasStyle->SetTitleFont(font,"y");
  atlasStyle->SetLabelFont(font,"z");
  atlasStyle->SetTitleFont(font,"z");
  
  atlasStyle->SetLabelSize(tsize,"x");
  atlasStyle->SetTitleSize(tsize,"x");
  atlasStyle->SetLabelSize(tsize,"y");
  atlasStyle->SetTitleSize(tsize,"y");
  atlasStyle->SetLabelSize(tsize,"z");
  atlasStyle->SetTitleSize(tsize,"z");

  // use bold lines and markers
  atlasStyle->SetMarkerStyle(20);
  atlasStyle->SetMarkerSize(0.9);
  atlasStyle->SetMarkerColor(4);
  atlasStyle->SetHistLineWidth(1.);
  atlasStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes

  // get rid of X error bars 
  //atlasStyle->SetErrorX(0.001);
  // get rid of error bar caps
  atlasStyle->SetEndErrorSize(0.);

  // do not display any of the standard histogram decorations
  atlasStyle->SetOptTitle();
  atlasStyle->SetOptStat(111111);
  //atlasStyle->SetOptStat(0);
  //atlasStyle->SetOptFit(1111);
  atlasStyle->SetOptFit(0);

 // put tick marks on top and RHS of plots
  //atlasStyle->SetPadTickX(1);
 // atlasStyle->SetPadTickY(1);
 
atlasStyle->SetStatY(0.99);                
// Set y-position (fraction of pad size)
atlasStyle->SetStatX(0.99);                
// Set x-position (fraction of pad size)
atlasStyle->SetStatW(0.2);                
// Set width of stat-box (fraction of pad size)
atlasStyle->SetStatH(0.1);                
// Set height of stat-box (fraction of pad size)
atlasStyle->SetLabelOffset(0.008,"X");
atlasStyle->SetLabelOffset(0.008,"Y");
  
  gROOT->SetStyle("ATLAS");
  gROOT->ForceStyle();
///////////////////////////////////////////////////////////////////////////////////////


   TFile *file = TFile::Open("InDetTrackD3PD.root","READ");
   
   TTree *T = (TTree*)file->Get("InDetTrackTree");

   histofile = TFile("Vertexing-Loose.root","recreate");


//////////////////////////////////////////////////////////////////////////////////////////

Int_t binsEta = 30;
Float_t minEta  = -2.5;
Float_t maxEta  =  2.5;
Int_t binsPt  = 30;
Float_t minPt   = 0.;
Float_t maxPt   = 10.0;


TH1F *numbertrk             = new TH1F("number_of_trk_per_evt", "", 50, 0, 1200);


TH1F *h_vx_n             = new TH1F("h_vx_n",              "", 10, 0, 10);

TH1F *h_vx_x  	   = new TH1F("h_vx_x",               "", 50, -0.4, 0.4);
TH1F *h_vx_y  	   = new TH1F("h_vx_y",               "", 50, -0.4, 0.4);
TH1F *h_vx_z  	   = new TH1F("h_vx_z",               "", 50, -100, 100);

TH1F *h_res_vx_x  	   = new TH1F("h_res_vx_x",               "", 50, -0.4, 0.4);
TH1F *h_res_vx_y  	   = new TH1F("h_res_vx_y",               "", 50, -0.4, 0.4);
TH1F *h_res_vx_z  	   = new TH1F("h_res_vx_z",               "", 50, -0.4, 0.4);

TH1F *h_vx_dummy             = new TH1F("h_vx_dummy",              "", 10, 0, 10);

 TH1F *h_trk_n_all      = new TH1F("h_trk_n_all",              "", 20, 0, 20);
 TH1F *h_trk_n_vx      = new TH1F("h_trk_n_vx",              "", 20, 0, 20);
 TH1F *h_trk_n_eff      = new TH1F("h_trk_n_eff",              "", 20, 0, 20);

///////////////////////////////////////////////////////////////////////////////////////////

Int_t counter = 0;
Int_t evtAfterTrigger = 0;
Int_t evtAfterPV      = 0;
Int_t evtAfterGoodTrk = 0;

		Int_t m_trk_n;
		T->SetBranchAddress("trk_n",&m_trk_n);
		
		std::vector<float> *m_trk_eta=0;
		T->SetBranchAddress("trk_eta",&m_trk_eta);

		std::vector<float> *m_trk_pt=0;
		T->SetBranchAddress("trk_pt",&m_trk_pt);

		std::vector<int> *m_vx_type=0;
		T->SetBranchAddress("vx_type",&m_vx_type);
		
		std::vector<int> *m_vx_nTracks=0;
		T->SetBranchAddress("vx_nTracks",&m_vx_nTracks);
		
		std::vector<float> *m_vx_x=0;
		T->SetBranchAddress("vx_x",&m_vx_x);		
		
		std::vector<float> *m_vx_y=0;
		T->SetBranchAddress("vx_y",&m_vx_y);
		
		std::vector<float> *m_vx_z=0;
		T->SetBranchAddress("vx_z",&m_vx_z);
		
		Int_t m_vx_n;
		T->SetBranchAddress("vx_n",&m_vx_n);		
		
		
		
		Int_t m_mc_n;
		T->SetBranchAddress("mc_n",&m_mc_n);

		std::vector<float> *m_mcVx_x=0;
		T->SetBranchAddress( "mcVx_x", &m_mcVx_x );	
		
		std::vector<float> *m_mcVx_y=0;
		T->SetBranchAddress( "mcVx_y", &m_mcVx_y );	
		
		std::vector<float> *m_mcVx_z=0;
		T->SetBranchAddress( "mcVx_z", &m_mcVx_z );	
			

                Int_t nentries = (Int_t)(T->GetEntries());

	        bool ok = 0;
		
/////////////////  MAIN LOOP  ///////////////////////////////////////////
   for (Int_t i=0;i<nentries;i++) {
   
   	T->GetEntry(i);
   
	//printf("Processing event: %i\n",counter);
	counter += 1;
	
		numbertrk->Fill(m_trk_n);
		h_trk_n_all->Fill(m_trk_n);

	if ( m_vx_n == 1){
	  printf("Tracks: %i\n",m_trk_n);
	  h_vx_dummy->Fill( m_trk_n, 1.);
	}
	else{
	  h_trk_n_vx->Fill( m_trk_n, 1.);
	}
		
    	h_vx_n->Fill( m_vx_n, 1.);
	
	// maximum events
	if (counter > MAX_EVTS && MAX_EVTS != -1) break;
	////////// EVENT SLECTION :
	/////////////////////////
	// 1) L1 Trigger Fired
	if (APPLY_TRIGGER_CUT){
        	ok = 0;
        	//  for (name in ch.ctp_decisionItems){
        	if (name == TRIGL1NAME){
                ok = 1;
                break;
                }
        	//if not ok continue;
       		// }     		
       	}	
    	evtAfterTrigger += 1;
					
	if ( APPLY_PV_CUT && ( (*m_vx_type)[0] == 0 || (*m_vx_nTracks)[0] < 3)) continue;
		
    	evtAfterPV += 1;
    	
	// 3) At least one good track in event
	if (APPLY_ONEGOODTRACK){
		ok = 0;
		
		for ( Int_t r=0; r< m_trk_n; r++ ){
			if ( trkCutsEvtSelection( T, i, r, (*m_trk_pt)[r], (*m_trk_eta)[r]) ){
			ok = 1;
			break;
			}
		////////////////////////////////////////////////////////////////
		if (!ok) continue;
		///////////////////////////////////////////////////////
		}
		evtAfterGoodTrk += 1;
	}
	//DONE with Event Selection
	
	//Reweight MC
    	Float_t ev_weight = 1.0;
    	 
    	if (APPLY_PV_CORR){
    	ev_weight = hCorrectPV.GetBinContent( hCorrectPV.FindBin( (*m_vx_z)[0]) );
    	}

	if (m_vx_n != 1){
	  h_vx_x->Fill( (*m_vx_x)[0],ev_weight);
	  h_vx_y->Fill( (*m_vx_y)[0],ev_weight);
	  h_vx_z->Fill( (*m_vx_z)[0],ev_weight);
	  
	  h_res_vx_x->Fill( (*m_mcVx_x)[0] - (*m_vx_x)[0],ev_weight);
	  h_res_vx_y->Fill( (*m_mcVx_y)[0] - (*m_vx_y)[0],ev_weight);
	  h_res_vx_z->Fill( (*m_mcVx_z)[0] - (*m_vx_z)[0],ev_weight);
	}
   
 }

printf ("events : %i (processed) %i (after trigger) %i (after PV cut) %i (after good trk)\n" ,counter, evtAfterTrigger, evtAfterPV, evtAfterGoodTrk);


TCanvas* c1=new TCanvas();
c1->SetWindowSize(800,600);
c1->Resize();

h_trk_n_all->Sumw2();
h_trk_n_vx->Sumw2();
h_trk_n_eff->Divide(h_trk_n_vx,h_trk_n_all,1.0,1.0,"b");
h_trk_n_eff->Scale(100);
h_trk_n_eff->SetMinimum(90);
 h_trk_n_eff->SetMaximum(105);
 h_trk_n_eff->GetXaxis()->SetTitle("n^{sel}_{trk}");
 h_trk_n_eff->GetYaxis()->SetTitle("Vertex efficiency (%)");


h_trk_n_eff->Draw("");
c1->cd();
c1->SaveAs("Vertex_eff.png");

h_vx_x->Draw("h");
c1->cd();
c1->SaveAs("Vertex_vx_x.png");

c1->Update();
h_vx_y->Draw("h");
c1->cd();
c1->SaveAs("Vertex_vx_y.png");

c1->Update();
h_vx_z->Draw("h");
c1->cd();
c1->SaveAs("Vertex_vx_z.png");

c1->Update();
h_vx_n->Draw("h");
c1->cd();
c1->SaveAs("Vertex_vx_n.png");

c1->Update();
h_res_vx_x->Draw("h");
c1->cd();
c1->SaveAs("Vertex_res_vx_x.png");

c1->Update();
h_res_vx_y->Draw("h");
c1->cd();
c1->SaveAs("Vertex_res_vx_y.png");

c1->Update();
h_res_vx_z->Draw("h");
c1->cd();
c1->SaveAs("Vertex_res_vx_z.png");

c1->Update();
h_vx_dummy->Draw("h");
c1->cd();
c1->SaveAs("Vertex_trk_dummy.png");

histofile.Write();
histofile.Close();
}
