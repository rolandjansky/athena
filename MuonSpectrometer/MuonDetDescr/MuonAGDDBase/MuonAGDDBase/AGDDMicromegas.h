/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDMicromegas_H
#define AGDDMicromegas_H

#include "AGDDKernel/AGDDDetector.h"
#include <string>
#include <vector>
#include <iostream>

class GeoMaterial;

class AGDDMicromegas: public AGDDDetector {
public:
	AGDDMicromegas(std::string s);
	void Register();
	void CreateVolume();
	void CreateSolid();
	static AGDDMicromegas* GetCurrent() {return current;}
private:
	void SetDetectorAddress(AGDDDetectorPositioner*);
	static AGDDMicromegas* current;
};

#endif
