/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <string>

#include "CxxUtils/checker_macros.h"
#include "PixelCalibAlgs/OfflineCalibrationLoop.h"

using namespace PixelCalib;

/// the function that does the job!
int doAll ATLAS_NOT_THREAD_SAFE (std::string input_file, std::string input_tag, // Thread unsafe OfflineCalibrationLoop class is used.
	  std::string output_tag,
		std::string collection,
	  std::string reference_file, std::string reference_tag,
	  int maxentries){

	OfflineCalibrationLoop *s = 0;

	std::cout <<  "Allocating classes for calibration" << std::endl;
	if(maxentries < 0){
		s = new OfflineCalibrationLoop(input_tag,"",output_tag, collection);
		s->ReadHistoFile(input_file);
	}else{
		s = new OfflineCalibrationLoop(input_tag,input_file,output_tag, collection);
		std::cout <<  "Looping over data for calibration" << std::endl;
		s->Iterate(maxentries);
	}

	std::cout <<  "Starting analysis for calibration" << std::endl;
	s->Analyze(reference_file,reference_tag);
	
	std::cout << "End processing!" << std::endl;
	delete s; s = 0;
	return 0;
}

/// main does include the input interpretation
int main ATLAS_NOT_THREAD_SAFE (int argc, char *argv[]){ // Thread unsafe doAll function is used.

std::cout << "ciao" << std::endl;

	if(argc < 5 || argc > 6){
		std::cout << std::endl;
		std::cout << "	-> starting tag should be the FIRST argument"<< std::endl;
		std::cout << "	-> final tag should be the SECOND argument"<< std::endl;
		std::cout << std::endl;
		std::cout << "	-> track collection should be the THIRD argument" << std::endl;
		std::cout << "---> if you want to fill histograms:" << std::endl;
		std::cout << "	-> FOURTH ARGUMENT could be both an ntuple";
		std::cout << " or a file that contains a list of ntuples" << std::endl;
		std::cout << "---> if you want to refit the costants:" << std::endl;
		std::cout << "	-> FOURTH ARGUMENT should be the name of the file";
		std::cout << " containing the histograms. The last argument should";
		std::cout << " be a negative number" << std::endl;
		std::cout << std::endl;
		std::cout << "---> if you want to compare with a reference file:" << std::endl;
		std::cout << "	-> FIFTH ARGUMENT must be the file containing";
		std::cout << " the reference histograms" << std::endl;
		std::cout << std::endl;
		std::cout << "	-> the number of tracks to be processed";
		std::cout << " CAN be used as LAST ARGUMENT. Give -1 for refit." << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;
		return -1;
	}

	std::string input_tag = std::string(argv[1]);
	std::string output_tag = std::string(argv[2]);
	std::string collection = std::string(argv[3]);
	std::string reference_tag = "";
	std::string input_file = std::string(argv[4]);
	std::string reference_file = "";
	long maxentries = 0;

	//if(argc == 7){
	//	maxentries = atol(argv[6]);
	//	reference_file = std::string(argv[5]);
	//}
	//this could never be reached because argc must be 5 or 6 by above condition... not sure what logic was here.

	if(argc == 6){
		maxentries = atol(argv[5]);
		if(maxentries == 0) reference_file = std::string(argv[5]);
	}

	if(maxentries < 0){
		std::string::size_type pos = input_file.find(".root");
		if( pos == std::string::npos){
			std::cout << "Bad input file: " << input_file << std::endl;
			return -1;
		}
	}

	//std::cout << argc << " " << input_tag << " " << output_tag << " " << reference_tag
	//	<< " " << input_file << " " << reference_file << " " << maxentries << std::endl;
	
	std::cout << input_file << " " << input_tag << " " << output_tag << " " << collection << " " << reference_file << " " << reference_tag << " " <<
	maxentries << std::endl;

	return doAll(input_file, input_tag, output_tag, collection,
			reference_file, reference_tag, maxentries);

}


