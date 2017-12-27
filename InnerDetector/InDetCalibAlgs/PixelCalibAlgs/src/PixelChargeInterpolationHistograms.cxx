/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelChargeInterpolationHistograms_C
#define PixelChargeInterpolationHistograms_C

#include <iostream>
#include <fstream>
#include <cmath>
#include <sstream>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>

#include <TProfile.h>
#include <TH2.h>
#include <TH1.h>
#include <TF1.h>

#include "PixelCalibAlgs/PixelChargeInterpolationHistograms.h"
#include "PixelConditionsData/PixelChargeInterpolationParameters.h"
#include "PixelCalibAlgs/MultiHisto.icc"
#include "PixelCalibAlgs/AtlasStyle.h"
#include "PixelCalibAlgs/OfflineCalibUtils.icc"

namespace PixelCalib{

PixelChargeInterpolationHistograms::PixelChargeInterpolationHistograms(std::string tag,
		const PixelChargeInterpolationParameters &model):
	m_tag(tag),
	m_parameters(0),
	m_etaProfile(0),
	m_phiProfile(0),
	m_etaH(0),
	m_phiH(0),
	m_OmegaPhih(0),
	m_OmegaEtah(0){

	std::vector<float> *layers = getLayersBins();
	std::vector<float> etabins = model.getEtaBins();
	std::vector<float> phibins = model.getAngleBins();
	std::vector<float> clustersizeEta = model.getClusterSizeYBins();
	std::vector<float> clustersizePhi = model.getClusterSizeXBins();

	unsigned int Neta = etabins.size()-1;
	unsigned int Nphi = phibins.size()-1;
	unsigned int NCSeta = clustersizeEta.size()-1;
	unsigned int NCSphi = clustersizePhi.size()-1;

	unsigned int ntotbins = Nphi + NCSphi + Neta + NCSeta + 4;	
	float *bins = new float[ntotbins];
	unsigned int shift = 0;
	for(unsigned int i = 0; i < clustersizePhi.size() && shift < ntotbins; i++, shift++){
		bins[shift] = clustersizePhi[i];
	}
	for(unsigned int i = 0; i < clustersizeEta.size() && shift < ntotbins; i++, shift++){
		bins[shift] = clustersizeEta[i];
	}
	for(unsigned int i = 0; i < etabins.size() && shift < ntotbins; i++, shift++){
		bins[shift] = etabins[i];
	}
	for(unsigned int i = 0; i < phibins.size() && shift < ntotbins; i++, shift++){
		bins[shift] = phibins[i];
	}

	m_parameters = new PixelChargeInterpolationParameters();
	m_parameters->setParameters(NCSphi, NCSeta, Neta, Nphi, 0, bins);
	m_parameters->setVersion(-1);

	// eta direction
	TProfile *Profmodel = new TProfile(("etaResVsOmega"+ tag).c_str(),
				"#eta residual vs charge sharing", 30, 0., 1.);
	
	TH2F *THmodel = new TH2F(("etaResVsOmegaH"+ tag).c_str(),
				"#eta residual vs charge sharing",
				50, 0., 1.,50,-1000,1000);

	std::vector<std::string> binsnames(3);
	std::vector<std::vector <float> > binsvectors(3);

	binsnames[LayerIndex] = "Layer";
	binsnames[AngleIndex] = "#eta";
	binsnames[ClustersizeIndex] = "ClusterSize";
	
	binsvectors[LayerIndex] = *layers;
	binsvectors[AngleIndex] = etabins;
	binsvectors[ClustersizeIndex] = clustersizeEta;

	m_etaProfile = new MultiHisto<TProfile>(*Profmodel,binsnames,binsvectors);
	m_etaH = new MultiHisto<TH2F>(*THmodel,binsnames,binsvectors);
	m_OmegaEtah_model = new TH1F(("m_OmegaEtah"+ tag).c_str(), "#Omega_{y} distribution",125,0,1);
	m_OmegaEtah = new MultiHisto<TH1F>(*m_OmegaEtah_model,binsnames,binsvectors);

	// phi direction
	Profmodel->SetName(("phiResVsOmega"+ tag).c_str());
	Profmodel->SetTitle("#phi residual vs charge sharing");
	
	TH2F *THmodel1 = new TH2F(("phiResVsOmegaH"+ tag).c_str(),
				"#phi residual vs charge balancing",
				50, 0., 1.,50,-200,200);
	
	binsnames[AngleIndex] = "#phi";

	binsvectors[AngleIndex] = phibins;
	binsvectors[ClustersizeIndex] = clustersizePhi;

	m_phiProfile = new MultiHisto<TProfile>(*Profmodel,binsnames,binsvectors);
	m_phiH = new MultiHisto<TH2F>(*THmodel1,binsnames,binsvectors);

	m_OmegaPhih_model = new TH1F(("m_OmegaPhih"+ tag).c_str(), "#Omega_{x} distribution",125,0,1);
	m_OmegaPhih = new MultiHisto<TH1F>(*m_OmegaPhih_model,binsnames,binsvectors);

	delete Profmodel;
	delete THmodel;
	delete THmodel1;
	delete[] bins;
	delete layers;
	Profmodel = 0;
	THmodel = 0;
	THmodel1 = 0;
	bins = 0;
	
}

//////////////////////////////////////////////////////////////////////////////////////////

PixelChargeInterpolationHistograms::~PixelChargeInterpolationHistograms(){

	delete m_etaProfile;
	delete m_phiProfile;
	delete m_etaH;
	delete m_phiH;
	delete m_parameters;
	m_etaProfile = 0;
	m_phiProfile = 0;
	m_etaH = 0;
	m_phiH = 0;
	m_parameters = 0;

	delete m_OmegaPhih; m_OmegaPhih = 0;
	delete m_OmegaEtah; m_OmegaEtah = 0;
	delete m_OmegaEtah_model; m_OmegaEtah_model = 0;
	delete m_OmegaPhih_model; m_OmegaPhih_model = 0;
}


//////////////////////////////////////////////////////////////////////////////////////////

int PixelChargeInterpolationHistograms::Fill(int DetType, double GeVTrkPt,
		double TrkEta, double DeltaCol, double reseta, double OmegaEta,
		double alpha, double DeltaRow, double resphi, double OmegaPhi){
	
		
	static std::vector<double> Pars(3);
	if(GeVTrkPt == 0) return -1; 

	if( DeltaCol > 1){ // otherwise none to share with!

		Pars[AngleIndex] = TrkEta;
		Pars[LayerIndex] = DetType;
		//std::cout << DetType << std::endl;
		Pars[ClustersizeIndex] = DeltaCol;
		m_OmegaEtah->Fill(OmegaEta,1,Pars);
		m_OmegaEtah_model->Fill(OmegaEta);

		if(OmegaEta > 0.1 && OmegaEta < 0.9){
			m_etaProfile->Fill(OmegaEta,reseta,Pars);
			m_etaH->Fill(OmegaEta,reseta,Pars);
		}
	}

	if( DeltaRow > 1){// otherwise none to share with!

		Pars[AngleIndex] = alpha;
		Pars[LayerIndex] = DetType;
		Pars[ClustersizeIndex] = DeltaRow;

		m_OmegaPhih->Fill(OmegaPhi,1,Pars);
		m_OmegaPhih_model->Fill(OmegaPhi);

		if(OmegaPhi > 0.1 && OmegaPhi < 0.9){
			m_phiProfile->Fill(OmegaPhi,resphi,Pars);
			m_phiH->Fill(OmegaPhi,resphi,Pars);
		}

	}

	

	return 0;

}

//////////////////////////////////////////////////////////////////////////////////////////

int PixelChargeInterpolationHistograms::Read(){

	int readhistos = 0;
	TDirectory *histodir = (TDirectory *)gDirectory->Get(m_etaProfile->GetName());
	readhistos += m_etaProfile->FillFromFile(histodir);
	histodir = (TDirectory *)gDirectory->Get(m_etaH->GetName());
	readhistos += m_etaH->FillFromFile(histodir);
	histodir = (TDirectory *)gDirectory->Get(m_phiProfile->GetName());
	readhistos += m_phiProfile->FillFromFile(histodir);
	histodir = (TDirectory *)gDirectory->Get(m_phiH->GetName());
	readhistos += m_phiH->FillFromFile(histodir);

	return readhistos;
}

//////////////////////////////////////////////////////////////////////////////////////////

int PixelChargeInterpolationHistograms::Write(){
	
	m_etaProfile->Write();
	m_phiProfile->Write();
	m_etaH->Write();
	m_phiH->Write();

	m_OmegaPhih->Write();
	m_OmegaEtah->Write();
	m_OmegaPhih_model->Write();
	m_OmegaEtah_model->Write();

	return m_etaProfile->GetNhistos() + m_phiProfile->GetNhistos()
		+ m_etaH->GetNhistos() + m_phiH->GetNhistos();
}

//////////////////////////////////////////////////////////////////////////////////////////

PixelChargeInterpolationParameters *PixelChargeInterpolationHistograms::Analyze(
		std::ofstream &logfile){

	logfile << "Fitting!" << std::endl;

	SetAtlasStyle();
	TCanvas *c1 = new TCanvas();
	c1->UseCurrentStyle();

	//char *currpath = getcwd(nullptr,0);
        //mkdir(m_etaProfile->GetName(),S_IRWXU | S_IRWXG | S_IRWXO);
	//chdir(m_etaProfile->GetName());

	for(unsigned int i = 0; i < m_etaProfile->GetNhistos(); i++){
		TProfile *swap = m_etaProfile->GetHisto(i);
		swap->UseCurrentStyle();
		double value = 0, error = 0;
		if(Fit(swap, &value, &error) ){
			logfile << swap->GetTitle() << " --> "
				<< value << " +/- " << error << std::endl; 
		}else logfile << swap->GetTitle()
			<< " --> Failing fit!" << std::endl;
		std::vector<int> indexes = m_etaProfile->GetDivisionsIndexes(i);
		m_parameters->setDeltaY(indexes[AngleIndex],
					indexes[ClustersizeIndex],
					indexes[LayerIndex],
					value/1000);
		m_parameters->setErrDeltaY(indexes[AngleIndex],
					indexes[ClustersizeIndex],
					indexes[LayerIndex],
					error/1000);
		if(swap->GetEntries() < 100) continue;
		std::string name = std::string(swap->GetName()) + std::string(".pdf");
		//c1->Print(name.c_str());
		TH2F *swap1 = m_etaH->GetHisto(i);
		swap1->UseCurrentStyle();
		swap1->SetMarkerSize(0.2);
		swap1->GetXaxis()->SetTitle("Charge sharing");
		swap1->GetYaxis()->SetTitle("Cluster center residuals (#mum)");
		swap1->GetYaxis()->SetTitleOffset(1.2);
		swap1->GetXaxis()->SetTitleOffset(1.25);
		swap1->DrawCopy();
		DrawTitleLatex(swap1->GetTitle(), 0.6,0.85);
		std::string name1 = std::string(swap1->GetName()) + std::string(".pdf");
		//c1->Print(name1.c_str());
	}

	//chdir(currpath);
        //mkdir(m_phiProfile->GetName(),S_IRWXU | S_IRWXG | S_IRWXO);
	//chdir(m_phiProfile->GetName());
	for(unsigned int i = 0; i < m_phiProfile->GetNhistos(); i++){
		TProfile *swap = m_phiProfile->GetHisto(i);
		swap->UseCurrentStyle();
		double value = 0, error = 0;
		if(Fit(swap, &value, &error) ){
			logfile << swap->GetTitle() << " --> "
				<< value << " +/- " << error << std::endl; 
		}else logfile << swap->GetTitle()
			<< " --> Failing fit!" << std::endl; 
		std::vector<int> indexes = m_phiProfile->GetDivisionsIndexes(i);
		m_parameters->setDeltaX(indexes[AngleIndex],
					indexes[ClustersizeIndex],
					indexes[LayerIndex],
					value/1000);
		m_parameters->setErrDeltaX(indexes[AngleIndex],
					indexes[ClustersizeIndex],
					indexes[LayerIndex],
					error/1000);
		if(swap->GetEntries() < 100) continue;
		std::string name = std::string(swap->GetName()) + std::string(".pdf");
		//c1->Print(name.c_str());
		TH2 *swap1 = m_phiH->GetHisto(i);
		swap1->UseCurrentStyle();
		swap1->SetMarkerSize(0.2);
		swap1->GetXaxis()->SetTitle("Charge sharing");
		swap1->GetYaxis()->SetTitle("Residuals from center of the cluster (#mum)");
		swap1->GetYaxis()->SetTitleOffset(1.2);
		swap1->GetXaxis()->SetTitleOffset(1.25);
		swap1->DrawCopy();
		DrawTitleLatex(swap1->GetTitle(), 0.6,0.85);
		std::string name1 = std::string(swap1->GetName()) + std::string(".pdf");
		//c1->Print(name1.c_str());
	}

	//chdir(currpath);
	//delete currpath;
	delete c1;
	c1 = 0;
	//currpath = 0;

	return m_parameters;
}

//////////////////////////////////////////////////////////////////////////////////////////

bool PixelChargeInterpolationHistograms::Fit(TProfile *swap, double *value, double *error){

	// perform fits
	if(swap->GetEntries() > 100){
		TF1 *fitfunc = new TF1("fitfunc","[1] * x + [0]",0.15,0.85);
		fitfunc->SetParameter(0,0);
		fitfunc->SetParameter(1,0);
		//fitfunc->SetParLimits(1,lowlim,hilim);
		if(swap->Fit("fitfunc","QR") == 0 && fitfunc->GetProb() > 0.005){
			*value = - fitfunc->GetParameter(1);
			*error = fitfunc->GetParError(1);
			if(fabs(*value) < 2*(*error) || *value < 0){
				*value = 0;
				*error = 0;
			}
			swap->GetXaxis()->SetTitle("Charge sharing");
			swap->GetYaxis()->SetTitle("Residuals from the center of the cluster (#mum)");
			swap->GetYaxis()->SetTitleOffset(1.2);
			swap->GetXaxis()->SetTitleOffset(1.25);
			swap->DrawCopy();
			
			std::ostringstream FitString;
			FitString.flags(std::ios::fixed);
			FitString.precision(2);
			FitString << "slope: " << *value << " #pm " << *error << " #mum";

			DrawATLASLabel(0.2,0.4);
			//DrawTitleLatex(FitString.str().c_str(), 0.2,0.3);
			DrawTitleLatex(swap->GetTitle(), 0.6,0.85);

			delete fitfunc;
			fitfunc = 0;
			return true;
		}else{
			std::cout << swap->GetTitle() << " " << swap->Fit("fitfunc","QR") << " " << fitfunc->GetProb() << std::endl;
			*value = 0;
			*error = 0;
			return false;
		}

	}else return false;
}

}// end of namespace PixelCalib

#endif // #ifdef PixelChargeInterpolationHistograms_C
