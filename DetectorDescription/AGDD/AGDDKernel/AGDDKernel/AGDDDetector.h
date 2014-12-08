/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDDetector_H
#define AGDDDetector_H

#include "AGDDKernel/AGDDVolume.h"
#include "AGDDKernel/AGDDDetectorPositioner.h"
#include <string>
#include <vector>
#include <iostream>

class GeoMaterial;

struct DetectorPositionParameters {
	DetectorPositionParameters():Zposition(0),Radius(0),PhiStart(0) {;}
	double Zposition;
	double Radius;
	double PhiStart;
};

class AGDDDetector: public AGDDVolume {
public:
	AGDDDetector(std::string s):AGDDVolume(s),_small_x(0),_large_x(0),_y(0),_z(0) {}
	AGDDDetector(std::string s,std::string t):AGDDVolume(s),_small_x(0),_large_x(0),_y(0),_z(0),detectorType(t) {}
	virtual void SetXYZ(std::vector<double> v) 
	{
		_small_x=v[0];
		_large_x=v[1];
		_y=v[2];
		_z=v[3];
	}
	
	void small_x(double x) {_small_x=x;}
	void large_x(double x) {_large_x=x;}
	void y(double yval) {_y=yval;}
	void z(double zval) {_z=zval;}
    void subType(std::string s) {sType=s;}
	double small_x() {return _small_x;}
	double large_x() {return _large_x;}
	double y() {return _y;}
	double z() {return _z;}
	std::string subType() {return sType;}
	std::string tech;
	virtual void CreateVolume() =0;
	virtual void CreateSolid() =0;
	
	std::string DetectorType() {return detectorType;}
	std::string DetectorID() {return detectorID;}
	
	void SetAddressAndPosition(AGDDDetectorPositioner *p) {
		AddDetectorPosition(p);
		SetDetectorAddress(p);
	}
	DetectorPositionParameters position;
	
	std::vector<AGDDDetectorPositioner*>& GetDetectorPositioners() {return detectorPositions;}
	
protected:
	double _small_x;
	double _large_x;
	double _y;
	double _z;
	
	// detector Type to avoid dynamic casting
	std::string detectorType;
	std::string detectorID;

	std::string sType;
	
	GeoMaterial* GetMMMaterial(std::string);
	
	void AddDetectorPosition(AGDDDetectorPositioner* p) {detectorPositions.push_back(p);}

	virtual void SetDetectorAddress(AGDDDetectorPositioner*) =0;
	std::vector<AGDDDetectorPositioner*> detectorPositions;
};

#endif
