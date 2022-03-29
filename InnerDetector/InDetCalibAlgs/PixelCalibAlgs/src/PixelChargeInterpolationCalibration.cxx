/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelChargeInterpolationCalibration_cxx
#define PixelChargeInterpolationCalibration_cxx

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <TDirectory.h>

#include "PixelCalibAlgs/PixelChargeInterpolationCalibration.h"
#include "PixelCalibAlgs/PixelChargeInterpolationHistograms.h"
#include "PixelConditionsData/PixelChargeInterpolationParameters.h"
#include "PixelCalibAlgs/PixelChargeInterpolationPlot.h"

namespace PixelCalib{

PixelChargeInterpolationCalibration::PixelChargeInterpolationCalibration(const std::string& tag,
		PixelChargeInterpolationParameters &model):
	m_DigitalCalibration(0),
	m_AnalogCalibration(0),
	m_plots(0){

	m_DigitalCalibration = new PixelChargeInterpolationHistograms(tag,model);
	m_AnalogCalibration = new PixelChargeInterpolationHistograms("analog_" + tag,model);

}

//////////////////////////////////////////////////////////////////////////////////////////

PixelChargeInterpolationCalibration::~PixelChargeInterpolationCalibration(){

	delete m_DigitalCalibration; m_DigitalCalibration = 0;
	delete m_AnalogCalibration; m_AnalogCalibration = 0;
	delete m_plots; m_plots = 0;

}

//////////////////////////////////////////////////////////////////////////////////////////

void PixelChargeInterpolationCalibration::Read(TDirectory *histofile){

	TDirectory *current = gDirectory;
	TDirectory *globaldir = gDirectory;
	if(histofile != 0) globaldir = histofile;

	globaldir->cd("ChargeInterpolation");
	m_DigitalCalibration->Read();
	m_AnalogCalibration->Read();
	
	current->cd();
}

//////////////////////////////////////////////////////////////////////////////////////////

void PixelChargeInterpolationCalibration::Write(TDirectory *writedir){

	TDirectory *current = gDirectory;
	TDirectory *globaldir = gDirectory;
	if(writedir != 0) globaldir = writedir;

	globaldir->mkdir("ChargeInterpolation")->cd();
	m_DigitalCalibration->Write();
	m_AnalogCalibration->Write();
	m_plots->Write();
	
	current->cd();

}

//////////////////////////////////////////////////////////////////////////////////////////

void PixelChargeInterpolationCalibration::Fill(Int_t DetType, Double_t GeVTrkPt,
		Double_t alpha, Double_t DeltaRow, Double_t digresphi,
		Double_t resphi, Double_t OmegaPhi,
		Double_t TrkEta, Double_t DeltaCol, Double_t digreseta,
		Double_t reseta, Double_t OmegaEta){

	// hack to uniform constants for all layers and for all disks
	int modifier = 0;
    int totlayers = 3;
    if(DetType == 0) totlayers = 1;
    else if(DetType < 4) modifier = 1; 
    else modifier = 4; 
	// fill with the same values for the relevant layers 
	for(int iLayer = 0; iLayer < totlayers; iLayer++){
		DetType = iLayer + modifier;
		m_DigitalCalibration->Fill(DetType, GeVTrkPt,
					TrkEta, DeltaCol, digreseta, OmegaEta,
					alpha, DeltaRow, digresphi, OmegaPhi);
		m_AnalogCalibration->Fill(DetType, GeVTrkPt,
					TrkEta, DeltaCol, reseta, OmegaEta,
					alpha, DeltaRow, resphi, OmegaPhi);
	}
 	
}

//////////////////////////////////////////////////////////////////////////////////////////

int PixelChargeInterpolationCalibration::Analyze( const std::string& output,
					std::vector<std::string> &reference_names){

	PixelChargeInterpolationParameters *DigitalParameters = 0;
	PixelChargeInterpolationParameters *AnalogParameters = 0;
	
        std::ofstream logfile;
  	logfile.open((output + ".log").c_str());
  	logfile << "Log file for the pixel calibration fits." << std::endl;	

	DigitalParameters = m_DigitalCalibration->Analyze(logfile);
	AnalogParameters = m_AnalogCalibration->Analyze(logfile);

  	logfile.close();	

	std::string name = "PixelChargeInterpolationData-" + output;
	size_t pos = output.find("PixelOfflineReco");
	if(pos != std::string::npos){
		name = "PixelChargeInterpolationData"
			+ output.substr(pos+16,std::string::npos);
	}
	DigitalParameters->Print(name);
	name = "Analog-" + name;
	AnalogParameters->Print(name);

	m_plots = new PixelChargeInterpolationPlot(*DigitalParameters);
	m_plots->AddReference(*AnalogParameters,
			std::string("Fit on analog residual (should give 0)").c_str(),3,std::string("P"));
	//std::cout << reference_names.size() << std::endl;
	for(unsigned int i = 0 ; i < reference_names.size() ; i++ ){
		//std::cout << reference_names[i] << std::endl;
                std::ifstream fin((reference_names[i]).c_str());
		if(!fin.fail()){
			fin.close();
			PixelChargeInterpolationParameters *RefParameters =
				new PixelChargeInterpolationParameters();
			RefParameters->Load(reference_names[i]);
			int color = 4+i;
			if(color == 5) color = 1;
			m_plots->AddReference(*RefParameters,reference_names[i],color,std::string("P"));
			delete RefParameters;
		}
	}
	m_plots->Plot(output);

	
	return 0;
}


} // end of namespace PixelCalib

#endif
