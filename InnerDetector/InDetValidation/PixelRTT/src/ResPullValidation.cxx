/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ResPullValidation_C
#define ResPullValidation_C

#include <vector>
#include <string>
#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <sys/stat.h>
#include <sys/types.h>
#include <TROOT.h>
#include <TH1.h>
#include <TProfile.h>
#include <TCanvas.h>
#include "PixelRTT/ResPullValidation.h"
#include "PixelRTT/RMSProfile.h"
#include "PixelCalibAlgs/MultiHisto.icc"
#include "PixelCalibAlgs/OfflineCalibUtils.icc"

namespace PixelValid{

ResPullValidation::ResPullValidation(std::string etaORphi, std::string pullORres, std::string cosmicORbeam):
	m_respullname(pullORres),m_anglename(etaORphi){


	if(cosmicORbeam != "cosmic"){
		m_datatype = "beam";
	}else{
		m_datatype = "cosmic";
	}
		
	std::vector <std::string> NameDiv;
	std::vector <int> NDiv;
	std::vector <double *> BinsDiv;

	m_globaldirname = m_anglename + m_respullname + "Validation";
		
	// first division (common to all!):
	int nlayerBins = 6;
	double *layerBins = IntegerBins(nlayerBins);
	NameDiv.push_back("layer");
	NDiv.push_back(nlayerBins);
	BinsDiv.push_back(layerBins);
	
	// second division:
	int nclusterSizeBins = 3;
	double clusterSizeBins[4] = {2,3,4,10};
	NameDiv.push_back("clustersize");
	NDiv.push_back(nclusterSizeBins);
	BinsDiv.push_back(clusterSizeBins);
	
	// these change according to the direction: 
        // AA20150110: Fix compilation warning: nangleBins and angleBins
	// are not used.
 	// int nangleBins;
	// double *angleBins;
	double lim;
	if(m_anglename == "eta"){
		// nangleBins = 10;
		// angleBins = IntegerBins(nangleBins,-2.5,0.5);
		if (m_respullname == "res" && m_datatype != "cosmic") lim = 0.5;
		else lim = 2;
	}else{
		m_anglename = "phi";
		// nangleBins = 10;
		// angleBins = IntegerBins(nangleBins,-6,2);
		if (m_respullname == "res" && m_datatype != "cosmic") lim = 0.2;
		else lim = 2;
	}



	TH1D *Histomodel = new TH1D((m_anglename + m_respullname).c_str(), 
			("#" + m_anglename + " " + m_respullname).c_str(),
			50,-lim,lim);

	


	m_Histogram =  new MultiHisto<TH1D>(*Histomodel,NameDiv,NDiv,BinsDiv);
	


	// Pt histograms allocation:
	int nptbins = 50;
	double lowpt = 0.5;
	if(m_datatype == "cosmic") lowpt = 0.05;
	Double_t *ptbins = LogaritmicBins(nptbins,lowpt,500); 

	RMSProfile *PtProfmodel =  new RMSProfile( (m_anglename + m_respullname + "Vspt").c_str(),
			("#" + m_anglename + " " + m_respullname + " vs p_{t}").c_str(),
			nptbins,ptbins);

	m_PtProfile = new MultiHisto<RMSProfile>(*PtProfmodel,NameDiv,NDiv,BinsDiv);

	// Angle histograms allocation:
	double hilim,lolim;
	if(m_anglename == "phi"){
		if(m_datatype == "cosmic"){
			hilim = 90;
			lolim = -90;
		}else{
			hilim = 13;
			lolim = -7;
		}
	}else{
		hilim = 2.5;
		lolim = - 2.5;
	}
	RMSProfile *AngleProfmodel = new RMSProfile( (m_respullname + "Vs" + m_anglename).c_str(),
			(m_respullname + " vs #" + m_anglename).c_str(), 50, lolim, hilim);
	
	m_AngleProfile = new MultiHisto<RMSProfile>(*AngleProfmodel,NameDiv,NDiv,BinsDiv);

	//std::cout << AngleProfmodel->ClassName() << std::endl;
	//std::cout << m_AngleProfile->GetHisto(2)->ClassName() << std::endl;
	
	delete[] layerBins;
	//delete[] clusterSizeBins;

}

//////////////////////////////////////////////////////////////////////////////////////////

int ResPullValidation::Read(){


	TDirectory *globaldir = (TDirectory *)gDirectory->Get(m_globaldirname.c_str());

	int readedhistos =0;
	TDirectory *histodir = (TDirectory *)globaldir->Get(m_AngleProfile->GetName());
	readedhistos += m_AngleProfile->FillFromFile(histodir);
	histodir = (TDirectory *)globaldir->Get(m_PtProfile->GetName());
	readedhistos += m_PtProfile->FillFromFile(histodir);
	histodir = (TDirectory *)globaldir->Get(m_Histogram->GetName());
	readedhistos += m_Histogram->FillFromFile(histodir);
	
	return readedhistos;
}

//////////////////////////////////////////////////////////////////////////////////////////

int ResPullValidation::Write(){

	TDirectory *current = gDirectory;
	TDirectory *globaldir = current->mkdir(m_globaldirname.c_str());
	globaldir->cd();
	int writtenhistos = 0;
	m_AngleProfile->Write();
	writtenhistos +=  m_AngleProfile->GetNhistos();
	m_PtProfile->Write();
	writtenhistos +=  m_PtProfile->GetNhistos();
	m_Histogram->Write();
	writtenhistos +=  m_Histogram->GetNhistos();
	current->cd();

	return writtenhistos;	
}

//////////////////////////////////////////////////////////////////////////////////////////

bool ResPullValidation::Fill(Int_t Layer, Double_t GeVTrkPt, Double_t Angle,
			Double_t ClusterSize, Double_t Residual){
	
	float HighPtRes=0;
	if(m_datatype == "cosmic") HighPtRes = 1;
	else if(m_anglename == "phi") HighPtRes=0.05;
	else HighPtRes=0.5;
	if( fabs(Residual) < sqrt((0.5/GeVTrkPt)*(0.5/GeVTrkPt)
				+ HighPtRes*HighPtRes) ){
		static std::vector<Double_t> Pars(3);
		Pars[1] = ClusterSize;
		Pars[0] = Layer;
		m_AngleProfile->Fill(Angle,Residual,Pars);
		//std::cout << GeVTrkPt << std::endl;
		m_PtProfile->Fill(GeVTrkPt,Residual,Pars);
		m_Histogram->Fill(Residual,1,Pars);
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////

int ResPullValidation::Analyze(TDirectory *ref_file){

	TDirectory *old = gDirectory;

	ResPullValidation *reference = 0;
	if(ref_file != 0 ){
		ref_file->cd();
		reference = new ResPullValidation(m_anglename,m_respullname);
		reference->Read();
	}

	char *currpath = getcwd(NULL,0);
        if (mkdir(m_globaldirname.c_str(),S_IRWXU | S_IRWXG | S_IRWXO)!=0) {
          std::stringstream message;
          message << "Failed to create directory: " << m_globaldirname;
          throw std::runtime_error(message.str());
        }
	if (chdir(m_globaldirname.c_str())!=0) {
          std::stringstream message;
          message << "Failed to enter directory: " << m_globaldirname;
          throw std::runtime_error(message.str());
        }
	
	
	int npthistos =  m_PtProfile->GetNhistos();
	for(int i = 0 ; i < npthistos ; i++){
		RMSProfile *swap = m_PtProfile->GetHisto(i);
		if(swap->GetEntries() < 100) continue;
		TCanvas *c1 = new TCanvas();
		c1->SetLogx();
		swap->SetLineColor(4);
		swap->SetMarkerColor(4);
		swap->SetMarkerStyle(27);
		swap->Draw("P");
		if(reference != 0 ){
			RMSProfile *swap1 = reference->GetPtProfile(i);
			swap1->SetLineColor(2);
			swap1->SetMarkerColor(2);
			swap1->SetMarkerStyle(22);
			swap1->Draw("same P");
		}
		std::string name = std::string(swap->GetName()) + std::string(".pdf");
		c1->Print(name.c_str());
		delete c1;
	}

	int nanglehistos = m_AngleProfile->GetNhistos();
	for(int i = 0 ; i < nanglehistos ; i++){
		RMSProfile *swap = m_AngleProfile->GetHisto(i);
		if(swap->GetEntries() < 100) continue;
		TCanvas *c1 = new TCanvas();
		swap->SetLineColor(4);
		swap->SetMarkerColor(4);
		swap->SetMarkerStyle(27);
		swap->Draw("P");
		if(reference != 0 ){
			RMSProfile *swap1 = reference->GetAngleProfile(i);
			swap1->SetLineColor(2);
			swap1->SetMarkerColor(2);
			swap1->SetMarkerStyle(22);
			swap1->Draw("same P");
		}
		std::string name = std::string(swap->GetName()) + std::string(".pdf");
		c1->Print(name.c_str());
		delete c1;
	}
	
	
	int nhistograms =  m_Histogram->GetNhistos();
	for(int i = 0 ; i < nhistograms ; i++){
		TH1D *swap = m_Histogram->GetHisto(i);
		if(swap->GetEntries() < 100) continue;
		TCanvas *c1 = new TCanvas();
		swap->SetLineColor(4);
		swap->SetMarkerColor(4);
		swap->SetMarkerStyle(27);
		swap->Draw("P");
		if(reference != 0 ){
			TH1D *swap1 = reference->GetHistogram(i);
			swap1->SetLineColor(2);
			swap1->SetMarkerColor(2);
			swap1->SetMarkerStyle(22);
			swap1->Draw("same P");
		}
		std::string name = std::string(swap->GetName()) + std::string(".pdf");
		c1->Print(name.c_str());
		delete c1;
	}

	if(currpath==NULL) {
          std::stringstream message;
          message << "Invalid current directory! ";
          throw std::runtime_error(message.str());
        }
        else if( chdir(currpath)!=0) {
          std::stringstream message;
          message << "Failed to enter current directory!";
          throw std::runtime_error(message.str());;
        }
        delete currpath;
	delete reference;

	old->cd();
	return npthistos + nanglehistos + nhistograms;
}

//////////////////////////////////////////////////////////////////////////////////////////
RMSProfile * ResPullValidation::GetPtProfile(int i){
	return m_PtProfile->GetHisto(i);

}

RMSProfile * ResPullValidation::GetAngleProfile(int i){
	return m_AngleProfile->GetHisto(i);

}

TH1D * ResPullValidation::GetHistogram(int i){
	return m_Histogram->GetHisto(i);

}


}
#endif // #ifdef ResPullValidation_C
