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


#include "TTree.h"
#include "TBrowser.h"
#include "TH2.h"
#include "TRandom.h"

#ifdef __MAKECINT__
#pragma link C++ class vector<float>+;
#endif

const Float_t MIN_MC_PROB = 0.499;

const Float_t PT_CUT = 500.;

// event selection
const Int_t APPLY_PV_CUT = 1;
const Int_t APPLY_TRIGGER_CUT  = 0;
const Int_t APPLY_ONEGOODTRACK = 1;
//const TRIGL1NAME = 'L1_MBTS_1';

const Int_t APPLY_PV_CORR  = 0;
const Int_t MAX_EVTS = -1;
const Int_t APPLY_TRK_SEL     = 1;

   std::vector<float> *reco=0;
   std::vector<int> *recoh=0;
   TBranch *br;
   TBranch *brh;   
   Float_t aa;
   Float_t aaa;
   
   
bool trkCutsEvtSelection(TTree *L,Int_t evt, Int_t r, Float_t eta){


    //br = L->GetBranch("trk_eta");
    //br->SetAddress(&reco);
    //br->GetEntry(evt);

    // |eta| > 2.5
    if (fabs(eta) > 2.5 ) return 0;
//////////////////////////////////////////////
    brh = L->GetBranch("trk_nPixHits");
    brh->SetAddress(&recoh);
    brh->GetEntry(evt);
    // Pixel hits
    if ( recoh->at(r) <= 1 ) return 0;
    

    brh = L->GetBranch("trk_nPixHoles");
    brh->SetAddress(&recoh);
    brh->GetEntry(evt);
    // Pixel holes
    if ( recoh->at(r) > 0 ) return 0;

    brh = L->GetBranch("trk_nSCTHits");
    brh->SetAddress(&recoh);
    brh->GetEntry(evt);
    // SCT hits
    if ( recoh->at(r) <=7) return 0;

    brh = L->GetBranch("trk_nSCTHoles");
    brh->SetAddress(&recoh);
    brh->GetEntry(evt);
    // SCT holes
    if (recoh->at(r) >= 2 ) return 0;

    br = L->GetBranch("trk_d0_wrtPV");
    br->SetAddress(&reco);
    br->GetEntry(evt);
    if (fabs( reco->at(r) ) >= 1. ) return 0;

    br = L->GetBranch("trk_z0_wrtPV");
    br->SetAddress(&reco);
    br->GetEntry(evt);
	aa = reco->at(r);

    br = L->GetBranch("trk_theta");
    br->SetAddress(&reco);
    br->GetEntry(evt);
	aaa = reco->at(r);

    // z0 sin(theta) cut
    if ( fabs( aa * TMath::Sin( aaa) ) >= 1. ) return 0;

    // all cuts good ==> good track    
    return 1;
}


   std::vector<float> *recot=0;
   std::vector<int> *recoht=0;
   TBranch *brt;
   TBranch *brht;
   
   Float_t aat;
   Float_t aaat;
   
bool trkSelCuts(TTree *L,Int_t evt, Int_t r, Float_t pt){

    //brt = L->GetBranch("trk_pt");
   // brt->SetAddress(&recot);
   // brt->GetEntry(evt); 
    // pT cut
    if (pt < PT_CUT) return 0;

//////////////////////////////////////////////
    brht = L->GetBranch("trk_nPixHoles");
    brht->SetAddress(&recoht);
    brht->GetEntry(evt);
    // Pixel holes
    if ( recoht->at(r) > 0 ) return 0;

    brht = L->GetBranch("trk_nPixHits");
    brht->SetAddress(&recoht);
    brht->GetEntry(evt);
    // Pixel hits
    if ( recoht->at(r) <= 1 ) return 0;

    brht = L->GetBranch("trk_nSCTHits");
    brht->SetAddress(&recoht);
    brht->GetEntry(evt);
    // SCT hits
    if (recoht->at(r) <=7 ) return 0;

    brht = L->GetBranch("trk_nSCTHoles");
    brht->SetAddress(&recoht);
    brht->GetEntry(evt);
    // SCT holes
    if (recoht->at(r) >= 2 ) return 0;

    brt = L->GetBranch("trk_d0_wrtPV");
    brt->SetAddress(&recot);
    brt->GetEntry(evt);
    if (fabs( recot->at(r) ) >= 1. ) return 0;

    brt = L->GetBranch("trk_z0_wrtPV");
    brt->SetAddress(&recot);
    brt->GetEntry(evt);
	aat = recot->at(r);

    brt = L->GetBranch("trk_theta");
    brt->SetAddress(&recot);
    brt->GetEntry(evt);
    aaat = recot->at(r);
    // z0 sin(theta) cut
    if ( fabs( aat * TMath::Sin( aaat) ) >= 1. ) return 0;

    // all cuts good ==> good track    
    return 1;
}


void HIInDetValidation_Fakes_Tight(){


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
  atlasStyle->SetTitleXOffset(1.1);
  atlasStyle->SetTitleYOffset(1.4);

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
  atlasStyle->SetMarkerColor(2);
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


  // TFile file("/home/mateusz/Pulpit/user.mdyndal.test_500MeV_noTRT/user.mdyndal.test_500MeV_noTRT.D3PD._00001.root");
   TFile *file = TFile::Open("InDetTrackD3PD.root","READ");
   
   TTree *T = (TTree*)file->Get("InDetTrackTree");

   histofile = TFile("Fakes-C-Tight.root","recreate");


//////////////////////////////////////////////////////////////////////////////////////////

Int_t binsEta = 30;
Float_t minEta  = -2.5;
Float_t maxEta  =  2.5;
Int_t binsPt  = 30;
Float_t minPt   = 0.;
Float_t maxPt   = 10.0;


TH1F *h_eta_allReco           = new TH1F("h_eta_allReco",       "", binsEta, minEta, maxEta);
TH1F *h_eta_noMatch           = new TH1F("h_eta_noMatch",       "", binsEta, minEta, maxEta);
TH1F *h_eta_fake              = new TH1F("h_eta_fake",          "FakeRatio(#eta)", binsEta, minEta, maxEta);

TH1F *h_pt_allReco            = new TH1F("h_pt_allReco",        "", binsPt, minPt, maxPt);
TH1F *h_pt_noMatch            = new TH1F("h_pt_noMatch",        "", binsPt, minPt, maxPt);
TH1F *h_pt_fake               = new TH1F("h_pt_fake",           "FakeRatio(p_{T})", binsPt, minPt, maxPt);

TH1F *h_phi_allReco           = new TH1F("h_phi_allReco",       "", 30, -4, 4);
TH1F *h_phi_noMatch           = new TH1F("h_phi_noMatch",       "", 30, -4, 4);
TH1F *h_phi_fake              = new TH1F("h_phi_fake",          "FakeRatio(#phi)", 30, -4, 4);

TH1F *h_D0_allReco            = new TH1F("h_D0_allReco",        "", 30, -2, 2);
TH1F *h_D0_noMatch            = new TH1F("h_D0_noMatch",        "", 30, -2, 2);
TH1F *h_D0_fake               = new TH1F("h_D0_fake",           "FakeRatio(d_{0})", 30, -2, 2);

TH1F *h_Z0_allReco            = new TH1F("h_Z0_allReco",        "", 30, -100, 100);
TH1F *h_Z0_noMatch            = new TH1F("h_Z0_noMatch",        "", 30, -100, 100);
TH1F *h_Z0_fake               = new TH1F("h_Z0_fake",           "FakeRatio(z_{0})", 30, -100, 100);


TH2F *hAllReco_etapt          = new TH2F("hAllReco_etapt",      "", binsEta, minEta, maxEta, binsPt, minPt, maxPt);

TH2F *hReco_fakes_etapt       = new TH2F("hReco_fakes_etapt",   "", binsEta, minEta, maxEta, binsPt, minPt, maxPt);

TH1F *hVal_PVz  	      = new TH1F("hVal_PVz",            "", 100, -100, 100);

///////////////////////////////////////////////////////////////////////////////////////////

Int_t counter = 0;
Int_t evtAfterTrigger = 0;
Int_t evtAfterPV      = 0;
Int_t evtAfterGoodTrk = 0;
    	

            	std::vector<int> *m_trk_mc_index=0;
            	T->SetBranchAddress("trk_mc_index",&m_trk_mc_index);
            	
        	std::vector<float> *m_trk_mc_probability=0;
        	T->SetBranchAddress("trk_mc_probability",&m_trk_mc_probability);
        	
		std::vector<float> *m_trk_eta=0;
		T->SetBranchAddress("trk_eta",&m_trk_eta);
		
		std::vector<float> *m_trk_phi=0;
        	T->SetBranchAddress("trk_phi",&m_trk_phi);
        	
		std::vector<float> *m_trk_pt=0;
		T->SetBranchAddress("trk_pt",&m_trk_pt);

        	std::vector<float> *m_trk_d0=0;
        	T->SetBranchAddress("trk_d0",&m_trk_d0);
        	
		std::vector<float> *m_trk_z0=0;
        	T->SetBranchAddress("trk_z0",&m_trk_z0);

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
   
                Int_t nentries = (Int_t)(T->GetEntries());

	        bool ok = 0;
	
/////////////////  MAIN LOOP  ///////////////////////////////////////////
   for (Int_t i=0;i<nentries;i++) {
   
   	T->GetEntry(i);
   
	printf("Processing event: %i\n",counter);
	counter += 1;
	
	// maximum events
	if (counter >= MAX_EVTS && MAX_EVTS != -1) break;
	////////// EVENT SLECTION :
	/////////////////////////
	// 1) L1 Trigger Fired
	/*if (APPLY_TRIGGER_CUT){
        	ok = 0;
        	//  for (name in ch.ctp_decisionItems){
        	if (name == TRIGL1NAME){
                ok = 1;
                break;
                }
        	//if not ok continue;
       		// }
       		
       	}	*/
    	evtAfterTrigger += 1;
   	
	// 2) Require a good Primary Vertex					
	if ( APPLY_PV_CUT && ( (*m_vx_type)[0] == 0 || (*m_vx_nTracks)[0] < 3)) continue;
		
    	evtAfterPV += 1;
    	
	// 3) At least one good track in event
	if (APPLY_ONEGOODTRACK){
		ok = 0;
		
		for ( Int_t r=0; r< m_trk_n; r++ ){
			if ( trkCutsEvtSelection( T, i, r, (*m_trk_eta)[r]) ){
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
    	 
    	/*if (APPLY_PV_CORR){
    	ev_weight = hCorrectPV.GetBinContent( hCorrectPV.FindBin( (*m_vx_z)[0]) );
    	}*/

    	hVal_PVz->Fill( (*m_vx_z)[0],ev_weight);
		
		
	for (Int_t k=0; k<m_trk_n; k++){
    	
	if (APPLY_TRK_SEL && !(trkSelCuts( T, i, k, (*m_trk_pt)[k]))) continue;

        h_phi_allReco->Fill( (*m_trk_phi)[k], ev_weight);
        h_D0_allReco->Fill( (*m_trk_d0)[k], ev_weight);
        h_Z0_allReco->Fill( (*m_trk_z0)[k], ev_weight);
       
        hAllReco_etapt->Fill(  (*m_trk_eta)[k], (*m_trk_pt)[k]/1000., ev_weight  );
        h_pt_allReco->Fill( (*m_trk_pt)[k]/1000., ev_weight);
        h_eta_allReco->Fill(  (*m_trk_eta)[k], ev_weight);


	if ( ( (*m_trk_mc_probability)[k] <= MIN_MC_PROB) ){
       		hReco_fakes_etapt->Fill(  (*m_trk_eta)[k], (*m_trk_pt)[k]/1000., ev_weight  );
       		h_eta_noMatch->Fill(  (*m_trk_eta)[k], ev_weight);
      		h_pt_noMatch->Fill( (*m_trk_pt)[k]/1000., ev_weight);

       		h_phi_noMatch->Fill( (*m_trk_phi)[k], ev_weight);
       		h_D0_noMatch->Fill( (*m_trk_d0)[k], ev_weight);
       		h_Z0_noMatch->Fill( (*m_trk_z0)[k], ev_weight);
	 	
	 		}
	
		}

   }

printf ("events : %i (processed) %i (after trigger) %i (after PV cut) %i (after good trk)\n" ,counter, evtAfterTrigger, evtAfterPV, evtAfterGoodTrk);


h_pt_allReco->Sumw2();
h_pt_noMatch->Sumw2();
h_pt_fake->Divide(h_pt_noMatch,h_pt_allReco,1.0,1.0,"b");
h_pt_fake->GetXaxis()->SetTitle("p_{T} [GeV]");
//h_pt_eff->GetYaxis()->SetTitle("Eff");
//h_pt_eff->SetTitle("p_{T}");

h_phi_allReco->Sumw2();
h_phi_noMatch->Sumw2();
h_phi_fake->Divide(h_phi_noMatch,h_phi_allReco,1.0,1.0,"b");
h_phi_fake->GetXaxis()->SetTitle("#phi");
//h_phi_eff->GetYaxis()->SetTitle("Eff");
//h_phi_eff->SetTitle("phi");

h_D0_allReco->Sumw2();
h_D0_noMatch->Sumw2();
h_D0_fake->Divide(h_D0_noMatch,h_D0_allReco,1.0,1.0,"b");
h_D0_fake->GetXaxis()->SetTitle("d_{0}");
//h_D0_eff->GetYaxis()->SetTitle("Eff");
//h_D0_eff->SetTitle("d0");

h_eta_allReco->Sumw2();
h_eta_noMatch->Sumw2();
h_eta_fake->Divide(h_eta_noMatch,h_eta_allReco,1.0,1.0,"b");
h_eta_fake->GetXaxis()->SetTitle("#eta");
//h_eta_eff->GetYaxis()->SetTitle("Eff");
//h_eta_eff->SetTitle("#eta");

h_Z0_allReco->Sumw2();
h_Z0_noMatch->Sumw2();
h_Z0_fake->Divide(h_Z0_noMatch,h_Z0_allReco,1.0,1.0,"b");
h_Z0_fake->GetXaxis()->SetTitle("z_{0}");
//h_Z0_eff->GetYaxis()->SetTitle("Eff");
//h_Z0_eff->SetTitle("z0");

TCanvas* c1=new TCanvas();
c1->SetWindowSize(800,600);
c1->Resize();

h_eta_fake->Scale(100);
h_eta_fake->Draw("E");
c1->cd();
c1->SaveAs("Fake_eta_tight.png");

c1->Update();
h_pt_fake->Scale(100);
h_pt_fake->Draw("E");
c1->cd();
c1->SaveAs("Fake_pt_tight.png");

c1->Update();
h_phi_fake->Scale(100);
h_phi_fake->Draw("E");
c1->cd();
c1->SaveAs("Fake_phi_tight.png");

c1->Update();
h_D0_fake->Scale(100);
h_D0_fake->Draw("E");
c1->cd();
c1->SaveAs("Fake_d0_tight.png");
c1->Update();

h_Z0_fake->Scale(100);
h_Z0_fake->Draw("E");
c1->cd();
c1->SaveAs("Fake_z0_tight.png");



Float_t fakr = h_phi_noMatch->GetEntries()/h_phi_allReco->GetEntries();
TString out=""; out+=(fakr*100.); out+=" % +- ";
out+=100.*(sqrt(fakr*(1-fakr)/h_phi_allReco->GetEntries()));
out+=" %";


string line;
Int_t ww=0;

ifstream Myfile("fakes_HIInDetValidation.txt");
if(Myfile.is_open())
{
while(!Myfile.eof())
{
getline(Myfile,line);
if (line =="--------------- Fake ratio summary (tight cuts) ------------") ww=1;
}
Myfile.close();
}
if (ww==0)
{
ofstream myfile("fakes_HIInDetValidation.txt", ios::out | ios::app);
myfile <<"--------------- Fake ratio summary (tight cuts) ------------\n";
myfile <<"Track_Tight fake ratio : " << out << endl;
}


histofile.Write();
histofile.Close();
}
