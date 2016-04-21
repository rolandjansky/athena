/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMDetectorDescription_H
#define MMDetectorDescription_H

#include "AGDDKernel/AGDDDetector.h"
#include "MuonAGDDDescription/MM_Technology.h"
#include <string>
#include <vector>
#include <iostream>

using MuonGM::MM_Technology;

struct MMReadoutParameters {
    double stripPitch;
    double gasThickness;
    double pcbThickness;
    std::vector<double> stereoAngel;
    std::vector<int> readoutSide;
};

class MMDetectorDescription: public AGDDDetector {
public:
	MMDetectorDescription(std::string s);
	void Register();

	static MMDetectorDescription* GetCurrent() {return current;}
	
	double sWidth() {return _small_x;}
	double lWidth() {return _large_x;}
	double Length() {return _y;}
	double Tck()    {return _z;}
	
	MM_Technology* GetTechnology();

	MMReadoutParameters roParameters;
	
	MMReadoutParameters& GetReadoutParameters() {return roParameters;}

protected:
	void SetDetectorAddress(AGDDDetectorPositioner*);
	static MMDetectorDescription* current;
};

#endif
