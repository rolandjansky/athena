/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDMicromegas_H
#define AGDDMicromegas_H

#include "AGDDKernel/AGDDDetector.h"
#include "MuonGeoModel/MM_Technology.h"
#include <string>
#include <vector>
#include <iostream>

class GeoMaterial;

using MuonGM::MM_Technology;

class AGDDMicromegas: public AGDDDetector {
public:
	AGDDMicromegas(std::string s);
	void Register();
	void CreateVolume();
	void CreateSolid();
	static AGDDMicromegas* GetCurrent() {return current;}
	
	double sWidth() {return _small_x;}
	double lWidth() {return _large_x;}
	double Length() {return _y;}
	double Tck()    {return _z;}
	
	MM_Technology* GetTechnology();
private:
	void SetDetectorAddress(AGDDDetectorPositioner*);
	static AGDDMicromegas* current;
};

#endif
