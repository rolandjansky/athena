/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////
//Method Process MDT Offline Histos After Merging
//Author Justin Griffiths griffith@cern.ch
//       Orin Harris omh@cern.ch
//Created Apr 2009
//Aug 2009 Major Update To Communicate w/ Database
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataQualityUtils/MonitoringFile.h"

#include <iostream>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <stdio.h>
#include <map>
#include <iomanip>
#include <set>

#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TClass.h"
#include "TKey.h"
#include "TMath.h"
#include "TF1.h"
#include "TTree.h"
#include "TBranch.h"
#include "TList.h"

#include "DataQualityUtils/MdtDeadNoisyUtils.h"

namespace dqutils {

void MonitoringFile::MDTPostProcess( std::string inFilename, int txtFileOutputType ) 
{    
	//    std::cout << "Running MDT post processing \n" ;

	bool goodMDTFile = MonitoringFile::MDTCheck( inFilename );

	if(goodMDTFile){
		MonitoringFile::MDTChamReAlign( inFilename );
		MonitoringFile::MDTChamEff( inFilename, "MDT", txtFileOutputType );
		MonitoringFile::MDTChamOcc( inFilename );
		MonitoringFile::MDTTDCSum( inFilename );
		MonitoringFile::MDTLowStat( inFilename );
	}    
	else std::cout << "No MDT Directory!" << std::endl;

	//     std::cout << "\n";
	//     std::cout << "Finish MDT analysis"<<std::endl;
	return;
}

void MonitoringFile::MDTChamReAlign(std::string inFilename, std::string _title_){
	//    std::cout << "Begin ReAlign MDT Chamber by Chamber Plots" << std::endl;

	if(MDTCheckAlign(inFilename, _title_)) {/*std::cout << "File Already Aligned. Exiting..." << std::endl;*/ return;}

	TFile* f = TFile::Open(inFilename.c_str(),"UPDATE");

	if (f == 0) {
		std::cerr << "MDTPostProcess(): "
				<< "Input file not opened \n";
		return;
	}
	if(f->GetSize()<1000.) {
		std::cerr << "MDTPostProcess(): "
				<< "Input file empty \n";
		return; 
	}

	TIter nextcd0(f->GetListOfKeys());
	while(TKey *key0 = dynamic_cast<TKey*> (nextcd0())){
		// Following can never test true, since while would fail.
		// In fact loop breaks at this point, may not be desired behavior.
		// Coverity 16305
		//if (key0 == 0) continue;
		TDirectory *dir0= dynamic_cast<TDirectory*> (key0->ReadObj());
		if (dir0 == 0) return;

		TString runNumber = dir0->GetName();
		TString mDir = runNumber+"/Muon/MuonRawDataMonitoring/" + _title_;
		TDirectory* mdt_base_dir = f->GetDirectory(mDir);
		if (! mdt_base_dir) continue;
		TIter nextcd1(mdt_base_dir->GetListOfKeys());
		//  std::cout << mDir << std::endl;                                                                                                                          
		while(TKey* key1 = dynamic_cast<TKey*>(nextcd1())) {//While in BA/EA/BC/EC                                                                   
			TString ecap_fullStr = mDir+"/"+key1->GetName();
			TDirectory* dir1 = f->GetDirectory(ecap_fullStr);
			if(!dir1) continue;
			dir1->cd();
			//Determine which directory to put new folders into
			if(strncmp(key1->GetName(),"Sum",2)==0)continue;
			if(strncmp(key1->GetName(),"Ove",2)==0)continue;
			if(gDirectory->GetDirectory("Chambers")) gDirectory->cd("Chambers");
			else if(gDirectory->GetDirectory("Chamber")) gDirectory->cd("Chamber");

			dir1 = gDirectory;
			TIter nextcd2(gDirectory->GetListOfKeys());

			TString metadataName;
			//The following if is a relic the else is what is likely to occur
			if(!(strncmp(key1->GetName(),dir1->GetName(),2)==0)) metadataName = mDir+"/"+key1->GetName()+"/"+gDirectory->GetName()+"/metadata";
			else metadataName = mDir +"/"+key1->GetName()+"/metadata";
			TTree* metadata = 0; (f->GetObject(metadataName,metadata));
			char nameData[99];
			char intervalData[99];
			char chainData[99];
			char mergeData[99];
			TBranch* b_nameData;
			TBranch* b_intervalData;
			TBranch* b_chainData;
			TBranch* b_mergeData;

			if(metadata){
				metadata->SetBranchAddress("Interval",intervalData,&b_intervalData);
				metadata->SetBranchAddress("Name",nameData,&b_nameData);
				metadata->SetBranchAddress("TriggerChain",chainData,&b_chainData);
				metadata->SetBranchAddress("MergeMethod",mergeData,&b_mergeData);
			}

			int treeCounter = 0;
			TTree* metadata_new = 0;
			OutputMetadata* outputmd = 0;
			TKey* key2 = 0;
			TDirectory* chambDir = 0;
			TDirectory* lastChamb = 0;

			while( (key2 = dynamic_cast<TKey*> (nextcd2()) ) != 0){
				if(strncmp(key2->GetName(),"metadata",5)==0){
					continue;
				}
				if(metadata) {metadata->GetEntry(treeCounter); ++treeCounter;}
				TString name = key2->GetName();
				if(TClass::GetClass(key2->GetClassName())->InheritsFrom("TDirectory")) { continue; }
				TString chamberName = name(0,7);

				if(!(chambDir = dir1->GetDirectory(chamberName))) {
					//First Save the new TTree to the last directory
					if(metadata_new){ 
						if(lastChamb) lastChamb->cd();
						metadata_new->Write();
						delete metadata_new;
					}
					chambDir = dir1->mkdir(chamberName);
					if(!chambDir) chambDir = dir1->GetDirectory(chamberName);
					//Create New MetaData to be stored later;
					metadata_new = new TTree( "metadata", "Monitoring Metadata" );
					metadata_new->SetDirectory(0);
					delete outputmd;
					outputmd = new OutputMetadata( metadata_new );
				}
				lastChamb = chambDir;
				chambDir->cd();
				TH1* obj = dynamic_cast<TH1*> (dir1->Get(key2->GetName()));
				TH1* obj2 = dynamic_cast<TH1*> (chambDir->Get(key2->GetName()));
				if(obj2 && obj) { obj->Add(obj2); }
				if(obj) { obj->SetFillColor(42); obj->Write("",TObject::kOverwrite); }
				dir1->cd();
				delete obj;
				if(obj2) delete obj2;
				key2->Delete();
				if(outputmd) outputmd->fill(nameData,intervalData,chainData,mergeData);
			}
			//Fill the metadata for the last directories
			if(metadata_new && chambDir) {chambDir->cd(); metadata_new->Write(); }

			TIter nextcd3(dir1->GetListOfKeys());
			TKey* key3 = 0;
			//Delete Straglers (1 Folder for each of BA/BC/EA/EC)
			while( (key3 = dynamic_cast<TKey*> (nextcd3()) ) != 0){
				TString histName = key3->GetName();
				if(!TClass::GetClass(key3->GetClassName())->InheritsFrom("TDirectory")) { key3->Delete();}
			}
			delete outputmd;
		}
	}

	f->Close();
	//    std::cout << "End ReAlign MDT Chamber by Chamber Plots" << std::endl;
	delete f;
	return;
}

void MonitoringFile::MDTChamEff( std::string inFilename, std::string _title_, int txtFileOutputType) {
	//const float m_noiseCut = 100000;
	//if(isIncremental || useOfflineTubeID || makePDF || doAllPDF){ std::cout << "dead code " << std::endl;}	
	
	MDTPostProcessor mf( inFilename, "ChamberEff Calculations");

	if ( !mf.IsOpen() ) {
		std::cerr << "MDTPostProcess(): "
				<< "Input file not opened \n";
		return;
	}
	if(mf.GetSize()<1000.) {
		std::cerr << "MDTPostProcess(): "
				<< "Input file empty \n";
		return; 
	}

	int numAllTubes = 0;
	int numDeadTubes = 0;
	int numDeadTubes_All = 0;
	int numNoisyTubes = 0;
	int numAllMezz = 0;
	int numDeadMezz = 0;
	int numAllLayers = 0;
	int numDeadLayers = 0;
	int numAllML = 0;
	int numDeadML = 0;
	int numDeadChambers = 0;
	int totBA = 331; int totBC = 329;
    int totEA = 257; int totEC = 257; //assuming 7 EEA chambers, 31 EEC chambers
	int numAllChambers = totBA + totBC + totEA + totEC;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	TIter next_run ( mf.GetListOfKeys() );
	TKey* key_run(0);
	while ((key_run = dynamic_cast<TKey*> ( next_run() )) !=0 ) {
		TString run_dir = key_run->GetName();
		////Needed for Database
		int runNumber_int = TString(run_dir(4,run_dir.Length()-4)).Atoi();
		if(runNumber_int<0) std::cerr << "Bad Run Number " << runNumber_int << std::endl;
		int ChannelIdCounter = 0;
		////

		if (!run_dir.Contains("run") )   continue;
		//Compute Tube Effs
		TString chamber_Area = run_dir + "/Muon/MuonRawDataMonitoring/"+_title_+"/";

		if(mf.GetDirectory(chamber_Area)) mf.cd(chamber_Area);
		//else {std::cerr << "No MDT Directory! " << std::endl; f->Close() ; delete f; return; }
		else {std::cout << "No MDT Directory! " << std::endl; mf.error(); return; }
		TIter next_ecap(gDirectory->GetListOfKeys());
		TDirectory* dir_Overview = mf.GetDirectory(chamber_Area+"Overview/");

		//Get Global Chamber Effs
		if(!dir_Overview) continue;
		bool EffG = true;
		TH2F* EffBCap = 0; 
		TH2F* EffECap = 0; 
		mf.get("EffsInBarrelPerChamber_ADCCut", EffBCap, dir_Overview);
		mf.get("EffsInEndCapPerChamber_ADCCut", EffECap, dir_Overview);
		if( !EffBCap ) mf.get("effsInBarrelPerChamber_ADCCut", EffBCap, dir_Overview); 
		if( !EffECap ) mf.get("effsInEndCapPerChamber_ADCCut", EffECap, dir_Overview); 

		//We get the noise and bkgrd hist from the MDT folder, thus nTriggers must come from there as well
		TH1F* hNTriggers = 0; mf.get(run_dir + "/Muon/MuonRawDataMonitoring/MDT/Overview/TotalNumber_of_MDT_hits_per_event_ADCCut",hNTriggers);
		int nTriggers = 1;
		if(hNTriggers) {
			nTriggers = (int) hNTriggers->GetEntries();
		}

		std::map<TString, int> nTubes_map;

		//Get number_of_Chambers_with_hits_per_event, number_of_Chambers_with_hits_per_event_ADCCut, TotalNumber_of_MDT_hits_per_event_big, TotalNumber_of_MDT_hits_per_event_big_ADCCut
		//...for the purpose of changing their directory/metadata
		TH1F* hNumChambHitsPerEvt = 0; 
		TH1F* hNumChambHitsPerEvtADCCut = 0;
		TH1F* hTotNumMDTHitsPerEvtBig = 0;  
		TH1F* hTotNumMDTHitsPerEvtBigADCCut = 0;  
		mf.get("number_of_Chambers_with_hits_per_event", hNumChambHitsPerEvt, dir_Overview);
		mf.get("number_of_Chambers_with_hits_per_event_ADCCut", hNumChambHitsPerEvtADCCut, dir_Overview);
		mf.get("TotalNumber_of_MDT_hits_per_event_big", hTotNumMDTHitsPerEvtBig, dir_Overview);
		mf.get("TotalNumber_of_MDT_hits_per_event_big_ADCCut", hTotNumMDTHitsPerEvtBigADCCut, dir_Overview);

		////////////////////////////////////////////////////////////////////////////////////////////////////
		///////Create Noisy/Dead element histos and noise rate//////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////
		TH1F* hDeadElements = new TH1F("MDTDeadElements","MDTDeadElements",1,0,1);
		//TH1F* hNoisyElements = new TH1F("MDTNoisyElements","MDTNoisyElements",1,0,1);
		//TH1F* hNoiseRate = new TH1F("TubeRate","TubeRate; Rate [kHz]; counts",1000,0,100);
		TDirectory* dir_Overview_expert = dir_Overview->GetDirectory("Expert");
		if(!dir_Overview_expert) dir_Overview_expert = dir_Overview->mkdir("Expert");
		mf.setMetaData(dir_Overview_expert,hDeadElements);
		//mf.setMetaData(dir_Overview_expert,hNoisyElements);
		// Create new directory for all the noise/bkd rate info
		//TDirectory* dir_Overview_expert_noisebkd = dir_Overview_expert->GetDirectory("Noise_Bkgrd_Rate");
		//if(!dir_Overview_expert_noisebkd) dir_Overview_expert_noisebkd = dir_Overview_expert->mkdir("Noise_Bkgrd_Rate");
		//mf.setMetaData(dir_Overview_expert_noisebkd,hNoiseRate);
		////////////////////////////////////////////////////////////////////////////////////////////////////
		///////Finish Creating histos Noisy/Dead elements///////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////      

		TH2F* EffECap_N = 0; 
		TH2F* EffBCap_N = 0; 

		// Create chamber volume map, for weighting occupancy plots
		TH2F * VolumeMapBCap = 0;
		TH2F * VolumeMapECap = 0;
		//TH2F * VolumeMapBCap_noisecut = 0;
		//TH2F * VolumeMapECap_noisecut = 0;

		// new occ plots, hits/evt/chamber-volume
		TH2F* EvtTubeNormOccBCap = 0;
		TH2F* EvtTubeNormOccECap = 0;
		//TH2F* EvtTubeNormOccNoiseCutBCap = 0;
		//TH2F* EvtTubeNormOccNoiseCutECap = 0;
		// raw hits/evt occ plots
		TDirectory* dir_Overview_expert_RawHits = dir_Overview_expert->GetDirectory("RawHitsPerChamber");
		if(!dir_Overview_expert_RawHits) dir_Overview_expert_RawHits = dir_Overview_expert->mkdir("RawHitsPerChamber");
		TH2F* EvtOccBCap = 0;
		TH2F* EvtOccECap = 0;
		//TH2F* EvtOccNoiseCutBCap = 0;
		//TH2F* EvtOccNoiseCutECap = 0;

		// noise vs eff hists
		/*TH1F* EffVSNoise_num = new TH1F("EffVSNoise_num","Tube eff_num vs tube noise rate",50,0,1000);
      mf.setDirectory(EffVSNoise_num);
      TH1F* EffVSNoise_den = new TH1F("EffVSNoise_den","Tube eff_den vs tube noise rate",50,0,1000);
      mf.setDirectory(EffVSNoise_den);
      TH1F* EffVSNoise_ADCCut_num = new TH1F("EffVSNoise_ADCCut_num","Tube eff_num vs tube (ADC>80) noise rate",50,0,1);
      mf.setDirectory(EffVSNoise_ADCCut_num);
      TH1F* EffVSNoise_ADCCut_den = new TH1F("EffVSNoise_ADCCut_den","Tube eff_den vs tube (ADC>80) noise rate",50,0,1);
      mf.setDirectory(EffVSNoise_ADCCut_den);
      TH1F* EffVSBkd_num = new TH1F("EffVSBkd_num","Tube eff_num vs tube (ADC>80) bkd rate",50,0,1);
      mf.setDirectory(EffVSBkd_num);
      TH1F* EffVSBkd_den = new TH1F("EffVSBkd_den","Tube eff_den vs tube (ADC>80) bkd rate",50,0,1);
      mf.setDirectory(EffVSBkd_den);

      //Plots to Fill Noise/Bkgrd Occupancy
      TH2F* NoiseBCap = 0;
      TH2F* NoiseECap = 0;
      TH2F* BkgrdBCap = 0;
      TH2F* BkgrdECap = 0;*/

		if(!EffBCap || !EffECap) { std::cerr << "MDT: No Global Eff Plots" << std::endl; EffG=false; }
		if(EffG) {
			MDTResetContents(EffBCap);
			MDTResetContents(EffECap);	
			// Create chamber volume map, for weighting occupancy plots
			VolumeMapBCap = (TH2F*) EffBCap->Clone();
			VolumeMapECap = (TH2F*) EffECap->Clone();
			VolumeMapBCap->SetName("VolumeMapBarrel");
			VolumeMapBCap->SetTitle("Volume Map (#tubes*tubeVol in m^3) Barrel");
			VolumeMapECap->SetName("VolumeMapEndcap");
			VolumeMapECap->SetTitle("Volume Map (#tubes*tubeVol in m^3) Endcap");
			mf.setMetaData(dir_Overview_expert, VolumeMapBCap, VolumeMapECap);
			/*VolumeMapBCap_noisecut = (TH2F*) EffBCap->Clone();
	VolumeMapECap_noisecut = (TH2F*) EffECap->Clone();
	VolumeMapBCap_noisecut->SetName("VolumeMapBarrel_noisecut");
	VolumeMapBCap_noisecut->SetTitle("Volume Map (m^3) Barrel, 100KHz noise cut");
	VolumeMapECap_noisecut->SetName("VolumeMapEndcap_noisecut");
	VolumeMapECap_noisecut->SetTitle("Volume Map (m^3) Endcap, 100KHz noise cut");
	mf.setMetaData(dir_Overview_expert, VolumeMapBCap_noisecut, VolumeMapECap_noisecut);*/
			EffBCap_N = (TH2F*) EffBCap->Clone();
			EffECap_N = (TH2F*) EffECap->Clone();
			mf.setDirectory(EffBCap_N);
			mf.setDirectory(EffECap_N);
			/*NoiseBCap = (TH2F*) EffBCap->Clone();
	NoiseECap = (TH2F*) EffECap->Clone();
	NoiseBCap->SetName("AverageNoisePerTubeBarrel");
	NoiseBCap->SetTitle("Avg # noise-hits/evt/m^3 Barrel");
	NoiseECap->SetName("AverageNoisePerTubeEndcap");
	NoiseECap->SetTitle("Avg # noise-hits/evt/m^3 Endcap");
	mf.setMetaData(dir_Overview_expert_noisebkd, NoiseBCap, NoiseECap);
	BkgrdBCap = (TH2F*) EffBCap->Clone();
	BkgrdECap = (TH2F*) EffECap->Clone();
	BkgrdBCap->SetName("AverageBkgrdPerTubeBarrel");
	BkgrdBCap->SetTitle("Avg # bkgrd-hits/evt/m^3 Barrel");
	BkgrdECap->SetName("AverageBkgrdPerTubeEndcap");
	BkgrdECap->SetTitle("Avg # bkgrd-hits/evt/m^3 Endcap");
	mf.setMetaData(dir_Overview_expert_noisebkd, BkgrdECap, BkgrdBCap);*/
			// Raw hits per event
			EvtOccBCap = (TH2F*) EffBCap->Clone();
			EvtOccECap = (TH2F*) EffECap->Clone();
			EvtOccBCap->SetName("HitsPerEvtInBarrelPerChamber_ADCCut");
			EvtOccBCap->SetTitle("Avg # hits/evt Barrel, ADCCut");
			EvtOccECap->SetName("HitsPerEvtInEndCapPerChamber_ADCCut");
			EvtOccECap->SetTitle("Avg # hits/evt Endcap, ADCCut");
			mf.setMetaData(dir_Overview_expert_RawHits, EvtOccBCap, EvtOccECap);
			/*EvtOccNoiseCutBCap = (TH2F*) EffBCap->Clone();
	EvtOccNoiseCutECap = (TH2F*) EffECap->Clone();
	EvtOccNoiseCutBCap->SetName("HitsPerEvtInBarrelPerChamber_ADCCut_NoiseCut");
	EvtOccNoiseCutBCap->SetTitle("Avg # hits/evt Barrel, ADCCut, 100KHz noise cut");
	EvtOccNoiseCutECap->SetName("HitsPerEvtInEndCapPerChamber_ADCCut_NoiseCut");
	EvtOccNoiseCutECap->SetTitle("Avg # hits/evt Endcap, ADCCut, 100KHz noise cut");
	mf.setMetaData(dir_Overview_expert_RawHits, EvtOccNoiseCutBCap, EvtOccNoiseCutECap);*/
		}
		while( TKey* key_ecap = dynamic_cast<TKey*> (next_ecap()) ) {//BA,BC,EA,EC
			if(strncmp(key_ecap->GetName(),"Overvi",6)==0) continue;	    
			if(strncmp(key_ecap->GetName(),"Summar",6)==0) continue;	    
			TString ecap_str = key_ecap->GetName();
			TString ecap_fullStr = ecap_str;
			ecap_str = ecap_str(3,2);
			TString ecapPath_str = chamber_Area+ecap_fullStr+"/Chambers";

			//	TDirectory* dir1 = f->GetDirectory(ecapPath_str);
			TDirectory* dir1 = mf.GetDirectory(ecapPath_str);
			if(!dir1) continue;
			TIter next_cham(dir1->GetListOfKeys());
			TString overview_str = chamber_Area+ecap_fullStr+"/Overview/";
			//	TDirectory* dir_Overview_local = f->GetDirectory(overview_str);
			TDirectory* dir_Overview_local = mf.GetDirectory(overview_str);
			//Get Local Chamber Effs
			bool EffL = true;
			bool EffInner_old = true; // protect agains pre-BIR/BIM separation
			TH2F* EffExtra = 0;
			TH2F* EffInner = 0;
			TH2F* EffMiddle = 0;
			TH2F* EffOuter = 0;
			TH2F* EffExtra_N = 0;
			TH2F* EffInner_N = 0;
			TH2F* EffMiddle_N = 0;
			TH2F* EffOuter_N = 0;

			TString ecap_fullStr_lower = "mdt"+ecap_str;
			TH1F* heff = new TH1F(ecap_fullStr_lower+"_TUBE_eff",ecap_fullStr_lower+"_TUBE_eff",100,0,1);
			TH1F* heffML = new TH1F(ecap_fullStr_lower+"_ML_eff",ecap_fullStr_lower+"_ML_eff",50,0,1);
			//Histogram To Trivially measure coverage per tube

			if(dir_Overview_local){
				mf.get("EffsIn"+ecap_str+"InnerPerMultiLayer_ADCCut",EffInner,dir_Overview_local); 
				mf.get("EffsIn"+ecap_str+"MiddlePerMultiLayer_ADCCut",EffMiddle,dir_Overview_local); 
				mf.get("EffsIn"+ecap_str+"OuterPerMultiLayer_ADCCut",EffOuter,dir_Overview_local); 
				mf.get("EffsIn"+ecap_str+"ExtraPerMultiLayer_ADCCut",EffExtra,dir_Overview_local); 
				if(!EffInner)  mf.get("effsIn"+ecap_str+"InnerPerMultiLayer_ADCCut",EffInner,dir_Overview_local); 
				if(!EffMiddle) mf.get("effsIn"+ecap_str+"MiddlePerMultiLayer_ADCCut",EffMiddle,dir_Overview_local); 
				if(!EffOuter)  mf.get("effsIn"+ecap_str+"OuterPerMultiLayer_ADCCut",EffOuter,dir_Overview_local); 
				if(!EffExtra)  mf.get("effsIn"+ecap_str+"ExtraPerMultiLayer_ADCCut",EffExtra,dir_Overview_local); 
			}
			if(!EffExtra || !EffInner || ! EffMiddle || !EffOuter) { std::cerr << "No Local Chamber Eff Plots" << std::endl; EffL = false;}

			if(EffL) {
				MDTResetContents((TH2F*)EffExtra); MDTResetContents((TH2F*)EffOuter); MDTResetContents((TH2F*)EffMiddle); MDTResetContents((TH2F*)EffInner);
				EffExtra_N = (TH2F*) EffExtra->Clone();
				EffInner_N = (TH2F*) EffInner->Clone();
				EffMiddle_N = (TH2F*) EffMiddle->Clone();
				EffOuter_N = (TH2F*) EffOuter->Clone();
				mf.setDirectory(EffExtra_N);
				mf.setDirectory(EffInner_N);
				mf.setDirectory(EffMiddle_N);
				mf.setDirectory(EffOuter_N);
				if(EffInner->GetNbinsY() == 36) EffInner_old = false;
			}

			std::vector<TString> chamberNames_vec;
			std::vector<float> chamberHits_vec;//get median value of hits per chamber in order to count HV off chambers
			while( TKey* key_cham = (TKey*) (next_cham()) ){//Chamber Folders
				if(strncmp(key_cham->GetName(),"meta",4)==0) continue;
				TString chamberStr = chamber_Area+ecap_fullStr+"/Chambers/"+key_cham->GetName();
				TDirectory* dirChamb = mf.GetDirectory(chamberStr);
				if(!dirChamb) continue;
				if(!(dirChamb)->InheritsFrom("TDirectory")) continue;
				TString dirName = dirChamb->GetName();
				chamberNames_vec.push_back(dirName);
				TH1F* tube = 0; mf.get(dirName+"_MDT_Station_TUBE_ADCCut",tube,dirChamb);
				if(tube) {
					chamberHits_vec.push_back((float) tube->GetEntries());	  
				}
			}	
			std::sort( chamberHits_vec.begin(), chamberHits_vec.end() );
			double medianChamberHits = 0; 
			unsigned nChambers_ = chamberHits_vec.size();
			if(nChambers_%2==0 && nChambers_ > 2) medianChamberHits = (chamberHits_vec.at(nChambers_/2) + chamberHits_vec.at(nChambers_/2 + 1))/nChambers_;
			else if(nChambers_>0) medianChamberHits = chamberHits_vec.at(nChambers_/2);
			int chamberCounter = 0;
			for(std::vector<TString>::const_iterator chamberStrItr = chamberNames_vec.begin(); chamberStrItr != chamberNames_vec.end(); ++chamberStrItr, ++chamberCounter){
				TString dirName = *chamberStrItr;
				TDirectory* dirChamb = mf.GetDirectory(chamber_Area+ecap_fullStr+"/Chambers/"+dirName);
				if(!dirChamb) continue;
				if(dirName=="BML6A13"||dirName=="BML6C13") continue;//These chambers do not exist


				////////////////////////////////////////////////////////////////////////////////////////////////////
				//////Dead Noisy Tube Calculations
				////////////////////////////////////////////////////////////////////////////////////////////////////
				////Arbitrary counter for database file
				bool hvOff = false;
				bool noDeadLayer = true; //flag to determine if chamber has dead ml,l, or mezz
				ChannelIdCounter++;
				TH1F* den = 0; mf.get(dirName+"_MDT_Station_EFFENTRIES",den,dirChamb);
				TH1F* num = 0; mf.get(dirName+"_MDT_Station_EFFCOUNTS",num,dirChamb);
				TH1F* tube = 0; mf.get(dirName+"_MDT_Station_TUBE_ADCCut",tube,dirChamb);
				unsigned numTubesInChamber = tube->GetNbinsX();

				////////////////////////////////////////////////////////////////////////////////////////////////////
				//////FillBackground Plots Calculations
				////////////////////////////////////////////////////////////////////////////////////////////////////

				if(tube){
					if( (dirName.BeginsWith("BIS8") || dirName.BeginsWith("BE"))) {
						if(tube->GetEntries() < 0.01 * medianChamberHits + 0.1) hvOff = true;
					} else if ( tube->GetEntries() < 0.07 * medianChamberHits + 0.1  ) hvOff = true;
					
					//TH1F* noise_clone = (TH1F*) noise->Clone();
					//noise_clone->Scale(1./nTriggers/300.*1e9);
					//TH1F* bkgrd_clone = (TH1F*) bkgrd->Clone();
					//bkgrd_clone->Scale(1./nTriggers/300.*1e9);
					//int nTubes_noiseCut = 0;
					//int nHits_noiseCut = 0;
					int nHits = (int)tube->GetEntries();

					/*for( int ibin = 1; ibin != bkgrd_clone->GetNbinsX()+1; ++ibin){
                       if(noise_clone->At(ibin) <= m_noiseCut) {
                        nTubes_noiseCut++;
                        nHits_noiseCut += (int)tube->At(ibin);
                       }
                     }*/

					// For filling Bkgrd/Noise Occupancy plots
					TString xAxis = dirName(0,1)+dirName(4,1)+dirName(3,1);
					TString yAxis = dirName(1,1)+","+dirName(5,2);
					//Seperate BIR/MXA11/15
					if( EffG && (dirName(0,3) == "BIR" || dirName(0,3) == "BIM") && (dirName(5,2) == "11" || dirName(5,2) == "15" ) && VolumeMapBCap->GetNbinsY() >= 58){
						yAxis += ",";
						yAxis += dirName(2,1);
					}
					//BML[45][AC]13-->BML[56][AC]13
					if( EffG && dirName(0,3) == "BML" && TString(dirName(3,1)).Atoi() >= 4 && dirName(5,2) == "13" && VolumeMapBCap->GetNbinsY() >= 58 ){
						xAxis = dirName(0,1) + dirName(4,1) + returnString( TString(dirName(3,1)).Atoi() + 1 );
					}
					//BME1[AC]14 (mistake - is actually 13 - so account for both cases!) -->BML4[AC]13
					if( EffG && dirName(0,3) == "BME" && VolumeMapBCap->GetNbinsY() >= 58 ){
						xAxis = dirName(0,1) + dirName(4,1) + "4";
						yAxis = "M,13";
					}
                                        if(EffG && dirName(0,3) == "BMF"){
                                                xAxis = dirName(0,1) + dirName(4,1) + returnString(TString(dirName(3,1)).Atoi()*2-1);
                                        
                                        } 
					double tubeLength = 4.9615;
					double tubeRadiusScale = 1;
					// 	    double maxTubeLengthBarrel = 4961.5;  // just FYI
					// 	    double maxTubeLengthEndcap = 5941.5;  // just FYI 
					GetTubeLength(tubeLength,dirName);    // should always fill tubeLength
					if(dirName(0,3) == "BME" || dirName(0,3) == "BMG") tubeRadiusScale = 0.25;
					double chamb_vol = (double)numTubesInChamber*tubeLength*0.0006881*tubeRadiusScale;   // 0.0006881 m2 = pi*tube_r^2
					// BME tubes are half the radius = 1/4 the volume/
					//double noiseCut_vol = (double)nTubes_noiseCut*tubeLength*0.0006881*tubeRadiusScale; // these represent the total volume in (m3) covered by the tubes in the chamber

					// Fill volume maps regardless of whether the chamber is called dead
					if (EffG) {
						if(xAxis.BeginsWith("B")) {
							VolumeMapBCap->Fill(xAxis,yAxis,chamb_vol);
							//VolumeMapBCap_noisecut->Fill(xAxis,yAxis,noiseCut_vol);
						}
						else {
							VolumeMapECap->Fill(xAxis,yAxis,chamb_vol);
							//VolumeMapECap_noisecut->Fill(xAxis,yAxis,noiseCut_vol);
						}
					}

					// Only do the rest if chamber is not called dead
					if( !hvOff ) {
						TString eta_ring = dirName(0,2) + dirName(4,1);
						TString eta_stat = dirName(3,1);
                                                if(dirName(0,3)=="BMF") eta_stat = returnString(TString(dirName(3,1)).Atoi()*2-1);
						if(eta_stat=="5"||eta_stat=="6"||eta_stat=="7"||eta_stat=="8") eta_ring+="5,6";
						if(eta_stat=="3"||eta_stat=="4") eta_ring+="3,4";
						if(eta_stat=="1"||eta_stat=="2"||eta_stat=="0") eta_ring+="1,2";
						//float noise_rate = 0;
						//float noise_hits = 0;
						//float bkgrd_rate = 0;
						//float bkgrd_hits = 0;

						/*for( int ibin = 1; ibin != bkgrd_clone->GetNbinsX()+1; ++ibin){
                            hNoiseRate->Fill(noise_clone->At(ibin)/1000.);
                            if(noise_clone->At(ibin) <= m_noiseCut) {
                                noise_rate += noise_clone->At(ibin);
                                bkgrd_rate += bkgrd_clone->At(ibin);
                                noise_hits += noise->At(ibin);
                                bkgrd_hits += bkgrd->At(ibin);
                            }
                        }*/

						if (EffG) {
							if(xAxis.BeginsWith("B")) {
								//NoiseBCap->Fill(xAxis, yAxis, noise_hits / ((double)nTriggers) );
								//BkgrdBCap->Fill(xAxis, yAxis, bkgrd_hits / ((double)nTriggers) );     
								EvtOccBCap->Fill(        xAxis, yAxis, nHits          / ((double)nTriggers) );
								//EvtOccNoiseCutBCap->Fill(xAxis, yAxis, nHits_noiseCut / ((double)nTriggers) );
							}
							else {
								//NoiseECap->Fill(xAxis, yAxis, noise_hits / ((double)nTriggers) );
								//BkgrdECap->Fill(xAxis, yAxis, bkgrd_hits / ((double)nTriggers) );     
								EvtOccECap->Fill(        xAxis, yAxis, nHits          / ((double)nTriggers) );
								//EvtOccNoiseCutECap->Fill(xAxis, yAxis, nHits_noiseCut / ((double)nTriggers) );
							}
						}

						//std::map<TString,float>::iterator noise_itr = noise_map.find(eta_ring);
						//std::map<TString,float>::iterator bkgrd_itr = bkgrd_map.find(eta_ring);
						std::map<TString,int>::iterator nTubes_itr = nTubes_map.find(eta_ring);
						/*if( noise_itr == noise_map.end() ) noise_map.insert(std::make_pair(eta_ring,noise_rate));
                         else (*noise_itr).second += noise_rate;
                         if( bkgrd_itr == bkgrd_map.end() ) bkgrd_map.insert(std::make_pair(eta_ring,bkgrd_rate));
                         else (*bkgrd_itr).second += bkgrd_rate;
                         if( nTubes_itr == nTubes_map.end() ) nTubes_map.insert(std::make_pair(eta_ring,nTubes_noiseCut));
                         else (*nTubes_itr).second += nTubes_noiseCut;
						 */
						////////////////////////////////////////////////////////////////////////////////////////////////////
						//////End FillBackground Plots Calculations
						//////Next do Efficiencies
						////////////////////////////////////////////////////////////////////////////////////////////////////

						TH1F* eff = new TH1F(dirName+"_MDT_Station_EFFPERTUBE",dirName+"_MDT_Station_EFFPERTUBE; tubeID; Number of Entries",numTubesInChamber,1,numTubesInChamber+1);
						mf.setMetaData(dirChamb,eff);
						if(!eff || !num || !den) continue;

						float countsML1, countsML2, entriesML1, entriesML2;
						MDTTubeEff(dirName,num,den,eff,countsML1,countsML2,entriesML1,entriesML2);
						//Fill heff if no dead layer,ML, or Mezz
						if(noDeadLayer) {
							for(int ibin = 1; ibin != eff->GetNbinsX(); ++ibin) {
								if( den->At(ibin) > 10 ) heff->Fill( (num->At(ibin))/(den->At(ibin)) ); 
							}
						}

						///Set the char labels for the efficiency plots
						TString stateta_IMO_c; TString statphi_IMO_c; TString stateta_c; TString statphi_c; TString statphi_c2;
						MDT2DHWName(dirName,  stateta_IMO_c,  statphi_IMO_c,  stateta_c,  statphi_c,  statphi_c2);
						//correct for BI[R/M][A/C][11/15]
						//Move BML[4,5][A/C]13-->BML[5,6][A/C]13
						stateta_IMO_c = xAxis;
						statphi_IMO_c = yAxis;

						if(EffG){
							if( entriesML1 + entriesML2 > 10 ) {
								if(ecap_str(0,1)=="B") {
									EffBCap->Fill(stateta_IMO_c,statphi_IMO_c,countsML1+countsML2);
									EffBCap_N->Fill(stateta_IMO_c,statphi_IMO_c,entriesML1+entriesML2);
								}
								else {
									EffECap->Fill(stateta_IMO_c,statphi_IMO_c,countsML1+countsML2);
									EffECap_N->Fill(stateta_IMO_c,statphi_IMO_c,entriesML1+entriesML2);
								}
							}
						}
						if(EffL){
							if(dirName(1,1)=="E") {
								if( entriesML1 > 10 ) {
									EffExtra->Fill(stateta_c, statphi_c, countsML1);
									EffExtra_N->Fill(stateta_c, statphi_c, entriesML1);	  
								}
								if( entriesML2 > 10 ) {
									if(dirName(1,0)=="E"){
										EffExtra->Fill(stateta_c, statphi_c2, countsML2);
										EffExtra_N->Fill(stateta_c,statphi_c2, entriesML2);
									}
								}
							}
							if(dirName(1,1)=="I" && (!EffInner_old || dirName(0,1)=="E")) {
								if( entriesML1 > 10 ) {
									EffInner->Fill(stateta_c, statphi_c, countsML1);
									EffInner_N->Fill(stateta_c, statphi_c, entriesML1);	  
								}
								if( entriesML2 > 10 ) {
									EffInner->Fill(stateta_c, statphi_c2, countsML2);
									EffInner_N->Fill(stateta_c,statphi_c2, entriesML2);
								}
							}
							if(dirName(1,1)=="M") {
								if( entriesML1 > 10 ) {
									EffMiddle->Fill(stateta_c, statphi_c, countsML1);
									EffMiddle_N->Fill(stateta_c, statphi_c, entriesML1);	  
								}
								if( entriesML2 > 10 ) {
									EffMiddle->Fill(stateta_c, statphi_c2, countsML2);
									EffMiddle_N->Fill(stateta_c,statphi_c2, entriesML2);
								}
							}
							if(dirName(1,1)=="O") {
								if( entriesML1 > 10 ) {
									EffOuter->Fill(stateta_c, statphi_c, countsML1);
									EffOuter_N->Fill(stateta_c, statphi_c, entriesML1);	  
								}
								if( entriesML2 > 10 ) {
									EffOuter->Fill(stateta_c, statphi_c2, countsML2);
									EffOuter_N->Fill(stateta_c,statphi_c2, entriesML2);
								}
							}
						}
						dirChamb->cd();
						eff->Write("",TObject::kOverwrite); 
						//Write to the file periodically
						//Wish we didn't have to do this
						//	      if(ChannelIdCounter%20==0) mf.Write();
						if(noDeadLayer) { if( entriesML2 > 10 ) heffML->Fill(countsML2/entriesML2); if( entriesML1 > 10 ) heffML->Fill(countsML1/entriesML1); }
					}
					//delete bkgrd_clone;
					//delete noise_clone;
				}
			}//Chamber Folders
			if(EffL && dir_Overview_local){
				dir_Overview_local->cd();
				heff->Write("",TObject::kOverwrite);
				heffML->Write("",TObject::kOverwrite);
				EffExtra->Divide(EffExtra_N);
				EffInner->Divide(EffInner_N);
				EffMiddle->Divide(EffMiddle_N);
				EffOuter->Divide(EffOuter_N);
				//Only Write Histogram if it originally had 36 y-axis bins
				if(ecap_str(0,1)=="E") {EffInner->Write("",TObject::kOverwrite); }
				else if(!EffInner_old) {EffInner->Write("",TObject::kOverwrite); }
				else delete EffInner_N;
				EffMiddle->Write("",TObject::kOverwrite);
				EffOuter->Write("",TObject::kOverwrite);
				EffExtra->Write("",TObject::kOverwrite);
			}
		}//End BA,BC,EA,EC
		if(EffG){
			if(dir_Overview_expert){ 
				dir_Overview_expert->cd();
				// Few histos moving to expert dir
				mf.setMetaData(dir_Overview_expert, hNumChambHitsPerEvt, hNumChambHitsPerEvtADCCut);
				if(hTotNumMDTHitsPerEvtBigADCCut && hTotNumMDTHitsPerEvtBig) mf.setMetaData(dir_Overview_expert, hTotNumMDTHitsPerEvtBig, hTotNumMDTHitsPerEvtBigADCCut);
				hNumChambHitsPerEvt->Write("",TObject::kOverwrite);
				hNumChambHitsPerEvtADCCut->Write("",TObject::kOverwrite);
				if(hTotNumMDTHitsPerEvtBig) hTotNumMDTHitsPerEvtBig->Write("",TObject::kOverwrite);
				if(hTotNumMDTHitsPerEvtBigADCCut) hTotNumMDTHitsPerEvtBigADCCut->Write("",TObject::kOverwrite);
				// Chamber volume maps // these are used below -- make sure they are left before the writes below
				// 	  VolumeMapBCap->Scale(1./VolumeMapBCap->GetMaximum());
				MDTSet2DRangeZ(VolumeMapBCap);
				VolumeMapBCap->Write("",TObject::kOverwrite);
				// 	  VolumeMapECap->Scale(1./VolumeMapECap->GetMaximum());
				MDTSet2DRangeZ(VolumeMapECap);
				VolumeMapECap->Write("",TObject::kOverwrite);
				// 	  VolumeMapBCap_noisecut->Scale(1./VolumeMapBCap_noisecut->GetMaximum());
				//MDTSet2DRangeZ(VolumeMapBCap_noisecut);
				//VolumeMapBCap_noisecut->Write("",TObject::kOverwrite);
				// 	  VolumeMapECap_noisecut->Scale(1./VolumeMapECap_noisecut->GetMaximum());
				//MDTSet2DRangeZ(VolumeMapECap_noisecut);
				//VolumeMapECap_noisecut->Write("",TObject::kOverwrite);
				// Hits per vol per event
				EvtTubeNormOccBCap = (TH2F*) EvtOccBCap->Clone();
				EvtTubeNormOccECap = (TH2F*) EvtOccECap->Clone();
				EvtTubeNormOccBCap->SetName("NormedHitsInBarrelPerChamber_ADCCut");
				EvtTubeNormOccBCap->SetTitle("Avg # hits/evt/m^3 Barrel, ADCCut");
				EvtTubeNormOccECap->SetName("NormedHitsInEndCapPerChamber_ADCCut");
				EvtTubeNormOccECap->SetTitle("Avg # hits/evt/m^3 Endcap, ADCCut");
				mf.setMetaData(dir_Overview, EvtTubeNormOccBCap, EvtTubeNormOccECap);
				EvtTubeNormOccBCap->Divide(VolumeMapBCap);
				MDTSet2DRangeZ(EvtTubeNormOccBCap);
				EvtTubeNormOccBCap->Write("",TObject::kOverwrite);
				EvtTubeNormOccECap->Divide(VolumeMapECap);
				MDTSet2DRangeZ(EvtTubeNormOccECap);
				EvtTubeNormOccECap->Write("",TObject::kOverwrite);
			}
			/*if(dir_Overview) {
                dir_Overview->cd();
                // Hits per vol per event, noise cut
                EvtTubeNormOccNoiseCutBCap = (TH2F*) EvtOccNoiseCutBCap->Clone();
                EvtTubeNormOccNoiseCutECap = (TH2F*) EvtOccNoiseCutECap->Clone();
                EvtTubeNormOccNoiseCutBCap->SetName("NormedHitsInBarrelPerChamber_ADCCut_NoiseCut");
                EvtTubeNormOccNoiseCutBCap->SetTitle("Avg # hits/evt/m^3 Barrel, ADCCut, 100KHz noise cut");
                EvtTubeNormOccNoiseCutECap->SetName("NormedHitsInEndCapPerChamber_ADCCut_NoiseCut");
                EvtTubeNormOccNoiseCutECap->SetTitle("Avg # hits/evt/m^3 Endcap, ADCCut, 100KHz noise cut");
                mf.setMetaData(dir_Overview, EvtTubeNormOccNoiseCutBCap, EvtTubeNormOccNoiseCutECap);
                EvtTubeNormOccNoiseCutBCap->Divide(VolumeMapBCap_noisecut);
                MDTSet2DRangeZ(EvtTubeNormOccNoiseCutBCap);
	  EvtTubeNormOccNoiseCutBCap->Write("",TObject::kOverwrite);
	  EvtTubeNormOccNoiseCutECap->Divide(VolumeMapECap_noisecut);
	  MDTSet2DRangeZ(EvtTubeNormOccNoiseCutECap);
	  EvtTubeNormOccNoiseCutECap->Write("",TObject::kOverwrite);
	}
	if(dir_Overview_expert_noisebkd){
	  dir_Overview_expert_noisebkd->cd();
	  NoiseBCap->Divide(VolumeMapBCap_noisecut);
	  MDTSet2DRangeZ(NoiseBCap);
	  NoiseBCap->Write("",TObject::kOverwrite);
	  NoiseECap->Divide(VolumeMapECap_noisecut);
	  MDTSet2DRangeZ(NoiseECap);
	  NoiseECap->Write("",TObject::kOverwrite);
	  BkgrdBCap->Divide(VolumeMapBCap_noisecut);
	  MDTSet2DRangeZ(BkgrdBCap);
	  BkgrdBCap->Write("",TObject::kOverwrite);
	  BkgrdECap->Divide(VolumeMapECap_noisecut);
	  MDTSet2DRangeZ(BkgrdECap);
	  BkgrdECap->Write("",TObject::kOverwrite);
	}*/
			if(dir_Overview_expert_RawHits) {
				dir_Overview_expert_RawHits->cd();
				// Raw hits per event (don't divide by volume map)
				MDTSet2DRangeZ(EvtOccBCap);
				EvtOccBCap->Write("",TObject::kOverwrite);
				MDTSet2DRangeZ(EvtOccECap);
				EvtOccECap->Write("",TObject::kOverwrite);
				//MDTSet2DRangeZ(EvtOccNoiseCutBCap);
				//EvtOccNoiseCutBCap->Write("",TObject::kOverwrite);
				//MDTSet2DRangeZ(EvtOccNoiseCutECap);
				//EvtOccNoiseCutECap->Write("",TObject::kOverwrite);
			}
		}

		// make corresponding summary text file
		if(txtFileOutputType > 0) {
			std::ofstream txtfile;
			std::string textfilename = inFilename + ".Summary.txt";
			txtfile.open(textfilename.c_str(), std::ios::app);	
			if( txtfile.is_open() ) {
				txtfile.precision(3);
				txtfile << "************************************************************************************************" << std::endl;
				txtfile << "**** Global summary of dead/noisy analysis: " << std::endl;
				txtfile << "******** Tubes:       " << std::setw(8) << numAllTubes << ",  Dead: " << std::setw(8) << std::fixed << 100.0 * (double)numDeadTubes / (double)numAllTubes << "%,  Noisy: " << std::setw(8) << 100.0 * (numAllTubes > 0 ? (double)numNoisyTubes / (double)numAllTubes : 0) << "%" << std::endl;
				txtfile << "******** Mezzanines:  " << std::setw(8) << numAllMezz << ",  Dead: " << std::setw(8) << std::fixed << 100.0 * (numAllMezz > 0 ? (double)numDeadMezz / (double)numAllMezz : 0) << "%,  Noisy: " << std::setw(9) << "n/a" << std::endl;
				txtfile << "******** Layers:      " << std::setw(8) << numAllLayers << ",  Dead: " << std::setw(8) << std::fixed << 100.0 * (numAllLayers > 0 ? (double)numDeadLayers / (double)numAllLayers : 0) << "%,  Noisy: " << std::setw(9) << "n/a" << std::endl;
				txtfile << "******** Multilayers: " << std::setw(8) << numAllML << ",  Dead: " << std::setw(8) << std::fixed << 100.0 * (numAllML > 0 ? (double)numDeadML / (double)numAllML : 0) << "%,  Noisy: " << std::setw(9) << "n/a" << std::endl;	  
				txtfile << "******** Chambers:    " << std::setw(8) << numAllChambers << ",  Dead: " << std::setw(8) << std::fixed << 100.0 * (numAllChambers > 0 ? (double)numDeadChambers / (double)numAllChambers : 0) << "%,  Noisy: " << std::setw(9) << "n/a" << std::endl;	    
				txtfile.close();
			}
		}
		//Fill Dead element info
		hDeadElements->Fill("Chambers",numDeadChambers);
		hDeadElements->Fill("MultiLayers",numDeadML);
		hDeadElements->Fill("Layers",numDeadLayers);
		hDeadElements->Fill("Mezzanines",numDeadMezz);
		hDeadElements->Fill("Tubes",numDeadTubes);
		hDeadElements->Fill("AllTubes",numDeadTubes_All);
		/*hNoisyElements->Fill("Chambers",0);
		hNoisyElements->Fill("MultiLayers",0);
		hNoisyElements->Fill("Layers",0);
		hNoisyElements->Fill("Mezzanines",0);
		hNoisyElements->Fill("Tubes",numNoisyTubes);
		//Fill Bkgrd/Noise Info
		TH1F* bkgrd_summary = new TH1F("bkgrd_summary","Background Rate; Region; average rate per tube [Hz]",1,0,1);
		TH1F* noise_summary = new TH1F("noise_summary","Noise Rate; Region; average rate per tube [Hz]",1,0,1);
		mf.setMetaData(dir_Overview_expert_noisebkd, bkgrd_summary, noise_summary);
		TH1F* BA_bkgrd = new TH1F("BA_bkgrd","Background Rate in Eta Rings BA; Eta Ring; average rate per tube [Hz]",1,0,1);
		TH1F* BA_noise = new TH1F("BA_noise","Noise Rate in Eta Rings BA; Eta Ring; average rate per tube [Hz]",1,0,1);
		mf.setMetaData(dir_Overview_expert_noisebkd, BA_bkgrd, BA_noise);
		TH1F* BC_bkgrd = new TH1F("BC_bkgrd","Background Rate in Eta Rings BC; Eta Ring; average rate per tube [Hz]",1,0,1);
		TH1F* BC_noise = new TH1F("BC_noise","Noise Rate in Eta Rings BC; Eta Ring; average rate per tube [Hz]",1,0,1);
		mf.setMetaData(dir_Overview_expert_noisebkd, BC_bkgrd, BC_noise);
		TH1F* EA_bkgrd = new TH1F("EA_bkgrd","Background Rate in Eta Rings EA; Eta Ring; average rate per tube [Hz]",1,0,1);
		TH1F* EA_noise = new TH1F("EA_noise","Noise Rate in Eta Rings EA; Eta Ring; average rate per tube [Hz]",1,0,1);
		mf.setMetaData(dir_Overview_expert_noisebkd, EA_bkgrd, EA_noise);
		TH1F* EC_bkgrd = new TH1F("EC_bkgrd","Background Rate in Eta Rings EC; Eta Ring; average rate per tube [Hz]",1,0,1);
		TH1F* EC_noise = new TH1F("EC_noise","Noise Rate in Eta Rings EC; Eta Ring; average rate per tube [Hz]",1,0,1);
		mf.setMetaData(dir_Overview_expert_noisebkd, EC_bkgrd, EC_noise);
		float bkgrd_overall = 0; float bkgrd_BA = 0; float bkgrd_BC = 0; float bkgrd_EA = 0; float bkgrd_EC = 0;
		float noise_overall = 0; float noise_BA = 0; float noise_BC = 0; float noise_EA = 0; float noise_EC = 0;
		int nTubes_overall = 0; int nTubes_BA = 0; int nTubes_BC = 0; int nTubes_EA = 0; int nTubes_EC = 0;*/
		/*for(std::map<TString,float>::const_iterator m_itr = bkgrd_map.begin(); m_itr != bkgrd_map.end(); ++m_itr){
	TString eta_ring = (*m_itr).first;
	float bkgrd = (*m_itr).second;
	float noise = 0;
	int nTubes = 0;
	std::map<TString,float>::const_iterator n_itr = noise_map.find(eta_ring.Data());
	std::map<TString,int>::const_iterator t_itr = nTubes_map.find(eta_ring.Data());
	if( n_itr != noise_map.end()) noise = (*n_itr).second;
	if( t_itr != nTubes_map.end()) nTubes = (*t_itr).second;
	bkgrd_overall+=bkgrd;
	noise_overall+=noise;
	nTubes_overall+=nTubes;
	if(eta_ring(0,1)=="B" && eta_ring(2,1)=="A") {
	  bkgrd_BA+=bkgrd; noise_BA+=noise; nTubes_BA+=nTubes;
	  if( eta_ring.Sizeof() >= 5 && eta_ring(1,1)!="E" && nTubes>0) {
	    BA_bkgrd->Fill(eta_ring, bkgrd/nTubes);
	    BA_noise->Fill(eta_ring, noise/nTubes);
	  }
	}
	if(eta_ring(0,1)=="B" && eta_ring(2,1)=="C") {
	  bkgrd_BC+=bkgrd; noise_BC+=noise; nTubes_BC+=nTubes;
	  if( eta_ring.Sizeof() >= 5 && eta_ring(1,1)!="E" && nTubes>0) {
	    BC_bkgrd->Fill(eta_ring, bkgrd/nTubes);
	    BC_noise->Fill(eta_ring, noise/nTubes);
	  }
	}
	if(eta_ring(0,1)=="E" && eta_ring(2,1)=="A") {
	  bkgrd_EA+=bkgrd; noise_EA+=noise; nTubes_EA+=nTubes;
	  if( eta_ring.Sizeof() >= 5 && eta_ring(1,1)!="E" && nTubes>0) {
	    EA_bkgrd->Fill(eta_ring, bkgrd/nTubes);
	    EA_noise->Fill(eta_ring, noise/nTubes);
	  }
	}
	if(eta_ring(0,1)=="E" && eta_ring(2,1)=="C") {
	  bkgrd_EC+=bkgrd; noise_EC+=noise; nTubes_EC+=nTubes;
	  if( eta_ring.Sizeof() >= 5 && eta_ring(1,1)!="E" && nTubes>0) {
	    EC_bkgrd->Fill(eta_ring, bkgrd/nTubes);
	    EC_noise->Fill(eta_ring, noise/nTubes);
	  }
	}
      }
      if (nTubes_overall>0) {
        bkgrd_summary->Fill("Overall",bkgrd_overall/nTubes_overall); 
        noise_summary->Fill("Overall",noise_overall/nTubes_overall); 
      }
      if (nTubes_BA>0) {
        bkgrd_summary->Fill("BA",bkgrd_BA/nTubes_BA); 
        noise_summary->Fill("BA",noise_BA/nTubes_BA); 
      }
      if (nTubes_BC>0) {
        bkgrd_summary->Fill("BC",bkgrd_BC/nTubes_BC); 
        noise_summary->Fill("BC",noise_BC/nTubes_BC); 
      }
      if (nTubes_EA>0) {
        bkgrd_summary->Fill("EA",bkgrd_EA/nTubes_EA); 
        noise_summary->Fill("EA",noise_EA/nTubes_EA); 
      }
      if (nTubes_EC>0) {
        bkgrd_summary->Fill("EC",bkgrd_EC/nTubes_EC); 
        noise_summary->Fill("EC",noise_EC/nTubes_EC); 
      }
		 
		EffVSNoise_num->Sumw2();
		EffVSNoise_den->Sumw2();
		TH1F* EffVSNoise = (TH1F*)EffVSNoise_num->Clone();
		EffVSNoise->SetName("EffVSNoise");
		EffVSNoise->SetTitle("Tube eff vs tube noise rate");
		EffVSNoise->GetXaxis()->SetTitle("noise rate [KHz]");
		EffVSNoise->GetYaxis()->SetTitle("eff");
		EffVSNoise->Divide(EffVSNoise_num,EffVSNoise_den,1,1,"B");
		mf.setMetaData(dir_Overview_expert_noisebkd,EffVSNoise);

		EffVSNoise_ADCCut_num->Sumw2();
		EffVSNoise_ADCCut_den->Sumw2();
		TH1F* EffVSNoise_ADCCut = (TH1F*)EffVSNoise_ADCCut_num->Clone();
		EffVSNoise_ADCCut->SetName("EffVSNoise_ADCCut");
		EffVSNoise_ADCCut->SetTitle("Tube eff vs tube (ADC>80) noise rate");
		EffVSNoise_ADCCut->GetXaxis()->SetTitle("noise rate [KHz]");
		EffVSNoise_ADCCut->GetYaxis()->SetTitle("eff");
		EffVSNoise_ADCCut->Divide(EffVSNoise_ADCCut_num,EffVSNoise_ADCCut_den,1,1,"B");
		mf.setMetaData(dir_Overview_expert_noisebkd,EffVSNoise_ADCCut);

		EffVSBkd_num->Sumw2();
		EffVSBkd_den->Sumw2();
		TH1F* EffVSBkd = (TH1F*)EffVSBkd_num->Clone();
		EffVSBkd->SetName("EffVSBkd");
		EffVSBkd->SetTitle("Tube eff vs tube (ADC>80) bkd rate");
		EffVSBkd->GetXaxis()->SetTitle("background rate [KHz]");
		EffVSBkd->GetYaxis()->SetTitle("eff");
		EffVSBkd->Divide(EffVSBkd_num,EffVSBkd_den,1,1,"B");
		mf.setMetaData(dir_Overview_expert_noisebkd,EffVSBkd);
*/
		if(dir_Overview_expert){
			dir_Overview_expert->cd();
			//MDTFinalizeWriteTH1FChar(hNoisyElements);
			MDTFinalizeWriteTH1FChar(hDeadElements);
		}
/*
		if(dir_Overview_expert_noisebkd && dir_Overview_expert){
			dir_Overview_expert_noisebkd->cd();
			hNoiseRate->Write("",TObject::kOverwrite);	
			EffVSNoise->Write("",TObject::kOverwrite);
			EffVSNoise_ADCCut->Write("",TObject::kOverwrite);
			EffVSBkd->Write("",TObject::kOverwrite);
			MDTFinalizeWriteTH1FChar(bkgrd_summary);
			MDTFinalizeWriteTH1FChar(noise_summary);	
			MDTFinalizeWriteTH1FChar(BA_bkgrd);
			MDTFinalizeWriteTH1FChar(BA_noise);	
			MDTFinalizeWriteTH1FChar(BC_bkgrd);
			MDTFinalizeWriteTH1FChar(BC_noise);	
			MDTFinalizeWriteTH1FChar(EA_bkgrd);
			MDTFinalizeWriteTH1FChar(EA_noise);	
			MDTFinalizeWriteTH1FChar(EC_bkgrd);
			MDTFinalizeWriteTH1FChar(EC_noise);		
		}
*/
		TDirectory* dir_Overview_expert_reco = dir_Overview_expert->GetDirectory("RecoMonitoring");
		if(dir_Overview_expert_reco && dir_Overview_expert) {
			dir_Overview_expert_reco->cd();	
			// 2D eff histos -- divide and change metadata before writing to new reco dir
			if (EffECap && EffBCap) {
				EffECap->Divide(EffECap_N);
				EffBCap->Divide(EffBCap_N);
				mf.setMetaData(dir_Overview_expert_reco, EffECap, EffBCap);
				EffECap->Write("",TObject::kOverwrite); 
				EffBCap->Write("",TObject::kOverwrite); 	  
			}
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////
	}//While loop over run_XXX directory

	return;
}

// This function is for grabbing overview hists from raw and doing something to them, rather than going through a chamber-by-chamber loop
void MonitoringFile::MDTChamOcc( std::string inFilename, std::string _title_ ) {
	MDTPostProcessor mf( inFilename , "ChamberOcc Calculations");
	if (!mf.IsOpen()) {
		std::cerr << "MDTPostProcess(): "
				<< "Input file not opened \n";
		return;
	}
	if(mf.GetSize()<1000.) {
		std::cerr << "MDTPostProcess(): "
				<< "Input file empty \n";
		return; 
	}

	TIter next_run ( mf.GetListOfKeys() );
	TKey* key_run(0);
	while ((key_run = dynamic_cast<TKey*> ( next_run() )) !=0 ) {
		TString run_dir = key_run->GetName();      
		if (!run_dir.Contains("run") )   continue;

		TDirectory* dir_Overview = mf.GetDirectory(run_dir + "/Muon/MuonRawDataMonitoring/"+_title_+"/Overview");
		if(!dir_Overview) continue;
		TH1F* hNTriggers = 0; mf.get("TotalNumber_of_MDT_hits_per_event_ADCCut",hNTriggers,dir_Overview);
		int nTriggers = 1;
		if(hNTriggers) {
			nTriggers = (int) hNTriggers->GetEntries();
		}

		TString expert_Area = run_dir + "/Muon/MuonRawDataMonitoring/MDT/Overview/Expert";
		TDirectory* dir_Overview_expert = mf.GetDirectory(expert_Area);

		TString RecoMonArea = expert_Area+"/RecoMonitoring";
		TDirectory* dir_Overview_expert_reco = mf.GetDirectory(RecoMonArea);
		if(dir_Overview_expert_reco && dir_Overview_expert) {
			dir_Overview_expert_reco->cd();
			TH2F* OccBCap = 0; mf.get("NumberOfHitsInBarrelPerChamber_onSegm_ADCCut",OccBCap,dir_Overview_expert_reco); 
			TH2F* OccECap = 0; mf.get("NumberOfHitsInEndCapPerChamber_onSegm_ADCCut",OccECap,dir_Overview_expert_reco); 
			TH2F* VolumeMapBCap = 0; mf.get("VolumeMapBarrel",VolumeMapBCap,dir_Overview_expert); 
			TH2F* VolumeMapECap = 0; mf.get("VolumeMapEndcap",VolumeMapECap,dir_Overview_expert); 
			if( OccBCap && OccECap && VolumeMapBCap && VolumeMapECap ){
				TH2F* OccBCap_norm = dynamic_cast<TH2F*> (OccBCap->Clone());
				TH2F* OccECap_norm = dynamic_cast<TH2F*> (OccECap->Clone());
				if (OccBCap_norm && OccECap_norm) {
					OccBCap_norm->Scale(1./nTriggers);
					OccBCap_norm->Divide(VolumeMapBCap);
					OccBCap_norm->SetName("HitsPerEventInBarrelPerChamber_onSegm_ADCCut");
					OccBCap_norm->SetTitle("Avg # hits-on-segm/evt/m^3 Barrel");
					OccECap_norm->Scale(1./nTriggers);	  
					OccECap_norm->Divide(VolumeMapECap);
					OccECap_norm->SetName("HitsPerEventInEndCapPerChamber_onSegm_ADCCut");
					OccECap_norm->SetTitle("Avg # hits-on-segm/evt/m^3 Endcap");
					MDTSet2DRangeZ(OccBCap_norm);
					MDTSet2DRangeZ(OccECap_norm);
					mf.setMetaData(dir_Overview_expert_reco, OccBCap_norm, OccECap_norm);
					OccBCap_norm->Write("",TObject::kOverwrite);
					OccECap_norm->Write("",TObject::kOverwrite);	
				}
			}
		}


	}//While loop over run_XXX directory

	return;
}

void MonitoringFile::MDTTDCSum( std::string inFilename, std::string _title_ ){ 

	MDTPostProcessor mf( inFilename, "Sector t0,tmax, & tdrift Calculations");

	if (!mf.IsOpen()) {
		std::cerr << "MDTPostProcess(): "
				<< "Input file not opened \n";
		return;
	}

	if(mf.GetSize()<1000.) {
		std::cerr << "MDTPostProcess(): "
				<< "Input file empty \n";
		return; 
	}


	TString run_dir = "";
	TIter next_run ( mf.GetListOfKeys() );
	TKey* key_run(0);

	while ((key_run = dynamic_cast<TKey*> ( next_run() )) !=0 ) {
		TObject* obj_run = key_run->ReadObj();
		TDirectory* tdir_run = dynamic_cast<TDirectory*>( obj_run );
		if (tdir_run ==0 ) {delete obj_run; continue; }
		TString tdir_run_name( tdir_run->GetName() );
		if (!tdir_run_name.Contains("run") ) continue;
		run_dir=tdir_run_name;

		//Compute Summary Hists
		TString Overview_Area = run_dir + "/Muon/MuonRawDataMonitoring/"+_title_;
		TDirectory* dir_Main = 0;

		if(mf.cd(Overview_Area)) dir_Main = mf.GetDirectory(Overview_Area);
		if(!dir_Main) { std::cerr << "No MDT Direcotry! "<< std::endl; mf.error(); return; }

		TIter next_ecap(dir_Main->GetListOfKeys());
		while( TKey* key_ecap = dynamic_cast<TKey*> (next_ecap()) ){//BA,BC,EA,EC
			if(strncmp(key_ecap->GetName(),"Overvi",6)==0) continue;	
			if(strncmp(key_ecap->GetName(),"Summar",6)==0) continue;
			TString dir_ecap_str = Overview_Area+"/"+key_ecap->GetName();
			//	TDirectory* dir_ecap = dynamic_cast<TDirectory*> (key_ecap->ReadObj());
			TDirectory* dir_ecap = mf.GetDirectory(dir_ecap_str);
			if(!dir_ecap) continue;
			TString ecap_str = key_ecap->GetName();
			TString ecap_strFull = ecap_str;
			if(ecap_str.Length()>4) ecap_str = ecap_str(3,2);
			else ecap_str = ecap_str(0,2);
			TIter next_Overview(dir_ecap->GetListOfKeys());
			while( TKey* key_Overview = dynamic_cast<TKey*> (next_Overview()) ){ //Overview Chambers
				if(strncmp(key_Overview->GetName(),"Chambe",6)==0) continue;		  
				TString sumt0_name = "MDT_t0_"+ecap_str;
				TString sumtmax_name = "MDT_tmax_"+ecap_str;
				TString sumtdrift_name = "MDT_tdrift_"+ecap_str;
				TString dir_Overview_str = dir_ecap_str+"/"+key_Overview->GetName();
				//	  TDirectory* dir_Overview = dynamic_cast<TDirectory*> (key_Overview->ReadObj());
				TDirectory* dir_Overview = mf.GetDirectory(dir_Overview_str);
				if(!dir_Overview) continue;
				TH1F* sumt0 = new TH1F(sumt0_name,sumt0_name,1,0,1);
				TH1F* sumtmax = new TH1F(sumtmax_name,sumtmax_name,1,0,1);
				TH1F* sumtdrift = new TH1F(sumtdrift_name,sumtdrift_name,1,0,1);
				mf.setDirectory(sumt0);
				sumt0->SetFillColor(42);
				sumt0->SetMarkerStyle(20);
				sumt0->SetMarkerColor(42);
				sumt0->SetAxisRange(400,800,"y");
				mf.setDirectory(sumtmax);
				sumtmax->SetFillColor(42);
				sumtmax->SetMarkerStyle(20);
				sumtmax->SetMarkerColor(42);
				sumtmax->SetAxisRange(800,1700,"y");
				mf.setDirectory(sumtdrift);
				sumtdrift->SetFillColor(42);
				sumtdrift->SetMarkerStyle(20);
				sumtdrift->SetMarkerColor(42);
				sumtdrift->SetAxisRange(0,1500,"y");
				// Ultraparanoia ...
				if(!(sumtdrift && sumt0 && sumtmax)) {
					std::cerr << "No Summary Histograms" << std::endl;
					mf.error();
					return;
				}	
				sumtmax->Reset(); sumt0->Reset(); sumtdrift->Reset();
				if(dir_Overview){
					if(!dir_Overview->GetDirectory("TDC")) continue;
				}
				TDirectory* dir_TDC = dir_Overview->GetDirectory("TDC"); //Directory of TDC spectra
				TIter next_tdc(dir_TDC->GetListOfKeys());
				int currentbin = 1;
				std::vector<TH1F*> hists;
				while(TKey* key_TDC = dynamic_cast<TKey*> (next_tdc())){//TDC spectra
					if(strncmp(key_TDC->GetName(),"metadata",6)==0) continue;	    
					TH1F* h = 0; mf.get(key_TDC->GetName(),h,dir_TDC);
					hists.push_back(h);
				}	  
				//TKeys now in alphabetical order after merge, revert to order they were initialized in ESDtoAOD step
				std::sort(hists.begin(), hists.end(), tdcSort);

				for(std::vector<TH1F*>::const_iterator hitr = hists.begin(); hitr!=hists.end(); ++hitr){
					TH1F* h = *hitr;
					if(!h) continue;

					double t0 = 0;
					double t0err = 0;
					double tmax = 0;
					double tmaxerr = 0;
					MDTFitTDC(h, t0, t0err, tmax, tmaxerr);

					TString layer;
					if(currentbin<17) layer = "In";
					else if (currentbin<33) layer = "Mid";
					else if (currentbin<49) layer = "Out";
					else layer = "Ext";
					int stPhi = currentbin%16;
					if(layer=="Ext" && (ecap_str=="BA"||ecap_str=="BC")) {
						stPhi = 2*currentbin%16;		
					}
					if(stPhi==0) stPhi=16;

					TString phi = returnString(stPhi);

					TString sector = ecap_str+"_"+layer+"_"+phi;
					if(h->GetEntries()<100) sector+="_OFF";
					else if(t0err>t0) sector+="_ERR";
					sumt0->Fill(sector,t0);
					sumt0->SetBinError(currentbin,t0err);

					sector = ecap_str+"_"+layer+"_"+phi;
					if(h->GetEntries()<100) sector+="_OFF";
					else if(tmaxerr>tmax) sector+="_ERR";
					sumtmax->Fill(sector,tmax);
					sumtmax->SetBinError(currentbin,tmaxerr);

					sector = ecap_str+"_"+layer+"_"+phi;
					if(h->GetEntries()<100) sector+="_OFF";
					else if(tmaxerr>tmax || t0err>t0 || tmax-t0 < 0 || tmax-t0 > 2000) sector+="_ERR";
					if(tmax-t0 < 0 || tmax-t0 > 2000) {
						sumtdrift->Fill(sector,0); 
						sumtdrift->SetBinError(currentbin,2000);
					}
					else {
						sumtdrift->Fill(sector,tmax-t0); 
						sumtdrift->SetBinError(currentbin,sqrt(tmaxerr*tmaxerr + t0err*t0err));
					}
					currentbin++;//initialized at 1 for first bin
					if(dir_TDC){
						dir_TDC->cd();
						h->SetAxisRange(0,h->GetBinContent(h->GetMaximumBin())*1.05,"y");
						h->Write("",TObject::kOverwrite);		
						//Seems to help to write to the tfile early and often
						//	      if (currentbin % 10 == 0) mf.Write();
					}
				}//TDC Spectra

				if(dir_Overview){
					dir_Overview->cd();
					sumt0->SetAxisRange(0,300,"y");
					sumtdrift->SetAxisRange(0,1200,"y");
					sumtmax->SetAxisRange(0,1500,"y");
					MDTFinalizeWriteTH1FChar(sumt0);
					MDTFinalizeWriteTH1FChar(sumtdrift);
					MDTFinalizeWriteTH1FChar(sumtmax);
					TH1F* hTDCSummary = 0; mf.get("MDTTDC_Summary_ADCCut_"+ecap_str,hTDCSummary, dir_Overview);
					TH1F* hTDCSummary_onsegm = 0; mf.get("MDTTDC_segm_Summary_ADCCut_"+ecap_str,hTDCSummary_onsegm, dir_Overview);
					if(hTDCSummary){
						// 	      double t0 = 0;
						// 	      double t0err = 0;
						// 	      double tmax = 0;
						// 	      double tmaxerr = 0;
						// 	      MDTFitTDC(hTDCSummary, t0, t0err, tmax, tmaxerr);
						hTDCSummary->Write("",TObject::kOverwrite);
					}
					if(hTDCSummary_onsegm){
						double t0 = 0;
						double t0err = 0;
						double tmax = 0;
						double tmaxerr = 0;
						MDTFitTDC(hTDCSummary_onsegm, t0, t0err, tmax, tmaxerr);
						hTDCSummary_onsegm->Write("",TObject::kOverwrite);
					}
				}
			}//Overview
		}

		TH1F* hTDCSummary = 0;
		TH1F* hTDCSummary_onsegm = 0;
		mf.get(Overview_Area+"/Overview/Overall_TDC_ADCCut_spectrum",hTDCSummary);
		mf.get(Overview_Area+"/Overview/Overall_TDC_onSegm_ADCCut_spectrum",hTDCSummary_onsegm);
		if(hTDCSummary){
			// 	double t0 = 0;
			// 	double t0err = 0;
			// 	double tmax = 0;
			// 	double tmaxerr = 0;
			// 	MDTFitTDC(hTDCSummary, t0, t0err, tmax, tmaxerr);
			mf.cd(Overview_Area+"/Overview/");
			hTDCSummary->Write("",TObject::kOverwrite);
		}
		if(hTDCSummary_onsegm){
			double t0 = 0;
			double t0err = 0;
			double tmax = 0;
			double tmaxerr = 0;
			MDTFitTDC(hTDCSummary_onsegm, t0, t0err, tmax, tmaxerr);
			mf.cd(Overview_Area+"/Overview/");
			hTDCSummary_onsegm->Write("",TObject::kOverwrite);
		}

	}//computing  All      

	return;
}

void  MonitoringFile::MDTLowStat( std::string inFilename, std::string _title_){

	MDTPostProcessor mf( inFilename, "MDT LowStat Analysis" );

	if (!mf.IsOpen()) {
		std::cerr << "MDTPostProcess(): "
				<< "Input file not opened \n";
		return;
	}

	if(mf.GetSize()<1000.) {
		std::cerr << "MDTPostProcess(): "
				<< "Input file empty \n";
		return; 
	}

	// get run directory name
	//Seemingly unnecessary lines are necessary
	TIter nextcd0(mf.GetListOfKeys());
	TKey *key0 = (TKey*)nextcd0();
	TDirectory *dir0= dynamic_cast<TDirectory*>(key0->ReadObj());
	if (!dir0) {
		std::cerr << "MDTPostProcess(): "
				<< "Unable to find run directory in input file\n";
		return;
	}
	///

	TIter next_run ( dir0->GetListOfKeys() );
	TKey* key_run = 0;
	while ((key_run = dynamic_cast<TKey*> ( next_run() )) !=0 ) {
		TKey* key_lowStat = 0;

		while ((key_lowStat = dynamic_cast<TKey*> ( next_run() )) !=0 ) {
			TString lowStat_str = key_lowStat->GetName();
			if(!lowStat_str.Contains("lowStat")) continue;

			TDirectory* lowStat_dir = dir0->GetDirectory(lowStat_str);
			if(!lowStat_dir) continue;
			TDirectory* dirOverview = lowStat_dir->GetDirectory(TString("Muon/MuonRawDataMonitoring/"+_title_+"/Overview"));
			TDirectory* dirOverviewBA = lowStat_dir->GetDirectory(TString("Muon/MuonRawDataMonitoring/"+_title_+"/MDTBA/Overview"));
			TDirectory* dirOverviewBC = lowStat_dir->GetDirectory(TString("Muon/MuonRawDataMonitoring/"+_title_+"/MDTBC/Overview"));
			TDirectory* dirOverviewEA = lowStat_dir->GetDirectory(TString("Muon/MuonRawDataMonitoring/"+_title_+"/MDTEA/Overview"));
			TDirectory* dirOverviewEC = lowStat_dir->GetDirectory(TString("Muon/MuonRawDataMonitoring/"+_title_+"/MDTEC/Overview"));

			if( !(dirOverview && dirOverviewBA && dirOverviewBC && dirOverviewEA && dirOverviewEC) ) {//Catch New Directory Name Fix
				dirOverview = lowStat_dir->GetDirectory(TString("Muon/MuonRawDataMonitoring/"+_title_+"/Overview_lowStat"));
				dirOverviewBA = lowStat_dir->GetDirectory(TString("Muon/MuonRawDataMonitoring/"+_title_+"/MDTBA/Overview_lowStat"));
				dirOverviewBC = lowStat_dir->GetDirectory(TString("Muon/MuonRawDataMonitoring/"+_title_+"/MDTBC/Overview_lowStat"));
				dirOverviewEA = lowStat_dir->GetDirectory(TString("Muon/MuonRawDataMonitoring/"+_title_+"/MDTEA/Overview_lowStat"));
				dirOverviewEC = lowStat_dir->GetDirectory(TString("Muon/MuonRawDataMonitoring/"+_title_+"/MDTEC/Overview_lowStat"));
			}

			if( !(dirOverview && dirOverviewBA && dirOverviewBC && dirOverviewEA && dirOverviewEC) ) continue;//Make sure mdt has lowStat entires before continuing

			TH1F* tdc = 0; mf.get("Overall_TDC_ADCCut_spectrum",tdc,dirOverview);
			TH1F* tdcBA = 0; mf.get("MDTTDC_Summary_ADCCut_BA",tdcBA,dirOverviewBA);
			TH1F* tdcBC = 0; mf.get("MDTTDC_Summary_ADCCut_BC",tdcBC,dirOverviewBC);
			TH1F* tdcEA = 0; mf.get("MDTTDC_Summary_ADCCut_EA",tdcEA,dirOverviewEA);
			TH1F* tdcEC = 0; mf.get("MDTTDC_Summary_ADCCut_EC",tdcEC,dirOverviewEC);
			TH1F* tdc_onSegm = 0; mf.get("Overall_TDC_onSegm_ADCCut_spectrum",tdc_onSegm,dirOverview);
			TH1F* tdcBA_onSegm = 0; mf.get("MDTTDC_segm_Summary_ADCCut_BA",tdcBA_onSegm,dirOverviewBA);
			TH1F* tdcBC_onSegm = 0; mf.get("MDTTDC_segm_Summary_ADCCut_BC",tdcBC_onSegm,dirOverviewBC);
			TH1F* tdcEA_onSegm = 0; mf.get("MDTTDC_segm_Summary_ADCCut_EA",tdcEA_onSegm,dirOverviewEA);
			TH1F* tdcEC_onSegm = 0; mf.get("MDTTDC_segm_Summary_ADCCut_EC",tdcEC_onSegm,dirOverviewEC);

			double t0 = 0;
			double t0err = 0;
			double tmax = 0;
			double tmaxerr = 0;

			if(tdc && dirOverview) { 
				// 	  MDTFitTDC(tdc,t0,t0err,tmax,tmaxerr);
				dirOverview->cd();
				tdc->Write("",TObject::kOverwrite);
			}
			if(tdcBA && dirOverviewBA) { 
				// 	  MDTFitTDC(tdcBA,t0,t0err,tmax,tmaxerr);
				dirOverviewBA->cd();
				tdcBA->Write("",TObject::kOverwrite);
			}
			if(tdcBC && dirOverviewBC) { 
				// 	  MDTFitTDC(tdcBC,t0,t0err,tmax,tmaxerr);
				dirOverviewBC->cd();
				tdcBC->Write("",TObject::kOverwrite);
			}
			if(tdcEA && dirOverviewEA) { 
				// 	  MDTFitTDC(tdcEA,t0,t0err,tmax,tmaxerr);
				dirOverviewEA->cd();
				tdcEA->Write("",TObject::kOverwrite);
			}
			if(tdcEC && dirOverviewEC) { 
				// 	  MDTFitTDC(tdcEC,t0,t0err,tmax,tmaxerr);
				dirOverviewEC->cd();
				tdcEC->Write("",TObject::kOverwrite);
			}
			if(tdc_onSegm && dirOverview) { 
				MDTFitTDC(tdc_onSegm,t0,t0err,tmax,tmaxerr);
				dirOverview->cd();
				tdc_onSegm->Write("",TObject::kOverwrite);
			}
			if(tdcBA_onSegm && dirOverviewBA) { 
				MDTFitTDC(tdcBA_onSegm,t0,t0err,tmax,tmaxerr);
				dirOverviewBA->cd();
				tdcBA_onSegm->Write("",TObject::kOverwrite);
			}
			if(tdcBC_onSegm && dirOverviewBC) { 
				MDTFitTDC(tdcBC_onSegm,t0,t0err,tmax,tmaxerr);
				dirOverviewBC->cd();
				tdcBC_onSegm->Write("",TObject::kOverwrite);
			}
			if(tdcEA_onSegm && dirOverviewEA) { 
				MDTFitTDC(tdcEA_onSegm,t0,t0err,tmax,tmaxerr);
				dirOverviewEA->cd();
				tdcEA_onSegm->Write("",TObject::kOverwrite);
			}
			if(tdcEC_onSegm && dirOverviewEC) { 
				MDTFitTDC(tdcEC_onSegm,t0,t0err,tmax,tmaxerr);
				dirOverviewEC->cd();
				tdcEC_onSegm->Write("",TObject::kOverwrite);
			}
		}// All LowStat Dirs
	}// All Runs  

	return;

}

bool MonitoringFile::MDTCheckAlign(std::string inFilename, std::string _title_){

	TFile* f = TFile::Open(inFilename.c_str(),"READ");

	if (f == 0) {
		std::cerr << "MonitoringFile::MDTPostProcess(): "
				<< "Input file not opened \n";
		return true;
	}
	if(f->GetSize()<1000.) {
		std::cerr << "MonitoringFile::MDTPostProcess(): "
				<< "Input file empty \n";
		return true; 
	}

	TIter nextcd0(gDirectory->GetListOfKeys());
	TKey *key0 = (TKey*)nextcd0();
	TDirectory *dir0= dynamic_cast<TDirectory*> (key0->ReadObj());
	dir0->cd();

	int keysBA = 0;
	int keysBC = 0;
	int keysEA = 0;
	int keysEC = 0;
	TDirectory* dirBA = dir0->GetDirectory(TString("Muon/MuonRawDataMonitoring/"+_title_+"/MDTBA/Chambers"));
	if(dirBA) keysBA = dirBA->GetNkeys();
	TDirectory* dirBC = dir0->GetDirectory(TString("Muon/MuonRawDataMonitoring/"+_title_+"/MDTBC/Chambers"));
	if(dirBC) keysBC = dirBC->GetNkeys();
	TDirectory* dirEA = dir0->GetDirectory(TString("Muon/MuonRawDataMonitoring/"+_title_+"/MDTEA/Chambers"));
	if(dirEA) keysEA = dirEA->GetNkeys();
	TDirectory* dirEC = dir0->GetDirectory(TString("Muon/MuonRawDataMonitoring/"+_title_+"/MDTEC/Chambers"));
	if(dirEC) keysEC = dirEC->GetNkeys();

	bool aligned = (keysBA < 600 && keysBC < 600 && keysEA < 600 && keysEC < 600);

	f->Close();
	delete f;

	return aligned;
}

bool MonitoringFile::MDTCheck(std::string inFilename){

	TFile* f = TFile::Open(inFilename.c_str(),"READ");

	if (f == 0 || ! f->IsOpen()) {
		std::cerr << "MonitoringFile::MDTPostProcess(): "
				<< "Input file not opened \n";
		delete f;
		return false;
	}
	if(f->GetSize()<1000.) {
		std::cerr << "MonitoringFile::MDTPostProcess(): "
				<< "Input file empty \n";
		delete f;
		return false; 
	}

	TIter nextcd0(gDirectory->GetListOfKeys());
	TKey *key0 = (TKey*)nextcd0();
	TDirectory *dir0 = 0;
	while (key0 != 0 && dir0 == 0) {
		dir0= dynamic_cast<TDirectory*> (key0->ReadObj());
		key0 = (TKey*)nextcd0();
	}
	if (dir0 == 0) {
		std::cerr << "MonitoringFile::MDTPostProcess(): "
				<< "Can't find top level directory \n";
		delete f;
		return false;
	}
	dir0->cd();

	TDirectory* dir = dir0->GetDirectory("Muon/MuonRawDataMonitoring/MDT");
	bool goodMDTFile = dir!=0;
	f->Close();
	delete f;
	return goodMDTFile;
}

void MonitoringFile::MDTTubeEff(const TString & dirName, const TH1F* num, const TH1F* den, TH1F* eff, float &countsML1, float &countsML2, float &entriesML1, float &entriesML2){
	int nBinsNum = num->GetNbinsX();
	int BinsML1 = nBinsNum/2;
	if(dirName(0,4)=="BIS8" || dirName(0,2)=="BE") BinsML1=nBinsNum; // only one ML
	countsML1 = 0;
	countsML2 = 0;
	entriesML1 = 0;
	entriesML2 = 0;
	if(nBinsNum!=den->GetNbinsX()) return;
	if(nBinsNum!=eff->GetNbinsX()) return;
	eff->Reset();
	for(int ibin = 1; ibin !=nBinsNum+1; ++ibin){
		float entries    = den->GetBinContent(ibin);
		float counts     = num->GetBinContent(ibin);
		float efficiency = (counts+1.)/(entries+2.);
		float error      = sqrt(efficiency*(1-efficiency))/sqrt(entries+3.);  
		if(ibin<BinsML1 || BinsML1==nBinsNum) {countsML1+=counts; entriesML1+=entries;}
		else {countsML2+=counts; entriesML2+=entries;}
		eff->SetBinContent(ibin, efficiency);
		eff->SetBinError(ibin,error);
	}
}

void MonitoringFile::MDT2DHWName(TString hardware_name, TString &stateta_IMO_c, TString &statphi_IMO_c, TString &stateta_c, TString &statphi_c,
		TString &statphi_c2){

	TString statphi_s = hardware_name(5,2);
	TString Barrel = hardware_name(0,1);
	TString Side = hardware_name(4,1);
	TString hwname = hardware_name(1,1);

	TString eta_s = hardware_name(3,1);

	//BME1[AC]14 (mistake - is actually 13 - so account for both cases!) -->BME4[AC]13 in histogram position
	if( hardware_name(0,3) == "BME" ){
		eta_s = "4";
		statphi_s = "13";
	}    

	statphi_c = statphi_s+",1";
	statphi_c2 = statphi_s+",2";

	if(hardware_name(0,3)=="BIR" || hardware_name(0,3)=="BIM") {
		statphi_c = statphi_c +","+hardware_name(2,1);
		statphi_c2 = statphi_c2 +","+hardware_name(2,1);
	}

	//BML[45][AC]13-->BML[56][AC]13
	if( hardware_name(0,3) == "BML" && TString(hardware_name(3,1)).Atoi() >= 4 && hardware_name(5,2) == "13" ){
		eta_s = returnString( TString(hardware_name(3,1)).Atoi() + 1 );
	}

        //BMF1,2,3 corresponds to eta stations 1,3,5
        if( hardware_name(0,3) == "BMF"){
            eta_s = returnString(TString(hardware_name(3,1)).Atoi()*2 - 1);
        }

	stateta_c = hardware_name(0,2);
	stateta_c += hardware_name(4,1);
	stateta_c+=eta_s;

	stateta_IMO_c = hardware_name(0,1);
	stateta_IMO_c += hardware_name(4,1);
	stateta_IMO_c += eta_s;

	statphi_IMO_c = hwname+","+statphi_s;

}

Double_t MonitoringFile::fittzero(Double_t *x,Double_t *par)
{
	Double_t fitvaltzero = par[0] + ( par[3] / ( 1 + ( TMath::Exp((-x[0]+par[1])/par[2]) ) ) );
	return fitvaltzero; 
} 

Double_t MonitoringFile::fittmax(Double_t *x,Double_t *par)
{  
	Double_t fitvaltmax = par[0] + ( par[3] / ( 1 + ( TMath::Exp((x[0]-par[1])/par[2]) ) ) );
	return fitvaltmax; 
} 

void  MonitoringFile::MDTFitTDC(TH1F* h, double &t0, double &t0err, double &tmax, double &tmaxerr){
	t0 = tmax = 0;
	t0err = tmaxerr = 0;
	double up = h->GetBinCenter(h->GetMaximumBin()+1);
	if( up > 200 ) up = 200;
	double down = up + 650;
	if( up < 50 ) up = 50;
	double parESD0 = h->GetBinContent(h->GetMinimumBin()); 
	double parESD1 = up;
	double parESD2 = 20;
	double parESD3 = h->GetBinContent(h->GetMaximumBin()) - h->GetBinContent(h->GetMinimumBin());  
	TF1 func1("func1", &fittzero,(Double_t)(0.),(Double_t)(up), 4);
	func1.SetParameters(parESD0, parESD1, parESD2, parESD3);
	func1.SetLineColor(kBlue+2);
	if(h->GetEntries()>100){
		h->Fit("func1","RQ");
		t0 = func1.GetParameter(1) ;
		t0err = func1.GetParError(1);
		double binAtT0 = (double)h->GetBinContent(h->FindBin(t0));
		if(binAtT0<1) binAtT0 = 1;
		t0err += 10.0 * func1.GetChisquare() / (0.01*binAtT0*binAtT0*(double)func1.GetNumberFitPoints()); // to additionally account for bad fits
	}

	parESD0 = h->GetBinContent(h->GetMinimumBin());
	parESD1 = down;
	parESD2 = 50;
	parESD3 = (h->GetBinContent(h->GetMaximumBin())-h->GetBinContent(h->GetMinimumBin()))/10.;
	TF1 func2("func2", &fittmax, (Double_t)(down-135), (Double_t)(down+135), 4);
	func2.SetParameters(parESD0,parESD1,parESD2,parESD3);
	func2.SetLineColor(kRed+1);
	if(h->GetEntries()>100){
		func2.SetParLimits(0, parESD0, 2.0*parESD0+1);
		func2.SetParLimits(2, 5, 90);
		func2.SetParLimits(3, 0.2*parESD3, 7*parESD3);
		h->Fit("func2","WWRQ+");
		//       std::cout << h->GetName() << std::endl;
		//       std::cout << " " << func2.GetParameter(0) << " " << func2.GetParameter(1) << " " << func2.GetParameter(2) << " " << func2.GetParameter(3) << std::endl;
		tmax = func2.GetParameter(1);
		tmaxerr = func2.GetParError(1);
		double binAtTmax = (double)h->GetBinContent(h->FindBin(tmax));
		if(binAtTmax<1) binAtTmax = 1;
		tmaxerr += 10.0 * func2.GetChisquare() / (0.01*binAtTmax*binAtTmax*(double)func2.GetNumberFitPoints()); // to additionally account for bad fits
	}

	h->GetXaxis()->SetTitle("[nsec] t0 = "+returnString(t0)+" +/- "+returnString(t0err)+" tmax = "+returnString(tmax)+" +/- "+returnString(tmaxerr));

	return;
}

void MonitoringFile::MDTSetMetaData(TDirectory* /*targetDir*/, TH1* /*h1*/, TH1* /*h2*/, TH1* /*h3*/){
	return;
	// TTree* metadata_overview_tr = 0;
	// if(!targetDir || !h1) return;
	// targetDir->GetObject("metadata",metadata_overview_tr);
	// //Fill Metadata for new objs.
	// std::string name_;
	// std::string intervalData_="run";
	// std::string chainData_="<none>";
	// std::string mergeData_="<default>";
	// if(metadata_overview_tr){
	//   OutputMetadata metadata_overview(metadata_overview_tr);
	//   char name[99];
	//   char intervalData[99];
	//   char chainData[99];
	//   char mergeData[99];
	//   TBranch* b_name;
	//   TBranch* b_intervalData;
	//   TBranch* b_chainData;
	//   TBranch* b_mergeData;
	//   TTree* tClone = (TTree*) metadata_overview_tr->Clone();
	//   metadata_overview_tr->SetDirectory(0);
	//   tClone->SetBranchAddress("Name",name,&b_name);
	//   tClone->SetBranchAddress("Interval",intervalData,&b_intervalData);
	//   tClone->SetBranchAddress("TriggerChain",chainData,&b_chainData);
	//   tClone->SetBranchAddress("MergeMethod",mergeData,&b_mergeData);
	//   //Check if Metadata Already Exists
	//   bool isMeta = false;
	//   for(int i = 0; i != tClone->GetEntries(); ++i){
	// 	tClone->GetEntry(i);
	// 	intervalData_ = intervalData;
	// 	chainData_ = chainData;
	// 	mergeData_ = mergeData;
	// 	name_ = name;
	// 	if(name_ == (std::string) h1->GetName()) isMeta = true;
	//   }
	//   delete tClone;
	//   if(isMeta) {
	// 	metadata_overview_tr->SetDirectory(0);
	// 	delete metadata_overview_tr;
	// 	return;
	//   }
	//   metadata_overview.fill(h1->GetName(), intervalData_.c_str(), chainData_.c_str(), mergeData_.c_str());
	//   if(h2) metadata_overview.fill(h2->GetName(), intervalData_.c_str(), chainData_.c_str(), mergeData_.c_str());
	//   if(h3) metadata_overview.fill(h3->GetName(), intervalData_.c_str(), chainData_.c_str(), mergeData_.c_str());
	//   //set new hists to 0 dir
	//   TDirectory* tempDir = gDirectory;
	//   targetDir->cd();
	//   metadata_overview_tr->Write("",TObject::kOverwrite);
	//   tempDir->cd();
	// }
	// else {
	//   TTree*  metadata_overview_tr = new TTree( "metadata", "Monitoring Metadata" );
	//   OutputMetadata metadata_overview(metadata_overview_tr);      
	//   if(h1) metadata_overview.fill( h1->GetName(), intervalData_.c_str(), chainData_.c_str(), mergeData_.c_str());
	//   TDirectory* tempDir = gDirectory;
	//   targetDir->cd();
	//   h1->SetDirectory(0);
	//   metadata_overview_tr->Write("",TObject::kOverwrite);
	//   metadata_overview_tr->SetDirectory(0);//make this safe to delete
	//   delete metadata_overview_tr;
	//   tempDir->cd();      
	// }
}

void MonitoringFile::MDTFinalizeWriteTH1FChar(TH1F* h){
	h->LabelsDeflate("x");
	if(h->GetNbinsX()>1) h->LabelsOption("v","x");
	h->Write("",TObject::kOverwrite);
}

void MonitoringFile::GetTubeLength( double & tubeLength, TString dirName ) {
	if( dirName(0,3) == "BEE" ) tubeLength = .9115;
	else if( dirName(0,3) == "BIS" && dirName(3,1) != "8" ) tubeLength = 1.6715;
	else if( dirName(0,4) == "BIS8" ) tubeLength = .8515;
	else if( dirName(0,3) == "BIL" ) tubeLength = 2.6715;
	else if( dirName(0,3) == "BIM" ) tubeLength = 1.5365;
	else if( dirName(0,3) == "BIR" ) tubeLength = 1.1055;
	else if( dirName(0,4) == "BIR1" ) tubeLength = 2.6715;
	else if( dirName(0,4) == "BIR2" || dirName(0,4) == "BIR4" || dirName(0,4) == "BIR5" ) tubeLength = 1.5365;
	else if( dirName(0,4) == "BIR3" || dirName(0,4) == "BIR6" ) tubeLength = 1.1055;
	else if( dirName(0,3) == "BME" ) tubeLength = 2.15; //approximate!
	else if( dirName(0,3) == "BMG" ) tubeLength = 1.12; //approximate!
	else if( dirName(0,3) == "BML" ) tubeLength = 3.5515;
	else if( dirName(0,3) == "BMS" || dirName(0,3) == "BMF" ) tubeLength = 3.0715;
	else if( dirName(0,3) == "BOL" ) tubeLength = 4.9615;
	else if( dirName(0,3) == "BOG" ) tubeLength = 3.7715;
	else if( dirName(0,3) == "BOS" || dirName(0,3) == "BOF" ) tubeLength = 3.7733;
	else if( dirName(0,4) == "EES1" ) tubeLength = 2.1945;
	else if( dirName(0,4) == "EES2" ) tubeLength = 2.5905;
	else if( dirName(0,4) == "EEL1" && dirName(5,2) == "05" ) tubeLength = 2.6265;
	else if( dirName(0,4) == "EEL1" ) tubeLength = 4.2015;
	else if( dirName(0,4) == "EEL2" ) tubeLength = 3.6015;	   
	else if( dirName(0,4) == "EIS1" ) tubeLength = 1.0605;
	else if( dirName(0,4) == "EIS2" ) tubeLength = 1.4115;
	else if( dirName(0,4) == "EIL1" ) tubeLength = 1.5465;
	else if( dirName(0,4) == "EIL2" ) tubeLength = 2.0865;
	else if( dirName(0,4) == "EIL3" && (dirName(5,2) == "11" || dirName(5,2) == "15") ) tubeLength = 1.7415;
	else if( dirName(0,4) == "EIL3" ) tubeLength = 2.0715;
	else if( dirName(0,4) == "EIL4" && (dirName(5,2) == "11" || dirName(5,2) == "15") ) tubeLength = 1.5515;
	else if( dirName(0,4) == "EIL4" && (dirName(5,2) == "01" || dirName(5,2) == "09") ) tubeLength = 1.5365;
	else if( dirName(0,4) == "EIL4" && (dirName(5,2) == "03" || dirName(5,2) == "05" || dirName(5,2) == "13") ) tubeLength = 2.8015;
	else if( dirName(0,4) == "EIL4" && (dirName(5,2) == "07") ) tubeLength = 2.0115;
	else if( dirName(0,4) == "EIL5" && (dirName(5,2) == "01" || dirName(5,2) == "09") ) tubeLength = 2.8015;
	else if( dirName(0,4) == "EML1" ) tubeLength = 1.5465;
	else if( dirName(0,4) == "EML2" ) tubeLength = 2.4465;
	else if( dirName(0,4) == "EML3" ) tubeLength = 3.4065;
	else if( dirName(0,4) == "EML4" ) tubeLength = 4.3665;
	else if( dirName(0,4) == "EML5" ) tubeLength = 5.3265;
	else if( dirName(0,4) == "EMS1" ) tubeLength = 1.0875;
	else if( dirName(0,4) == "EMS2" ) tubeLength = 1.6635;
	else if( dirName(0,4) == "EMS3" ) tubeLength = 2.2395;
	else if( dirName(0,4) == "EMS4" ) tubeLength = 2.8155;
	else if( dirName(0,4) == "EMS5" ) tubeLength = 3.3915;
	else if( dirName(0,4) == "EOL1" ) tubeLength = 2.0415;
	else if( dirName(0,4) == "EOL2" ) tubeLength = 3.0015;
	else if( dirName(0,4) == "EOL3" ) tubeLength = 3.7815;
	else if( dirName(0,4) == "EOL4" ) tubeLength = 4.5015;
	else if( dirName(0,4) == "EOL5" ) tubeLength = 5.2215;
	else if( dirName(0,4) == "EOL6" ) tubeLength = 5.9415;
	else if( dirName(0,4) == "EOS1" ) tubeLength = 1.4655;
	else if( dirName(0,4) == "EOS2" ) tubeLength = 1.9695;
	else if( dirName(0,4) == "EOS3" ) tubeLength = 2.4735;
	else if( dirName(0,4) == "EOS4" ) tubeLength = 2.9415;
	else if( dirName(0,4) == "EOS5" ) tubeLength = 3.3735;
	else if( dirName(0,4) == "EOS6" ) tubeLength = 3.8055;
	// 	    else std::cout << "Missing chamb: " << dirName << " " << nTubes << std::endl;
}

TString MonitoringFile::returnString(int a){
	std::stringstream ss; ss << a;
	TString b; ss >> b;
	return b;
}

TString MonitoringFile::returnString(double a){
	std::stringstream ss; ss << a;
	TString b; ss >> b;
	return b;
}

/////////dead/noisy/ineff functions

TString MonitoringFile::ConvertTubeVectorToString( std::vector<int> & v, TH1F* h, TString chamber ) {
	TString theList;
	if( v.size() > 0 ) theList = TubeID_to_ID_L_ML(v.at(0), chamber, h->GetNbinsX());
	for(unsigned int i=1; i<v.size(); i++) theList += "," + TubeID_to_ID_L_ML(v.at(i), chamber, h->GetNbinsX());
	return theList;
}

TString MonitoringFile::ConvertLayerVectorToString( std::vector<int> & v, TString chamber ) {
	TString theList;
	if( v.size() > 0 ) theList = Layer_to_L_ML(v.at(0), chamber);
	for(unsigned int i=1; i<v.size(); i++) theList += "," + Layer_to_L_ML(v.at(i), chamber);
	return theList;
}

TString MonitoringFile::ConvertMezzVectorToString( std::vector<int> & v, TH1F* h, TString chamber ) {
	TString theList;
	if( v.size() > 0 ) theList = Mezz_to_ML_mezz(v.at(0), chamber, h->GetNbinsX());
	for(unsigned int i=1; i<v.size(); i++) theList += "," + Mezz_to_ML_mezz(v.at(i), chamber, h->GetNbinsX());
	return theList;
}

TString MonitoringFile::ConvertVectorToString( std::vector<int> & v ) {
	TString theList;
	if( v.size() > 0 ) theList = returnString(v.at(0));
	for(unsigned int i=1; i<v.size(); i++) theList += "," + returnString(v.at(i));
	return theList;
}

TString MonitoringFile::Mezz_to_ML_mezz(int mezz, const TString & hardware_name, int totalTubes) {
	int mezzML = dqutils_mdtdeadnoisy::Get_ML_of_Mezz_degenerate(mezz, hardware_name, totalTubes);

	// Convert to string format
	TString mezz_str;
	if(mezz<10) mezz_str = "0" + returnString(mezz);
	else mezz_str = returnString(mezz);

	return (returnString(mezzML)+mezz_str);
}

TString MonitoringFile::TubeID_to_ID_L_ML(int & tubeID, const TString & hardware_name, int totalTubes) {
	int derived_tube = 1;
	int derived_layer = 1;
	int derived_ML = 1;

	// Get Number of X
	int derived_NumTubePerLayer = 1;
	int derived_NumTubePerML = 1;
	int derived_NumLayer = 1;
	int derived_NumLayerPerML = 1;
	int derived_NumML = 1;
	if( hardware_name(0,4) == "BIS8" /*&& hardware_name(5,2) == "12"*/ ) {
		derived_NumLayerPerML = 3;
		derived_NumML = 1;
	}
	else if( /*hardware_name(0,4) == "BIS8" ||*/ hardware_name(0,3) == "BEE" ) {
		derived_NumLayerPerML = 4;
		derived_NumML = 1;    
	}
	else if( hardware_name(0,2) == "BI" || hardware_name(0,2) == "EI" ) {
		derived_NumLayerPerML = 4;
		derived_NumML = 2;
	}
	else {
		derived_NumLayerPerML = 3;
		derived_NumML = 2;
	}

	derived_NumLayer = derived_NumLayerPerML * derived_NumML;
	derived_NumTubePerML = totalTubes / derived_NumML;
	derived_NumTubePerLayer = totalTubes / derived_NumLayer;

	// Corrections for derived_NumTubePerLayer
	if( hardware_name(0,4) == "BMS4" || hardware_name(0,4) == "BMS6" )
		derived_NumTubePerLayer = 48;
	if((hardware_name(5,2) == "11" || hardware_name(5,2) == "15")) {
		if( hardware_name(0,4) == "BIR1" )
			derived_NumTubePerLayer = 30;
		if( hardware_name(0,4) == "BIR2" )
			derived_NumTubePerLayer = 30;
		if( hardware_name(0,4) == "BIR3" )
			derived_NumTubePerLayer = 36;
		if( hardware_name(0,4) == "BIR4" )
			derived_NumTubePerLayer = 30;
		if( hardware_name(0,4) == "BIR5" )
			derived_NumTubePerLayer = 24;
	}
	//     if( hardware_name(0,4)=="EIL4" && ( hardware_name(5,2)=="09" || hardware_name(5,2)=="01" ) ) // possible MdtIdHelper problem
	//       derived_NumTubePerLayer = 54;

	// Now get X
	derived_ML = (tubeID-1)/derived_NumTubePerML + 1;
	derived_layer = (tubeID-1)/derived_NumTubePerLayer + 1;
	derived_tube = tubeID - (derived_layer - 1) * derived_NumTubePerLayer;
	if(derived_ML==2) derived_layer -= derived_NumLayerPerML;

	// Convert to string format
	TString tube_str;
	if(derived_tube<10) tube_str = "0" + returnString(derived_tube);
	else tube_str = returnString(derived_tube);

	return (returnString(derived_ML)+returnString(derived_layer)+tube_str);
}

TString MonitoringFile::Layer_to_L_ML(int & layer, const TString & hardware_name) {
	int derived_layer = 1;
	int derived_ML = 1;

	// Get Number of X
	// int derived_NumLayer = 1;
	int derived_NumLayerPerML = 1;
	// int derived_NumML = 1;
	if( hardware_name(0,4) == "BIS8" /*&& hardware_name(5,2) == "12"*/ ) {
		derived_NumLayerPerML = 3;
		// derived_NumML = 1;
	}
	else if( /*hardware_name(0,4) == "BIS8" ||*/ hardware_name(0,3) == "BEE" ) {
		derived_NumLayerPerML = 4;
		// derived_NumML = 1;    
	}
	else if( hardware_name(0,2) == "BI" || hardware_name(0,2) == "EI" ) {
		derived_NumLayerPerML = 4;
		// derived_NumML = 2;
	}
	else {
		derived_NumLayerPerML = 3;
		// derived_NumML = 2;
	}
	// derived_NumLayer = derived_NumLayerPerML * derived_NumML;

	// Now get X
	derived_ML = (layer-1)/derived_NumLayerPerML + 1;
	derived_layer = layer;
	if(derived_ML==2) derived_layer -= derived_NumLayerPerML;

	// Convert to string format and return
	return (returnString(derived_ML)+returnString(derived_layer));
}

void MonitoringFile::MDTSet2DRangeZ(TH2F* h) {   
	double minz = 0.00000001; 
	double maxz = h->GetMaximum(); 
	if(h->GetMinimum(minz) > minz) minz = h->GetMinimum(minz);
	if( maxz <= minz ) return;
	if( maxz/minz < 10.0 ) maxz = 10.0*minz; // always give range >= factor of 10 so that the labels will show in logz (root is stupid)

	h->SetAxisRange(minz,maxz,"z");
}

void MonitoringFile::MDTResetContents(TH2F* h) {    
	// copy all functions of histogram before resetting
	TList* h_funcs; 
	h_funcs = dynamic_cast<TList*> (h->GetListOfFunctions()->Clone());
	h->Reset(); 
	//now insert these functions back into the hist
	TIter iterE(h_funcs);
	while ( (iterE()) ){h->GetListOfFunctions()->Add(*iterE);}
	delete h_funcs; 
	// are the functions still valid after list deletion?
	//http://root.cern.ch/root/html/TList#TList:_TList  should be!
	// 	TIter iterTest(EffBCap->GetListOfFunctions());
	// 	while( iterTest() ) std::cout << (*iterTest)->GetName() << std::endl;    
}

MonitoringFile::MDTPostProcessor::MDTPostProcessor(std::string inFilename, std::string name) :
    		TFile( inFilename.c_str(), "UPDATE" ),
    		m_name(name),
    		m_error(0)
{    
	//     if(this->IsOpen()) std::cout << "Begin " << m_name << " MonitoringFile: " << this->GetName() << " Size: " << this->GetSize() << std::endl;
	//     else std::cerr << "ERROR " << m_name << " Could not open MonitoringFile: " << inFilename << std::endl;
	if(!this->IsOpen()) std::cerr << "ERROR " << m_name << " Could not open MonitoringFile: " << inFilename << std::endl;
}

MonitoringFile::MDTPostProcessor::~MDTPostProcessor(){
	//     std::cout << "Destructing " << m_name << " Collected " << m_garbage.size() << " Objects to be deleted" << std::endl;
	if(!m_error) this->Write(); 
	else std::cout << "WARNING " << m_name << " Chose Not To Write " << this->GetName() << std::endl;
	this->Close();
	for(std::set<TObject*>::const_iterator git = m_garbage.begin(); git != m_garbage.end(); ++git){delete *git;}
	//    std::cout << "\n";
}

void MonitoringFile::MDTPostProcessor::error(){ m_error = true; }

template <class T>
void MonitoringFile::MDTPostProcessor::get(const char* namecycle, T* &ptr, TDirectory* baseDir, TDirectory* target){

	if (baseDir) baseDir->GetObject( namecycle, ptr ); // suggested root function
	else this->GetObject( namecycle, ptr ); //suggested root function
	if(!ptr) return;
	TH1* obj = reinterpret_cast<TH1*> (ptr);

	obj->SetDirectory(target);
	if( target == 0 ) m_garbage.insert(obj);
}

void MonitoringFile::MDTPostProcessor::setDirectory( TH1* h, TDirectory* dir ){
	h->SetDirectory(dir);
	if ( dir == 0 ) m_garbage.insert(h);
}

void MonitoringFile::MDTPostProcessor::setDirectory( TH2* h, TDirectory* dir ){
	h->SetDirectory(dir);
	if ( dir == 0 ) m_garbage.insert(h);
}

void MonitoringFile::MDTPostProcessor::setMetaData(TDirectory* targetDir, TH1* h1, TH1* h2, TH1* h3) { 
	//we will collect the garbage and set the directory to zero    
	h1->SetDirectory(0);
	m_garbage.insert(h1);
	if(h2) { h2->SetDirectory(0); m_garbage.insert(h2); }
	if(h3) { h3->SetDirectory(0); m_garbage.insert(h3); }
	//now call old function being sure not to fill a garbage set
	return MonitoringFile::MDTSetMetaData( targetDir, h1, h2, h3);
}

bool MonitoringFile::tdcSort(const TH1* h1, const TH1* h2){
	TString name1 = h1->GetName();
	TString name2 = h2->GetName();

	name1.ReplaceAll("MDTTDC_ADCCut_","");
	name2.ReplaceAll("MDTTDC_ADCCut_","");
	name1.ReplaceAll("BA_","") ;     name1.ReplaceAll("BC_","") ;     name1.ReplaceAll("EA_","") ;     name1.ReplaceAll("EC_","") ;
	name2.ReplaceAll("BA_","") ;     name2.ReplaceAll("BC_","") ;     name2.ReplaceAll("EA_","") ;     name2.ReplaceAll("EC_","") ;
	TString layer1 = name1(0,name1.Index("_"));
	TString layer2 = name2(0,name2.Index("_"));
	if( layer1 == "Extra" && layer2 != "Extra") return false;
	else if (layer2 == "Extra" && layer1 != "Extra") return true;
	if(layer1 < layer2) return true;
	if(layer1 > layer2) return false;

	name1.ReplaceAll(((TString)layer1+"_StPhi"),"") ;
	name2.ReplaceAll(((TString)layer1+"_StPhi"),"") ;
	std::stringstream ss1;
	std::stringstream ss2;
	int phi1; int phi2;

	ss1 << name1;
	ss1 >> phi1;
	ss2 << name2;
	ss2 >> phi2;

	if( phi1 < phi2) return true;
	else return false;
}

}//namespace


