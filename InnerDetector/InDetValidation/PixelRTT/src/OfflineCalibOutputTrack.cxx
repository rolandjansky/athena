/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OfflineCalibOutputTrack_cxx
#define OfflineCalibOutputTrack_cxx
#ifndef PixelNtupleTracks_cxx
#define PixelNtupleTracks_cxx

#include "PixelRTT/OfflineCalibOutputTrack.h"
#include "PathResolver/PathResolver.h"
#include "PixelCalibAlgs/PixelChargeInterpolationCalibration.h"
#include "PixelCalibAlgs/PixelChargeInterpolationValidation.h"
#include "PixelConditionsData/PixelChargeInterpolationParameters.h"
#include "PixelRTT/ResPullValidation.h"
#include <TFile.h>
#include <iostream>
#include <string>
#include <utility>


using namespace std;


namespace PixelValid{

OfflineCalibOutputTrack::OfflineCalibOutputTrack(std::string input,
				const std::string& output,
				std::string collection):
	PixelNtupleTracks(std::move(input),std::move(collection)){

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
	parametersModel = nullptr;
	
	m_ResPhiValid = new ResPullValidation("phi","res",datatype);
	m_PullPhiValid = new ResPullValidation("phi","pull",datatype);
	m_ResEtaValid = new ResPullValidation("eta","res",datatype);
	m_PullEtaValid = new ResPullValidation("eta","pull",datatype);

	std::cout << "OfflineCalibOutputTrack()" << std::endl;	

}

//////////////////////////////////////////////////////////////////////////////////////////

OfflineCalibOutputTrack::~OfflineCalibOutputTrack(){
	
	delete m_Calibration;
	delete m_Validation;
	m_Calibration = nullptr;
	m_Validation  = nullptr;
	
	delete m_ResPhiValid;
	delete m_PullPhiValid;
	delete m_ResEtaValid;
	delete m_PullEtaValid;
	m_ResPhiValid = nullptr;
	m_PullPhiValid = nullptr;
	m_ResEtaValid = nullptr;
	m_PullEtaValid = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////

void OfflineCalibOutputTrack::ReadHistoFile(const std::string& input){
	
	std::cout << "ReadHistoFile()" << std::endl;
	TDirectory *olddir = gDirectory;

	TFile *inputhisto = TFile::Open(input.c_str(),"READ");
	TDirectory *mydir = inputhisto->GetDirectory("ChargeSharingConstants");
	m_Calibration->Read(mydir);
	m_Validation->Read(mydir);

	inputhisto->cd();

	m_ResPhiValid->Read();
	m_PullPhiValid->Read();
	m_ResEtaValid->Read();
	m_PullEtaValid->Read();

	olddir->cd();
	inputhisto->Close();	
	delete inputhisto;
	std::cout << "ReadHistoFile()" << std::endl;
}

//////////////////////////////////////////////////////////////////////////////////////////

void OfflineCalibOutputTrack::Iterate(long maxentries){

	std::cout << "Iterate()" << std::endl;
   if (fChain == nullptr) return;
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


		m_ResPhiValid->Fill(DetType,GeVTrkPt,alpha,DeltaRow,ResPhi);
		m_PullPhiValid->Fill(DetType,GeVTrkPt,alpha,DeltaRow,PullPhi);
		m_ResEtaValid->Fill(DetType,GeVTrkPt,TrkEta,DeltaCol,ResEta);
		m_PullEtaValid->Fill(DetType,GeVTrkPt,TrkEta,DeltaCol,PullEta);

	  }// end of for   PixelHits
   }// end of for   entries
	std::cout << "Iterate()" << std::endl;
}

//////////////////////////////////////////////////////////////////////////////////////////

void OfflineCalibOutputTrack::Analyse(const std::string& output, const std::string& reference){

	std::cout << "Analyse()" << std::endl;
	std::string logfilename = output  + ".log";	

	std::vector<std::string> references;
	references.emplace_back("ChargeSharingConstants-Model");
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
	
	TFile *ref_file = nullptr;
	if(reference != ""){
		ref_file = TFile::Open(reference.c_str(),"READ");
	}
	m_ResPhiValid->Analyze(ref_file);
	m_PullPhiValid->Analyze(ref_file);
	m_ResEtaValid->Analyze(ref_file);
	m_PullEtaValid->Analyze(ref_file);
	m_Validation->Analyze(ref_file);
	if(ref_file != nullptr && ref_file->IsOpen()) ref_file->Close();
	delete ref_file;

	myhtmlfile.close();
	
	std::string histofilename = output  + ".root";
	TFile *histofile = TFile::Open(histofilename.c_str(),"RECREATE");
	if(histofile !=nullptr ){
		m_Calibration->Write();
		m_Validation->Write();
		TDirectory *old = gDirectory;

		m_ResPhiValid->Write();
		m_PullPhiValid->Write();
		m_ResEtaValid->Write();
		m_PullEtaValid->Write();
		old->cd();
		histofile->Close();
	}
	delete histofile;	
	
	std::cout << "Analyse() " << std::endl;

}


}

#endif
#endif
