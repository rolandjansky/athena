/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDsTGC_H
#define AGDDsTGC_H

#include "MuonAGDDDescription/sTGCDetectorDescription.h"
#include "AGDDKernel/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>



class AGDDsTGC: public sTGCDetectorDescription, public AGDDVolume {
public:
    AGDDsTGC(std::string s);

	void CreateVolume();
	void CreateSolid();

private:

};

#endif
