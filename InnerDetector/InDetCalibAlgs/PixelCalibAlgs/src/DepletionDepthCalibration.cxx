/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

DepletionDepthCalibration::DepletionDepthCalibration(int layer,const std::string& cosmicORbeam):
	m_curLayer(layer),m_curType(cosmicORbeam){
	
  m_EtaModule = 0;
  m_PhiModule = 0;

	if(m_curType != "cosmic") m_curType = "beam";

	double *etaBins, *phiBins;
	int netaBins, nphiBins;
	
	if(m_curLayer == 5){
		m_layername = "Disk2";
		nphiBins = DiskPhiModuleBins(phiBins);
		netaBins = DiskEtaModuleBins(etaBins);
	}else if(m_curLayer == 4){
		m_layername = "Disk1";
		nphiBins = DiskPhiModuleBins(phiBins);
		netaBins = DiskEtaModuleBins(etaBins);
	}else if(m_curLayer == 3){
		m_layername = "Disk0";
		nphiBins = DiskPhiModuleBins(phiBins);
		netaBins = DiskEtaModuleBins(etaBins);
	}else if(m_curLayer == 2){
		m_layername = "Layer2";
		nphiBins = Layer2PhiModuleBins(phiBins);
		netaBins = EtaModuleBins(etaBins);		
	}else if(m_curLayer == 1){
		m_layername = "Layer1";
		nphiBins = Layer1PhiModuleBins(phiBins);
		netaBins = EtaModuleBins(etaBins);		
	}else{
		m_curLayer = 0;
		m_layername = "bLayer";
		nphiBins = BlayerPhiModuleBins(phiBins);
		netaBins = EtaModuleBins(etaBins);
	}

	std::vector <std::string> NameDiv;
	std::vector <int> NDiv;
	std::vector <double *> BinsDiv;
	TProfile *Profmodel = new TProfile((m_layername + "ClusterSizeVsTheta").c_str(),
				(m_layername + " Cluster Size Vs #theta").c_str(), 50, -90, 90);
	// second division good for all!!
	NameDiv.push_back("etaModule");
	NDiv.push_back(netaBins);
	BinsDiv.push_back(etaBins);
	// first division will change for each layer:
	if(m_curType != "cosmic"){
		NameDiv.push_back("phiModule");
		NDiv.push_back(nphiBins);
		BinsDiv.push_back(phiBins);
	}

	m_LayerProfile = new MultiHisto<TProfile>(*Profmodel,NameDiv,NDiv,BinsDiv);
	
	int nhistos = m_LayerProfile->GetNhistos();
	m_DepletionDepth0 = new TVectorT<double>(nhistos);
	m_DepletionDepth0Error = new TVectorT<double>(nhistos);
	m_DepletionDepth1 = new TVectorT<double>(nhistos);
	m_DepletionDepth1Error = new TVectorT<double>(nhistos);
	m_DepletionDepth2 = new TVectorT<double>(nhistos);
	m_DepletionDepth2Error = new TVectorT<double>(nhistos);
	m_EtaModule = new TVectorT<double>(nhistos);
	if(m_curType != "cosmic") m_PhiModule = new TVectorT<double>(nhistos);
	
	for(int i = 0; i < nhistos; i++){
		std::vector<int> indexes = m_LayerProfile->GetDivisionsIndexes(i);
		if(m_curType != "cosmic") (*m_PhiModule)(i) = phiBins[indexes[1]];
		(*m_EtaModule)(i) = etaBins[indexes[0]];
		(*m_DepletionDepth0)(i) = 0;
		(*m_DepletionDepth0Error)(i) = 0;
		(*m_DepletionDepth1)(i) = 0;
		(*m_DepletionDepth1Error)(i) = 0;
		(*m_DepletionDepth2)(i) = 0;
		(*m_DepletionDepth2Error)(i) = 0;
	}

	delete[] etaBins;
	delete[] phiBins;

}

//////////////////////////////////////////////////////////////////////////////////////////

int DepletionDepthCalibration::Read(){
	
	m_DepletionDepth0->Read((m_layername + "m_DepletionDepth0").c_str());
	m_DepletionDepth0Error->Read((m_layername + "DepletionDepth0Errors").c_str());
	m_DepletionDepth1->Read((m_layername + "m_DepletionDepth1").c_str());
	m_DepletionDepth1Error->Read((m_layername + "DepletionDepth1Errors").c_str());
	m_DepletionDepth2->Read((m_layername + "m_DepletionDepth2").c_str());
	m_DepletionDepth2Error->Read((m_layername + "DepletionDepthErrors2").c_str());
	m_EtaModule->Read((m_layername + "m_EtaModule").c_str());
	if(m_curType != "cosmic") m_PhiModule->Read((m_layername + "m_PhiModule").c_str());

	TDirectory *histodir = (TDirectory *)gDirectory->Get(m_LayerProfile->GetName());

	return m_LayerProfile->FillFromFile(histodir);
}

//////////////////////////////////////////////////////////////////////////////////////////

int DepletionDepthCalibration::Write(){
	
	m_LayerProfile->Write();
	m_DepletionDepth0->Write((m_layername + "m_DepletionDepth0").c_str());
	m_DepletionDepth0Error->Write((m_layername + "DepletionDepth0Errors").c_str());
	m_DepletionDepth1->Write((m_layername + "m_DepletionDepth1").c_str());
	m_DepletionDepth1Error->Write((m_layername + "DepletionDepth1Errors").c_str());
	m_DepletionDepth2->Write((m_layername + "m_DepletionDepth2").c_str());
	m_DepletionDepth2Error->Write((m_layername + "DepletionDepthErrors2").c_str());
	m_EtaModule->Write((m_layername + "m_EtaModule").c_str());
	if(m_curType != "cosmic") m_PhiModule->Write((m_layername + "m_PhiModule").c_str());

	return m_LayerProfile->GetNhistos();

}

//////////////////////////////////////////////////////////////////////////////////////////

bool DepletionDepthCalibration::Fill(Int_t Layer, Int_t EtaIndex, Int_t PhiIndex,
		Double_t ClusterSize, Double_t Eta){

	bool passed = kFALSE;
	double theta = 180.* 2. * atan(exp(- Eta)) / M_PI;
	if(  Layer == m_curLayer ){
		passed = kTRUE;
                std::vector<Double_t> Pars(2);
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

		m_LayerProfile->Fill(theta,ClusterSize,Pars);
	}
	return passed;
}

//////////////////////////////////////////////////////////////////////////////////////////
int DepletionDepthCalibration::Analyze(std::ofstream &logfile){

	int nfits = 0;	
	for(unsigned int i = 0; i < m_LayerProfile->GetNhistos(); i++){
		TProfile *swap = m_LayerProfile->GetHisto(i);
		double eta = (*m_EtaModule)(i);
		int sign = int(eta/fabs(eta));
		double lowlimit = -60;
		double highlimit = 60;
		if(m_curType != "cosmic"){
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
			(*m_DepletionDepth0)(i) = fitfunc->GetParameter(0);
			(*m_DepletionDepth0Error)(i) = fitfunc->GetParError(0);
			(*m_DepletionDepth1)(i) = fitfunc->GetParameter(1);
			(*m_DepletionDepth1Error)(i) = fitfunc->GetParError(1);
			(*m_DepletionDepth2)(i) = fitfunc->GetParameter(2);
			(*m_DepletionDepth2Error)(i) = fitfunc->GetParError(2);
			logfile << swap->GetTitle() << " --> " << std::endl;
		}else logfile << swap->GetTitle() << " --> Failing fit!" << std::endl;
	}
	return nfits;	
}

//////////////////////////////////////////////////////////////////////////////////////////

void DepletionDepthCalibration::PlotValidation(TCanvas *c1, const std::string& outname){

	int nmodules = m_DepletionDepth0->GetNrows();
	TH1D *graph0 = new TH1D( (m_layername + "DepletionDepth0Histo").c_str(),
			(m_layername + " Width vs Module Index").c_str(),
			nmodules,0,nmodules);
	TH1D *graph1 = new TH1D( (m_layername + "DepletionDepth1Histo").c_str(),
			(m_layername + " Vertex x-coordinate vs Module Index").c_str(),
			nmodules,0,nmodules);
	TH1D *graph2 = new TH1D( (m_layername + "DepletionDepth2Histo").c_str(),
			(m_layername + " Vertex y-coordinate vs Module Index").c_str(),
			nmodules,0,nmodules);

	for(int i = 0; i < nmodules; i++){
		double par0 = (*m_DepletionDepth0)[i];
		double error0 = (*m_DepletionDepth0Error)[i];
		double par1 = (*m_DepletionDepth1)[i];
		double error1 = (*m_DepletionDepth1Error)[i];
		double par2 = (*m_DepletionDepth2)[i];
		double error2 = (*m_DepletionDepth2Error)[i];
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
		//graph1->DrawCopy("same");
		//c1->cd(3);
		graph2->DrawCopy("same");
	}
	c1->Print(outname.c_str());
}


}
#endif // #ifdef LorentzOfflineCalib_C
