/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <string>

#include "CxxUtils/checker_macros.h"
#include "PixelRTT/OfflineCalibOutputTrack.h"

using namespace PixelValid;

int doAll(std::string input, std::string output, std::string reference, int maxentries);

int main ATLAS_NOT_THREAD_SAFE (int argc, char *argv[]){ // Thread unsafe doAll function is used.

	if(argc < 3 || argc > 5){
		std::cout << std::endl;
		std::cout << "Please enter input files on command line!" << std::endl;
		std::cout << std::endl;		
		//PixelCalib::PrintFillInstruction();
		std::cout << std::endl;		
		//PixelCalib::PrintCompareInstruction();
		std::cout << std::endl;		
		return -1;
	}

	std::string input = std::string(argv[1]);
	std::string output = std::string(argv[2]);
	std::string reference = "";
	long maxentries = 0;

	if(argc == 5){
		maxentries = atol(argv[4]);
		reference = std::string(argv[3]);
	}else if(argc == 4){
		maxentries = atol(argv[3]);
		if(maxentries == 0) reference = std::string(argv[3]);
	}
	
	return doAll(input,output,reference,maxentries);
}



/// the function that actually implements things!!
int doAll ATLAS_NOT_THREAD_SAFE (std::string input, std::string output, std::string reference, int maxentries){ // Thread unsafe OfflineCalibOutputTrack class is used.

	OfflineCalibOutputTrack *s = 0;

	std::cout <<  "Allocating classes for calibration" << std::endl;
	if(maxentries < 0){
		s = new OfflineCalibOutputTrack("",output);
		std::cout <<  "Reading data for calibration" << std::endl;
		s->ReadHistoFile(input);
	}else{
		s = new OfflineCalibOutputTrack(input,output);
		std::cout <<  "Looping over data for calibration" << std::endl;
		s->Iterate(maxentries);
	}

	std::cout <<  "Starting analysis for calibration" << std::endl;
	s->Analyse(output,reference);
	
	std::cout << "End processing!" << std::endl;
	delete s;
	return 0;
}
