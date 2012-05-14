/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//This macro prints the cutflow for the 1-muon channel (later also 1-electron channel)
//It refers to the SUSYObjDef class
//author: Sophie Henrot-Versille


#define Reco0Lep_cxx
#include "SUSYTools/Reco0Lep.h"
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


using namespace std;
//using namespace SUSY;

//gROOT->Reset();
/*
  int main(){
  Reco0Lep t;
  t.Loop();

  return EXIT_SUCCESS;
  }
*/


///////////////////////////////////////////////////////////////////////////////////////////////////////////
//main program

void Reco0Lep::Loop(int ana,std::string argStr)
{


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
      //      std::cout << "open " << fileList[iFile].c_str() << std::endl;
      chain->AddFile(fileList[iFile].c_str());
    }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////
  //Init tree

  TTree *tree;
  tree = chain;
  Init(tree);
  //cout << "init ok " << endl;
 
#ifdef USEPILEUP
  //   ROOT.gSystem.Load('libPileupReweighting.so')
  m_tPileUp = new Root::TPileupReweighting( "MyPileUpReweightingTool" );
  // All four arguments are of type TString
  m_dataRootFileName="susy_0Lepton_avgintperbx_GRLv18.root";
  m_dataRootHistName="avgintperbx";
  m_mcRootFileName="mu_mc10b.root";
  m_mcRootHistName="mu_mc10b";
  
  int isGood =0;
  isGood = m_tPileUp->initialize( m_dataRootFileName,
				  m_dataRootHistName,
				  m_mcRootFileName,
				  m_mcRootHistName );
  if(isGood!=0) cout << "problem in Pileup initialization::isGood=" << isGood << endl;

#endif
#ifdef USEGRL
   Root::TGoodRunsListReader* grlR = new Root::TGoodRunsListReader();
   grlR->SetXMLFile("data11_7TeV.periodAllYear_DetStatus-v17-pro08-03_Susy.xml"); //<---- for instance choose the one you want !
   grlR->Interpret();
   m_grl = grlR->GetMergedGoodRunsList();
   m_grl.Summary();
#endif 

   egammaOQ m_myOQ; 
  std::string curr_file;
  int filecount(0);

  if (fChain == 0) return;
  Long64_t nentries = fChain->GetEntriesFast();
  ///////////////////////////////////////////////////////////////////////////////////////////
  //Second part: Definitions: output file

  TString two="output.root";
  TFile outputfile(two,"recreate");

  //cutflow histogram...

  TH1F *histo_cutflow = new TH1F("cutflow","cutflow", 15, 0., 15.);

  //counters
  double count_vetocrack = 0.;
  double countlar = 0.;
  double countlarhole = 0.;
  double counta = 0.;
  double count_mucos = 0.;
  double countc = 0.;
  double countd = 0.;
  double count00ana2 = 0.;
  double count00ana3 = 0.;
  double count_jet1 = 0.;
  double count_jet2ana2 = 0.;
  double count_jet2ana3 = 0.;
  double count_jet3ana2 = 0.;
  double count_jet3ana33 = 0.;
  double count_jet3ana34 = 0.;
  double count_jet3ana35 = 0.;
  double count_jet3ana36 = 0.;
  double count_new = 0.;
  double count_meffana33 = 0.;
  double count_meffana34 = 0.;
  double count = 0;
  double countOri = 0;
  double countelec =0;
  double countmuon =0;
  double count_grl=0;
  double count_grlnopu=0;

  double countlarnopu =0.;
  double countlarholenopu =0.;
  double countanopu =0.;
  double countcnopu =0.;
  double countdnopu =0.;
  double count00ana2nopu =0.;
  double count00ana3nopu =0.;
  double count_jet1nopu =0.;
  double count_jet2ana2nopu =0.;
  double count_jet2ana3nopu =0.;
  double count_jet3ana2nopu =0.;
  double count_jet3ana33nopu =0.;
  double count_jet3ana34nopu =0.;
  double count_jet3ana35nopu =0.;
  double count_jet3ana36nopu =0.;
  double count_newnopu =0.;
  double count_meffana33nopu =0.;
  double count_meffana34nopu =0.;

  double count_metana2nopu =0.;
  double count_meffana2nopu =0.;
  double count_metana2 =0.;
  double count_meffana2 =0.;
  double count_metana3nopu = 0.;
  double count_metana3= 0.;
 
  //initializing of some classes
  TDirectory *pwd=gDirectory;
  susyobj.initialize();
  pwd->cd();

  int kIsData = 0;

  //Loop over all events  

  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    //    cout << jentry << endl;
   Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;

    if (curr_file!=chain->GetFile()->GetName())
      {
	//	std::cout<<"New File: "<<chain->GetFile()->GetName()<<std::endl;
	curr_file=chain->GetFile()->GetName();
	filecount++;
      }

    //Clear arrays 
    susyobj.Reset();

    float weight = 0.;
    float weightnopu = 0.;
    int myRunNumber;
    //MC:
    if (kIsData==0) {
      weight =   (*mcevt_weight)[0][0];//->at(0)(0);
      //      myRunNumber = 167521;
      myRunNumber = 180614;
    }
    //data:
    else {
      weight = 1.;
      myRunNumber = 180614;//RunNumber;
    }
    countOri+=weight;
    //cout << "pppppppppppp" << kIsData << " " << weight <<  endl;
    weightnopu=weight;
#ifdef USEPILEUP
    float pileupEventWeight(-1.0);
    float mu=lbn;
 
    if(!m_tPileUp) cout << "problem222" << endl;
    int isGood=0;
    isGood = m_tPileUp->getPileupWeight( mu, pileupEventWeight );
    if(kIsData==0) weight*=pileupEventWeight; 
    
#endif
    //cout << weight << endl;
    count += weight;
    //    if(count%10000==0) std::cout<<count<<std::endl;

    //get muons, electrons and jets - define vector of indices of good muons, jets and electrons
    bool grlcheck=true;
#ifdef USEGRL
    if(kIsData==1) grlcheck= m_grl.HasRunLumiBlock(RunNumber,lbn);
#endif
    
    if(grlcheck){
      //cout << weight << endl;
    count_grl += weight;
    count_grlnopu += weightnopu;
  
    vector<int> elec_index_noiso;
    vector<int> elec_index_met;
    vector<int> muon_index_noiso;
    vector<int> muon_index_forMET;
    vector<int> jet_index;
    vector<int> jet_indexForCut;
    vector<int> elec_index;
    vector<int> muon_index;
    vector<int> muon_index10;

    histo_cutflow->Fill(0.,weight);

    //trigger
    if (kIsData==0||(kIsData==1&&EF_j75_a4tc_EFFS_xe45_loose_noMu))
      {
	histo_cutflow->Fill(1.,weight);
	counta += weight;
	countanopu += weightnopu;
	//cout <<"trigger" <<  weight << endl;
	
	//MET and jet variables
	int badjet=0;
	
	vector<double> jetptemjes;
	float met = 0.;
	float metx = 0.;
	float mety = 0.;
	//get electrons - first electrons for veto events with electrons in crack region
	elec_index_met=get_good_electron_index(kIsData,myRunNumber);
	elec_index_noiso=get_overlap_electron_index(elec_index_met);
	
	int vetocrack=0;
	
	//get jets - jets to close to electrons (Delta R < 0.2) are already rejected
	jet_index=get_good_jet_index(elec_index_noiso.size(),elec_index_noiso, kIsData); 
        jet_indexForCut=get_good_jet_indexForCut(elec_index_noiso.size(),elec_index_noiso, kIsData); 
        
 	for(int k=0;k<(int)jet_indexForCut.size();k++) {
 	  if ( kIsData==1&&jet_AntiKt4TopoNewEM_pt->at(jet_indexForCut[k])>20000.  && JetID::isBadJet(JetID::LooseBad, jet_AntiKt4TopoNewEM_emfrac->at(jet_indexForCut[k]), jet_AntiKt4TopoNewEM_hecf->at(jet_indexForCut[k]), jet_AntiKt4TopoNewEM_LArQuality->at(jet_indexForCut[k]), jet_AntiKt4TopoNewEM_HECQuality->at(jet_indexForCut[k]), jet_AntiKt4TopoNewEM_Timing->at(jet_indexForCut[k]), jet_AntiKt4TopoNewEM_sumPtTrk->at(jet_indexForCut[k])/1000., jet_AntiKt4TopoNewEM_emscale_eta->at(jet_indexForCut[k]),jet_AntiKt4TopoNewEM_pt->at(jet_indexForCut[k])/1000., jet_AntiKt4TopoNewEM_fracSamplingMax->at(jet_indexForCut[k]), jet_AntiKt4TopoNewEM_NegativeE->at(jet_indexForCut[k]))) badjet = 1;
 	}
	
	
	if(vetocrack==0) {
	  count_vetocrack+=weight;
	  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	  //get emscale for jet pt - fix for incorrect jet pt in rel15 - deactivate for rel 16 ntuples created with SUSYD3PDMaker 00-08-07 or higer
	  
	  for(unsigned int jet_ems = 0; jet_ems < jet_index.size(); jet_ems++){
	    jetptemjes.push_back(jet_AntiKt4TopoNewEM_pt->at(jet_index[jet_ems]));
	  }
	  
	  ///////////////////////////////////////////////////////////////////////////////
	  //jets must be sorted again according to their pt - keep relation to eta and phi of the jet - use a container for this
	  vector<Container> jet; 
	      
	  for (unsigned int i=0; i<jetptemjes.size(); i++)
	    {
	      Container x;
	      x.pt=jetptemjes.at(i);
	      x.phi=jet_AntiKt4TopoNewEM_phi->at(jet_index[i]);
	      x.eta=jet_AntiKt4TopoNewEM_eta->at(jet_index[i]);
	      jet.push_back(x);
	    }
	  
	  //and sort them
	  
	  if (jetptemjes.size()>0) {
	    std::sort(jet.begin(),jet.end());
	    std::reverse(jet.begin(),jet.end());		
	    
	  }
	  
	  //Recalculation of MET
	  //Get muons for recalculation of MET
	  muon_index_noiso=get_good_muon_index2(kIsData,20000);
	  muon_index_forMET=get_good_muon_index2(kIsData,10000);

	  double etx_RefFinal = MET_Simplified20_RefFinal_etx_CentralReg + MET_Simplified20_RefFinal_etx_EndcapRegion + MET_Simplified20_RefFinal_etx_ForwardReg;
	  double ety_RefFinal = MET_Simplified20_RefFinal_ety_CentralReg + MET_Simplified20_RefFinal_ety_EndcapRegion + MET_Simplified20_RefFinal_ety_ForwardReg;
	  double etx_RefJet = MET_Simplified20_RefJet_etx_CentralReg + MET_Simplified20_RefJet_etx_EndcapRegion + MET_Simplified20_RefJet_etx_ForwardReg;
	  double ety_RefJet = MET_Simplified20_RefJet_ety_CentralReg + MET_Simplified20_RefJet_ety_EndcapRegion + MET_Simplified20_RefJet_ety_ForwardReg;
	  
	  TVector2 metv = susyobj.GetMET(etx_RefFinal,ety_RefFinal,etx_RefJet,ety_RefJet,MET_Simplified20_Muon_Total_Staco_etx,MET_Simplified20_Muon_Total_Staco_ety,muon_index_forMET, elec_index_met,false);
	  metx = metv.X();
	  mety = metv.Y();
	  met=sqrt(metx*metx+mety*mety);
	  
	  //Get isolated electrons and muons - final analysis objects
	  muon_index=get_good_muon_index(jet_index.size(),jet_index, muon_index_noiso);
	  muon_index10=get_good_muon_index(jet_index.size(),jet_index, muon_index_forMET);
	  elec_index=get_iso_electron_index(jet_index.size(),jet_index, elec_index_noiso);
	  
	  //0 lepton cut no elec nor muon
	  if(elec_index.size()!=0) countelec+=weight;

	  int n_muon = muon_index.size();
	  int keep=1;
	  
	  for(int imuon=0;imuon<n_muon;imuon++){
	    if (susyobj.IsCosmicMuon(mu_staco_z0_exPV->at(muon_index[imuon]),mu_staco_d0_exPV->at(muon_index[imuon]),1.,.2)) keep=0;
	  }
	  
	  if(muon_index.size()!=0&&keep==1) countmuon+=weight;
	  
	  if((badjet==0&&kIsData==1) || kIsData==0){
	    count_jet1 += weight;
	    count_jet1nopu += weightnopu;
	    histo_cutflow->Fill(2.,weight);
	    
	    int chferror=0;
	    // analysis cutA
	    for(int o=0;o<(int)jet.size();o++){
	      int test=0;
	      double premier=jet_AntiKt4TopoNewEM_sumPtTrk->at(jet_index[o])/jet_AntiKt4TopoNewEM_pt->at(jet_index[o]);
	      if(premier<0.05&&fabs(jet_AntiKt4TopoNewEM_eta->at(jet_index[o]))<2&&jet_AntiKt4TopoNewEM_pt->at(jet_index[o])>100000) test=1;
	      if(ana==2&&o<2) chferror+=test;
	      if(ana==3&&o<3) chferror+=test;
	      if(ana>3&&o<4) chferror+=test;
	    }
	    //	    if(larError!=0) cout << EventNumber << endl;
		float time2j=-999;
		float time3j=-999;
		float time4j=-999;
		if(jet_index.size()>=2)
		  time2j = (jet_AntiKt4TopoNewEM_E->at(jet_index[0])*jet_AntiKt4TopoNewEM_Timing->at(jet_index[0])+
			    jet_AntiKt4TopoNewEM_E->at(jet_index[1])*jet_AntiKt4TopoNewEM_Timing->at(jet_index[1]))
		    /(jet_AntiKt4TopoNewEM_E->at(jet_index[0])+jet_AntiKt4TopoNewEM_E->at(jet_index[1]));
		
		if(jet_index.size()>=3)
		  time3j = (jet_AntiKt4TopoNewEM_E->at(jet_index[0])*jet_AntiKt4TopoNewEM_Timing->at(jet_index[0])+
			    jet_AntiKt4TopoNewEM_E->at(jet_index[1])*jet_AntiKt4TopoNewEM_Timing->at(jet_index[1])+
			    jet_AntiKt4TopoNewEM_E->at(jet_index[2])*jet_AntiKt4TopoNewEM_Timing->at(jet_index[2]))/(jet_AntiKt4TopoNewEM_E->at(jet_index[0])+jet_AntiKt4TopoNewEM_E->at(jet_index[1])+jet_AntiKt4TopoNewEM_E->at(jet_index[2]));
		
		if(jet_index.size()>=4)
		  time4j = (jet_AntiKt4TopoNewEM_E->at(jet_index[0])*jet_AntiKt4TopoNewEM_Timing->at(jet_index[0])+
			    jet_AntiKt4TopoNewEM_E->at(jet_index[1])*jet_AntiKt4TopoNewEM_Timing->at(jet_index[1])+
			    jet_AntiKt4TopoNewEM_E->at(jet_index[2])*jet_AntiKt4TopoNewEM_Timing->at(jet_index[2])+
			    jet_AntiKt4TopoNewEM_E->at(jet_index[3])*jet_AntiKt4TopoNewEM_Timing->at(jet_index[3]))
		    /(jet_AntiKt4TopoNewEM_E->at(jet_index[0])+jet_AntiKt4TopoNewEM_E->at(jet_index[1])+jet_AntiKt4TopoNewEM_E->at(jet_index[2])+jet_AntiKt4TopoNewEM_E->at(jet_index[3]));
		
		//	cout << time3j << endl;

	      int checklarhole=0;
	      for(int i=0;i<(int)jet_index.size();i++)
		{
		  if(i<=3&&susyobj.IsLArHoleVeto(jet_AntiKt4TopoNewEM_pt->at(jet_index[i]),
						 jet_AntiKt4TopoNewEM_eta->at(jet_index[i]),
						 jet_AntiKt4TopoNewEM_phi->at(jet_index[i]),
						 jet_AntiKt4TopoNewEM_BCH_CORR_JET->at(jet_index[i]),
						 jet_AntiKt4TopoNewEM_BCH_CORR_CELL->at(jet_index[i]),
						 kIsData,40000.,RunNumber)) checklarhole=1;
		}
	      
	      if(checklarhole==0)
		{
		  countlarhole += weight;
		  countlarholenopu += weightnopu;
	    if(kIsData==0||(kIsData==1&&larError==0))	    {
	      countlar += weight;
	      countlarnopu += weightnopu;
 
		{
		  //	      histo_cutflow->Fill(12.,weight);
	      {
		//vertex-cut
		int vtxfound=0;
		for(int v=0; v<1; v++){
		  if(vx_nTracks->at(v)>4) vtxfound+=1;
		}
		if(vtxfound>0){
		  
		  if(keep==1) count_mucos+=weight;
		  histo_cutflow->Fill(3.,weight);
		  countc += weight;
		  countcnopu += weightnopu;
		  if(keep==1&&elec_index.size()==0 && muon_index.size()== 0){
		    
		    
		
		countd += weight;
		countdnopu += weightnopu;
		histo_cutflow->Fill(4.,weight);
		
		if((ana==3&&fabs(time3j)<5)||(ana==2&&fabs(time2j)<5)||(ana>=4&&fabs(time4j)<5)){
		if (chferror==0&&jet_index.size() >= 3) {
		  if (jet[0].pt>130000. && jet[1].pt>=40000.&& jet[2].pt>=40000.&&ana>=3) {
		    //cout << EventNumber << endl;
		    if(ana==3) histo_cutflow->Fill(5.,weight);
		    count00ana3 += weight;
		    count00ana3nopu += weightnopu;
		    
		    if (met > 130000.) {
		      count_metana3 += weight;
		      count_metana3nopu += weightnopu;
		      if(ana==3) histo_cutflow->Fill(6.,weight);
		      
		      //Delta phi(MET, jet_i) > 0.2 cut
		      double phi_met = TMath::ATan2(mety,metx);
		      
		      double dphi1 = fabs(jet[0].phi-phi_met);
		      double dphi2 = fabs(jet[1].phi-phi_met);
		      double dphi3 = fabs(jet[2].phi-phi_met);
		      
		      if (dphi1 > 3.141592653) dphi1 = fabs(dphi1 - (2.*3.141592653));
		      if (dphi2 > 3.141592653) dphi2 = fabs(dphi2 - (2.*3.141592653));
		      if (dphi3 > 3.141592653) dphi3 = fabs(dphi3 - (2.*3.141592653));
			  
		      if(dphi1 > 0.4 && dphi2 > 0.4 && dphi3 > 0.4) {
			if(ana==3) histo_cutflow->Fill(7.,weight);
			count_jet2ana3+=weight;
			count_jet2ana3nopu+=weightnopu;
			

			//Get M_T and M_eff
			double meff3 = calculate_meff(jet, met,3);
			double meff4 = 0.;
			double meffincl = calculate_meffincl(jet, met,jet.size());
			// ana = 2 cut A
			// ana = 3 cut B
			// ana = 4 cut C
			// ana = 5 cut D
			// ana = 6 cut E
			
			//CUT B
			{
			  //MET>0.25 M_eff cut
			  if (met > 0.25 *meff3) {
			    if(ana==3) histo_cutflow->Fill(9.,weight);
			    if(ana==3) { count_meffana33 += weight;
			      count_meffana33nopu += weightnopu;}
			    if (meff3 > 1000000.) {
			      if(ana==3) { count_jet3ana33+=weight;
				count_jet3ana33nopu+=weightnopu;}
			      if(ana==3) histo_cutflow->Fill(10.,weight);
			    }
			  }
			}
			
			if(jet.size()>=4&&jet[3].pt>40000.){
			  //CUT C
			  meff4 = calculate_meff(jet, met,4);
			  if (met > 0.25 *meff4) {
			    if(ana==4) histo_cutflow->Fill(9.,weight);
			    if(ana==4) { count_meffana34 += weight;
			      count_meffana34nopu += weightnopu;}
			    if (meff4 > 500000.) {
			      if(ana==4) {count_jet3ana34+=weight;
				count_jet3ana34nopu+=weightnopu;}
			      if(ana==4) histo_cutflow->Fill(10.,weight);
			    }
			    if (meff4 > 1000000.) {
			      if(ana==5) {count_jet3ana35+=weight;
				count_jet3ana35nopu+=weightnopu;}
			      if(ana==5) histo_cutflow->Fill(10.,weight);
			    }
			  }
			  if(jet[3].pt>80000.&&met > 0.2 *meff4){
			    if(ana==6) {count_new+=weight;
			      count_newnopu+=weightnopu;}
			    
			    if (meffincl > 1100000.) {
			      if(ana==6) {count_jet3ana36+=weight;
				count_jet3ana36nopu+=weightnopu;}
			      if(ana==6) histo_cutflow->Fill(10.,weight);
			    }
			    
			  }
			}
		      }
		    }//dphi
		      
		  } //jet_pt		 
		} //jet size
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//jet cuts
		if (chferror==0&&jet_index.size() >= 2) {
		  if (jet[0].pt>=130000. && jet[1].pt>=40000.) {
		    
		    if(ana==2) histo_cutflow->Fill(5.,weight);
		    if(ana==2) {count00ana2 += weight;
		      count00ana2nopu += weightnopu;}
		    
		    
		    if (met > 130000.) {
		      if(ana==2) {
			count_metana2 += weight;
			count_metana2nopu += weightnopu;}
		      if(ana==2) histo_cutflow->Fill(6.,weight);
			  
		      //Delta phi(MET, jet_i) > 0.2 cut
		      double phi_met = TMath::ATan2(mety,metx);
			  
		      double dphi1 = fabs(jet[0].phi-phi_met);
		      double dphi2 = fabs(jet[1].phi-phi_met);
		      double dphi3 = 0;
		      if (dphi1 > 3.141592653) dphi1 = fabs(dphi1 - (2.*3.141592653));
		      if (dphi2 > 3.141592653) dphi2 = fabs(dphi2 - (2.*3.141592653));
		      bool dphicut=false;
		      
		      if ( dphi1 > 0.4 && dphi2 > 0.4 ) { dphicut=true;};
		      if(jet_index.size()>2&&jet[2].pt>40000) 
			{
			  dphi3=fabs(jet[2].phi-phi_met);
			  if (dphi3 > 3.141592653) dphi3 = fabs(dphi3 - (2.*3.141592653));
			  if (dphi3<0.4) dphicut=false;
			}
		      
		      if(dphicut){
			if(ana==2) histo_cutflow->Fill(7.,weight);
			if(ana==2) {count_jet2ana2+=weight;
			  count_jet2ana2nopu+=weightnopu;}
			
			
			//Get M_T and M_eff
			double meff = calculate_meff(jet, met,2);
			
			//MET cut
			{
			  if (met > 0.3 *meff) {
			    if(ana==2) histo_cutflow->Fill(9.,weight);
			    if(ana==2) {count_meffana2 += weight;
			      count_meffana2nopu += weightnopu;}
			    //M_eff cut
			    if (meff > 1000000.) {
			      if(ana==2){ count_jet3ana2+=weight;
				count_jet3ana2nopu+=weightnopu;}
			      if(ana==2) histo_cutflow->Fill(10.,weight);
			    }
			  }
			}
		      }
		    }
		  } 
		}}
		} }
		}
	      }
	    }
	    }
	  }
	}
      }
    }
  }

  //newtree->Print();
  //newtree->AutoSave();
  cout << endl;
  string b;
  if(ana==2) b="A";
  if(ana==3) b="B";
  if(ana==4) b="C";
  if(ana==5) b="D";
  if(ana==6) b="E";
  
  std::cout<<"************* cutflow 0 leptons and CUT:" << b << " **************"<<std::endl;
  cout << "Events in file:                    " << countOri << endl;
  cout << "PU :                    " << count << endl;
  cout << "GRL :                              " << count_grlnopu << " " << count_grl << endl;
  cout << "Trigger:                           " << countanopu << " " << counta << endl;  
  cout << "Jet cleaning:                      " << count_jet1nopu << " " <<count_jet1 << endl;
  cout << "LarHole:                          " << countlarholenopu << " " << countlarhole << endl;
  cout << "LarError:                          " << countlarnopu <<  " " << countlar << endl;
  cout << "Vertex cut:                        " << countcnopu << " " << countc << endl;   
  //  cout << "crack Veto cut:                    " << count_vetocrack << endl; 
  cout << "CosmicMuons" << count_mucos << endl;
  cout << "0 lepton: no muon nor elec:        " << countdnopu << " " << countd << endl;
  cout << "Elect:                             " << countelec << endl;
  cout << "Muon:                              " << countmuon << endl;
  cout << endl;
  if(ana==2){
    cout << "Cut A:: >=2 jets(130,40):          " << count00ana2nopu << " " << count00ana2 << endl;  
    cout << "Cut A:: MET > 130 GeV:             " << count_metana2nopu << " "  <<count_metana2 << endl; 
    cout << "Cut A:: Delta phi:                 " << count_jet2ana2nopu << " " << count_jet2ana2  << endl;  
    //   cout << "Cut A:: M_T > 100 GeV: " << count_mtana2 << endl; 
    cout << "Cut A:: MET > 0.3 Meff:            " << count_meffana2nopu<< " " <<count_meffana2 << endl;  
    cout << "Cut A:: Meff > 1000 GeV:           " << count_jet3ana2nopu << " " << count_jet3ana2 << endl;   
    cout << endl;
  }
  if(ana>=3){
    cout << "CutBCDE:: >=3 jets(130,40,40):     " << count00ana3nopu << " " <<  count00ana3 << endl;  
    cout << "CutBCDE:: MET > 130 GeV:           " << count_metana3nopu << " " << count_metana3 << endl; 
    cout << "CutBCDE:: Delta phi :              " << count_jet2ana3nopu << " " << count_jet2ana3 << endl; 
    cout << endl;
  }
  if(ana==3){
    cout << "CutB:: MET > 0.25 Meff(3jets):     " << count_meffana33nopu << " " << count_meffana33 << endl;  
    cout << "CutB:: Meff(3jets) > 1000 GeV:     " << count_jet3ana33nopu << " " << count_jet3ana33 << endl;   
    cout << endl;
  }
  if(ana==4){
    cout << "CutC:: MET > 0.25 Meff(4jets):     " << count_meffana34nopu << " " << count_meffana34 << endl;  
    cout << "CutC:: Meff(4jets) > 500 GeV:      " << count_jet3ana34nopu << " " << count_jet3ana34 << endl;   
    cout << endl;
  }
  if(ana==5){
    cout << "CutD:: Meff(4jets) > 1000 GeV:     " << count_jet3ana35nopu << " " << count_jet3ana35 << endl;   
    cout << endl;
  }
  if(ana==6){
    cout << "CutE:: MET/Meff(4j) >0.2:      " << count_newnopu << " " << count_new << endl;   
    cout << "CutE:: Meff(incl) > 1100 GeV:      " << count_jet3ana36nopu << " " << count_jet3ana36 << endl;   
    cout << endl;
  }
  histo_cutflow->Write();

  outputfile.Close();

  //check if all files have been read 
  if(static_cast<int>(fileList.size())!=filecount)
    {
      std::cerr<<"Not all files read!"<<std::endl;
      exit(1);
    }
  else
    cout << "All ok." << endl;
} // end of Reco0Lep::Loop


vector<int> Reco0Lep::get_good_electron_index(int kIsData,int myRunNumber){
  vector<int> elec_index;
  for(int ielec=0;ielec<el_n;ielec++){
    unsigned int goodElec=false;
    if ((el_OQ->at(ielec)&1446)==0 && m_myOQ.checkOQClusterElectron(myRunNumber,el_cl_eta->at(ielec),el_cl_phi->at(ielec))!=3)
      goodElec=true; 
    // cout << "this is a good electron" << endl;
    //     if((el_OQ->at(ielec)&1446)==0 ) goodElec=true; 
    if (susyobj.IsElectron(ielec,myRunNumber, 
			   el_cl_eta->at(ielec), el_cl_phi->at(ielec), el_cl_E->at(ielec), el_tracketa->at(ielec), el_trackphi->at(ielec),
			   el_author->at(ielec), el_isEM->at(ielec), el_OQ->at(ielec), el_nPixHits->at(ielec), el_nSCTHits->at(ielec),
			   kIsData,20000.,2.47)) {
      if(goodElec) elec_index.push_back(ielec);
    }
  }
  return elec_index;
}

vector<int> Reco0Lep::get_overlap_electron_index(vector<int> elec_index_met){
  vector<int> elec_index;
  int n_elec = elec_index_met.size();
  for(int ielec=0;ielec<n_elec;ielec++){
    if (susyobj.GetElecTLV(elec_index_met[ielec]).Pt() > 20000.) elec_index.push_back(elec_index_met[ielec]); 
  }
  return elec_index;
}


vector<int> Reco0Lep::get_iso_electron_index(int n_jet,vector<int> jet_index, vector<int> elec_index_noiso){
  vector<int> elec_index;
    int n_elec = elec_index_noiso.size();
   for(int ielec=0;ielec<n_elec;ielec++){
     if (El_JetinDR04(elec_index_noiso[ielec],n_jet,jet_index) == 0)  elec_index.push_back(elec_index_noiso[ielec]); 
   }
   return elec_index;
}


vector<int> Reco0Lep::get_good_muon_index(int n_jet,vector<int> jet_index, vector<int> muon_index2){
  vector<int> muon_index;
  int n_muon = muon_index2.size();
  for(int imuon=0;imuon<n_muon;imuon++){
    //if (mu_staco_ptcone20->at(muon_index2[imuon]) < 1800. && 
    if(Mu_JetinDR04(muon_index2[imuon],n_jet,jet_index) == 0) {
      muon_index.push_back(muon_index2[imuon]);
    }
  }
  return muon_index;
}

vector<int> Reco0Lep::get_good_muon_index2(int kIsData,float ptcut ){
  vector<int> muon_index2;
  for(int imuon=0;imuon<mu_staco_n;imuon++){
    TLorentzVector mymuon;
    mymuon.SetPxPyPzE(mu_staco_px->at(imuon),mu_staco_py->at(imuon),mu_staco_pz->at(imuon),mu_staco_E->at(imuon));
    if (susyobj.IsMuon(imuon,// EventNumber,
		       mymuon.Pt(),mymuon.Eta(),mymuon.Phi(), mu_staco_E->at(imuon),
		       mu_staco_me_qoverp_exPV->at(imuon), mu_staco_id_qoverp_exPV->at(imuon),
		       mu_staco_me_theta_exPV->at(imuon), mu_staco_id_theta_exPV->at(imuon),  mu_staco_id_theta->at(imuon),
		       mu_staco_isCombinedMuon->at(imuon), mu_staco_isLowPtReconstructedMuon->at(imuon),
		       mu_staco_loose->at(imuon),
		       mu_staco_expectBLayerHit->at(imuon), mu_staco_nBLHits->at(imuon), mu_staco_nPixHits->at(imuon),
		       mu_staco_nPixelDeadSensors->at(imuon), mu_staco_nPixHoles->at(imuon), mu_staco_nSCTHits->at(imuon),
		       mu_staco_nSCTDeadSensors->at(imuon), mu_staco_nSCTHoles->at(imuon), mu_staco_nTRTHits->at(imuon), mu_staco_nTRTOutliers->at(imuon),
		       kIsData,ptcut,2.4)) muon_index2.push_back(imuon);
  }
  return muon_index2;
}

vector<int> Reco0Lep::get_good_jet_index(int n_elec,vector<int> elec_index, int kIsData){
  vector<int> jet_index;
  for(int ijet=0;ijet<jet_AntiKt4TopoNewEM_n;ijet++){
    if (get_jet(ijet,n_elec,elec_index, kIsData)==1) jet_index.push_back(ijet);
  }
  return jet_index;
}
vector<int> Reco0Lep::get_good_jet_indexForCut(int n_elec,vector<int> elec_index, int kIsData){
  vector<int> jet_index;
  for(int ijet=0;ijet<jet_AntiKt4TopoNewEM_n;ijet++){
    if (get_jetForCut(ijet,n_elec,elec_index, kIsData)==1) jet_index.push_back(ijet);
  }
  return jet_index;
}


int Reco0Lep::Mu_JetinDR04(int imuon,int n_jet,vector<int> jet_index){ 

  double dr_jet;
  double diff_phi;
  vector<double> jetptemjes;
  for(int ijet=0;ijet<n_jet;ijet++){
    TLorentzVector mymuon;
    mymuon.SetPxPyPzE(mu_staco_px->at(imuon),mu_staco_py->at(imuon),mu_staco_pz->at(imuon),mu_staco_E->at(imuon));

    diff_phi=fabs((*jet_AntiKt4TopoNewEM_phi)[jet_index[ijet]]-(mymuon.Phi()));
    if(diff_phi>3.141592653){
      diff_phi=diff_phi-(2.*3.141592653);
    }
    dr_jet=sqrt(pow(((*jet_AntiKt4TopoNewEM_eta)[jet_index[ijet]]-(mymuon.Eta())),2)+pow(diff_phi,2)); 
    if(dr_jet<=0.4) return 1;	
  }
  return 0;
}


int Reco0Lep::El_JetinDR04(int ielec,int n_jet,vector<int> jet_index){ 
  
  double dr_jet;
  double diff_phi;
  vector<double> jetptemjes;
  
  for(int ijet=0;ijet<n_jet;ijet++){
    diff_phi=fabs((*jet_AntiKt4TopoNewEM_phi)[jet_index[ijet]]-((*el_phi)[ielec]));
    if(diff_phi>3.141592653){
      diff_phi=diff_phi-(2.*3.141592653);
    }
    dr_jet=sqrt(pow(((*jet_AntiKt4TopoNewEM_eta)[jet_index[ijet]]-((*el_eta)[ielec])),2)+pow(diff_phi,2));
//     if(dr_jet<=0.4 && dr_jet>0.2) return 1;
    if(dr_jet<=0.4) return 1;
  }
  return 0;
}


int Reco0Lep::get_jet(int ijet, int n_elec, vector<int> elec_index, int kIsData){

  if (susyobj.IsJet(ijet, jet_AntiKt4TopoNewEM_pt->at(ijet), jet_AntiKt4TopoNewEM_eta->at(ijet), jet_AntiKt4TopoNewEM_phi->at(ijet),
		    jet_AntiKt4TopoNewEM_E->at(ijet), jet_AntiKt4TopoNewEM_emscale_eta->at(ijet), 
		    jet_AntiKt4TopoNewEM_emfrac->at(ijet), jet_AntiKt4TopoNewEM_hecf->at(ijet),
		    jet_AntiKt4TopoNewEM_LArQuality->at(ijet), jet_AntiKt4TopoNewEM_HECQuality->at(ijet), jet_AntiKt4TopoNewEM_Timing->at(ijet),
		    jet_AntiKt4TopoNewEM_sumPtTrk->at(ijet), jet_AntiKt4TopoNewEM_fracSamplingMax->at(ijet),
		    jet_AntiKt4TopoNewEM_NegativeE->at(ijet), kIsData,20000.,2.8,JetID::LooseBad)){
    double dr_jet;
    double diff_phi;
    for(int iele=0;iele<n_elec;iele++){
      diff_phi=fabs((*jet_AntiKt4TopoNewEM_phi)[ijet]-((*el_phi)[elec_index[iele]]));
      if(diff_phi>3.141592653){
	diff_phi=diff_phi-(2.*3.141592653);
      }
      dr_jet=sqrt(pow(((*jet_AntiKt4TopoNewEM_eta)[ijet]-((*el_eta)[elec_index[iele]])),2)+pow(diff_phi,2));
      if(dr_jet<=0.2) return 0;
    }
	
    return 1;
  }
  return 0;
}

int Reco0Lep::get_jetForCut(int ijet, int n_elec, vector<int> elec_index, int kIsData){
  double dr_jet;
  double diff_phi;
  for(int iele=0;iele<n_elec;iele++){
    diff_phi=fabs((*jet_AntiKt4TopoNewEM_phi)[ijet]-((*el_phi)[elec_index[iele]]));
    if(diff_phi>3.141592653){
      diff_phi=diff_phi-(2.*3.141592653);
    }
    dr_jet=sqrt(pow(((*jet_AntiKt4TopoNewEM_eta)[ijet]-((*el_eta)[elec_index[iele]])),2)+pow(diff_phi,2));
    if(dr_jet<=0.2) return 0;
  }
  
  return 1;
}


double Reco0Lep::calculate_meff(vector<Container> jet,  double met, int nbjet){
  
  double meff=met;
  if((int)jet.size()<nbjet) nbjet=jet.size();
  for(int jet0=0;jet0<nbjet;jet0++) {
    meff+=jet[jet0].pt;
  }
  return meff;
}

double Reco0Lep::calculate_meffincl(vector<Container> jet,  double met, int nbjet){
  
  double meff=met;
  if((int)jet.size()<nbjet) nbjet=jet.size();
  for(int jet0=0;jet0<nbjet;jet0++) {
    if(jet[jet0].pt>40000.) meff+=jet[jet0].pt;
  }
  return meff;
}




double Reco0Lep::calculate_mt_1lepton(int n_elec, int n_muon, vector<int> elec_index, vector<int> muon_index, double metx, double mety, double met){
       
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




