/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CosmicTrackValidation_C
#define CosmicTrackValidation_C

#include <vector>
#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <sys/stat.h>
#include <sys/types.h>
#include <string>
#include <TROOT.h>
#include "PixelCalibAlgs/MultiHisto.h"
#include <TH1.h>
#include <TProfile.h>
#include <TCanvas.h>
#include "PixelRTT/TrackValidation.h"
#include "PixelRTT/RMSProfile.h"
#include "PixelCalibAlgs/MultiHisto.icc"
#include "PixelCalibAlgs/OfflineCalibUtils.icc"

namespace PixelValid{

CosmicTrackValidation::CosmicTrackValidation(){

	globaldirname = "TrackValidation";
	std::vector <std::string> NameDiv;
	std::vector <int> NDiv;
	std::vector <double *> BinsDiv;
	
	// first division (common to all!):
	int nlayerBins = 6;
	double *layerBins = IntegerBins(nlayerBins);
	NameDiv.push_back("layer");
	NDiv.push_back(nlayerBins);
	BinsDiv.push_back(layerBins);
	
	// th1 distribution:
	double ptlim = 100;
	double 	anglelim = 90;
	double  etalim = 2.5;

	TH1D *ptHistomodel = new TH1D("ptditribution","Pt distribution",50,-ptlim,ptlim);
	PtHistogram =  new MultiHisto<TH1D>(*ptHistomodel,NameDiv,NDiv,BinsDiv);
	
	
	TH1D *angleHistomodel = new TH1D("angleditribution","Angle distribution",50,-anglelim,anglelim);
	AngleHistogram =  new MultiHisto<TH1D>(*angleHistomodel,NameDiv,NDiv,BinsDiv);
	
	
	TH1D *etaHistomodel = new TH1D("etaditribution","Eta distribution",50,-etalim,etalim);
	EtaHistogram =  new MultiHisto<TH1D>(*etaHistomodel,NameDiv,NDiv,BinsDiv);
	
	
	// second division:
	int nclusterSizeBins = 4;
	double *clusterSizeBins = IntegerBins(nclusterSizeBins,1);
	//NameDiv.push_back("clustersize");
	//NDiv.push_back(nclusterSizeBins);
	//BinsDiv.push_back(clusterSizeBins);

	// Pt histograms allocation:
	int nptbins = 50;
	Double_t *ptbins = LogaritmicBins(nptbins,0.05,500); 

	TProfile *PtProfmodel =  new TProfile("nhitsVspt","nhits vs p_{t}",nptbins,ptbins);
	PtProfile = new MultiHisto<TProfile>(*PtProfmodel,NameDiv,NDiv,BinsDiv);

	// Angle histograms allocation:
	TProfile *AngleProfmodel = new TProfile("nhitsVsAngle","nhits vs angle", 50, anglelim, -anglelim);
	AngleProfile = new MultiHisto<TProfile>(*AngleProfmodel,NameDiv,NDiv,BinsDiv);

	// Eta histograms allocation:
	TProfile *EtaProfmodel = new TProfile("nhitsVsEta","nhits vs eta", 50, etalim, -etalim);
	EtaProfile = new MultiHisto<TProfile>(*EtaProfmodel,NameDiv,NDiv,BinsDiv);
	//std::cout << AngleProfmodel->ClassName() << std::endl;
	//std::cout << AngleProfile->GetHisto(2)->ClassName() << std::endl;
	
	delete[] layerBins;
	delete[] clusterSizeBins;

}

//////////////////////////////////////////////////////////////////////////////////////////

int CosmicTrackValidation::Read(){


	TDirectory *globaldir = (TDirectory *)gDirectory->Get(globaldirname.c_str());

	int readedhistos =0;
	TDirectory *histodir = (TDirectory *)globaldir->Get(AngleProfile->GetName());
	readedhistos += AngleProfile->FillFromFile(histodir);
	histodir = (TDirectory *)globaldir->Get(EtaProfile->GetName());
	readedhistos += EtaProfile->FillFromFile(histodir);
	histodir = (TDirectory *)globaldir->Get(PtProfile->GetName());
	readedhistos += PtProfile->FillFromFile(histodir);
	histodir = (TDirectory *)globaldir->Get(AngleHistogram->GetName());
	readedhistos += AngleHistogram->FillFromFile(histodir);
	histodir = (TDirectory *)globaldir->Get(EtaHistogram->GetName());
	readedhistos += EtaHistogram->FillFromFile(histodir);
	histodir = (TDirectory *)globaldir->Get(PtHistogram->GetName());
	readedhistos += PtHistogram->FillFromFile(histodir);
	
	return readedhistos;
}

//////////////////////////////////////////////////////////////////////////////////////////

int CosmicTrackValidation::Write(){

	TDirectory *current = gDirectory;
	TDirectory *globaldir = current->mkdir(globaldirname.c_str());
	globaldir->cd();
	int writtenhistos = 0;
	AngleProfile->Write();
	writtenhistos +=  AngleProfile->GetNhistos();
	EtaProfile->Write();
	writtenhistos +=  EtaProfile->GetNhistos();
	PtProfile->Write();
	writtenhistos +=  PtProfile->GetNhistos();
	AngleHistogram->Write();
	writtenhistos +=  AngleHistogram->GetNhistos();
	EtaHistogram->Write();
	writtenhistos +=  EtaHistogram->GetNhistos();
	PtHistogram->Write();
	writtenhistos +=  PtHistogram->GetNhistos();
	current->cd();

	return writtenhistos;	
}

//////////////////////////////////////////////////////////////////////////////////////////

bool CosmicTrackValidation::Fill(Int_t Layer, Double_t GeVTrkPt, Double_t Angle,
			Double_t Eta, Int_t nhits){
	bool passed = kFALSE;
	//float HighPtRes=0;
	//if(anglename == "phi") HighPtRes=0.01;
	//else HighPtRes=0.1;
	//if( fabs(Residual) < sqrt((0.5/GeVTrkPt)*(0.5/GeVTrkPt) 
        //    + 5*HighPtRes*5*HighPtRes) ){
		passed = kTRUE;
		static std::vector<Double_t> Pars(3);
		//Pars[1] = ClusterSize;
		Pars[0] = Layer;
		AngleProfile->Fill(Angle,nhits,Pars);
		EtaProfile->Fill(Eta,nhits,Pars);
		//std::cout << GeVTrkPt << std::endl;
		PtProfile->Fill(GeVTrkPt,nhits,Pars);
		AngleHistogram->Fill(Angle,1,Pars);
		EtaHistogram->Fill(Eta,1,Pars);
		PtHistogram->Fill(GeVTrkPt,1,Pars);
	//}
	return passed;
}

//////////////////////////////////////////////////////////////////////////////////////////

int CosmicTrackValidation::Analyze(TDirectory *ref_file){

	TDirectory *old = gDirectory;
	TCanvas **c1;	
	int nplots = PlotAll(&c1,0,"*h",1);

	CosmicTrackValidation *reference = 0;
	if(ref_file != 0 ){
		ref_file->cd();
		reference = new CosmicTrackValidation();
		reference->Read();
		reference->PlotAll(&c1,nplots,"E1 same",2);
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
	// AA20150108
        // Clean up temporay canvases and reference object.
	// (Fix for coverity errors 16787 and 16788 
	for(int i = 0 ; i < nplots ; i++) {
	  c1[i]->Print(".pdf");
	  delete c1[i];
	}
	delete[] c1;
        delete reference;

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

	old->cd();
	return nplots;
}

//////////////////////////////////////////////////////////////////////////////////////////

int CosmicTrackValidation::PlotAll(TCanvas ***c1, int ncanvas, Option_t* option,Int_t color){
	
	int npthistos =  PtProfile->GetNhistos();
	int nanglehistos =  AngleProfile->GetNhistos();
	int netahistos =  EtaProfile->GetNhistos();
	int nanglehistograms =  AngleHistogram->GetNhistos();
	int netahistograms =  EtaHistogram->GetNhistos();
	int npthistograms =  PtHistogram->GetNhistos();
	int nhistos =  npthistos + nanglehistos + netahistos + nanglehistograms + netahistograms + npthistograms;

	if(ncanvas == 0 || *c1 == 0){
		*c1 = new TCanvas*[nhistos];
		ncanvas = nhistos;
		for(int i = 0 ; i < ncanvas ; i++) (*c1)[i] = new TCanvas();
	}else if(ncanvas != nhistos){
		std::cout << "Number of canvases is not compatible with" <<
			globaldirname << "!" << std::endl;
		return 0;
	}

	int canvas_index = -1;
	for(int i =0 ; i < npthistos ; i++ ){
		canvas_index++;
		(*c1)[canvas_index]->cd();
		(*c1)[canvas_index]->SetLogx();
		TProfile *swap = PtProfile->GetHisto(i);
		(*c1)[canvas_index]->SetName(swap->GetName());
		swap->SetLineColor(color);
		swap->Draw(option);
	}

	for(int i =0 ; i < nanglehistos ; i++ ){
		canvas_index++;
		(*c1)[canvas_index]->cd();
		TProfile *swap = AngleProfile->GetHisto(i);
		(*c1)[canvas_index]->SetName(swap->GetName());
		swap->SetLineColor(color);
		swap->Draw(option);
	}
	
	for(int i =0 ; i < netahistos ; i++ ){
		canvas_index++;
		(*c1)[canvas_index]->cd();
		TProfile *swap = EtaProfile->GetHisto(i);
		(*c1)[canvas_index]->SetName(swap->GetName());
		swap->SetLineColor(color);
		swap->Draw(option);
	}
	

	for(int i =0 ; i < nanglehistograms ; i++ ){
		canvas_index++;
		(*c1)[canvas_index]->cd();
		TH1D *swap = AngleHistogram->GetHisto(i);
		(*c1)[canvas_index]->SetName(swap->GetName());
		swap->SetLineColor(color);
		swap->Draw(option);
	}

	for(int i =0 ; i < netahistograms ; i++ ){
		canvas_index++;
		(*c1)[canvas_index]->cd();
		TH1D *swap = EtaHistogram->GetHisto(i);
		(*c1)[canvas_index]->SetName(swap->GetName());
		swap->SetLineColor(color);
		swap->Draw(option);
	}


	for(int i =0 ; i < npthistograms ; i++ ){
		canvas_index++;
		(*c1)[canvas_index]->cd();
		TH1D *swap = PtHistogram->GetHisto(i);
		(*c1)[canvas_index]->SetName(swap->GetName());
		swap->SetLineColor(color);
		swap->Draw(option);
	}
	
	return nhistos;
}

}
#endif // #ifdef CosmicTrackValidation_C
