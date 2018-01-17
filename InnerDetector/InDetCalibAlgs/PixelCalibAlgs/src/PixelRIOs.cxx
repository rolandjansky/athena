/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelRIOs_cxx
#define PixelRIOs_cxx

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#include <TStopwatch.h>
#include <TFile.h>

#include "PixelCalibAlgs/PixelRIOs.h"
#include "PixelCalibAlgs/PixelChargeInterpolationCalibration.h"
#include "PixelCalibAlgs/PixelChargeInterpolationValidation.h"
#include "PixelConditionsData/PixelChargeInterpolationParameters.h"

namespace PixelCalib{


PixelRIOs::PixelRIOs(std::string starting_tag,
						std::string input,
						std::string final_tag,
						std::string collection):
      b_event_number(nullptr),
      b_number_cluster_per_event(nullptr),
      b_type_of_trigger(nullptr),
      b_PixClusLocX(nullptr),
      b_PixClusLocY(nullptr),
      b_PixClusLocXcentroid(nullptr),
      b_PixClusLocYcentroid(nullptr),
      b_PixClusLocX_LorentzCorrection(nullptr),
      b_PixClusGloX(nullptr),
      b_PixClusGloY(nullptr),
      b_PixClusGloZ(nullptr),
      b_PixClusEta(nullptr),
      b_PixClusPhi(nullptr),
      b_PixECBarrel(nullptr),
      b_PixClusLayer(nullptr),
      b_PixEtaModule(nullptr),
      b_PixPhiModule(nullptr),
      b_PixClusGroupsize(nullptr),
      b_PixClusRow(nullptr),
      b_PixClusCol(nullptr),
      b_PixDeltaRow(nullptr),
      b_PixDeltaCol(nullptr),
      b_PixOmegaPhi(nullptr),
      b_PixOmegaEta(nullptr),
      b_PixClusToT(nullptr),
      b_PixClusCharge(nullptr),
      b_PixClusLvl1(nullptr),
      b_PixClusGanged(nullptr),
      b_PixClusFake(nullptr),
      b_PixClusAmbiguous(nullptr),
      b_PixClusEtaIndex(nullptr),
      b_PixClusPhiIndex(nullptr),
      b_PixClusChargeList(nullptr),
      b_PixClusToTList(nullptr),
      b_PixClusLVLAList(nullptr),
      b_number_hits_per_event(nullptr),
      b_PixHitXstartpos(nullptr),
      b_PixHitYstartpos(nullptr),
      b_PixHitZstartpos(nullptr),
      b_PixHitXendpos(nullptr),
      b_PixHitYendpos(nullptr),
      b_PixHitZendpos(nullptr),
      b_PixHitBarrelEndcap(nullptr),
      b_PixHitLayerDisk(nullptr),
      b_PixHitEtaModule(nullptr),
      b_PixHitPhiModule(nullptr),
      b_PixHitEtaIndex(nullptr),
      b_PixHitPhiIndex(nullptr),
      b_PixHitAngle(nullptr),
      b_PixHitAngle2(nullptr),
      b_PixHitAngle_Lorcorr(nullptr),
      b_PixHitEnergyLoss(nullptr),
      b_PixHitTime(nullptr),
      b_PixHitPDGParticle(nullptr),
      b_PixHitParticlePt(nullptr),
      b_PixHitFlag(nullptr),
      b_MC_Xpos(nullptr),
      b_MC_Ypos(nullptr),
      b_MC_IncidentAngle(nullptr),
      b_MC_IncidentAngle2(nullptr),
      b_MC_IncidentAngle_Lorcorr(nullptr),
      b_MC_EnergyLoss(nullptr),
      b_MC_Time(nullptr),
      b_MC_PDGParticle(nullptr),
      b_MC_ParticlePt(nullptr),
      b_MC_Flag(nullptr),
      b_PixClusLocXana(nullptr),
      b_PixClusLocYana(nullptr),
      b_PixClusErrX(nullptr),
      b_PixClusErrY(nullptr),
      b_NotAssociated_Xpos(nullptr),
      b_NotAssociated_Ypos(nullptr),
      b_NotAssociated_BarrelEndcap(nullptr),
      b_NotAssociated_LayerDisk(nullptr),
      b_NotAssociated_EtaModule(nullptr),
      b_NotAssociated_PhiModule(nullptr),
      b_NotAssociated_IncidentAngle(nullptr),
      b_NotAssociated_IncidentAngle2(nullptr),
      b_NotAssociated_IncidentAngle_Lorcorr(nullptr),
      b_NotAssociated_EnergyLoss(nullptr),
      b_NotAssociated_Time(nullptr),
      b_NotAssociated_PDGParticle(nullptr),
      b_NotAssociated_ParticlePt(nullptr),
      b_NotAssociated_Flag(nullptr),
      m_timer(0), 
      m_Calibration(0), 
      m_Validation(0),
      m_StartingTag(starting_tag), 
      m_FinalTag(final_tag)
  {
	m_timer = new TStopwatch;
	m_timer->Start();

	std::string constantsfile = starting_tag;
	if(std::ifstream(constantsfile.c_str()).fail()){
		std::cout << "Failing to open " << constantsfile <<
			" for initialization..." << std::endl;
		abort();
	}

	PixelChargeInterpolationParameters *parametersModel =
						new PixelChargeInterpolationParameters();
	parametersModel->Load(constantsfile);
	
	m_Calibration =
		new PixelChargeInterpolationCalibration(m_FinalTag, *parametersModel);
	m_Validation =
		new PixelChargeInterpolationValidation(m_FinalTag, *parametersModel);

	delete parametersModel;
	parametersModel = 0;

	TChain *clusters_tree = 0;
	if(input != "" ){
		clusters_tree = new TChain(collection.c_str());
		if( input.find(".root") != std::string::npos ){
			if(clusters_tree->AddFile(input.c_str()) == 0){
				std::cout << "Failing to add " << input <<
				" to the chain..." << std::endl;
				abort();
			}
		}else{
                        std::ifstream myfile(input.c_str());
			if (myfile.is_open()){
				for(;;){
					getline(myfile,input);
					if(myfile.eof()) break;
					clusters_tree->AddFile(input.c_str());
				}
				myfile.close();
			}else{
				std::cout << "Failing to open " << input << "..." << std::endl;
				abort();	
			}
		}
	}

	PixClusLocX = 0;
	PixClusLocY = 0;
	PixClusLocXcentroid = 0;
	PixClusLocYcentroid = 0;
	PixClusLocX_LorentzCorrection = 0;
	PixClusGloX = 0;
	PixClusGloY = 0;
	PixClusGloZ = 0;
	PixClusEta = 0;
	PixClusPhi = 0;
	PixECBarrel = 0;
	PixClusLayer = 0;
	PixEtaModule = 0;
	PixPhiModule = 0;
	PixClusGroupsize = 0;
	PixClusRow = 0;
	PixClusCol = 0;
	PixDeltaRow = 0;
	PixDeltaCol = 0;
	PixOmegaPhi = 0;
	PixOmegaEta = 0;
	PixClusToT = 0;
	PixClusCharge = 0;
	PixClusLvl1 = 0;
	PixClusGanged = 0;
	PixClusFake = 0;
	PixClusAmbiguous = 0;
	PixClusEtaIndex = 0;
	PixClusPhiIndex = 0;
	PixClusChargeList = 0;
	PixClusToTList = 0;
	PixClusLVLAList = 0;
	PixHitXstartpos = 0;
	PixHitYstartpos = 0;
	PixHitZstartpos = 0;
	PixHitXendpos = 0;
	PixHitYendpos = 0;
	PixHitZendpos = 0;
	PixHitBarrelEndcap = 0;
	PixHitLayerDisk = 0;
	PixHitEtaModule = 0;
	PixHitPhiModule = 0;
	PixHitEtaIndex = 0;
	PixHitPhiIndex = 0;
	PixHitAngle = 0;
	PixHitAngle2 = 0;
	PixHitAngle_Lorcorr = 0;
	PixHitEnergyLoss = 0;
	PixHitTime = 0;
	PixHitPDGParticle = 0;
	PixHitParticlePt = 0;
	PixHitFlag = 0;
	MC_Xpos = 0;
	MC_Ypos = 0;
	MC_IncidentAngle = 0;
	MC_IncidentAngle2 = 0;
	MC_IncidentAngle_Lorcorr = 0;
	MC_EnergyLoss = 0;
	MC_Time = 0;
	MC_PDGParticle = 0;
	MC_ParticlePt = 0;
	MC_Flag = 0;
	PixClusLocXana = 0;
	PixClusErrY = 0;
	PixClusErrX = 0;
	PixClusLocYana = 0;
	NotAssociated_Xpos = 0;
	NotAssociated_Ypos = 0;
	NotAssociated_BarrelEndcap = 0;
	NotAssociated_LayerDisk = 0;
	NotAssociated_EtaModule = 0;
	NotAssociated_PhiModule = 0;
	NotAssociated_IncidentAngle = 0;
	NotAssociated_IncidentAngle2 = 0;
	NotAssociated_IncidentAngle_Lorcorr = 0;
	NotAssociated_EnergyLoss = 0;
	NotAssociated_Time = 0;
	NotAssociated_PDGParticle = 0;
	NotAssociated_ParticlePt = 0;
	NotAssociated_Flag = 0;

	fChain = clusters_tree;
	fCurrent = -1;

  if (!fChain) { return; }

	fChain->SetBranchAddress("PixelEventNumber", &PixelEventNumber, &b_event_number);
	fChain->SetBranchAddress("PixelClusNum", &PixelClusNum, &b_number_cluster_per_event);
	fChain->SetBranchAddress("LVL1TriggerType", &LVL1TriggerType, &b_type_of_trigger);
	fChain->SetBranchAddress("PixClusLocX", &PixClusLocX, &b_PixClusLocX);
	fChain->SetBranchAddress("PixClusLocY", &PixClusLocY, &b_PixClusLocY);
	fChain->SetBranchAddress("PixClusLocXcentroid", &PixClusLocXcentroid,
		       	&b_PixClusLocXcentroid);
	fChain->SetBranchAddress("PixClusLocYcentroid", &PixClusLocYcentroid,
		       	&b_PixClusLocYcentroid);
	fChain->SetBranchAddress("PixClusLocX_LorentzCorrection",
		       	&PixClusLocX_LorentzCorrection, &b_PixClusLocX_LorentzCorrection);
	fChain->SetBranchAddress("PixClusGloX", &PixClusGloX, &b_PixClusGloX);
	fChain->SetBranchAddress("PixClusGloY", &PixClusGloY, &b_PixClusGloY);
	fChain->SetBranchAddress("PixClusGloZ", &PixClusGloZ, &b_PixClusGloZ);
	fChain->SetBranchAddress("PixClusEta", &PixClusEta, &b_PixClusEta);
	fChain->SetBranchAddress("PixClusPhi", &PixClusPhi, &b_PixClusPhi);
	fChain->SetBranchAddress("PixECBarrel", &PixECBarrel, &b_PixECBarrel);
	fChain->SetBranchAddress("PixClusLayer", &PixClusLayer, &b_PixClusLayer);
	fChain->SetBranchAddress("PixEtaModule", &PixEtaModule, &b_PixEtaModule);
	fChain->SetBranchAddress("PixPhiModule", &PixPhiModule, &b_PixPhiModule);
	fChain->SetBranchAddress("PixClusGroupsize", &PixClusGroupsize, &b_PixClusGroupsize);
	fChain->SetBranchAddress("PixClusRow", &PixClusRow, &b_PixClusRow);
	fChain->SetBranchAddress("PixClusCol", &PixClusCol, &b_PixClusCol);
	fChain->SetBranchAddress("PixDeltaRow", &PixDeltaRow, &b_PixDeltaRow);
	fChain->SetBranchAddress("PixDeltaCol", &PixDeltaCol, &b_PixDeltaCol);
	fChain->SetBranchAddress("PixOmegaPhi", &PixOmegaPhi, &b_PixOmegaPhi);
	fChain->SetBranchAddress("PixOmegaEta", &PixOmegaEta, &b_PixOmegaEta);
	fChain->SetBranchAddress("PixClusToT", &PixClusToT, &b_PixClusToT);
	fChain->SetBranchAddress("PixClusCharge", &PixClusCharge, &b_PixClusCharge);
	fChain->SetBranchAddress("PixClusLvl1", &PixClusLvl1, &b_PixClusLvl1);
	fChain->SetBranchAddress("PixClusGanged", &PixClusGanged, &b_PixClusGanged);
	fChain->SetBranchAddress("PixClusFake", &PixClusFake, &b_PixClusFake);
	fChain->SetBranchAddress("PixClusAmbiguous", &PixClusAmbiguous, &b_PixClusAmbiguous);
	fChain->SetBranchAddress("PixClusEtaIndex", &PixClusEtaIndex, &b_PixClusEtaIndex);
	fChain->SetBranchAddress("PixClusPhiIndex", &PixClusPhiIndex, &b_PixClusPhiIndex);
	fChain->SetBranchAddress("PixClusChargeList", &PixClusChargeList, &b_PixClusChargeList);
	fChain->SetBranchAddress("PixClusToTList", &PixClusToTList, &b_PixClusToTList);
	fChain->SetBranchAddress("PixClusLVLAList", &PixClusLVLAList, &b_PixClusLVLAList);
	fChain->SetBranchAddress("PixelHitsNum", &PixelHitsNum, &b_number_hits_per_event);
	fChain->SetBranchAddress("PixHitXstartpos", &PixHitXstartpos, &b_PixHitXstartpos);
	fChain->SetBranchAddress("PixHitYstartpos", &PixHitYstartpos, &b_PixHitYstartpos);
	fChain->SetBranchAddress("PixHitZstartpos", &PixHitZstartpos, &b_PixHitZstartpos);
	fChain->SetBranchAddress("PixHitXendpos", &PixHitXendpos, &b_PixHitXendpos);
	fChain->SetBranchAddress("PixHitYendpos", &PixHitYendpos, &b_PixHitYendpos);
	fChain->SetBranchAddress("PixHitZendpos", &PixHitZendpos, &b_PixHitZendpos);
	fChain->SetBranchAddress("PixHitBarrelEndcap", &PixHitBarrelEndcap,
		       	&b_PixHitBarrelEndcap);
	fChain->SetBranchAddress("PixHitLayerDisk", &PixHitLayerDisk, &b_PixHitLayerDisk);
	fChain->SetBranchAddress("PixHitEtaModule", &PixHitEtaModule, &b_PixHitEtaModule);
	fChain->SetBranchAddress("PixHitPhiModule", &PixHitPhiModule, &b_PixHitPhiModule);
	fChain->SetBranchAddress("PixHitEtaIndex", &PixHitEtaIndex, &b_PixHitEtaIndex);
	fChain->SetBranchAddress("PixHitPhiIndex", &PixHitPhiIndex, &b_PixHitPhiIndex);
	fChain->SetBranchAddress("PixHitAngle", &PixHitAngle, &b_PixHitAngle);
	fChain->SetBranchAddress("PixHitAngle2", &PixHitAngle2, &b_PixHitAngle2);
	fChain->SetBranchAddress("PixHitAngle_Lorcorr", &PixHitAngle_Lorcorr,
		       	&b_PixHitAngle_Lorcorr);
	fChain->SetBranchAddress("PixHitEnergyLoss", &PixHitEnergyLoss, &b_PixHitEnergyLoss);
	fChain->SetBranchAddress("PixHitTime", &PixHitTime, &b_PixHitTime);
	fChain->SetBranchAddress("PixHitPDGParticle", &PixHitPDGParticle, &b_PixHitPDGParticle);
	fChain->SetBranchAddress("PixHitParticlePt", &PixHitParticlePt, &b_PixHitParticlePt);
	fChain->SetBranchAddress("PixHitFlag", &PixHitFlag, &b_PixHitFlag);
	fChain->SetBranchAddress("MC_Xpos", &MC_Xpos, &b_MC_Xpos);
	fChain->SetBranchAddress("MC_Ypos", &MC_Ypos, &b_MC_Ypos);
	fChain->SetBranchAddress("MC_IncidentAngle", &MC_IncidentAngle, &b_MC_IncidentAngle);
	fChain->SetBranchAddress("MC_IncidentAngle2", &MC_IncidentAngle2, &b_MC_IncidentAngle2);
	fChain->SetBranchAddress("MC_IncidentAngle_Lorcorr", &MC_IncidentAngle_Lorcorr,
		       	&b_MC_IncidentAngle_Lorcorr);
	fChain->SetBranchAddress("MC_EnergyLoss", &MC_EnergyLoss, &b_MC_EnergyLoss);
	fChain->SetBranchAddress("MC_Time", &MC_Time, &b_MC_Time);
	fChain->SetBranchAddress("MC_PDGParticle", &MC_PDGParticle, &b_MC_PDGParticle);
	fChain->SetBranchAddress("MC_ParticlePt", &MC_ParticlePt, &b_MC_ParticlePt);
	fChain->SetBranchAddress("MC_Flag", &MC_Flag, &b_MC_Flag);
	fChain->SetBranchAddress("PixClusLocXana", &PixClusLocXana, &b_PixClusLocXana);
	fChain->SetBranchAddress("PixClusLocYana", &PixClusLocYana, &b_PixClusLocYana);
	fChain->SetBranchAddress("PixClusErrX", &PixClusErrX, &b_PixClusErrX);
	fChain->SetBranchAddress("PixClusErrY", &PixClusErrY, &b_PixClusErrY);
	fChain->SetBranchAddress("NotAssociated_Xpos", &NotAssociated_Xpos,
		       	&b_NotAssociated_Xpos);
	fChain->SetBranchAddress("NotAssociated_Ypos", &NotAssociated_Ypos,
		       	&b_NotAssociated_Ypos);
	fChain->SetBranchAddress("NotAssociated_BarrelEndcap", &NotAssociated_BarrelEndcap,
		       	&b_NotAssociated_BarrelEndcap);
	fChain->SetBranchAddress("NotAssociated_LayerDisk", &NotAssociated_LayerDisk,
		       	&b_NotAssociated_LayerDisk);
	fChain->SetBranchAddress("NotAssociated_EtaModule", &NotAssociated_EtaModule,
		       	&b_NotAssociated_EtaModule);
	fChain->SetBranchAddress("NotAssociated_PhiModule", &NotAssociated_PhiModule,
		       	&b_NotAssociated_PhiModule);
	fChain->SetBranchAddress("NotAssociated_IncidentAngle", &NotAssociated_IncidentAngle,
		       	&b_NotAssociated_IncidentAngle);
	fChain->SetBranchAddress("NotAssociated_IncidentAngle2", &NotAssociated_IncidentAngle2,
		       	&b_NotAssociated_IncidentAngle2);
	fChain->SetBranchAddress("NotAssociated_IncidentAngle_Lorcorr",
		       	&NotAssociated_IncidentAngle_Lorcorr,
		       	&b_NotAssociated_IncidentAngle_Lorcorr);
	fChain->SetBranchAddress("NotAssociated_EnergyLoss", &NotAssociated_EnergyLoss,
		       	&b_NotAssociated_EnergyLoss);
	fChain->SetBranchAddress("NotAssociated_Time", &NotAssociated_Time,
		       	&b_NotAssociated_Time);
	fChain->SetBranchAddress("NotAssociated_PDGParticle", &NotAssociated_PDGParticle,
		       	&b_NotAssociated_PDGParticle);
	fChain->SetBranchAddress("NotAssociated_ParticlePt", &NotAssociated_ParticlePt,
		       	&b_NotAssociated_ParticlePt);
	fChain->SetBranchAddress("NotAssociated_Flag", &NotAssociated_Flag,
		       	&b_NotAssociated_Flag);


	m_timer->Stop();
	std::cout << "========== Time to allocate classes =========" << std::endl;
	std::cout << "Real time: " << m_timer->RealTime() << std::endl;
	std::cout << "CPU time: " <<  m_timer->CpuTime() << std::endl;
	std::cout << "=============================================" << std::endl;

}

/////////////////////////////////////////////////////////////////////////////////////////////


PixelRIOs::~PixelRIOs(){

	delete m_timer;
	delete m_Calibration;
	delete m_Validation;
	
	m_timer = 0;
	m_Calibration = 0;
	m_Validation  = 0;


//	if (!fChain) return;
//	delete fChain->GetCurrentFile();
}

/////////////////////////////////////////////////////////////////////////////////////////////

void PixelRIOs::ReadHistoFile(std::string input){

	m_timer->Start();

	TFile *inputhisto = TFile::Open(input.c_str(),"READ");
	TDirectory *mydir = inputhisto->GetDirectory(m_FinalTag.c_str());
	m_Calibration->Read(mydir);
	m_Validation->Read(mydir);
	inputhisto->Close();	
	delete inputhisto;
	inputhisto = 0;

	m_timer->Stop();
	std::cout << "=========== Time to read from file ==========" << std::endl;
	std::cout << "Real time: " << m_timer->RealTime() << std::endl;
	std::cout << "CPU time: " <<  m_timer->CpuTime() << std::endl;
	std::cout << "=============================================" << std::endl;
	

}

//////////////////////////////////////////////////////////////////////////////////////////


void PixelRIOs::Iterate(long maxentries){

	m_timer->Start();	
	if (fChain == 0) return;
	long nentries = fChain->GetEntriesFast();	
	if(maxentries > 0) nentries = ( nentries < maxentries ) ? nentries : maxentries;
	long jentry = 0, clusters = 0, all_clusters =0, my_clusters = 0;
	int output_entry = int(nentries/10.);
	if(output_entry > 1E5) output_entry = int(1E5);

	for (jentry = 0; jentry < nentries; jentry++){
		Long64_t centry = fChain->LoadTree(jentry);
		if (centry < 0) break;
		TChain *chain = (TChain*)fChain;
		if (chain->GetTreeNumber() != fCurrent) fCurrent = chain->GetTreeNumber();
		if( !(jentry%output_entry) ) std::cout << "Track n. " << jentry << std::endl;
		fChain->GetEntry(jentry);

		/// here clusters are available!!
		all_clusters += PixelClusNum;
		for(int i = 0; i < PixelClusNum; i++){
			if((*MC_Flag)[i] == -100) continue;
			clusters++;
		
		        int DetType = 0;
		        if((*PixECBarrel)[i] == 0) DetType = int( (*PixClusLayer)[i]);	
			else DetType = 3 + int((*PixClusLayer)[i]);	

			double alpha = (*MC_IncidentAngle_Lorcorr)[i] * 180 / M_PI; 
			double TrkEta = (*MC_IncidentAngle2)[i];  
			if(TrkEta > M_PI/2) TrkEta -= M_PI;
			else if(TrkEta < - M_PI/2) TrkEta += M_PI;
			TrkEta = - log( tan( (M_PI/2 - TrkEta)/2 ) );
			double digreseta = ((*PixClusLocYcentroid)[i] - (*MC_Ypos)[i])*1000;
			double digresphi = ((*PixClusLocXcentroid)[i] - (*MC_Xpos)[i])*1000;
			//double digreseta = ((*PixClusLocY)[i] - (*MC_Ypos)[i])*1000;
			//double digresphi = ((*PixClusLocX)[i] - (*MC_Xpos)[i])*1000;
			double reseta = ((*PixClusLocYana)[i] - (*MC_Ypos)[i])*1000;
			double resphi = ((*PixClusLocXana)[i] - (*MC_Xpos)[i])*1000;
			double ERRphi = (*PixClusErrX)[i]*1000; 
			double ERReta = (*PixClusErrY)[i]*1000;
			double DeltaRow = (*PixDeltaRow)[i];
			double DeltaCol = (*PixDeltaCol)[i];
			double OmegaEta = (*PixOmegaEta)[i];
			double OmegaPhi = (*PixOmegaPhi)[i];
			double PullEta = -9999.;
			if (ERReta != 0) PullEta = reseta/ERReta;
			double PullPhi = -9999.;
			if (ERRphi != 0) PullPhi = resphi/ERRphi;
			double charge = (*PixClusCharge)[i];

			double ResCutPhi = 80*3;
			double ResCutEta = 400*3;
			if( DetType > 2 ) continue;
			if( fabs(digresphi) > ResCutPhi) continue;
			if( fabs(digreseta) > ResCutEta) continue;


			if( int((*PixClusCol)[i])%18 == 0 ||
				int((*PixClusCol)[i])%18 == 1 ||
				int((*PixClusCol)[i])%18 == 16 ||
				int((*PixClusCol)[i])%18 == 17) continue;
		
			//if( fabs(TrkEta) < 1) continue;
			//if(fabs(TrkEta) > 0.5 && int((*PixClusGroupsize)[i])%2 == 0) continue;
			//if( fabs(alpha) > 20) continue;

			my_clusters++;
			m_Calibration->Fill(DetType,10,
					alpha, DeltaRow, digresphi, resphi, OmegaPhi,
					TrkEta, DeltaCol, digreseta, reseta, OmegaEta);
			m_Validation->Fill(DetType,10,
					alpha, DeltaRow, digresphi, resphi, ERRphi, PullPhi,
					TrkEta, DeltaCol, digreseta, reseta, ERReta, PullEta, charge);
	
		
		
		}// end of PixClusNum
		
	}// end of entries
	m_timer->Stop();
	std::cout << "========== Time to fill histograms ==========" << std::endl;
	std::cout << "Real time: " << m_timer->RealTime() << std::endl;
	std::cout << "CPU time: " <<  m_timer->CpuTime() << std::endl;
	std::cout << jentry << " entries." << std::endl;
	std::cout << all_clusters << " pixel clusters in total." << std::endl;	
	std::cout << clusters << " pixel clusters with truth associated." << std::endl;	
	std::cout << my_clusters << " pixel clusters after cuts." << std::endl;	
	std::cout << "=============================================" << std::endl;

}

//////////////////////////////////////////////////////////////////////////////////////////

void PixelRIOs::Analyze(std::string reference,std::string reference_tag){

	m_timer->Start();

	//int nfits = 0; 
	//std::string output = "ChargeInterpolation" + m_FinalTag;
	std::vector<std::string> references;
	references.push_back(m_StartingTag);
	m_Calibration->Analyze(m_FinalTag,references);

	TFile *ref_file = 0;
	TDirectory *mydir = 0;
	if(reference != ""){
		ref_file = TFile::Open(reference.c_str(),"READ");
		mydir = ref_file->GetDirectory(reference_tag.c_str());
	}
	m_Validation->Analyze(mydir);
	if(ref_file != 0 && ref_file->IsOpen()) ref_file->Close();
	delete ref_file;
	ref_file = 0;
	mydir = 0;


	std::string errorfilename = "PixelClusterOnTrackErrorData"
	       	+ m_FinalTag.substr(m_FinalTag.find("-"));
	m_Validation->WriteErrorsFile(errorfilename);

	std::string histofilename = m_FinalTag + std::string(".root");
	TFile *histofile = TFile::Open(histofilename.c_str(),"RECREATE");
	if(histofile !=0 ){
		m_Calibration->Write();
		m_Validation->Write();
		histofile->Close();
	}
	delete histofile;	
	histofile = 0;
	

	m_timer->Stop();
	std::cout << "=========== Time to analyze data ===========" << std::endl;
	std::cout << "Real time: " << m_timer->RealTime() << std::endl;
	std::cout << "CPU time: " <<  m_timer->CpuTime() << std::endl;
	//std::cout << nfits << " fits performed." << std::endl;
	std::cout << "=============================================" << std::endl;

}
/////////////////////////////////////////////////////////////////////////////////////////////
}// end of namespace
#endif
