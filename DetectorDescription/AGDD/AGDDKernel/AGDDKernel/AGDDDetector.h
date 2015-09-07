/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDDetector_H
#define AGDDDetector_H

#include "AGDDKernel/AGDDDetectorPositioner.h"
#include <string>
#include <vector>
#include <iostream>

class GeoMaterial;

class AGDDDetector {
public:
	AGDDDetector(std::string s):_small_x(0),_large_x(0),_y(0),_z(0),name(s) {}
	AGDDDetector(std::string s,std::string t):_small_x(0),_large_x(0),_y(0),_z(0),detectorType(t),name(s) {}
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
	
	std::string GetName() {return name;}
	
	std::string DetectorType() {return detectorType;}
	std::string DetectorID() {return detectorID;}
	
	void SetAddressAndPosition(AGDDDetectorPositioner*) ;
	
	std::vector<AGDDDetectorPositioner*>& GetDetectorPositioners();
	
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
	
	void AddDetectorPosition(AGDDDetectorPositioner* p);

	virtual void SetDetectorAddress(AGDDDetectorPositioner*) =0;
	std::vector<AGDDDetectorPositioner*> detectorPositions;
	
	std::string name;
};

#endif
