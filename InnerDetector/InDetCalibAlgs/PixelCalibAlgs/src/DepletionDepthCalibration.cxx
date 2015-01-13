/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DepletionDepthCalibration_C
#define DepletionDepthCalibration_C

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <TProfile.h>
#include <TVectorT.h>
#include <TF1.h>
#include <TCanvas.h>
#include "PixelCalibAlgs/DepletionDepthCalibration.h"
#include "PixelCalibAlgs/MultiHisto.icc"
#include "PixelCalibAlgs/OfflineCalibUtils.icc"

namespace PixelCalib{

DepletionDepthCalibration::DepletionDepthCalibration(int layer,std::string cosmicORbeam):
	curLayer(layer),curType(cosmicORbeam){
	
  EtaModule = 0;
  PhiModule = 0;

	if(curType != "cosmic") curType = "beam";

	double *etaBins, *phiBins;
	int netaBins, nphiBins;
	
	if(curLayer == 5){
		layername = "Disk2";
		nphiBins = DiskPhiModuleBins(phiBins);
		netaBins = DiskEtaModuleBins(etaBins);
	}else if(curLayer == 4){
		layername = "Disk1";
		nphiBins = DiskPhiModuleBins(phiBins);
		netaBins = DiskEtaModuleBins(etaBins);
	}else if(curLayer == 3){
		layername = "Disk0";
		nphiBins = DiskPhiModuleBins(phiBins);
		netaBins = DiskEtaModuleBins(etaBins);
	}else if(curLayer == 2){
		layername = "Layer2";
		nphiBins = Layer2PhiModuleBins(phiBins);
		netaBins = EtaModuleBins(etaBins);		
	}else if(curLayer == 1){
		layername = "Layer1";
		nphiBins = Layer1PhiModuleBins(phiBins);
		netaBins = EtaModuleBins(etaBins);		
	}else{
		curLayer = 0;
		layername = "bLayer";
		nphiBins = BlayerPhiModuleBins(phiBins);
		netaBins = EtaModuleBins(etaBins);
	}

	std::vector <std::string> NameDiv;
	std::vector <int> NDiv;
	std::vector <double *> BinsDiv;
	TProfile *Profmodel = new TProfile((layername + "ClusterSizeVsTheta").c_str(),
				(layername + " Cluster Size Vs #theta").c_str(), 50, -90, 90);
	// second division good for all!!
	NameDiv.push_back("etaModule");
	NDiv.push_back(netaBins);
	BinsDiv.push_back(etaBins);
	// first division will change for each layer:
	if(curType != "cosmic"){
		NameDiv.push_back("phiModule");
		NDiv.push_back(nphiBins);
		BinsDiv.push_back(phiBins);
	}

	LayerProfile = new MultiHisto<TProfile>(*Profmodel,NameDiv,NDiv,BinsDiv);
	
	int nhistos = LayerProfile->GetNhistos();
	DepletionDepth0 = new TVectorT<double>(nhistos);
	DepletionDepth0Error = new TVectorT<double>(nhistos);
	DepletionDepth1 = new TVectorT<double>(nhistos);
	DepletionDepth1Error = new TVectorT<double>(nhistos);
	DepletionDepth2 = new TVectorT<double>(nhistos);
	DepletionDepth2Error = new TVectorT<double>(nhistos);
	EtaModule = new TVectorT<double>(nhistos);
	if(curType != "cosmic") PhiModule = new TVectorT<double>(nhistos);
	
	for(int i = 0; i < nhistos; i++){
		std::vector<int> indexes = LayerProfile->GetDivisionsIndexes(i);
		if(curType != "cosmic") (*PhiModule)(i) = phiBins[indexes[1]];
		(*EtaModule)(i) = etaBins[indexes[0]];
		(*DepletionDepth0)(i) = 0;
		(*DepletionDepth0Error)(i) = 0;
		(*DepletionDepth1)(i) = 0;
		(*DepletionDepth1Error)(i) = 0;
		(*DepletionDepth2)(i) = 0;
		(*DepletionDepth2Error)(i) = 0;
	}

	delete[] etaBins;
	delete[] phiBins;

}

//////////////////////////////////////////////////////////////////////////////////////////

int DepletionDepthCalibration::Read(){
	
	DepletionDepth0->Read((layername + "DepletionDepth0").c_str());
	DepletionDepth0Error->Read((layername + "DepletionDepth0Errors").c_str());
	DepletionDepth1->Read((layername + "DepletionDepth1").c_str());
	DepletionDepth1Error->Read((layername + "DepletionDepth1Errors").c_str());
	DepletionDepth2->Read((layername + "DepletionDepth2").c_str());
	DepletionDepth2Error->Read((layername + "DepletionDepthErrors2").c_str());
	EtaModule->Read((layername + "EtaModule").c_str());
	if(curType != "cosmic") PhiModule->Read((layername + "PhiModule").c_str());

	TDirectory *histodir = (TDirectory *)gDirectory->Get(LayerProfile->GetName());

	return LayerProfile->FillFromFile(histodir);
}

//////////////////////////////////////////////////////////////////////////////////////////

int DepletionDepthCalibration::Write(){
	
	LayerProfile->Write();
	DepletionDepth0->Write((layername + "DepletionDepth0").c_str());
	DepletionDepth0Error->Write((layername + "DepletionDepth0Errors").c_str());
	DepletionDepth1->Write((layername + "DepletionDepth1").c_str());
	DepletionDepth1Error->Write((layername + "DepletionDepth1Errors").c_str());
	DepletionDepth2->Write((layername + "DepletionDepth2").c_str());
	DepletionDepth2Error->Write((layername + "DepletionDepthErrors2").c_str());
	EtaModule->Write((layername + "EtaModule").c_str());
	if(curType != "cosmic") PhiModule->Write((layername + "PhiModule").c_str());

	return LayerProfile->GetNhistos();

}

//////////////////////////////////////////////////////////////////////////////////////////

bool DepletionDepthCalibration::Fill(Int_t Layer, Int_t EtaIndex, Int_t PhiIndex,
		Double_t ClusterSize, Double_t Eta){

	bool passed = kFALSE;
	double theta = 180.* 2. * atan(exp(- Eta)) / M_PI;
	if(  Layer == curLayer ){
		passed = kTRUE;
		static std::vector<Double_t> Pars(2);
		Pars[1] = PhiIndex;
		Pars[0] = EtaIndex;
			
		if(Layer < 3){
			theta = 90 - theta;
			if(EtaIndex > 0) theta += 1.1;
			else if(EtaIndex < 0) theta -= 1.1;
		}else if(Eta < 0){
			Pars[0] = - 1;
			theta = 180 - theta;
		}

		LayerProfile->Fill(theta,ClusterSize,Pars);
	}
	return passed;
}

//////////////////////////////////////////////////////////////////////////////////////////
int DepletionDepthCalibration::Analyze(ofstream &logfile){

	int nfits = 0;	
	for(unsigned int i = 0; i < LayerProfile->GetNhistos(); i++){
		TProfile *swap = LayerProfile->GetHisto(i);
		double eta = (*EtaModule)(i);
		int sign = int(eta/fabs(eta));
		double lowlimit = -60;
		double highlimit = 60;
		if(curType != "cosmic"){
			if(fabs(eta) > 4){
				lowlimit = sign * 85;
				highlimit = sign * 50;
			}else if(fabs(eta) > 3 ){
				lowlimit = sign * 75;
				highlimit = sign * 30;
			}else if(fabs(eta) > 2 ){
				lowlimit = sign * 70;
				highlimit = sign * 20;
			}
		}
		TF1 *fitfunc = new TF1("fitfunc","[0] * (x - [1])^2 + [2]",lowlimit,highlimit);
		fitfunc->SetParameter(0,2E-4);
		fitfunc->SetParLimits(0,1E-5,5E-4);
		fitfunc->FixParameter(1,0);
		fitfunc->SetParameter(2,1.05);
		fitfunc->SetParLimits(2,1.02,1.08);
		//TF1 *fitfunc = new TF1("fitfunc","[0] / TMath::Abs(cos(TMath::Pi()*x/180)) + [1]",lowlimit,highlimit);
		//fitfunc->SetParameter(0,2E-1);
		//fitfunc->SetParameter(1,1.05);
		if(swap->Fit("fitfunc","QR") == 0){
			nfits++;
			(*DepletionDepth0)(i) = fitfunc->GetParameter(0);
			(*DepletionDepth0Error)(i) = fitfunc->GetParError(0);
			(*DepletionDepth1)(i) = fitfunc->GetParameter(1);
			(*DepletionDepth1Error)(i) = fitfunc->GetParError(1);
			(*DepletionDepth2)(i) = fitfunc->GetParameter(2);
			(*DepletionDepth2Error)(i) = fitfunc->GetParError(2);
			logfile << swap->GetTitle() << " --> " << std::endl;
		}else logfile << swap->GetTitle() << " --> Failing fit!" << std::endl;
	}
	return nfits;	
}

//////////////////////////////////////////////////////////////////////////////////////////

void DepletionDepthCalibration::PlotValidation(TCanvas *c1, std::string outname){

	int nmodules = DepletionDepth0->GetNrows();
	TH1D *graph0 = new TH1D( (layername + "DepletionDepth0Histo").c_str(),
			(layername + " Width vs Module Index").c_str(),
			nmodules,0,nmodules);
	TH1D *graph1 = new TH1D( (layername + "DepletionDepth1Histo").c_str(),
			(layername + " Vertex x-coordinate vs Module Index").c_str(),
			nmodules,0,nmodules);
	TH1D *graph2 = new TH1D( (layername + "DepletionDepth2Histo").c_str(),
			(layername + " Vertex y-coordinate vs Module Index").c_str(),
			nmodules,0,nmodules);

	for(int i = 0; i < nmodules; i++){
		double par0 = (*DepletionDepth0)[i];
		double error0 = (*DepletionDepth0Error)[i];
		double par1 = (*DepletionDepth1)[i];
		double error1 = (*DepletionDepth1Error)[i];
		double par2 = (*DepletionDepth2)[i];
		double error2 = (*DepletionDepth2Error)[i];
		graph0->SetBinContent(i,par0);
		graph0->SetBinError(i,error0);
		graph1->SetBinContent(i,par1);
		graph1->SetBinError(i,error1);
		graph2->SetBinContent(i,par2);
		graph2->SetBinError(i,error2);
	}
	c1->Clear();	
	c1->Divide(1,2);
	c1->cd(1);
	graph0->SetMaximum(5E-4);
	graph0->SetMinimum(1E-5);
	graph0->DrawCopy();
	c1->cd(2);
	graph1->SetMaximum(0.5);
	graph1->SetMinimum(-0.5);
	//graph1->DrawCopy();
	//c1->cd(3);
	graph2->SetMaximum(1.08);
	graph2->SetMinimum(1.02);
	graph2->DrawCopy();
	for (int i = 0; i < nmodules; i++){
		int color = 2;
		if( int( (*EtaModule)[i] ) %2 ) color = 3;
		graph0->SetFillColor(color);
		graph0->SetLineColor(color);
		graph0->SetMarkerStyle(20);
		graph0->SetMarkerColor(1);	
		graph0->SetMarkerSize(0.3);		
		graph0->GetXaxis()->SetRange(i,i+1);
		graph1->SetFillColor(color);
		graph1->SetLineColor(color);
		graph1->SetMarkerStyle(20);
		graph1->SetMarkerSize(0.3);		
		graph1->SetMarkerColor(1);
		graph1->GetXaxis()->SetRange(i,i+1);
		graph2->SetFillColor(color);
		graph2->SetLineColor(color);
		graph2->SetMarkerStyle(20);
		graph2->SetMarkerSize(0.3);		
		graph2->SetMarkerColor(1);
		graph2->GetXaxis()->SetRange(i,i+1);
		c1->cd(1);
		graph0->DrawCopy("same");
		c1->cd(2);
		//graph1->DrawCopy("same");
		//c1->cd(3);
		graph2->DrawCopy("same");
	}
	c1->Print(outname.c_str());
}


}
#endif // #ifdef LorentzOfflineCalib_C
