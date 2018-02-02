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

	static MMDetectorDescription* GetCurrent() {return s_current;}
	
	double sWidth() const {return small_x();}
	double lWidth() const {return large_x();}
	double Length() const {return y();}
	double Tck()    const {return z();}

	void xFrame(double y) {m_xFrame=y;}
	double xFrame() const {return m_xFrame;}

	void ysFrame(double y) {m_ysFrame=y;}
	double ysFrame() const {return m_ysFrame;}

	void ylFrame(double y) {m_ylFrame=y;}
	double ylFrame() const {return m_ylFrame;}

        MuonGM::MM_Technology* GetTechnology();

	MMReadoutParameters roParameters;
	
	MMReadoutParameters& GetReadoutParameters() {return roParameters;}

protected:
	double m_xFrame;
	double m_ysFrame;
	double m_ylFrame;

	void SetDetectorAddress(AGDDDetectorPositioner*);
	static MMDetectorDescription* s_current;
};

#endif
