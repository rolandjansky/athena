/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

LorentzAngleCalibration::LorentzAngleCalibration(int layer,const std::string& cosmicORbeam):
	m_curLayer(layer),m_curType(cosmicORbeam){

	if(m_curType != "cosmic") m_curType = "beam";

	double *etaBins, *phiBins;
	int netaBins, nphiBins;
		
	m_EtaModule = 0;
	m_PhiModule = 0;
	
	netaBins = EtaModuleBins(etaBins);
	if(m_curLayer == 2){
		m_layername = "Layer2";
		nphiBins = Layer2PhiModuleBins(phiBins);
	}else if(m_curLayer == 1){
		m_layername = "Layer1";
		nphiBins = Layer1PhiModuleBins(phiBins);
	}else{
		m_curLayer = 0;
		m_layername = "bLayer";
		nphiBins = BlayerPhiModuleBins(phiBins);
	}

	std::vector <std::string> NameDiv;
	std::vector <int> NDiv;
	std::vector <double *> BinsDiv;

	double lowlimit,highlimit;
	if(m_curType == "cosmic"){
		lowlimit = -40;
		highlimit = 60;
	}else{
		lowlimit = -10;
		highlimit = 30;
	}
	TProfile *Profmodel = new TProfile((m_layername + "ClusterSizeVsAngle").c_str(),
				(m_layername + " Cluster Size Vs Angle").c_str(), 50,
				lowlimit, highlimit);
	// second division good for all!!
	NameDiv.push_back("etaModule");
	NDiv.push_back(netaBins);
	BinsDiv.push_back(etaBins);
	if(m_curType != "cosmic"){
		// first division will change for each layer:
		NameDiv.push_back("phiModule");
		NDiv.push_back(nphiBins);
		BinsDiv.push_back(phiBins);
	}

	m_LayerProfile = new MultiHisto<TProfile>(*Profmodel,NameDiv,NDiv,BinsDiv);

	int nhistos = m_LayerProfile->GetNhistos();
	m_LorentzAngle0 = new TVectorT<double>(nhistos);
	m_LorentzAngle0Errors = new TVectorT<double>(nhistos);
	m_LorentzAngle1 = new TVectorT<double>(nhistos);
	m_LorentzAngle1Errors = new TVectorT<double>(nhistos);
	m_LorentzAngle2 = new TVectorT<double>(nhistos);
	m_LorentzAngle2Errors = new TVectorT<double>(nhistos);
	m_EtaModule = new TVectorT<double>(nhistos);
	if(m_curType != "cosmic") m_PhiModule = new TVectorT<double>(nhistos);
	
	for(int i = 0; i < nhistos; i++){
		std::vector<int> indexes = m_LayerProfile->GetDivisionsIndexes(i);
		if(m_curType != "cosmic") (*m_PhiModule)(i) = phiBins[indexes[1]];
		(*m_EtaModule)(i) = etaBins[indexes[0]];
		(*m_LorentzAngle0)(i) = 0;
		(*m_LorentzAngle0Errors)(i) = 0;
		(*m_LorentzAngle1)(i) = 0;
		(*m_LorentzAngle1Errors)(i) = 0;
		(*m_LorentzAngle2)(i) = 0;
		(*m_LorentzAngle2Errors)(i) = 0;
	}
	
	delete[] etaBins;
	delete[] phiBins;
}

//////////////////////////////////////////////////////////////////////////////////////////

int LorentzAngleCalibration::Read(){

	m_LorentzAngle0->Read((m_layername + "m_LorentzAngle0").c_str());
	m_LorentzAngle0Errors->Read((m_layername + "m_LorentzAngle0Errors").c_str());
	m_LorentzAngle1->Read((m_layername + "m_LorentzAngle1").c_str());
	m_LorentzAngle1Errors->Read((m_layername + "m_LorentzAngle1Errors").c_str());
	m_LorentzAngle2->Read((m_layername + "m_LorentzAngle2").c_str());
	m_LorentzAngle2Errors->Read((m_layername + "m_LorentzAngle2Errors").c_str());
	m_EtaModule->Read((m_layername + "m_EtaModule").c_str());
	if(m_curType != "cosmic") m_PhiModule->Read((m_layername + "m_PhiModule").c_str());
	
	TDirectory *histodir = (TDirectory *)gDirectory->Get(m_LayerProfile->GetName());

	return m_LayerProfile->FillFromFile(histodir);

}

//////////////////////////////////////////////////////////////////////////////////////////

int LorentzAngleCalibration::Write(){

	m_LayerProfile->Write();
	m_LorentzAngle0->Write((m_layername + "m_LorentzAngle0").c_str());
	m_LorentzAngle0Errors->Write((m_layername + "m_LorentzAngle0Errors").c_str());
	m_LorentzAngle1->Write((m_layername + "m_LorentzAngle1").c_str());
	m_LorentzAngle1Errors->Write((m_layername + "m_LorentzAngle1Errors").c_str());
	m_LorentzAngle2->Write((m_layername + "m_LorentzAngle2").c_str());
	m_LorentzAngle2Errors->Write((m_layername + "m_LorentzAngle2Errors").c_str());
	m_EtaModule->Write((m_layername + "m_EtaModule").c_str());
	if(m_curType != "cosmic") m_PhiModule->Write((m_layername + "m_PhiModule").c_str());

	return m_LayerProfile->GetNhistos();

}

//////////////////////////////////////////////////////////////////////////////////////////

bool LorentzAngleCalibration::Fill(Int_t Layer, Int_t EtaIndex, Int_t PhiIndex,
		Double_t ClusterSize, Double_t Angle){
	bool passed = kFALSE;
	if( Layer == m_curLayer ){
		passed = kTRUE;
                std::vector<Double_t> Pars(2);
		Pars[1] = PhiIndex;
		Pars[0] = EtaIndex;

		m_LayerProfile->Fill(Angle,ClusterSize,Pars);
	}
	return passed;
}

//////////////////////////////////////////////////////////////////////////////////////////

int LorentzAngleCalibration::Analyze(std::ofstream &logfile){
	
	int nfits = 0;
	for(unsigned int i = 0; i < m_LayerProfile->GetNhistos(); i++){
		TProfile *swap = m_LayerProfile->GetHisto(i);
		double lowlimit,highlimit;
		if(m_curType == "cosmic"){
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
			(*m_LorentzAngle0)(i) = fitfunc->GetParameter(0);
			(*m_LorentzAngle0Errors)(i) = fitfunc->GetParError(0);
			(*m_LorentzAngle1)(i) = fitfunc->GetParameter(1);
			(*m_LorentzAngle1Errors)(i) = fitfunc->GetParError(1);
			(*m_LorentzAngle2)(i) = fitfunc->GetParameter(2);
			(*m_LorentzAngle2Errors)(i) = fitfunc->GetParError(2);
			logfile << swap->GetTitle() << " --> " << std::endl;
		}else logfile << swap->GetTitle() << " --> Failing fit!" << std::endl;

	}
	return nfits;
}


//////////////////////////////////////////////////////////////////////////////////////////

void LorentzAngleCalibration::PlotValidation(TCanvas *c1, const std::string& outname){

	int nmodules = m_LorentzAngle0->GetNrows();
	TH1D *graph0 = new TH1D( (m_layername + "LorentzAngle0Histo").c_str(),
			(m_layername + " Width vs Module Index").c_str(),
			nmodules,0,nmodules);
	TH1D *graph1 = new TH1D( (m_layername + "LorentzAngle1Histo").c_str(),
			(m_layername + " Lorentz Angle vs Module Index").c_str(),
			nmodules,0,nmodules);
	TH1D *graph2 = new TH1D( (m_layername + "LorentzAngle2Histo").c_str(),
			(m_layername + " Vertex y-coordinate vs Module Index").c_str(),
			nmodules,0,nmodules);
	TH1D *graph3 = new TH1D( (m_layername + "LorentzAngle").c_str(),
			(m_layername + " Lorentz Angle distribution").c_str(),
			50,0,25);
	for(int i = 0; i < nmodules; i++){
		double par0 = (*m_LorentzAngle0)[i];
		double error0 = (*m_LorentzAngle0Errors)[i];
		double angle = (*m_LorentzAngle1)[i];
		double error = (*m_LorentzAngle1Errors)[i];
		double par2 = (*m_LorentzAngle2)[i];
		double error2 = (*m_LorentzAngle2Errors)[i];
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
		if( int( (*m_EtaModule)[i] ) %2 ) color = 3;
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
