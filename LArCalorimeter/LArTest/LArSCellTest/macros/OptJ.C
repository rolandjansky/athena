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

void OptJ(){


//TFile f("DumpAllCl.DumpAll.root");
//TFile f("DumpAll.El.root");
//TFile f("DumpAll.J.root");
TFile f("test20161201C/DumpAllCl.root");
bool debug=false;

f.cd("DumpAll");
TTree* evt = (TTree*)f.Get("DumpAll/Evt");
TTree* selectron = (TTree*)f.Get("DumpAll/SCluster");
TTree* truth = (TTree*)f.Get("DumpAll/Truth");
TTree* l1 = (TTree*)f.Get("DumpAll/L1");
TTree* cell = (TTree*)f.Get("DumpAll/CellInFront");


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

std::vector< std::vector<float> > *m_cell_infront_e(0);
std::vector< std::vector<float> > *m_cell_infront_eta(0);
std::vector< std::vector<float> > *m_cell_infront_phi(0);
std::vector< std::vector<float> > *m_cell_infront_layer(0);
//std::vector< std::vector<float> > *m_cell_infront_time;
//std::vector< std::vector<float> > *m_cell_infront_quality;

cell->SetBranchAddress("e",&m_cell_infront_e);
cell->SetBranchAddress("eta",&m_cell_infront_eta);
cell->SetBranchAddress("phi",&m_cell_infront_phi);
cell->SetBranchAddress("layer",&m_cell_infront_layer);

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



TH1F* det_pt = new TH1F("det_pt","det_pt",60,0,60);
TH1F* num1_pt = new TH1F("num1_pt","num1_pt",60,0,60);
TH1F* num2_pt = new TH1F("num2_pt","num2_pt",60,0,60);
TH1F* num3_pt = new TH1F("num3_pt","num3_pt",60,0,60);
TH1F* num4_pt = new TH1F("num4_pt","num4_pt",60,0,60);

TH1F* det_eta = new TH1F("det_eta","det_eta",50,-2.5,2.5);
TH1F* num1_eta = new TH1F("num1_eta","num1_eta",50,-2.5,2.5);
TH1F* num2_eta = new TH1F("num2_eta","num2_eta",50,-2.5,2.5);
TH1F* num3_eta = new TH1F("num3_eta","num3_eta",50,-2.5,2.5);
TH1F* num4_eta = new TH1F("num4_eta","num4_eta",50,-2.5,2.5);

// Some variables to cut
TH1F* rEta = new TH1F("rEta","rEta",50,0.5,1.0);
TH1F* had1 = new TH1F("had1","had1",80,-0.02,0.38);
TH1F* f1 = new TH1F("f1","f1",60,-0.2,1.0);
TH1F* wstot = new TH1F("wstot","wstot",60,-0.02,0.1);
TH1F* f3 = new TH1F("f3","f3",270,-0.1,0.8);
TH1F* had = new TH1F("had","had",50,0,0.1);
TH1F* eratio = new TH1F("eratio","eratio",60,-0.1,1.1);

TH2F* rEta_vs_eta = new TH2F("rEta_vs_eta","rEta vs eta",50,0,2.5,50,0.5,1.0);
TH2F* had1_vs_eta = new TH2F("had1_vs_eta","had1 vs eta",50,0,2.5,50,0.5,1.0);
TH2F* wstot_vs_eta = new TH2F("wstot_vs_eta","wstot vs eta",50,0,2.5,60,-0.02,0.1);
TH2F* f1_vs_eta = new TH2F("f1_vs_eta","f1 vs eta",50,0,2.5,60,-0.2,1.0);
TH2F* f3_vs_eta = new TH2F("f3_vs_eta","f3 vs eta",50,0,2.5,40,-0.1,0.3);
TH2F* eratio_vs_eta = new TH2F("eratio_vs_eta","eratio vs eta",50,0,2.5,60,-0.1,1.1);

if ( debug ) nevts = 5;

for(int ev=0;ev<nevts;++ev){

        evt->GetEntry(ev);
        selectron->GetEntry(ev);
	l1->GetEntry(ev);
	cell->GetEntry(ev);

	if ( debug ) {
        std::cout << " test : " << ev << " " << evt_count << " " << evt_nvtx << std::endl;
        for(int i=0;i<selectron_et->size();i++){
                std::cout << "\t SC: " << i << " " << selectron_et->at(i) << " " << selectron_eta->at(i) << " " << selectron_phi->at(i)<< std::endl;
        }
	for(int i=0;i<l1_et->size();i++){
		std::cout << "\t L1: " << i << " " << l1_et->at(i) << " " << l1_eta->at(i) << " " << l1_phi->at(i) << std::endl;
	}
	}

	for(int i=0;i<l1_et->size();i++){

		float et_l1 = l1_et->at(i);
		if ( et_l1 < 4e3 ) continue;
		float eta_l1 = l1_eta->at(i);
		if ( TMath::Abs(eta_l1) > 2.55 ) continue;
		bool in_crack ( (TMath::Abs(eta_l1) > 1.37) && (TMath::Abs(eta_l1) < 1.54) );
		float phi_l1 = l1_phi->at(i);
		if ( !in_crack) det_pt->Fill(et_l1/1e3);
		det_eta->Fill(eta_l1);
		int found_sc(-1);
		for( int l=0; l<selectron_et->size(); l++){
			if ( TMath::Abs(selectron_eta->at(l) - l1_eta->at(i) ) > 0.12) continue;
			if ( check_phi(selectron_phi->at(l), l1_phi->at(i) ) > 0.12 ) continue;
			found_sc = l;
		}
		if ( found_sc < 0 ) continue;
		// In principle some communicating sequence is available
		/* Perform sequencial cuts */
		if ( !in_crack) num1_pt->Fill(et_l1/1e3);
		num1_eta->Fill(eta_l1);
		if ( l1_et->at(i) < 10000 ) continue;
		if ( l1_hadCore->at(i) > 2e3 ) continue;
	           if ( !in_crack) num2_pt->Fill(et_l1/1e3);
		   num2_eta->Fill(eta_l1);
		   if ( selectron_et->at(found_sc) < 11e3) continue;
	           if ( !in_crack) num3_pt->Fill(et_l1/1e3);
		   num3_eta->Fill(eta_l1);
		   float rEta_var = selectron_e237->at(found_sc);
		   if ( selectron_e277->at(found_sc) > 0 ) rEta_var /= selectron_e277->at(found_sc);
		   else continue;
		   if (!in_crack) rEta->Fill(rEta_var);
		   if (!in_crack) rEta_vs_eta->Fill(eta_l1,rEta_var);
                   if ( rEta_var < 0.795 ) continue;
                   if ( !in_crack) num4_pt->Fill(et_l1/1e3);
                   num4_eta->Fill(eta_l1);
                   float f1_var = selectron_e1->at(found_sc);
                   if ( selectron_energy->at(found_sc) > 0 ) f1_var /= selectron_energy->at(found_sc);
                   if (!in_crack) f1->Fill(f1_var);
		   if (!in_crack) f1_vs_eta->Fill(eta_l1,f1_var);
                   float wstot_var = selectron_wstot->at(found_sc);
                   if (!in_crack) wstot->Fill(wstot_var);
                   if (!in_crack) wstot_vs_eta->Fill(eta_l1,wstot_var);
                   float f3_var = selectron_e3->at(found_sc);
                   if ( selectron_energy->at(found_sc) > 0 ) f3_var /= selectron_energy->at(found_sc);
                   if (!in_crack) f3->Fill(f3_var);
		   if (!in_crack) f3_vs_eta->Fill(eta_l1,f3_var);
                   float had_var = selectron_ehad1->at(found_sc);
                   if ( selectron_energy->at(found_sc) > 0 ) had_var /=selectron_energy->at(found_sc);
                   if ( !in_crack) had1->Fill(had_var);

                   float etaCluster = selectron_eta->at(found_sc);
                   float phiCluster = selectron_phi->at(found_sc);
                   std::cout << "Cluster : " << selectron_et->at(found_sc) << " " << etaCluster << " " << phiCluster  << std::endl;
                   std::vector<float> etas;
                   for ( int icell=0; icell < m_cell_infront_e->size(); icell++){
                        std::vector<float>& vec_e = m_cell_infront_e->at(icell);
                        std::vector<float>& vec_eta = m_cell_infront_eta->at(icell);
                        std::vector<float>& vec_phi = m_cell_infront_phi->at(icell);
                        std::vector<float>& vec_lay = m_cell_infront_layer->at(icell);
                        for(unsigned int ii=0;ii<vec_e.size();++ii){
                                if ( TMath::Abs(vec_eta.at(ii) - etaCluster) > 0.07 ) continue;
                                if ( check_phi(vec_phi.at(ii),phiCluster) > 0.11 ) continue;
                                float etaCell = vec_eta.at(ii);
                                float min_dist = 999.0;
                                for(int jj=0;jj<etas.size(); jj++){
                                    float dist = TMath::Abs(etas.at(jj) - etaCell);
                                    if ( dist < min_dist ) min_dist = dist;
                                }
                                if ( min_dist > 0.002 ) etas.push_back( etaCell );
                        } // vector again
                   } // icell for
                   float tt=0;
                   std::sort(etas.begin(), etas.end() );
                   std::vector<float> en1st( etas.size(), 0.0);
                   for (int jj=0;jj<etas.size(); jj++) {
                   for ( int icell=0; icell < m_cell_infront_e->size(); icell++){
                        std::vector<float>& vec_e = m_cell_infront_e->at(icell);
                        std::vector<float>& vec_eta = m_cell_infront_eta->at(icell);
                        std::vector<float>& vec_phi = m_cell_infront_phi->at(icell);
                        std::vector<float>& vec_lay = m_cell_infront_layer->at(icell);
                        for(unsigned int ii=0;ii<vec_e.size();++ii){
                                if ( TMath::Abs(vec_eta.at(ii) - etaCluster) > 0.08 ) continue;
                                if ( check_phi(vec_phi.at(ii),phiCluster) > 0.105 ) continue;
                                if ( TMath::Abs(vec_eta.at(ii) - etas.at(jj) ) > 0.0025 ) continue;
                                en1st.at(jj)+=vec_e.at(ii);
                                //float min_dist = 999.0;
                        } // vector again
                   } // icell for
                   } // loop to fill en1st
                   float emax=-999.0;
                   int max_index=-1;
                   for (int jj=0;jj<etas.size(); jj++) {
                        if ( en1st.at(jj) > emax ) {
                           emax = en1st.at(jj);
                           max_index = jj;
                        }
                   }
                   float e2max=-999;
                   int max2_index=-1;
                   if ( max_index > -1 ) {
                   for (int jj=1;jj<etas.size()-1; jj++) {
                        if ( ! ( ( en1st.at(jj) > en1st.at(jj-1) ) && ( en1st.at(jj) > en1st.at(jj+1) ) ) ) continue; // must be local maximum
                        if ( jj == max_index ) continue;
                        if ( en1st.at(jj) > e2max ) {
                           e2max = en1st.at(jj);
                           max2_index = jj;
                        }
                   }
                   }
                   float tmperatio = 999.0;
                   if ( (emax>-100) && ( e2max>-100) && (emax+e2max> 0 ) )
                        tmperatio = (emax-e2max) / (emax + e2max);
                   if (!in_crack) eratio->Fill(tmperatio);
                   eratio_vs_eta->Fill(eta_l1,tmperatio);
	} // truth

}

TFile out("testOutJ.Cl.root","RECREATE");
det_pt->Write();
det_eta->Write();
num1_pt->Write();
num1_eta->Write();
num2_pt->Write();
num2_eta->Write();
num3_pt->Write();
num3_eta->Write();
num4_pt->Write();
num4_eta->Write();
rEta->Write();
f1->Write();
wstot->Write();
f3->Write();
had1->Write();
rEta_vs_eta->Write();
f1_vs_eta->Write();
wstot_vs_eta->Write();
f3_vs_eta->Write();
eratio->Write();
eratio_vs_eta->Write();
out.Write();
out.Close();


}
