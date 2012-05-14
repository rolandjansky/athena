/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * This prints the cutflow for the 2-lepton channel
 * It refers to the SUSYObjDef class
 *
 * Author: Ryuichi Ueno (ryuichi.ueno@cern.ch)
 *      Based on 1-lepton code (Reco_dpd2.cxx) by 
 *      Jeanette Lorenz (jeanette.miriam.lorenz@cern.ch)
 * Date: July 2011
 */

#define Reco2Lep_cxx
#include "SUSYTools/Reco2Lep.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>
#include <TMatrixD.h>
#include <TVectorD.h>
#include <TH1F.h>
#include <TLorentzVector.h>
#include <TVector2.h>
#include <TSystem.h>
#include <TChain.h>
#include <TString.h>
#include <TTree.h>
#include <TLegend.h>
#include <cmath>
#include <fstream>
#include <algorithm>
#include <exception>
#include <vector>
#include "SUSYTools/egammaPIDdefs.h"
//#include "SUSYTools/checkOQ.h"
#include "MuonMomentumCorrections/SmearingClass.h"
//#include "StacoCBScaleEffFactors.h" 
//#include "egammaSFclass.h"
#include "SUSYTools/SUSYObjDef.h"
#include "SUSYTools/JetID.hpp"

using namespace std;
//using namespace SUSY;

//------------------------------------------------------------------------------------------------
// Main Program

void Reco2Lep::Loop(const string &argStr){

	//--- First part: Read in file list (split by ',') --------------------------------------------
	vector<string> fileList;
	for (size_t i=0,n; i <= argStr.length(); i=n+1)	{
		n = argStr.find_first_of(',',i);
		if (n == string::npos)
			n = argStr.length();
		string tmp = argStr.substr(i,n-i);
		string ttmp;
		for(unsigned int j=0; j<tmp.size(); j++){
			//if job is Grid job: filelist may not contain any additional '\n' as this would results in problems while trying to open the files
			if(tmp[j]==' ' || tmp[j]=='\n') continue;
			ttmp+=tmp[j];
		}
		fileList.push_back(ttmp);
	}

	// open input files
	TChain *chain = new TChain("susy","");
	for (unsigned int iFile=0; iFile<fileList.size(); ++iFile){
		cout << "open " << fileList[iFile].c_str() << endl;
		chain->AddFile(fileList[iFile].c_str());
	}

	//Init tree
	TTree *tree;
	tree = chain;
	Init(tree);

	string curr_file;
	int filecount(0);

	if (fChain == 0) return;
	Long64_t nentries = fChain->GetEntriesFast();
	int nevts = fChain->GetEntries();

	//--- Definition of output file ---------------------------------------------------------------
	TFile* outfile =  new TFile(outputFile, "recreate");

	// Cutflow histograms
	TH1D *hOSee_cut = new TH1D("hOSee_cut", "OSee Cutflow", 18, 0., 18.);
	TH1D *hOSem_cut = new TH1D("hOSem_cut", "OSem Cutflow", 18, 0., 18.);
	TH1D *hOSme_cut = new TH1D("hOSme_cut", "OSme Cutflow", 18, 0., 18.);
	TH1D *hOSmm_cut = new TH1D("hOSmm_cut", "OSmm Cutflow", 18, 0., 18.);
	TH1D *hSSee_cut = new TH1D("hSSee_cut", "SSee Cutflow", 15, 0., 15.);
	TH1D *hSSem_cut = new TH1D("hSSem_cut", "SSem Cutflow", 15, 0., 15.);
	TH1D *hSSme_cut = new TH1D("hSSme_cut", "SSme Cutflow", 15, 0., 15.);
	TH1D *hSSmm_cut = new TH1D("hSSmm_cut", "SSmm Cutflow", 15, 0., 15.);

	// counters
	int count = 0;
	
	//initializing of some classes
	//egammaOQ myOQ;
	TDirectory *pwd=gDirectory;
	pwd->cd();

	//egammaSFclass egammaSFclass; // Class used to correct MC electron reco efficiencies
	//EnergyRescaler geRescale; // Class used to rescale electron energy in data and smear electron energy in MC
	//geRescale.useDefaultCalibConstants();
	//Analysis::StacoCBScaleEffFactors gStacoCBSCF; // Class used to rescale MC muon reco efficiencies

	// Pileup Reweighting
	if(pileup){
		el_PileUp = new Root::TPileupReweighting("Electrons");
		int isGood=0;
		isGood=el_PileUp->initialize(elPileUpFile1, "avgintperbx",
											elPileUpFile2, "mu_mc10b");
		if(isGood!=0) cout << "Problem in Electron Pileup initialization::isGood=" << isGood << endl;

		mu_PileUp = new Root::TPileupReweighting("Muons");
		isGood=mu_PileUp->initialize(muPileUpFile1, "avgintperbx",
											muPileUpFile2, "mu_mc10b");
		if(isGood!=0) cout << "Problem in Muon Pileup initialization::isGood=" << isGood << endl;
	
		for(int i=0; i< 10; i++)
			cout << "weight(mu = " << i << "):\t" << el_PileUp->getPileupWeight(i) << "\t" << mu_PileUp->getPileupWeight(i) << endl;
	}

	// For muon trigger rescaling in MC
	if(trgwgt){
		TFile reweight_file(trgwgtFile, "OPEN");    //Open root file holding the two histogram
		THnSparseD* trig_numerator_hist = (THnSparseD*) reweight_file.Get("passes");    //Get the numerator histogram
		THnSparseD* trig_denominator_hist = (THnSparseD*) reweight_file.Get("trials");    //Get the denominator histogram
		TrigWeighter = new APReweightND(trig_denominator_hist, trig_numerator_hist, true); //Instantiate the trigger weighter

		delete trig_denominator_hist; //Cleanup input numerator histogram 
		delete trig_numerator_hist;   //Cleanup input denominator histogram
		reweight_file.Close();        //Close input file 
	}

	// GRL
	if(kIsData==1){
   	Root::TGoodRunsListReader* grlR = new Root::TGoodRunsListReader();
   	grlR->SetXMLFile(grlFile);
   	grlR->Interpret();
   	m_grl = grlR->GetMergedGoodRunsList();
   	m_grl.Summary();
	}

	/*
	//Load the "map":
	TDirectory *pwd=gDirectory;
	myOQ.initialize();
	weight_muon.AddWeightToEvt( TrigWeighter->GetWeight(value_array) );if (kIsData == 1){
	myOQ.egammaOQ::LoadOQMaps(RunNumber); //Now you can do it for every event because if the run is the same as the previous run, it returns immediately.
	} else {
	myOQ.egammaOQ::LoadOQMaps(167521);
	}
	pwd->cd();
	*/

	pwd->cd();//reset correct directory  
	susyobj.initialize();
  
	//--- Loop over all events --------------------------------------------------------------------
	cout << "Looping over all events!---" << endl;
	Long64_t nbytes = 0, nb = 0;
	for (Long64_t jentry=0; jentry<nentries;jentry++) {
		Long64_t ientry = LoadTree(jentry);
		if (ientry < 0) break;
		nb = fChain->GetEntry(jentry);   nbytes += nb;

		if (curr_file!=chain->GetFile()->GetName()){
			curr_file=chain->GetFile()->GetName();
			filecount++;
		}
		
		susyobj.Reset();  // Clear arrays
      if(count%10000==0) 
			cout << "Processing Event " << count << " / " << nevts << "..." << endl;

		double weight_e = 1.;
		double weight_m = 1.;
		int myRunNumber;
		if (kIsData==0) { // MC
			if(pileup){
				weight_e *= el_PileUp->getPileupWeight(lbn);
				weight_m *= mu_PileUp->getPileupWeight(lbn);
			}
			myRunNumber = 180614;  // Reject electrons in dead LAr regions
		} else { // Data
			myRunNumber = RunNumber;
		}

      count++;
		hOSee_cut->Fill(0., weight_e);	hOSem_cut->Fill(0., weight_e);
		hOSme_cut->Fill(0., weight_m);	hOSmm_cut->Fill(0., weight_m);
		hSSee_cut->Fill(0., weight_e);	hSSem_cut->Fill(0., weight_e);
		hSSme_cut->Fill(0., weight_m);	hSSmm_cut->Fill(0., weight_m);

		// Check GRL
		bool grlcheck=true;
		if(kIsData==1) grlcheck = m_grl.HasRunLumiBlock(RunNumber,lbn);

		if(!grlcheck) continue; // --- Cut 1: GRL --------------------------------------------------
      	
		hOSee_cut->Fill(1., weight_e);	hOSem_cut->Fill(1., weight_e);
		hOSme_cut->Fill(1., weight_m);	hOSmm_cut->Fill(1., weight_m);
		hSSee_cut->Fill(1., weight_e);	hSSem_cut->Fill(1., weight_e);
		hSSme_cut->Fill(1., weight_m);	hSSmm_cut->Fill(1., weight_m);

		if(!(kIsData==0 || larError==0)) continue; // --- Cut 2: LarError --------------------------

		hOSee_cut->Fill(2., weight_e);	hOSem_cut->Fill(2., weight_e);
		hOSme_cut->Fill(2., weight_m);	hOSmm_cut->Fill(2., weight_m);
		hSSee_cut->Fill(2., weight_e);	hSSem_cut->Fill(2., weight_e);
		hSSme_cut->Fill(2., weight_m);	hSSmm_cut->Fill(2., weight_m);

		//get muons, electrons and jets - define vector of indices of good muons, jets and electrons
		vector<int> el_index_baseline;
		//vector<int> el_index_crack;
		vector<int> el_index_met;
		vector<int> el_index_signal;
		vector<int> mu_index_baseline;
		vector<int> mu_index_met;
		vector<int> mu_index_cosmics;
		vector<int> mu_index_signal;
		vector<int> jet_index_bad;
		vector<int>	jet_index_good;
				
		// --- Get Electrons
		el_index_baseline = get_electron_baseline(myRunNumber);
		el_index_met		= get_electron_baseline(myRunNumber);
		//el_index_crack 	= get_electron_crack(el_index_baseline);

		// --- Get Muons
		mu_index_baseline = get_muon_baseline();
		mu_index_met		= get_muon_baseline();

		// --- Get Jets
		int nbadjets=0;
		jet_index_good		= get_jet_good(el_index_baseline, nbadjets);

		//--- Apply Lepton veto
		for(unsigned int ijet=0; ijet<jet_index_good.size();ijet++){
								
			vector<int> el_copy = el_index_baseline;
			for(unsigned int iel=0; iel<el_copy.size(); iel++){
				if(!(susyobj.GetElecTLV(el_copy[iel]).DeltaR(susyobj.GetJetTLV(jet_index_good[ijet]))>0.4))
					el_index_baseline.erase(remove(el_index_baseline.begin(),el_index_baseline.end(), el_copy[iel]),el_index_baseline.end());
			}

			vector<int> mu_copy = mu_index_baseline;
			for(unsigned int imu=0; imu<mu_copy.size(); imu++){
				if(!(susyobj.GetMuonTLV(mu_copy.at(imu)).DeltaR(susyobj.GetJetTLV(jet_index_good[ijet]))>0.4))
					mu_index_baseline.erase(remove(mu_index_baseline.begin(),mu_index_baseline.end(), mu_copy[imu]),mu_index_baseline.end());
			}
		}
		//--- Get Signal Electrons and Cosmic/Signal Muons
		el_index_signal	= get_electron_signal(el_index_baseline);
		mu_index_signal	= get_muon_signal(mu_index_baseline);
		mu_index_cosmics 	= get_muon_cosmics(mu_index_baseline);


		int checklarhole=0;	
		for(unsigned int iel=0; iel<el_index_baseline.size(); iel++){
			if(susyobj.IsLArHole(el_cl_eta->at(el_index_baseline[iel]), 
						el_cl_phi->at(el_index_baseline[iel])))
				checklarhole++;
		}
				
		if(checklarhole!=0) continue; // --- Cut 3: Electron LarHole -------------------------------

		hOSee_cut->Fill(3., weight_e);	hOSem_cut->Fill(3., weight_e);
		hOSme_cut->Fill(3., weight_m);	hOSmm_cut->Fill(3., weight_m);
		hSSee_cut->Fill(3., weight_e);	hSSem_cut->Fill(3., weight_e);
		hSSme_cut->Fill(3., weight_m);	hSSmm_cut->Fill(3., weight_m);
				
		for(int ijet=0; ijet<jet_AntiKt4TopoNewEM_n; ijet++) {	  
			if (susyobj.IsLArHoleVeto(jet_AntiKt4TopoNewEM_pt->at(ijet),
			 		jet_AntiKt4TopoNewEM_eta->at(ijet), jet_AntiKt4TopoNewEM_phi->at(ijet),
			 		jet_AntiKt4TopoNewEM_BCH_CORR_JET->at(ijet),
			 		jet_AntiKt4TopoNewEM_BCH_CORR_CELL->at(ijet),
					kIsData,40000.,RunNumber)) 
				checklarhole++;
		}

		if(checklarhole!=0) continue; // --- Cut 4: Jet LarHole ------------------------------------

		hOSee_cut->Fill(4., weight_e);	hOSem_cut->Fill(4., weight_e);
		hOSme_cut->Fill(4., weight_m);	hOSmm_cut->Fill(4., weight_m);
		hSSee_cut->Fill(4., weight_e);	hSSem_cut->Fill(4., weight_e);
		hSSme_cut->Fill(4., weight_m);	hSSmm_cut->Fill(4., weight_m);

		// Trigger
		bool trigger_muon = false;
		bool trigger_elec = false;
		if(kIsData==1){ // Data
			if (EF_mu18)  trigger_muon = true;  
			if (EF_e20_medium) trigger_elec = true; 
		} else { // MC
			trigger_muon = true;
			if(EF_e20_medium) trigger_elec = true;
		}

		if (!(trigger_muon || trigger_elec)) continue; // --- Cut 5: Trigger -----------------------

		if (trigger_muon){
			hOSme_cut->Fill(5., weight_m);	hOSmm_cut->Fill(5., weight_m);
			hSSme_cut->Fill(5., weight_m);	hSSmm_cut->Fill(5., weight_m);
		}
		if (trigger_elec){
			hOSee_cut->Fill(5., weight_e);	hOSem_cut->Fill(5., weight_e);
			hSSee_cut->Fill(5., weight_e);	hSSem_cut->Fill(5., weight_e);
		}

		// Calculation of MET
		double etx_RefFinal = MET_Simplified20_RefFinal_etx_CentralReg +
									MET_Simplified20_RefFinal_etx_EndcapRegion +
									MET_Simplified20_RefFinal_etx_ForwardReg;
		double ety_RefFinal = MET_Simplified20_RefFinal_ety_CentralReg +
									MET_Simplified20_RefFinal_ety_EndcapRegion +
									MET_Simplified20_RefFinal_ety_ForwardReg;
		double etx_RefJet = 	MET_Simplified20_RefJet_etx_CentralReg +
									MET_Simplified20_RefJet_etx_EndcapRegion +
									MET_Simplified20_RefJet_etx_ForwardReg;
		double ety_RefJet =	MET_Simplified20_RefJet_ety_CentralReg +
									MET_Simplified20_RefJet_ety_EndcapRegion +
									MET_Simplified20_RefJet_ety_ForwardReg;
							
		TVector2 met = susyobj.GetMET(etx_RefFinal, ety_RefFinal, 
												etx_RefJet, ety_RefJet,
												MET_Simplified20_Muon_Total_Staco_etx,
												MET_Simplified20_Muon_Total_Staco_ety,
												mu_index_met, el_index_met, false);

		if(nbadjets>0) continue; // --- Cut 6: Bad Jet (Only on Data) -------------------------------

		if (trigger_muon){
			hOSme_cut->Fill(6., weight_m);	hOSmm_cut->Fill(6., weight_m);
			hSSme_cut->Fill(6., weight_m);	hSSmm_cut->Fill(6., weight_m);
		}
		if (trigger_elec){
			hOSee_cut->Fill(6., weight_e);	hOSem_cut->Fill(6., weight_e);
			hSSee_cut->Fill(6., weight_e);	hSSem_cut->Fill(6., weight_e);
		}

		if(vx_nTracks->at(0)<=4) continue; // --- Cut 7: Vertex Cut --------------------------------
								
		if (trigger_muon){
			hOSme_cut->Fill(7., weight_m);	hOSmm_cut->Fill(7., weight_m);
			hSSme_cut->Fill(7., weight_m);	hSSmm_cut->Fill(7., weight_m);
		}
		if (trigger_elec){
			hOSee_cut->Fill(7., weight_e);	hOSem_cut->Fill(7., weight_e);
			hSSee_cut->Fill(7., weight_e);	hSSem_cut->Fill(7., weight_e);
		}

		//veto on crack electrons - do not apply for 2011 analysis
		int vetocrack=0;
		/*
		for(unsigned int ielec=0;ielec<elec_index_noiso.size();ielec++) {
			if(fabs((el_cl_eta->at(elec_index_noiso[ielec])))>1.37 && fabs((el_cl_eta->at(elec_index_noiso[ielec])))<1.52)
			vetocrack=1;
		}
		*/

		if(vetocrack!=0) continue;

		// Create Jet Container
		vector<double> jetptemjes;
		vector<Container> jet;

		for(unsigned int jet_ems = 0; jet_ems < jet_index_good.size(); jet_ems++)
			jetptemjes.push_back(jet_AntiKt4TopoNewEM_pt->at(jet_index_good[jet_ems]));

		for(unsigned int i=0; i<jet_index_good.size(); i++){
			Container x;
			x.pt = jetptemjes.at(i);
			x.phi = jet_AntiKt4TopoNewEM_phi->at(jet_index_good[i]);
			x.eta = jet_AntiKt4TopoNewEM_eta->at(jet_index_good[i]);
			x.index = jet_index_good[i];
			jet.push_back(x);
		}
		if(jetptemjes.size()>0){
			sort(jet.begin(),jet.end());
			reverse(jet.begin(),jet.end());
		}

		// Cosmic muon veto
		if(mu_index_cosmics.size()!=0) continue; // --- Cut 8: Cosmic Muon Veto --------------------
		if (trigger_muon){
			hOSme_cut->Fill(8., weight_m);	hOSmm_cut->Fill(8., weight_m);
			hSSme_cut->Fill(8., weight_m);	hSSmm_cut->Fill(8., weight_m);
		}
		if (trigger_elec){
			hOSee_cut->Fill(8., weight_e);	hOSem_cut->Fill(8., weight_e);
			hSSee_cut->Fill(8., weight_e);	hSSem_cut->Fill(8., weight_e);
		}

		// Now the Dilepton Cutflow
		bool passOSee=false;
		bool passOSem=false;
		bool passOSme=false;
		bool passOSmm=false;
		bool passSSee=false;
		bool passSSem=false;
		bool passSSme=false;
		bool passSSmm=false;
		TLorentzVector l1, l2;
		double cl1, cl2;
		double mll;

		// Calculate Lepton SF
		if(lsf){
			// Compute event weight for MC when selecting leptons
			// Event weight is computed after overlap removal (could be debated)
			// Still need to figure out whether noniso muons should also be rescaled?
			// Warning: Do NOT reweight MC when comparing to new 2011 data!
			double lep_SF = 1.0;
			for(unsigned int iel=0; iel<el_index_baseline.size(); iel++)
				lep_SF *= susyobj.GetSignalElecSF(el_cl_eta->at(el_index_baseline[iel]),
						el_cl_E->at(el_index_baseline[iel])/cosh(el_cl_eta->at(el_index_baseline[iel])));

			for(unsigned int imu=0; imu<mu_index_baseline.size(); imu++)
				lep_SF *= susyobj.GetSignalMuonSF(mu_index_baseline[imu]);

			weight_e *= lep_SF;
			weight_m *= lep_SF;
		}

		// --- Cuts 9 and 10: Dilepton Requirement and m_ll ----------------------------------------
		if(trigger_elec && mu_index_baseline.size()==0&&el_index_baseline.size()==2){
			l1 = susyobj.GetElecTLV(el_index_baseline[0]);
			l2 = susyobj.GetElecTLV(el_index_baseline[1]);
			if(l1.Pt()>25000 || l2.Pt()>25000 ){
				hOSee_cut->Fill(9., weight_e);	hSSee_cut->Fill(9., weight_e);
				cl1 = el_charge->at(el_index_baseline[0]);
				cl2 = el_charge->at(el_index_baseline[1]);
				mll = (l1 + l2).M();
				if(mll>12000.){
					if(cl1*cl2 < 0){ 
						hOSee_cut->Fill(10., weight_e);
						passOSee = true;
					} else { 
						hSSee_cut->Fill(10., weight_e);
						passSSee = true;
					}
				}
			}
		}

		if(trigger_elec && mu_index_baseline.size()==1&&el_index_baseline.size()==1){
			l1 = susyobj.GetElecTLV(el_index_baseline[0]);
			l2 = susyobj.GetMuonTLV(mu_index_baseline[0]);
			if(l1.Pt()>25000){
				hOSem_cut->Fill(9., weight_e);	hSSem_cut->Fill(9., weight_e);
				cl1 = el_charge->at(el_index_baseline[0]);
				cl2 = mu_staco_charge->at(mu_index_baseline[0]);
				mll = (l1 + l2).M();
				if(mll>12000.){
					if(cl1*cl2 < 0){ 
						hOSem_cut->Fill(10., weight_e);
						passOSem = true;
					} else {
						hSSem_cut->Fill(10., weight_e);
						passSSem = true;
					}
				}
			}
		}

		if(trigger_muon && mu_index_baseline.size()==1&&el_index_baseline.size()==1){
			// Apply trigger reweighting
			if(trgwgt)	weight_m *= get_trigger_weight(mu_index_baseline);

			l1 = susyobj.GetMuonTLV(mu_index_baseline[0]);
			l2 = susyobj.GetElecTLV(el_index_baseline[0]);
			if((l1.Pt()>20000) && (l2.Pt()<25000 || !trigger_elec)){
				hOSme_cut->Fill(9., weight_m);	hSSme_cut->Fill(9., weight_m);
				cl1 = mu_staco_charge->at(mu_index_baseline[0]);
				cl2 = el_charge->at(el_index_baseline[0]);
				mll = (l1 + l2).M();
				if(mll>12000.){
					if(cl1*cl2 < 0){
						hOSme_cut->Fill(10., weight_m);
						passOSme = true;
					} else {
						hSSme_cut->Fill(10., weight_m);
						passSSme = true;
					}
				}
			}
		}

		if(trigger_muon && mu_index_baseline.size()==2&&el_index_baseline.size()==0){
			// Apply trigger reweighting
			if(trgwgt)	weight_m *= get_trigger_weight(mu_index_baseline);

			l1 = susyobj.GetMuonTLV(mu_index_baseline[0]);
			l2 = susyobj.GetMuonTLV(mu_index_baseline[1]);
			if(l1.Pt()>20000 || l2.Pt()>20000){
				hOSmm_cut->Fill(9., weight_m);	hSSmm_cut->Fill(9., weight_m);
				cl1 = mu_staco_charge->at(mu_index_baseline[0]);
				cl2 = mu_staco_charge->at(mu_index_baseline[1]);
				mll = (l1 + l2).M();
				if(mll>12000.){
					if(cl1*cl2 < 0){
						hOSmm_cut->Fill(10., weight_m);
						passOSmm = true;
					} else {
						hSSmm_cut->Fill(10., weight_m);
						passSSmm = true;
					}
				}
			}
		}

		// Calculate mT and meff - NOT CURRENTLY USED
		//...

		// --- Cut 11: Tight iso cut ---------------------------------------------------------------
		passOSee = passOSee&&el_index_signal.size()==2&&mu_index_signal.size()==0;
		passSSee = passSSee&&el_index_signal.size()==2&&mu_index_signal.size()==0;
		passOSem = passOSem&&el_index_signal.size()==1&&mu_index_signal.size()==1;
		passSSem = passSSem&&el_index_signal.size()==1&&mu_index_signal.size()==1;
		passOSme = passOSme&&el_index_signal.size()==1&&mu_index_signal.size()==1;
		passSSme = passSSme&&el_index_signal.size()==1&&mu_index_signal.size()==1;
		passOSmm = passOSmm&&el_index_signal.size()==0&&mu_index_signal.size()==2;
		passSSmm = passSSmm&&el_index_signal.size()==0&&mu_index_signal.size()==2;

		if(passOSee) hOSee_cut->Fill(11., weight_e);
		if(passSSee) hSSee_cut->Fill(11., weight_e);
		if(passOSem) hOSem_cut->Fill(11., weight_e);
		if(passSSem) hSSem_cut->Fill(11., weight_e);
		if(passOSme) hOSme_cut->Fill(11., weight_m);
		if(passSSme) hSSme_cut->Fill(11., weight_m);
		if(passOSmm) hOSmm_cut->Fill(11., weight_m);
		if(passSSmm) hSSmm_cut->Fill(11., weight_m);

		// --- Cut 12 and after: Signal Regions ----------------------------------------------------
		// Opposite-Sign Signal Regions:
		//-------------------------------
		// MET Cuts
		if(met.Mod() > 80000.){
			if(passOSee) hOSee_cut->Fill(12., weight_e);
			if(passOSem){ 
				hOSem_cut->Fill(12., weight_e);
			//--- Debug -----
			//cout << "Event # " << EventNumber << "" << endl;
			//cout << "Electron pt (el_cl_pt): " << el_cl_pt->at(el_index_signal[0]) << " / eta (el_cl_eta): " << el_cl_eta->at(el_index_signal[0]) << " / phi (el_cl_phi): " << el_cl_phi->at(el_index_signal[0]) << endl;
			//cout << "Muon pt (mu_staco_pt): " << mu_staco_pt->at(mu_index_signal[0])  << " / eta (mu_staco_eta): " << mu_staco_eta->at(mu_index_signal[0]) << " / phi (mu_staco_phi): " << mu_staco_phi->at(mu_index_signal[0]) << endl;
			//--- End Debug -
			}
			if(passOSme) hOSme_cut->Fill(12., weight_m);
			if(passOSmm) hOSmm_cut->Fill(12., weight_m);
			if(met.Mod() > 250000.){ //--- SR1
				if(passOSee) hOSee_cut->Fill(13., weight_e);
				if(passOSem) hOSem_cut->Fill(13., weight_e);
				if(passOSme) hOSme_cut->Fill(13., weight_m);
				if(passOSmm) hOSmm_cut->Fill(13., weight_m);
			}
		}

		//--- SR2: 3-jet Signal Region
		if(jet_index_good.size()>=3 &&
				jet[0].pt>80000. && jet[1].pt>40000. && jet[2].pt>40000.){
			if(passOSee) hOSee_cut->Fill(14., weight_e);
			if(passOSem) hOSem_cut->Fill(14., weight_e);
			if(passOSme) hOSme_cut->Fill(14., weight_m);
			if(passOSmm) hOSmm_cut->Fill(14., weight_m);
			if(met.Mod()>220000.){
				if(passOSee) hOSee_cut->Fill(15., weight_e);
				if(passOSem) hOSem_cut->Fill(15., weight_e);
				if(passOSme) hOSme_cut->Fill(15., weight_m);
				if(passOSmm) hOSmm_cut->Fill(15., weight_m);
			}
		}

		//--- SR3: 4-jet Signal Region
		if(jet_index_good.size()>=4 &&
				jet[0].pt>100000. && jet[1].pt>70000. && jet[2].pt>70000.  && jet[3].pt>70000.){
			if(passOSee) hOSee_cut->Fill(16., weight_e);
			if(passOSem) hOSem_cut->Fill(16., weight_e);
			if(passOSme) hOSme_cut->Fill(16., weight_m);
			if(passOSmm) hOSmm_cut->Fill(16., weight_m);
			if(met.Mod()>100000.){
				if(passOSee) hOSee_cut->Fill(17., weight_e);	
				if(passOSem) hOSem_cut->Fill(17., weight_e);
				if(passOSme) hOSme_cut->Fill(17., weight_m);
				if(passOSmm) hOSmm_cut->Fill(17., weight_m);
			}
		}
	
		// Same Sign Signal Regions:
		//---------------------------
		//--- SS SR1: MET > 100GeV 
		if(met.Mod()>100000.){
			if(passSSee) hSSee_cut->Fill(12., weight_e);
			if(passSSem) hSSem_cut->Fill(12., weight_e);
			if(passSSme) hSSme_cut->Fill(12., weight_m);
			if(passSSmm) hSSmm_cut->Fill(12., weight_m);
		}

		//--- SS SR2: 2-jet Signal Region
		if(jet_index_good.size()>=2 &&
				jet[0].pt>50000. && jet[1].pt>50000.){
			if(passSSee) hSSee_cut->Fill(13., weight_e);	
			if(passSSem) hSSem_cut->Fill(13., weight_e);
			if(passSSme) hSSme_cut->Fill(13., weight_m);	
			if(passSSmm) hSSmm_cut->Fill(13., weight_m);
			if(met.Mod()>80000.){
				if(passSSee) hSSee_cut->Fill(14., weight_e);
				if(passSSem) hSSem_cut->Fill(14., weight_e);
				if(passSSme) hSSme_cut->Fill(14., weight_m);
				if(passSSmm) hSSmm_cut->Fill(14., weight_m);
			}
		}

	} //end of loop
   
	/* Lumi weighting is deprecated - should be done post-processing if needed
	if(lumi){ // Luminosity Weighting
		hOSee_cut->Scale((lumi_total*sigma_su4)/hOSee_cut->GetBinContent(1));
		hOSem_cut->Scale((lumi_total*sigma_su4)/hOSem_cut->GetBinContent(1));
		hOSme_cut->Scale((lumi_total*sigma_su4)/hOSme_cut->GetBinContent(1));
		hOSmm_cut->Scale((lumi_total*sigma_su4)/hOSmm_cut->GetBinContent(1));
		hSSee_cut->Scale((lumi_total*sigma_su4)/hSSee_cut->GetBinContent(1));
		hSSem_cut->Scale((lumi_total*sigma_su4)/hSSem_cut->GetBinContent(1));
		hSSme_cut->Scale((lumi_total*sigma_su4)/hSSme_cut->GetBinContent(1));
		hSSmm_cut->Scale((lumi_total*sigma_su4)/hSSmm_cut->GetBinContent(1));
	}
	*/

   cout << "\n--- Cutflow Results: Pileup=" << pileup << " / Trigger Weighting=" << trgwgt << " / Lepton Scaling=" << lsf;
	cout << "\n--- Common Cuts ------------------------------------" << endl;
	cout << "Cuts \t el-el \t el-mu \t mu-el \t mu-mu" << endl;
	TString CommonLabels[9] = {"No Cut:", "GRL:", "LAr Error:", "Electron LArHole:", "Jet LArHole:",
								"Trigger:", "Jet/MET Cleaning:", "Primary Vertex:", "Cosmic Muon Veto:"}; 
	for(int ncuts=0; ncuts<9; ncuts++){
   	cout << CommonLabels[ncuts] << "\t";
		cout << hOSee_cut->GetBinContent(ncuts+1) << "\t";
		cout << hOSem_cut->GetBinContent(ncuts+1) << "\t"; 
		cout << hOSme_cut->GetBinContent(ncuts+1) << "\t";
		cout << hOSmm_cut->GetBinContent(ncuts+1) << endl;
	}
	cout << "--- OS Cuts ------------------------------------" << endl;
	cout << "Cuts \t OSee \t OSem \t OSme \t OSmm" << endl;
	TString OSLabels[9] = {"Dilepton Requirement", "Signs & m_ll > 12 GeV:", "Tight Iso Lepton:", "MET > 80 GeV:",
								"SR1: MET > 250 GeV:", "SR2: Njet(80,40,40):", "     3jet+MET > 220 GeV:",
								"SR3: Njet(100,70,70,70):", "     4jet+MET > 100 GeV:"};
	for(int ncuts=0; ncuts<9; ncuts++){
   	cout << OSLabels[ncuts] << "\t";
		cout << hOSee_cut->GetBinContent(ncuts+10) << "\t";
		cout << hOSem_cut->GetBinContent(ncuts+10) << "\t"; 
		cout << hOSme_cut->GetBinContent(ncuts+10) << "\t";
		cout << hOSmm_cut->GetBinContent(ncuts+10) << endl;
	}
   cout << "--- SS Cuts ------------------------------------" << endl;
	cout << "Cuts \t SSee \t SSem \t SSme \t SSmm" << endl;
	TString SSLabels[6] = {"Dilepton Requirement", "Signs & m_ll > 12 GeV:", "Tight Iso Lepton:", 
								"SR1: MET > 100 GeV:", "SR2: Njet(50,50):", "     2jet+MET > 80 GeV:"};
   for(int ncuts=0; ncuts<6; ncuts++){
		cout << SSLabels[ncuts] << "\t";
		cout << hSSee_cut->GetBinContent(ncuts+10) << "\t";
		cout << hSSem_cut->GetBinContent(ncuts+10) << "\t";
		cout << hSSme_cut->GetBinContent(ncuts+10) << "\t";
		cout << hSSmm_cut->GetBinContent(ncuts+10) << endl;
	}
   cout << "------------------------------------------------" << endl;
	
	// Save histograms and close the file
	hOSee_cut->Write(); hOSem_cut->Write(); hOSme_cut->Write(); hOSmm_cut->Write();
	hSSee_cut->Write(); hSSem_cut->Write(); hSSme_cut->Write(); hSSmm_cut->Write();
	outfile->Close();


	//check if all files have been read 
	if(static_cast<int>(fileList.size())!=filecount){
		cerr << "Not all files read!" << endl;
		exit(1);
	}else
		cout << "All ok." << endl;

} // end of Reco2Lep::Loop

//--- Additional Functions ------------------------------------------------------------------------
double Reco2Lep::get_trigger_weight(vector<int> mu_index){

	double mu_weight =0.;
	// Apply the trigger scale factors (only for muon stream):
	// E-Mu Trigger SF
	if(mu_index.size()==1){
		TLorentzVector mu = susyobj.GetMuonTLV(mu_index[0]);
		APEvtWeight weight_muon;
		double value_array[5] = {mu.Pt(), mu.Eta(), mu.Phi(),
			mu_staco_ptcone20->at(mu_index[0]),
			(double)(mu_staco_isCombinedMuon->at(mu_index[0])==true)};
		weight_muon.AddWeightToEvt( TrigWeighter->GetWeight(value_array) );
		mu_weight = weight_muon.GetWeight(); 
	}

	// DiMu Trigger SF
	if(mu_index.size()==2){
		TLorentzVector mus[2];
		mus[0] = susyobj.GetMuonTLV(mu_index[0]);
		mus[1] = susyobj.GetMuonTLV(mu_index[1]);
		double mu_w[2];
		for(int k=0; k<2; k++){
			APEvtWeight weight_muon;
			double value_array[5] = {mus[k].Pt(), mus[k].Eta(), mus[k].Phi(),
				mu_staco_ptcone20->at(mu_index[k]),
				(double)(mu_staco_isCombinedMuon->at(mu_index[k])==true)};
			weight_muon.AddWeightToEvt( TrigWeighter->GetWeight(value_array) );
			mu_w[k] = weight_muon.GetWeight();
		}

		// Calculate dimuon weight
		mu_weight = 1-((1-mu_w[0])*(1-mu_w[1]));
	}
	return mu_weight;
}

vector<int> Reco2Lep::get_electron_baseline(int myRunNumber){
	vector<int> elec_index;
	for(int ielec=0;ielec<el_n;ielec++){
		if (susyobj.IsElectron(ielec,myRunNumber, 
					el_cl_eta->at(ielec), el_cl_phi->at(ielec), el_cl_E->at(ielec), el_tracketa->at(ielec), el_trackphi->at(ielec),
					el_author->at(ielec), el_isEM->at(ielec), el_OQ->at(ielec), el_nPixHits->at(ielec), el_nSCTHits->at(ielec),
					kIsData, 20000, 2.47, false)) {
			elec_index.push_back(ielec);
		}
		//if (get_electron(ielec,myOQ,kIsData,geRescale)==1) elec_index.push_back(ielec);
	}
	return elec_index;
}

vector<int> Reco2Lep::get_electron_signal(vector<int> elec_index){
	vector<int> elec_index_signal;
	for(unsigned int ielec=0; ielec<elec_index.size(); ielec++){
		// signal = pt cut 20 GeV with ptcone20/pt < 0.1
		if (susyobj.IsSignalElectron(elec_index[ielec],el_isEM->at(elec_index[ielec]),el_ptcone20->at(elec_index[ielec]),20000., 0.1)) 
			elec_index_signal.push_back(elec_index[ielec]); 
	}
	return elec_index_signal;
}

/*
vector<int> Reco2Lep::get_electron_crack(vector<int> elec_index){
	vector<int> elec_index_crack;
	for(unsigned int ielec=0; ielec<elec_index.size(); ielec++){
		if (susyobj.IsInCrack(el_cl_eta->at(elec_index[ielec]))) 
			elec_index_crack.push_back(elec_index[ielec]); 
	}
	return elec_index_crack;
}
*/

vector<int> Reco2Lep::get_muon_baseline(){
	vector<int> muon_index;
	for(int imuon=0;imuon<mu_staco_n;imuon++){
		if(susyobj.IsMuon(imuon,
				mu_staco_pt->at(imuon), mu_staco_eta->at(imuon), mu_staco_phi->at(imuon), mu_staco_E->at(imuon),
				//Renaud mu_staco_qoverp_exPV->at(imuon), mu_staco_cov_qoverp_exPV->at(imuon),
				mu_staco_me_qoverp_exPV->at(imuon), mu_staco_id_qoverp_exPV->at(imuon),
				mu_staco_me_theta_exPV->at(imuon), mu_staco_id_theta_exPV->at(imuon),  mu_staco_id_theta->at(imuon),
				mu_staco_isCombinedMuon->at(imuon), mu_staco_isLowPtReconstructedMuon->at(imuon),
				mu_staco_loose->at(imuon),
				mu_staco_expectBLayerHit->at(imuon), mu_staco_nBLHits->at(imuon), mu_staco_nPixHits->at(imuon),
				mu_staco_nPixelDeadSensors->at(imuon), mu_staco_nPixHoles->at(imuon), mu_staco_nSCTHits->at(imuon),
				mu_staco_nSCTDeadSensors->at(imuon), mu_staco_nSCTHoles->at(imuon), mu_staco_nTRTHits->at(imuon), mu_staco_nTRTOutliers->at(imuon),
				kIsData,10000.,2.4)) 
			muon_index.push_back(imuon);
	}
	return muon_index;
}

vector<int> Reco2Lep::get_muon_cosmics(vector<int> muon_index){
	vector<int> muon_index_cosmics;
	for(unsigned int imuon=0; imuon<muon_index.size(); imuon++){
		if (susyobj.IsCosmicMuon(mu_staco_z0_exPV->at(muon_index[imuon]),mu_staco_d0_exPV->at(muon_index[imuon]),1.0, 0.2))
			muon_index_cosmics.push_back(muon_index[imuon]);
	}
	return muon_index_cosmics;
}

vector<int> Reco2Lep::get_muon_signal(vector<int> muon_index){
	vector<int> muon_index_signal;
	for(unsigned int imuon=0; imuon<muon_index.size(); imuon++){
		if(susyobj.IsSignalMuon(muon_index[imuon], mu_staco_ptcone20->at(muon_index[imuon]), 10000.))
			muon_index_signal.push_back(muon_index[imuon]);
	}
	return muon_index_signal;
}

vector<int> Reco2Lep::get_jet_good(vector<int> elec_index, int &nbadjets){
	vector<int> jet_index;
	for(int ijet=0; ijet<jet_AntiKt4TopoNewEM_n; ijet++){

		bool goodjet = susyobj.IsJet(ijet, jet_AntiKt4TopoNewEM_pt->at(ijet),
													jet_AntiKt4TopoNewEM_eta->at(ijet), 
													jet_AntiKt4TopoNewEM_phi->at(ijet),
													jet_AntiKt4TopoNewEM_E->at(ijet), 
													jet_AntiKt4TopoNewEM_emscale_eta->at(ijet),
													jet_AntiKt4TopoNewEM_emfrac->at(ijet),
													jet_AntiKt4TopoNewEM_hecf->at(ijet),
													jet_AntiKt4TopoNewEM_LArQuality->at(ijet),
													jet_AntiKt4TopoNewEM_HECQuality->at(ijet),
													jet_AntiKt4TopoNewEM_Timing->at(ijet),
													jet_AntiKt4TopoNewEM_sumPtTrk->at(ijet),
													jet_AntiKt4TopoNewEM_fracSamplingMax->at(ijet),
													jet_AntiKt4TopoNewEM_NegativeE->at(ijet),
													kIsData, 20000., 4.5, JetID::LooseBad);

		TLorentzVector thisjet = susyobj.GetJetTLV(ijet);
		if (thisjet.Pt() <= 20000. || jet_AntiKt4TopoNewEM_E->at(ijet) < 0. || 
				TMath::Abs(jet_AntiKt4TopoNewEM_eta->at(ijet)) >= 4.5)
			continue; // Skip Jet

		bool overlap = false;
		for(unsigned int iel=0; iel<elec_index.size(); iel++){
			if(susyobj.GetElecTLV(elec_index[iel]).DeltaR(thisjet)<=0.2)
				overlap = true;
		}
		if(overlap) continue; // Skip Jet

		if(!goodjet){ nbadjets++; continue; } // Increment nbadjets and skip jet

		if(TMath::Abs(jet_AntiKt4TopoNewEM_eta->at(ijet)) < 2.8)
				jet_index.push_back(ijet);
	} 

	return jet_index;
}

