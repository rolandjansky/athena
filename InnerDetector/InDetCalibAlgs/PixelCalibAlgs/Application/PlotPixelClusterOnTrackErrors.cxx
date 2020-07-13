/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <fstream>
#include <vector>

#include <TApplication.h>

#include "CxxUtils/checker_macros.h"
#include "PixelCalibAlgs/PixelClusterOnTrackErrorPlot.h"
#include "PixelConditionsData/PixelClusterOnTrackErrorData.h"

using namespace PixelCalib;

int main ATLAS_NOT_THREAD_SAFE (int argc, char *argv[]){ // Thread usnafe PixelClusterOnTrackErrorPlot class is used.

	TApplication myapp("application", 0 , 0);

	if(argc < 1){
		std::cout << "Usage: " << argv[0] <<
			" FileContainingConstants OtherFileContainingConstants" << std::endl;
		return -1;
	}

	std::vector<PixelClusterOnTrackErrorData *> vec(0);

	for(int i = 1; i < argc; i++){
		if(std::ifstream(argv[i]).fail()){
			std::cout << "Failing to open " << argv[i] << std::endl;
			return -1;
		}
		PixelClusterOnTrackErrorData *parametersModel =
						new PixelClusterOnTrackErrorData();
		parametersModel->Load(argv[i]);
		vec.push_back(parametersModel);
	}

	PixelClusterOnTrackErrorPlot *plots = new PixelClusterOnTrackErrorPlot(*(vec[0]),false);
	for(unsigned int i = 1; i < vec.size(); i++){
		int color = 3+i;
		if(color == 5) color = 1;
		plots->AddReference(*(vec[i]),std::string(argv[i+1]),color,std::string("P"));
	}
	plots->Plot(argv[1]);

	myapp.Run();


	delete plots;
	plots = 0;
	for(unsigned int i = 0; i < vec.size(); i++) delete vec[i];

	return 0;
}
