/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OfflineCalibOutputTrack_cxx
#define OfflineCalibOutputTrack_cxx
#ifndef PixelNtupleTracks_cxx
#define PixelNtupleTracks_cxx

#include <iostream>
#include <string>
#include <TFile.h>
#include "PixelRTT/OfflineCalibOutputTrack.h"
#include "PixelCalibAlgs/PixelChargeInterpolationCalibration.h"
#include "PixelCalibAlgs/PixelChargeInterpolationValidation.h"
#include "PixelConditionsData/PixelChargeInterpolationParameters.h"
#include "PixelRTT/ResPullValidation.h"
#include "PathResolver/PathResolver.h"


namespace PixelValid{

OfflineCalibOutputTrack::OfflineCalibOutputTrack(std::string input,
				std::string output,
				std::string collection):
	PixelNtupleTracks(input,collection){

	std::cout << "OfflineCalibOutputTrack()" << std::endl;
	
	std::string datatype;
	if(output.find("cosmic") == std::string::npos) datatype = output;
	else datatype = "cosmic";

	std::string constantsfile = "ChargeSharingConstants-Model";
	if(ifstream(constantsfile.c_str()).fail()){
		std::cout << "Failing to open " << constantsfile <<
			" for initialization..." << std::endl;
		abort();
	}

	PixelCalib::PixelChargeInterpolationParameters *parametersModel =
						new PixelCalib::PixelChargeInterpolationParameters();
	parametersModel->Load(constantsfile);
	
	m_Calibration =
		new PixelCalib::PixelChargeInterpolationCalibration("ChargeSharingConstants", *parametersModel);
	m_Validation =
		new PixelCalib::PixelChargeInterpolationValidation("ChargeSharingConstants", *parametersModel);

	delete parametersModel;
	parametersModel = 0;
	
	ResPhiValid = new ResPullValidation("phi","res",datatype);
	PullPhiValid = new ResPullValidation("phi","pull",datatype);
	ResEtaValid = new ResPullValidation("eta","res",datatype);
	PullEtaValid = new ResPullValidation("eta","pull",datatype);

	std::cout << "OfflineCalibOutputTrack()" << std::endl;	

}

//////////////////////////////////////////////////////////////////////////////////////////

OfflineCalibOutputTrack::~OfflineCalibOutputTrack(){
	
	delete m_Calibration;
	delete m_Validation;
	m_Calibration = 0;
	m_Validation  = 0;
	
	delete ResPhiValid;
	delete PullPhiValid;
	delete ResEtaValid;
	delete PullEtaValid;
	ResPhiValid = 0;
	PullPhiValid = 0;
	ResEtaValid = 0;
	PullEtaValid = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////

void OfflineCalibOutputTrack::ReadHistoFile(std::string input){
	
	std::cout << "ReadHistoFile()" << std::endl;
	TDirectory *olddir = gDirectory;

	TFile *inputhisto = TFile::Open(input.c_str(),"READ");
	TDirectory *mydir = inputhisto->GetDirectory("ChargeSharingConstants");
	m_Calibration->Read(mydir);
	m_Validation->Read(mydir);

	inputhisto->cd();

	ResPhiValid->Read();
	PullPhiValid->Read();
	ResEtaValid->Read();
	PullEtaValid->Read();

	olddir->cd();
	inputhisto->Close();	
	delete inputhisto;
	std::cout << "ReadHistoFile()" << std::endl;
}

//////////////////////////////////////////////////////////////////////////////////////////

void OfflineCalibOutputTrack::Iterate(long maxentries){

	std::cout << "Iterate()" << std::endl;
   if (fChain == 0) return;
   Long64_t nentries = fChain->GetEntriesFast();
   if(maxentries > 0) nentries = ( nentries < maxentries ) ? nentries : maxentries;
   Long64_t jentry=0, pixentries = 0;
   for (jentry=0; jentry<nentries;jentry++) {
      if ( LoadTree(jentry) < 0) break;
      if( !(jentry%100000) )
	      std::cout << "Track n. " << jentry << std::endl;
      fChain->GetEntry(jentry);
     
      if(nPixelHits > 0) pixentries++;
      for(Int_t i = 0;i<nPixelHits;i++){

	      Int_t DetType = GetDetType(i);
		Double_t alpha = GetRealAlpha(i);
		//Double_t alphaM = GetAlphaMeas(i);
		Double_t TrkEta = GetTrkEta(i);
		Double_t GeVTrkPt = GetGeVpT(i);
		double digreseta = GetMicronDigResEta(i);
		double digresphi = GetMicronDigResPhi(i);
		double ResEta = GetMicronResEta(i);
		double ResPhi = GetMicronResPhi(i);
		double ERReta = GetMicronErrEta(i);
		double ERRphi = GetMicronErrPhi(i);
		Double_t PullPhi = GetPullPhi(i);
		Double_t PullEta = GetPullEta(i);
		Double_t DeltaRow = GetDeltaRow(i);
		Double_t DeltaCol = GetDeltaCol(i);
		Double_t OmegaEta = GetOmegaEta(i);
		Double_t OmegaPhi = GetOmegaPhi(i);
		//Int_t EtaIndex = GetEtaModule(i);
		//Int_t PhiIndex = GetPhiModule(i);

		m_Calibration->Fill(DetType,GeVTrkPt,
					alpha, DeltaRow, digresphi, ResPhi, OmegaPhi,
					TrkEta, DeltaCol, digreseta, ResEta, OmegaEta);
		// Obsolete code and just updated to fix compilation error
                // Use the OfflineCalibrationLoop class in PixelCalibAlgs insteas
		m_Validation->Fill(DetType,GeVTrkPt,
				   alpha, DeltaRow, digresphi, ResPhi, ERRphi, 0.,
				   TrkEta, DeltaCol, digreseta, ResEta, ERReta,0., -1.);


		ResPhiValid->Fill(DetType,GeVTrkPt,alpha,DeltaRow,ResPhi);
		PullPhiValid->Fill(DetType,GeVTrkPt,alpha,DeltaRow,PullPhi);
		ResEtaValid->Fill(DetType,GeVTrkPt,TrkEta,DeltaCol,ResEta);
		PullEtaValid->Fill(DetType,GeVTrkPt,TrkEta,DeltaCol,PullEta);

	  }// end of for   PixelHits
   }// end of for   entries
	std::cout << "Iterate()" << std::endl;
}

//////////////////////////////////////////////////////////////////////////////////////////

void OfflineCalibOutputTrack::Analyse(std::string output, std::string reference){

	std::cout << "Analyse()" << std::endl;
	std::string logfilename = output  + ".log";	

	std::vector<std::string> references;
	references.push_back("ChargeSharingConstants-Model");
	m_Calibration->Analyze("ChargeSharingConstants",references);


	std::string myhtmlfilename = output + ".html";
	ofstream myhtmlfile;
  	myhtmlfile.open(myhtmlfilename.c_str());
	
	std::string standard_htmlfilename = "PixelRTT_TracksPlots.html";
	std::string standard_htmlfile_fullpath =
		PathResolver::find_file(standard_htmlfilename, "DATAPATH");
	ifstream standard_htmlfile;
	standard_htmlfile.open(standard_htmlfile_fullpath.c_str());
	
	std::string swap;
	while( getline(standard_htmlfile,swap) ) myhtmlfile << swap << "\n";

	standard_htmlfile.close();
	
	TFile *ref_file = 0;
	if(reference != ""){
		ref_file = TFile::Open(reference.c_str(),"READ");
	}
	ResPhiValid->Analyze(ref_file);
	PullPhiValid->Analyze(ref_file);
	ResEtaValid->Analyze(ref_file);
	PullEtaValid->Analyze(ref_file);
	m_Validation->Analyze(ref_file);
	if(ref_file != 0 && ref_file->IsOpen()) ref_file->Close();
	delete ref_file;

	myhtmlfile.close();
	
	std::string histofilename = output  + ".root";
	TFile *histofile = TFile::Open(histofilename.c_str(),"RECREATE");
	if(histofile !=0 ){
		m_Calibration->Write();
		m_Validation->Write();
		TDirectory *old = gDirectory;

		ResPhiValid->Write();
		PullPhiValid->Write();
		ResEtaValid->Write();
		PullEtaValid->Write();
		old->cd();
		histofile->Close();
	}
	delete histofile;	
	
	std::cout << "Analyse() " << std::endl;

}


}

#endif
#endif
