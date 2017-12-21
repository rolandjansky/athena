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

	m_globaldirname = "TrackValidation";
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
	m_PtHistogram =  new MultiHisto<TH1D>(*ptHistomodel,NameDiv,NDiv,BinsDiv);
	
	
	TH1D *angleHistomodel = new TH1D("angleditribution","Angle distribution",50,-anglelim,anglelim);
	m_AngleHistogram =  new MultiHisto<TH1D>(*angleHistomodel,NameDiv,NDiv,BinsDiv);
	
	
	TH1D *etaHistomodel = new TH1D("etaditribution","Eta distribution",50,-etalim,etalim);
	m_EtaHistogram =  new MultiHisto<TH1D>(*etaHistomodel,NameDiv,NDiv,BinsDiv);
	
	
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
	m_PtProfile = new MultiHisto<TProfile>(*PtProfmodel,NameDiv,NDiv,BinsDiv);

	// Angle histograms allocation:
	TProfile *AngleProfmodel = new TProfile("nhitsVsAngle","nhits vs angle", 50, anglelim, -anglelim);
	m_AngleProfile = new MultiHisto<TProfile>(*AngleProfmodel,NameDiv,NDiv,BinsDiv);

	// Eta histograms allocation:
	TProfile *EtaProfmodel = new TProfile("nhitsVsEta","nhits vs eta", 50, etalim, -etalim);
	m_EtaProfile = new MultiHisto<TProfile>(*EtaProfmodel,NameDiv,NDiv,BinsDiv);
	//std::cout << AngleProfmodel->ClassName() << std::endl;
	//std::cout << m_AngleProfile->GetHisto(2)->ClassName() << std::endl;
	
	delete[] layerBins;
	delete[] clusterSizeBins;

}

//////////////////////////////////////////////////////////////////////////////////////////

int CosmicTrackValidation::Read(){


	TDirectory *globaldir = (TDirectory *)gDirectory->Get(m_globaldirname.c_str());

	int readedhistos =0;
	TDirectory *histodir = (TDirectory *)globaldir->Get(m_AngleProfile->GetName());
	readedhistos += m_AngleProfile->FillFromFile(histodir);
	histodir = (TDirectory *)globaldir->Get(m_EtaProfile->GetName());
	readedhistos += m_EtaProfile->FillFromFile(histodir);
	histodir = (TDirectory *)globaldir->Get(m_PtProfile->GetName());
	readedhistos += m_PtProfile->FillFromFile(histodir);
	histodir = (TDirectory *)globaldir->Get(m_AngleHistogram->GetName());
	readedhistos += m_AngleHistogram->FillFromFile(histodir);
	histodir = (TDirectory *)globaldir->Get(m_EtaHistogram->GetName());
	readedhistos += m_EtaHistogram->FillFromFile(histodir);
	histodir = (TDirectory *)globaldir->Get(m_PtHistogram->GetName());
	readedhistos += m_PtHistogram->FillFromFile(histodir);
	
	return readedhistos;
}

//////////////////////////////////////////////////////////////////////////////////////////

int CosmicTrackValidation::Write(){

	TDirectory *current = gDirectory;
	TDirectory *globaldir = current->mkdir(m_globaldirname.c_str());
	globaldir->cd();
	int writtenhistos = 0;
	m_AngleProfile->Write();
	writtenhistos +=  m_AngleProfile->GetNhistos();
	m_EtaProfile->Write();
	writtenhistos +=  m_EtaProfile->GetNhistos();
	m_PtProfile->Write();
	writtenhistos +=  m_PtProfile->GetNhistos();
	m_AngleHistogram->Write();
	writtenhistos +=  m_AngleHistogram->GetNhistos();
	m_EtaHistogram->Write();
	writtenhistos +=  m_EtaHistogram->GetNhistos();
	m_PtHistogram->Write();
	writtenhistos +=  m_PtHistogram->GetNhistos();
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
		m_AngleProfile->Fill(Angle,nhits,Pars);
		m_EtaProfile->Fill(Eta,nhits,Pars);
		//std::cout << GeVTrkPt << std::endl;
		m_PtProfile->Fill(GeVTrkPt,nhits,Pars);
		m_AngleHistogram->Fill(Angle,1,Pars);
		m_EtaHistogram->Fill(Eta,1,Pars);
		m_PtHistogram->Fill(GeVTrkPt,1,Pars);
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
	
	int npthistos =  m_PtProfile->GetNhistos();
	int nanglehistos =  m_AngleProfile->GetNhistos();
	int netahistos =  m_EtaProfile->GetNhistos();
	int nanglehistograms =  m_AngleHistogram->GetNhistos();
	int netahistograms =  m_EtaHistogram->GetNhistos();
	int npthistograms =  m_PtHistogram->GetNhistos();
	int nhistos =  npthistos + nanglehistos + netahistos + nanglehistograms + netahistograms + npthistograms;

	if(ncanvas == 0 || *c1 == 0){
		*c1 = new TCanvas*[nhistos];
		ncanvas = nhistos;
		for(int i = 0 ; i < ncanvas ; i++) (*c1)[i] = new TCanvas();
	}else if(ncanvas != nhistos){
		std::cout << "Number of canvases is not compatible with" <<
			m_globaldirname << "!" << std::endl;
		return 0;
	}

	int canvas_index = -1;
	for(int i =0 ; i < npthistos ; i++ ){
		canvas_index++;
		(*c1)[canvas_index]->cd();
		(*c1)[canvas_index]->SetLogx();
		TProfile *swap = m_PtProfile->GetHisto(i);
		(*c1)[canvas_index]->SetName(swap->GetName());
		swap->SetLineColor(color);
		swap->Draw(option);
	}

	for(int i =0 ; i < nanglehistos ; i++ ){
		canvas_index++;
		(*c1)[canvas_index]->cd();
		TProfile *swap = m_AngleProfile->GetHisto(i);
		(*c1)[canvas_index]->SetName(swap->GetName());
		swap->SetLineColor(color);
		swap->Draw(option);
	}
	
	for(int i =0 ; i < netahistos ; i++ ){
		canvas_index++;
		(*c1)[canvas_index]->cd();
		TProfile *swap = m_EtaProfile->GetHisto(i);
		(*c1)[canvas_index]->SetName(swap->GetName());
		swap->SetLineColor(color);
		swap->Draw(option);
	}
	

	for(int i =0 ; i < nanglehistograms ; i++ ){
		canvas_index++;
		(*c1)[canvas_index]->cd();
		TH1D *swap = m_AngleHistogram->GetHisto(i);
		(*c1)[canvas_index]->SetName(swap->GetName());
		swap->SetLineColor(color);
		swap->Draw(option);
	}

	for(int i =0 ; i < netahistograms ; i++ ){
		canvas_index++;
		(*c1)[canvas_index]->cd();
		TH1D *swap = m_EtaHistogram->GetHisto(i);
		(*c1)[canvas_index]->SetName(swap->GetName());
		swap->SetLineColor(color);
		swap->Draw(option);
	}


	for(int i =0 ; i < npthistograms ; i++ ){
		canvas_index++;
		(*c1)[canvas_index]->cd();
		TH1D *swap = m_PtHistogram->GetHisto(i);
		(*c1)[canvas_index]->SetName(swap->GetName());
		swap->SetLineColor(color);
		swap->Draw(option);
	}
	
	return nhistos;
}

}
#endif // #ifdef CosmicTrackValidation_C
