/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
    std::vector<double> stereoAngle;
    std::vector<int> readoutSide;
    double zpos;
    double distanceFromZAxis;  //inner radius: distance from the IP till the bottom part of the module
    double roLength;   //module's radial size
    double activeBottomLength;  // active area: bottom length
    double activeTopLength;    //active area: top length
    double activeH;            //active area: radial size
     double minYPhiR;          //active area: the distance between the first eta and stereo strips (active) [R:right, L:left, min:bottom part, max:top part]. LM1 is the special case as there is no space to route any strips at the bottom, we use two distances (left and right).
    double minYPhiL;
    double maxYPhi;
    int nMissedTopEta;      //number of strips that are not connected at any FE board (eta layer)
    int nMissedBottomEta;
    int nMissedTopStereo;   //number of strips that are not connected at any FE board (stereo layer)
    int nMissedBottomStereo;
    int nRoutedTop;     // number of strips needed to cover the low efficient parts of the module (these strips are shorter in lenght than the normal ones)
    int nRoutedBottom;
    double dlStereoTop;
    double dlStereoBottom;
    int tStrips; //total strips per module (disconnected strips are included)
    double driftGap; 
};

class MMDetectorDescription: public AGDDDetector {
public:
	MMDetectorDescription(std::string s);
	void Register();

	static MMDetectorDescription* GetCurrent() {return current;}
	
	double sWidth() {return small_x();}
	double lWidth() {return large_x();}
	double Length() {return y();}
	double Tck()    {return z();}

	void xFrame(double y) {_xFrame=y;}
	double xFrame() {return _xFrame;}

	void ysFrame(double y) {_ysFrame=y;}
	double ysFrame() {return _ysFrame;}

	void ylFrame(double y) {_ylFrame=y;}
	double ylFrame() {return _ylFrame;}
	
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
