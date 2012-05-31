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

void HIInDetValidation_Eff_Loose(){

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
  atlasStyle->SetOptStat();
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

   histofile = TFile("Histos-Tracking-MCprob-Loose.root","recreate");


//////////////////////////////////////////////////////////////////////////////////////////

Int_t binsEta = 30;
Float_t minEta  = -2.5;
Float_t maxEta  =  2.5;
Int_t binsPt  = 30;
Float_t minPt   = 0.;
Float_t maxPt   = 10.0;


TH1F *numbertrk             = new TH1F("number_of_trk_per_evt", "", 50, 0, 1200);
TH1F *h_eta                     = new TH1F("h_eta",                 "", binsEta, minEta, maxEta);
TH1F *h_eta_truth           = new TH1F("h_eta_truth",           "", binsEta, minEta, maxEta);
TH1F *h_eta_reco            = new TH1F("h_eta_reco",            "", binsEta, minEta, maxEta);
TH1F *h_eta_eff              = new TH1F("h_eta_eff",             "Efficiency(#eta)", binsEta, minEta, maxEta);

TH1F *h_pt                      = new TH1F("h_pt",                  "", binsPt, minPt, maxPt);
TH1F *h_pt_truth            = new TH1F("h_pt_truth",            "", binsPt, minPt, maxPt);
TH1F *h_pt_reco             = new TH1F("h_pt_reco",             "", binsPt, minPt, maxPt);
TH1F *h_pt_eff               = new TH1F("h_pt_eff",             "Efficiency(p_{T})", binsPt, minPt, maxPt);

TH1F *h_phi_truth            = new TH1F("h_phi_truth",          "", 30, -4, 4);
TH1F *h_phi_reco             = new TH1F("h_phi_reco",           "", 30, -4, 4);
TH1F *h_phi_eff              = new TH1F("h_phi_eff",            "Efficiency(#phi)", 30, -4, 4);

TH1F *h_D0_truth            = new TH1F("h_D0_truth",            "", 30, -1.5, 1.5);
TH1F *h_D0_reco             = new TH1F("h_D0_reco",             "", 30, -1.5, 1.5);
TH1F *h_D0_eff              = new TH1F("h_D0_eff",              "Efficiency(d_{0})", 30, -1.5, 1.5);


TH1F *h_Z0_truth            = new TH1F("h_Z0_truth",            "", 30, -10, 10);
TH1F *h_Z0_reco             = new TH1F("h_Z0_reco",             "", 30, -10, 10);
TH1F *h_Z0_eff              = new TH1F("h_Z0_eff",              "Efficiency(z_{0})", 30, -10, 10);


TH2F *hMC_etapt             = new TH2F("hMC_etaphi",            "", binsEta, minEta, maxEta, binsPt, minPt, maxPt);
//hReco_incl_etapt      = ROOT.TH2F("hReco_incl_etaphi",    "", binsEta, minEta, maxEta, binsPt, minPt, maxPt);
TH2F *hReco_matchCone_etapt = new TH2F("hReco_matchCone_etapt", "", binsEta, minEta, maxEta, binsPt, minPt, maxPt);
//hReco_matchHits_etapt = ROOT.TH2F("hReco_matchHits_etapt","", binsEta, minEta, maxEta, binsPt, minPt, maxPt);

TH1F *h_ResEta              = new TH1F("h_ResEta",              "", 50, -0.02, 0.02);
TH1F *h_ResPhi              = new TH1F("h_ResPhi",              "", 50, -0.02, 0.02);
TH1F *h_ResPt              = new TH1F("h_ResPt",                "", 50, -0.2, 0.2);
TH1F *h_ResD0              = new TH1F("h_ResD0",                "", 50, -2, 2);
TH1F *h_ResZ0              = new TH1F("h_ResZ0",                "", 50, -2, 2);


TH1F *h_Pull_phi            = new TH1F("h_Pull_phi",              "", 50, -6, 6);
TH1F *h_Pull_d0            = new TH1F("h_Pull_d0",              "", 50, -6, 6);
TH1F *h_Pull_z0            = new TH1F("h_Pull_z0",              "", 50, -6, 6);
TH1F *h_Pull_qoverp            = new TH1F("h_Pull_qoverp",              "", 50, -6, 6);
TH1F *h_Pull_eta            = new TH1F("h_Pull_eta",              "", 50, -6, 6);

TH1F *hVal_PVz  	   = new TH1F("hVal_PVz",               "", 100, -100, 100);

///////////////////////////////////////////////////////////////////////////////////////////

Int_t counter = 0;
Int_t evtAfterTrigger = 0;
Int_t evtAfterPV      = 0;
Int_t evtAfterGoodTrk = 0;

            	
            	   std::vector<int> *m_mc_gen_barcode=0;
            	  T->SetBranchAddress("mc_gen_barcode",&m_mc_gen_barcode);
            	
            //		   std::vector<float> *reco_main1=0;
		//TBranch *br_main1 = T->GetBranch("mc_gen_pt");
		std::vector<float> *m_mc_gen_pt=0;
		 T->SetBranchAddress("mc_gen_pt",&m_mc_gen_pt);
		
        	
        	//	std::vector<float> *reco_main2=0;
		//TBranch *br_main2 = T->GetBranch("mc_gen_eta");
        	std::vector<float> *m_mc_gen_eta=0;
        	 T->SetBranchAddress("mc_gen_eta",&m_mc_gen_eta);
        	
        	
        	////////////////
        	   //    std::vector<float> *new1=0;
		//TBranch *br_new1 = T->GetBranch("mc_gen_phi");
        	std::vector<float> *m_mc_gen_phi=0;
        	 T->SetBranchAddress("mc_gen_phi",&m_mc_gen_phi);
        	
        	
        	    //   std::vector<float> *new2=0;
		//TBranch *br_new2 = T->GetBranch("mc_perigee_d0");
        	std::vector<float> *m_mc_perigee_d0=0;
        	 T->SetBranchAddress("mc_perigee_d0",&m_mc_perigee_d0);
        	
        
        	 //      std::vector<float> *new3=0;
		//TBranch *br_new3 = T->GetBranch("mc_perigee_z0");
        	std::vector<float> *m_mc_perigee_z0=0;
        	 T->SetBranchAddress("mc_perigee_z0",&m_mc_perigee_z0);
        	
        	//	   std::vector<int> *s2=0;		
		//TBranch *b_destep2 = T->GetBranch("mc_gen_status");
            	std::vector<int> *m_mc_gen_status=0;
            	T->SetBranchAddress("mc_gen_status",&m_mc_gen_status);
            	
            	
            	//	std::vector<float> *reco_main3=0;	
		//TBranch *br_main3 = T->GetBranch("mc_charge");
        	std::vector<float> *m_mc_charge=0;
        	T->SetBranchAddress("mc_charge",&m_mc_charge);
        	
        	
        	//		std::vector<int> *s3=0;
		//TBranch *b_destep3 = T->GetBranch("trk_mc_index");
            	std::vector<int> *m_trk_mc_index=0;
            	T->SetBranchAddress("trk_mc_index",&m_trk_mc_index);
            	
            	
            	   //         	std::vector<float> *reco_main4=0;	
		//TBranch *br_main4 = T->GetBranch("trk_mc_probability");
        	std::vector<float> *m_trk_mc_probability=0;
        	T->SetBranchAddress("trk_mc_probability",&m_trk_mc_probability);
        	
        	
        	 //       	std::vector<float> *reco_main5=0;	
		//TBranch *br_main5 = T->GetBranch("trk_eta");
		std::vector<float> *m_trk_eta=0;
		T->SetBranchAddress("trk_eta",&m_trk_eta);
		
        	
        	//        	std::vector<float> *reco_main6=0;	
		//TBranch *br_main6 = T->GetBranch("trk_phi");
		std::vector<float> *m_trk_phi=0;
        	T->SetBranchAddress("trk_phi",&m_trk_phi);
        	
        	
        	//		std::vector<float> *reco_main7=0;	
		//TBranch *br_main7 = T->GetBranch("mc_gen_phi");
		std::vector<float> *m_mc_gen_phi=0;
        	T->SetBranchAddress("mc_gen_phi",&m_mc_gen_phi);
        	
        	
        	//        	std::vector<float> *reco_main8=0;	
		//TBranch *br_main8 = T->GetBranch("trk_pt");
		std::vector<float> *m_trk_pt=0;
		T->SetBranchAddress("trk_pt",&m_trk_pt);
		
        	
        	 //               std::vector<float> *reco_main9=0;	
		//TBranch *br_main9 = T->GetBranch("mc_gen_pt");
		std::vector<float> *m_mc_gen_pt=0;
		T->SetBranchAddress("mc_gen_pt",&m_mc_gen_pt);
		
        	
        	    //    	std::vector<float> *reco_main10=0;	
		//TBranch *br_main10 = T->GetBranch("trk_d0");
        	std::vector<float> *m_trk_d0=0;
        	T->SetBranchAddress("trk_d0",&m_trk_d0);
        	
        	
        	        //	std::vector<float> *reco_main11=0;	
		//TBranch *br_main11 = T->GetBranch("mc_perigee_d0");
        	std::vector<float> *m_mc_perigee_d0=0;
        	T->SetBranchAddress("mc_perigee_d0",&m_mc_perigee_d0);
        	
        	
        	
        	     //   	std::vector<float> *reco_main12=0;	
		//TBranch *br_main12 = T->GetBranch("trk_z0");
		std::vector<float> *m_trk_z0=0;
        	T->SetBranchAddress("trk_z0",&m_trk_z0);
        	
        	
        	     //           std::vector<float> *reco_main13=0;	
		//TBranch *br_main13 = T->GetBranch("mc_perigee_z0");
		std::vector<float> *m_mc_perigee_z0=0;
		T->SetBranchAddress("mc_perigee_z0",&m_mc_perigee_z0);
		
		Int_t m_trk_n;
		T->SetBranchAddress("trk_n",&m_trk_n);
		
		
		std::vector<int> *m_vx_type=0;
		T->SetBranchAddress("vx_type",&m_vx_type);
		
		std::vector<int> *m_vx_nTracks=0;
		T->SetBranchAddress("vx_nTracks",&m_vx_nTracks);
		
		std::vector<float> *m_vx_z=0;
		T->SetBranchAddress("vx_z",&m_vx_z);
		
		Int_t m_mc_n;
		T->SetBranchAddress("mc_n",&m_mc_n);
		
		std::vector<float> *m_trk_err_phi=0;
		T->SetBranchAddress("trk_err_phi",&m_trk_err_phi);
		
		std::vector<float> *m_trk_err_d0=0;
		T->SetBranchAddress("trk_err_d0",&m_trk_err_d0);
		
		std::vector<float> *m_trk_err_z0=0;
		T->SetBranchAddress("trk_err_z0",&m_trk_err_z0);
		
		std::vector<float> *m_trk_err_qoverp=0;
		T->SetBranchAddress("trk_err_qoverp",&m_trk_err_qoverp);
		
		std::vector<float> *m_trk_err_theta=0;
		T->SetBranchAddress("trk_err_theta",&m_trk_err_theta);
		
		
		std::vector<float> *m_trk_qoverp=0;
		T->SetBranchAddress("trk_qoverp",&m_trk_qoverp);
		
		std::vector<float> *m_mc_perigee_qoverp=0;
		T->SetBranchAddress("mc_perigee_qoverp",&m_mc_perigee_qoverp);
		
///////////////////////////////   
   
                Int_t nentries = (Int_t)(T->GetEntries());

	        bool ok = 0;
	        
	        Int_t x1 = 600;
	        Int_t x2 = 2000;
	        
	       Float_t e1=0, e2=0, e3=0, e4=0, e5=0, e6=0;
	       Int_t n1=0, n2=0, n3=0;	
       
	
/////////////////  MAIN LOOP  ///////////////////////////////////////////
   for (Int_t i=0;i<nentries;i++) {
   
   	T->GetEntry(i);
   
	printf("Processing event: %i\n",counter);
	counter += 1;
	
	  	//b = T->GetBranch("trk_n");
   		//b->SetAddress(&d);
		//b->GetEntry(i); 
		numbertrk->Fill(m_trk_n);
		printf("Tracks: %i\n",m_trk_n);
	
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

    	hVal_PVz->Fill( (*m_vx_z)[0],ev_weight);
    	
	if ( m_trk_n<x1 ) n1++;
	if (  m_trk_n>x1 && m_trk_n<x2 ) n2++;
	if ( m_trk_n>x2 )n3++;
		
	for (Int_t k=0; k<m_mc_n; k++){
	
	// select barcode < 200000 (already done)   	
	if ((*m_mc_gen_barcode)[k] > 200000 | (*m_mc_gen_barcode)[k] == 0 ) continue;
	
	// pT > PT_CUT MeV
        if ((*m_mc_gen_pt)[k] < PT_CUT) continue;
        
	// |eta| < 0.8
        if (fabs( (*m_mc_gen_eta)[k]) >2.5) continue;	

	// require particle to be stable
	if ((*m_mc_gen_status)[k] !=1) continue;
	
	// charge != 0	
        if ((*m_mc_charge)[k] ==0) continue;

        // cut truth particles with an undefined PDG ID
        //truth_pdg = evt.mc_pdg[t]
        
	        if ( m_trk_n<x1 ) e4++; 
                if (  m_trk_n>=x1 && m_trk_n<x2 )e5++; 
		if ( m_trk_n>=x2 ) e6++; 

        // fill hists
       hMC_etapt->Fill(  (*m_mc_gen_eta)[k], (*m_mc_gen_pt)[k]/1000., ev_weight  );
       h_eta_truth->Fill(  (*m_mc_gen_eta)[k], ev_weight);
       h_pt_truth->Fill( (*m_mc_gen_pt)[k]/1000., ev_weight);

       h_phi_truth->Fill( (*m_mc_gen_phi)[k], ev_weight);
       h_D0_truth->Fill( (*m_mc_perigee_d0)[k], ev_weight);
       h_Z0_truth->Fill( (*m_mc_perigee_z0)[k], ev_weight);

	for (Int_t l=0; l<m_trk_n; l++){
	//printf("MyLog: %f\n",s3->at(l));
	if (k == (*m_trk_mc_index)[l] && ((*m_trk_mc_probability)[l] >= MIN_MC_PROB) && trkCuts(T, i, l,  (*m_trk_pt)[l], (*m_trk_eta)[l]) ) ){
	
        	if ( m_trk_n<x1 ) e1++;
                if (  m_trk_n>=x1 && m_trk_n<x2 ) e2++;
		if ( m_trk_n>=x2 ) e3++;
		
	 	hReco_matchCone_etapt->Fill(  (*m_mc_gen_eta)[k], (*m_mc_gen_pt)[k]/1000., ev_weight );
                h_eta_reco->Fill(  (*m_mc_gen_eta)[k], ev_weight);
                h_pt_reco->Fill( (*m_mc_gen_pt)[k]/1000., ev_weight);
                
                h_phi_reco->Fill( (*m_mc_gen_phi)[k], ev_weight);
                h_D0_reco->Fill( (*m_mc_perigee_d0)[k], ev_weight);
                h_Z0_reco->Fill( (*m_mc_perigee_z0)[k], ev_weight);
                
                                
                h_ResEta->Fill( (*m_trk_eta)[l]- (*m_mc_gen_eta)[k], ev_weight);
                h_ResPhi->Fill( (*m_trk_phi)[l]-(*m_mc_gen_phi)[k], ev_weight);
                h_ResPt->Fill( (*m_trk_pt)[l]/1000.-(*m_mc_gen_pt)[k]/1000., ev_weight);
                h_ResD0->Fill( (*m_trk_d0)[l]-(*m_mc_perigee_d0)[k], ev_weight);
                h_ResZ0->Fill( (*m_trk_z0)[l]-(*m_mc_perigee_z0)[k], ev_weight);
                
                h_Pull_phi->Fill( ( (*m_trk_phi)[l]-(*m_mc_gen_phi)[k] ) / (*m_trk_err_phi)[l], ev_weight );
                h_Pull_d0->Fill( ( (*m_trk_d0)[l]-(*m_mc_perigee_d0)[k] ) / (*m_trk_err_d0)[l], ev_weight );
                h_Pull_z0->Fill( ( (*m_trk_z0)[l]-(*m_mc_perigee_z0)[k] ) / (*m_trk_err_z0)[l], ev_weight );
                h_Pull_qoverp->Fill( ( (*m_trk_qoverp)[l]-(*m_mc_perigee_qoverp)[k] ) / (*m_trk_err_qoverp)[l], ev_weight );
                h_Pull_eta->Fill( ( (*m_trk_eta)[l]-(*m_mc_gen_eta)[k] ) / ( fabs( - 1./TMath::Sin(      -TMath::Log( TMath::Tan( (*m_trk_eta)[l]/2.) )      ) * (*m_trk_err_theta)[l] ) ), ev_weight );                
	 break;}

		}
	}
   
   }
   
const Float_t bins[] = {10., (Float_t)x1, (Float_t)x2, 10000.};

TH1F *h_number                    = new TH1F("h_number",                 "Efficiency vs # of tracks per evt", 3, bins); 

	if (n3 !=0){
	h_number->Fill(e6/n3, e3/e6);
	h_number->SetBinError(3, (sqrt(e3/e6*(1-e3/e6)/e6))); }
	
	if (n2 !=0){
	h_number->Fill( e5/n2, e2/e5);
	h_number->SetBinError(2, (sqrt(e2/e5*(1-e2/e5)/e5))); }
	
	if (n1 !=0){
	h_number->Fill(e4/n1, e1/e4);
	h_number->SetBinError(1, (sqrt(e1/e4*(1-e1/e4)/e4))); }

	printf ("%f %f  %f  %f\n" ,e4/n1, e5/n2, e6/n3, 10.*e6/n3);

printf ("events : %i (processed) %i (after trigger) %i (after PV cut) %i (after good trk)\n" ,counter-1, evtAfterTrigger, evtAfterPV, evtAfterGoodTrk);


h_eta_truth->Sumw2();
h_eta_reco->Sumw2();
h_eta_eff->Divide(h_eta_reco,h_eta_truth,1.0,1.0,"b");
h_eta_eff->GetXaxis()->SetTitle("#eta");
//h_eta_eff->GetYaxis()->SetTitle("Eff");
//h_eta_eff->SetTitle("#eta");

h_pt_truth->Sumw2();
h_pt_reco->Sumw2();
h_pt_eff->Divide(h_pt_reco,h_pt_truth,1.0,1.0,"b");
h_pt_eff->GetXaxis()->SetTitle("p_{T} [GeV]");
//h_pt_eff->GetYaxis()->SetTitle("Eff");
//h_pt_eff->SetTitle("p_{T}");

h_phi_truth->Sumw2();
h_phi_reco->Sumw2();
h_phi_eff->Divide(h_phi_reco,h_phi_truth,1.0,1.0,"b");
h_phi_eff->GetXaxis()->SetTitle("#phi");
//h_phi_eff->GetYaxis()->SetTitle("Eff");
//h_phi_eff->SetTitle("phi");

h_D0_truth->Sumw2();
h_D0_reco->Sumw2();
h_D0_eff->Divide(h_D0_reco,h_D0_truth,1.0,1.0,"b");
h_D0_eff->GetXaxis()->SetTitle("d_{0}");
//h_D0_eff->GetYaxis()->SetTitle("Eff");
//h_D0_eff->SetTitle("d0");

h_Z0_truth->Sumw2();
h_Z0_reco->Sumw2();
h_Z0_eff->Divide(h_Z0_reco,h_Z0_truth,1.0,1.0,"b");
h_Z0_eff->GetXaxis()->SetTitle("z_{0}");
//h_Z0_eff->GetYaxis()->SetTitle("Eff");
//h_Z0_eff->SetTitle("z0");


TCanvas* c1=new TCanvas();
c1->SetWindowSize(800,600);
c1->Resize();

h_eta_eff->Scale(100);

h_eta_eff->Draw("E");
c1->cd();
c1->SaveAs("Eff_eta_loose.png");

c1->Update();
h_pt_eff->Scale(100);
h_pt_eff->Draw("E");
c1->cd();
c1->SaveAs("Eff_pt_loose.png");

c1->Update();
h_phi_eff->Scale(100);
h_phi_eff->Draw("E");
c1->cd();
c1->SaveAs("Eff_phi_loose.png");

c1->Update();
h_D0_eff->Scale(100);
h_D0_eff->Draw("E");
c1->cd();
c1->SaveAs("Eff_d0_loose.png");
c1->Update();
h_Z0_eff->Scale(100);
h_Z0_eff->Draw("E");
c1->cd();
c1->SaveAs("Eff_z0_loose.png");

////////////////////////////////////////////////////////////////////////////////////////////////////////
c1->Update();
h_ResPhi->GetXaxis()->SetTitle("#phi");
h_ResPhi->Draw("h");
c1->cd();
c1->SaveAs("Res_phi_loose.png");

c1->Update();
h_ResEta->GetXaxis()->SetTitle("#eta");
h_ResEta->Draw("h");
c1->cd();
c1->SaveAs("Res_eta_loose.png");

c1->Update();
h_ResPt->GetXaxis()->SetTitle("p_{T}");
h_ResPt->Draw("h");
c1->cd();
c1->SaveAs("Res_pt_loose.png");

c1->Update();
h_ResD0->GetXaxis()->SetTitle("d_{0}");
h_ResD0->Draw("h");
c1->cd();
c1->SaveAs("Res_d0_loose.png");

c1->Update();
h_ResZ0->GetXaxis()->SetTitle("z_{0}");
h_ResZ0->Draw("h");
c1->cd();
c1->SaveAs("Res_z0_loose.png");
//////////////////////////////////////////////////////////////////////////////////////////////////

c1->Update();
h_Pull_phi->GetXaxis()->SetTitle("#phi");
h_Pull_phi->Draw("h");
c1->cd();
c1->SaveAs("Pull_phi_loose.png");

c1->Update();
h_Pull_eta->GetXaxis()->SetTitle("#eta");
h_Pull_eta->Draw("h");
c1->cd();
c1->SaveAs("Pull_eta_loose.png");

c1->Update();
h_Pull_qoverp->GetXaxis()->SetTitle("q/p");
h_Pull_qoverp->Draw("h");
c1->cd();
c1->SaveAs("Pull_qoverp_loose.png");

c1->Update();
h_Pull_d0->GetXaxis()->SetTitle("d_{0}");
h_Pull_d0->Draw("h");
c1->cd();
c1->SaveAs("Pull_d0_loose.png");

c1->Update();
h_Pull_z0->GetXaxis()->SetTitle("z_{0}");
h_Pull_z0->Draw("h");
c1->cd();
c1->SaveAs("Pull_z0_loose.png");



c1->SetLogx();

c1->Update();

gr = new TGraphErrors(h_number);
gr->Draw("ALP");

h_number->Scale(100);
h_number->GetXaxis()->SetTitle("number of tracks per evt");
h_number->GetYaxis()->SetTitle("Efficiency [%]");

h_number ->Draw("E1");
   
  /*  TLatex Tl;
   Tl.SetTextSize(0.04);
   Tl.SetTextAlign(11);
Tl.DrawLatex(10., 0.8, "<100");
Tl.DrawLatex(85., 1., "100 #leq trk<500");
Tl.DrawLatex(2000., 0.75, "#geq 500");*/


c1->cd();
c1->SaveAs("Eff_number.png");
 
 
Float_t effi = h_phi_reco->GetEntries()/h_phi_truth->GetEntries();
TString out=""; out+=(effi*100.); out+=" % +- ";
out+=100.*(sqrt(effi*(1-effi)/h_phi_truth->GetEntries()));
out+=" %";


string line;
Int_t ww=0;

ifstream Myfile("efficiency_HIInDetValidation.txt");
if(Myfile.is_open())
{
while(!Myfile.eof())
{
getline(Myfile,line);
if (line =="--------------- Efficiency summary (loose cuts) ------------") ww=1;
}
Myfile.close();
}
if (ww==0)
{
ofstream myfile("efficiency_HIInDetValidation.txt", ios::out | ios::app);
myfile <<"--------------- Efficiency summary (loose cuts) ------------\n";
myfile <<"Track_Loose efficiency : " << out << endl;
}
 

histofile.Write();
histofile.Close();
}
