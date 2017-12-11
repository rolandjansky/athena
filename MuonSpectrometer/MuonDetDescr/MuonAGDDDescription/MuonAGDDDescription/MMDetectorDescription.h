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
    double driftThickness;
    std::vector<double> stereoAngel;
    std::vector<int> readoutSide;
    double zpos;
    double distanceFromZAxis;
    double roLength;
};

class MMDetectorDescription: public AGDDDetector {
public:
	MMDetectorDescription(std::string s);
	void Register();

	static MMDetectorDescription* GetCurrent() {return current;}
	
	double sWidth() const {return small_x();}
	double lWidth() const {return large_x();}
	double Length() const {return y();}
	double Tck()    const {return z();}

	void xFrame(double y) {_xFrame=y;}
	double xFrame() const {return _xFrame;}

	void ysFrame(double y) {_ysFrame=y;}
	double ysFrame() const {return _ysFrame;}

	void ylFrame(double y) {_ylFrame=y;}
	double ylFrame() const {return _ylFrame;}
	
	MM_Technology* GetTechnology();

	MMReadoutParameters roParameters;
	
	MMReadoutParameters& GetReadoutParameters() {return roParameters;}

protected:
	double _xFrame;
	double _ysFrame;
	double _ylFrame;

	void SetDetectorAddress(AGDDDetectorPositioner*);
	static MMDetectorDescription* current;
};

#endif
