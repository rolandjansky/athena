/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//This macro prints the cutflow for the 1-muon channel and for the 1-electron channel
//It refers to the SUSYObjDef class
//author: Jeanette Lorenz (jeanette.miriam.lorenz@cern.ch)


#define Reco_dpd2_cxx
#include "SUSYTools/Reco_dpd2.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include<iostream>
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
#include "SUSYTools/egammaPIDdefs.h"
//#include "SUSYTools/checkOQ.h"
#include "MuonMomentumCorrections/SmearingClass.h"
//#include "StacoCBScaleEffFactors.h" 
//#include "egammaSFclass.h"
#include "SUSYTools/SUSYObjDef.h"
#include "SUSYTools/JetID.hpp"


using namespace std;
//using namespace SUSY;

//gROOT->Reset();
/*
int main(){
  Reco_dpd2 t;
  t.Loop();

  return EXIT_SUCCESS;
}
*/


///////////////////////////////////////////////////////////////////////////////////////////////////////////
//main program

void Reco_dpd2::Loop(const std::string &argStr)
{

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//First part: Read in file list
/*
  std::string argStr;

  char buf[256+1];
  unsigned int delpos;
  std::ifstream ifs("input.txt");
  while (true)
    {
      ifs.read(buf,256);
      if (ifs.eof())
        {
          if (ifs.gcount() == 0) break;
          delpos = ifs.gcount()-1;
        }
      else
        {
          delpos = ifs.gcount();
        }
      buf[delpos] = 0x00;
      argStr += buf;
    }

*/
  //std::cin >> argStr;

  // split by ','
  std::vector<std::string> fileList;
  for (size_t i=0,n; i <= argStr.length(); i=n+1)
    {
      n = argStr.find_first_of(',',i);
      if (n == string::npos)
        n = argStr.length();
      std::string tmp = argStr.substr(i,n-i);
      std::string ttmp;
      for(unsigned int j=0; j<tmp.size(); j++)
	{
	  //if job is Grid job: filelist may not contain any additional '\n' as this would results in problems while trying to open the files
	  if(tmp[j]==' ' || tmp[j]=='\n') continue;
	  ttmp+=tmp[j];
	}
      fileList.push_back(ttmp);
    }

  // open input files
  TChain * chain = new TChain("susy","");
  for (unsigned int iFile=0; iFile<fileList.size(); ++iFile)
    {
      std::cout << "open " << fileList[iFile].c_str() << std::endl;
      chain->AddFile(fileList[iFile].c_str());
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Init tree

  TTree *tree;
  tree = chain;
  Init(tree);

  std::string curr_file;
  int filecount(0);

  if (fChain == 0) return;
  Long64_t nentries = fChain->GetEntriesFast();
  ///////////////////////////////////////////////////////////////////////////////////////////
  //Second part: Definitions: output file

  TString two="output.root";
  TFile outputfile(two,"recreate");
  std::ofstream ofs("output2.txt");

  //cutflow histogram...

  TH1F *histo_cutflow_muon = new TH1F("cutflow_muon","cutflow_muon", 15, 0., 15.);
  TH1F *histo_cutflow_elec = new TH1F("cutflow_elec","cutflow_elec", 15, 0., 15.);

  //counters
  double count_muon[15] = {0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
  double count_elec[15] = {0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};

  int count = 0;

  //initializing of some classes
  //egammaOQ myOQ;
  TDirectory *pwd=gDirectory;
  susyobj.initialize();
  pwd->cd();//reset correct directory  
  fakeMetEst.initialize("../data/fest_periodF_v1.root");
  pwd->cd();//reset correct directory  

  //egammaSFclass egammaSFclass; // Class used to correct MC electron reco efficiencies
  //EnergyRescaler geRescale; // Class used to rescale electron energy in data and smear electron energy in MC
  //geRescale.useDefaultCalibConstants();
  //Analysis::StacoCBScaleEffFactors gStacoCBSCF; // Class used to rescale MC muon reco efficiencies

  
  //For trigger rescaling
  TFile reweight_file("../data/EF_mu18.root", "OPEN");    //Open root file holding the two histogram
  THnSparseD* trig_numerator_hist = (THnSparseD*) reweight_file.Get("passes");    //Get the numerator histogram
  THnSparseD* trig_denominator_hist = (THnSparseD*) reweight_file.Get("trials");    //Get the denominator histogram

  TrigWeighter = new APReweightND(trig_denominator_hist, trig_numerator_hist, true);    //Instantiate the trigger weighter

  delete trig_denominator_hist;    //Cleanup input numerator histogram (not needed anymore)
  delete trig_numerator_hist;    //Cleanup input denominator histogram (not needed anymore)
  reweight_file.Close();    //Close input file (not needed anymore)

  pwd->cd();//reset correct directory  
  
  //data kIsData =1; MC kIsData=0;
  int kIsData = 0;

   //TTree *newtree = fChain->CloneTree(0);
   /*
   //Load the "map":
   TDirectory *pwd=gDirectory;
   myOQ.initialize();
   if (kIsData == 1) 
   {
   myOQ.egammaOQ::LoadOQMaps(RunNumber); //Now you can do it for every event because if the run is the same as the previous run, it returns immediately.
   }
   else 
   {
   myOQ.egammaOQ::LoadOQMaps(167521);
   }
   pwd->cd();
   */

   //Loop over all events  

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;

      if (curr_file!=chain->GetFile()->GetName())
	{
	  std::cout<<"New File: "<<chain->GetFile()->GetName()<<std::endl;
	  curr_file=chain->GetFile()->GetName();
	  filecount++;
	}

      //Clear arrays 
      susyobj.Reset();

      float weight = 0.;
      int myRunNumber;
      //MC:
      if (kIsData==0) {
	weight =   mcevt_weight->at(0).at(0);
	myRunNumber = 180614;
      }
      //data:
      else {
	weight = 1.;
	myRunNumber = RunNumber;
      }


      count += int(weight);
      count_muon[0] += weight;
      count_elec[0] += weight;
      if(count%10000==0) std::cout<<count<<std::endl;

      //get muons, electrons and jets - define vector of indices of good muons, jets and electrons

      vector<int> elec_index_noiso;
      vector<int> elec_index_met;
      vector<int> muon_index_noiso;
      vector<int> jet_index;
      vector<int> jet_index_signal;
      vector<int> elec_index;
      vector<int> muon_index;
      vector<int> muon_index_iso;
      vector<int> muon_index_cosmics;
      vector<int> elec_index_iso;

      histo_cutflow_muon->Fill(0.,weight);
      histo_cutflow_elec->Fill(0.,weight);

      bool trigger_muon = false;
      bool trigger_elec = false;

      //trigger
      if (!kIsData || EF_mu18)  trigger_muon = true;  //apply the trigger decision only to data - uncomment if you want to apply trigger reweighting
      //if (EF_mu18) trigger_muon = true; //MC and data the same at the moment - comment if you want to apply trigger reweighting
      if (EF_e20_medium) trigger_elec = true; 
      if (trigger_muon || trigger_elec) {

	//get electrons
	elec_index_met=get_good_electron_index(myRunNumber, kIsData);
	
	double weight_electron = weight;
	
	//MET and jet variables
	int badjet=0;
	int LArjet=0;
	
	vector<double> jetptemjes;
	float met = 0.;
	float metx = 0.;
	float mety = 0.;
	float etx_RefFinal = 0.;
	float ety_RefFinal = 0.;
	float etx_RefJet = 0.;
	float ety_RefJet = 0.;
        

	//elec_index_noiso=get_overlap_electron_index(elec_index_met);
	
	int vetocrack=0;
	
	//veto on crack electrons - do not apply for 2011 analysis
	/*
	  for(unsigned int ielec=0;ielec<elec_index_noiso.size();ielec++) {
	  if(fabs((el_cl_eta->at(elec_index_noiso[ielec])))>1.37 && fabs((el_cl_eta->at(elec_index_noiso[ielec])))<1.52)
	  vetocrack=1;
	  }
	*/
	
	
	//get jets - jets too close to electrons (Delta R < 0.2) are already rejected
	jet_index=get_good_jet_index(elec_index_met.size(),elec_index_met, kIsData); // HAVE TO MAKE OVERLAP REMOVAL BY HAND!!
	

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//get emscale for jet pt - fix for incorrect jet pt in rel15 - deactivate for rel 16 ntuples created with SUSYD3PDMaker 00-08-07 or higer
	/*
	for(unsigned int jet_ems = 0; jet_ems < jet_index.size(); jet_ems++){
	  jetptemjes.push_back(jet_AntiKt4TopoNewEM_pt->at(jet_index[jet_ems]));
	}
	*/
	///////////////////////////////////////////////////////////////////////////////
	//if jet p_t was corrected, then the jets must be sorted again according to their pt - keep relation to eta and phi of the jet - use a container for this
	vector<Container> jet; 
	
	for (unsigned int i=0; i<jet_index.size(); i++)
	  {
	    Container x;
	    x.pt=jet_AntiKt4TopoNewEM_pt->at(jet_index[i]);
	    x.phi=jet_AntiKt4TopoNewEM_phi->at(jet_index[i]);
	    x.eta=jet_AntiKt4TopoNewEM_eta->at(jet_index[i]);
	    x.index=jet_index[i];
	    jet.push_back(x);
	  }
	
	//and sort them
	
	if (jet_index.size()>0) {
	  std::sort(jet.begin(),jet.end());
	  std::reverse(jet.begin(),jet.end());		
	  
	}

	//reject jets from pileup vetices

	jet_index_signal=get_signal_jet_index(jet);
	
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Recalculation of MET
	
	//Get muons for recalculation of MET
	
	muon_index_noiso=get_good_muon_index2(kIsData);

	etx_RefFinal = MET_Simplified20_RefFinal_etx_CentralReg + MET_Simplified20_RefFinal_etx_EndcapRegion + MET_Simplified20_RefFinal_etx_ForwardReg;
	ety_RefFinal = MET_Simplified20_RefFinal_ety_CentralReg + MET_Simplified20_RefFinal_ety_EndcapRegion + MET_Simplified20_RefFinal_ety_ForwardReg;
	etx_RefJet = MET_Simplified20_RefJet_etx_CentralReg + MET_Simplified20_RefJet_etx_EndcapRegion + MET_Simplified20_RefJet_etx_ForwardReg;
	ety_RefJet = MET_Simplified20_RefJet_ety_CentralReg + MET_Simplified20_RefJet_ety_EndcapRegion + MET_Simplified20_RefJet_ety_ForwardReg;
	
	//old MET definition 
	//TVector2 SimplifiedRefFinal = susyobj.GetMET(MET_Simplified20_RefFinal_etx, MET_Simplified20_RefFinal_ety, MET_Simplified20_RefJet_etx, MET_Simplified20_RefJet_ety, MET_Simplified20_Muon_Total_Staco_etx, MET_Simplified20_Muon_Total_Staco_ety, muon_index_noiso, elec_index_met);
	
	//new MET definition (29.06.2011)
	TVector2 SimplifiedRefFinal = susyobj.GetMET(etx_RefFinal,
						     ety_RefFinal,
						     etx_RefJet,
						     ety_RefJet,
						     MET_Simplified20_Muon_Total_Staco_etx, 
						     MET_Simplified20_Muon_Total_Staco_ety,
						     muon_index_noiso, elec_index_met);
	
	metx = SimplifiedRefFinal.X();
	mety = SimplifiedRefFinal.Y();
	
	met=sqrt(metx*metx+mety*mety);

	//Get isolated electrons and muons - final analysis objects
	
	muon_index=get_good_muon_index(jet_index.size(),jet_index, muon_index_noiso/*, kIsData*/);
	muon_index_iso=get_good_muon_index_iso(muon_index);
	muon_index_cosmics = get_muon_cosmics(muon_index);
	elec_index=get_iso_electron_index(jet_index.size(),jet_index, elec_index_met/*, kIsData*/);
	elec_index_iso=get_signal_electron_index(elec_index);

	if (elec_index.size() > 0) weight_electron *= susyobj.GetTrigElecSF(el_cl_eta->at(elec_index[0]));
	
	if (trigger_muon) histo_cutflow_muon->Fill(1.,weight);
	if (trigger_elec) histo_cutflow_elec->Fill(1.,weight_electron);
	if (trigger_muon) count_muon[1] += weight;
	if (trigger_elec) count_elec[1] += weight_electron;



		//remove events with bad jets
	for(int k=0;k<jet_AntiKt4TopoNewEM_n;k++) {	  
	  if ( jet_AntiKt4TopoNewEM_pt->at(k)>20000. &&  
	       JetID::isBadJet(JetID::LooseBad,jet_AntiKt4TopoNewEM_emfrac->at(k),jet_AntiKt4TopoNewEM_hecf->at(k),
			       jet_AntiKt4TopoNewEM_LArQuality->at(k),jet_AntiKt4TopoNewEM_HECQuality->at(k),
			       jet_AntiKt4TopoNewEM_Timing->at(k),jet_AntiKt4TopoNewEM_sumPtTrk->at(k)/1000.,
			       jet_AntiKt4TopoNewEM_emscale_eta->at(k),jet_AntiKt4TopoNewEM_pt->at(k)/1000.,
			       jet_AntiKt4TopoNewEM_fracSamplingMax->at(k),jet_AntiKt4TopoNewEM_NegativeE->at(k))){

	    double dr_jet_el;
	    double diff_phi_el;

	    for(unsigned int iele=0;iele<elec_index_met.size();iele++){

	      diff_phi_el=fabs(jet_AntiKt4TopoNewEM_phi->at(k)-susyobj.GetElecTLV(elec_index_met[iele]).Phi());
	      if(diff_phi_el>3.141592653){
		diff_phi_el=diff_phi_el-(2.*3.141592653);
	      }
	      dr_jet_el=sqrt(pow((jet_AntiKt4TopoNewEM_eta->at(k)-susyobj.GetElecTLV(elec_index_met[iele]).Eta()),2)+pow(diff_phi_el,2));

	      if(dr_jet_el>0.2) badjet++;

	    }
	  } 
	  
	  //smart LAr hole veto
	  if (susyobj.IsLArHole(jet_AntiKt4TopoNewEM_eta->at(k),jet_AntiKt4TopoNewEM_phi->at(k))) {
	      if (kIsData == 1){
		if ( fakeMetEst.isBad(jet_AntiKt4TopoNewEM_pt->at(k),
					     jet_AntiKt4TopoNewEM_BCH_CORR_JET->at(k),
					     jet_AntiKt4TopoNewEM_BCH_CORR_CELL->at(k),
					     jet_AntiKt4TopoNewEM_BCH_CORR_DOTX->at(k),
					     jet_AntiKt4TopoNewEM_phi->at(k),metx,mety,10000.,10.,-1.,-1.)) LArjet++;
	      }
	      else {
		if ( fakeMetEst.isBadEmul(jet_AntiKt4TopoNewEM_pt->at(k),
						  jet_AntiKt4TopoNewEM_eta->at(k),
						  jet_AntiKt4TopoNewEM_phi->at(k),metx,mety,
						  jet_AntiKt4TopoNewEM_BCH_CORR_JET->at(k),
						  jet_AntiKt4TopoNewEM_BCH_CORR_CELL->at(k),
						  jet_AntiKt4TopoNewEM_BCH_CORR_DOTX->at(k),10000.,10.,-1.,-1.)) LArjet++;
	      }

	    }

	      /* // simple LAr hole veto
	  if ( k < 3 && susyobj.IsLArHoleVeto(jet_AntiKt4TopoNewEM_pt->at(k), 
					      jet_AntiKt4TopoNewEM_eta->at(k),
					      jet_AntiKt4TopoNewEM_phi->at(k),
					      jet_AntiKt4TopoNewEM_BCH_CORR_JET->at(k), 
					      jet_AntiKt4TopoNewEM_BCH_CORR_CELL->at(k), 
					      kIsData,40000.) )
	    LArjet++;
	      */
	  
	}
	
	
	if((kIsData == 1 && badjet==0 && larError==0) || kIsData == 0){ //jet cleaning

	  if (trigger_muon) count_muon[2] += weight;
	  if (trigger_elec) count_elec[2] += weight_electron;
	  if (trigger_muon) histo_cutflow_muon->Fill(2.,weight);
	  if (trigger_elec) histo_cutflow_elec->Fill(2.,weight_electron);


	  if ( LArjet>0 ) continue; //reject events with jets falling in the LAr hole

	  if (trigger_muon) count_muon[3] += weight;
	  if (trigger_elec) count_elec[3] += weight_electron;
	  
	  //vertex-cut
	  int vtxfound=0;
	  /*
	    for(int v=0; v<vx_n; v++){
	    if(vx_nTracks->at(v)>4) vtxfound+=1;
	    }
	  */
	  if(susyobj.IsGoodVertex(vx_nTracks)) vtxfound+=1;
	  if(vtxfound>0){
	    
	    if (trigger_muon) histo_cutflow_muon->Fill(3.,weight);
	    if (trigger_elec) histo_cutflow_elec->Fill(3.,weight_electron);
	    if (trigger_muon) count_muon[4] += weight;
	    if (trigger_elec) count_elec[4] += weight_electron;
	    
	    
	    if(vetocrack==0) {


	      ///////////////////////////////////////////////////////////////
	      
	      //1-lepton cut: 1-muon channel
	      
	      if (muon_index_cosmics.size() == 0) {
		
		if (trigger_muon) count_muon[5] += weight;
		if (trigger_elec) count_elec[5] += weight_electron;	
		//if(muon_index.size()== 1) {
		if(trigger_muon && elec_index.size()==0 && muon_index_iso.size()== 1  && muon_index.size()== 1 ){
		  //if(elec_index.size()==1 && elec_index_iso.size()==1  && muon_index.size()== 0 ){
		  //&& susyobj.GetMuonTLV(muon_index[0]).Pt() > 20000.) {
		
		//scale event:
		  if (kIsData==0) weight *= susyobj.GetSignalMuonSF(muon_index_iso[0]);
		  //if (kIsData==0) weight *= susyobj.GetSignalElecSF(el_cl_eta->at( elec_index_iso[0]),el_cl_E->at(elec_index_iso[0])/cosh(el_cl_eta->at(elec_index_iso[0])));


		//Trigger matching
		if (!kIsData || susyobj.MuonHasTriggerMatch(muon_index_iso[0],mu_staco_n,trig_EF_trigmuonef_n,trig_EF_trigmuonef_track_n,mu_staco_pt,mu_staco_eta, mu_staco_phi,trig_EF_trigmuonef_track_CB_pt,trig_EF_trigmuonef_track_CB_eta,trig_EF_trigmuonef_track_CB_phi, trig_EF_trigmuonef_track_CB_hasCB)) {

	        if(!kIsData){ // Use trigger reweighting instead of the MC trigger simulation for muons

        	        APEvtWeight weight_muon;    //Creates and event weight instance for muons with the chosen map
        	        double value_array[5] = {(*mu_staco_pt)[muon_index_iso[0]], (*mu_staco_eta)[muon_index_iso[0]], (*mu_staco_phi)[muon_index_iso[0]], (*mu_staco_ptcone20)[muon_index_iso[0]], (double) ((*mu_staco_isCombinedMuon)[muon_index_iso[0]] == true)};    //Combine the 5D object information into a temporary field
                	weight_muon.AddWeightToEvt( TrigWeighter->GetWeight(value_array) );    //Add the Weight associated with the parameters of the current object to the sum of weights for the current event
                	weight*= weight_muon.GetWeight(); //uncomment if you want to use trigger reweighting!

        	}


		  count_muon[6] += weight;
		  histo_cutflow_muon->Fill(4.,weight);



		  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		  //jet cuts
		    if (jet_index_signal.size() >= 3) {
		      //if (jet[0].pt>=60000. && jet[2].pt>=25000.) {
		      if (jet_AntiKt4TopoNewEM_pt->at(jet_index_signal[0])>=60000. && jet_AntiKt4TopoNewEM_pt->at(jet_index_signal[2])>=25000.) { 

			histo_cutflow_muon->Fill(5.,weight);
			count_muon[7] += weight;

			//Delta phi(MET, jet_i) > 0.2 cut
			double phi_met = TMath::ATan2(mety,metx);

			double dphi1 = fabs(jet[0].phi-phi_met);
			double dphi2 = fabs(jet[1].phi-phi_met);
			double dphi3 = fabs(jet[2].phi-phi_met);

                        if (dphi1 > 3.141592653) dphi1 = fabs(dphi1 - (2.*3.141592653));
			if (dphi2 > 3.141592653) dphi2 = fabs(dphi2 - (2.*3.141592653));
			if (dphi3 > 3.141592653) dphi3 = fabs(dphi3 - (2.*3.141592653));

			if(dphi1 > 0.2 && dphi2 > 0.2 && dphi3 > 0.2) {

			  count_muon[8] += weight;
			  histo_cutflow_muon->Fill(6.,weight);
			  
			  //Get M_T and M_eff
			  double mt = calculate_mt_1lepton(elec_index.size(), muon_index_iso.size(), elec_index, muon_index_iso, metx, mety, met);
			  double meff = calculate_meff(jet_index_signal, elec_index.size(), muon_index_iso.size(), elec_index, muon_index_iso, met);

			  //MET cut
			  if (met > 125000.) {
			    histo_cutflow_muon->Fill(7.,weight);
			    count_muon[9] += weight;

			    //M_T cut
			    if(mt > 100000.) {
			      histo_cutflow_muon->Fill(8.,weight);
			      count_muon[10] += weight;
			      
			      //MET>0.25 M_eff cut
			      if (met > 0.25 *meff) {
				histo_cutflow_muon->Fill(9.,weight);
				count_muon[11] += weight;
				//M_eff cut
				if (meff > 500000.) {
				  histo_cutflow_muon->Fill(10.,weight);
				  count_muon[12] += weight;
				}
			      }
			    }
			  }
			  
			}//dphi
			
		      } //jet_pt		 
		    } //jet size
		    //} //met
		    //} // mu_z0
		    //} //otx-veto
		    //} //currently end of veto on crack region
		 } //end trigger matching
		} // end of cut 1 lepton

		if(trigger_elec && elec_index.size()==1 && elec_index_iso.size()==1  && muon_index.size()== 0 ){
		  //&& susyobj.GetMuonTLV(muon_index[0]).Pt() > 20000.) {
		
		//scale event:
		  if (kIsData==0) weight_electron *= susyobj.GetSignalElecSF(el_cl_eta->at( elec_index_iso[0]),el_cl_E->at(elec_index_iso[0])/cosh(el_cl_eta->at(elec_index_iso[0])));

		  count_elec[6] += weight_electron;
		  histo_cutflow_elec->Fill(4.,weight_electron);



		  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		  //jet cuts
		    if (jet_index_signal.size() >= 3) {
		      //if (jet[0].pt>=60000. && jet[2].pt>=25000.) {
		      if (jet_AntiKt4TopoNewEM_pt->at(jet_index_signal[0])>=60000. && jet_AntiKt4TopoNewEM_pt->at(jet_index_signal[2])>=25000.) { 

			histo_cutflow_elec->Fill(5.,weight_electron);
			count_elec[7] += weight_electron;

			//Delta phi(MET, jet_i) > 0.2 cut
			double phi_met_elec = TMath::ATan2(mety,metx);

			double dphi1_elec = fabs(jet[0].phi-phi_met_elec);
			double dphi2_elec = fabs(jet[1].phi-phi_met_elec);
			double dphi3_elec = fabs(jet[2].phi-phi_met_elec);

                        if (dphi1_elec > 3.141592653) dphi1_elec = fabs(dphi1_elec - (2.*3.141592653));
			if (dphi2_elec > 3.141592653) dphi2_elec = fabs(dphi2_elec - (2.*3.141592653));
			if (dphi3_elec > 3.141592653) dphi3_elec = fabs(dphi3_elec - (2.*3.141592653));

			if(dphi1_elec > 0.2 && dphi2_elec > 0.2 && dphi3_elec > 0.2) {

			  count_elec[8] += weight_electron;
			  histo_cutflow_elec->Fill(6.,weight_electron);
			  
			  //Get M_T and M_eff
			  double mt_elec = calculate_mt_1lepton(elec_index_iso.size(), muon_index.size(), elec_index_iso, muon_index, metx, mety, met);
			  double meff_elec = calculate_meff(jet_index_signal, elec_index_iso.size(), muon_index.size(), elec_index_iso, muon_index, met);

			  //MET cut
			  if (met > 125000.) {
			    histo_cutflow_elec->Fill(7.,weight_electron);
			    count_elec[9] += weight_electron;

			    //M_T cut
			    if(mt_elec > 100000.) {
			      histo_cutflow_elec->Fill(8.,weight_electron);
			      count_elec[10] += weight_electron;

			      //MET>0.25 M_eff cut
			      if (met > 0.25 *meff_elec) {
				histo_cutflow_elec->Fill(9.,weight_electron);
				count_elec[11] += weight_electron;
				//M_eff cut
				if (meff_elec > 500000.) {
				  histo_cutflow_elec->Fill(10.,weight_electron);
				  count_elec[12] += weight_electron;
				}
			      }
			    }
			  }
			  
			}//dphi
			
		      } //jet_pt		 
		    } //jet size
		    //} //met
		    //} // mu_z0
		    //} //otx-veto
		    //} //currently end of veto on crack region
		} // end of cut 1 lepton

		}//cosmics veto
	      } //veto on crack region	      
	    } //nvertix - or - end of veto on crack region
	  } //first jet quality cut
	}//trigger
	//} //lbn
   } //end of loop
   
   pwd->cd();
   //newtree->Print();
   //newtree->AutoSave();
   cout << endl;
   std::cout<<"************* cutflow (1-muon+3jets): **************"<<std::endl;
   cout << "Events in file: " << count << endl;
   cout << "Trigger: " << count_muon[1] << endl;  
   cout << "Jet cleaning: " << count_muon[2] << endl;  
   cout << "LAr hole veto: " << count_muon[3] << endl;  
   cout << "Vertex cut: " << count_muon[4] << endl;   
   //cout << "Veto on events with electrons in the crack region: " << count_vetocrack << endl; 
   cout << "cosmics veto: " << count_muon[5] << endl;    
   cout << "1-lepton cut: " << count_muon[6] << endl; 
   cout << ">=3 jets(60,25,25): " << count_muon[7] << endl;  
   cout << "Delta phi: " << count_muon[8] << endl;  
   cout << "MET > 125 GeV: " << count_muon[9] << endl; 
   cout << "M_T > 100 GeV: " << count_muon[10] << endl; 
   cout << "MET > 0.25 Meff: " << count_muon[11] << endl;  
   cout << "Meff > 500 GeV: " << count_muon[12] << endl;   

   histo_cutflow_muon->Write();

   cout << endl;
   std::cout<<"************* cutflow (1-elec+3jets): **************"<<std::endl;
   cout << "Events in file: " << count << endl;
   cout << "Trigger: " << count_elec[1] << endl;  
   cout << "Jet cleaning: " << count_elec[2] << endl;  
   cout << "LAr hole veto: " << count_elec[3] << endl;  
   cout << "Vertex cut: " << count_elec[4] << endl;   
   //cout << "Veto on events with electrons in the crack region: " << count_vetocrack << endl; 
   cout << "cosmics veto: " << count_elec[5] << endl;    
   cout << "1-lepton cut: " << count_elec[6] << endl; 
   cout << ">=3 jets(60,25,25): " << count_elec[7] << endl;  
   cout << "Delta phi: " << count_elec[8] << endl;  
   cout << "MET > 125 GeV: " << count_elec[9] << endl; 
   cout << "M_T > 100 GeV: " << count_elec[10] << endl; 
   cout << "MET > 0.25 Meff: " << count_elec[11] << endl;  
   cout << "Meff > 500 GeV: " << count_elec[12] << endl;   

   histo_cutflow_elec->Write();

   outputfile.Close();

   //check if all files have been read 
   if(static_cast<int>(fileList.size())!=filecount)
        {
        std::cerr<<"Not all files read!"<<std::endl;
        exit(1);
        }
   else
     cout << "All ok." << endl;

} // end of Reco_dpd2::Loop


vector<int> Reco_dpd2::get_good_electron_index(int myRunNumber, int kIsData){
   vector<int> elec_index;
   for(int ielec=0;ielec<el_n;ielec++){
     if (susyobj.IsElectron(ielec,myRunNumber, 
			    el_cl_eta->at(ielec), el_cl_phi->at(ielec), el_cl_E->at(ielec), el_tracketa->at(ielec), el_trackphi->at(ielec),
			    el_author->at(ielec), el_isEM->at(ielec), el_OQ->at(ielec), el_nPixHits->at(ielec), el_nSCTHits->at(ielec),
			    kIsData)) {
       elec_index.push_back(ielec);
     }
   }
   return elec_index;
}

vector<int> Reco_dpd2::get_overlap_electron_index(vector<int> elec_index_met){
   vector<int> elec_index;
   int n_elec = elec_index_met.size();
   for(int ielec=0;ielec<n_elec;ielec++){
     if (susyobj.GetElecTLV(elec_index_met[ielec]).Pt() > 20000.) elec_index.push_back(elec_index_met[ielec]); 
   }
   return elec_index;
}



vector<int> Reco_dpd2::get_iso_electron_index(int n_jet,vector<int> jet_index, vector<int> elec_index_noiso /*, int kIsData*/){
  vector<int> elec_index;
  int n_elec = elec_index_noiso.size();
  for(int ielec=0;ielec<n_elec;ielec++){
    int overlap_removal = 0;
    for(int jeti = 0; jeti<n_jet; jeti++){
      if (susyobj.GetElecTLV(elec_index_noiso[ielec]).DeltaR(susyobj.GetJetTLV(jet_index[jeti])) <= 0.4) overlap_removal+=1;
    }
    //if (El_JetinDR04(elec_index_noiso[ielec],n_jet,jet_index) == 0)  elec_index.push_back(elec_index_noiso[ielec]); 
    if (overlap_removal == 0) elec_index.push_back(elec_index_noiso[ielec]); 
  }
  return elec_index;
}

vector<int> Reco_dpd2::get_signal_electron_index(vector<int> elec_index){
  vector<int> elec_index_iso;
  int n_elec_iso = elec_index.size();
  for(int ielec_iso=0;ielec_iso<n_elec_iso;ielec_iso++){
    if (susyobj.IsSignalElectron(elec_index[ielec_iso],el_isEM->at(elec_index[ielec_iso]),
				 el_ptcone20->at(elec_index[ielec_iso]),25000.)) 
      elec_index_iso.push_back(elec_index[ielec_iso]); 
  }
  return elec_index_iso;
}


vector<int> Reco_dpd2::get_good_muon_index(int n_jet,vector<int> jet_index, vector<int> muon_index2 /*, int kIsData*/){
  vector<int> muon_index;
  //int overlap_removal = 0;
  int n_muon = muon_index2.size();
  for(int imuon=0;imuon<n_muon;imuon++){
    //if (mu_staco_ptcone20->at(muon_index2[imuon]) < 1800. && susyobj.IsSignalMuon(mu_staco_z0_exPV->at(muon_index2[imuon])) && Mu_JetinDR04(muon_index2[imuon],n_jet,jet_index) == 0) {
    //if (Mu_JetinDR04(muon_index2[imuon],n_jet,jet_index) == 0  && susyobj.GetMuonTLV(muon_index2[imuon]).Pt() > 20000.) {
    if (Mu_JetinDR04(muon_index2[imuon],n_jet,jet_index) == 0) {
      //for(int jeti = 0; jeti<n_jet; jeti++){
      //	 if (susyobj.GetMuonTLV(muon_index2[imuon]).DeltaR(susyobj.GetJetTLV(jet_index[jeti])) <= 0.4) overlap_removal+=1;
      //}
      //if (overlap_removal == 0) muon_index.push_back(muon_index2[imuon]); 
      muon_index.push_back(muon_index2[imuon]);
    }
  }
  return muon_index;
}

vector<int> Reco_dpd2::get_muon_cosmics(vector<int> muon_index){
  vector<int> muon_index_cosmics;
  int n_muon = muon_index.size();
  for(int imuon=0;imuon<n_muon;imuon++){
    if (susyobj.IsCosmicMuon(mu_staco_z0_exPV->at(muon_index[imuon]),mu_staco_d0_exPV->at(muon_index[imuon]),5.,2.)) {
      muon_index_cosmics.push_back(muon_index[imuon]);
    }
  }
  return muon_index_cosmics;
}

vector<int> Reco_dpd2::get_good_muon_index_iso(vector<int> muon_index){
  vector<int> muon_index_iso;
  int n_muon = muon_index.size();
  for(int imuon=0;imuon<n_muon;imuon++){
    if (susyobj.IsSignalMuon(muon_index[imuon],mu_staco_ptcone20->at(muon_index[imuon]))) {
      muon_index_iso.push_back(muon_index[imuon]);
    }
  }
  return muon_index_iso;
}


vector<int> Reco_dpd2::get_good_muon_index2(int kIsData){
  vector<int> muon_index2;
  for(int imuon=0;imuon<mu_staco_n;imuon++){
    if (susyobj.IsMuon(imuon,// EventNumber,
		       mu_staco_pt->at(imuon), mu_staco_eta->at(imuon), mu_staco_phi->at(imuon), mu_staco_E->at(imuon),
		       //Renaud mu_staco_qoverp_exPV->at(imuon), mu_staco_cov_qoverp_exPV->at(imuon),
		       mu_staco_me_qoverp_exPV->at(imuon), mu_staco_id_qoverp_exPV->at(imuon),
		       mu_staco_me_theta_exPV->at(imuon), mu_staco_id_theta_exPV->at(imuon),  mu_staco_id_theta->at(imuon),
		       mu_staco_isCombinedMuon->at(imuon), mu_staco_isLowPtReconstructedMuon->at(imuon),
		       mu_staco_loose->at(imuon),
		       mu_staco_expectBLayerHit->at(imuon), mu_staco_nBLHits->at(imuon), mu_staco_nPixHits->at(imuon),
		       mu_staco_nPixelDeadSensors->at(imuon), mu_staco_nPixHoles->at(imuon), mu_staco_nSCTHits->at(imuon),
		       mu_staco_nSCTDeadSensors->at(imuon), mu_staco_nSCTHoles->at(imuon), mu_staco_nTRTHits->at(imuon), 
		       mu_staco_nTRTOutliers->at(imuon),
		       kIsData,10000.,2.4)) muon_index2.push_back(imuon);
  }
  return muon_index2;
}

vector<int> Reco_dpd2::get_good_jet_index(int n_elec,vector<int> elec_index, int kIsData){
  vector<int> jet_index;
  for(int ijet=0;ijet<jet_AntiKt4TopoNewEM_n;ijet++){
    if (get_jet(ijet,n_elec,elec_index, kIsData)==1) jet_index.push_back(ijet);
  }
  return jet_index;
}

vector<int> Reco_dpd2::get_signal_jet_index(vector<Container> jet){
  vector<int> jet_index_signal;
  int n_jet = jet.size();
  for(int ijet=0;ijet<n_jet;ijet++){
    if (fabs(jet_AntiKt4TopoNewEM_jvtxf->at(jet[ijet].index)) > 0.75) jet_index_signal.push_back(jet[ijet].index);
  }
  return jet_index_signal;
}


int Reco_dpd2::Mu_JetinDR04(int imuon,int n_jet,vector<int> jet_index){ 

  double dr_jet;
  double diff_phi;
  vector<double> jetptemjes;
  for(int ijet=0;ijet<n_jet;ijet++){
    diff_phi=fabs((*jet_AntiKt4TopoNewEM_phi)[jet_index[ijet]]-((*mu_staco_phi)[imuon]));
    if(diff_phi>3.141592653){
      diff_phi=diff_phi-(2.*3.141592653);
    }
    dr_jet=sqrt(pow(((*jet_AntiKt4TopoNewEM_eta)[jet_index[ijet]]-((*mu_staco_eta)[imuon])),2)+pow(diff_phi,2)); 
    if(dr_jet<=0.4) return 1;	
  }
  return 0;
}

int Reco_dpd2::El_JetinDR04(int ielec,int n_jet,vector<int> jet_index){ 
  
  double dr_jet;
  double diff_phi;
  vector<double> jetptemjes;
  
  for(int ijet=0;ijet<n_jet;ijet++){
    diff_phi=fabs((*jet_AntiKt4TopoNewEM_phi)[jet_index[ijet]]-((*el_phi)[ielec]));
    if(diff_phi>3.141592653){
      diff_phi=diff_phi-(2.*3.141592653);
    }
    dr_jet=sqrt(pow(((*jet_AntiKt4TopoNewEM_eta)[jet_index[ijet]]-((*el_eta)[ielec])),2)+pow(diff_phi,2));
    if(dr_jet<=0.4 && dr_jet>0.2) return 1;
    
  }
  return 0;
}


int Reco_dpd2::get_jet(int ijet, int n_elec, vector<int> elec_index, int kIsData){
  
  if (susyobj.IsJet(ijet, jet_AntiKt4TopoNewEM_pt->at(ijet), jet_AntiKt4TopoNewEM_eta->at(ijet), jet_AntiKt4TopoNewEM_phi->at(ijet),
		    jet_AntiKt4TopoNewEM_E->at(ijet), jet_AntiKt4TopoNewEM_emscale_eta->at(ijet), jet_AntiKt4TopoNewEM_emfrac->at(ijet), 
		    jet_AntiKt4TopoNewEM_hecf->at(ijet), jet_AntiKt4TopoNewEM_LArQuality->at(ijet), jet_AntiKt4TopoNewEM_HECQuality->at(ijet), 
		    jet_AntiKt4TopoNewEM_Timing->at(ijet), jet_AntiKt4TopoNewEM_sumPtTrk->at(ijet), jet_AntiKt4TopoNewEM_fracSamplingMax->at(ijet),
		    jet_AntiKt4TopoNewEM_NegativeE->at(ijet), kIsData,20000.,10.) 
      && jet_AntiKt4TopoNewEM_pt->at(ijet) > 20000. && fabs(jet_AntiKt4TopoNewEM_eta->at(ijet)) < 2.8) {
    double dr_jet;
    double diff_phi;
    for(int iele=0;iele<n_elec;iele++){
      diff_phi=fabs((*jet_AntiKt4TopoNewEM_phi)[ijet]-susyobj.GetElecTLV(elec_index[iele]).Phi());
      if(diff_phi>3.141592653){
	diff_phi=diff_phi-(2.*3.141592653);
      }
      dr_jet=sqrt(pow(((*jet_AntiKt4TopoNewEM_eta)[ijet]-susyobj.GetElecTLV(elec_index[iele]).Eta()),2)+pow(diff_phi,2));
      if(dr_jet<=0.2) return 0;
    }
    
    return 1;
  }
  return 0;
}


double Reco_dpd2::calculate_meff(vector<int> jet, int n_elec, int n_muon, vector<int> elec_index, vector<int> muon_index, double met){

  double meff=met;
  int maxjet;
  if(jet.size()>=3) maxjet=3;
  else maxjet=jet.size();
  for(int jet0=0;jet0<maxjet;jet0++) {
    //meff+=jet[jet0].pt;
    meff+=jet_AntiKt4TopoNewEM_pt->at(jet[jet0]);
  }
  for(int el=0;el<n_elec;el++) {
    TLorentzVector tlelec = susyobj.GetElecTLV(elec_index[el]);
    meff += tlelec.Pt();
  }
  for(int mu=0;mu<n_muon;mu++) {
    TLorentzVector tlmuon = susyobj.GetMuonTLV(muon_index[mu]);
    meff += tlmuon.Pt();
  }
  return meff;
}




double Reco_dpd2::calculate_mt_1lepton(int n_elec, int n_muon, vector<int> elec_index, vector<int> muon_index, double metx, double mety, double met){
  
  double mt=0.; 
  if(n_muon==1) {
    TLorentzVector tlmuon = susyobj.GetMuonTLV(muon_index[0]);
    double p1p2=( (tlmuon.Px())*metx+(tlmuon.Py())*mety );
    double cosphi=p1p2/(tlmuon.Pt()*met);
    mt=sqrt(2*tlmuon.Pt()*met*(1-cosphi));
  }
  else if(n_elec==1) {
    TLorentzVector tlelec = susyobj.GetElecTLV(elec_index[0]);
    double p1p2=( (tlelec.Px())*metx+(tlelec.Py())*mety );
    double cosphi=p1p2/(tlelec.Pt()*met);
    mt=sqrt(2*tlelec.Pt()*met*(1-cosphi));	  
  }
  else std::cout << "ERROR !!! The number of lepton is incorrect in MT calculation!" <<std::endl;
  
  return mt;
}


