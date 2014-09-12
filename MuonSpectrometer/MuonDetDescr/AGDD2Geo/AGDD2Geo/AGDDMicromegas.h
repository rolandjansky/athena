/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDMicromegas_H
#define AGDDMicromegas_H

#include "AGDD2Geo/AGDDDetector.h"
#include <string>
#include <vector>
#include <iostream>

class GeoMaterial;

class AGDDMicromegas: public AGDDDetector {
public:
	AGDDMicromegas(std::string s):AGDDDetector(s,"Micromegas") {}
	void CreateVolume();
	void CreateSolid();
private:
	void SetDetectorAddress(AGDDDetectorPositioner*);
};

#endif
