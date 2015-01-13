/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LorentzAngleCalibration_C
#define LorentzAngleCalibration_C

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <string>
#include <TProfile.h>
#include <TVectorT.h>
#include <TF1.h>
#include <TCanvas.h>
#include "PixelCalibAlgs/LorentzAngleCalibration.h"
#include "PixelCalibAlgs/MultiHisto.icc"
#include "PixelCalibAlgs/OfflineCalibUtils.icc"

namespace PixelCalib{

LorentzAngleCalibration::LorentzAngleCalibration(int layer,std::string cosmicORbeam):
	curLayer(layer),curType(cosmicORbeam){

	if(curType != "cosmic") curType = "beam";

	double *etaBins, *phiBins;
	int netaBins, nphiBins;
		
	EtaModule = 0;
	PhiModule = 0;
	
	netaBins = EtaModuleBins(etaBins);
	if(curLayer == 2){
		layername = "Layer2";
		nphiBins = Layer2PhiModuleBins(phiBins);
	}else if(curLayer == 1){
		layername = "Layer1";
		nphiBins = Layer1PhiModuleBins(phiBins);
	}else{
		curLayer = 0;
		layername = "bLayer";
		nphiBins = BlayerPhiModuleBins(phiBins);
	}

	std::vector <std::string> NameDiv;
	std::vector <int> NDiv;
	std::vector <double *> BinsDiv;

	double lowlimit,highlimit;
	if(curType == "cosmic"){
		lowlimit = -40;
		highlimit = 60;
	}else{
		lowlimit = -10;
		highlimit = 30;
	}
	TProfile *Profmodel = new TProfile((layername + "ClusterSizeVsAngle").c_str(),
				(layername + " Cluster Size Vs Angle").c_str(), 50,
				lowlimit, highlimit);
	// second division good for all!!
	NameDiv.push_back("etaModule");
	NDiv.push_back(netaBins);
	BinsDiv.push_back(etaBins);
	if(curType != "cosmic"){
		// first division will change for each layer:
		NameDiv.push_back("phiModule");
		NDiv.push_back(nphiBins);
		BinsDiv.push_back(phiBins);
	}

	LayerProfile = new MultiHisto<TProfile>(*Profmodel,NameDiv,NDiv,BinsDiv);

	int nhistos = LayerProfile->GetNhistos();
	LorentzAngle0 = new TVectorT<double>(nhistos);
	LorentzAngle0Errors = new TVectorT<double>(nhistos);
	LorentzAngle1 = new TVectorT<double>(nhistos);
	LorentzAngle1Errors = new TVectorT<double>(nhistos);
	LorentzAngle2 = new TVectorT<double>(nhistos);
	LorentzAngle2Errors = new TVectorT<double>(nhistos);
	EtaModule = new TVectorT<double>(nhistos);
	if(curType != "cosmic") PhiModule = new TVectorT<double>(nhistos);
	
	for(int i = 0; i < nhistos; i++){
		std::vector<int> indexes = LayerProfile->GetDivisionsIndexes(i);
		if(curType != "cosmic") (*PhiModule)(i) = phiBins[indexes[1]];
		(*EtaModule)(i) = etaBins[indexes[0]];
		(*LorentzAngle0)(i) = 0;
		(*LorentzAngle0Errors)(i) = 0;
		(*LorentzAngle1)(i) = 0;
		(*LorentzAngle1Errors)(i) = 0;
		(*LorentzAngle2)(i) = 0;
		(*LorentzAngle2Errors)(i) = 0;
	}
	
	delete[] etaBins;
	delete[] phiBins;
}

//////////////////////////////////////////////////////////////////////////////////////////

int LorentzAngleCalibration::Read(){

	LorentzAngle0->Read((layername + "LorentzAngle0").c_str());
	LorentzAngle0Errors->Read((layername + "LorentzAngle0Errors").c_str());
	LorentzAngle1->Read((layername + "LorentzAngle1").c_str());
	LorentzAngle1Errors->Read((layername + "LorentzAngle1Errors").c_str());
	LorentzAngle2->Read((layername + "LorentzAngle2").c_str());
	LorentzAngle2Errors->Read((layername + "LorentzAngle2Errors").c_str());
	EtaModule->Read((layername + "EtaModule").c_str());
	if(curType != "cosmic") PhiModule->Read((layername + "PhiModule").c_str());
	
	TDirectory *histodir = (TDirectory *)gDirectory->Get(LayerProfile->GetName());

	return LayerProfile->FillFromFile(histodir);

}

//////////////////////////////////////////////////////////////////////////////////////////

int LorentzAngleCalibration::Write(){

	LayerProfile->Write();
	LorentzAngle0->Write((layername + "LorentzAngle0").c_str());
	LorentzAngle0Errors->Write((layername + "LorentzAngle0Errors").c_str());
	LorentzAngle1->Write((layername + "LorentzAngle1").c_str());
	LorentzAngle1Errors->Write((layername + "LorentzAngle1Errors").c_str());
	LorentzAngle2->Write((layername + "LorentzAngle2").c_str());
	LorentzAngle2Errors->Write((layername + "LorentzAngle2Errors").c_str());
	EtaModule->Write((layername + "EtaModule").c_str());
	if(curType != "cosmic") PhiModule->Write((layername + "PhiModule").c_str());

	return LayerProfile->GetNhistos();

}

//////////////////////////////////////////////////////////////////////////////////////////

bool LorentzAngleCalibration::Fill(Int_t Layer, Int_t EtaIndex, Int_t PhiIndex,
		Double_t ClusterSize, Double_t Angle){
	bool passed = kFALSE;
	if( Layer == curLayer ){
		passed = kTRUE;
		static std::vector<Double_t> Pars(2);
		Pars[1] = PhiIndex;
		Pars[0] = EtaIndex;

		LayerProfile->Fill(Angle,ClusterSize,Pars);
	}
	return passed;
}

//////////////////////////////////////////////////////////////////////////////////////////

int LorentzAngleCalibration::Analyze(ofstream &logfile){
	
	int nfits = 0;
	for(unsigned int i = 0; i < LayerProfile->GetNhistos(); i++){
		TProfile *swap = LayerProfile->GetHisto(i);
		double lowlimit,highlimit;
		if(curType == "cosmic"){
			lowlimit = -5;
			highlimit = 25;
		}else{
			lowlimit = 12;
			highlimit = 24;
		}

		TF1 *fitfunc = new TF1("fitfunc","[0] * (x - [1])^2 + [2]",lowlimit,highlimit);
		fitfunc->SetParameter(0,1);
		fitfunc->SetParLimits(0,1E-5,1E-2);
		fitfunc->SetParameter(1,14);
		fitfunc->SetParLimits(1,10,20);
		fitfunc->SetParameter(2,1);
		fitfunc->SetParLimits(2,1,1.5);
		if(swap->Fit("fitfunc","QR") == 0){
			nfits++;
			(*LorentzAngle0)(i) = fitfunc->GetParameter(0);
			(*LorentzAngle0Errors)(i) = fitfunc->GetParError(0);
			(*LorentzAngle1)(i) = fitfunc->GetParameter(1);
			(*LorentzAngle1Errors)(i) = fitfunc->GetParError(1);
			(*LorentzAngle2)(i) = fitfunc->GetParameter(2);
			(*LorentzAngle2Errors)(i) = fitfunc->GetParError(2);
			logfile << swap->GetTitle() << " --> " << std::endl;
		}else logfile << swap->GetTitle() << " --> Failing fit!" << std::endl;

	}
	return nfits;
}


//////////////////////////////////////////////////////////////////////////////////////////

void LorentzAngleCalibration::PlotValidation(TCanvas *c1, std::string outname){

	int nmodules = LorentzAngle0->GetNrows();
	TH1D *graph0 = new TH1D( (layername + "LorentzAngle0Histo").c_str(),
			(layername + " Width vs Module Index").c_str(),
			nmodules,0,nmodules);
	TH1D *graph1 = new TH1D( (layername + "LorentzAngle1Histo").c_str(),
			(layername + " Lorentz Angle vs Module Index").c_str(),
			nmodules,0,nmodules);
	TH1D *graph2 = new TH1D( (layername + "LorentzAngle2Histo").c_str(),
			(layername + " Vertex y-coordinate vs Module Index").c_str(),
			nmodules,0,nmodules);
	TH1D *graph3 = new TH1D( (layername + "LorentzAngle").c_str(),
			(layername + " Lorentz Angle distribution").c_str(),
			50,0,25);
	for(int i = 0; i < nmodules; i++){
		double par0 = (*LorentzAngle0)[i];
		double error0 = (*LorentzAngle0Errors)[i];
		double angle = (*LorentzAngle1)[i];
		double error = (*LorentzAngle1Errors)[i];
		double par2 = (*LorentzAngle2)[i];
		double error2 = (*LorentzAngle2Errors)[i];
		graph0->SetBinContent(i,par0);
		graph0->SetBinError(i,error0);
		graph1->SetBinContent(i,angle);
		graph1->SetBinError(i,error);
		graph2->SetBinContent(i,par2);
		graph2->SetBinError(i,error2);
		graph3->Fill(angle);
	}
	c1->Clear();	
	c1->Divide(2,2);
	c1->cd(1);
	graph0->SetMaximum(1E-2);
	graph0->SetMinimum(1E-5);
	graph0->DrawCopy();
	c1->cd(2);
	graph1->SetMaximum(20);
	graph1->SetMinimum(10);
	graph1->DrawCopy();
	c1->cd(3);
	graph2->SetMaximum(1.5);
	graph2->SetMinimum(1);
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
		graph1->DrawCopy("same");
		c1->cd(3);
		graph2->DrawCopy("same");
	}
	c1->cd(4);
	graph3->DrawCopy();

	c1->Print(outname.c_str());
}

}
#endif // #ifdef LorentzAngleCalibration_C
