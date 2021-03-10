/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "vector"
#include "iostream"
#include "TMath.h"
#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"

float check_phi(const float phi1, const float phi2) {
        float dphi = TMath::Abs( phi1 - phi2 );
        dphi = TMath::Abs( TMath::Pi() - dphi);
        dphi = TMath::Abs( TMath::Pi() - dphi);
	return dphi;
}

void testZ(){


//TFile f("test20161201A/DumpAllCl.root");
TFile f("test20161201C/DumpAllCl.root");
//TFile f("DumpAll.El.root");

f.cd("DumpAll");
TTree* evt = (TTree*)f.Get("DumpAll/Evt");
TTree* selectron = (TTree*)f.Get("DumpAll/SCluster");
TTree* truth = (TTree*)f.Get("DumpAll/Truth");
TTree* l1 = (TTree*)f.Get("DumpAll/L1");


int nevts = evt->GetEntries();
int nevts_sc = selectron->GetEntries();

std::cout << nevts << " " << nevts_sc << std::endl;

float evt_count;
float evt_nvtx;

evt->SetBranchAddress("count",&evt_count);
evt->SetBranchAddress("nVtx",&evt_nvtx);

std::vector<float> *selectron_et(0);
std::vector<float> *selectron_eta(0);
std::vector<float> *selectron_phi(0);
std::vector<float> *selectron_e237(0);
std::vector<float> *selectron_e277(0);
std::vector<float> *selectron_energy(0);
std::vector<float> *selectron_ehad1(0);
std::vector<float> *selectron_e0(0);
std::vector<float> *selectron_e1(0);
std::vector<float> *selectron_e2(0);
std::vector<float> *selectron_e3(0);
std::vector<float> *selectron_wstot(0);
std::vector<float> *selectron_time(0);
selectron->SetBranchAddress("selectron_et",&selectron_et);
selectron->SetBranchAddress("selectron_eta",&selectron_eta);
selectron->SetBranchAddress("selectron_phi",&selectron_phi);
selectron->SetBranchAddress("selectron_e237",&selectron_e237);
selectron->SetBranchAddress("selectron_e277",&selectron_e277);
selectron->SetBranchAddress("selectron_ehad1",&selectron_ehad1);
selectron->SetBranchAddress("selectron_energy",&selectron_energy);
selectron->SetBranchAddress("selectron_e0",&selectron_e0);
selectron->SetBranchAddress("selectron_e1",&selectron_e1);
selectron->SetBranchAddress("selectron_e2",&selectron_e2);
selectron->SetBranchAddress("selectron_e3",&selectron_e3);
selectron->SetBranchAddress("selectron_wstot",&selectron_wstot);
selectron->SetBranchAddress("selectron_time",&selectron_time);

std::vector<float> *truth_pt(0);
std::vector<float> *truth_eta(0);
std::vector<float> *truth_phi(0);
std::vector<float> *truth_pdg(0);
std::vector<float> *truth_barcode(0);
std::vector<float> *truth_charge(0);
truth->SetBranchAddress("truth_pt",&truth_pt);
truth->SetBranchAddress("truth_eta",&truth_eta);
truth->SetBranchAddress("truth_phi",&truth_phi);
truth->SetBranchAddress("truth_pdg",&truth_pdg);
truth->SetBranchAddress("truth_barcode",&truth_barcode);
truth->SetBranchAddress("truth_charge",&truth_charge);

std::vector<float> *l1_et(0);
std::vector<float> *l1_eta(0);
std::vector<float> *l1_phi(0);
std::vector<float> *l1_hadCore(0);
l1->SetBranchAddress("l1_et",&l1_et);
l1->SetBranchAddress("l1_eta",&l1_eta);
l1->SetBranchAddress("l1_phi",&l1_phi);
l1->SetBranchAddress("l1_hadcore",&l1_hadCore);



//TH2F* f3_vs_eta = new TH2F("f3_vs_eta","f3 vs eta",50,0,2.5,40,-0.1,0.3);
TH1F* massZ = new TH1F("massZ","massZ",145,5,150);
TH1F* time_inside = new TH1F("time_inside","time_inside",200,-1.0,9.0);
TH1F* time_outside = new TH1F("time_outside","time_outside",200,-1.0,9.0);
TH2F* mass_time = new TH2F("mass_time","mass versus time",145,5,150,200,-1.0,9.0);

for(int ev=0;ev<nevts;++ev){

        evt->GetEntry(ev);
        selectron->GetEntry(ev);
	truth->GetEntry(ev);
	l1->GetEntry(ev);

	int found_l1(-1);
	std::vector< int > el_idx;
	for(int j=0; j<l1_et->size(); j++) {
		
		if ( l1_et->at(j) < 10000 ) continue;
		//if ( l1_hadCore->at(j) > 2.0 ) continue;
		if ( l1_hadCore->at(j) > 2e3 ) continue;
		found_l1 = j;
		float etaL1 = l1_eta->at(found_l1);
		float phiL1 = l1_phi->at(found_l1);
		for( int l=0; l<selectron_et->size(); l++){
		  if ( TMath::Abs(selectron_eta->at(l) - etaL1 ) > 0.12) continue;
		  if ( check_phi(selectron_phi->at(l), phiL1 ) > 0.12 ) continue;
		  if ( selectron_et->at(l) < 18e3) continue;
		  float rEta_var = selectron_e237->at(l);
                  if ( selectron_e277->at(l) > 0 ) rEta_var /= selectron_e277->at(l);
                  else continue;
		  if ( rEta_var < 0.795 ) continue;
		  el_idx.push_back(l);
		}
	} // l1 part
	if ( el_idx.size() < 2 )  continue;
	for(size_t idx1=0;idx1<el_idx.size();idx1++){
		for(size_t idx2=idx1+1; idx2 < el_idx.size(); idx2++ ){
			size_t i1 = el_idx.at(idx1);
			size_t i2 = el_idx.at(idx2);
			//std::cout << i1 << " " << i2 << " ";
			//std::cout << selectron_et->at(i1) << " " << selectron_eta->at(i1) << " " << selectron_phi->at(i1) << " ";
			//std::cout << selectron_et->at(i2) << " " << selectron_eta->at(i2) << " " << selectron_phi->at(i2) << std::endl;
			TLorentzVector v1,v2;
			v1.SetPtEtaPhiM( selectron_et->at(i1), selectron_eta->at(i1), selectron_phi->at(i1), 0.51 ); // assuming electron mass
			v2.SetPtEtaPhiM( selectron_et->at(i2), selectron_eta->at(i2), selectron_phi->at(i2), 0.51 ); // assuming electron mass
			float mass = (v1+v2).M()/1e3;
			float time_diff = TMath::Abs(selectron_time->at(i1) - selectron_time->at(i2) );
			if ( (mass > 75) && (mass < 95) )
			   time_inside->Fill( time_diff );
			else
			   time_outside->Fill( time_diff );
			massZ->Fill(mass);
		        mass_time->Fill ( mass , time_diff );
		}
	}

}

TFile out("testZinJ.root","RECREATE");
massZ->Write();
time_inside->Write();
time_outside->Write();
mass_time->Write();
out.Write();
out.Close();


}
