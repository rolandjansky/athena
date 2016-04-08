/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDMicromegas_H
#define AGDDMicromegas_H

#include "MuonAGDDDescription/MMDetectorDescription.h"
#include "AGDDKernel/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>



class AGDDMicromegas: public MMDetectorDescription, public AGDDVolume {
public:
    AGDDMicromegas(std::string s);

	void CreateVolume();
	void CreateSolid();

private:

};

#endif
