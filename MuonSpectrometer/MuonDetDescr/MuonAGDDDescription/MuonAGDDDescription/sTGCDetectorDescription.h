/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTGCDetectorDescription_H
#define sTGCDetectorDescription_H

#include "AGDDKernel/AGDDDetector.h"
#include "MuonAGDDDescription/sTGC_Technology.h"
#include <string>
#include <vector>
#include <iostream>

class GeoMaterial;

struct sTGCReadoutParameters {
    double sPadWidth;
    double lPadWidth;
    double sStripWidth;
    double lStripWidth;
    std::vector<double> padH;
    std::vector<int> nPadPhi;
    double anglePadPhi;
    std::vector<double> firstPadPhiDivision_A;
    std::vector<double> firstPadPhiDivision_C;
    std::vector<double> PadPhiShift_A;
    std::vector<double> PadPhiShift_C;
    std::vector<double> nPadH;
    std::vector<double> firstPadH;
    std::vector<int> firstPadRow;
    std::vector<int> nWires;
    std::vector<double> wireCutout;
    std::vector<double> firstWire;
    int wireGroupWidth;
    int nStrips;
    std::vector<int> firstTriggerBand;
    std::vector<int> nTriggerBands;
    std::vector<int> firstStripInTrigger;
    std::vector<double> firstStripWidth;
    std::vector<int> StripsInBandsLayer1;
    std::vector<int> StripsInBandsLayer2;
    std::vector<int> StripsInBandsLayer3;
    std::vector<int> StripsInBandsLayer4;
    std::vector<int> nWireGroups;
    std::vector<int> firstWireGroup;
};

class sTGCDetectorDescription: public AGDDDetector {
public:
    sTGCDetectorDescription(std::string s);
    void Register();
	
        virtual void SetXYZ(const std::vector<double>& v) override
	{
                small_x(v[0]);
		large_x(v[1]);
		y(v[2]);
		z(v[3]);
		m_yCutout=v[4];
		m_stripPitch=v[5];
		m_wirePitch=v[6];
	}
	
	double sWidth() const {return small_x();}
	double lWidth() const {return large_x();}
	double Length() const {return y();}
	double Tck()    const {return z();}

	void yCutout(double y) {m_yCutout=y;}
	double yCutout() const {return m_yCutout;}
	
	void stripPitch(double y) {m_stripPitch=y;}
	double stripPitch() const {return m_stripPitch;}

	void wirePitch(double y) {m_wirePitch=y;}
	double wirePitch() const {return m_wirePitch;}

	void xFrame(double y) {m_xFrame=y;}
	double xFrame() const {return m_xFrame;}
	
	void ysFrame(double y) {m_ysFrame=y;}
	double ysFrame() const {return m_ysFrame;}
	
	void ylFrame(double y) {m_ylFrame=y;}
	double ylFrame() const {return m_ylFrame;}

	sTGCReadoutParameters roParameters;
	
	sTGCReadoutParameters& GetReadoutParameters() {return roParameters;}

	static sTGCDetectorDescription* GetCurrent() {return s_current;}
	
	MuonGM::sTGC_Technology* GetTechnology();

protected:

	double m_yCutout;
	double m_stripPitch;
	double m_wirePitch;

	double m_xFrame;
	double m_ysFrame;
	double m_ylFrame;

	static sTGCDetectorDescription* s_current;
	virtual void SetDetectorAddress(AGDDDetectorPositioner*) override;
};

#endif
