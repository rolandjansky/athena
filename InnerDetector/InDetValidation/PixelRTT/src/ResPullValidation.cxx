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
	respullname(pullORres),anglename(etaORphi){


	if(cosmicORbeam != "cosmic"){
		datatype = "beam";
	}else{
		datatype = "cosmic";
	}
		
	std::vector <std::string> NameDiv;
	std::vector <int> NDiv;
	std::vector <double *> BinsDiv;

	globaldirname = anglename + respullname + "Validation";
		
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
	if(anglename == "eta"){
		anglename = "eta";
		// nangleBins = 10;
		// angleBins = IntegerBins(nangleBins,-2.5,0.5);
		if (respullname == "res" && datatype != "cosmic") lim = 0.5;
		else lim = 2;
	}else{
		anglename = "phi";
		// nangleBins = 10;
		// angleBins = IntegerBins(nangleBins,-6,2);
		if (respullname == "res" && datatype != "cosmic") lim = 0.2;
		else lim = 2;
	}



	TH1D *Histomodel = new TH1D((anglename + respullname).c_str(), 
			("#" + anglename + " " + respullname).c_str(),
			50,-lim,lim);

	


	Histogram =  new MultiHisto<TH1D>(*Histomodel,NameDiv,NDiv,BinsDiv);
	


	// Pt histograms allocation:
	int nptbins = 50;
	double lowpt = 0.5;
	if(datatype == "cosmic") lowpt = 0.05;
	Double_t *ptbins = LogaritmicBins(nptbins,lowpt,500); 

	RMSProfile *PtProfmodel =  new RMSProfile( (anglename + respullname + "Vspt").c_str(),
			("#" + anglename + " " + respullname + " vs p_{t}").c_str(),
			nptbins,ptbins);

	PtProfile = new MultiHisto<RMSProfile>(*PtProfmodel,NameDiv,NDiv,BinsDiv);

	// Angle histograms allocation:
	double hilim,lolim;
	if(anglename == "phi"){
		if(datatype == "cosmic"){
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
	RMSProfile *AngleProfmodel = new RMSProfile( (respullname + "Vs" + anglename).c_str(),
			(respullname + " vs #" + anglename).c_str(), 50, lolim, hilim);
	
	AngleProfile = new MultiHisto<RMSProfile>(*AngleProfmodel,NameDiv,NDiv,BinsDiv);

	//std::cout << AngleProfmodel->ClassName() << std::endl;
	//std::cout << AngleProfile->GetHisto(2)->ClassName() << std::endl;
	
	delete[] layerBins;
	//delete[] clusterSizeBins;

}

//////////////////////////////////////////////////////////////////////////////////////////

int ResPullValidation::Read(){


	TDirectory *globaldir = (TDirectory *)gDirectory->Get(globaldirname.c_str());

	int readedhistos =0;
	TDirectory *histodir = (TDirectory *)globaldir->Get(AngleProfile->GetName());
	readedhistos += AngleProfile->FillFromFile(histodir);
	histodir = (TDirectory *)globaldir->Get(PtProfile->GetName());
	readedhistos += PtProfile->FillFromFile(histodir);
	histodir = (TDirectory *)globaldir->Get(Histogram->GetName());
	readedhistos += Histogram->FillFromFile(histodir);
	
	return readedhistos;
}

//////////////////////////////////////////////////////////////////////////////////////////

int ResPullValidation::Write(){

	TDirectory *current = gDirectory;
	TDirectory *globaldir = current->mkdir(globaldirname.c_str());
	globaldir->cd();
	int writtenhistos = 0;
	AngleProfile->Write();
	writtenhistos +=  AngleProfile->GetNhistos();
	PtProfile->Write();
	writtenhistos +=  PtProfile->GetNhistos();
	Histogram->Write();
	writtenhistos +=  Histogram->GetNhistos();
	current->cd();

	return writtenhistos;	
}

//////////////////////////////////////////////////////////////////////////////////////////

bool ResPullValidation::Fill(Int_t Layer, Double_t GeVTrkPt, Double_t Angle,
			Double_t ClusterSize, Double_t Residual){
	
	float HighPtRes=0;
	if(datatype == "cosmic") HighPtRes = 1;
	else if(anglename == "phi") HighPtRes=0.05;
	else HighPtRes=0.5;
	if( fabs(Residual) < sqrt((0.5/GeVTrkPt)*(0.5/GeVTrkPt)
				+ HighPtRes*HighPtRes) ){
		static std::vector<Double_t> Pars(3);
		Pars[1] = ClusterSize;
		Pars[0] = Layer;
		AngleProfile->Fill(Angle,Residual,Pars);
		//std::cout << GeVTrkPt << std::endl;
		PtProfile->Fill(GeVTrkPt,Residual,Pars);
		Histogram->Fill(Residual,1,Pars);
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
		reference = new ResPullValidation(anglename,respullname);
		reference->Read();
	}

	char *currpath = getcwd(NULL,0);
        if (mkdir(globaldirname.c_str(),S_IRWXU | S_IRWXG | S_IRWXO)!=0) {
          std::stringstream message;
          message << "Failed to create directory: " << globaldirname;
          throw std::runtime_error(message.str());
        }
	if (chdir(globaldirname.c_str())!=0) {
          std::stringstream message;
          message << "Failed to enter directory: " << globaldirname;
          throw std::runtime_error(message.str());
        }
	
	
	int npthistos =  PtProfile->GetNhistos();
	for(int i = 0 ; i < npthistos ; i++){
		RMSProfile *swap = PtProfile->GetHisto(i);
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

	int nanglehistos = AngleProfile->GetNhistos();
	for(int i = 0 ; i < nanglehistos ; i++){
		RMSProfile *swap = AngleProfile->GetHisto(i);
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
	
	
	int nhistograms =  Histogram->GetNhistos();
	for(int i = 0 ; i < nhistograms ; i++){
		TH1D *swap = Histogram->GetHisto(i);
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
          throw std::runtime_error(message.str());
        }
	delete currpath;
	delete reference;

	old->cd();
	return npthistos + nanglehistos + nhistograms;
}

//////////////////////////////////////////////////////////////////////////////////////////
RMSProfile * ResPullValidation::GetPtProfile(int i){
	return PtProfile->GetHisto(i);

}

RMSProfile * ResPullValidation::GetAngleProfile(int i){
	return AngleProfile->GetHisto(i);

}

TH1D * ResPullValidation::GetHistogram(int i){
	return Histogram->GetHisto(i);

}


}
#endif // #ifdef ResPullValidation_C
