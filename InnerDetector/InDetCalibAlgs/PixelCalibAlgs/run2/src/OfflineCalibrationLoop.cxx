/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OfflineCalibrationLoop_cxx
#define OfflineCalibrationLoop_cxx
#ifndef PixelNtupleTracks_cxx
#define PixelNtupleTracks_cxx

#include <iostream>
#include <fstream>
#include <string>

#include <TStopwatch.h>
#include <TFile.h>

#include "PixelCalibAlgs/OfflineCalibrationLoop.h"
#include "PixelCalibAlgs/PixelChargeInterpolationCalibration.h"
#include "PixelCalibAlgs/PixelChargeInterpolationValidation.h"
#include "PixelConditionsData/PixelChargeInterpolationParameters.h"


namespace PixelCalib{

OfflineCalibrationLoop::OfflineCalibrationLoop(const std::string& starting_tag,
                                               const std::string& input,
                                               const std::string& final_tag,
                                               const std::string& collection):
			PixelNtupleTracks(input,collection),
			m_timer(0), m_Calibration(0), m_Validation(0),
			m_StartingTag(starting_tag), m_FinalTag(final_tag){

	m_timer = new TStopwatch;
	m_timer->Start();

	std::string constantsfile = "PixelChargeInterpolationData-" + starting_tag;
	size_t pos = starting_tag.find("PixelOfflineReco");
	if(pos != std::string::npos){
		constantsfile = "PixelChargeInterpolationData"
			+ starting_tag.substr(pos+16,std::string::npos);
	}

	if(std::ifstream(constantsfile.c_str()).fail()){
		std::cout << "Failing to open " << constantsfile <<
			" for initialization..." << std::endl;
		abort();
	}

	PixelChargeInterpolationParameters *parametersModel =
						new PixelChargeInterpolationParameters();
	parametersModel->Load(constantsfile);
	
	m_Calibration =
		new PixelChargeInterpolationCalibration(m_FinalTag, *parametersModel);
	m_Validation =
		new PixelChargeInterpolationValidation(m_FinalTag, *parametersModel);

	delete parametersModel;
	parametersModel = 0;

	m_timer->Stop();
	std::cout << "========== Time to allocate classes =========" << std::endl;
	std::cout << "Real time: " << m_timer->RealTime() << std::endl;
	std::cout << "CPU time: " <<  m_timer->CpuTime() << std::endl;
	std::cout << "=============================================" << std::endl;

}

//////////////////////////////////////////////////////////////////////////////////////////

OfflineCalibrationLoop::~OfflineCalibrationLoop(){
	
	delete m_timer;
	delete m_Calibration;
	delete m_Validation;
	
	m_timer = 0;
	m_Calibration = 0;
	m_Validation  = 0;

}


//////////////////////////////////////////////////////////////////////////////////////////

void OfflineCalibrationLoop::ReadHistoFile(const std::string& input){

	m_timer->Start();

	TFile *inputhisto = TFile::Open(input.c_str(),"READ");
	TDirectory *mydir = inputhisto->GetDirectory(m_FinalTag.c_str());
	m_Calibration->Read(mydir);
	m_Validation->Read(mydir);
	inputhisto->Close();	
	delete inputhisto;
	inputhisto = 0;

	m_timer->Stop();
	std::cout << "=========== Time to read from file ==========" << std::endl;
	std::cout << "Real time: " << m_timer->RealTime() << std::endl;
	std::cout << "CPU time: " <<  m_timer->CpuTime() << std::endl;
	std::cout << "=============================================" << std::endl;
	

}

//////////////////////////////////////////////////////////////////////////////////////////


void OfflineCalibrationLoop::Iterate(long maxentries){

	m_timer->Start();
	if (fChain == 0) return;
	long nentries = fChain->GetEntriesFast();
	if(maxentries > 0) nentries = ( nentries < maxentries ) ? nentries : maxentries;
	long jentry = 0, pixentries = 0, clusters = 0, all_clusters =0, my_clusters = 0;
	long my_clusters2r = 0;
	long my_clusters3r = 0;
	long my_clusters3c = 0;
	long my_clusters2c = 0;

	int output_entry = int(nentries/10.);
	if(output_entry > 1E5) output_entry = int(1E5);
	for (jentry=0; jentry<nentries;jentry++) {
		if ( LoadTree(jentry) < 0) break;
		if( !(jentry%output_entry) ){
		       	std::cout << "Track n. " << jentry << std::endl;
		       	std::cout << "Cluster n. " << my_clusters << std::endl;
		}
		//if(my_clusters > 1000000) break;
		fChain->GetEntry(jentry);
		
		//int nSCTBarrelHits = 0;
		//int nPixelBarrelHits= 0;
		//int nTRTBarrelHits = 0;
		//for (int iHits=0; iHits < nHits; iHits++){
		//	if (fabs((*HitSurfaceZ)[iHits])< 800  && (*DetectorType)[iHits] == 3)
		//		nTRTBarrelHits++;
		//	if (fabs((*HitSurfaceZ)[iHits])< 800 && (*DetectorType)[iHits] == 2)
		//		nSCTBarrelHits++;
		//	if (fabs((*HitSurfaceZ)[iHits])< 400 && (*DetectorType)[iHits] == 1)
		//		nPixelBarrelHits++;
		//}

		 // track quality cuts
		/// clusters before cuts
		all_clusters += nPixelHits;
		//if(Chi2overNdof > 5) continue; 
		if(nPixelHits < 1) continue;
		//if( GetGeVGlobalpT() < 5 ) continue;
		if( GetGeVGlobalpT() < 10 ) continue;
		if( (5*nSCTHits + nTRTHits ) < 30) continue;
		// tracks after general cuts
		pixentries++;
		// clusters after general cuts
		clusters += nPixelHits;
		for(int i = 0; i < nPixelHits; i++){
			
			int DetType = GetDetType(i);
			//double LorAlpha = GetLorAlpha(i);
			double alpha = GetRealAlpha(i);
			double TrkEta = GetTrkEta(i);
			double GeVTrkPt = GetGeVpT(i);
			double digreseta = GetMicronDigResEta(i);
			double digresphi = GetMicronDigResPhi(i);
			double reseta = GetMicronResEta(i);
			double resphi = GetMicronResPhi(i);
			double ERReta = GetMicronErrEta(i);
			double ERRphi = GetMicronErrPhi(i);
			double DeltaRow = GetDeltaRow(i);
			double DeltaCol = GetDeltaCol(i);
			double OmegaEta = GetOmegaEta(i);
			double OmegaPhi = GetOmegaPhi(i);
			double pulleta = GetPullEta(i);
			double pullphi = GetPullPhi(i);
			double charge = GetCharge(i);

			
			//if( DetType > 2 ) continue;
			//if( LorAlpha == 0 ) continue;
			//if( fabs(GeVTrkPt) < 5 ) continue;

			//if( fabs(alpha) > 20) continue;			
			//std::cout << "Calib Loop before cuts: "<< DetType << std::endl;

			double ResCutPhi = 80;
			double ResCutEta = 400;
			if(GeVTrkPt < 10E10){
				ResCutPhi = sqrt( (1000/GeVTrkPt)*(1000/GeVTrkPt)
							+ ResCutPhi*ResCutPhi);
				ResCutEta = sqrt( (1000/GeVTrkPt)*(1000/GeVTrkPt)
							+ ResCutEta*ResCutEta);
			}else{
				ResCutPhi*=3;
				ResCutEta*=3;
			
			}

			if( fabs(digresphi) > ResCutPhi) continue;
			if( fabs(digreseta) > ResCutEta) continue;
			// clusters after private cuts
			my_clusters++;
			if(DeltaRow == 2)  my_clusters2r++;
			if(DeltaRow == 3)  my_clusters3r++;
			if(DeltaCol == 3)  my_clusters3c++;
			if(DeltaCol == 2)  my_clusters2c++;

			//std::cout << "Calib Loop after cuts: "<< DetType << std::endl;
			m_Calibration->Fill(DetType,GeVTrkPt,
					alpha, DeltaRow, digresphi, resphi, OmegaPhi,
					TrkEta, DeltaCol, digreseta, reseta, OmegaEta);
			m_Validation->Fill(DetType,GeVTrkPt,
					alpha, DeltaRow, digresphi, resphi, ERRphi, pullphi,
					TrkEta, DeltaCol, digreseta, reseta, ERReta, pulleta, charge);

		}// end of for   PixelHits
	}// end of for   entries

	m_timer->Stop();
	std::cout << "========== Time to fill histograms ==========" << std::endl;
	std::cout << "Real time: " << m_timer->RealTime() << std::endl;
	std::cout << "CPU time: " <<  m_timer->CpuTime() << std::endl;
	std::cout << jentry << " tracks collected." << std::endl;
	std::cout << pixentries << " tracks after general cuts." << std::endl;
	std::cout << all_clusters << " pixel clusters in total." << std::endl;	
	std::cout << clusters << " pixel clusters after general cuts." << std::endl;	
	std::cout << my_clusters << " pixel clusters after private cuts." << std::endl;	
	std::cout << my_clusters2r << " 2 row pixel clusters." << std::endl;
	std::cout << my_clusters3r << " 3 row pixel clusters." << std::endl;
	std::cout << my_clusters2c << " 2 col pixel clusters." << std::endl;
	std::cout << my_clusters3c << " 3 col pixel clusters." << std::endl;
	std::cout << "=============================================" << std::endl;
}

//////////////////////////////////////////////////////////////////////////////////////////

void OfflineCalibrationLoop::Analyze(const std::string& reference,
                                     const std::string& reference_tag){

	m_timer->Start();

	//int nfits = 0; 
	//std::string output = "ChargeInterpolation" + m_FinalTag;
	std::vector<std::string> references;
	std::string constantsfile = "PixelChargeInterpolationData-" + m_StartingTag;
	size_t pos = m_StartingTag.find("PixelOfflineReco");
	if(pos != std::string::npos){
		constantsfile = "PixelChargeInterpolationData"
			+ m_StartingTag.substr(pos+16,std::string::npos);
	}
	references.push_back(constantsfile);
	m_Calibration->Analyze(m_FinalTag,references);

	TFile *ref_file = 0;
	TDirectory *mydir = 0;
	if(reference != ""){
		ref_file = TFile::Open(reference.c_str(),"READ");
		mydir = ref_file->GetDirectory(reference_tag.c_str());
	}
	m_Validation->Analyze(mydir);
	if(ref_file != 0 && ref_file->IsOpen()) ref_file->Close();
	delete ref_file;
	ref_file = 0;
	mydir = 0;

	std::string histofilename = m_FinalTag + std::string(".root");
	TFile *histofile = TFile::Open(histofilename.c_str(),"RECREATE");
	if(histofile !=0 ){
		m_Calibration->Write();
		m_Validation->Write();
		histofile->Close();
	}
	delete histofile;	
	histofile = 0;
	
	m_timer->Stop();
	std::cout << "=========== Time to analyze data ===========" << std::endl;
	std::cout << "Real time: " << m_timer->RealTime() << std::endl;
	std::cout << "CPU time: " <<  m_timer->CpuTime() << std::endl;
	//std::cout << nfits << " fits performed." << std::endl;
	std::cout << "=============================================" << std::endl;

}

}
#endif
#endif
