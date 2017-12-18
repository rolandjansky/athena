/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelChargeInterpolationValidation_C
#define PixelChargeInterpolationValidation_C

#include <string>
#include <vector>
#include <sstream>
#include <fstream>

#include <TDirectory.h>
#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>

#include "PixelCalibAlgs/OfflineCalibUtils.icc"
#include "PixelCalibAlgs/PixelChargeInterpolationValidation.h"
#include "PixelCalibAlgs/PixelResidualHistograms.h"
#include "PixelConditionsData/PixelChargeInterpolationParameters.h"
#include "PixelConditionsData/PixelClusterOnTrackErrorData.h"

static const int NLAYERS = 4;
static const int NCLUSSIZES = 10;

namespace PixelCalib{


PixelChargeInterpolationValidation::PixelChargeInterpolationValidation(std::string tag,
					const PixelChargeInterpolationParameters &model):
	m_eta(0), 
	m_phi(0), 
	m_etaDig(0), 
	m_phiDig(0), 
	m_etaPull(0), 
	m_phiPull(0),
	m_etaErr(0), 
	m_phiErr(0),
	m_eta_phi(0), 
	m_phi_eta(0), 
	m_etaDig_phi(0), 
	m_phiDig_eta(0), 
	m_etaPull_phi(0), 
	m_phiPull_eta(0),
	m_etaErr_phi(0), 
	m_phiErr_eta(0), 
	m_charge_eta(0), 
	m_charge_phi(0), 
	m_name(tag){

	std::vector<float> *layers = getLayersBins();
	std::vector<float> etabins = model.getEtaBins();
	std::vector<float> phibins = model.getAngleBins();
	std::vector<float> clustersizeEta = model.getClusterSizeYBins();
	clustersizeEta.insert(clustersizeEta.begin(),0.5);
	clustersizeEta.push_back(100);
	std::vector<float> clustersizePhi = model.getClusterSizeXBins();
	clustersizePhi.insert(clustersizePhi.begin(),0.5);
	clustersizePhi.push_back(100);

	std::vector<float> phibins_Layer;
	for(int i  = -60; i <= 60; i+=2) phibins_Layer.push_back(i);
	std::vector<float> etabins_Layer;
	for(float i  = -2.5; i <= 2.5; i+=0.125) etabins_Layer.push_back(i);
	std::vector<float> ptbins_Layer;
	int ntot = 9;
	double start = 0.1;
	double end = 10E3;
	for(int i  = 0; i < ntot+1; i++){
		ptbins_Layer.push_back( start * pow( end/start, double(i)/double(ntot) ) );
	}



	std::vector<std::string> binsnames(3);
	std::vector<std::vector <float> > binsvectors(3);

	binsnames[LayerIndex] = "Layer";
	binsnames[AngleIndex] = "#eta_{i}";
	binsnames[ClustersizeIndex] = "ClusterSize";
	
	binsvectors[LayerIndex] = *layers;
	binsvectors[AngleIndex] = etabins_Layer;
	binsvectors[ClustersizeIndex] = clustersizeEta;

	m_eta = new PixelResidualHistograms(std::string("ResEta"),
			std::string("Local y residuals - Analog position"),
			500,100,binsvectors,binsnames);

	m_etaDig = new PixelResidualHistograms(std::string("ResEtaDig"),
			std::string("Local y residuals - Center position"),
			500,100,binsvectors,binsnames);
		
	m_etaPull = new PixelResidualHistograms(std::string("PullEta"),
			std::string("Local y pulls - Analog position"),
			10.,100,binsvectors,binsnames);
	
	m_etaErr = new PixelResidualHistograms(std::string("ErrEta"),
			std::string("Local y errors - Cluster uncertainty"),
			1000,250,binsvectors,binsnames);	
			
	m_phi_eta = new PixelResidualHistograms(std::string("ResPhiEta"),
			std::string("Local x residuals - Analog position"),
			150, 100, binsvectors,binsnames);

	m_phiDig_eta = new PixelResidualHistograms(std::string("ResPhiDigEta"),
			std::string("Local x residuals - Center position"),
			150, 100, binsvectors,binsnames);
		
	m_phiPull_eta = new PixelResidualHistograms(std::string("PullPhiEta"),
			std::string("Local x pulls - Analog position"),
			10.,100,binsvectors,binsnames);
	
	m_phiErr_eta = new PixelResidualHistograms(std::string("ErrPhiEta"),
			std::string("Local x errors - Cluster uncertainty"),
			100,100,binsvectors,binsnames);
			
	m_charge_eta	 = new PixelResidualHistograms(std::string("ChargeEta"),
			std::string("Charge"),
		  600000,600,binsvectors,binsnames);
	
	binsnames[AngleIndex] = "#phi_{i} [#circ]";

	binsvectors[AngleIndex] = phibins_Layer;
	binsvectors[ClustersizeIndex] = clustersizePhi;

	m_phi = new PixelResidualHistograms(std::string("ResPhi"),
			std::string("Local x residuals - Analog position"),
			150, 100, binsvectors,binsnames);

	m_phiDig = new PixelResidualHistograms(std::string("ResPhiDig"),
			std::string("Local x residuals - Center position"),
			150, 100, binsvectors,binsnames);
		
	m_phiPull = new PixelResidualHistograms(std::string("PullPhi"),
			std::string("Local x pulls - Analog position"),
			10.,100,binsvectors,binsnames);
	
	m_phiErr = new PixelResidualHistograms(std::string("ErrPhi"),
			std::string("Local x errors - Cluster uncertainty"),
			100,100,binsvectors,binsnames);
			
	m_eta_phi = new PixelResidualHistograms(std::string("ResEtaPhi"),
			std::string("Local y residuals - Analog position"),
			500,100,binsvectors,binsnames);

	m_etaDig_phi = new PixelResidualHistograms(std::string("ResEtaDigPhi"),
			std::string("Local y residuals - Center position"),
			500,100,binsvectors,binsnames);
		
	m_etaPull_phi = new PixelResidualHistograms(std::string("PullEtaPhi"),
			std::string("Local y pulls - Analog position"),
			10.,100,binsvectors,binsnames);
	
	m_etaErr_phi = new PixelResidualHistograms(std::string("ErrEtaPhi"),
			std::string("Local y errors - Cluster uncertainty"),
			1000,250,binsvectors,binsnames);
			
	m_charge_phi = new PixelResidualHistograms(std::string("ChargePhi"),
			std::string("Charge"),
		  600000,600,binsvectors,binsnames);
	
	std::vector<std::string> binsnames_Layer(2);
	std::vector<std::vector <float> > binsvectors_Layer(2);
	
	binsnames_Layer[0] = "p_{T} [GeV]";
	binsnames_Layer[1] = "#eta_{i}";
	
	binsvectors_Layer[0] = ptbins_Layer;
	binsvectors_Layer[1] = etabins_Layer;

	for(int i = 0 ; i < NLAYERS; i++){
		
		std::cout << "layers " << NLAYERS << " " << i << std::endl;
	
		std::ostringstream name;
		std::ostringstream title;		
		name << "ResEta_Layer" << i;
		title << "Local y resolution - Layer " << i ;
		m_eta_Layer.push_back(
				new PixelResidualHistograms(name.str().c_str(),
				 ( title.str() + " - Analog position").c_str(),
				500, 100, binsvectors_Layer, binsnames_Layer)
				);
		m_etaDig_Layer.push_back(
				new PixelResidualHistograms( (name.str() + "Dig").c_str(),
				 ( title.str() + " - Center position").c_str(),
				500, 100, binsvectors_Layer, binsnames_Layer)
				);
		m_etaPull_Layer.push_back(
				new PixelResidualHistograms( (name.str() + "Pull").c_str(),
				( title.str() + " - Pulls").c_str(),
				10., 100, binsvectors_Layer, binsnames_Layer)
				);
	}

	
	binsnames_Layer[1] = "#phi_{i} [#circ]";
	binsvectors_Layer[1] = phibins_Layer;

	for(int i = 0 ; i < NLAYERS; i++){
		std::ostringstream name;
		std::ostringstream title;		
		name << "ResPhi_Layer" << i;
		title << "Local x resolution - Layer " << i ;
		m_phi_Layer.push_back(
				new PixelResidualHistograms(name.str().c_str(),
				 ( title.str() + " - Analog position").c_str(),
				150, 100, binsvectors_Layer, binsnames_Layer)
				);
		m_phiDig_Layer.push_back(
				new PixelResidualHistograms( (name.str() + "Dig").c_str(),
				 ( title.str() + " - Center position").c_str(),
				150, 100, binsvectors_Layer, binsnames_Layer)
				);
		m_phiPull_Layer.push_back(
				new PixelResidualHistograms( (name.str() + "Pull").c_str(),
				( title.str() + " - Pulls").c_str(),
				10., 100, binsvectors_Layer, binsnames_Layer)
				);
	}

	std::vector<std::string> binsnames_Clustersize(1);
	std::vector<std::vector <float> > binsvectors_Clustersize(1);

	binsnames_Clustersize[0] = "#eta_{i}";
	binsvectors_Clustersize[0] = etabins_Layer;

	for(int i = 0 ; i < NCLUSSIZES; i++){
		std::ostringstream name;
		std::ostringstream title;		
		name << "ResEta_Clustersize" << i+1;
		title << "Local y resolution - Clustersize " << i+1;
		m_eta_Clustersize.push_back(
				new PixelResidualHistograms(name.str().c_str(),
				 ( title.str() + " - Analog position").c_str(),
				400, 100, binsvectors_Clustersize, binsnames_Clustersize)
				);
		m_etaDig_Clustersize.push_back(
				new PixelResidualHistograms( (name.str() + "Dig").c_str(),
				 ( title.str() + " - Center position").c_str(),
				400, 100, binsvectors_Clustersize, binsnames_Clustersize)
				);
		m_etaErr_Clustersize.push_back(
				new PixelResidualHistograms( (name.str() + "Err").c_str(),
				 ( title.str() + " - Cluster uncertainty").c_str(),
				1000, 250, binsvectors_Clustersize, binsnames_Clustersize)
				);
		m_etaPull_Clustersize.push_back(
				new PixelResidualHistograms( (name.str() + "Pull").c_str(),
				 ( title.str() + " - Pull").c_str(),
				10., 100., binsvectors_Clustersize, binsnames_Clustersize)
				);
		std::ostringstream namecs;
		std::ostringstream titlecs;		
		namecs << "Eta_Clustersize" << i+1;
		titlecs << "#eta_{i} distribution - Clustersize " << i+1 ;
		m_etaClustersize.push_back(
				new TH1F(namecs.str().c_str(), titlecs.str().c_str(),
					etabins_Layer.size(), etabins_Layer[0],
					etabins_Layer[etabins_Layer.size()-1])
					);
	}


	binsnames_Clustersize[0] = "#phi_{i} [#circ]";
	binsvectors_Clustersize[0] = phibins_Layer;

	for(int i = 0 ; i < NCLUSSIZES; i++){
		std::ostringstream name;
		std::ostringstream title;		
		name << "ResPhi_Clustersize" << i+1;
		title << "Local x resolution - Clustersize " << i+1;
		m_phi_Clustersize.push_back(
				new PixelResidualHistograms(name.str().c_str(),
				 ( title.str() + " - Analog position").c_str(),
				150, 100, binsvectors_Clustersize, binsnames_Clustersize)
				);
		m_phiDig_Clustersize.push_back(
				new PixelResidualHistograms( (name.str() + "Dig").c_str(),
				 ( title.str() + " - Center position").c_str(),
				150, 100, binsvectors_Clustersize, binsnames_Clustersize)
				);
		m_phiErr_Clustersize.push_back(
				new PixelResidualHistograms( (name.str() + "Err").c_str(),
				 ( title.str() + " - Cluster uncertainty").c_str(),
				100, 100, binsvectors_Clustersize, binsnames_Clustersize)
				);
		m_phiPull_Clustersize.push_back(
				new PixelResidualHistograms( (name.str() + "Pull").c_str(),
				 ( title.str() + " - Pull").c_str(),
				10., 100., binsvectors_Clustersize, binsnames_Clustersize)
				);
		std::ostringstream namecs;
		std::ostringstream titlecs;		
		namecs << "Phi_Clustersize" << i+1;
		titlecs << "#phi_{i} distribution - Clustersize " << i+1 ;
		m_phiClustersize.push_back(
				new TH1F(namecs.str().c_str(), titlecs.str().c_str(),
					400, -100,100)
					);
	}

	m_eta_phi_correlation = new TH2F("eta_phi_correlation",
		       	"#phi_{i} #eta_{i} correlation", 100, -100,100, 100, -2.5, 2.5);

	m_eta_phi_correlation->GetYaxis()->SetTitle("#eta_{i}");
	m_eta_phi_correlation->GetXaxis()->SetTitle("#phi_{i}");

	m_CSeta_CSphi_correlation = new TH2F("CSeta_CSphi_correlation",
		       	"#Deltarow #Deltacolumn  correlation", 10, 0,10, 10, 0, 10);

	m_CSeta_CSphi_correlation->GetYaxis()->SetTitle("#eta_{i}");
	m_CSeta_CSphi_correlation->GetXaxis()->SetTitle("#phi_{i}");


	double ptbins_Layerpointer[101];
	int ntot2 = 100;
	double start2 = 0.1;
	double end2 = 10E3;
	for(int i  = 0; i < ntot2+1; i++){
		ptbins_Layerpointer[i] = start2 * pow( end2/start2, double(i)/double(ntot2) );
	}
	m_ResEtaDig_vs_p = new TH2F("ResEtaDig_vs_p",
		       	"Local y residuals vs p_{T}",
			100, ptbins_Layerpointer, 100, -1000, 1000);
	m_ResPhiDig_vs_p = new TH2F("ResPhiDig_vs_p", "Local x residuals vs p_{T}",
		       	100, ptbins_Layerpointer, 100, -400, 400);


	delete layers;

}

//////////////////////////////////////////////////////////////////////////////////////////

PixelChargeInterpolationValidation::~PixelChargeInterpolationValidation(){

	delete m_phi;
	delete m_eta;
	delete m_phiDig;
	delete m_etaDig;
	delete m_phiPull; 
	delete m_etaPull; 
	delete m_phiErr;
	delete m_etaErr;
	delete m_phi_eta;
	delete m_eta_phi;
	delete m_phiDig_eta;
	delete m_etaDig_phi;
	delete m_phiPull_eta; 
	delete m_etaPull_phi; 
	delete m_phiErr_eta;
	delete m_etaErr_phi;
	delete m_charge_eta;
	delete m_charge_phi;

	m_phi = 0;
	m_eta = 0;
	m_phiDig = 0;
	m_etaDig = 0;
	m_phiPull = 0; 
	m_etaPull = 0; 
	m_phiErr = 0; 
	m_etaErr = 0;
	m_phi_eta = 0;
	m_eta_phi = 0;
	m_phiDig_eta = 0;
	m_etaDig_phi = 0;
	m_phiPull_eta = 0; 
	m_etaPull_phi = 0; 
	m_phiErr_eta = 0; 
	m_etaErr_phi = 0;	
	m_charge_eta = 0;
	m_charge_phi = 0;

	for(int i = 0 ; i < NLAYERS; i++){
		delete m_eta_Layer[i];
		delete m_etaDig_Layer[i];
		delete m_etaPull_Layer[i];
		delete m_phi_Layer[i];
		delete m_phiDig_Layer[i];
		delete m_phiPull_Layer[i];
		
		m_eta_Layer[i] = 0;
		m_etaDig_Layer[i] = 0;
		m_etaPull_Layer[i] = 0;
		m_phi_Layer[i] = 0;
		m_phiDig_Layer[i] = 0;
		m_phiPull_Layer[i] = 0;
	}

	for(int i = 0 ; i < NCLUSSIZES; i++){
		delete m_eta_Clustersize[i];
		delete m_etaDig_Clustersize[i];
		delete m_etaPull_Clustersize[i];
		delete m_etaErr_Clustersize[i];
		delete m_phi_Clustersize[i];
		delete m_phiDig_Clustersize[i];
		delete m_phiPull_Clustersize[i];
		delete m_phiErr_Clustersize[i];
		m_eta_Clustersize[i] = 0;
		m_etaDig_Clustersize[i] = 0;
		m_etaPull_Clustersize[i] = 0;
		m_etaErr_Clustersize[i] = 0;
		m_phi_Clustersize[i] = 0;
		m_phiDig_Clustersize[i] = 0;
		m_phiPull_Clustersize[i] = 0;
		m_phiErr_Clustersize[i] = 0;
		delete m_phiClustersize[i]; m_phiClustersize[i] = 0;
		delete m_etaClustersize[i]; m_etaClustersize[i] = 0;
	}

	delete m_eta_phi_correlation; m_eta_phi_correlation = 0;
	delete m_CSeta_CSphi_correlation; m_CSeta_CSphi_correlation = 0;
	delete m_ResEtaDig_vs_p; m_ResEtaDig_vs_p = 0;
	delete m_ResPhiDig_vs_p; m_ResPhiDig_vs_p = 0;


}


//////////////////////////////////////////////////////////////////////////////////////////

int PixelChargeInterpolationValidation::Read(TDirectory *file){


	TDirectory *current = gDirectory;
	if(file !=0) file->cd();
	TDirectory *globaldir = (TDirectory *)gDirectory->Get("Validation");
	globaldir->cd();
	int readhistos = 0;
	readhistos += m_phi->Read();
	readhistos += m_eta->Read();
	readhistos += m_phiDig->Read();
	readhistos += m_etaDig->Read();
	readhistos += m_phiPull->Read(); 
	readhistos += m_etaPull->Read(); 
	readhistos += m_phiErr->Read(); 
	readhistos += m_etaErr->Read();
	readhistos += m_phi_eta->Read();
	readhistos += m_eta_phi->Read();
	readhistos += m_phiDig_eta->Read();
	readhistos += m_etaDig_phi->Read();
	readhistos += m_phiPull_eta->Read(); 
	readhistos += m_etaPull_phi->Read(); 
	readhistos += m_phiErr_eta->Read(); 
	readhistos += m_etaErr_phi->Read();
	readhistos += m_charge_eta->Read();
	readhistos += m_charge_phi->Read();
	for(int i = 0 ; i < NLAYERS; i++){
		m_eta_Layer[i]->Read();
		m_etaDig_Layer[i]->Read();
		m_etaPull_Layer[i]->Read();
		m_phi_Layer[i]->Read();
		m_phiDig_Layer[i]->Read();
		m_phiPull_Layer[i]->Read();
	}
	for(int i = 0 ; i < NCLUSSIZES; i++){
		m_eta_Clustersize[i]->Read();
		m_etaDig_Clustersize[i]->Read();
		m_etaErr_Clustersize[i]->Read();
		m_etaPull_Clustersize[i]->Read();
		m_phi_Clustersize[i]->Read();
		m_phiDig_Clustersize[i]->Read();
		m_phiErr_Clustersize[i]->Read();
		m_phiErr_Clustersize[i]->Read();
		//m_phiClustersize[i]->Read();
		//m_etaClustersize[i]->Read();
	}
	//m_eta_phi_correlation
	//m_CSeta_CSphi_correlation

	current->cd();
	return readhistos;
}

//////////////////////////////////////////////////////////////////////////////////////////

int PixelChargeInterpolationValidation::Write(TDirectory *writedir){

	TDirectory *current = gDirectory;
	TDirectory *globaldir;
	if(writedir != 0) writedir->cd();
	globaldir = gDirectory->mkdir("Validation");
	globaldir->cd();
	int writtenhistos = 0;
	writtenhistos +=  m_phi->Write(true);
	writtenhistos +=  m_eta->Write(true);
	writtenhistos +=  m_phiDig->Write(true);
	writtenhistos +=  m_etaDig->Write(true);
	writtenhistos +=  m_phiPull->Write(true);
	writtenhistos +=  m_etaPull->Write(true);
	writtenhistos +=  m_phiErr->Write(true);
	writtenhistos +=  m_etaErr->Write(true);
	writtenhistos +=  m_phi_eta->Write(true);
	writtenhistos +=  m_eta_phi->Write(true);
	writtenhistos +=  m_phiDig_eta->Write(true);
	writtenhistos +=  m_etaDig_phi->Write(true);
	writtenhistos +=  m_phiPull_eta->Write(true);
	writtenhistos +=  m_etaPull_phi->Write(true);
	writtenhistos +=  m_phiErr_eta->Write(true);
	writtenhistos +=  m_etaErr_phi->Write(true);
	writtenhistos +=  m_charge_eta->Write(true);
	writtenhistos +=  m_charge_phi->Write(true);

	for(int i = 0 ; i < NLAYERS; i++){
		m_eta_Layer[i]->Write(true);
		m_etaDig_Layer[i]->Write(true);
		m_etaPull_Layer[i]->Write(true);
		m_phi_Layer[i]->Write(true);
		m_phiDig_Layer[i]->Write(true);
		m_phiPull_Layer[i]->Write(true);
	}
	for(int i = 0 ; i < NCLUSSIZES; i++){
		m_eta_Clustersize[i]->Write(true);
		m_etaDig_Clustersize[i]->Write(true);
		m_etaErr_Clustersize[i]->Write(true);
		m_etaPull_Clustersize[i]->Write(true);
		m_phi_Clustersize[i]->Write(true);
		m_phiDig_Clustersize[i]->Write(true);
		m_phiErr_Clustersize[i]->Write(true);
		m_phiPull_Clustersize[i]->Write(true);
		m_phiClustersize[i]->Write();
		m_etaClustersize[i]->Write();
	}

	m_eta_phi_correlation->Write();
	m_CSeta_CSphi_correlation->Write();
	m_ResEtaDig_vs_p->Write();
	m_ResPhiDig_vs_p->Write();

	current->cd();
	return writtenhistos;	
}

//////////////////////////////////////////////////////////////////////////////////////////

void PixelChargeInterpolationValidation::WriteErrorsFile(std::string name){

	// Let's write resolutions into text file (see:
	// void PixelClusterOnTrackErrorData::Print(std::string file) const
	PixelClusterOnTrackErrorData *parameters = new PixelClusterOnTrackErrorData();

	TProfile *etaModel = m_eta_Clustersize[0]->GetRMSProfile("#eta_{i}");
	TProfile *phiModel = m_phi_Clustersize[0]->GetRMSProfile("#phi_{i} [#circ]");
	//int ncsx = m_phi_Clustersize.size();
	//int ncsy = m_eta_Clustersize.size(); // number of cluster size (x-direction) bins
	int ncsx = 4;
	int ncsy = 4;
	int neta = etaModel->GetNbinsX(); // number of eta values bins
	int nalpha = phiModel->GetNbinsX(); // number of incidence angle bins
	int ntotbins = ncsx + ncsy + neta + nalpha;
	float *values = new float[ntotbins];
	int globalindex = 0;
	for(int i = 0; i < ncsx && globalindex < ntotbins; i++, globalindex++)
	        values[globalindex] = i + 0.5;
	for(int i = 0; i < ncsy && globalindex < ntotbins; i++, globalindex++)
		values[globalindex] = i + 0.5;
	values[globalindex++] = -2.5;
	for(int i = 1; i < neta && globalindex < ntotbins; i++, globalindex++)
		values[globalindex] = etaModel->GetBinLowEdge(i+1);
	values[globalindex++] = -180;
	for(int i = 1; i < nalpha && globalindex < ntotbins; i++, globalindex++)
		values[globalindex] = phiModel->GetBinLowEdge(i+1);

	parameters->setParameters(ncsx, ncsy, neta, nalpha,0,values);	

	int ntotyconstnx = ncsx * nalpha;
	globalindex = 0;
	for(int i = 0; i < nalpha && globalindex < ntotyconstnx; i++)
		for(int j = 0; j < ncsx && globalindex < ntotyconstnx; j++, globalindex++){
			double value = m_phi_Clustersize[j]->GetRMSProfile("#phi_{i} [#circ]")->GetBinContent(i+1)/1000;
			if (value == 0) value = (j+1)*50/sqrt(12)/1000;
			parameters->setPixelBarrelPhiError(globalindex,value);
		}
	int ntotyconstny = ncsx * ncsy * neta;
	globalindex = 0;
	for(int i = 0; i < neta && globalindex < ntotyconstny; i++)
		for(int j = 0; j < ncsx && globalindex < ntotyconstny; j++)
			for(int k = 0; k < ncsy  && globalindex < ntotyconstny; k++, globalindex++){
				double value = m_eta_Clustersize[k]->GetRMSProfile("#eta_{i}")->GetBinContent(i+1)/1000;
				if (value == 0) value = (k+1)*400/sqrt(12)/1000;
				parameters->setPixelBarrelEtaError(globalindex,value);
			}
	parameters->Print(name.c_str());

        delete parameters;
	delete[] values;
}

//////////////////////////////////////////////////////////////////////////////////////////

void PixelChargeInterpolationValidation::Fill(int Layer, double GeVTrkPt,
		double Phi, double CSphi, double digResPhi, double ResPhi, double ErrPhi, Double_t PullPhi,
		double Eta, double CSeta, double digResEta, double ResEta, double ErrEta, Double_t PullEta, double Charge){

	static std::vector< float >parameters(3);
	parameters[LayerIndex] = Layer;
	parameters[AngleIndex] = Eta;
	parameters[ClustersizeIndex] = CSeta;
	m_eta->Fill(float(ResEta),parameters);
	m_etaDig->Fill(float(digResEta),parameters);
	m_etaPull->Fill(float(PullEta),parameters);
	m_etaErr->Fill(float(ErrEta), parameters);
	m_phi_eta->Fill(float(ResPhi),parameters);
	m_phiDig_eta->Fill(float(digResPhi),parameters);
	m_phiPull_eta->Fill(float(PullPhi),parameters);
	m_phiErr_eta->Fill(float(ErrPhi), parameters);
	m_charge_eta->Fill(float(Charge), parameters);

	parameters[AngleIndex] = Phi;
	parameters[ClustersizeIndex] = CSphi;
	m_phi->Fill(float(ResPhi),parameters);
	m_phiDig->Fill(float(digResPhi),parameters);
	m_phiPull->Fill(float(PullPhi),parameters);
	m_phiErr->Fill(float(ErrPhi), parameters);
	m_eta_phi->Fill(float(ResEta),parameters);
	m_etaDig_phi->Fill(float(digResEta),parameters);
	m_etaPull_phi->Fill(float(PullEta),parameters);
	m_etaErr_phi->Fill(float(ErrEta), parameters);
	m_charge_phi->Fill(float(Charge), parameters);

	if( Layer < NLAYERS){ 
		static std::vector< float >parameters_Layer(2);
		if(GeVTrkPt < 10E10) parameters_Layer[0] = GeVTrkPt;
		else parameters_Layer[0] = 1.;

		parameters_Layer[1] = Eta;
		m_eta_Layer[Layer]->Fill(float(ResEta),parameters_Layer);
		m_etaDig_Layer[Layer]->Fill(float(digResEta),parameters_Layer);
		m_etaPull_Layer[Layer]->Fill(float(PullEta), parameters_Layer);

		parameters_Layer[1] = Phi;
		m_phi_Layer[Layer]->Fill(float(ResPhi),parameters_Layer);
		m_phiDig_Layer[Layer]->Fill(float(digResPhi),parameters_Layer);
		m_phiPull_Layer[Layer]->Fill(float(PullPhi), parameters_Layer);
	}

	static std::vector< float >parameters_Clustersize(1);

	if( int(CSeta)-1 < NCLUSSIZES){ 
		parameters_Clustersize[0] = Eta;
		m_eta_Clustersize[int(CSeta)-1]->Fill(float(ResEta),parameters_Clustersize);
		m_etaDig_Clustersize[int(CSeta)-1]->Fill(float(digResEta),parameters_Clustersize);
		m_etaErr_Clustersize[int(CSeta)-1]->Fill(float(ErrEta),parameters_Clustersize);
		m_etaPull_Clustersize[int(CSeta)-1]->Fill(float(PullEta),parameters_Clustersize);
		m_etaClustersize[int(CSeta)-1]->Fill(Eta);
	}
	if( int(CSphi)-1 < NCLUSSIZES){
		parameters_Clustersize[0] = Phi;
		m_phi_Clustersize[int(CSphi)-1]->Fill(float(ResPhi),parameters_Clustersize);
		m_phiDig_Clustersize[int(CSphi)-1]->Fill(float(digResPhi),parameters_Clustersize);
		//std::cout << ErrPhi << " " << CSphi << " " << Phi << std::endl;
		m_phiErr_Clustersize[int(CSphi)-1]->Fill(float(ErrPhi),parameters_Clustersize);
		m_phiPull_Clustersize[int(CSphi)-1]->Fill(float(PullPhi),parameters_Clustersize);
		m_phiClustersize[int(CSphi)-1]->Fill(Phi);
	}

	m_eta_phi_correlation->Fill(Phi,Eta);
	m_CSeta_CSphi_correlation->Fill(Phi,Eta);
	m_ResEtaDig_vs_p->Fill(GeVTrkPt,digResEta);
	m_ResPhiDig_vs_p->Fill(GeVTrkPt,digResPhi);
	return;

}

//////////////////////////////////////////////////////////////////////////////////////////

void PixelChargeInterpolationValidation::Analyze(TDirectory *ref_file){

	if(ref_file == 0) return;

	/*
	PixelResidualHistograms *eta_reference = 0;
	PixelResidualHistograms *phi_reference = 0;
	PixelResidualHistograms *eta_referencePerLayer = 0;
	PixelResidualHistograms *phi_referencePerLayer = 0;
	PixelResidualHistograms *eta_referencePerAngle = 0;
	PixelResidualHistograms *phi_referencePerAngle = 0;
	PixelResidualProfiles* phi_referenceProfileLayer = 0;
	PixelResidualProfiles* eta_referenceProfileLayer = 0;
	PixelResidualProfiles* phiPt_referenceProfileLayer = 0;
	PixelResidualProfiles* etaPt_referenceProfileLayer = 0;
	PixelResidualHistograms* phi_referencePerPt = 0;
	PixelResidualHistograms* eta_referencePerPt = 0;
	PixelResidualHistograms* phi_referencePerCs = 0;
	PixelResidualHistograms* eta_referencePerCs = 0;
	std::string ref_name = std::string("Reference ");

	if(ref_file != 0 ){
		eta_reference = m_etaVector->Clone();
		eta_reference->Read(ref_file);
		phi_reference = m_phiVector->Clone();
		phi_reference->Read(ref_file);
		eta_referencePerLayer = m_etaPerLayer->Clone();
		eta_referencePerLayer->Read(ref_file);
		phi_referencePerLayer = m_phiPerLayer->Clone();
		phi_referencePerLayer->Read(ref_file);
		eta_referencePerAngle = m_etaPerAngle->Clone();
		eta_referencePerAngle->Read(ref_file);
		phi_referencePerAngle = m_phiPerAngle->Clone();
		phi_referencePerAngle->Read(ref_file);
		phi_referenceProfileLayer = m_phivsAnglePerLayer->Clone();
		phi_referenceProfileLayer->Read(ref_file);
		eta_referenceProfileLayer = m_etavsAnglePerLayer->Clone();
		eta_referenceProfileLayer->Read(ref_file);
		phiPt_referenceProfileLayer = m_phivsPtPerLayer->Clone();
		phiPt_referenceProfileLayer->Read(ref_file);
		etaPt_referenceProfileLayer = m_etavsPtPerLayer->Clone();
		etaPt_referenceProfileLayer->Read(ref_file);
		phi_referencePerPt = m_phiPerPt->Clone();
		phi_referencePerPt->Read(ref_file);
		eta_referencePerPt = m_etaPerPt->Clone();
		eta_referencePerPt->Read(ref_file);
		phi_referencePerCs = m_phiPerCs->Clone();
		phi_referencePerCs->Read(ref_file);
		eta_referencePerCs = m_etaPerCs->Clone();
		eta_referencePerCs->Read(ref_file);
	}else{
		eta_reference = m_etaDigVector;
		phi_reference = m_phiDigVector;
		eta_referencePerLayer = m_etaDigPerLayer;
		phi_referencePerLayer = m_phiDigPerLayer;
		eta_referencePerAngle = m_etaDigPerAngle;
		phi_referencePerAngle = m_phiDigPerAngle;
		phi_referenceProfileLayer = m_phiDigvsAnglePerLayer;
		eta_referenceProfileLayer = m_etaDigvsAnglePerLayer;
		phiPt_referenceProfileLayer = m_phiDigvsPtPerLayer;
		etaPt_referenceProfileLayer = m_etaDigvsPtPerLayer;
		phi_referencePerPt = m_phiDigPerPt;
		eta_referencePerPt = m_phiDigPerPt;
		phi_referencePerCs = m_phiDigPerCs;
		eta_referencePerCs = m_etaDigPerCs;
		ref_name = std::string("Digital ");
	}

	m_etaVector->Analyze(eta_reference, ref_name);
	m_phiVector->Analyze(phi_reference, ref_name);
	m_etaPerLayer->Analyze(eta_referencePerLayer, ref_name);
	m_phiPerLayer->Analyze(phi_referencePerLayer, ref_name);
	m_etaPerAngle->Analyze(eta_referencePerAngle, ref_name);
	m_phiPerAngle->Analyze(phi_referencePerAngle, ref_name);

	//m_phivsAnglePerLayer->Analyze(phi_referenceProfileLayer, ref_name);
	//m_etavsAnglePerLayer->Analyze(eta_referenceProfileLayer, ref_name);

	//m_phivsPtPerLayer->Analyze(phiPt_referenceProfileLayer, ref_name,std::string("log"));
	//m_etavsPtPerLayer->Analyze(etaPt_referenceProfileLayer, ref_name,std::string("log"));
	m_phiPerPt->Analyze(phi_referencePerPt, ref_name);
	m_etaPerPt->Analyze(eta_referencePerPt, ref_name);

	m_phiPerCs->Analyze(phi_referencePerCs, ref_name);
	m_etaPerCs->Analyze(eta_referencePerCs, ref_name);

	if(ref_file != 0 ){ 
		delete eta_reference;
		delete phi_reference;
		delete eta_referencePerLayer;
		delete phi_referencePerLayer;
		delete eta_referencePerAngle;
		delete phi_referencePerAngle;
		delete phi_referenceProfileLayer;
		delete eta_referenceProfileLayer;
		delete phiPt_referenceProfileLayer;
		delete etaPt_referenceProfileLayer;
		delete phi_referencePerPt;
		delete eta_referencePerPt;
		delete phi_referencePerCs;
		delete eta_referencePerCs;
	}

	phi_reference = 0;
	eta_reference = 0;
	phi_referencePerLayer = 0;
	eta_referencePerLayer = 0;
	phi_referencePerAngle = 0;
	eta_referencePerAngle = 0;
	phi_referenceProfileLayer = 0;
	eta_referenceProfileLayer = 0;
	phiPt_referenceProfileLayer = 0;
	etaPt_referenceProfileLayer = 0;
	phi_referencePerPt = 0;
	eta_referencePerPt = 0;
	phi_referencePerCs = 0;
	eta_referencePerCs = 0;
	*/

}

}

#endif // #ifdef PixelChargeInterpolationValidation_C
